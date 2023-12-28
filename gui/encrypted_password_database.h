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

wxWindow 2.X and 3.X application frame work.
*/
// ----------------------------------------------------------- // 
#ifndef __WX_APP_FRAMEWORK_HPP__
#define __WX_APP_FRAMEWORK_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// External project components
// --------------------------------------------------------------
#include "m_globals.h"
// --------------------------------------------------------------

// --------------------------------------------------------------
// Include files
// --------------------------------------------------------------
#include "wx_incs.h"
// --------------------------------------------------------------

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
const int MAIN_FRAME_ID     = 724;
const int MAIN_SPLITWIN_ID  = 725;
const int MAIN_TEXTWIN_ID   = 726;
const int MAIN_STATUSWIN_ID = 727;
const int CHILD_FRAME_ID    = 728;
// --------------------------------------------------------------

// --------------------------------------------------------------
// Toolbar settings and IDs
// --------------------------------------------------------------
static const long MAIN_TOOLBAR_STYLE = wxTB_FLAT|wxTB_DOCKABLE|wxTB_TEXT;
const int MAIN_TOOLBAR_ID =  450;
const int MAIN_TOOLBAR_COMBO_ID = 451;
// --------------------------------------------------------------

// --------------------------------------------------------------
// // Define new frame types for wxWindow frames and sub-frames
// --------------------------------------------------------------
class MainFrame;            // Main window frame
class wxappfxProgramConfig; // Program's configurable parameters
class CryptDBDocument;
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for the controls and the menu commands
// --------------------------------------------------------------
enum {
  // Application framework function event handler IDs
  WXAPPFW_APP = 0,
  WXAPPFW_APP_EXIT, // Exit this application

  // File function event handler IDs
  WXAPPFW_FILE,
  WXAPPFW_FILE_BACKUP,        // Make a backup copy of the open file 
  WXAPPFW_FILE_CLOSE,         // Close the open file
  WXAPPFW_FILE_COMPARE,       // Compare a file for error verification
  WXAPPFW_FILE_EXPORT,        // Export the open file to another format
  WXAPPFW_FILE_IMPORT,        // Import a formated file into this file
  WXAPPFW_FILE_MERGE,         // Merge a file of the same format 
  WXAPPFW_FILE_NEW,           // Create a new file
  WXAPPFW_FILE_OPEN,          // Open an existing file
  WXAPPFW_FILE_PROPERTIES,    // Display the properties of this file
  WXAPPFW_FILE_REBUILD,       // Rebuild a damaged or corrupt file  
  WXAPPFW_FILE_REBUILDCONFIG, // Rebuild a damaged or corrupt config file  
  WXAPPFW_FILE_SAVE,          // Save all changes to this file
  WXAPPFW_FILE_SAVEAS,        // Save this file as (or rename)
  WXAPPFW_FILE_TEMPLATE,      // Create an import/export template
  
  // Edit function event handler IDs
  WXAPPFW_EDIT,
  WXAPPFW_EDIT_CUT,         // Cut data and paste to clipboard
  WXAPPFW_EDIT_COPY,        // Copy data to clipboard
  WXAPPFW_EDIT_PASTE,       // Paste data from clipboard
  WXAPPFW_EDIT_PREFERENCES, // Edit user preferences
  WXAPPFW_EDIT_CLEAR,       // Clear text control
  WXAPPFW_EDIT_UNDO,        // Undo last edit
  WXAPPFW_EDIT_REDO,        // Redo last edit

  // Database function event handler IDs
  WXAPPFW_DATABASE,
  WXAPPFW_DATABASE_ADD,         // Add an object to the database
  WXAPPFW_DATABASE_CHANGE,      // Change an existing object in the database
  WXAPPFW_DATABASE_DELETE,      // Delete an object in the database
  WXAPPFW_DATABASE_DISPLAY,     // Display the database
  WXAPPFW_DATABASE_FIND,        // Search for an object in the database
  WXAPPFW_DATABASE_YES2B,       // Toggle Y/N options for an object
  WXAPPFW_DATABASE_YES2BALL,    // Toggle Y/N options for all objects
  WXAPPFW_DATABASE_YES2BNONE,   // Reset Y/N for all objects
  WXAPPFW_DATABASE_YES2BTOTAL,  // Display Y/N totals
  
  // MSW Printing function event handler IDs
  WXAPPFW_PRINT,
  WXAPPFW_PRINT_MSWPRINT,     // Print using the wxWindows interface
  WXAPPFW_PRINT_MSWSETUP,     // wxWindows printer setup
  WXAPPFW_PRINT_MSWPAGESETUP, // wxWindows page setup
  WXAPPFW_PRINT_MSWPREVIEW,   // wxWindows print preview

  // Postscript Printing function event handler IDs
  WXAPPFW_PRINT_PSPRINT,      // Print to postscript printer
  WXAPPFW_PRINT_PSSETUP,      // Postscript printer setup
  WXAPPFW_PRINT_PSPAGESETUP,  // Postscript printer setup
  WXAPPFW_PRINT_PSPREVIEW,    // Postscript printer preview
  WXAPPFW_PRINT_LPFILE,       // Send formatted file to line printer
  WXAPPFW_PRINT_LPSETUP,      // Line printer setup

  // Text Printing function event handler IDs
  WXAPPFW_PRINT_ASPRINT,      // Print to ASCII text file
  WXAPPFW_PRINT_ASSETUP,      // ASCII text file printer setup
  WXAPPFW_PRINT_ASPAGESETUP,  // ASCII text file page setup
  WXAPPFW_PRINT_ASPREVIEW,    // ASCII text file preview

  // HTML Printing function event handler IDs
  WXAPPFW_PRINT_HTMPRINT,     // Print to HTML file
  WXAPPFW_PRINT_HTMSETUP,     // HTML printer setup
  WXAPPFW_PRINT_HTMPAGESETUP, // HTML page setup
  WXAPPFW_PRINT_HTMPREVIEW,   // HTML file preview
  
  // View function event handler IDs
  WXAPPFW_VIEW,
  WXAPPFW_VIEW_CLEAR,        // Clear text window, cavas, or grid
  WXAPPFW_VIEW_ROWLABELS,    // View grid row labels
  WXAPPFW_VIEW_COLLABELS,    // View grid column labels
  WXAPPFW_VIEW_SPLITHOR,     // Split the frame horizontally
  WXAPPFW_VIEW_SPLITVER,     // Split the frame vertically
  WXAPPFW_VIEW_UNSPLIT,      // Remove the split
  WXAPPFW_VIEW_TOOLBAR,      // Toggle the tool bar
  WXAPPFW_VIEW_TOOLBAROR,    // Toggle the tool bar orientation
  WXAPPFW_VIEW_STATUS,       // View the status window/frame
  WXAPPFW_VIEW_CLOSE,        // Close the status window

  // Extended view function event handler IDs
  WXAPPFW_VIEW_CONFIG,       // View the program configuration
  WXAPPFW_VIEW_ACCESSRIGHTS, // View the users access rights
  WXAPPFW_VIEW_HISTORY,      // View the program's history file
  WXAPPFX_VIEW_CLEARHISTORY, // Clear the history file
  WXAPPFX_VIEW_LOGFILE,      // View the program's log file
  WXAPPFX_VIEW_CLEARLOGFILE, // Clear the log file
  
  // Extended view functions for database grids
  WXAPPFW_VIEW_AUTOSIZE,
  WXAPPFW_VIEW_DISABLE_AUTOSIZE,
  WXAPPFW_VIEW_CELLOVERFLOW,
  WXAPPFW_VIEW_GRIDLABELBACKGROUNDCOLOR,
  WXAPPFW_VIEW_GRIDLABELFONT,
  WXAPPFW_VIEW_GRIDLABELTEXTCOLOR,
  WXAPPFW_VIEW_GRIDLINES,
  WXAPPFW_VIEW_GRIDLINECOLOR,
  WXAPPFW_VIEW_GRIDBACKGROUNDCOLOR,
  WXAPPFW_VIEW_GRIDTEXTCOLOR,
  WXAPPFW_VIEW_GRIDTEXTFONT,
  WXAPPFW_VIEW_HYPERLINKCOLOR,
  WXAPPFW_VIEW_HYPERLINKFONT,
  WXAPPFW_VIEW_SKIPLINES,
  WXAPPFW_VIEW_SKIPLINECOLOR,
  WXAPPFW_VIEW_TOGGLEGRIDLINES,

  // Help function event handler IDs
  WXAPPFW_HELP,
  WXAPPFW_HELP_CONTACT,  // Display author contact information
  WXAPPFW_HELP_HOMEPAGE, // Display this program's WWW home page 
  WXAPPFW_HELP_ABOUT,    // Display information about this program 
  WXAPPFW_HELP_ONLINE,   // Invoke online help

  // Extended help function event handler IDs
  WXAPPFW_HELP_GUIVER,  // Display GUI library information
  WXAPPFW_HELP_DBVER,   // Display database library information
  WXAPPFW_HELP_SOCKVER, // Display socket library information
  WXAPPFW_HELP_THRVER,  // Display thread library information

  // MDI (Multiple Document Interface) event handler IDs
  WXAPPFW_MDI_CHILD_QUIT,
  WXAPPFW_MDI_REFRESH,
  WXAPPFW_MDI_CHANGE_TITLE,
  WXAPPFW_MDI_CHANGE_POSITION,
  WXAPPFW_MDI_CHANGE_SIZE,

  // Authentication event handler IDs
  WXAPPFW_AUTH,

  // Crypto event handler IDs
  WXAPPFW_CRYPTO,

  // Document event handler IDs
  WXAPPFW_DOC,

  // Font event handler IDs
  WXAPPFW_FONT,

  // Grid event handler IDs
  WXAPPFW_GRID,

  // Image event handler IDs
  WXAPPFW_IMAGE,

  // Socket event handler IDs
  WXAPPFW_SOCKET,
  WXAPPFW_SOCKET_DOWNLOAD,
  WXAPPFW_SOCKET_UPLOAD,

  // String event handler IDs
  WXAPPFW_STRING,

  // Thread event handler IDs
  WXAPPFW_THREAD,

  // Application event hander ID
  WXAPPFW_APP1_HANDLER,
  WXAPPFW_APP2_HANDLER,
  WXAPPFW_APP3_HANDLER,
  WXAPPFW_APP4_HANDLER,
  WXAPPFW_APP5_HANDLER,

  // User name event hander ID
  WXAPPFW_USERS,
  WXAPPFW_USERS_ADD,
  WXAPPFW_USERS_REMOVE,
  WXAPPFW_USERS_LIST,

  // Extra features
  WXAPPFW_CLEAR_CLIPBOARD
};
// --------------------------------------------------------------

// --------------------------------------------------------------
// Standalone Functions
// --------------------------------------------------------------
struct gxDatabaseConfig;

void InitProgramConfig(int argc, char *argv[]);
void wxStrStripChar(wxString &s, char c);
void FixHomeDirID(gxString &dir);
int CheckEnvVar(const char *var_name, gxString &var_value);
int LoadCustomColors(wxColourData *data);
int SaveCustomColors(wxColourData *retData);
int IsHyperlink(const gxString &s);

// Database helper functions
POD *OpenDatabase(CryptDBDocument *child_frame, gxString &fname, POD *pod, 
		  gxDatabaseConfig &db_config,
		  gxString &data_ext, gxString &index_ext,
		  int display_errors = 0, int set_admin_rights = 1);
int RebuildIndexFile(gxString &index_file, POD *pod, 
		     int echo_results = 0, int reload_grid = 1);

// InfoHog list helper functions
int NumListEntries();
int BuildVirtualDB(CryptDBDocument *child_frame);

// Database statistics functions
int TestDatabaseForRunningThreads();
int TestDatabase(int test_num_entries, int test_admin_rights, 
		 int test_threads);
void EchoDBError(POD *pod, wxTextCtrl *statusWin);
int CheckError(gxDatabase *f, wxTextCtrl *statusWin);
int CheckError(gxDatabaseError err, wxTextCtrl *statusWin);
void DisplayDataFileStats(POD *pod, wxTextCtrl *statusWin);
void DisplayIndexFileStats(POD *pod, wxTextCtrl *statusWin);
unsigned BtreeSearch(gxBtree *btx, int item, POD *pod,
		     INFOHOG_t &ob, int find_all = 0); 
void BtreeKeySearch(INFOHOGKEY &key, int item, POD *pod,
		    INFOHOG_t &ob, unsigned &objects_found, int find_all);
// --------------------------------------------------------------

// Application and framework include files
#include "mswprint.h"   // Microsoft Windows printing framework 
#include "a_panel.h"    // Add panel
#include "c_panel.h"    // Change panel
#include "d_panel.h"    // Display panel
#include "e_panel.h"    // Edit panel
#include "f_panel.h"    // Find panel
#include "g_thread.h"   // GUI theads
#include "lprinter.h"   // Line printer configuration
#include "lp_panel.h"   // Line printer setup panel
#include "mciplay.h"    // Multi-media players
#include "mswpage.h"    // Page setup panel
#include "m_doc.h"      // Document framework
#include "m_grid.h"     // Grid framework
#include "pspage.h"     // Postscript page setup panel
#include "psprint.h"    // Postscript printing framework
#include "startdoc.h"   // Open known file types
#include "st_frame.h"   // Status frame
#include "st_panel.h"   // Status panel
#include "winapp.h"     // wxWindows application class
#include "new_panel.h"      // New DB panel
#include "open_panel.h"     // Open DB panel
#include "add_user_panel.h" // Add user access panel
#include "p_config.h"       // Program configuration data
#include "m_frame.h"        // Main frame for this application

#endif // __WX_APP_FRAMEWORK__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
