/*
 * gbfix.c
 * 
 * GBFix - Main Module
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
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include module header(s):
#include "gbfix.h"

const char g_szAppName[] = "GBFix";
const char g_szAppVer[] = "0.2.2-proto";

int main (int argc, char* argv[]) {
	
	printf("%s v%s\n\n", g_szAppName, g_szAppVer);
	
	RUN_PARAMS rpParams;
	HDR_UPDATES hdrUpdates;
	
	memset(&rpParams, 0, sizeof(RUN_PARAMS));
	memset(&hdrUpdates, 0, sizeof(HDR_UPDATES));
	
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
					setExitCode(&rpParams, EXIT_FAILURE);
					break;
				}
				
				// Allocate string buffer.
				rpParams.cchFileName = strlen(optarg);
				rpParams.pszFileName = malloc(rpParams.cchFileName * sizeof(char));
				
				// Copy into string buffer.
				if ((rpParams.pszFileName == NULL) || 
				!strncpy(rpParams.pszFileName, optarg, rpParams.cchFileName)) {
					perror("Could not copy file name to internal buffer.\n");
					errno = 0;
					setExitCode(&rpParams, EXIT_FAILURE);
					break;
				}
				
				// Update flags.
				rpParams.uFlags |= RPF_ROMFILE;
				
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
				
				hdrUpdates.uFlags |= UPF_REGION;
				hdrUpdates.hdr.uRegion = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 's':
				// Set SGB flags.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_SGBF;
				hdrUpdates.hdr.uSgbFlag = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 'V':
				// Set ROM version.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_ROMVER;
				hdrUpdates.hdr.uRomVer = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 't':
				// Set ROM title.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_TITLE;
				strncpy(hdrUpdates.hdr.title.szTitle, optarg, 15);
				break;
				
			case 'm':
				// Set manufacturer.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_MANU;
				//strncpy(hdrUpdates.hdr.title.newTitle.strManufacturer, optarg, );
				memcpy(hdrUpdates.hdr.title.newTitle.strManufacturer, optarg, 4);
				break;
				
			case 'c':
				// Set CGB flags.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_CGBF;
				hdrUpdates.hdr.title.newTitle.uCgbFlag = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 'C':
				// Set cart type.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_CARTTYPE;
				hdrUpdates.hdr.uCartType = (uint8_t)strtoul(optarg, NULL, 0);
				break;
				
			case 'R':
				// Set RAM size.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_RAMSIZE;
				hdrUpdates.hdr.uCartType = (uint8_t)strtoul(optarg, NULL, 0);
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
	
	if (rpParams.uFlags & RPF_ROMFILE) {
		
		// Check for file name buffer.
		if (rpParams.pszFileName == NULL) {
			fprintf(stderr, "Error: File name buffer not allocated.\n");
			setExitCode(&rpParams, EXIT_FAILURE);
			doExit(&rpParams);
		}
		
		// Allocate space for header.
		PGBHEAD pgbHdr;
		if ((pgbHdr = malloc(sizeof(GBHEAD))) == NULL) {
			perror("Could not allocate buffer for header.\n");
			errno = 0;
			setExitCode(&rpParams, EXIT_FAILURE);
			doExit(&rpParams);
		}
		
		// Load header.
		if (loadHeaderFromFile(rpParams.pszFileName, pgbHdr)) {
			perror("Failed to load ROM header.\n");
			errno = 0;
			setExitCode(&rpParams, EXIT_FAILURE);
			free(pgbHdr);
			doExit(&rpParams);
		}
		
		validateChksums(pgbHdr, &rpParams);
		
		// Print ROM info.
		if (!(rpParams.uFlags & RPF_NOROMINFO)) {
			printf("Using file: \"%s\"\n", rpParams.pszFileName);
			printRomInfo(pgbHdr);
		}
		
		free(pgbHdr);
		
	}
	
	doExit(&rpParams);
	
	// Exit program.
	return EXIT_SUCCESS;
	
}

void validateChksums (PGBHEAD pHdr, PRUN_PARAMS prpParams) {
	
	uint8_t uNewHdrChksum = mkGbHdrChksum(pHdr);
	// uint16_t uNewGlobalChksum;
	
	if (pHdr->uHdrChksum != uNewHdrChksum) {
		if (prpParams->uFlags & RPF_UPDATEROM) {
			pHdr->uHdrChksum = uNewHdrChksum;
		} else {
			printf("Warning: Header checksum is invalid. ROM will be unbootable! Correct value is 0x%X.\n", uNewHdrChksum);
		}
	}
	
}

// EOF
