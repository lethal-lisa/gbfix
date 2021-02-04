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
const char g_szAppVer[] = "0.0-proto";

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
						printGplNotice();
						setExitCode(&rpParams, EXIT_SUCCESS);
						break;
						
					default:
						fprintf(stderr, "Unsupported option: %s (%o)\n", optLongOpts[iLongOpt].name, iLongOpt);
						
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
				rpParams.pszFileName = malloc(strlen(optarg) * sizeof(char));
				rpParams.cchFileName = strlen(rpParams.pszFileName);
				strcpy(rpParams.pszFileName, optarg);
				rpParams.uFlags |= RPF_ROMFILE;
				break;
				
			case 'v':
				// Set verbose mode.
				printf("Using verbose mode.\n");
				break;
				
			case '?':
				// Unknown command.
				rpParams.uFlags |= RPF_UNKNOWNPARAM;
				break;
				
			default:
				// Unknown getopt_long return value.
				fprintf(stderr, "Error: getopt_long returned unknown value (0x%x).\n", nOpt);
				setExitCode(&rpParams, EXIT_FAILURE);
			}
		}
	} else {
		fprintf(stderr, "No options specified.\nUse %s -h to see options.\n", argv[0]);
		setExitCode(&rpParams, EXIT_FAILURE);
	}
	
	if (rpParams.uFlags & RPF_ROMFILE) {
		
		printf("Using file: %s\n", rpParams.pszFileName);
		PGBHEAD pgbHdr;
		if ((pgbHdr = loadHeaderFromFile(rpParams.pszFileName)) == NULL) exit(EXIT_FAILURE);
		
		printf("%s ROM file size: %ldkB\n", rpParams.pszFileName, getRomSize(pgbHdr));
		
		free(pgbHdr);
		free(rpParams.pszFileName);
		
	}
	
	if (rpParams.uFlags & RPF_EXIT) exit(rpParams.nExitCode);
	
	// Exit program.
	return EXIT_SUCCESS;
	
}

inline void setExitCode (PRUN_PARAMS pParams, const long int nExitCode) {
	
	if (pParams == NULL) return;
	
	pParams->uFlags |= RPF_EXIT;
	pParams->nExitCode = nExitCode;
	
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
