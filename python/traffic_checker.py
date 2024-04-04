from uio import Uio
import sys
import os

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
        self.version_build = (version_hi >> 16) & 0xFF
        self.data_width    = 2**((version_hi >>  0) & 0x0F)
        self.mw_xfer_size  = 2**((mw_param   >>  0) & 0x0F)
        self.mr_xfer_size  = 2**((mr_param   >>  0) & 0x0F)

    def print_info(self, tag="Traffic_Checker", file=sys.stdout):
        print ("{0} Version                : {1}.{2}.{3}".format(tag, self.version_major, self.version_minor, self.version_build),file=file)
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
        
