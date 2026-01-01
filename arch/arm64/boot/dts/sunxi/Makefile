#dtb-$(CONFIG_ARCH_SUNXI) += board.dtb
#dtb-$(CONFIG_ARCH_SUNXI) += sun50iw9-${BOARDDTS}.dtb
dtb-y += board.dtb
dtb-y += sun50iw9-kickpi-k2b.dtb

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files	:= *.dtb
DTC_FLAGS_board += -@

subdir-y       := $(dts-dirs) overlay
