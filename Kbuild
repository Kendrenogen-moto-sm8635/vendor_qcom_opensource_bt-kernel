ifeq ($(CONFIG_MSM_BT_POWER),m)
KBUILD_CPPFLAGS += -DCONFIG_MSM_BT_POWER
endif

ifeq ($(CONFIG_BTFM_SLIM),m)
KBUILD_CPPFLAGS += -DCONFIG_BTFM_SLIM
endif

ifeq ($(CONFIG_I2C_RTC6226_QCA),m)
KBUILD_CPPFLAGS += -DCONFIG_I2C_RTC6226_QCA
endif

ifeq ($(CONFIG_SLIM_BTFM_CODEC), m)
KBUILD_CPPFLAGS += -DCONFIG_SLIM_BTFM_CODEC
endif

ifeq ($(CONFIG_BT_HW_SECURE_DISABLE), y)
KBUILD_CPPFLAGS += -DCONFIG_BT_HW_SECURE_DISABLE
endif

ifeq ($(CONFIG_BTFM_SWR),m)
KBUILD_CPPFLAGS += -DCONFIG_BTFM_SWR
endif

obj-$(CONFIG_MSM_BT_POWER) += pwr/
obj-$(CONFIG_BTFM_SLIM) += slimbus/
obj-$(CONFIG_I2C_RTC6226_QCA) += rtc6226/
obj-$(CONFIG_BTFM_CODEC) += btfmcodec/
obj-$(CONFIG_SLIM_BTFM_CODEC) += slimbus/
obj-$(CONFIG_BTFM_SWR) += soundwire/
