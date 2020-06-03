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
SSRD_LOGGING = Simple# Boost#

# Comment out if iogen is not built for this platform
HAVE_IOGEN:=Yep

# On fedora 30, this has the KEY_ macro values
# Comment this out if VKEY_HEADER is not valid
VKEY_HEADER = /usr/include/linux/input-event-codes.h

#Install CLI11 with the system installer, so not neeeded
CLI11_CPPFLAGS = 


# fedora 30
# It seems that Linux needs boost_log, but not Windows.
# For some reason, need to use the static boost_log,
# but -lboost_log was picking up the dynamic one.
# So, using the verbatim BOOST_LDLIBS
# boost_thread is used only with NoClick, and only on Linux.
#BOOST_LDLIBS = /usr/lib64/libboost_log.a -lboost_thread -lboost_filesystem  -lpthread -lrt
#BOOST_LIB_NAMES =
# fedora 32
BOOST_LDLIBS =
BOOST_LIB_NAMES =boost_log boost_thread boost_filesystem  pthread rt

# Install BOST with system installer, so don't need special flags
BOOST_CPPFLAGS =
BOOST_LIB_DIRS =


CPPFLAGS0 = $(USR_CPPFLAGS) -I$(SOURCE_DIR) -I$(INC_INST_DIR)
CPPFLAGS = $(CPPFLAGS0) #Add flags not for clang

CXXFLAGS0 = -fPIC
CXXFLAGS = $(USR_CXXFLAGS) $(CXXFLAGS0) $(CXX_DBG_FLAGS)

LDFLAGS = $(USR_LDFLAGS) $(LD_DBG_FLAGS) $(patsubst %, -L%, $(USR_LIB_DIRS))
LDLIBS = $(patsubst %, -l%, $(USR_LIB_NAMES))


#For buidling iogen
#fedora 30
CLANG_LDDIRS=/usr/lib64/llvm
#fedora 32
CLANG_LDDIRS=/usr/lib64/llvm9.0/lib


#fedora 30
#CLANG_LIB_NAMES=clang clangAST clangLex clangBasic clangFrontend clangSerialization LLVM
#fedora 32.  Also, softlinked  ln -s libclangBasic.so.9 libclangBasic.so
CLANG_LIB_NAMES=clang LLVM-10    clangBasic 


#To pass to the iogen command line
#fedora 30
#CLANG_FLAGS=-I/usr/include/c++/9 -I/usr/include/c++/9/x86_64-redhat-linux -I/usr/lib/gcc/x86_64-redhat-linux/9/include
#fedora 32
CLANG_FLAGS=-I/usr/lib/gcc/x86_64-redhat-linux/10/include


#If using QT
QT_LIB_DIRS=
QT_INC_DIR =/usr/include/qt5
QT_MOC_EXE= moc-qt5

#If using QWT
QWT_INC_DIR = $(QT_INC_DIR)/qwt
QWT_CPPFLAGS = #-I$(QWT_INC_DIR)
QWT_LIB_NAME = #qwt-qt5


#####################################################################
########## Unusual to edit below this line ##########################
#Window's quote only
WQ=#
CD := cd
CP := cp -f
MV := mv -f
RM := rm -f
LNS := ln -f -s
TOUCH := touch
MKDIR := mkdir -p
RMDIR := rmdir
AR_RV = gcc-ar rv  #The space is significant.  gcc-ar for lto

CXX=g++
LINKER=g++

EXE_SUFFIX=
LIB_SUFFIX=.a
LIB_PREFIX=lib
O_SUFFIX=.o
LINKER_OUT_FLAG=-o #Space needed

