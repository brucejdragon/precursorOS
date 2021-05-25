
include ../../Build/Makefile.include
include ../Build/Makefile-kernel.include

# Assign some variables that will be common across all architectures.
KRunTime_sources		= DisplayTextStream.c \
						  KOut.c \
						  KShutdown.c \
						  TextWriter.c

KRunTime_includedirs	= ../../../Include
KRunTime_targetdir		= ../../../Lib
KRunTime_target			= libKRunTime.a

# Assign the configurations to each "project" according to architecture...
KRunTime_x86_uni_configs		= $(kernel_x86_uni_configs)
KRunTime_x86_uni_sources		= $(KRunTime_sources)
KRunTime_x86_uni_includedirs	= $(KRunTime_includedirs) \
									../../../Include/Kernel/Architecture/x86 \
									../../../Include/Kernel/Architecture/x86_uni
KRunTime_x86_uni_targetdir		= $(KRunTime_targetdir)
KRunTime_x86_uni_target			= $(KRunTime_target)

# Make sure the generated rules can find all the sources.
VPATH = ../Architecture/x86/KRunTime

$(eval $(call createStandardPrologue,KRunTime_x86_uni))
$(eval $(call createStandardLibRules,KRunTime_x86_uni))

