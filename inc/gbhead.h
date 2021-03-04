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
	
	Special Thanks to Pan Docs; available at: https://gbdev.io/pandocs/
	
*/

#include <stdint.h>

// ---------------------------------------------------------------------
// Define flags.
// ---------------------------------------------------------------------

enum {
	HDRREV_DMG, // Original header revision.
	HDRREV_SGB, // Super GameBoy header revision.
	HDRREV_CGB, // GameBoy Color header revision.
	// HDRREV_CGB2, // Second GameBoy Color header revision (with manufacturer field).
	HDRREV_UNKNOWN // Unknown header revision.
};

enum {
	LICETYPE_OLD,
	LICETYPE_NEW,
	LICETYPE_UNKNOWN
};

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
enum {
	REGION_JAPAN,
	REGION_INTERNATIONAL
};

// New licensee code.
#define LICENSEE_NEW 0x33

// Cartridge type values.
enum {
	CT_ROM_ONLY = 0x00,
	CT_MBC1 = 0x01,
	CT_MBC1_RAM,
	CT_MBC1_BATTERY_RAM,
	CT_MBC2 = 0x05,
	CT_MBC2_BATTERY,
	CT_ROM_RAM = 0x08,
	CT_ROM_BATTERY_RAM,
	CT_MMM01 = 0x0B,
	CT_MMM01_RAM,
	CT_MMM01_BATTERY_RAM,
	CT_MBC3_BATTERY_TIMER = 0x0F,
	CT_MBC3_BATTERY_RAM_TIMER,
	CT_MBC3,
	CT_MBC3_RAM,
	CT_MBC3_BATTERY_RAM,
	CT_MBC5 = 0x19,
	CT_MBC5_RAM,
	CT_MBC5_BATTERY_RAM,
	CT_MBC5_RUMBLE,
	CT_MBC5_RAM_RUMBLE,
	CT_MBC5_BATTERY_RAM_RUMBLE,
	CT_MBC6 = 0x20,
	CT_MBC7_BATTERY_RAM_RUMBLE_SENSOR,
	CT_CAMERA = 0xFC,
	CT_TAMA5 = 0xFD,
	CT_HuC3 = 0xFE,
	CT_HuC1_BATTERY_RAM
};

// ---------------------------------------------------------------------
// Define structures.
// ---------------------------------------------------------------------

typedef struct tagGBH_TITLE
{
	union {
		struct tagOldTitle
		{
			char strTitle[16] __attribute__((nonstring));
		} oldTitle;
		struct tagNewTitle
		{
			char strTitle[11] __attribute__((nonstring));
			char strManufacturer[4] __attribute__((nonstring));
			uint8_t uCgbFlag;
		} newTitle;
	};
} __attribute__((packed, aligned(4))) GBH_TITLE, *PGBH_TITLE;

// GameBoy ROM header structure.
typedef struct tagGBHEAD
{
	uint8_t uEntryPoint[4];
	uint8_t uNintendoLogo[48];
	/*union tagTitle {
		char szTitle[16];
		struct tagNewTitle {
			char strTitle[11];
			char strManufacturer[4];
			uint8_t uCgbFlag;
		} newTitle;
	} title;*/
	GBH_TITLE htTitle;
	uint8_t uLicensee[2];
	uint8_t uSgbFlag;
	uint8_t uCartType;
	uint8_t uRomSize;
	uint8_t uRamSize;
	uint8_t uRegion;
	uint8_t uOldLicensee;
	uint8_t uRomVer;
	uint8_t uHdrChksum;
	//uint16_t uGlobalChksum;
	uint8_t uGlobalChksum[2];
} __attribute__((packed, aligned(4))) GBHEAD, *PGBHEAD;

// ---------------------------------------------------------------------
// Declare functions.
// ---------------------------------------------------------------------

unsigned int getHdrRev (const PGBHEAD pHdr);

// Licensee code functions.
int isNewLicensee (const PGBHEAD pHdr);
uint8_t getLicenseeCode (const PGBHEAD pHdr);

// String retrieval functions.
const char* getHdrRevStr (const unsigned int uHdrRev);
const char* getLicenseeTypeStr (const PGBHEAD pHdr);
const char* getRegionStr (const PGBHEAD pHdr);

// Data correction functions.
long int getRomSizeInkB (const PGBHEAD pHdr);
uint16_t correctGlobalChksum (const PGBHEAD pHdr);

// Checksum functions.
// TODO: Implement mkGbGlobalChksum.
// uint16_t mkGbGlobalChksum (const PGBHEAD pHdr);
uint8_t mkGbHdrChksum (const PGBHEAD pHdr);

// File I/O functions.
int loadHeaderFromFile (const char* pszFileName, PGBHEAD pHdr);
int saveHeaderToFile (const char* pszFileName, const PGBHEAD pHdr);

#endif /* _GBHEAD_H_ */

// EOF
