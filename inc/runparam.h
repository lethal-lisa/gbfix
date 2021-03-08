/*
 * inc/runparam.h
 * 
 * GBFix - Runtime Parameters Module Header
 * 
 * Copyright 2021 Lisa Murray
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
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

#include "gbhead.h"
#include <stddef.h>

// ---------------------------------------------------------------------
// Define flags.
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// Flags for structure tagHDR_UPDATES.
// ---------------------------------------------------------------------

enum {
	UPF_TITLE = 0x0001,
	UPF_MANU = 0x0002,
	UPF_CGBF = 0x0004,
	UPF_LICENSE = 0x0008,
	UPF_SGBF = 0x0010,
	UPF_CARTTYPE = 0x0020,
	UPF_RAMSIZE = 0x0040,
	UPF_REGION = 0x0080,
	UPF_ROMVER = 0x0100,
	UPF_MASK = 0x01FF
};

// ---------------------------------------------------------------------
// Flags for structure tagRUN_PARAMS.
// ---------------------------------------------------------------------

enum {
	RPF_EXIT = 0x0001, // Enable exit.
	RPF_UNKNOWNPARAM = 0x0002, // Unknown command line parameter detected.
	RPF_VERBOSE = 0x0004, // Verbose mode enabled.
	RPF_NOROMINFO = 0x0008, // Disable ROM info output.
	RPF_ROMFILE = 0x0100, // ROM file specified.
	RPF_UPDATEROM = 0x0200, // ROM is to be updated.
	RPF_DRYRUN = 0x0400, // Dry-run mode enabled.
	RPF_MASK = 0x070F // Mask of all flags.
};

// ---------------------------------------------------------------------
// Define structures.
// ---------------------------------------------------------------------

// Structure containing information about what to update in the ROM.
typedef struct tagHDR_UPDATES
{
	unsigned long int uFlags; // Flags about what is to be updated.
	// unsigned short int uHdrRev; // Header revision code.
	// PGBHEAD pHdr; // Fake header containing new information.
	GBH_TITLE htTitle;
	uint8_t uLicensee;
	uint8_t uSgbFlag;
	uint8_t uCartType;
	uint8_t uRamSize;
	uint8_t uRegion;
	uint8_t uRomVer;
} __attribute__((packed, aligned(4))) HDR_UPDATES, *PHDR_UPDATES;

// Structure containing information about the user's choices and what
// operations to perform.
typedef struct tagRUN_PARAMS
{
	unsigned long int uFlags; // Flags about
	unsigned long int nExitCode; // Code to exit with.
	size_t cchFileName; // Length of file name buffer in chars.
	char* pszFileName; // File name buffer.
	PGBHEAD pHdr; // Pointer to ROM header structure.
	PHDR_UPDATES pHdrUps; // Pointer to header updates structure.
} __attribute__((packed, aligned(4))) RUN_PARAMS, *PRUN_PARAMS;

// ---------------------------------------------------------------------
// Declare functions.
// ---------------------------------------------------------------------

void setExitCode (PRUN_PARAMS pParams, const long int nExitCode);
void doExit (PRUN_PARAMS pParams);

#endif /* _RUNPARAM_H_ */

// EOF
