#
# Rules for building library
#

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------
ROOT_PATH := ../../../..

include $(ROOT_PATH)/gcc.mk

# ----------------------------------------------------------------------------
# library and objects
# ----------------------------------------------------------------------------
LIBS := libfreertos.a

ifneq ($(__CONFIG_ROM_FREERTOS), y)
DIRS := ./Source

ifeq ($(__CONFIG_CPU_CM4F), y)
  DIRS += ./Source/portable/GCC/ARM_CM4F
else
  DIRS += ./Source/portable/GCC/ARM_CM3
endif

SRCS := $(sort $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS]))))

SRCS += ./Source/portable/MemMang/heap_adapt
else
ifeq ($(__CONFIG_OS_FREERTOS_USE_IDLE_HOOK), y)
SRCS := $(sort $(basename Patch/tasks.c))
endif
endif

OBJS := $(addsuffix .o,$(SRCS))

# library make rules
include $(LIB_MAKE_RULES)
