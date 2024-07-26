#location of freertos source files
FREERTOS_PATH=$(SOURCELIB_ROOT)/components/os/FreeRTOS

#Name of project
PROJ_NAME=main

# Your local source files
SRCS = $(PROJ_NAME).c
SRCS += $(SOURCELIB_ROOT)/../repo/mylib/myconfig.h
CFLAGS += -DENABLE_DEBUG_UART -DMYCONFIG -DFreeRTOS
###################################################
ROOT=$(shell pwd)

OBJS=$(SRCS:.c=.o)

###################################################

.PHONY: $(shell mkdir -p obj) lib proj all depend
all: proj

#Include freertos makefile
include $(FREERTOS_PATH)/filelist.mk

#include nrf24l01+ makefile
include $(SOURCELIB_ROOT)/components/peripherals/nrf24l01plus/nucleo-f429/filelist.mk

#include nucleo-f429 makefile
include $(SOURCELIB_ROOT)/../repo/pf/filelist.mk
include $(SOURCELIB_ROOT)/components/boards/nucleo-f429zi/board.mk
include $(SOURCELIB_ROOT)/components/mk/m_launch.mk
include $(SOURCELIB_ROOT)/components/mk/m_common.mk

lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

include .depend
