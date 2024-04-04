
################################################################
# This is a generated script based on design: design_1
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2023.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source design_1_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xczu3eg-sbva484-1-e
   set_property BOARD_PART xilinx.com:kv260_som_som240_1_connector_kv260_carrier_som240_1_connector:part0:1.3 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name design_1

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:axi_bram_ctrl:4.1\
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:xlslice:1.0\
xilinx.com:ip:system_ila:1.1\
xilinx.com:ip:zynq_ultra_ps_e:3.5\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################

source [file join $project_directory "add_fan_enable.tcl"  ]

# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_msg_id "BD_TCL-101" "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create instance: zynq_ultra_ps_e_0, and set properties
  set zynq_ultra_ps_e_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e:3.5 zynq_ultra_ps_e_0 ]
  apply_bd_automation -rule xilinx.com:bd_rule:zynq_ultra_ps_e -config {apply_board_preset "1"} $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__USE__M_AXI_GP0   {1} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__USE__M_AXI_GP1   {0} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__USE__S_AXI_GP0   {1} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__USE__IRQ0        {1} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__USE__FABRIC__RST {1} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__FPGA_PL0_ENABLE  {1} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__FPGA_PL1_ENABLE  {0} ] $zynq_ultra_ps_e_0
  set_property -dict [list CONFIG.PSU__CRL_APB__PL0_REF_CTRL__FREQMHZ {250.000000} ] $zynq_ultra_ps_e_0

  # 
  # Add FAN_EN
  #
  add_fan_enable $zynq_ultra_ps_e_0 FAN_EN ttc0
  # 
  # Create instance: proc_sys_reset_0 and set properties
  #
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]
  # 
  # Create instance: AXI_TRAFFIC_CHECKER and set properties
  #
  set AXI_TRAFFIC_CHECKER_0 [ create_bd_cell -type ip -vlnv ikwzm:PIPEWORK:AXI_TRAFFIC_CHECKER:0.2 AXI_TRAFFIC_CHECKER_0 ]
  set_property -dict [ list CONFIG.BUILD_VERSION   {2} ] $AXI_TRAFFIC_CHECKER_0
  set_property -dict [ list CONFIG.M_ADDR_WIDTH   {64} ] $AXI_TRAFFIC_CHECKER_0
  set_property -dict [ list CONFIG.M_DATA_WIDTH  {128} ] $AXI_TRAFFIC_CHECKER_0
  set_property -dict [ list CONFIG.M_ID_WIDTH      {6} ] $AXI_TRAFFIC_CHECKER_0
  set_property -dict [ list CONFIG.M_AUSER_WIDTH   {1} ] $AXI_TRAFFIC_CHECKER_0
  #
  # Create instance: axi_interconnect_csr, and set properties
  #
  set axi_interconnect_csr [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_csr ]
  set_property -dict [ list CONFIG.NUM_MI {1} ] $axi_interconnect_csr
  #
  # Create instance: axi_interconnect_hpc0, and set properties
  #
  set axi_interconnect_hpc0  [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_hpc0 ]
  set_property -dict [ list CONFIG.NUM_MI {1} ] $axi_interconnect_hpc0
  #
  # Connect
  #
  connect_bd_net -net SYS_CLK0   [get_bd_pins zynq_ultra_ps_e_0/pl_clk0]           \
                                 [get_bd_pins zynq_ultra_ps_e_0/maxihpm0_fpd_aclk] \
                                 [get_bd_pins zynq_ultra_ps_e_0/saxihpc0_fpd_aclk] \
                                 [get_bd_pins proc_sys_reset_0/slowest_sync_clk]   \
                                 [get_bd_pins axi_interconnect_csr/ACLK]           \
                                 [get_bd_pins axi_interconnect_csr/S00_ACLK]       \
                                 [get_bd_pins axi_interconnect_csr/M00_ACLK]       \
                                 [get_bd_pins axi_interconnect_hpc0/ACLK]          \
                                 [get_bd_pins axi_interconnect_hpc0/S00_ACLK]      \
                                 [get_bd_pins axi_interconnect_hpc0/M00_ACLK]      \
                                 [get_bd_pins AXI_TRAFFIC_CHECKER_0/ACLK]    

  connect_bd_net -net PL_RESETN  [get_bd_pins zynq_ultra_ps_e_0/pl_resetn0] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net SYS_RESETN [get_bd_pins proc_sys_reset_0/peripheral_aresetn] \
                                 [get_bd_pins axi_interconnect_csr/ARESETN]        \
                                 [get_bd_pins axi_interconnect_csr/S00_ARESETN]    \
                                 [get_bd_pins axi_interconnect_csr/M00_ARESETN]    \
                                 [get_bd_pins axi_interconnect_hpc0/ARESETN]       \
                                 [get_bd_pins axi_interconnect_hpc0/S00_ARESETN]   \
                                 [get_bd_pins axi_interconnect_hpc0/M00_ARESETN]   \
                                 [get_bd_pins AXI_TRAFFIC_CHECKER_0/ARESETn]    
  connect_bd_net -net IRQ0       [get_bd_pins AXI_TRAFFIC_CHECKER_0/IRQ]           \
                                 [get_bd_pins zynq_ultra_ps_e_0/pl_ps_irq0]
  #
  # Connect Interfaces
  #
  connect_bd_intf_net -intf_net AXI_CSR     -boundary_type upper [get_bd_intf_pins zynq_ultra_ps_e_0/M_AXI_HPM0_FPD]  [get_bd_intf_pins axi_interconnect_csr/S00_AXI]
  connect_bd_intf_net -intf_net AXI_HPC0    -boundary_type upper [get_bd_intf_pins zynq_ultra_ps_e_0/S_AXI_HPC0_FPD]  [get_bd_intf_pins axi_interconnect_hpc0/M00_AXI]
  connect_bd_intf_net -intf_net AXI_TC0_CSR -boundary_type upper [get_bd_intf_pins axi_interconnect_csr/M00_AXI] [get_bd_intf_pins AXI_TRAFFIC_CHECKER_0/C]
  connect_bd_intf_net -intf_net AXI_TC0_M   -boundary_type upper [get_bd_intf_pins axi_interconnect_hpc0/S00_AXI] [get_bd_intf_pins AXI_TRAFFIC_CHECKER_0/M]
  #
  # Assign Address Map
  #
  create_bd_addr_seg -range 0x1000      -offset  0xA0000000 [get_bd_addr_spaces zynq_ultra_ps_e_0/Data] [get_bd_addr_segs AXI_TRAFFIC_CHECKER_0/C/reg0] SEG_axi_traffic_checker_0_Reg
  create_bd_addr_seg -range  0x80000000 -offset  0x00000000 [get_bd_addr_spaces AXI_TRAFFIC_CHECKER_0/M] [get_bd_addr_segs zynq_ultra_ps_e_0/SAXIGP0/HPC0_DDR_LOW ] SEG_zynq_ultra_ps_e_0_HPC0_DDR_LOW
  create_bd_addr_seg -range 0x100000000 -offset 0x800000000 [get_bd_addr_spaces AXI_TRAFFIC_CHECKER_0/M] [get_bd_addr_segs zynq_ultra_ps_e_0/SAXIGP0/HPC0_DDR_HIGH] SEG_zynq_ultra_ps_e_0_HPC0_DDR_HIGH
  
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


common::send_msg_id "BD_TCL-1000" "WARNING" "This Tcl script was generated from a block design that has not been validated. It is possible that design <$design_name> may result in errors during validation."

