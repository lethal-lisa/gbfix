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
const char g_szAppVer[] = "0.2-proto";
const char g_szDivider[] = "\n--[ %s ]--\n";

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
			
			if (rpParams.uFlags & RPF_EXIT) break;
			
			// Reset long option index.
			iLongOpt = 0;
			
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
				{ 0, 0, 0, 0}
			};
			
			// Get options.
			if ((nOpt = getopt_long(argc, argv, "hf:vr:s:V:", optLongOpts, &iLongOpt)) == -1) {
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
				
			case 'r':
				// Set ROM region.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_REGION;
				hdrUpdates.hdr.uRegion = (unsigned char)strtoul(optarg, NULL, 0);
				break;
				
			case 's':
				// Set SGB flags.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_SGBF;
				hdrUpdates.hdr.uSgbFlag = (unsigned char)strtoul(optarg, NULL, 0);
				break;
				
			case 'V':
				// Set ROM version.
				rpParams.uFlags |= RPF_UPDATEROM;
				
				hdrUpdates.uFlags |= UPF_ROMVER;
				hdrUpdates.hdr.uRomVer = (unsigned char)strtoul(optarg, NULL, 0);
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
		
		// Print verbose mode information.
		if (rpParams.uFlags & RPF_VERBOSE) {
			printf("\n");
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

void printRomInfo (const PGBHEAD pgbHdr) {
	
	if (pgbHdr == NULL) {
		fprintf(stderr, "Bad header info structure.\n");
		return;
	}
	
	printf("ROM Info:\n\n");
	printf("\tLicensee Code:\t\t0x%X (%s type)\n", getLicenseeCode(pgbHdr), getLicenseeTypeStr(pgbHdr));
	printf("\tROM Size:\t\t%ldkB (%ldB)\n", getRomSize(pgbHdr), getRomSize(pgbHdr) * 1024);
	printf("\tRegion:\t\t\t%s (0x%X)\n", getRegionStr(pgbHdr), pgbHdr->uRegion);
	printf("\tROM Version:\t\t0x%X\n", pgbHdr->uRomVer);
	printf("\tHeader Checksum:\t0x%X\n", pgbHdr->uHdrChkSum);
	printf("\tGlobal Checksum:\t0x%X\n", correctGlobalChkSum(pgbHdr));
	
	printf("\n");
	
}

// Show help message.
void printHelp () {
	
	printf("Help");
	printf(g_szDivider, "General Operation");
	printf("\t-h, --help         Show this help.\n");
	printf("\t    --gpl          Show the GNU GPL3 notice.\n");
	printf("\t-f, --file <file>  Set file to use to <file>.\n");
	printf("\t-v, --verbose      Enable verbose mode.\n");
	printf("\t-d, --dry-run      Don't make changes, only show what changes would be made.\n");
	printf("\t    --norominfo    Don't show ROM information.\n");
	printf(g_szDivider, "ROM Manipulation");
	printf("\t-r, --region <region>  Set ROM region to <region>.\n");
	printf("\t-s, --sgbflags <flags> Set SGB (Super GameBoy) flags to <flags>.\n");
	printf("\t-V, --romver <ver>     Set ROM version to <ver>.\n");
	printf("\n");
	
}

// Regurgitate the GPL3 notice.
void printGplNotice () {
	
	printf("This program is free software; you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation; either version 2 of the License, or\n\
(at your option) any later version.\n\
\n");
	printf("This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n");
	printf("You should have received a copy of the GNU General Public License\n\
along with this program; if not, write to the Free Software\n\
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,\n\
MA 02110-1301, USA.\n\n");
	
}

// EOF
