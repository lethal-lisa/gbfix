## ---------------------------------------------------------------------
## 
## GBFix makefile
## 
## Usage:
## make [build] - Build GBFix.
## sudo make install - Install built package to ${DEST}.
## make clean   - Remove extra files.
## 
## Copyright 2021 Lisa Murray
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
## MA 02110-1301, USA.
## 
## ---------------------------------------------------------------------

## ---------------------------------------------------------------------
## Set phony & default targets, and override the default suffix rules.
## ---------------------------------------------------------------------
.PHONY: build install clean
.SUFFIXES:

.DEFAULT_GOAL := build

include makefile.osdetect

## ---------------------------------------------------------------------
## Setup project details.
## ---------------------------------------------------------------------

TARGET   := $(shell basename ${CURDIR})
BUILD    := 
SOURCES  := obj
DATA     := 
INCLUDES := inc
DEST     ?= /bin/

LIBS     := 
LIBDIRS  :=

OBJS     := ${TARGET}.o
OBJS     += ${SOURCES}/gbhead.o
OBJS     += ${SOURCES}/messages.o
OBJS     += ${SOURCES}/runparam.o

ifdef OS_DOSLIKE
	EXE_SUFFIX = .exe
else
	EXE_SUFFIX = 
endif
TARGET   := ${TARGET}${EXE_SUFFIX}

## ---------------------------------------------------------------------
## Set flags for code generation.
## ---------------------------------------------------------------------

CC       := gcc
LD       := gcc
OBJCOPY  := objcopy

CFLAGS   =  -Wall -O3\
	-funsigned-char\
	-fomit-frame-pointer -fno-strict-aliasing -fshort-enums\
	-fno-printf-return-value

ifdef INCLUDE
	CFLAGS += ${INCLUDE}
endif

LDFLAGS  := ${CFLAGS}

## ---------------------------------------------------------------------
## Compilation rules.
## ---------------------------------------------------------------------

.INTERMEDIATE: ${TARGET}.elf
.DELETE_ON_ERROR: ${OBJS} ${TARGET}.elf

build: ${TARGET}
	chmod +x $<

## Strip binaries.
${TARGET}: ${TARGET}.elf
	-@echo 'Stripping symbols from "$<"... ("$<"->"$@")'
	${OBJCOPY} -vgO elf64-x86-64 $< $@

## Link objects.
${TARGET}.elf: ${TARGET}.o ${OBJS}
	-@echo 'Linking objects... ("$^"->"$@")'
	${LD} $^ $(LDFLAGS) -o $@

## Compile objects.
${OBJS}: %.o : %.c
	-@echo 'Compiling object "$@"... ("$<"->"$@")'
	${CC} ${CFLAGS} -c $< -o $@

## Install built file.
install: ${TARGET}
	-@echo 'Installing "$<"...'
	chmod +x $<
	chown root:root $<
	cp -vf $< ${DEST}

## Remove unnecessary binary files.
.IGNORE: clean
clean:
	-@echo 'Cleaning up intermediary files...'
	@rm -vf ${SOURCES}/*.o *.o *.elf

## EOF
