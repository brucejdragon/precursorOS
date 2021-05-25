##############################################################################
#
#              Copyright (C) 2004-2005 Bruce Johnston
#
##############################################################################
#
#   //osdev/precursor/Source/Kernel/Makefile
#
##############################################################################
#
#	Originating Author:	BruceJ
#	Originating Date:	2004/Sep/18
#
##############################################################################
#
# This is the main Makefile for the Precursor Microkernel. Its default rule
# builds the kernel image as well as a bootable floppy image containing GRUB
# and the kernel. In addition, it defines phony rules that can copy various
# configurations of the image to a real floppy disk. Use these with care!
#
##############################################################################


include ../Build/Makefile.include
include ./Build/Makefile-kernel.include

# Assign some variables that will be common across all architectures.
kernel_includedirs	= # No includes.
kernel_targetdir	= ../../Bin
kernel_target		= precurnl
kernel_libdir		= ../../Lib

# Order matters here!
kernel_libs			= libExecutive.a \
					  libKRunTime.a \
					  libMM.a \
					  libHAL.a \
					  libKCommon.a

kernel_subdirs		= $(kernel_libs:lib%.a=%)

# No need to define configs -- it is already defined in the kernel include file.
kernel_x86_uni_sources		= Boot_x86_uni.s

kernel_x86_uni_includedirs	= $(kernel_includedirs)
kernel_x86_uni_targetdir	= $(kernel_targetdir)
kernel_x86_uni_target		= $(kernel_target)
kernel_x86_uni_libdir		= $(kernel_libdir)
kernel_x86_uni_libs			= $(kernel_libs)
kernel_x86_uni_subdirs		= $(kernel_subdirs)

# Make sure the generated rules can find all the sources.
VPATH = ./Architecture/x86/Boot

# Define some additional variables.
kernel_x86_uni_imagename	= $(kernel_x86_uni_target).img
kernel_x86_uni_linkerscript	= ./Build/$(kernel_x86_uni_target)_x86_uni.ld
kernel_x86_uni_deploydir	= ../../Deploy

kernel_x86_uni_extras =		$(foreach config,$(kernel_x86_uni_configs),$(kernel_x86_uni_targetdir)/$(config)/$(kernel_x86_uni_imagename) )
kernel_x86_uni_extras +=	$(foreach config,$(kernel_x86_uni_configs),$(kernel_x86_uni_targetdir)/$(config)/menu.cfg )
kernel_x86_uni_extras +=	$(foreach config,$(kernel_x86_uni_configs),$(kernel_x86_uni_targetdir)/$(config)/testmodule.txt )

# Use the linker script when linking.
checked_x86_uni_CFLAGS	+= -Xlinker -T -Xlinker $(kernel_x86_uni_linkerscript)
free_x86_uni_CFLAGS		+= -Xlinker -T -Xlinker $(kernel_x86_uni_linkerscript)


$(eval $(call createStandardPrologue,kernel_x86_uni))
$(eval $(call createStandardExeRules,kernel_x86_uni))


##################################################
# Custom rules for this makefile. -- x86-specific.
##################################################


# Creates rules that copy a particular configuration of the x86 kernel to a boot floppy image,
# and rules to copy that floppy image to a floppy disk. One of the rules prepares a boot menu
# config file for the floppy image. Note that the "empty" boot floppy image must be named
# bootfloppy.img and the menu config file must be named menu.cfg.
# Parameters:
#	$(1):	Parent directory of the sub-directory that contains the kernel image to copy.
#	$(2):	Configuration name. This is also the name of the sub-directory at which to find the
#			kernel image.
#	$(3):	Name of the kernel image to copy.
#	$(4):	Directory containing the "empty" floppy image and boot menu config file.
#	$(5):	Name of the floppy image to create and copy to a disk.
define createx86TestImageRules
$(1)/$(2)/$(5):	$(1)/$(2)/$(3) $(4)/bootfloppy.img $(1)/$(2)/menu.cfg $(1)/$(2)/testmodule.txt
				cp $(4)/bootfloppy.img $$@
				chmod +w $$@
				mcopy -o -i $$@ $$< ::/boot
				mcopy -o -i $$@ $(1)/$(2)/menu.cfg ::/boot
				mcopy -o -i $$@ $(1)/$(2)/testmodule.txt ::/boot
				@echo -----------------------------------------------------------------------
				@echo Precursor Microkernel $(2) build completed successfully!
				@echo -----------------------------------------------------------------------

$(1)/$(2)/menu.cfg:	$(4)/menu.cfg
					sed s/%target%/$(3)/ $$< > $$@

$(1)/$(2)/testmodule.txt:	$(4)/testmodule.txt
							cp $$< $$@

.PHONY:		floppy_$(2)

floppy_$(2):	$(1)/$(2)/$(5)
				dd if=$(1)/$(2)/$(5) of=/dev/fd0

endef


$(foreach config,$(kernel_x86_uni_configs),$(eval $(call createx86TestImageRules,$(kernel_x86_uni_targetdir),$(config),$(kernel_x86_uni_target),$(kernel_x86_uni_deploydir),$(kernel_x86_uni_imagename))))


