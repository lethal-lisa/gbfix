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

/*
 * 
 * name: isNewLicensee
 * 
 * 		Returns whether or not the licensee code is of the new type.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure to use.
 * 
 * @return: int
 * 		Returns LICETYPE_NEW or LICETYPE_OLD depending on the type of
 * 	the licensee code. Also returns 0 and sets errno on an error.
 * 
 */
int isNewLicensee (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	if (pHdr->uOldLicensee == LICENSEE_NEW) return LICETYPE_NEW;
	return LICETYPE_OLD;
	
}

/*
 * 
 * name: getLicenseeCode
 * 
 * 		Obtains the licensee code regardless of type.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure to use.
 * 
 * @return: uint8_t
 * 		Returns the licensee code, or sets errno and returns zero on
 * 	error. Sets errno to EFAULT if pHdr was NULL, or EINVAL if the
 * 	licensee code is of type new, and the two do not match.
 * 
 */
uint8_t getLicenseeCode (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	if (!isNewLicensee(pHdr)) {
		if (errno > 0) return 0;
		return pHdr->uOldLicensee;
	}
	
	if (pHdr->uLicensee[0] != pHdr->uLicensee[1]) {
		// fprintf(stderr, "Error: New licensee codes do not match.\n");
		// return pHdr->uLicensee[0];
		errno = EINVAL;
		return 0;
	}
	return pHdr->uLicensee[0];
	
}

/*
 * 
 * name: getLicenseeTypeStr
 * 
 * 		Returns the licensee type as a constant char string.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure to use.
 * 
 * @return: const char*
 * 		Returns the licensee type as either "New" or "Old", or
 * 	"Unknown" if an error occured. Sets errno on error.
 * 
 */
const char* getLicenseeTypeStr (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return s_pszUnknown;
	}
	
	if (!isNewLicensee(pHdr)) {
		if (errno > 0) return s_pszUnknown;
		return "Old";
	}
	return "New";
	
}

/*
 * 
 * name: getRegionStr
 * 
 * 		Returns the ROM's region as a constant char string.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure to use.
 * 
 * @return: const char*
 * 		Returns the region of the ROM, or "Unknown" if the region is
 * 	unknown or an error occured. Sets errno if an error occured. Sets
 * 	EFAULT if pHdr was NULL.
 * 
 */
const char* getRegionStr (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return s_pszUnknown;
	}
	
	if (pHdr->uRegion >= REGION_INTERNATIONAL) return "International";
	if (pHdr->uRegion == REGION_JAPAN) return "Japan";
	return s_pszUnknown;
	
}

/*
 * 
 * name: getRomSize
 * 
 * 		Gets the size field of the ROM's header, and converts it into
 * 	kilobytes.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure to use.
 * 
 * @return: long int
 * 		Returns the ROM size, or 0 on error. Sets errno on an error.
 * 	Sets EFAULT if pHdr was NULL, or EINVAL if the ROM size field's value
 * is zero.
 * 
 */
long int getRomSizeInkB (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	if (pHdr->uRomSize == 0) {
		errno = EINVAL;
		return 0;
	}
	
	return (long int)(32 << pHdr->uRomSize);
	
}

/*
 * 
 * name: correctGlobalChksum
 * 
 * 		Corrects the global checksum to the host machine's endianness.
 * 	This does nothing on big-endian machines, but performs a byteswap
 * 	on little-endian ones.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure containing
 * 		the checksum to correct.
 * 
 * @return: uint16_t
 * 		Returns either the corrected checksum, or 0 on error. Sets 
 * 	errno on error. Sets either EFAULT if pHdr was NULL, or EINVAL if 
 * 	the checksum's value is zero.
 * 
 */
uint16_t correctGlobalChksum (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	if (pHdr->uGlobalChksum == 0) {
		errno = EINVAL;
		return 0;
	}
	
	uint16_t uRetVal;
	
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		// If little endian.
		//return be16toh(pHdr->uGlobalChksum);
		uRetVal = ((pHdr->uGlobalChksum[1] << 8) | pHdr->uGlobalChksum[0]);
		
	#else
		// If big endian.
		//return pHdr->uGlobalChksum;
		uRetVal = ((pHdr->uGlobalChksum[0] << 8) | pHdr->uGlobalChksum[1]);
	#endif
	
	return uRetVal;
	
}

// Generates a header checksum.
/*
 * 
 * name: mkGbHdrChksum
 * 
 * 		Generates a new header checksum.
 * 
 * @param:
 * 		const PGBHEAD pHdr:
 * 			Constant pointer to the GameBoy header structure to use.
 * 
 * @return: uint8_t
 * 		Returns the newly generated checksum, or sets errno and
 * 	returns zero on error.
 * 
 */
uint8_t mkGbHdrChksum (const PGBHEAD pHdr) {
	
	if (pHdr == NULL) {
		errno = EFAULT;
		return 0;
	}
	
	unsigned long int uChksum = 0; // Buffer for the checksum.
	int iByte; // Index of current byte.
	
	//for (iByte = 0x34; iByte < 0x4C; iByte++)
	for (iByte = 0; iByte < sizeof(GBHEAD); iByte++)
		uChksum = uChksum - ((unsigned char*) pHdr)[iByte] - 1;
	
	return (uint8_t)(uChksum & 0xFF);
	
}

/*
 * 
 * name: loadHeaderFromFile
 * 
 * 		Loads the GameBoy header structure from a given file.
 * 
 * @param:
 * 		const char* pszFileName:
 * 			Name of the file to read from.
 * 
 * 		PGBHEAD pHdr:
 * 			Pointer to the header structure to read data into.
 * 
 * @return: int
 * 		Returns zero on success, or sets errno and returns nonzero on
 * 	error.
 * 
 */
int loadHeaderFromFile (const char* pszFileName, PGBHEAD pHdr) {
	
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
/*
 * 
 * name: saveHeaderToFile
 * 
 * 		Saves a GameBoy header structure to a given file.
 * 
 * @param:
 * 		const char* pszFileName:
 * 			Name of the file to write to.
 * 
 * 		PGBHEAD pHdr:
 * 			Pointer to the header structure to read data into.
 * 
 * @return: int
 * 		Returns zero on success, or sets errno and returns nonzero on
 * 	error.
 * 
 */
int saveHeaderToFile (const char* pszFileName, const PGBHEAD pHdr) {
	
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
