Build Bitstream files
------------------------------------------------------------------------------------

### Requirement

* Xilinx Vivado 2023.1

### Build traffic_checker_hp_128.bin

#### Create Project

```
Vivado > Tools > Run Tcl Script... > traffic_checker_hp_128/create_project.tcl
```

#### Implementation

```
Vivado > Tools > Run Tcl Script... > traffic_checker_hp_128/implementation.tcl
```

#### Convert from Bitstream File to Binary File

```console
vivado% cd traffic_checker_hp_128
vivado% bootgen -image traffic_checker_hp_128.bif -arch zynqmp -w -o ../traffic_checker_hp_128.bin
vivado% cd ..
```

#### Compress traffic_checker_hp_128.bin to traffic_checker_hp_128.bin.gz

```console
vivado% gzip traffic_checker_hp_128.bin
```

### Build traffic_checker_acp_128.bin

#### Create Project

```
Vivado > Tools > Run Tcl Script... > traffic_checker_acp_128/create_project.tcl
```

#### Implementation

```
Vivado > Tools > Run Tcl Script... > traffic_checker_acp_128/implementation.tcl
```

#### Convert from Bitstream File to Binary File

```console
vivado% cd traffic_checker_acp_128
vivado% bootgen -image traffic_checker_acp_128.bif -arch zynqmp -w -o ../traffic_checker_acp_128.bin
vivado% cd ..
```

#### Compress traffic_checker_acp_128.bin to traffic_checker_acp_128.bin.gz

```console
vivado% gzip traffic_checker_acp_128.bin
```

