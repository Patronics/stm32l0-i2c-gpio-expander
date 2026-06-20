PROJECT = i2c-gpio
BUILD_DIR = bin

CFILES = main.c

#optional includes, currently unused
#SHARED_DIR = ../includes
#CFILES += api.c
#AFILES += api-asm.S

# TODO - you will need to edit these two lines!
DEVICE = stm32l011f3
OOCD_INTERFACE = CMSIS-DAP
OOCD_TARGET = stm32l0
#currently unused, as standard configuration files are sufficient:
#OOCD_FILE = ../openocd-stm32l0.cfg




# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk

#valid commands:
#make
#make clean
