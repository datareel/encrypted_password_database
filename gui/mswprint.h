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

Microsoft Windows printing framework
*/
// ----------------------------------------------------------- // 
#ifndef __MSW_PRINT_HPP__
#define __MSW_PRINT_HPP__

#include "app_defs.h"

// Define this directive to use the wxWindows printing features under
// Microsoft Windows. A separate PostScript driver is provided for
// UNIX systems.
#ifdef __USE_MSW_PRINTING__

#if !wxUSE_PRINTING_ARCHITECTURE
#error You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h to compile
#endif

// --------------------------------------------------------------
// wxWindows include files
// --------------------------------------------------------------
#include "wx/metafile.h"
#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/accel.h"
// --------------------------------------------------------------

// --------------------------------------------------------------
// Constants
// --------------------------------------------------------------
// wxWindows MSW Printer defaults
// NOTE: Left/Right and Top/Bottom magrins are in logical units (20 per inch). 
const int mswpLINES_PER_PAGE = 39;    // Default lines per page
const int mswpFONT_SIZE = 9;          // Default font size
const int mswpLEFT_MARGIN = 10;       // Default for .5" left margin
const int mswpRIGHT_MARGIN = 10;      // Default for .5" right margin
const int mswpORIENTATION = 0;        // Default to Landscape
const int mswpFONTCODE = 0;           // Default font code: 0 = wxSWISS
const int mswpPAPERCODE = 0;          // Default paper code: 0 = wxPAPER_LETTER
const int mswpTOP_MARGIN = 20;        // 1" top margin with space for page header
const int mswpBOTTOM_MARGIN = 20;     // 1" bottom margin with space for page number
const int mswpTOP_MARGIN_NO_HEADER = 10;   // .5" top margin no space for page header
const int mswpBOTTOM_MARGIN_NO_FOOTER = 10;// .5" bottom margin no space for page number
const int mswpLINE_WIDTH = 2;         // Line width for separator lines
const int mswpPRINTABLE_OFFSET_X = 5; // Printable page area x offset
const int mswpPRINTABLE_OFFSET_Y = 5; // Printable page area y offset
const double mswpCELL_SCALE = .7;     // Scale factor for adjusting cell sizes
const int mswpHEADER_FONT_SIZE = 10;  // Default page header font size 
const int mswpFOOTER_FONT_SIZE = 10;  // Default page footer font size 
// --------------------------------------------------------------

// --------------------------------------------------------------
// Print configuration
// --------------------------------------------------------------
// Data structure use to hold various parameters for MSW printing
struct MSWPrintingParameters
{
  MSWPrintingParameters();
  ~MSWPrintingParameters() { }

  void SetPaperSize();
  void SetOrientation();
  
  // Database printing parameters
  int print_list;        // True if printing a preformatted list
  int print_all_entries; // Ture if printing database entries
  int print_all_cols;    // True if pringting all database columns
  
  // Global print data, to remember settings during the session
  wxPrintData print_data;

  // Database grid line page formating
  int draw_grid_lines;     // True if drawing grid lines
  int lineto_titlebar;     // Draw grid lines to end of title bar
  int lineto_right_margin; // Draw grid lines to end of right margin
  int lineto_endof_grid;   // Draw grid lines to the end of screen grid

  // Current print settings
  int lines_per_page;  // Number of line per page
  int font_size;       // Font or point size fot the object's data
  int orientation;     // 0 == Landscape, 1 == Portrait
  float left_margin;   // Left magrin in logical units (20 per inch) 
  float right_margin;  // Right magrin in logical units (20 per inch) 
  float top_margin;    // Top magrin in logical units (20 per inch)
  float bottom_margin; // Bottom magrin in logical units (20 per inch)
  double cell_scale;   // Value used to scale cells to the screen DPI
  int font_code;       // Font code family must match the FontChoiceStrings array:
                       // 0 = wxSWISS
                       // 1 = wxROMAN
                       // 2 = wxDECORATIVE
                       // 3 = wxMODERN 
                       // 4 = wxSCRIPT
                       // 5 = wxDEFAULT

  int paper_code;      // Paper codes must match the PaperChoiceStrings array
                       // 0 = wxPAPER_LETTER
                       // 1 = wxPAPER_LEGAL
                       // 2 = wxPAPER_TABLOID
                       // 3 = wxPAPER_A3
                       // 4 = wxPAPER_A4
};
// --------------------------------------------------------------

class mswPrintout: public wxPrintout
{
public:
  mswPrintout(const wxString &title);
  mswPrintout();
  
public: // Override virtual functions in base class 
  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void OnPreparePrinting();
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom,
		   int *selPageTo);

public: // Helper functions
  int InitPrintout();
  int InitPrintPreview();
  void SetAllFonts();
  void SetFont();
  void SetHeaderFont();
  void SetFooterFont();
  void SetItemBarFont();
  void ScaleDC(wxDC *dc);
  void PrintText(wxDC *dc, wxString &s, int max_length,
		 float xpos, float ypos);
  void PrintText(wxDC *dc, wxString &s, float xpos, float ypos);
  void PrintText(wxDC *dc, char *s, int max_length, float xpos, float ypos);
  void PrintText(wxDC *dc, char *s, float xpos, float ypos);
  void PrintText(wxDC *dc, char c, float xpos, float ypos);  
  void PrintText(wxDC *dc, int i, float xpos, float ypos);
  void PrintText(wxDC *dc, int i, int max_length, float xpos, float ypos);
  void PrintText(wxDC *dc, long i, float xpos, float ypos);
  void PrintText(wxDC *dc, long i,  int max_length, float xpos, float ypos);
  void PrintText(wxDC *dc, double i, float xpos, float ypos);
  void PrintText(wxDC *dc, double i, int max_length, float xpos, float ypos);
  void PrintText(wxDC *dc, float i, float xpos, float ypos);
  void PrintText(wxDC *dc, float i, int max_length, float xpos, float ypos);
  void DrawTextPage(wxDC *dc, int page);
  void WritePageHeader(wxDC *dc, wxString &doc_name, wxString &doc_date);
  void WritePageNumber(wxDC *dc, int pagenum);

private:
  CryptDBDocument *child_frame; // Pointer to the current child frame
  gxDatabaseConfig *db_config;  // Pointer to the child's database config
  MSWPrintingParameters *print_config; // Pointer to the child's printer config 

private: // Font dependent members
  // Text font for page header and page numbers
  wxFont printer_font;
  wxFont label_font;
  wxFont header_font;
  wxFont footer_font;

  float char_width;    // Width of each character based on the font type

private: // Page dependent members
  int page_width;  // Page width in millimetres
  int page_height; // Page height in millimetres
  int last_page;   // Last page in this document
  int first_page;  // First page in this document
  int num_lines;   // Number of text lines in this document
  float start_x;   // Starting x position 
  float start_y;   // Starting y position 

private: // Device Context/Page scaling members
  float logUnitsFactor; // Factor for converting millimetres to logical units
  float scale;          // Factor for printout/screen scaling.
  float overallScale;   // Overall scaling factor
  int ppiScreenX;       // Logical pixels per inch of screen
  int ppiScreenY;       // Logical pixels per inch of screen
  int ppiPrinterX;      // Logical pixels per inch of screen
  int ppiPrinterY;      // Logical pixels per inch of screen
};

#endif // __USE_MSW_PRINTING__

#endif // __MSW_PRINT_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
