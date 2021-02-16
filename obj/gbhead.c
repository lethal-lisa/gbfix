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

#include "../inc/gbhead.h"

static PGBHEAD handleGbHdrIOErr (FILE* pFile, PGBHEAD pgbhHdr, const char* pszMsg);

unsigned char isNewLicensee (const PGBHEAD pHdr) {
	
	if (pHdr->uOldLicensee == LICENSEE_NEW) return -1;
	return 0;
	
}

unsigned char getLicenseeCode (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) return 0;
	
	if (isNewLicensee(pHdr)) {
		if (pHdr->uLicensee[0] != pHdr->uLicensee[1]) {
			fprintf(stderr, "New licensee codes do not match.");
			return 0;
		}
		return pHdr->uLicensee[0];
	}
	
	return pHdr->uOldLicensee;
	
}

unsigned char mkGbHdrChksum (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) return 0;
	
	//unsigned char* pTemp = (unsigned char*) pHdr;
	unsigned long int uChkSum = 0;
	
	int iByte;
	for (iByte = 0x34; iByte < 0x4C; iByte++)
		uChkSum = uChkSum - ((unsigned char*) pHdr)[iByte] - 1;
	
	return (unsigned char)(uChkSum & 0xFF);
	
}

long int getRomSize (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) return 0;
	
	return (long int)(32 << pHdr->uRomSize);
	
}

static PGBHEAD handleGbHdrIOErr (FILE* pFile, PGBHEAD pgbhHdr, const char* pszMsg) {
	
	if (pszMsg != NULL) perror(pszMsg);
	
	if (pFile != NULL && (fclose(pFile))) perror("Failed to close file.\n");
	if (pgbhHdr) free(pgbhHdr);
	
	return pgbhHdr;
	
}

PGBHEAD loadHeaderFromFile (const char* pszFileName) {
	
	FILE* pFile;
	PGBHEAD pgbhHdr;
	
	// Allocate buffer for ROM header.
	if ((pgbhHdr = malloc(sizeof(GBHEAD))) == NULL)
		return handleGbHdrIOErr(pFile, pgbhHdr, "Error allocating buffer for ROM header.\n");
	
	// Open file.
	if ((pFile = fopen(pszFileName, "rb")) == NULL)
		return handleGbHdrIOErr(NULL, pgbhHdr, "Error opening ROM file.\n");
	
	// Seek to header offset.
	if (fseek(pFile, 0x0100, SEEK_SET))
		return handleGbHdrIOErr(pFile, pgbhHdr, "Error seeking to header.\n");
	
	// Read in header.
	if (fread(pgbhHdr, sizeof(GBHEAD), 1, pFile) < 1)
		return handleGbHdrIOErr(pFile, pgbhHdr, "Error reading header from file.\n");
	
	// Close file.
	if (fclose(pFile)) perror("Error closing file.\n");
	
	return pgbhHdr;
	
}

// EOF
