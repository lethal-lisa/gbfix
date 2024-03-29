/*
 * obj/runparam.c
 * 
 * GBFix - Runtime Parameters Module
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
#include <memory.h>
#include <stdlib.h>

// Include module header(s):
#include "../inc/runparam.h"

void setExitCode (PRUN_PARAMS pParams, const long int nExitCode) {
	
	// Make sure pParams is non-null.
	if (pParams == NULL) return;
	
	// Set exit flag and code.
	pParams->uFlags |= RPF_EXIT;
	pParams->nExitCode = nExitCode;
	
}

void doExit (PRUN_PARAMS pParams) {
	
	// Make sure pParams is non-null.
	if (pParams == NULL) exit(EXIT_FAILURE);
	
	// Free filename buffer.
	if (pParams->pszFileName != NULL) free(pParams->pszFileName);
	
	// Free GB header struct.
	if (pParams->pHdr != NULL) free(pParams->pHdr);
	
	// Free header updates structure.
	if (pParams->pHdrUps != NULL) free(pParams->pHdrUps);
	
	// Check for specific exit flag.
	if ((pParams->uFlags & RPF_MASK) & RPF_EXIT) exit(pParams->nExitCode);
	
	// Use generic exit.
	exit(EXIT_SUCCESS);
	
}

// EOF
