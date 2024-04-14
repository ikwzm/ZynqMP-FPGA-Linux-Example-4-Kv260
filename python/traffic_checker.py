from uio import Uio
import sys
import os
import math

class Traffic_Checker:

    VERSION_REGS_ADDR   = 0x0000
    MR_PARAM_REGS_ADDR  = 0x0008
    MW_PARAM_REGS_ADDR  = 0x000C
    MR_ADDR_REGS_ADDR   = 0x0010
    MR_SIZE_REGS_ADDR   = 0x0018
    MR_MODE_REGS_ADDR   = 0x001C
    MR_STAT_REGS_ADDR   = 0x001E
    MR_CTRL_REGS_ADDR   = 0x001F

    MW_ADDR_REGS_ADDR   = 0x0020
    MW_SIZE_REGS_ADDR   = 0x0028
    MW_MODE_REGS_ADDR   = 0x002C
    MW_STAT_REGS_ADDR   = 0x002E
    MW_CTRL_REGS_ADDR   = 0x002F

    MR_MON_CTRL_ADDR    = 0x0080
    MR_MON_COUNT_ADDR   = 0x0088
    MR_MON_ADDR_ADDR    = 0x0090
    MR_MON_AVAL_ADDR    = 0x0098
    MR_MON_ARDY_ADDR    = 0x00A0
    MR_MON_DATA_ADDR    = 0x00A8
    MR_MON_DVAL_ADDR    = 0x00B0
    MR_MON_DRDY_ADDR    = 0x00B8

    MW_MON_CTRL_ADDR    = 0x00C0
    MW_MON_COUNT_ADDR   = 0x00C8
    MW_MON_ADDR_ADDR    = 0x00D0
    MW_MON_AVAL_ADDR    = 0x00D8
    MW_MON_ARDY_ADDR    = 0x00E0
    MW_MON_DATA_ADDR    = 0x00E8
    MW_MON_DVAL_ADDR    = 0x00F0
    MW_MON_DRDY_ADDR    = 0x00F8

    MR_MODE_IRQ_ENABLE  = (0x1 <<  0)
    MR_MODE_AXI_CACHE   = (0xF <<  4)
    MR_MODE_AXI_PROT    = (0x2 <<  8)
    MR_MODE_SPECULATIVE = (0x1 << 14)

    MW_MODE_IRQ_ENABLE  = (0x1 <<  0)
    MW_MODE_AXI_CACHE   = (0xF <<  4)
    MW_MODE_AXI_PROT    = (0x2 <<  8)
    MW_MODE_SPECULATIVE = (0x1 << 14)

    CTRL_RESET          = 0x80
    CTRL_PAUSE          = 0x40
    CTRL_STOP           = 0x20
    CTRL_START          = 0x10
    CTRL_IRQ_ENABLE     = 0x04
    CTRL_FIRST          = 0x02
    CTRL_LAST           = 0x01

    FCLK0_RATE_FILE     = "/sys/class/fclkcfg/amba_pl@0:fclk0/rate"
    FCLK0_RATE_DEFAULT  = 100000000

    def __init__(self, regs, coherent):
        self.regs     = regs
        self.coherent = coherent
        self.mr_mode  = Traffic_Checker.MR_MODE_SPECULATIVE | Traffic_Checker.MR_MODE_AXI_CACHE | Traffic_Checker.MR_MODE_AXI_PROT
        self.mw_mode  = Traffic_Checker.MW_MODE_SPECULATIVE | Traffic_Checker.MW_MODE_AXI_CACHE | Traffic_Checker.MW_MODE_AXI_PROT
        self.read_info()
        self.read_frequency()
    
    def read_info(self):
        version_hi = self.regs.read_word(Traffic_Checker.VERSION_REGS_ADDR+4)
        mr_param   = self.regs.read_word(Traffic_Checker.MR_PARAM_REGS_ADDR )
        mw_param   = self.regs.read_word(Traffic_Checker.MW_PARAM_REGS_ADDR )
        self.version_major = (version_hi >> 28) & 0x0F
        self.version_minor = (version_hi >> 24) & 0x0F
        self.version_build = (version_hi >> 18) & 0x3F
        self.axi_interface = (version_hi >> 16) & 0x03
        self.data_width    = 2**((version_hi >>  0) & 0x0F)
        self.mw_xfer_size  = 2**((mw_param   >>  0) & 0x0F)
        self.mr_xfer_size  = 2**((mr_param   >>  0) & 0x0F)

    def axi_interface_to_string(self):
        if   self.axi_interface == 1:
            return "HP"
        elif self.axi_interface == 2:
            return "HPC"
        elif self.axi_interface == 3:
            return "ACP"
        else:
            return "Unknown"

    def print_info(self, tag="Traffic_Checker", file=sys.stdout):
        print ("{0} Version                : {1}.{2}.{3}".format(tag, self.version_major, self.version_minor, self.version_build),file=file)
        print ("{0} Frequency              : {1} MHz"    .format(tag, math.ceil(self.frequency/(1000*1000)),file=file))
        print ("{0} AXI Interface Type     : {1}"        .format(tag, self.axi_interface_to_string()),file=file)
        print ("{0} AXI Data Width         : {1} Bit"    .format(tag, self.data_width), file=file)
        print ("{0} Write Transaction Size : {1} Byte"   .format(tag, self.mw_xfer_size), file=file)
        print ("{0} Read  Transaction Size : {1} Byte"   .format(tag, self.mr_xfer_size), file=file)
        print ("{0} Cache Coherent         : {1}"        .format(tag, self.coherent), file=file)
        
    def read_frequency(self):
        if (os.path.exists(Traffic_Checker.FCLK0_RATE_FILE)):
            file = open(Traffic_Checker.FCLK0_RATE_FILE)
            self.frequency = int(file.read())
            file.close()
        else:
            self.frequency = Traffic_Checker.FCLK0_RATE_DEFAULT

    def mw_setup(self, addr, size):
        self.regs.write_word(Traffic_Checker.MW_ADDR_REGS_ADDR+0, (addr  >>  0) & 0xFFFFFFFF)
        self.regs.write_word(Traffic_Checker.MW_ADDR_REGS_ADDR+4, (addr  >> 32) & 0xFFFFFFFF)
        self.regs.write_word(Traffic_Checker.MW_SIZE_REGS_ADDR+0, (size  >>  0) & 0xFFFFFFFF)
        mode = self.mw_mode | Traffic_Checker.MW_MODE_IRQ_ENABLE
        self.regs.write_word(Traffic_Checker.MW_MODE_REGS_ADDR  , mode)

    def mw_start(self):
        ctrl = Traffic_Checker.CTRL_START | Traffic_Checker.CTRL_FIRST | Traffic_Checker.CTRL_LAST | Traffic_Checker.CTRL_IRQ_ENABLE
        self.regs.write_byte(Traffic_Checker.MW_CTRL_REGS_ADDR, ctrl)

    def mw_clear_status(self):
        self.regs.write_byte(Traffic_Checker.MW_STAT_REGS_ADDR, 0x00)
        
    def mw_reset(self):
        self.regs.write_byte(Traffic_Checker.MW_CTRL_REGS_ADDR, Traffic_Checker.CTRL_RESET)
        self.regs.write_byte(Traffic_Checker.MW_CTRL_REGS_ADDR, 0)
        
    def mr_setup(self, addr, size):
        self.regs.write_word(Traffic_Checker.MR_ADDR_REGS_ADDR+0, (addr  >>  0) & 0xFFFFFFFF)
        self.regs.write_word(Traffic_Checker.MR_ADDR_REGS_ADDR+4, (addr  >> 32) & 0xFFFFFFFF)
        self.regs.write_word(Traffic_Checker.MR_SIZE_REGS_ADDR+0, (size  >>  0) & 0xFFFFFFFF)
        mode = self.mr_mode | Traffic_Checker.MR_MODE_IRQ_ENABLE
        self.regs.write_word(Traffic_Checker.MR_MODE_REGS_ADDR  , mode)

    def mr_start(self):
        ctrl = Traffic_Checker.CTRL_START | Traffic_Checker.CTRL_FIRST | Traffic_Checker.CTRL_LAST | Traffic_Checker.CTRL_IRQ_ENABLE
        self.regs.write_byte(Traffic_Checker.MR_CTRL_REGS_ADDR, ctrl)

    def mr_clear_status(self):
        self.regs.write_byte(Traffic_Checker.MR_STAT_REGS_ADDR, 0x00)
        
    def mr_reset(self):
        self.regs.write_byte(Traffic_Checker.MR_CTRL_REGS_ADDR, Traffic_Checker.CTRL_RESET)
        self.regs.write_byte(Traffic_Checker.MR_CTRL_REGS_ADDR, 0)
        
    def mw_monitor_clear(self):
        self.regs.write_byte(Traffic_Checker.MW_MON_CTRL_ADDR+7, 0x80)

    def mw_read_monitor(self):
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_COUNT_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_COUNT_ADDR+4)
        self.mw_monitor_count = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_ADDR_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_ADDR_ADDR+4)
        self.mw_monitor_addr  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_AVAL_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_AVAL_ADDR+4)
        self.mw_monitor_aval  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_ARDY_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_ARDY_ADDR+4)
        self.mw_monitor_ardy  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_DATA_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_DATA_ADDR+4)
        self.mw_monitor_data  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_DVAL_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_DVAL_ADDR+4)
        self.mw_monitor_dval  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MW_MON_DRDY_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MW_MON_DRDY_ADDR+4)
        self.mw_monitor_drdy  = (val_hi << 32) | (val_lo)

    def mw_print_monitor(self, tag="Traffic_Checker", file=sys.stdout):
        print ("{0} MW Total Count         : {1}"    .format(tag, self.mw_monitor_count))
        print ("{0} MW Address Xfer Count  : {1}"    .format(tag, self.mw_monitor_addr))
        print ("{0} MW Address Valid Count : {1}"    .format(tag, self.mw_monitor_aval))
        print ("{0} MW Address Ready Count : {1}"    .format(tag, self.mw_monitor_ardy))
        print ("{0} MW Data Xfer Count     : {1}"    .format(tag, self.mw_monitor_data))
        print ("{0} MW Data Valid Count    : {1}"    .format(tag, self.mw_monitor_dval))
        print ("{0} MW Data Ready Count    : {1}"    .format(tag, self.mw_monitor_drdy))
        
    def mr_monitor_clear(self):
        self.regs.write_byte(Traffic_Checker.MR_MON_CTRL_ADDR+7, 0x80)

    def mr_read_monitor(self):
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_COUNT_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_COUNT_ADDR+4)
        self.mr_monitor_count = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_ADDR_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_ADDR_ADDR+4)
        self.mr_monitor_addr  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_AVAL_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_AVAL_ADDR+4)
        self.mr_monitor_aval  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_ARDY_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_ARDY_ADDR+4)
        self.mr_monitor_ardy  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_DATA_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_DATA_ADDR+4)
        self.mr_monitor_data  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_DVAL_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_DVAL_ADDR+4)
        self.mr_monitor_dval  = (val_hi << 32) | (val_lo)
        val_lo = self.regs.read_word(Traffic_Checker.MR_MON_DRDY_ADDR)
        val_hi = self.regs.read_word(Traffic_Checker.MR_MON_DRDY_ADDR+4)
        self.mr_monitor_drdy  = (val_hi << 32) | (val_lo)

    def mr_print_monitor(self, tag="Traffic_Checker", file=sys.stdout):
        print ("{0} MR Total Count         : {1}"    .format(tag, self.mr_monitor_count))
        print ("{0} MR Address Xfer Count  : {1}"    .format(tag, self.mr_monitor_addr))
        print ("{0} MR Address Valid Count : {1}"    .format(tag, self.mr_monitor_aval))
        print ("{0} MR Address Ready Count : {1}"    .format(tag, self.mr_monitor_ardy))
        print ("{0} MR Data Xfer Count     : {1}"    .format(tag, self.mr_monitor_data))
        print ("{0} MR Data Valid Count    : {1}"    .format(tag, self.mr_monitor_dval))
        print ("{0} MR Data Ready Count    : {1}"    .format(tag, self.mr_monitor_drdy))
        
