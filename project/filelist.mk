########################################################################
# LIBRARY SOURCES
########################################################################

# Set mylib folder path.
MYLIB_PATH=$(SOURCELIB_ROOT)/../repo/mylib

# Set folder path with header files to include.
CFLAGS += -I$(MYLIB_PATH)

# List all c file locations that must be included
LIBSRCS += $(MYLIB_PATH)/s4743527_lta1000g.c $(MYLIB_PATH)/s4743527_console.c \
		$(MYLIB_PATH)/s4743527_hamming.c $(MYLIB_PATH)/s4743527_mfs_led.c \
		$(MYLIB_PATH)/s4743527_rgb.c s4743527_rcmcont.c \
		$(MYLIB_PATH)/s4743527_txradio.c $(MYLIB_PATH)/s4743527_board_pb.c \
		s4743527_rcmdisplay.c $(MYLIB_PATH)/s4743527_mfs_ssd.c \
		$(FREERTOS_PATH)/portable/MemMang/heap_2.c
