// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2024 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
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

Line printer configuration
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

#ifdef __USE_POSTSCRIPT_PRINTING__

// --------------------------------------------------------------
// Global variable initialzation
// --------------------------------------------------------------
LinePrinterConfig lpConfigStruct;
LinePrinterConfig *lpConfig = &lpConfigStruct;
// --------------------------------------------------------------

LinePrinterConfig::LinePrinterConfig () {
#if defined (__UNIX__)
  default_lpCommand = "lp";
  default_redirectDevice = "/dev/null";
#elif defined (__WIN32__)
  default_lpCommand = "type";
  default_redirectDevice = "LPT2";
#else
#error "You must define an OS type: __UNIX__ or __WIN32__"
#endif
  default_tempFile = "lptemp.ps";
  num_prints = 0;
}

#endif // __USE_POSTSCRIPT_PRINTING__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
