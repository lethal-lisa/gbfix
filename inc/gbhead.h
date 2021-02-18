/*
 * inc/gbhead.h
 * 
 * GBFix - GB ROM Header Info Module Header
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

#ifndef _GBHEAD_H_
#define _GBHEAD_H_

/*
	GameBoy Mask ROM Layout:
	
	$0000-$00FF:	Interrupt handler information.
	$0100-$014F:	ROM header.
	$0150-$FFFF:	Game/application code.
	
	Header Layout:
	$0100-$0103:	Entry point ($00 $C3 ($50 $01)).
	$0104-$0133:	Nintendo logo.
		$0134-$0143:	Registration title.
		Or
		$0134-$013E:	Registration title.
		$013F-$0142:	Manufacturer code.
		$0143:			CGB flag.
	$0144-$0145:	Licensee code (new).
	$0146:			SGB flag.
	$0147:			Cart type.
	$0148:			ROM size (32kB Shl N).
	$0149:			RAM size.
	$014A:			Region.
	$014B:			Licensee code (old).
	$014C:			Software version.
	$014D:			Header checksum.
	$014E-$014F:	Global checksum (big endian).
*/

// ---------------------------------------------------------------------
// Define flags.
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// Flags for structure tagGBHEAD.
// ---------------------------------------------------------------------

// CGB feature flags:
enum {
	CGBF_FUNC = 0x80,
	CGBF_PGB1 = 0x04,
	CGBF_PGB2 = 0x08,
	CGBF_CGBONLY = 0x40,
	CGBF_MASK = 0xCC
};

// SGB feature flags:
#define SGBF_SGBSUPPORT 0x03

// Region IDs:
#define REGION_JAPAN 0x00
#define REGION_INTERNATIONAL 0x01

// New licensee code.
#define LICENSEE_NEW 0x33

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
// Define structures.
// ---------------------------------------------------------------------

// GameBoy ROM header structure.
typedef struct tagGBHEAD
{
	unsigned char uEntryPoint[4];
	unsigned char uNintendoLogo[48];
	union tagTitle {
		char szTitle[16];
		struct tagNewTitle {
			char szTitle[11];
			char strManufacturer[4];
			unsigned char uCgbFlag;
		} newTitle;
	} title;
	unsigned char uLicensee[2];
	unsigned char uSgbFlag;
	unsigned char uCartType;
	unsigned char uRomSize;
	unsigned char uRamSize;
	unsigned char uRegion;
	unsigned char uOldLicensee;
	unsigned char uRomVer;
	unsigned char uHdrChkSum;
	unsigned short uGlobalChkSum;
} __attribute__((packed, aligned(4))) GBHEAD, *PGBHEAD;

// Structure containing information about what to update in the ROM.
typedef struct tagHDR_UPDATES
{
	unsigned long uFlags; // Flags about what is to be updated.
	GBHEAD hdr; // Fake header containing new information.
} __attribute__((packed, aligned(4))) HDR_UPDATES, *PHDR_UPDATES;

// ---------------------------------------------------------------------
// Define functions.
// ---------------------------------------------------------------------

// Licensee code functions.
int isNewLicensee (const PGBHEAD pHdr);
unsigned char getLicenseeCode (const PGBHEAD pHdr);

const char* getLicenseeTypeStr (const PGBHEAD pHdr);
const char* getRegionStr (const PGBHEAD pHdr);

unsigned short int correctGlobalChkSum (const PGBHEAD pHdr);
long int getRomSize (const PGBHEAD pHdr);
unsigned char mkGbHdrChksum (const PGBHEAD pHdr);

// File I/O functions.
long int loadHeaderFromFile (const char* pszFileName, PGBHEAD pHdr);
long int saveHeaderToFile (const char* pszFileName, const PGBHEAD pHdr);

#endif /* _GBHEAD_H_ */

// EOF
