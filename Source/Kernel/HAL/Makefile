
include ../../Build/Makefile.include
include ../Build/Makefile-kernel.include

# Assign some variables that will be common across all architectures.
HAL_includedirs	= ../../../Include
HAL_targetdir	= ../../../Lib
HAL_target		= libHAL.a

# Assign the configurations to each "project" according to architecture...
HAL_x86_uni_configs		= $(kernel_x86_uni_configs)
HAL_x86_uni_sources		= Atomic_x86.c \
						  Atomic_x86_asm.s \
						  IO.s \
						  InterruptController_x86_8259A.c \
						  KernelDisplay_x86_Vga.c \
						  LockImpl_x86_uni.c \
						  LockImpl_x86_uni_asm.s \
						  Processor_x86_uni.c \
						  Processor_x86_uni_asm.s \
						  ShutdownHardware_x86_uni.c \
						  TrapFrame_x86.c

HAL_x86_uni_includedirs	= $(HAL_includedirs) \
							../../../Include/Kernel/Architecture/x86 \
							../../../Include/Kernel/Architecture/x86_uni
HAL_x86_uni_targetdir	= $(HAL_targetdir)
HAL_x86_uni_target		= $(HAL_target)

# Make sure the generated rules can find all the sources.
VPATH = ../Architecture/x86/HAL

$(eval $(call createStandardPrologue,HAL_x86_uni))
$(eval $(call createStandardLibRules,HAL_x86_uni))

