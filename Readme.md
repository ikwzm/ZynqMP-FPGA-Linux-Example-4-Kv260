ZynqMP-FPGA-Example-4 for Kv260
====================================================================================

Overvier
------------------------------------------------------------------------------------

### Requirement

* Board: any of the following
  - Kv260
* OS: any of the following
  - https://github.com/ikwzm/ZynqMP-FPGA-Debian13
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
shell$ cd device-tree-overlay
shell$ make traffic_checker_hp_128_uio_250MHz.dts
./dtbo-compile -P -DFPGA_REGION=\\\"/fpga-region\\\" -DS_AXI_TYPE_HP_128  -DDRV_TYPE_UIO  -DFCLK_RATE_250MHz -O dts -o traffic_checker_hp_128_uio_250MHz.dts traffic_checker_template.dts
shell$ cd ..
```

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i --dts device-tree-overlay/traffic_checker_hp_128_uio_250MHz.dts
shell$ ls -la /dev/udmabuf_traffic_checker
crw------- 1 root root 234, 0 Nov 20 07:14 /dev/udmabuf_traffic_checker
```

#### Run Python Script

```console
shell$ sudo python3 python/mw_test.py -M 16 -N 10
Write_Traffic_Check : Version                : 0.6.0
Write_Traffic_Check : Frequency              : 250 MHz
Write_Traffic_Check : AXI Interface Type     : HP
Write_Traffic_Check : AXI Data Width         : 128 Bit
Write_Traffic_Check : Write Transaction Size : 4096 Byte
Write_Traffic_Check : Read  Transaction Size : 4096 Byte
Write_Traffic_Check : Cache Coherent         : False
Write_Traffic_Check : bytes_per_word         : 4
Write_Traffic_Check : write_words            : 4194304
Write_Traffic_Check : try_loops              : 10
Write_Traffic_Check : time                   :    4.515 # [msec]
Write_Traffic_Check : time                   :    4.449 # [msec]
Write_Traffic_Check : time                   :    4.373 # [msec]
Write_Traffic_Check : time                   :    4.338 # [msec]
Write_Traffic_Check : time                   :    4.361 # [msec]
Write_Traffic_Check : time                   :    4.382 # [msec]
Write_Traffic_Check : time                   :    4.400 # [msec]
Write_Traffic_Check : time                   :    4.392 # [msec]
Write_Traffic_Check : time                   :    4.338 # [msec]
Write_Traffic_Check : time                   :    4.357 # [msec]
Write_Traffic_Check : average_time           :    4.390 # [msec]
Write_Traffic_Check : throughput             : 3821.335 # [mbytes/sec]
[      0       1       2 ... 4194301 4194302 4194303]
Write_Traffic_Check : MW Total Count         : 1056801
Write_Traffic_Check : MW Address Xfer Count  : 4096
Write_Traffic_Check : MW Address Valid Count : 4096
Write_Traffic_Check : MW Address Ready Count : 1056801
Write_Traffic_Check : MW Data Xfer Count     : 1048576
Write_Traffic_Check : MW Data Valid Count    : 1048576
Write_Traffic_Check : MW Data Ready Count    : 1056801
```

```console
shell$ sudo python3 python/mr_test.py -M 16 -N 10
Read_Traffic_Check : Version                : 0.6.0
Read_Traffic_Check : Frequency              : 250 MHz
Read_Traffic_Check : AXI Interface Type     : HP
Read_Traffic_Check : AXI Data Width         : 128 Bit
Read_Traffic_Check : Write Transaction Size : 4096 Byte
Read_Traffic_Check : Read  Transaction Size : 4096 Byte
Read_Traffic_Check : Cache Coherent         : False
Read_Traffic_Check : bytes_per_word         : 4
Read_Traffic_Check : read_words             : 4194304
Read_Traffic_Check : try_loops              : 10
Read_Traffic_Check : time                   :    8.730 # [msec]
Read_Traffic_Check : time                   :    8.706 # [msec]
Read_Traffic_Check : time                   :    8.565 # [msec]
Read_Traffic_Check : time                   :    8.571 # [msec]
Read_Traffic_Check : time                   :    8.616 # [msec]
Read_Traffic_Check : time                   :    8.547 # [msec]
Read_Traffic_Check : time                   :    8.549 # [msec]
Read_Traffic_Check : time                   :    8.548 # [msec]
Read_Traffic_Check : time                   :    8.608 # [msec]
Read_Traffic_Check : time                   :    8.571 # [msec]
Read_Traffic_Check : average_time           :    8.601 # [msec]
Read_Traffic_Check : throughput             : 1950.602 # [mbytes/sec]
Read_Traffic_Check : MR Total Count         : 2105388
Read_Traffic_Check : MR Address Xfer Count  : 4096
Read_Traffic_Check : MR Address Valid Count : 4096
Read_Traffic_Check : MR Address Ready Count : 2105388
Read_Traffic_Check : MR Data Xfer Count     : 1048576
Read_Traffic_Check : MR Data Valid Count    : 2105341
Read_Traffic_Check : MR Data Ready Count    : 1048618
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_hp_128_uio_250MHz
```

### Use traffic_checker_acp_128

#### Install BitStream to FPGA and Device Tree

```console
shell$ cd device-tree-overlay
shell$ make traffic_checker_acp_128_uio_250MHz.dts
./dtbo-compile -P -DFPGA_REGION=\\\"/fpga-region\\\" -DS_AXI_TYPE_ACP_128 -DDRV_TYPE_UIO  -DFCLK_RATE_250MHz -O dts -o traffic_checker_acp_128_uio_250MHz.dts traffic_checker_template.dts
shell$ cd ..
```

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i --dts device-tree-overlay/traffic_checker_acp_128_uio_250MHz.dts
shell$ ls -la /dev/udmabuf_traffic_checker
crw------- 1 root root 234, 0 Nov 20 07:20 /dev/udmabuf_traffic_checker
```

#### Run Python Script

```console
shell$ sudo python3 python/mw_test.py -M 16 -N 10
Write_Traffic_Check : Version                : 0.6.0
Write_Traffic_Check : Frequency              : 250 MHz
Write_Traffic_Check : AXI Interface Type     : ACP
Write_Traffic_Check : AXI Data Width         : 128 Bit
Write_Traffic_Check : Write Transaction Size : 4096 Byte
Write_Traffic_Check : Read  Transaction Size : 4096 Byte
Write_Traffic_Check : Cache Coherent         : True
Write_Traffic_Check : bytes_per_word         : 4
Write_Traffic_Check : write_words            : 4194304
Write_Traffic_Check : try_loops              : 10
Write_Traffic_Check : time                   :   11.306 # [msec]
Write_Traffic_Check : time                   :   11.151 # [msec]
Write_Traffic_Check : time                   :   11.445 # [msec]
Write_Traffic_Check : time                   :   11.342 # [msec]
Write_Traffic_Check : time                   :   11.203 # [msec]
Write_Traffic_Check : time                   :   11.481 # [msec]
Write_Traffic_Check : time                   :   11.462 # [msec]
Write_Traffic_Check : time                   :   11.414 # [msec]
Write_Traffic_Check : time                   :   11.182 # [msec]
Write_Traffic_Check : time                   :   11.378 # [msec]
Write_Traffic_Check : average_time           :   11.336 # [msec]
Write_Traffic_Check : throughput             : 1479.952 # [mbytes/sec]
[      0       1       2 ... 4194301 4194302 4194303]
Write_Traffic_Check : MW Total Count         : 2779047
Write_Traffic_Check : MW Address Xfer Count  : 4096
Write_Traffic_Check : MW Address Valid Count : 2761932
Write_Traffic_Check : MW Address Ready Count : 4148
Write_Traffic_Check : MW Data Xfer Count     : 1048576
Write_Traffic_Check : MW Data Valid Count    : 2770762
Write_Traffic_Check : MW Data Ready Count    : 1048630
```

```console
shell$ sudo python3 python/mr_test.py -M 16 -N 10
Read_Traffic_Check : Version                : 0.6.0
Read_Traffic_Check : Frequency              : 250 MHz
Read_Traffic_Check : AXI Interface Type     : ACP
Read_Traffic_Check : AXI Data Width         : 128 Bit
Read_Traffic_Check : Write Transaction Size : 4096 Byte
Read_Traffic_Check : Read  Transaction Size : 4096 Byte
Read_Traffic_Check : Cache Coherent         : True
Read_Traffic_Check : bytes_per_word         : 4
Read_Traffic_Check : read_words             : 4194304
Read_Traffic_Check : try_loops              : 10
Read_Traffic_Check : time                   :   31.675 # [msec]
Read_Traffic_Check : time                   :   31.389 # [msec]
Read_Traffic_Check : time                   :   31.059 # [msec]
Read_Traffic_Check : time                   :   31.071 # [msec]
Read_Traffic_Check : time                   :   31.100 # [msec]
Read_Traffic_Check : time                   :   30.966 # [msec]
Read_Traffic_Check : time                   :   31.235 # [msec]
Read_Traffic_Check : time                   :   30.943 # [msec]
Read_Traffic_Check : time                   :   31.225 # [msec]
Read_Traffic_Check : time                   :   30.961 # [msec]
Read_Traffic_Check : average_time           :   31.162 # [msec]
Read_Traffic_Check : throughput             :  538.381 # [mbytes/sec]
Read_Traffic_Check : MR Total Count         : 7663323
Read_Traffic_Check : MR Address Xfer Count  : 4096
Read_Traffic_Check : MR Address Valid Count : 5959028
Read_Traffic_Check : MR Address Ready Count : 4492
Read_Traffic_Check : MR Data Xfer Count     : 1048576
Read_Traffic_Check : MR Data Valid Count    : 1835008
Read_Traffic_Check : MR Data Ready Count    : 6606553
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_acp_128_uio_250MHz
```

Quick Start (for video4linux(v4l2) driver and v4l2 capture program)
------------------------------------------------------------------------------------

### Build

#### Build v4l2 driver

```console
shell$ cd v4l2/driver
shell$ make
make -C /lib/modules/6.12.45-zynqmp-fpga-generic/build ARCH=arm64 CROSS_COMPILE= M=/home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver CONFIG_VB2_TEST=m modules
make[1]: Entering directory '/usr/src/linux-headers-6.12.45-zynqmp-fpga-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: aarch64-linux-gnu-gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
  You are using:           gcc (Debian 14.2.0-19) 14.2.0
  CC [M]  /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/vb2-test-core.o
  LD [M]  /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/vb2-test.o
  MODPOST /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/Module.symvers
  LD [M]  /home/fpga/work/ZynqMP-FPGA-Linux-Example-4-Kv260/v4l2/driver/vb2-test.ko
make[1]: Leaving directory '/usr/src/linux-headers-6.12.45-zynqmp-fpga-generic'
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
shell$ cd device-tree-overlay
shell$ make traffic_checker_hp_128_v4l2_250MHz.dts
./dtbo-compile -P -DFPGA_REGION=\\\"/fpga-region\\\" -DS_AXI_TYPE_HP_128  -DDRV_TYPE_V4L2 -DFCLK_RATE_250MHz -O dts -o traffic_checker_hp_128_v4l2_250MHz.dts traffic_checker_template.dts
shell$ cd ..
```

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i --dts device-tree-overlay/traffic_checker_hp_128_v4l2_250MHz.dts
shell$ ls -la /dev/video0 
crw-rw---- 1 root video 81, 0 Nov 20 07:26 /dev/video0
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
  0: {start: 0xffff9f598000, size: 4096000}
  1: {start: 0xffff9f1b0000, size: 4096000}
  2: {start: 0xffff9edc8000, size: 4096000}
  3: {start: 0xffff9e9e0000, size: 4096000}
Times: # microseconds 
  0: { Total: 150638, Wait: 1038, Dequeue: 8, Check: 149542, Enqueue: 49 }
  1: { Total: 148736, Wait: 8, Dequeue: 5, Check: 148701, Enqueue: 20 }
  2: { Total: 148895, Wait: 5, Dequeue: 2, Check: 148853, Enqueue: 33 }
  3: { Total: 148799, Wait: 6, Dequeue: 4, Check: 148768, Enqueue: 19 }
  4: { Total: 148710, Wait: 4, Dequeue: 3, Check: 148686, Enqueue: 15 }
  5: { Total: 148772, Wait: 4, Dequeue: 2, Check: 148749, Enqueue: 16 }
  6: { Total: 148648, Wait: 4, Dequeue: 2, Check: 148625, Enqueue: 15 }
  7: { Total: 148817, Wait: 4, Dequeue: 2, Check: 148794, Enqueue: 16 }
  8: { Total: 148642, Wait: 4, Dequeue: 2, Check: 148619, Enqueue: 15 }
  9: { Total: 148671, Wait: 3, Dequeue: 2, Check: 148647, Enqueue: 16 }
  Avarage: { Total: 148933, Wait: 108, Dequeue: 3.2, Check: 148798, Enqueue: 21.4}
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
  0: {start: 0xffff8e2e8000, size: 4096000, fd: 5}
  1: {start: 0xffff8df00000, size: 4096000, fd: 6}
  2: {start: 0xffff8db18000, size: 4096000, fd: 7}
  3: {start: 0xffff8d730000, size: 4096000, fd: 8}
Times: # microseconds 
  0: { Total: 19537, Wait: 1035, Dequeue: 6, Check: 18431, Enqueue: 64 }
  1: { Total: 18069, Wait: 15, Dequeue: 6, Check: 17987, Enqueue: 58 }
  2: { Total: 17813, Wait: 9, Dequeue: 6, Check: 17732, Enqueue: 64 }
  3: { Total: 17327, Wait: 15, Dequeue: 6, Check: 17239, Enqueue: 65 }
  4: { Total: 17232, Wait: 17, Dequeue: 6, Check: 17150, Enqueue: 58 }
  5: { Total: 17411, Wait: 16, Dequeue: 6, Check: 17331, Enqueue: 56 }
  6: { Total: 17283, Wait: 16, Dequeue: 6, Check: 17200, Enqueue: 60 }
  7: { Total: 17329, Wait: 15, Dequeue: 6, Check: 17249, Enqueue: 56 }
  8: { Total: 17230, Wait: 16, Dequeue: 7, Check: 17148, Enqueue: 57 }
  9: { Total: 17823, Wait: 14, Dequeue: 6, Check: 17745, Enqueue: 56 }
  Avarage: { Total: 17705.4, Wait: 116.8, Dequeue: 6.1, Check: 17521.2, Enqueue: 59.4}
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_hp_128_v4l2_250MHz
```

### Use traffic_checker_acp_128

#### Install BitStream to FPGA and Device Tree

```console
shell$ cd device-tree-overlay
shell$ make traffic_checker_acp_128_v4l2_250MHz.dts
./dtbo-compile -P -DFPGA_REGION=\\\"/fpga-region\\\" -DS_AXI_TYPE_ACP_128 -DDRV_TYPE_V4L2  -DFCLK_RATE_250MHz -O dts -o traffic_checker_acp_128_v4l2_250MHz.dts traffic_checker_template.dts
shell$ cd ..
```

```console
shell$ sudo ./device-tree-overlay/dtbo-config -i --dts device-tree-overlay/traffic_checker_acp_128_v4l2_250MHz.dts
shell$ ls -la /dev/video0 
crw-rw---- 1 root video 81, 0 Nov 20 07:30 /dev/video0
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
  0: {start: 0xffffa1748000, size: 4096000}
  1: {start: 0xffffa1360000, size: 4096000}
  2: {start: 0xffffa0f78000, size: 4096000}
  3: {start: 0xffffa0b90000, size: 4096000}
Times: # microseconds 
  0: { Total: 20180, Wait: 2667, Dequeue: 25, Check: 17411, Enqueue: 75 }
  1: { Total: 17444, Wait: 23, Dequeue: 8, Check: 17340, Enqueue: 71 }
  2: { Total: 17431, Wait: 22, Dequeue: 8, Check: 17326, Enqueue: 73 }
  3: { Total: 17788, Wait: 23, Dequeue: 7, Check: 17684, Enqueue: 72 }
  4: { Total: 17648, Wait: 19, Dequeue: 7, Check: 17547, Enqueue: 72 }
  5: { Total: 17426, Wait: 18, Dequeue: 7, Check: 17328, Enqueue: 70 }
  6: { Total: 17426, Wait: 19, Dequeue: 8, Check: 17320, Enqueue: 77 }
  7: { Total: 17509, Wait: 21, Dequeue: 8, Check: 17334, Enqueue: 143 }
  8: { Total: 17901, Wait: 25, Dequeue: 9, Check: 17792, Enqueue: 73 }
  9: { Total: 18131, Wait: 22, Dequeue: 7, Check: 17975, Enqueue: 125 }
  Avarage: { Total: 17888.4, Wait: 285.9, Dequeue: 9.4, Check: 17505.7, Enqueue: 85.1}
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
  0: {start: 0xffffae368000, size: 4096000, fd: 5}
  1: {start: 0xffffadf80000, size: 4096000, fd: 6}
  2: {start: 0xffffadb98000, size: 4096000, fd: 7}
  3: {start: 0xffffad7b0000, size: 4096000, fd: 8}
Times: # microseconds 
  0: { Total: 20364, Wait: 2632, Dequeue: 26, Check: 17620, Enqueue: 84 }
  1: { Total: 17481, Wait: 21, Dequeue: 8, Check: 17371, Enqueue: 79 }
  2: { Total: 17419, Wait: 18, Dequeue: 9, Check: 17313, Enqueue: 77 }
  3: { Total: 17446, Wait: 20, Dequeue: 10, Check: 17337, Enqueue: 77 }
  4: { Total: 17773, Wait: 21, Dequeue: 8, Check: 17664, Enqueue: 77 }
  5: { Total: 18040, Wait: 22, Dequeue: 8, Check: 17927, Enqueue: 81 }
  6: { Total: 17735, Wait: 18, Dequeue: 7, Check: 17626, Enqueue: 82 }
  7: { Total: 17557, Wait: 22, Dequeue: 7, Check: 17439, Enqueue: 87 }
  8: { Total: 17458, Wait: 22, Dequeue: 8, Check: 17349, Enqueue: 77 }
  9: { Total: 17436, Wait: 22, Dequeue: 9, Check: 17324, Enqueue: 80 }
  Avarage: { Total: 17870.9, Wait: 281.8, Dequeue: 10, Check: 17497, Enqueue: 80.1}
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_acp_128_v4l2_250MHz
```

Build Bitstream files
------------------------------------------------------------------------------------

 * [fpga/Build.md](fpga/Build.md)


