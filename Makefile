##############################################################################
#
#              Copyright (C) 2004-2005 Bruce Johnston
#
##############################################################################
#
#   //osdev/precursor/Makefile
#
##############################################################################
#
#	Originating Author:	BruceJ
#	Originating Date:	2004/Sep/28
#
##############################################################################
#
# This is the main Makefile for the Precursor OS. It define a phony rule "all"
# to build the kernel image, and in the case of the x86 kernel, a GRUB-bootable
# floppy image as well. In addition, it defines a phony rule called "docs" that
# will use Doxygen to automatically generate HTML documentation from all the
# source code.
#
##############################################################################


#############################################################################
# Variables specific to this makefile.
#############################################################################
docsdir			= ./Docs
kerneldir		= ./Source/Kernel
bindir			= ./Bin
imagename		= precurnl
doxylog			= $(docsdir)/doxygen.log	# Stand-in target for all doxygen output.


# Bring in the list of kernel configurations.
include $(kerneldir)/Build/Makefile-kernel.include

kernel_binaries		= $(foreach config,$(kernel_configs),$(bindir)/$(config)/$(imagename) )
kernel_clean_rules	= $(foreach config,$(kernel_configs),clean_$(config) )


#################################
# Custom rules for this makefile.
#################################

.PHONY:				all docs clean $(kerneldir) $(kernel_configs) $(kernel_clean_rules)

all:				$(kerneldir) docs

$(kerneldir):
					$(MAKE) all -C $(kerneldir)

$(kernel_configs):
					$(MAKE) $@ -C $(kerneldir)

docs:				$(doxylog)


# If the kernel image changed, then the source changed, so the docs should change.
$(doxylog):			$(docsdir)/Doxyfile $(kernel_binaries)
					doxygen $< > $@ 2>&1

clean:
					rm -f -r $(docsdir)/html $(doxylog)
					$(MAKE) clean -C $(kerneldir)

$(kernel_clean_rules):
					$(MAKE) $@ -C $(kerneldir)

# That's it!

