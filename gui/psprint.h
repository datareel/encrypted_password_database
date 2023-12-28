// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
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

Postscript printing framework
*/
// ----------------------------------------------------------- // 
#ifndef __POSTSCRIPT_PRINT_HPP__
#define __POSTSCRIPT_PRINT_HPP__

#include "app_defs.h"

#ifdef __USE_APP_POSTSCRIPT_PRINTING__

// --------------------------------------------------------------
// Postscript include files
// --------------------------------------------------------------
#include "pscript.h"
// --------------------------------------------------------------

// --------------------------------------------------------------
// Postscript configuration
// --------------------------------------------------------------
struct PscriptPrintParms {
  PscriptPrintParms() {
    // Setup the values for user configurable PostScript parameters
    cell_len = 19;    
    font_size = 7; 
    orientation = 0;
    paper_code = PostScriptDrv::LETTER_SIZE;
    item_font = PostScriptDrv::COURIER_BOLD_OBLIQUE;
    cell_font = PostScriptDrv::COURIER;
    
    prev_cell_len = cell_len;
    prev_font_size = font_size;
    prev_orientation = orientation;
    prev_paper_code = paper_code;
    prev_item_font = item_font;
    prev_cell_font = cell_font;

    default_cell_len = 19;    
    default_font_size = 7; 
    default_orientation = 0; 
    default_paper_code = PostScriptDrv::LETTER_SIZE;
    default_item_font = PostScriptDrv::COURIER_BOLD_OBLIQUE;
    default_cell_font = PostScriptDrv::COURIER;
  }
  
  int cell_len;
  int lines_per_page;
  int lr_margin;
  double font_size;
  int orientation;
  PostScriptDrv::PSPaperSizes paper_code;
  PostScriptDrv::PSFonts item_font;
  PostScriptDrv::PSFonts cell_font;
  wxString document_name;
  wxString prev_doc_name;
  wxString default_doc_name;
  
  int prev_cell_len;
  double prev_font_size;
  int prev_orientation;
  PostScriptDrv::PSPaperSizes prev_paper_code;
  PostScriptDrv::PSFonts prev_item_font;
  PostScriptDrv::PSFonts prev_cell_font;

  int default_cell_len;
  double default_font_size;
  int default_orientation;
  PostScriptDrv::PSPaperSizes default_paper_code;
  PostScriptDrv::PSFonts default_item_font;
  PostScriptDrv::PSFonts default_cell_font;
};
// --------------------------------------------------------------

// --------------------------------------------------------------
// Globals
// --------------------------------------------------------------
extern PscriptPrintParms PSPrintSetupStruct;
extern PscriptPrintParms *PSPrintSetup;
// --------------------------------------------------------------

#endif //__USE_APP_POSTSCRIPT_PRINTING__

#endif // __POSTSCRIPT_PRINT_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
