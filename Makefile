
# bldev is the utility to build a development tree.
# This make file builds bldev without a development tree.
# 

help:
	@echo
	@echo This Makefile is for creating some of the build utilities,
	@echo which can be used to build a development tree
	@echo on the target machine.
	@echo 
	@echo Execute the following command on the target machine:
	@echo "    "make CONFIG=.../Makefile.cfg DEVTOP=.../defTop boot
	@echo CDs to DEVTOP
	@echo Creates a sub folder folder \"boot\" to work in.
	@echo .../Makefile.cfg = Path to the configuration file,
	@echo "                   "probably in config/PlatformName
	@echo 
	@echo The boot-related targets are
	@echo  boot --- Build the bldev utility, and move them into PLATBIN .
	@echo  dev -- Create the development tree.  Part of boot, too.
	@echo  help --- This message
	@echo  bootclean -- Remove the boot folder: DEVTOP/boot

PLATFORM_INC = $(realpath $(dir $(CONFIG))/Platform.inc)
MAKEFILE_INC = $(realpath $(dir $(CONFIG))/../Makefile.inc)
include $(CONFIG)
include $(PLATFORM_INC)

$(shell mkdir -p $(DEVTOP))

CONFIG1= $(realpath $(CONFIG))
DEVTOP1 = $(realpath $(DEVTOP))

ifeq ($(OS),Windows_NT)
  #If you have more than one bash, you can use a full path.
  SHELL=bash.exe
endif


SRCROOT=$(CURDIR)/dev/src/boot

#These will be constructed
MAKEFILE2 = $(DEVTOP1)/boot/Makefile_boot
bldevEXE = bldev$(EXE_SUFFIX)
setdevEXE = setdev$(EXE_SUFFIX)
setpaletteEXE = setpalette$(EXE_SUFFIX)

info:
ifeq ($(DEVTOP),)
	@echo The DEVTOP environment variable is not set
	exit -1
endif
	@echo CONFIG1=$(CONFIG1)
	@echo OS=$(OS)
	@echo DEVTOP = X$(DEVTOP)X
	@echo DEVTOP1=$(DEVTOP1)
	@echo SRCROOT=$(SRCROOT)
	@echo MKDIR=$(MKDIR)
	@echo MAKEFILE_INC=$(MAKEFILE_INC)
	@echo PLATFORM_INC=$(PLATFORM_INC)
	@echo LINKER=$(LINKER)
	@echo CURDIR = $(CURDIR)

boot0: info work 
	$(MV) $(DEVTOP1)/boot/$(bldevEXE) $(WQ)$(PLATBIN)$(WQ)
	$(MV) $(DEVTOP1)/boot/$(setdevEXE) $(WQ)$(PLATBIN)$(WQ)
ifeq ($(OS),Windows_NT)
	$(MV) $(DEVTOP1)/boot/$(setpaletteEXE) $(WQ)$(PLATBIN)$(WQ)
endif

boot: boot0 dev

FORCE:

dev: FORCE
	bldev --cfg $(CONFIG1) $(DEVTOP1) $(CURDIR)/dev
	@echo Now run: setdev -d $(WQ)$(DEVTOP)$(WQ) -f TAG --bg rrbbgg --fg rrbbgg 

BSRCS=bldev_Info.cpp bldev.cpp BootUtil.cpp BootParse.cpp
BOBJS=$(BSRCS:.cpp=$(O_SUFFIX))

SSRCS= SetDevMain.cpp bldev_Info.cpp BootUtil.cpp BootParse.cpp
SOBJS=$(SSRCS:.cpp=$(O_SUFFIX))

PSRCS= setpalette.cpp
POBJS=$(PSRCS:.cpp=$(O_SUFFIX))


work:
	@-sh -c "mkdir -p $(DEVTOP1)/boot"
	echo MAKEFILE_INC := $(MAKEFILE_INC)  > $(MAKEFILE2)2
	echo PLATFORM_INC := $(PLATFORM_INC)  >> $(MAKEFILE2)2
	echo MAKEFILE_CFG = $(realpath $(CONFIG)) >> $(MAKEFILE2)2
	$(CP) Makefile_boot $(MAKEFILE2)
	echo include  $(MAKEFILE2) >> $(MAKEFILE2)2
	echo vpath %.cpp $(SRCROOT)/src >> $(MAKEFILE2)
	echo vpath %.h $(SRCROOT)/src >> $(MAKEFILE2)
	echo vpath %.cpp $(SRCROOT)/bldev >> $(MAKEFILE2)
	echo vpath %.h $(SRCROOT)/bldev >> $(MAKEFILE2)
	echo vpath %.cpp $(SRCROOT)/setdev >> $(MAKEFILE2)
	echo CPPFLAGS=-I$(SRCROOT)/src -I$(SRCROOT)/bldev $(CLI11_CPPFLAGS) $(BOOST_CPPFLAGS) >> $(MAKEFILE2)
ifeq ($(OS),Windows_NT)
	echo vpath %.cpp $(SRCROOT)/setdev/Windows >> $(MAKEFILE2)
	echo CPPFLAGS += -I$(SRCROOT)/src/Windows  >> $(MAKEFILE2)
endif
	$(MAKE) BOBJS="$(BOBJS)" SOBJS="$(SOBJS)" POBJS="$(POBJS)" -C $(DEVTOP1)/boot -f $(MAKEFILE2)2 everything

bootclean:
	$(RM) $(DEVTOP1)/boot/*
	$(RMDIR) $(DEVTOP1)/boot
