/*
 * gbfix.c
 * 
 * GBFix - Main Module
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

// Include used C header(s):
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include module header(s):
#include "gbfix.h"

const char s_pszAppName[] = "GBFix";
const char s_pszAppVer[] = "0.3.3-proto";

int doFileOperations (PRUN_PARAMS prp);
static inline void validateChksums (PRUN_PARAMS prp);

int main (int argc, char* argv[]) {
	
	// Print application name and version identifier.
	printf("%s v%s\n", s_pszAppName, s_pszAppVer);
	
	RUN_PARAMS rpParams; // Runtime parameters.
	
	// Initialize runtime parameters.
	memset(&rpParams, 0, sizeof(RUN_PARAMS));
	
	if ((rpParams.pHdrUps = malloc(sizeof(HDR_UPDATES))) == NULL) {
		fprintf(stderr, "Error: Could not allocate buffer for header updates.\n");
		setExitCode(&rpParams, EXIT_FAILURE);
		doExit(&rpParams);
	}
	
	memset(rpParams.pHdrUps, 0, sizeof(HDR_UPDATES));
	
	// Process command-line arguments.
	if (argc > 1) {
		
		int nOpt; // getopt return value.
		int iLongOpt; // Long option index.
		while (1) {
			
			// Structure containing long options.
			static struct option optLongOpts[] = {
				{ "help", no_argument, 0, 'h' },
				{ "gpl", no_argument, 0, 0 },
				{ "file", required_argument, 0, 'f' },
				{ "verbose", no_argument, 0, 'v' },
				{ "dry-run", no_argument, 0, 'd' },
				{ "norominfo", no_argument, 0, 0 },
				{ "region", required_argument, 0, 'r' },
				{ "sgbflags", required_argument, 0, 's' },
				{ "romver", required_argument, 0, 'V' },
				{ "title", required_argument, 0, 't' },
				{ "manufacturer", required_argument, 0, 'm' },
				{ "cgbflags", required_argument, 0, 'c' },
				{ "carttype", required_argument, 0, 'C' },
				{ "ramsize", required_argument, 0, 'R' },
				{ 0, 0, 0, 0}
			};
			
			// Exit if necessary.
			if (rpParams.uFlags & RPF_EXIT) break;
			
			// Get options.
			iLongOpt = 0; // Reset long option index.
			if ((nOpt = getopt_long(argc, argv, "hf:vdr:s:V:t:m:c:C:R:", optLongOpts, &iLongOpt)) == -1) {
				setExitCode(&rpParams, EXIT_SUCCESS);
				break;
			}
			
			// Process options.
			switch (nOpt) {
			case 0:
				// Process long options.
				switch (iLongOpt) {
				case 1:
					// Print out GPL notice.
					printGplNotice();
					setExitCode(&rpParams, EXIT_SUCCESS);
					break;
					
				case 5:
					// Set norominfo flag.
					rpParams.uFlags |= RPF_NOROMINFO;
					break;
					
				default:
					// Handle unsupported long option.
					fprintf(stderr, "Error: Unsupported long option: %s (%d)\n", optLongOpts[iLongOpt].name, iLongOpt);
					setExitCode(&rpParams, EXIT_FAILURE);
					
				}
				break;
				
			case 'h':
				// Show help message.
				printHelp();
				setExitCode(&rpParams, EXIT_SUCCESS);
				break;
				
			case 'f':
				// Select file.
				
				if (rpParams.uFlags & RPF_ROMFILE || rpParams.pszFileName != NULL) {
					fprintf(stderr, "Error: ROM file already selected. Cannot operate on two files.\n");
					break;
				} else {
					rpParams.uFlags |= RPF_ROMFILE;
				}
				
				// Allocate string buffer.
				rpParams.cchFileName = strlen(optarg);
				if ((rpParams.pszFileName = malloc(rpParams.cchFileName * sizeof(char))) == NULL) {
					perror("Could not allocate memory for file name buffer.\n");
					errno = 0;
					setExitCode(&rpParams, EXIT_FAILURE);
					break;
				}
				memset(rpParams.pszFileName, 0, rpParams.cchFileName * sizeof(char));
				
				// Copy into string buffer.
				if (!strncpy(rpParams.pszFileName, optarg, rpParams.cchFileName)) {
					perror("Could not copy file name to internal buffer.\n");
					errno = 0;
					setExitCode(&rpParams, EXIT_FAILURE);
					break;
				}
				
				break;
				
			case 'v':
				// Set verbose mode.
				rpParams.uFlags |= RPF_VERBOSE;
				printf("Using verbose mode.\n");
				break;
				
			case 'd':
				// Enable dry-run.
				rpParams.uFlags |= RPF_DRYRUN;
				break;
				
			case 'r':
				// Set ROM region.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_REGION;
				rpParams.pHdrUps->uRegion = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 's':
				// Set SGB flags.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_SGBF;
				rpParams.pHdrUps->uSgbFlag = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 'V':
				// Set ROM version.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_ROMVER;
				rpParams.pHdrUps->uRomVer = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 't':
				// Set ROM title.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_TITLE;
				
				size_t cbTitle = strlen(optarg);
				if (cbTitle > 16) {
					fprintf(stderr, "Warning: Maximum title length exceeded. Output will be truncated.\n");
					cbTitle = 16;
				}
				
				memset(&rpParams.pHdrUps->htTitle.oldTitle.strTitle, 0, 16);
				strncpy(rpParams.pHdrUps->htTitle.oldTitle.strTitle, optarg, cbTitle);
				break;
				
			case 'm':
				// Set manufacturer.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_MANU;
				
				size_t cbManu = strlen(optarg);
				if (cbManu > 4) {
					fprintf(stderr, "Warning: Maximum manufacturer code length exceeded. Output will be truncated.\n");
					cbManu = 4;
				}
				memset(&rpParams.pHdrUps->htTitle.newTitle.strManufacturer, 0, 4);
				memcpy(rpParams.pHdrUps->htTitle.newTitle.strManufacturer, optarg, cbManu);
				break;
				
			case 'c':
				// Set CGB flags.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_CGBF;
				rpParams.pHdrUps->htTitle.newTitle.uCgbFlag = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 'C':
				// Set cart type.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_CARTTYPE;
				rpParams.pHdrUps->uCartType = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 'R':
				// Set RAM size.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				rpParams.pHdrUps->uFlags |= UPF_RAMSIZE;
				rpParams.pHdrUps->uRamSize = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case '?':
				// Unknown command.
				rpParams.uFlags |= RPF_UNKNOWNPARAM;
				break;
				
			default:
				// Unknown getopt_long return value.
				fprintf(stderr, "Error: getopt_long returned unknown value (0x%X).\n", nOpt);
				setExitCode(&rpParams, EXIT_FAILURE);
			}
		}
	} else {
		fprintf(stderr, "Error: No options specified.\nUse %s -h to see options.\n", argv[0]);
		setExitCode(&rpParams, EXIT_FAILURE);
	}
	
	// Perform operations on the ROM header.
	if (doFileOperations(&rpParams))
		fprintf(stderr, "Error: Fatal error while performing file operations.\n");
	
	// Exit program.
	doExit(&rpParams);
	return EXIT_SUCCESS;
	
}

int doFileOperations (PRUN_PARAMS prp) {
	
	if (prp == NULL) {
		errno = EFAULT;
		perror("Bad/NULL pointer passed as runtime parameters.\n");
		errno = 0;
		return 1;
	}
	
	// Check for ROM file flag set.
	if (!(prp->uFlags & RPF_ROMFILE)) {
		setExitCode(prp, EXIT_SUCCESS);
		return 0;
	}
	
	// Check file name buffer.
	if (prp->pszFileName == NULL) {
		fprintf(stderr, "Error: File name buffer not allocated.\n");
		setExitCode(prp, EXIT_FAILURE);
		return 1;
	}
	
	// Allocate header buffer.
	if ((prp->pHdr = malloc(sizeof(GBHEAD))) == NULL) {
		perror("Could not allocate buffer for header.\n");
		errno = 0;
		setExitCode(prp, EXIT_FAILURE);
		return 1;
	}
	
	// Read header from file.
	if (loadHeaderFromFile(prp->pszFileName, prp->pHdr)) {
		perror("Failed to load ROM header.\n");
		errno = 0;
		setExitCode(prp, EXIT_FAILURE);
		return 1;
	}
	
	// Print ROM info.
	if (!(prp->uFlags & RPF_NOROMINFO)) {
		printf("Using file: \"%s\"\n", prp->pszFileName);
		printRomInfo(prp->pHdr);
	}
	
	// Skip file updates if update flag not set.
	if (!(prp->uFlags & RPF_UPDATEROM)) {
		setExitCode(prp, EXIT_SUCCESS);
		return 0;
	}
	
	// TODO: Add routine to copy updates from the update structure into header to write back.
	
	validateChksums(prp);
	
	// Print updated ROM header information.
	if (prp->uFlags & RPF_VERBOSE || prp->uFlags & RPF_DRYRUN) {
		printf("Updated ROM header:\n");
		printRomInfo(prp->pHdr);
	}
	
	// Prevent save if dry run is enabled.
	if (prp->uFlags & RPF_DRYRUN) {
		setExitCode(prp, EXIT_SUCCESS);
		return 0;
	}
	
	// Write header back to file.
	if (saveHeaderToFile(prp->pszFileName, prp->pHdr)) {
		perror("Failed to save ROM header to file.\n");
		errno = 0;
		setExitCode(prp, EXIT_FAILURE);
		return 1;
	}
	
	// Set successful exit code & exit.
	setExitCode(prp, EXIT_SUCCESS);
	return 0;
	
}

static inline void validateChksums (PRUN_PARAMS prp) {
	
	uint8_t uNewHdrChksum = mkGbHdrChksum(prp->pHdr);
	// uint16_t uNewGlobalChksum = mkGbHdrGlobalChksum(prp->pgbHdr);
	
	if (prp->pHdr->uHdrChksum != uNewHdrChksum) {
		if (prp->uFlags & RPF_UPDATEROM) {
			if (prp->uFlags & RPF_VERBOSE) printf("Updating header checksum to 0x%X.\n", uNewHdrChksum);
			prp->pHdr->uHdrChksum = uNewHdrChksum;
		} else {
			printf("Warning: Header checksum is invalid. ROM will be unbootable! Correct value is 0x%X.\n", uNewHdrChksum);
		}
	}
	
	/*
	if (correctGlobalChksum(prp->pgbHdr) != uNewGlobalChksum) {
		if (prp->uFlags & RPF_UPDATEROM) {
			if (prp->uFlags & RPF_VERBOSE) printf("Updating global checksum to 0x%X.\n", uNewHdrChksum);
			prp->pHdr->uGlobalChksum = uNewGlobalChksum;
		} else {
			printf("Warning: Global checksum is invalid. Real hardware \
will not care, but emulators might give warnings!\n");
		}
	}
	*/
	
}

// EOF
