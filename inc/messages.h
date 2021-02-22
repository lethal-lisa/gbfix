/*
 * inc/messages.h
 * 
 * GBFix - Messages Module Header
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

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include "gbhead.h"

// Declare external variables and constants:
extern const char g_szDivider[];

// Declare functions:
void printRomInfo (const PGBHEAD pgbHdr);

void printGplNotice ();
void printHelp ();

#endif /* _MESSAGES_H_ */
