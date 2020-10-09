MODULE := engines/scooby

MODULE_OBJS := \
    detection.o \
    file.o \
    gfx.o \
    md/vdp.o \
    scooby.o

# This module can be built as a plugin
ifeq ($(ENABLE_SCOOBY), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
