PROJECT = cdh_tab
BUILD_DIR = build

PQ_CDH_DIR = ./pq-cdh
CFILES = flight_cdh_blr.c
CFILES += mcu_tab.c tab.c

# Edit these two lines as needed
DEVICE=stm32l496rgt3
OOCD_FILE = board/stm32l4-generic.cfg

# All lines below probably should not be edited
VPATH += $(PQ_CDH_DIR)
INCLUDES += $(patsubst %,-I%, . $(PQ_CDH_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
