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

*/
// ----------------------------------------------------------- // 
#ifndef __M_DATABASE_HPP__
#define __M_DATABASE_HPP__

#include "m_globals.h"

// --------------------------------------------------------------
// Constants
// --------------------------------------------------------------
// Total number of data members in the persistent object class
// that will equate to the total number of columns used by the
// database application for constructing grids.
const int NumDataMembers = 8;

// Total number of data members per panel
const int MaxPanelMembers = NumDataMembers;

// Default number of entries to use when the database is empty or
// first constructed.
const int DefaultNumEntries = 100;

// Total number of previously inserted, changed, deleted objects
const int NumPanelEntries = 255;

// Default grid column sizes
const int DefaultColSize = 200;

// Min/Max grid column sizes
const int MinColSize = 10;
const int MaxColSize = 1000;

// Font constants
const int FontElements = 8;
// (1) User defined (bool) 1 or 0 if user has set the font
// (2) wxFontEncoding (enum)
// (3) wxFontFamily (enum)
// (4) PointSize (int)
// (5) Style (int)
// (6) Underlined (bool)
// (7) wxFontWeight (enum)
// (8) Has face name (bool) 1 or 0 if the font has a face name
// Font typ stored as a string
// FaceName (string)
// Unstored font type strings
// NativeFontInfoDesc (string)

// Color constants
const int NumCustomColors = 16;
// --------------------------------------------------------------

// --------------------------------------------------------------
// Type Definitions
// --------------------------------------------------------------
// Type definition used to define the InfoHog database
// for this application
typedef DBString INFOHOG_t; // Key member type
typedef InfoHog<INFOHOG_t> INFOHOG;
typedef InfoHogKey<INFOHOG_t> INFOHOGKEY; 
// --------------------------------------------------------------

struct gxDatabaseConfig {
  gxDatabaseConfig() { Clear(); }
  ~gxDatabaseConfig() { }
  gxDatabaseConfig(const gxDatabaseConfig &ob) { Copy(ob); }
  gxDatabaseConfig &operator=(const gxDatabaseConfig &ob);

  void Copy(const gxDatabaseConfig &ob);
  int ReadConfig(gxDatabase *f);
  int Flush(gxDatabase *f);
  int TestStaticArea(gxDatabase *f);
  int WriteConfig(gxDatabase *f, int flushdb = 1);
  int WriteDBName(gxDatabase *f, const char *s);
  int WriteColSize(gxDatabase *f, int p, int value);
  int WriteColSize(gxDatabase *f);
  int WriteColName(gxDatabase *f, int p, const char *s);
  int WriteViewGridLines(gxDatabase *f);
  int WriteViewLabels(gxDatabase *f);
  int WriteViewRowNumbers(gxDatabase *f);
  int WriteAutoSize(gxDatabase *f);
  int WriteCellOverflow(gxDatabase *f);
  int WriteDisplayField(gxDatabase *f);
  unsigned SizeOf();
  
#ifdef __wxWINALL__
  // Color functions
  int WriteGridLineColor(gxDatabase *f, wxColour *color);
  int GetGridLineColor(wxColour *color);
  int WriteViewSkipLines(gxDatabase *f);
  int WriteSkipLineColor(gxDatabase *f, wxColour *color);
  int GetSkipLineColor(wxColour *color);
  int GetGridBackgroundColor(wxColour *color);
  int WriteGridBackgroundColor(gxDatabase *f, wxColour *color);
  int GetGridTextColor(wxColour *color);
  int WriteGridTextColor(gxDatabase *f, wxColour *color);
  int GetGridLabelBackgroundColor(wxColour *color);
  int WriteGridLabelBackgroundColor(gxDatabase *f, wxColour *color);
  int GetGridLabelTextColor(wxColour *color);
  int WriteGridLabelTextColor(gxDatabase *f, wxColour *color);
  int GetHyperlinkColor(wxColour *color);
  int WriteHyperlinkColor(gxDatabase *f, wxColour *color);
  int GetPrintHeaderColor(wxColour *color);
  int WritePrintHeaderColor(gxDatabase *f, wxColour *color);
  int GetPrintFooterColor(wxColour *color);
  int WritePrintFooterColor(gxDatabase *f, wxColour *color);

  // Font functions
  int GetGridLabelFont(wxFont *font);
  int WriteGridLabelFont(gxDatabase *f, wxFont *font);
  int GetGridTextFont(wxFont *font);
  int WriteGridTextFont(gxDatabase *f, wxFont *font);
  int GetHyperlinkFont(wxFont *font);
  int WriteHyperlinkFont(gxDatabase *f, wxFont *font);
  int GetPrintHeaderFont(wxFont *font);
  int WritePrintHeaderFont(gxDatabase *f, wxFont *font);
  int GetPrintFooterFont(wxFont *font);
  int WritePrintFooterFont(gxDatabase *f, wxFont *font);
#endif
  
  // Print functions
  int WritePrintField(gxDatabase *f);
  int WritePrintGridLines(gxDatabase *f);
  int WritePrintDocName(gxDatabase *f, const char *s);
  void SetPrintDocName(const char *s);
  int WritePrintDocCustomHeader(gxDatabase *f, const char *s);
  int WritePrintDocCustomFooter(gxDatabase *f, const char *s);
  int WritePrintDocTimeAndDate(gxDatabase *f);
  int WritePrintOrientation(gxDatabase *f);
  int WritePrintLeftMarginSize(gxDatabase *f);
  int WritePrintPaperSize(gxDatabase *f);
  int WritePrintPageHeader(gxDatabase *f);
  int WritePrintPageFooter(gxDatabase *f);
  int WritePrintGridLabels(gxDatabase *f);
  int WritePrintRowNumbers(gxDatabase *f);
  int WritePrintPageScale(gxDatabase *f);

  // Text functions
  char GetTextDelimiter() { return text_delimiter[0]; }
  int WriteTextDelimiter(gxDatabase *f, char dchar);

  int Validate();
  void Clear();
  void SetColName(int col, const char *s);
  void SetDatabaseName(const char *s);
  char *GetColName(int col, char *sbuf);
  long Version();

  // Internal processing functions
#ifdef __wxWINALL__
  int get_font(gxINT32 *elements, DBString *font_name, wxFont *font);
  int write_font(gxDatabase *f, gxINT32 *elements, DBString *font_name, wxFont *font);
  
#endif
  int is_string(const char *s);
  int is_string(const DBString *s) { return 1; } // DBStrings are encrypted so we always return true
  int test_color(gxINT32 &c);

  // WARNING: Do not change the order of the following data members
  // NOTE: This order must match the order of the class data members
  // or the addressing, read, write functions will not work.
  gxINT32 version_number;                    // DB config version number
  DBString database_name;                // Persistent database name
  DBString column_names[NumDataMembers]; // Persistent column name
  gxINT32 view_labels;                       // True to view lables
  gxINT32 view_row_numbers;                  // True to view rows numbers
  gxINT32 col_sizes[NumDataMembers];        // Persistent grid column sizes
  gxINT32 print_field[NumDataMembers];      // List of print fields  
  gxINT32 cell_attrib[NumDataMembers];      // Cell attributes
  gxINT32 cell_align[NumDataMembers];       // Cell alignment
  gxINT32 cell_border[NumDataMembers];      // Cell border
  gxINT32 cell_color[NumDataMembers];       // Cell color
  gxINT32 cell_fill[NumDataMembers];        // Cell fill color
  gxINT32 cell_text_color[NumDataMembers];  // Cell text color
  gxINT32 cell_text_font[NumDataMembers];   // Cell text font
  gxINT32 cell_type[NumDataMembers];        // Cell type
  gxINT32 cell_pattern[NumDataMembers];     // Cell pattern
  gxINT32 cell_protection[NumDataMembers];  // Cell protection
  gxINT32 label_align[NumDataMembers];      // Label alignment
  gxINT32 label_border[NumDataMembers];     // Label border
  gxINT32 label_color[NumDataMembers];      // Label color
  gxINT32 label_fill[NumDataMembers];       // Label fill color    
  gxINT32 label_text_font[NumDataMembers];  // Label text font
  gxINT32 label_text_color[NumDataMembers]; // Label text color
  gxINT32 label_pattern[NumDataMembers];    // Label pattern
  gxINT32 label_protection[NumDataMembers]; // Label protection
  gxINT32 display_field[NumDataMembers];    // List of display fields
  gxINT32 cell_misc[NumDataMembers];        // Cell reserved misc space
  gxINT32 label_misc[NumDataMembers];       // Label reserved misc space

  // Extended grid functions
  gxINT32 auto_size;
  gxINT32 cell_overflow;
  gxINT32 view_grid_lines; 
  gxINT32 grid_line_color;
  gxINT32 view_skip_lines;
  gxINT32 skip_line_color;
  gxINT32 grid_background_color;       // Global grid background color (all cells)
  gxINT32 grid_text_color;             // Global grid text color (all cells)
  gxINT32 grid_label_background_color; // Global grid label background color
  gxINT32 grid_label_text_color;
  gxINT32 hyperlink_color;
  gxINT32 grid_label_font[FontElements];
  DBString grid_label_font_name;
  gxINT32 grid_text_font[FontElements];
  DBString grid_text_font_name;
  gxINT32 hyperlink_font[FontElements];
  DBString hyperlink_font_name;
  
  // Extended print functions
  DBString print_doc_name;
  gxINT32 print_doc_time_and_date;
  gxINT32 print_grid_lines;
  gxINT32 print_orientation;
  gxINT32 print_left_margin_size;
  gxINT32 print_paper_size;
  gxINT32 print_page_header;
  DBString print_doc_custom_header;
  gxINT32 print_header_color;
  gxINT32 print_header_font[FontElements];
  DBString print_header_font_name;
  gxINT32 print_page_footer;
  DBString print_doc_custom_footer;
  gxINT32 print_footer_color;
  gxINT32 print_footer_font[FontElements];
  DBString print_footer_font_name;
  gxINT32 print_skip_lines;
  gxINT32 print_grid_labels;
  gxINT32 print_row_numbers;
  gxINT32 print_page_scale;

  // Cell font name array
  gxINT32 cell_text_fonts[NumDataMembers][FontElements]; 
  DBString cell_text_font_names[NumDataMembers];

  // Label font name array
  gxINT32 label_text_fonts[NumDataMembers][FontElements];
  DBString label_text_font_names[NumDataMembers];

  // Extended text functions
  char text_delimiter[4];
};

// Interactive database configuration and GUI parameters
struct gxDatabaseParms
{
  gxDatabaseParms();
  ~gxDatabaseParms() { 
    if(pod) delete pod;
    pod = 0;
  }
  
  gxString WildCard1;     // Right most character used for wild card searches
  int default_num_rows;   // Default number of grid rows for empty databases
  int column_label_size;  // Column label size
  int row_label_size;     // Row label size
  int display_status;     // True if diplaying database status messages
  gxString dbgrid_labels[NumDataMembers]; // Column labels
  gxString dbgrid_string[NumDataMembers]; // String rep of displayable members 
  gxDatabaseConfig db_config; // Database specific configuration parameters

  int current_row;       // Current grid row
  int current_col;       // Current grid column
#ifdef __wxWINALL__
  wxString current_name; // Current item name
  wxString prev_name;    // Previous cell name
#endif
  int prev_row;          // Previous grid row
  int prev_col;          // Previous grid column
  int adding_cell_keyname;   // True if adding a new object from the grid
  int changing_cell_keyname; // True if changing a key name from the grid

  // Database Interface
  POD *pod;         // Persistent object database manager for main database
  int admin_rights; // True if the database is open for read/write access
  FAU static_area;  // Allocated data file space for program variables
  int num_trees;    // Total number of trees per index file
  BtreeNodeOrder_t node_order;   // Number of keys per tree node
  gxString data_file_extension;  // File extension for all dat files
  gxString index_file_extension; // File extension for all idx files 
  gxString data_file;            // Name of the currently opened data file
  gxString index_file;           // Name of the currently opened index file
  char database_revision;        // Database revision letter
  gxString username;  // Username associated with this database
  MemoryBuffer crypt_key; // Key used to encrypt and decrypt database files
};

// Global data structures used to organize and store data file nodes addresses
extern gxList<FAU> db_search_dllist; // Doubly linked list
extern gxList<FAU> *dllist;          // Doubly linked list pointer
extern gxListNode<FAU> *dllistptr;   // Doubly linked list node pointer

// Utility functions
// Function used to read comma seperated values from data file where double quotes are used to escape values with commas
gxString *ParseCVSLine(const gxString &input_str, unsigned &num_arr, int trim_spaces = 1, int trim_quotes = 1);


#endif // __M_DATABASE_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
