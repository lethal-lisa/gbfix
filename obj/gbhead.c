/*
 * obj/gbhead.c
 * 
 * GBFix - GB ROM Header Info Module
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

// Include used C header(s):
#include <endian.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Include module header(s):
#include "../inc/gbhead.h"

const char s_pszUnknown[] = "Unknown";

// Returns nonzero if licensee is of new type.
int isNewLicensee (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	if (pHdr->uOldLicensee == LICENSEE_NEW) return -1;
	return 0;
	
}

// Obtains the licensee code.
unsigned char getLicenseeCode (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	if (isNewLicensee(pHdr)) {
		if (pHdr->uLicensee[0] != pHdr->uLicensee[1]) {
			fprintf(stderr, "Error: New licensee codes do not match.\n");
			return pHdr->uLicensee[0];
		}
		return pHdr->uLicensee[0];
	}
	
	return pHdr->uOldLicensee;
	
}

// Get the licensee type as a string.
const char* getLicenseeTypeStr (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return s_pszUnknown;
	}
	
	if (isNewLicensee(pHdr)) return "New";
	return "Old";
	
}

// Get the ROM region as a string.
const char* getRegionStr (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return s_pszUnknown;
	}
	
	if (pHdr->uRegion >= REGION_INTERNATIONAL) return "International";
	if (pHdr->uRegion == REGION_JAPAN) return "Japan";
	return s_pszUnknown;
	
}

uint16_t correctGlobalChksum (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		return be16toh(pHdr->uGlobalChksum);
	#else
		return pHdr->uGlobalChksum;
	#endif
	
}

// Returns the ROM size in kilobytes.
long int getRomSize (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	return (long int)(32 << pHdr->uRomSize);
	
}

// Generates a header checksum.
uint8_t mkGbHdrChksum (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	unsigned long int uChksum = 0;
	
	int iByte;
	//for (iByte = 0x34; iByte < 0x4C; iByte++)
	for (iByte = 0; iByte < sizeof(GBHEAD); iByte++)
		uChksum = uChksum - ((unsigned char*) pHdr)[iByte] - 1;
	
	return (uint8_t)(uChksum & 0xFF);
	
}

// Loads the header in from a file.
long int loadHeaderFromFile (const char* pszFileName, PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return -1;
	}
	
	FILE* pFile;
	
	// Open the file for binary read.
	if ((pFile = fopen(pszFileName, "rb")) == NULL) return -1;
	
	// Seek to header offset and read the header in.
	if (fseek(pFile, 0x0100, SEEK_SET) || (fread(pHdr, sizeof(GBHEAD), 1, pFile) < 1)) {
		fclose(pFile);
		return -1;
	}
	
	// Close the file.
	fclose(pFile);
	return 0;
	
}

// Saves the header to a file.
long int saveHeaderToFile (const char* pszFileName, const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return -1;
	}
	
	FILE* pFile;
	
	// Open the file for binary write.
	if ((pFile = fopen(pszFileName, "wb")) == NULL) return -1;
	
	// Seek to header offset and write in the header.
	if (fseek(pFile, 0x0100, SEEK_SET) || (fwrite(pHdr, sizeof(GBHEAD), 1, pFile) < 1)) {
		fclose(pFile);
		return -1;
	}
	
	// Close the file.
	fclose(pFile);
	return 0;
	
}

// EOF
