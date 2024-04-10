from udmabuf         import Udmabuf
from uio             import Uio
from traffic_checker import Traffic_Checker
import numpy as np
import math
import time
import argparse

class Write_Traffic_Check:
    
    def __init__(self):
        if    Uio.find_device_file('uio_traffic_checker_hp'):
           self.uio      = Uio('uio_traffic_checker_hp')
           self.coherent = False
        elif  Uio.find_device_file('uio_traffic_checker_hpc'):
           self.uio      = Uio('uio_traffic_checker_hpc')
           self.coherent = True
        elif  Uio.find_device_file('uio_traffic_checker_acp'):
           self.uio      = Uio('uio_traffic_checker_acp')
           self.coherent = True
        else:
            raise FileNotFoundError
        self.tc          = Traffic_Checker(self.uio.regs(), self.coherent)
        self.udmabuf     = Udmabuf('udmabuf_traffic_checker' )
        self.dtype       = np.uint32
        self.max_size    = (self.udmabuf.buf_size) // (np.dtype(self.dtype).itemsize)

    def memmap(self, size):
        if (size > self.max_size):
            raise ValueError('size({0}) is over size of buffer({1})' .format(size,self.max_size))
        return self.udmabuf.memmap(dtype=self.dtype , shape=(size))

    def allocate_array(self, size):
        self.array = self.memmap(size)
        self.set_sync_area()

    def set_sync_area(self):
        self.udmabuf.set_sync_to_cpu(0, self.array.size * (np.dtype(self.dtype).itemsize))

    def sync_for_device(self):
        self.udmabuf.sync_for_device()
        
    def sync_for_cpu(self):
        self.udmabuf.sync_for_cpu()
        
    def run(self):
        start = time.time()
        self.tc.mw_setup(self.udmabuf.phys_addr, self.array.size * (np.dtype(self.dtype).itemsize))
        self.uio.irq_on()
        self.tc.mw_start()
        self.uio.wait_irq()
        self.tc.mw_clear_status()
        self.run_time = time.time() - start
                      
if __name__ == '__main__':
    print_tag = "Write_Traffic_Check :"
    parser = argparse.ArgumentParser(description='Test Bench for Traffic_Checker')
    parser.add_argument('-W', '--words'  , help='Write size(Words) ', action='store', type=int, default=1024)
    parser.add_argument('-B', '--bytes'  , help='Write size(Bytes) ', action='store', type=int, default=0)
    parser.add_argument('-K', '--kbytes' , help='Write size(KBytes) ', action='store', type=int, default=0)
    parser.add_argument('-M', '--mbytes' , help='Write size(MBytes) ', action='store', type=int, default=0)
    parser.add_argument('-N', '--number' , help='number of loops '   , action='store', type=int, default=1)

    args   = parser.parse_args()
    test   = Write_Traffic_Check()
    test.tc.print_info(print_tag)

    bytes_per_word = np.dtype(test.dtype).itemsize

    if   args.mbytes > 0:
        words = math.ceil((args.mbytes*(1024*1024)) / bytes_per_word)
    elif args.kbytes > 0:
        words = math.ceil((args.kbytes*(1024     )) / bytes_per_word)
    elif args.bytes  > 0:
        words = math.ceil((args.bytes             ) / bytes_per_word)
    else:
        words = args.words

    test.allocate_array(words)
    test.array[:] = 0

    test_bytes  = words * bytes_per_word
    total_time  = 0
    total_bytes = 0
    count       = 0

    print ("{0} bytes_per_word         : {1}".format(print_tag, bytes_per_word))
    print ("{0} write_words            : {1}".format(print_tag, words))
    print ("{0} try_loops              : {1}".format(print_tag, args.number))

    for i in range (0,args.number):
        test.sync_for_device()
        test.run()
        test.sync_for_cpu()
        total_time   = total_time  + test.run_time
        total_bytes  = total_bytes + test_bytes
        count        = count       + 1
        print ("{0} time                   : {1:>8.3f} # [msec]".format(print_tag, round(test.run_time*1000.0,3)))

    average_time = round((total_time/count)*1000.0             ,3)
    throughput   = round(((total_bytes/total_time)/(1000*1000)),3)

    print ("{0} average_time           : {1:>8.3f} # [msec]"      .format(print_tag, average_time))
    print ("{0} throughput             : {1:>8.3f} # [mbytes/sec]".format(print_tag, throughput  ))

    print(test.array)
    
