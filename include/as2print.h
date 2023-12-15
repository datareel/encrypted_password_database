// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.
 
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  
USA

These classes and functions are used to print data to ASCII
text files and format data for various print functions.
*/
// ----------------------------------------------------------- //   
#ifndef __GX_ASPRINT_HPP__
#define __GX_ASPRINT_HPP__

#include "m_globals.h"
#include "gxdlcode.h"

// Constants for print functions
const int as2PrintCols = 80;      // Columns for portrait printouts
const int as2PrintColsLong = 132; // Columns for landscape printouts
const int as2PrintRows = 25;      // Rows for portrait printouts
const int as2PrintRowsLong = 25;  // Rows for landscape printouts

// Control characters
const char as2LineFeed = '\n';

// Function prototypes for standalone print functions
GXDLCODE_API void ASPrint(const char *s, DiskFileB &stream, int offset, 
			  int pos = 0, const char pad = ' ');
GXDLCODE_API void ASPrint(const char *s, const char filter, 
			  DiskFileB &stream, int offset,
			  int pos = 0, const char pad = ' ');
GXDLCODE_API void ASPrint(const char c, DiskFileB &stream, int offset, 
			  int pos = 0,
			  const char pad = ' ');
GXDLCODE_API void ASPrint(int val, DiskFileB &stream, int offset, 
			  int pos = 0, const char pad = ' ');
GXDLCODE_API void ASPrint(long val, DiskFileB &stream, int offset, 
			  int pos = 0,
			  const char pad = ' ');
GXDLCODE_API void ASPrint(double val, DiskFileB &stream, 
			  int offset, int pos = 0,
			  const char pad = ' ');

#endif // __GX_ASPRINT_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
