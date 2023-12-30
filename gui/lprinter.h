// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2024 DataReel Software Development
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

Line printer configuration
*/
// ----------------------------------------------------------- // 
#ifndef __LINE_PRINTER_HPP__
#define __LINE_PRINTER_HPP__

#include "app_defs.h"

#ifdef __USE_POSTSCRIPT_PRINTING__

// --------------------------------------------------------------
// Line printer include files
// --------------------------------------------------------------

// --------------------------------------------------------------

// --------------------------------------------------------------
// Line printer configuration
// --------------------------------------------------------------
struct LinePrinterConfig {
  LinePrinterConfig ();
  ~LinePrinterConfig () { }

  wxString lpCommand;      // Line printer command
  wxString redirectDevice; // Redirect device
  wxString tempFile;       // Temporary file 
  int num_prints;          // Number of file printed
  
  wxString prev_lpCommand;
  wxString prev_redirectDevice;
  wxString prev_tempFile;

  wxString default_lpCommand;
  wxString default_redirectDevice;
  wxString default_tempFile;
};
// --------------------------------------------------------------

// --------------------------------------------------------------
// Globals
// --------------------------------------------------------------
extern LinePrinterConfig lpConfigStruct;
extern LinePrinterConfig *lpConfig;
// --------------------------------------------------------------

#endif //__USE_POSTSCRIPT_PRINTING__

#endif // __LINE_PRINTER_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
