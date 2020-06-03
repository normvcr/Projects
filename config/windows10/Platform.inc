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
#Comment this out to remove all logging statements in the souce code.
#This macro can also be set per source file, or finer.
SSRD_LOGGING=Simple# Boost#

# Comment out if iogen is not built for this platform
HAVE_IOGEN=Yep

# On Windows 10his has the VK_ macro values
VKEY_HEADER0=k:\10\Include\10.0.18362.0\um\WinUser.h
# Comment this out if VKEY_HEADER is not valid
VKEY_HEADER=$(subst \,/,$(VKEY_HEADER0))

#!!! Use drive letters to get rid of white space in paths
#!!!
# The folder containing cl.exe and link.exe
# If you need to define this, be sure it ends in a slash, /
MSDIR:=#v:/2019/Community/VC/Tools/MSVC/14.26.28801/bin/HostX64/x64/

# Currently used only on Windows,
# in dev/src/noclick/noclick/Makefile.in
RESOURCE_DIR = I:/resources

#This is where this heaedrs-only package was installed
CLI11_CPPFLAGS = -II:/external/CLI11

# Only BOOST_ROOT_DIR needs to be edited
BOOST_ROOT_DIR = C:/systems/Boost/boost_1_73_0
BOOST_LIB_DIRS = $(BOOST_ROOT_DIR)/lib64-msvc-14.2
BOOST_CPPFLAGS = -I$(BOOST_ROOT_DIR)


CPPFLAGS0 = $(USR_CPPFLAGS) -I$(SOURCE_DIR) -I$(INC_INST_DIR)
CPPFLAGS = $(CPPFLAGS0) #Add flags not for clang

CXXFLAGS0 =
CXXFLAGS = $(USR_CXXFLAGS) $(CXXFLAGS0) $(CXX_DBG_FLAGS) -EHsc

LDFLAGS = $(USR_LDFLAGS) $(LD_DBG_FLAGS) $(patsubst %, -LIBPATH:%, $(USR_LIB_DIRS))
LDLIBS = $(USR_LDLIBS) $(patsubst %, %.lib, $(USR_LIB_NAMES))


# If buidling iogen
# Only CLANG_ROOT_DIR needs to be edited
CLANG_ROOT_DIR = C:\systems\LLVM
CLANG_CPPFLAGS= -I$(CLANG_ROOT_DIR)\llvm-10.0.0.src\tools\clang\include \
-I$(CLANG_ROOT_DIR)\build\tools\clang\include \
-I$(CLANG_ROOT_DIR)\llvm-10.0.0.src\include \
-I$(CLANG_ROOT_DIR)\build\include

CLANG_LDDIRS=$(CLANG_ROOT_DIR)\build\Debug\lib
CLANG_LIB_NAMES=libclang clangBasic clangAST LLVMSupport clangLex LLVMCore  LLVMBinaryFormat LLVMFrontendOpenMP LLVMRemarks LLVMBitstreamReader

#For iogen command line
CLANG_FLAGS = -Wno-unknown-argument


# Only QT_ROOT_DIR needs to be edited, if ussing QT
QT_ROOT_DIR ="C:\systems\QT\5.15.0\msvc2019_64"
QT_LIB_DIRS =$(QT_ROOT_DIR)/lib
QT_INC_DIR = $(QT_ROOT_DIR)/include
QT_BIN_DIR =$(QT_ROOT_DIR)/bin
QT_MOC_EXE =$(QT_BIN_DIR)/moc.exe

# If using QWT
QWT_INC_DIR = C:\QWT613\qwt-6.1.3\src
#Build the QWT libraries to sit in the dev folder
QWT_LIB_DIR = $(DEVTOP)/libExt

#####################################################################
########## Unusual to edit below this line ##########################
SYS_INCLUDE=-I\"$(subst ;,\" -I\","$(INCLUDE)")\"

# Written twice:
# Keep the editor colorizer happy in emacs makefile mode
# Windows Quote
WQ="
WQ="

# Depends on msys2
CD := cd
CP := cp -f #copy /Y
MV := mv -f #move /Y
RM := rm -f #del /Q
LNS := ln -f -s
TOUCH := touch
MKDIR := mkdir
RMDIR := rmdir

#Not actually equivalent to  ar -rv.  See LB_OBJS0, below
AR_RV := $(MSDIR)lib -OUT:#No space at the end.

CXX=$(MSDIR)cl.exe
LINKER=$(MSDIR)link.exe

EXE_SUFFIX=.exe
LIB_SUFFIX=.lib
LIB_PREFIX=
O_SUFFIX=.obj
LINKER_OUT_FLAG=-OUT:#No space

clean::
	-$(RM) *.exe







