/*
 * obj/messages.c
 * 
 * GBFix - Messages Module
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
#include <stdio.h>

// Include module header(s):
#include "../inc/messages.h"

const char g_szDivider[] = "\n--[ %s ]--\n";

void printRomInfo (const PGBHEAD pgbHdr) {
	
	if (pgbHdr == NULL) {
		fprintf(stderr, "Error: Bad header info structure.\n");
		return;
	}
	
	printf(g_szDivider, "ROM Info");
	
	printf("\nDMG/SGB Format Title:\n");
	printf("\tTitle:\t\t\t\"%s\"\n\n", pgbHdr->title.szTitle);
	
	printf("CGB Format Title:\n");
	printf("\tTitle:\t\t\t\"%s\"\n", pgbHdr->title.newTitle.szTitle);
	printf("\tManufacturer:\t\t\"%s\"\n", pgbHdr->title.newTitle.strManufacturer);
	printf("\tCGB Flags:\t\t0x%X\n\n", pgbHdr->title.newTitle.uCgbFlag);
	
	printf("\tLicensee Code:\t\t0x%X (%s type)\n", getLicenseeCode(pgbHdr), getLicenseeTypeStr(pgbHdr));
	printf("\tROM Size:\t\t%ldkB (%ldB)\n", getRomSize(pgbHdr), getRomSize(pgbHdr) * 1024);
	printf("\tRegion:\t\t\t%s (0x%X)\n", getRegionStr(pgbHdr), pgbHdr->uRegion);
	printf("\tROM Version:\t\t0x%X\n", pgbHdr->uRomVer);
	printf("\tHeader Checksum:\t0x%X\n", pgbHdr->uHdrChksum);
	printf("\tGlobal Checksum:\t0x%X\n", correctGlobalChksum(pgbHdr));
	
	printf("\n");
	
}

// Show help message.
void printHelp () {
	
	printf(g_szDivider, "Help");
	printf("\t-h, --help                Show this help.\n");
	printf("\t    --gpl                 Show the GNU GPL3 notice.\n");
	printf("\t-f, --file <FILE>         Set file to use to <FILE>.\n");
	printf("\t-v, --verbose             Enable verbose mode.\n");
	printf("\t-d, --dry-run             Don't make changes, only show what changes would be made.\n");
	printf("\t    --norominfo           Don't show ROM information.\n");
	printf(g_szDivider, "ROM Manipulation");
	printf("\t-r, --region <REGION>     Set ROM region to <REGION>.\n");
	printf("\t-s, --sgbflags <FLAGS>    Set SGB (Super GameBoy) flags to <FLAGS>.\n");
	printf("\t-V, --romver <VER>        Set ROM version to <VER>.\n");
	printf("\t-t, --title <TITLE>       Set ROM title to <TITLE>.\n");
	printf("\t-m, --manufacturer <MANU> Set ROM manufacturer code to <MANU>. Only available on \"CGB\" type ROMs.\n");
	printf("\t-c, --cgbflags <CGBFLAGS> Set CGB flags to <CGBFLAGS>. Only available on \"CGB\" type ROMs.\n");
	printf("\t-C, --carttype <CART>     Set cart type to <CART>.\n");
	printf("\t-R, --ramsize <SIZE>      Set save RAM size to <SIZE>.\n");
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
