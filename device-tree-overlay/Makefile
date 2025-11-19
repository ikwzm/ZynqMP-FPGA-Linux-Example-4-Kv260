
DT_BASE := /sys/firmware/devicetree/base

ifneq      ($(wildcard $(DT_BASE)/fpga-region),)
    FPGA_REGION := /fpga-region
else ifneq ($(wildcard $(DT_BASE)/fpga-full),)
    FPGA_REGION := /fpga-full
else
    $(error Neigher fpga-region nor fpga-full exists under $(DT_BASE))
endif

DTS_FILES := traffic_checker_acp_128_uio_100MHz.dts  \
             traffic_checker_acp_128_uio_250MHz.dts  \
             traffic_checker_acp_128_v4l2_100MHz.dts \
             traffic_checker_acp_128_v4l2_250MHz.dts \
             traffic_checker_hp_128_uio_100MHz.dts   \
             traffic_checker_hp_128_uio_250MHz.dts   \
             traffic_checker_hp_128_v4l2_100MHz.dts  \
             traffic_checker_hp_128_v4l2_250MHz.dts  \
             traffic_checker_hpc_128_uio_100MHz.dts  \
             traffic_checker_hpc_128_uio_250MHz.dts  \
             traffic_checker_hpc_128_v4l2_100MHz.dts \
             traffic_checker_hpc_128_v4l2_250MHz.dts \
             $(ENDLIST)

all: $(DTS_FILES)

clean:
	-rm $(DTS_FILES)

traffic_checker_acp_128_uio_100MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_ACP_128 -DDRV_TYPE_UIO  -DFCLK_RATE_100MHz -O dts -o $@ $<

traffic_checker_acp_128_uio_250MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_ACP_128 -DDRV_TYPE_UIO  -DFCLK_RATE_250MHz -O dts -o $@ $<

traffic_checker_hpc_128_uio_100MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HPC_128 -DDRV_TYPE_UIO  -DFCLK_RATE_100MHz -O dts -o $@ $<

traffic_checker_hpc_128_uio_250MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HPC_128 -DDRV_TYPE_UIO  -DFCLK_RATE_250MHz -O dts -o $@ $<

traffic_checker_hp_128_uio_100MHz.dts  : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HP_128  -DDRV_TYPE_UIO  -DFCLK_RATE_100MHz -O dts -o $@ $<

traffic_checker_hp_128_uio_250MHz.dts  : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HP_128  -DDRV_TYPE_UIO  -DFCLK_RATE_250MHz -O dts -o $@ $<

traffic_checker_acp_128_v4l2_100MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_ACP_128 -DDRV_TYPE_V4L2  -DFCLK_RATE_100MHz -O dts -o $@ $<

traffic_checker_acp_128_v4l2_250MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_ACP_128 -DDRV_TYPE_V4L2  -DFCLK_RATE_250MHz -O dts -o $@ $<

traffic_checker_hpc_128_v4l2_100MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HPC_128 -DDRV_TYPE_V4L2  -DFCLK_RATE_100MHz -O dts -o $@ $<

traffic_checker_hpc_128_v4l2_250MHz.dts : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HPC_128 -DDRV_TYPE_V4L2 -DFCLK_RATE_250MHz -O dts -o $@ $<

traffic_checker_hp_128_v4l2_100MHz.dts  : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HP_128  -DDRV_TYPE_V4L2 -DFCLK_RATE_100MHz -O dts -o $@ $<

traffic_checker_hp_128_v4l2_250MHz.dts  : traffic_checker_template.dts
	dtbo-compile -P -DFPGA_REGION=\\\"$(FPGA_REGION)\\\" -DS_AXI_TYPE_HP_128  -DDRV_TYPE_V4L2 -DFCLK_RATE_250MHz -O dts -o $@ $<
