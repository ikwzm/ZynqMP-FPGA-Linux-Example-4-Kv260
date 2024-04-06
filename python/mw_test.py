from udmabuf         import Udmabuf
from uio             import Uio
from traffic_checker import Traffic_Checker
import numpy as np
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

    test   = Write_Traffic_Check()
    test.tc.print_info("Write_Traffic_Check :")

    size = 1024
    test.allocate_array(size)
    test.array[:] = 0
    
    total_time = 0
    total_size = 0
    count      = 0
    test.sync_for_device()
    test.run()
    test.sync_for_cpu()
    print(test.array)
    
