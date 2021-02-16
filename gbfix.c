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

#include "gbfix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

const char g_szAppName[] = "GBFix";
const char g_szAppVer[] = "0.1-proto";

int main (int argc, char* argv[]) {
	
	printf("%s v%s\n\n", g_szAppName, g_szAppVer);
	
	RUN_PARAMS rpParams;
	memset(&rpParams, 0, sizeof(RUN_PARAMS));
	
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
				{ 0, 0, 0, 0}
			};
			
			// Get options.
			if ((nOpt = getopt_long(argc, argv, "hf:v", optLongOpts, &iLongOpt)) == -1) {
				setExitCode(&rpParams, EXIT_SUCCESS);
				break;
			}
			
			// Process options.
			switch (nOpt) {
			case 0:
				// Process long options.
				if (iLongOpt > 0) {
					switch (iLongOpt) {
					case 1:
						// Print out GPL notice.
						printGplNotice();
						setExitCode(&rpParams, EXIT_SUCCESS);
						break;
						
					default:
						// Handle unsupported long option.
						fprintf(stderr, "Error: Unsupported long option: %s (%d)\n", optLongOpts[iLongOpt].name, iLongOpt);
						setExitCode(&rpParams, EXIT_FAILURE);
						
					}
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
				rpParams.pszFileName = malloc(strlen(optarg) * sizeof(char));
				if (rpParams.pszFileName == NULL) {
					perror("Could not allocate buffer for file name.\n");
					setExitCode(&rpParams, EXIT_FAILURE);
					break;
				}
				
				// Get length of new string buffer.
				if ((rpParams.cchFileName = strlen(rpParams.pszFileName)) <= 0) {
					fprintf(stderr, "Error: File name buffer is of an invalid length (%lo chars).", rpParams.cchFileName);
					setExitCode(&rpParams, EXIT_FAILURE);
					break;
				}
				
				// Copy into string buffer.
				strcpy(rpParams.pszFileName, optarg);
				
				// Update flags.
				rpParams.uFlags |= RPF_ROMFILE;
				
				break;
				
			case 'v':
				// Set verbose mode.
				rpParams.uFlags |= RPF_VERBOSE;
				printf("Using verbose mode.\n");
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
			printf("Using file: %s\n", rpParams.pszFileName);
			printf("\n");
		}
		
		// Load header.
		PGBHEAD pgbHdr = loadHeaderFromFile(rpParams.pszFileName);
		
		// Print ROM info.
		printRomInfo(pgbHdr);
		
		if (pgbHdr != NULL) free(pgbHdr);
		
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
	printf("\tLicensee Code:\t\t0x%X", getLicenseeCode(pgbHdr));
	if (isNewLicensee(pgbHdr)) {
		printf(" (New type)\n");
	} else {
		printf(" (Old type)\n");
	}
	printf("\tROM Size:\t\t%ldkB (%ldB)\n", getRomSize(pgbHdr), getRomSize(pgbHdr) * 1024);
	printf("\tRegion Code:\t\t0x%X\n", pgbHdr->uRegion);
	printf("\tROM Version:\t\t0x%X\n", pgbHdr->uRomVer);
	printf("\tHeader Checksum:\t0x%X\n", pgbHdr->uHdrChkSum);
	printf("\tGlobal Checksum:\t0x%X\n", pgbHdr->uGlobalChkSum);
	
	printf("\n");
	
}

// Show help message.
void printHelp () {
	
	printf("Help\n");
	printf("\t-h/--help\t\tShow this help.\n");
	printf("\t--gpl\t\t\tShow the GNU GPL3 notice.\n");
	printf("\t-v/--verbose\t\tEnable verbose mode.\n");
	printf("\t-f/--file <file>\tSet file to use to <file>.\n");
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
