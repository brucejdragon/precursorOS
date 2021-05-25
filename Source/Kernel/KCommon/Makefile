
include ../../Build/Makefile.include
include ../Build/Makefile-kernel.include

# Assign some variables that will be common across all architectures.
KCommon_sources		= KString.c
KCommon_includedirs	= ../../../Include
KCommon_targetdir	= ../../../Lib
KCommon_target		= libKCommon.a

# Assign the configurations to each "project" according to architecture...
KCommon_x86_uni_configs		= $(kernel_x86_uni_configs)
KCommon_x86_uni_sources		= $(KCommon_sources) \
							  KMem_x86.s \
							  KDebug_x86.s

KCommon_x86_uni_includedirs	= $(KCommon_includedirs) \
								../../../Include/Kernel/Architecture/x86 \
								../../../Include/Kernel/Architecture/x86_uni
KCommon_x86_uni_targetdir	= $(KCommon_targetdir)
KCommon_x86_uni_target		= $(KCommon_target)

# Make sure the generated rules can find all the sources.
VPATH = ../Architecture/x86/KCommon

$(eval $(call createStandardPrologue,KCommon_x86_uni))
$(eval $(call createStandardLibRules,KCommon_x86_uni))

