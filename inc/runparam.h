/*
 * inc/runparam.h
 * 
 * GBFix - Runtime Parameters Module Header
 * 
 * Copyright 2021 Lisa Murray
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#ifndef _RUNPARAM_H_
#define _RUNPARAM_H_

#include <stddef.h>

// enum RPFLAGS
// {
enum {
	RPF_EXIT = 0x00000001,
	RPF_UNKNOWNPARAM = 0x00000002,
	RPF_VERBOSE = 0x00000004,
	RPF_NOROMINFO = 0x00000008,
	RPF_ROMFILE = 0x00000100,
	RPF_UPDATEROM = 0x00000200,
	RPF_MASK = 0x00000307
};

typedef struct tagRUN_PARAMS
{
	unsigned long int uFlags;
	long int nExitCode;
	size_t cchFileName;
	char* pszFileName;
} __attribute__((packed, aligned(4))) RUN_PARAMS, *PRUN_PARAMS;

void setExitCode (PRUN_PARAMS pParams, const long int nExitCode);
void doExit (PRUN_PARAMS pParams);

#endif /* _RUNPARAM_H_ */

// EOF
