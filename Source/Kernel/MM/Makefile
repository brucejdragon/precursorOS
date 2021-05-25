
include ../../Build/Makefile.include
include ../Build/Makefile-kernel.include

# Assign some variables that will be common across all architectures.
MM_sources		= ConcatPmmRegionList.c \
				  PhysicalMemoryManager.c \
				  PmmBitmapAllocator.c \
				  PmmRegion.c \
				  PmmWatermarkAllocator.c

MM_includedirs	= ../../../Include
MM_targetdir	= ../../../Lib
MM_target		= libMM.a

# Assign the configurations to each "project" according to architecture...
MM_x86_uni_configs		= $(kernel_x86_uni_configs)
MM_x86_uni_sources		= $(MM_sources)

MM_x86_uni_includedirs	= $(MM_includedirs) \
								../../../Include/Kernel/Architecture/x86 \
								../../../Include/Kernel/Architecture/x86_uni
MM_x86_uni_targetdir	= $(MM_targetdir)
MM_x86_uni_target		= $(MM_target)

# Make sure the generated rules can find all the sources.
VPATH = ../Architecture/x86/MM

$(eval $(call createStandardPrologue,MM_x86_uni))
$(eval $(call createStandardLibRules,MM_x86_uni))

