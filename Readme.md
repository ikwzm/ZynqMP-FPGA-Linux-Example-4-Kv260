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
shell$ sudo python3 python/mw_test.py
Write_Traffic_Check : Version                : 0.3.1
Write_Traffic_Check : AXI Data Width         : 128 Bit
Write_Traffic_Check : Write Transaction Size : 4096 Byte
Write_Traffic_Check : Read  Transaction Size : 4096 Byte
Write_Traffic_Check : Cache Coherent         : False
[   0    1    2 ... 1021 1022 1023]
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
shell$ sudo python3 python/mw_test.py
Write_Traffic_Check : Version                : 0.3.3
Write_Traffic_Check : AXI Data Width         : 128 Bit
Write_Traffic_Check : Write Transaction Size : 4096 Byte
Write_Traffic_Check : Read  Transaction Size : 4096 Byte
Write_Traffic_Check : Cache Coherent         : True
[   0    1    2 ... 1021 1022 1023]
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
shell$ g++ -g -o v4l2_capture_test v4l2_capture_test.cpp
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
shell$ sudo ./v4l2/capture/v4l2_capture_test 
VideoDevice    : /dev/video0
Format: 
  Width        : 800
  Height       : 480
  BytesPerLine : 800
  SizeImage    : 384000
Buffers: 
  Type : V4L2_MEMORY_MMAP
  Size : 4
  0: {start: 0x7f9fa02000, size: 384000}
  1: {start: 0x7f9f9a4000, size: 384000}
  2: {start: 0x7f9f946000, size: 384000}
  3: {start: 0x7f9f8e8000, size: 384000}
Times: # microseconds 
  0: { Total: 13657, Wait: 225, Dequeue: 6, Check: 13412, Enqueue: 12 }
  1: { Total: 13434, Wait: 8, Dequeue: 5, Check: 13415, Enqueue: 4 }
  2: { Total: 13391, Wait: 3, Dequeue: 2, Check: 13380, Enqueue: 5 }
  3: { Total: 13387, Wait: 4, Dequeue: 3, Check: 13373, Enqueue: 6 }
  4: { Total: 13299, Wait: 4, Dequeue: 2, Check: 13287, Enqueue: 3 }
  5: { Total: 13326, Wait: 2, Dequeue: 2, Check: 13317, Enqueue: 3 }
  6: { Total: 13305, Wait: 2, Dequeue: 2, Check: 13296, Enqueue: 3 }
  7: { Total: 13307, Wait: 2, Dequeue: 2, Check: 13297, Enqueue: 4 }
  8: { Total: 13250, Wait: 3, Dequeue: 2, Check: 13241, Enqueue: 3 }
  9: { Total: 13226, Wait: 3, Dequeue: 2, Check: 13218, Enqueue: 3 }
  Avarage: { Total: 13358.2, Wait: 25.6, Dequeue: 2.8, Check: 13323.6, Enqueue: 4.6}
```

#### Run v4l2_capture_test with V4L2_MEMORY_DMABUF

```console
shell$ sudo ./v4l2/capture/v4l2_capture_test -D
VideoDevice    : /dev/video0
DmaHeapDevice  : /dev/dma_heap/reserved
Format: 
  Width        : 800
  Height       : 480
  BytesPerLine : 800
  SizeImage    : 384000
Buffers: 
  Type : V4L2_MEMORY_DMABUF
  Size : 4
  0: {start: 0x7fbcbe2000, size: 384000, fd: 5}
  1: {start: 0x7fbcb84000, size: 384000, fd: 6}
  2: {start: 0x7fbcb26000, size: 384000, fd: 7}
  3: {start: 0x7fbcac8000, size: 384000, fd: 8}
Times: # microseconds 
  0: { Total: 1926, Wait: 221, Dequeue: 6, Check: 1691, Enqueue: 6 }
  1: { Total: 1629, Wait: 3, Dequeue: 2, Check: 1618, Enqueue: 4 }
  2: { Total: 1678, Wait: 3, Dequeue: 2, Check: 1668, Enqueue: 4 }
  3: { Total: 1629, Wait: 3, Dequeue: 2, Check: 1619, Enqueue: 3 }
  4: { Total: 1685, Wait: 3, Dequeue: 2, Check: 1675, Enqueue: 4 }
  5: { Total: 1644, Wait: 3, Dequeue: 2, Check: 1631, Enqueue: 6 }
  6: { Total: 1631, Wait: 3, Dequeue: 2, Check: 1621, Enqueue: 4 }
  7: { Total: 1778, Wait: 3, Dequeue: 2, Check: 1763, Enqueue: 10 }
  8: { Total: 1633, Wait: 4, Dequeue: 3, Check: 1622, Enqueue: 4 }
  9: { Total: 1741, Wait: 3, Dequeue: 2, Check: 1730, Enqueue: 5 }
  Avarage: { Total: 1697.4, Wait: 24.9, Dequeue: 2.5, Check: 1663.8, Enqueue: 5}
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
shell$ sudo ./v4l2/capture/v4l2_capture_test
VideoDevice    : /dev/video0
Format: 
  Width        : 800
  Height       : 480
  BytesPerLine : 800
  SizeImage    : 384000
Buffers: 
  Type : V4L2_MEMORY_MMAP
  Size : 4
  0: {start: 0x7fa5a22000, size: 384000}
  1: {start: 0x7fa59c4000, size: 384000}
  2: {start: 0x7fa5966000, size: 384000}
  3: {start: 0x7fa5908000, size: 384000}
Times: # microseconds 
  0: { Total: 1868, Wait: 223, Dequeue: 7, Check: 1622, Enqueue: 14 }
  1: { Total: 1756, Wait: 10, Dequeue: 5, Check: 1721, Enqueue: 16 }
  2: { Total: 1677, Wait: 9, Dequeue: 6, Check: 1652, Enqueue: 7 }
  3: { Total: 1644, Wait: 6, Dequeue: 5, Check: 1625, Enqueue: 7 }
  4: { Total: 1784, Wait: 7, Dequeue: 5, Check: 1755, Enqueue: 16 }
  5: { Total: 1649, Wait: 9, Dequeue: 5, Check: 1624, Enqueue: 9 }
  6: { Total: 1817, Wait: 7, Dequeue: 4, Check: 1788, Enqueue: 17 }
  7: { Total: 1996, Wait: 9, Dequeue: 5, Check: 1960, Enqueue: 22 }
  8: { Total: 1692, Wait: 8, Dequeue: 5, Check: 1663, Enqueue: 15 }
  9: { Total: 1650, Wait: 9, Dequeue: 6, Check: 1624, Enqueue: 9 }
  Avarage: { Total: 1753.3, Wait: 29.7, Dequeue: 5.3, Check: 1703.4, Enqueue: 13.2}
```
#### Run v4l2_capture_test with V4L2_MEMORY_DMABUF

```console
shell$ sudo v4l2/capture/v4l2_capture_test -D
VideoDevice    : /dev/video0
DmaHeapDevice  : /dev/dma_heap/reserved
Format: 
  Width        : 800
  Height       : 480
  BytesPerLine : 800
  SizeImage    : 384000
Buffers: 
  Type : V4L2_MEMORY_DMABUF
  Size : 4
  0: {start: 0x7f856f2000, size: 384000, fd: 5}
  1: {start: 0x7f85694000, size: 384000, fd: 6}
  2: {start: 0x7f85636000, size: 384000, fd: 7}
  3: {start: 0x7f855d8000, size: 384000, fd: 8}
Times: # microseconds 
  0: { Total: 1849, Wait: 225, Dequeue: 8, Check: 1602, Enqueue: 12 }
  1: { Total: 1661, Wait: 8, Dequeue: 4, Check: 1632, Enqueue: 15 }
  2: { Total: 1627, Wait: 8, Dequeue: 5, Check: 1603, Enqueue: 10 }
  3: { Total: 1672, Wait: 7, Dequeue: 3, Check: 1640, Enqueue: 19 }
  4: { Total: 1631, Wait: 10, Dequeue: 6, Check: 1603, Enqueue: 11 }
  5: { Total: 1625, Wait: 6, Dequeue: 5, Check: 1602, Enqueue: 10 }
  6: { Total: 1664, Wait: 7, Dequeue: 5, Check: 1634, Enqueue: 16 }
  7: { Total: 1624, Wait: 9, Dequeue: 6, Check: 1600, Enqueue: 8 }
  8: { Total: 1776, Wait: 6, Dequeue: 4, Check: 1746, Enqueue: 18 }
  9: { Total: 1628, Wait: 10, Dequeue: 5, Check: 1603, Enqueue: 8 }
  Avarage: { Total: 1675.7, Wait: 29.6, Dequeue: 5.1, Check: 1626.5, Enqueue: 12.7}
```

### Uninstall Device Tree

```console
shell$ sudo ./device-tree-overlay/dtbo-config -r traffic_checker_acp_128_v4l2
```

