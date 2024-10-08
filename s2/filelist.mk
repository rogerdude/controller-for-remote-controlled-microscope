########################################################################
# LIBRARY SOURCES
########################################################################

# Set mylib folder path.
MYLIB_PATH=$(SOURCELIB_ROOT)/../repo/mylib

# Set folder path with header files to include.
CFLAGS += -I$(MYLIB_PATH)

# List all c file locations that must be included
LIBSRCS += $(MYLIB_PATH)/s4743527_rgb.c $(MYLIB_PATH)/s4743527_mfs_trimpot.c \
		$(MYLIB_PATH)/s4743527_lta1000g.c $(MYLIB_PATH)/s4743527_mfs_pb.c
