#Name of project
PROJ_NAME=main

#Your local source files
SRCS = $(PROJ_NAME).c
CFLAGS += -DENABLE_DEBUG_UART # enable Debug Com Port
###################################################
ROOT=$(shell pwd)

OBJS=$(SRCS:.c=.o)

###################################################

.PHONY: $(shell mkdir -p obj) lib proj all depend
all: proj

#include nucelo-f429zi makefile - NOTE MUST BE LAST TO INCLUDE
include $(SOURCELIB_ROOT)/../repo/s1/filelist.mk
include $(SOURCELIB_ROOT)/components/boards/nucleo-f429zi/board.mk
include $(SOURCELIB_ROOT)/components/mk/m_launch.mk
include $(SOURCELIB_ROOT)/components/mk/m_common.mk

lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

include .depend
