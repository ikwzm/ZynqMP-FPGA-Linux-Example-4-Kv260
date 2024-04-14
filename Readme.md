ZynqMP-FPGA-Example-4 for Kv260
====================================================================================

Overvier
------------------------------------------------------------------------------------

### Requirement

* Board: any of the following
  - Kv260
* OS: any of the following
  - https://github.com/ikwzm/ZynqMP-FPGA-Debian12
  - https://github.com/ikwzm/ZynqMP-FPGA-Ubuntu22.04-Console
  - https://github.com/ikwzm/ZynqMP-FPGA-Ubuntu22.04-Desktop

### Licensing

Distributed under the BSD 2-Clause License.


Install 
------------------------------------------------------------------------------------

### Download ZynqMP-FPGA-Linux-Example-4-Kv260

```console
shell$ git clone https://github.com/ikwzm/ZynqMP-FPGA-Linux-Example-4-Kv260.git
shell$ cd ZynqMP-FPGA-Linux-Example-4-Kv260
```

### Install BitStream Files to /lib/firmware

```console
shell$ sudo sh -c 'gzip -d -f -c fpga/traffic_checker_hp_128.bin.gz  > /lib/firmware/traffic_checker_hp_128.bin'
shell$ sudo sh -c 'gzip -d -f -c fpga/traffic_checker_acp_128.bin.gz > /lib/firmware/traffic_checker_acp_128.bin'
```

Quick Start (for python script with uio and u-dma-buf)
------------------------------------------------------------------------------------

### Use traffic_checker_hp_128

#### Install BitStream to FPGA and Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i traffic_checker_hp_128_uio --dts device-tree-overlay/traffic_checker_hp_128_uio.dts
shell$ ls -la /dev/udmabuf_traffic_checker
crw------- 1 root root 239, 0 Apr  6 11:05 /dev/udmabuf_traffic_checker
```

#### Run Python Script

```console
shell$ sudo python3 python/mw_test.py -M 16 -N 10
Write_Traffic_Check : Version                : 0.4.0
Write_Traffic_Check : Frequency              : 100 MHz
Write_Traffic_Check : AXI Interface Type     : HP
Write_Traffic_Check : AXI Data Width         : 128 Bit
Write_Traffic_Check : Write Transaction Size : 4096 Byte
Write_Traffic_Check : Read  Transaction Size : 4096 Byte
Write_Traffic_Check : Cache Coherent         : False
Write_Traffic_Check : bytes_per_word         : 4
Write_Traffic_Check : write_words            : 4194304
Write_Traffic_Check : try_loops              : 10
Write_Traffic_Check : time                   :   10.836 # [msec]
Write_Traffic_Check : time                   :   10.758 # [msec]
Write_Traffic_Check : time                   :   10.788 # [msec]
Write_Traffic_Check : time                   :   10.699 # [msec]
Write_Traffic_Check : time                   :   10.750 # [msec]
Write_Traffic_Check : time                   :   10.736 # [msec]
Write_Traffic_Check : time                   :   10.672 # [msec]
Write_Traffic_Check : time                   :   10.667 # [msec]
Write_Traffic_Check : time                   :   10.696 # [msec]
Write_Traffic_Check : time                   :   10.769 # [msec]
Write_Traffic_Check : average_time           :   10.737 # [msec]
Write_Traffic_Check : throughput             : 1562.562 # [mbytes/sec]
[      0       1       2 ... 4194301 4194302 4194303]
Write_Traffic_Check : MW Total Count         : 1056787
Write_Traffic_Check : MW Address Xfer Count  : 4096
Write_Traffic_Check : MW Address Valid Count : 4096
Write_Traffic_Check : MW Address Ready Count : 1056787
Write_Traffic_Check : MW Data Xfer Count     : 1048576
Write_Traffic_Check : MW Data Valid Count    : 1048576
Write_Traffic_Check : MW Data Ready Count    : 1056787
```

```console
shell$ sudo python3 python/mr_test.py -M 16 -N 10
Read_Traffic_Check : Version                : 0.4.0
Read_Traffic_Check : Frequency              : 100 MHz
Read_Traffic_Check : AXI Interface Type     : HP
Read_Traffic_Check : AXI Data Width         : 128 Bit
Read_Traffic_Check : Write Transaction Size : 4096 Byte
Read_Traffic_Check : Read  Transaction Size : 4096 Byte
Read_Traffic_Check : Cache Coherent         : False
Read_Traffic_Check : bytes_per_word         : 4
Read_Traffic_Check : read_words             : 4194304
Read_Traffic_Check : try_loops              : 10
Read_Traffic_Check : time                   :   21.329 # [msec]
Read_Traffic_Check : time                   :   21.224 # [msec]
Read_Traffic_Check : time                   :   21.217 # [msec]
Read_Traffic_Check : time                   :   21.234 # [msec]
Read_Traffic_Check : time                   :   21.223 # [msec]
Read_Traffic_Check : time                   :   21.198 # [msec]
Read_Traffic_Check : time                   :   21.219 # [msec]
Read_Traffic_Check : time                   :   21.223 # [msec]
Read_Traffic_Check : time                   :   21.199 # [msec]
Read_Traffic_Check : time                   :   21.183 # [msec]
Read_Traffic_Check : average_time           :   21.225 # [msec]
Read_Traffic_Check : throughput             :  790.443 # [mbytes/sec]
Read_Traffic_Check : MR Total Count         : 2105364
Read_Traffic_Check : MR Address Xfer Count  : 4096
Read_Traffic_Check : MR Address Valid Count : 4096
Read_Traffic_Check : MR Address Ready Count : 2105364
Read_Traffic_Check : MR Data Xfer Count     : 1048576
Read_Traffic_Check : MR Data Valid Count    : 2105341
Read_Traffic_Check : MR Data Ready Count    : 1048594
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_hp_128_uio
```

### Use traffic_checker_acp_128

#### Install BitStream to FPGA and Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i traffic_checker_acp_128_uio --dts device-tree-overlay/traffic_checker_acp_128_uio.dts
shell$ ls -la /dev/udmabuf_traffic_checker
crw------- 1 root root 239, 0 Apr  6 11:05 /dev/udmabuf_traffic_checker
```

#### Run Python Script

```console
shell$ sudo python3 python/mw_test.py -M 16 -N 10
Write_Traffic_Check : Version                : 0.4.0
Write_Traffic_Check : Frequency              : 100 MHz
Write_Traffic_Check : AXI Interface Type     : ACP
Write_Traffic_Check : AXI Data Width         : 128 Bit
Write_Traffic_Check : Write Transaction Size : 4096 Byte
Write_Traffic_Check : Read  Transaction Size : 4096 Byte
Write_Traffic_Check : Cache Coherent         : True
Write_Traffic_Check : bytes_per_word         : 4
Write_Traffic_Check : write_words            : 4194304
Write_Traffic_Check : try_loops              : 10
Write_Traffic_Check : time                   :   11.281 # [msec]
Write_Traffic_Check : time                   :   11.266 # [msec]
Write_Traffic_Check : time                   :   11.250 # [msec]
Write_Traffic_Check : time                   :   11.327 # [msec]
Write_Traffic_Check : time                   :   11.283 # [msec]
Write_Traffic_Check : time                   :   11.250 # [msec]
Write_Traffic_Check : time                   :   11.321 # [msec]
Write_Traffic_Check : time                   :   11.309 # [msec]
Write_Traffic_Check : time                   :   11.304 # [msec]
Write_Traffic_Check : time                   :   11.407 # [msec]
Write_Traffic_Check : average_time           :   11.300 # [msec]
Write_Traffic_Check : throughput             : 1484.742 # [mbytes/sec]
[      0       1       2 ... 4194301 4194302 4194303]
Write_Traffic_Check : MW Total Count         : 1118515
Write_Traffic_Check : MW Address Xfer Count  : 4096
Write_Traffic_Check : MW Address Valid Count : 1101839
Write_Traffic_Check : MW Address Ready Count : 4119
Write_Traffic_Check : MW Data Xfer Count     : 1048576
Write_Traffic_Check : MW Data Valid Count    : 1110284
Write_Traffic_Check : MW Data Ready Count    : 1048601
```

```console
shell$ sudo python3 python/mr_test.py -M 16 -N 10
Read_Traffic_Check : Version                : 0.4.0
Read_Traffic_Check : Frequency              : 100 MHz
Read_Traffic_Check : AXI Interface Type     : ACP
Read_Traffic_Check : AXI Data Width         : 128 Bit
Read_Traffic_Check : Write Transaction Size : 4096 Byte
Read_Traffic_Check : Read  Transaction Size : 4096 Byte
Read_Traffic_Check : Cache Coherent         : True
Read_Traffic_Check : bytes_per_word         : 4
Read_Traffic_Check : read_words             : 4194304
Read_Traffic_Check : try_loops              : 10
Read_Traffic_Check : time                   :   31.389 # [msec]
Read_Traffic_Check : time                   :   31.057 # [msec]
Read_Traffic_Check : time                   :   30.980 # [msec]
Read_Traffic_Check : time                   :   30.890 # [msec]
Read_Traffic_Check : time                   :   30.875 # [msec]
Read_Traffic_Check : time                   :   31.018 # [msec]
Read_Traffic_Check : time                   :   30.612 # [msec]
Read_Traffic_Check : time                   :   31.016 # [msec]
Read_Traffic_Check : time                   :   30.863 # [msec]
Read_Traffic_Check : time                   :   30.762 # [msec]
Read_Traffic_Check : average_time           :   30.946 # [msec]
Read_Traffic_Check : throughput             :  542.141 # [mbytes/sec]
Read_Traffic_Check : MR Total Count         : 3053001
Read_Traffic_Check : MR Address Xfer Count  : 4096
Read_Traffic_Check : MR Address Valid Count : 2345924
Read_Traffic_Check : MR Address Ready Count : 4261
Read_Traffic_Check : MR Data Xfer Count     : 1048576
Read_Traffic_Check : MR Data Valid Count    : 1835008
Read_Traffic_Check : MR Data Ready Count    : 1996231
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_acp_128_uio
```

Quick Start (for video4linux(v4l2) driver and v4l2 capture program)
------------------------------------------------------------------------------------

### Build

#### Build v4l2 driver

```console
shell$ cd v4l2/driver
shell$ make
make -C /lib/modules/6.1.70-zynqmp-fpga-trial/build ARCH=arm64 CROSS_COMPILE= M=/home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver CONFIG_VB2_TEST=m modules
make[1]: Entering directory '/usr/src/linux-headers-6.1.70-zynqmp-fpga-trial'
  CC [M]  /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/vb2-test-core.o
  LD [M]  /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/vb2-test.o
  MODPOST /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/Module.symvers
  LD [M]  /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/vb2-test.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.1.70-zynqmp-fpga-trial'
shell$ cd ../..
```

#### Build v4l2 capture program

```console
shell$ cd v4l2/capture
shell$ make
g++ -g -o v4l2_capture_test v4l2_capture_test.cpp
shell$ cd ../..
```

### Install v4l2 driver

```console
shell$ cd v4l2/driver
shell$ sudo modprobe videobuf2-dma-contig
shell$ sudo insmod vb2-test.ko
shell$ cd ../..
```

### Use traffic_checker_hp_128

#### Install BitStream to FPGA and Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i traffic_checker_hp_128_v4l2 --dts device-tree-overlay/traffic_checker_hp_128_v4l2.dts
shell$ ls -la /dev/video0 
crw-rw----+ 1 root video 81, 0 Apr  6 11:32 /dev/video0
```

#### Run v4l2_capture_test with V4L2_MEMORY_MMAP

```console
shell$ sudo ./v4l2/capture/v4l2_capture_test -W 1280 -H 800 -C 10 -Q 4 
VideoDevice    : /dev/video0
Format: 
  Width        : 1280
  Height       : 800
  BytesPerLine : 5120
  SizeImage    : 4096000
Buffers: 
  Type : V4L2_MEMORY_MMAP
  Size : 4
  0: {start: 0x7f91aa8000, size: 4096000}
  1: {start: 0x7f916c0000, size: 4096000}
  2: {start: 0x7f912d8000, size: 4096000}
  3: {start: 0x7f90ef0000, size: 4096000}
Times: # microseconds 
  0: { Total: 146320, Wait: 2765, Dequeue: 9, Check: 143486, Enqueue: 58 }
  1: { Total: 140160, Wait: 7, Dequeue: 6, Check: 140118, Enqueue: 27 }
  2: { Total: 140080, Wait: 4, Dequeue: 4, Check: 140041, Enqueue: 29 }
  3: { Total: 140562, Wait: 5, Dequeue: 3, Check: 140520, Enqueue: 31 }
  4: { Total: 139940, Wait: 4, Dequeue: 4, Check: 139907, Enqueue: 22 }
  5: { Total: 140522, Wait: 4, Dequeue: 3, Check: 140471, Enqueue: 42 }
  6: { Total: 139895, Wait: 5, Dequeue: 4, Check: 139859, Enqueue: 26 }
  7: { Total: 140096, Wait: 4, Dequeue: 3, Check: 140043, Enqueue: 44 }
  8: { Total: 140078, Wait: 5, Dequeue: 5, Check: 140035, Enqueue: 31 }
  9: { Total: 139895, Wait: 4, Dequeue: 4, Check: 139865, Enqueue: 20 }
  Avarage: { Total: 140755, Wait: 280.7, Dequeue: 4.5, Check: 140434, Enqueue: 33}
```

#### Run v4l2_capture_test with V4L2_MEMORY_DMABUF

```console
shell$ sudo ./v4l2/capture/v4l2_capture_test -W 1280 -H 800 -C 10 -Q 4 -D
VideoDevice    : /dev/video0
DmaHeapDevice  : /dev/dma_heap/reserved
Format: 
  Width        : 1280
  Height       : 800
  BytesPerLine : 5120
  SizeImage    : 4096000
Buffers: 
  Type : V4L2_MEMORY_DMABUF
  Size : 4
  0: {start: 0x7f8f6b8000, size: 4096000, fd: 5}
  1: {start: 0x7f8f2d0000, size: 4096000, fd: 6}
  2: {start: 0x7f8eee8000, size: 4096000, fd: 7}
  3: {start: 0x7f8eb00000, size: 4096000, fd: 8}
Times: # microseconds 
  0: { Total: 20136, Wait: 2563, Dequeue: 9, Check: 17505, Enqueue: 56 }
  1: { Total: 17198, Wait: 6, Dequeue: 6, Check: 17127, Enqueue: 57 }
  2: { Total: 17209, Wait: 11, Dequeue: 6, Check: 17136, Enqueue: 54 }
  3: { Total: 17213, Wait: 11, Dequeue: 6, Check: 17136, Enqueue: 58 }
  4: { Total: 17367, Wait: 12, Dequeue: 6, Check: 17290, Enqueue: 57 }
  5: { Total: 17194, Wait: 12, Dequeue: 6, Check: 17118, Enqueue: 55 }
  6: { Total: 17210, Wait: 12, Dequeue: 6, Check: 17138, Enqueue: 53 }
  7: { Total: 17202, Wait: 6, Dequeue: 6, Check: 17130, Enqueue: 58 }
  8: { Total: 17203, Wait: 12, Dequeue: 6, Check: 17127, Enqueue: 56 }
  9: { Total: 17198, Wait: 13, Dequeue: 6, Check: 17123, Enqueue: 54 }
  Avarage: { Total: 17513, Wait: 265.8, Dequeue: 6.3, Check: 17183, Enqueue: 55.8}
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_hp_128_v4l2
```

### Use traffic_checker_acp_128

#### Install BitStream to FPGA and Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i traffic_checker_acp_128_v4l2 --dts device-tree-overlay/traffic_checker_acp_128_v4l2.dts
shell$ ls -la /dev/video0 
crw-rw----+ 1 root video 81, 0 Apr  6 11:32 /dev/video0
```

#### Run v4l2_capture_test with V4L2_MEMORY_MMAP

```console
shell$ sudo ./v4l2/capture/v4l2_capture_test -W 1280 -H 800 -C 10 -Q 4 
VideoDevice    : /dev/video0
Format: 
  Width        : 1280
  Height       : 800
  BytesPerLine : 5120
  SizeImage    : 4096000
Buffers: 
  Type : V4L2_MEMORY_MMAP
  Size : 4
  0: {start: 0x7f91998000, size: 4096000}
  1: {start: 0x7f915b0000, size: 4096000}
  2: {start: 0x7f911c8000, size: 4096000}
  3: {start: 0x7f90de0000, size: 4096000}
Times: # microseconds 
  0: { Total: 18434, Wait: 906, Dequeue: 24, Check: 17420, Enqueue: 82 }
  1: { Total: 18113, Wait: 19, Dequeue: 8, Check: 17996, Enqueue: 88 }
  2: { Total: 18273, Wait: 19, Dequeue: 7, Check: 18162, Enqueue: 82 }
  3: { Total: 17964, Wait: 18, Dequeue: 7, Check: 17855, Enqueue: 81 }
  4: { Total: 17910, Wait: 19, Dequeue: 8, Check: 17801, Enqueue: 81 }
  5: { Total: 17918, Wait: 19, Dequeue: 8, Check: 17809, Enqueue: 81 }
  6: { Total: 18222, Wait: 21, Dequeue: 7, Check: 18114, Enqueue: 77 }
  7: { Total: 17916, Wait: 19, Dequeue: 8, Check: 17806, Enqueue: 81 }
  8: { Total: 17923, Wait: 19, Dequeue: 7, Check: 17815, Enqueue: 79 }
  9: { Total: 17900, Wait: 19, Dequeue: 7, Check: 17791, Enqueue: 81 }
  Avarage: { Total: 18057.3, Wait: 107.8, Dequeue: 9.1, Check: 17856.9, Enqueue: 81.3}
```
#### Run v4l2_capture_test with V4L2_MEMORY_DMABUF

```console
shell$ sudo ./v4l2/capture/v4l2_capture_test -W 1280 -H 800 -C 10 -Q 4 -D
VideoDevice    : /dev/video0
DmaHeapDevice  : /dev/dma_heap/reserved
Format: 
  Width        : 1280
  Height       : 800
  BytesPerLine : 5120
  SizeImage    : 4096000
Buffers: 
  Type : V4L2_MEMORY_DMABUF
  Size : 4
  0: {start: 0x7fa9688000, size: 4096000, fd: 5}
  1: {start: 0x7fa92a0000, size: 4096000, fd: 6}
  2: {start: 0x7fa8eb8000, size: 4096000, fd: 7}
  3: {start: 0x7fa8ad0000, size: 4096000, fd: 8}
Times: # microseconds 
  0: { Total: 24036, Wait: 2666, Dequeue: 24, Check: 21244, Enqueue: 100 }
  1: { Total: 17539, Wait: 18, Dequeue: 8, Check: 17424, Enqueue: 86 }
  2: { Total: 17425, Wait: 18, Dequeue: 8, Check: 17314, Enqueue: 83 }
  3: { Total: 17390, Wait: 17, Dequeue: 9, Check: 17270, Enqueue: 93 }
  4: { Total: 17630, Wait: 12, Dequeue: 9, Check: 17531, Enqueue: 76 }
  5: { Total: 17413, Wait: 16, Dequeue: 8, Check: 17307, Enqueue: 80 }
  6: { Total: 17416, Wait: 20, Dequeue: 10, Check: 17304, Enqueue: 80 }
  7: { Total: 17678, Wait: 17, Dequeue: 10, Check: 17572, Enqueue: 76 }
  8: { Total: 17395, Wait: 19, Dequeue: 9, Check: 17287, Enqueue: 78 }
  9: { Total: 17424, Wait: 17, Dequeue: 9, Check: 17318, Enqueue: 78 }
  Avarage: { Total: 18134.6, Wait: 282, Dequeue: 10.4, Check: 17757.1, Enqueue: 83}
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_acp_128_v4l2
```

