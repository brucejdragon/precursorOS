
include ../../Build/Makefile.include
include ../Build/Makefile-kernel.include

# Assign some variables that will be common across all architectures.
Executive_sources		= ExceptionDispatcher.c main.c
Executive_includedirs	= ../../../Include ./
Executive_targetdir		= ../../../Lib
Executive_target		= libExecutive.a

# Assign the configurations to each "project" according to architecture...
Executive_x86_uni_configs		= $(kernel_x86_uni_configs)
Executive_x86_uni_sources		= $(Executive_sources) \
								  BootLoaderInfo_x86_Multiboot.c \
								  BootLoaderInfoTranslator_x86_Multiboot.c \
								  ExceptionDispatcher_x86.c \
								  InterruptDispatcher_x86_uni.c \
								  MBMemFieldsPmmRegionList.c \
								  MBMemmapPmmRegionList.c \
								  MBModulePmmRegionList.c \
								  Multiboot.c \
								  WritableTrapFrame_x86.c

Executive_x86_uni_includedirs	= $(Executive_includedirs) \
									../../../Include/Kernel/Architecture/x86 \
									../../../Include/Kernel/Architecture/x86_uni
Executive_x86_uni_targetdir		= $(Executive_targetdir)
Executive_x86_uni_target		= $(Executive_target)

# Make sure the generated rules can find all the sources.
VPATH = ../Architecture/x86/Executive

$(eval $(call createStandardPrologue,Executive_x86_uni))
$(eval $(call createStandardLibRules,Executive_x86_uni))

