# Copyright 2020 Norman J. Goldstein
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Keyboard codes are converted to an enum with
# this prefix, in dev/src/misc/src/KbdCodes.hpp
# Platform.inc defines the VKEY_HEADER variable
VKEY_PREFIX = VKEY_

CLIOGEN=iogen

INCLUDES = $(HEADERS)
INCLUDES_MV = $(HEADERS_MV)

ifneq ($(strip $(SSRD_LOGGING)),)
CPPFLAGS += -DSSRD_LOGGING=$(SSRD_LOGGING)
endif

ifneq ($(strip $(BINARY_NAME)),)
BINARY = $(BINARY_NAME)$(EXE_SUFFIX)
else
BINARY=
endif

ifneq ($(strip $(BINARIES_NAMES)),)
BINARIES = $(patsubst %, %$(EXE_SUFFIX), $(BINARIES_NAMES))
else
BINARIES=
endif


vpath %.inc $(SOURCE_DIR)   #Header file file includes
vpath %.rc $(SOURCE_DIR)   #Resource files
vpath %.int $(SOURCE_DIR)   #Template file includes
vpath %.h $(SOURCE_DIR)
vpath %.hpp $(SOURCE_DIR)
vpath %.cpp $(SOURCE_DIR)
vpath %.cc $(SOURCE_DIR)
vpath %.tpp $(SOURCE_DIR) #Template file function bodies
vpath %.c $(SOURCE_DIR)
vpath %.cu $(SOURCE_DIR)
vpath %.for $(SOURCE_DIR)
vpath %.f90 $(SOURCE_DIR)
vpath %.f $(SOURCE_DIR)
vpath %.pl $(SOURCE_DIR)
vpath %.sh $(SOURCE_DIR)
vpath %.version $(SOURCE_DIR)
vpath %db.xml $(SOURCE_DIR)

LB_OBJS = $(patsubst %, %$(O_SUFFIX), $(LIBRARY_OBJ_NAMES))
BIN_OBJS = $(patsubst %, %$(O_SUFFIX), $(BINARY_OBJ_NAMES))


.PHONY: all clean libs incinst bins bininst

.SUFFIXES: $(EXE_SUFFIX) $(O_SUFFIX)  .i .E .pl .c .cpp .cu .hxml .for .f90 .f .h .hpp db.xml .res .rc

.PRECIOUS: iog_%.cpp moc_%.cpp

all incinst bininst libs:: $(SUBDIRS)

bins:: $(SUBDIRS) libs

# Can't use exe_suffix, as it can be empty
# Make sure that O_SUFFIX and LIB_SUFFIX are not empty!
clean:: $(SUBDIRS)
	-$(RM) *$(O_SUFFIX) *$(LIB_SUFFIX) *~ iog_* moc_* $(BINARY) $(BINARIES) $(HEADERS_MV)

define run-subdirs
	cd $@ && $(WQ)$(MAKE)$(WQ) $(MAKECMDGOALS)
endef

#---------------------------------
#----- If SUBDIRS is specified ...
ifneq ($(strip $(SUBDIRS)),)
$(SUBDIRS):  FORCE
	@ if [ -d "$@" ]; then\
	$(run-subdirs);\
	fi
endif
FORCE: ;

LDLIBS += $(USR_LDLIBS)
LDFLAGS += $(USR_LDFLAGS)


#---------------------------------
ifneq ($(strip $(INCLUDES)),)

incinst:: $(INCLUDES)
	for X in $? ; do \
	  echo \#include \"$$X\" > \
	  $(WQ)$(INC_INST_DIR)/`basename $$X`$(WQ) ; done
endif

ifneq ($(strip $(INCLUDES_MV)),)

incinst:: $(INCLUDES_MV)
	for X in $? ; do \
	$(MV) $$X $(INC_INST_DIR)   ; done
endif
#---------------------------------


#Windows ignores the -dD flag
.cpp.i:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -E -dD $^ > $@


.cpp$(O_SUFFIX):
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c $<


#-I. is to find the initial (empty) iog_%.h or og_%.hpp
CLANG_FLAGS+=-xc++ -I. 

iog_%.cpp iog_%.h: %.h
	$(CLIOGEN) -i $< ${$(notdir $(basename $@))_FLAGS}  -o $(basename $@)  -- $(CLANG_FLAGS) $(CXXFLAGS0) $(CPPFLAGS0) $(TARGET_ARCH)

iog_%.cpp iog_%.h: %.hpp
	$(CLIOGEN) -i $< ${$(notdir $(basename $@))_FLAGS}  -o $(basename $@)  -- $(CLANG_FLAGS) $(CXXFLAGS0) $(CPPFLAGS0) $(TARGET_ARCH)

moc_%.cpp: %.hpp
	$(QT_MOC_EXE) $< -o $@

moc_%.cpp: %.h
	$(QT_MOC_EXE) $< -o $@


#---------------------------------
#----- If LIBRARY is specified ...
ifneq ($(strip $(LIBRARY_NAME)),)
LIBRARY = $(LIB_INST_DIR)/$(LIB_PREFIX)$(LIBRARY_NAME)$(LIB_SUFFIX)

ifeq ($(OS),Windows_NT)
ifneq ($(wildcard $(LIBRARY)),)
#Windows librarian needs the libary name on the libe line
#if the library already exists
LB_OBJS0 = $(LIBRARY)
endif

ifeq ($(SUBSYSTEM_WINDOWS),1)
  LD_DBG_FLAGS += /SUBSYSTEM:WINDOWS
endif

endif

else
LIBRARY =
endif



ifneq ($(strip $(LIBRARY)),)
all libs:: $(LIBRARY)

#On Windows, if the library is already existing, you need
#to specify it as one of the files to add,
#otherwise the library ends up being truncated.
#This is LB_OBJS0
$(LIBRARY): $(LB_OBJS)
	$(AR_RV)${WQ}$(LIBRARY)$(WQ) $(USR_LIBFLAGS) $(LB_OBJS0) $?
endif
#------------------------------------


#---------------------------------
ifneq ($(strip $(BINARY)),)
all bins:: $(BINARY)

$(BINARY): $(BIN_OBJS) $(LIBRARY)
	$(LINKER) $(LINKER_OUT_FLAG)$@ $(LDFLAGS) $(BIN_OBJS) $(LDLIBS) 

bins :: libs

bininst::
	$(MV) $(BINARY) $(BIN_INST_DIR)
endif


ifneq ($(strip $(BINARIES)),)
all bins:: $(BINARIES)

$(BINARIES): $(LIBRARY)

bins:: libs

bininst::
	$(MV) $(BINARIES) $(BIN_INST_DIR)
endif


#-------------- Just a solitary link of a single source file
$(O_SUFFIX)$(EXE_SUFFIX):
	$(LINKER) $(LINKER_OUT_FLAG)$@ $(LDFLAGS) $^ $(LDLIBS) 

#endif
#----------------------------------------

