// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 07/23/2024
// Copyright (c) 2001-2025 DataReel Software Development
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

Main frame for this application.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

// --------------------------------------------------------------
// event tables and other macros for wxWindows
// --------------------------------------------------------------
// The event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MainFrame, wxMDIParentFrame)
  // Frame event handlers
  EVT_CLOSE(MainFrame::OnCloseWindow)

  // File Menu
  EVT_MENU(WXAPPFW_APP_EXIT,  MainFrame::OnExit)
  EVT_MENU(WXAPPFW_FILE_NEW, MainFrame::OnNew)
  EVT_MENU(WXAPPFW_FILE_OPEN, MainFrame::OnOpen)
  EVT_MENU(WXAPPFW_FILE_IMPORT, MainFrame::OnImport)
  EVT_MENU(WXAPPFW_FILE_EXPORT, MainFrame::OnExport)
  EVT_MENU(WXAPPFW_FILE_TEMPLATE, MainFrame::OnTemplate)
  EVT_MENU(WXAPPFW_FILE_BACKUP, MainFrame::OnBackup)
  EVT_MENU(WXAPPFW_FILE_MERGE, MainFrame::OnMerge)
  EVT_MENU(WXAPPFW_FILE_COMPARE, MainFrame::OnCompare)
  EVT_MENU(WXAPPFW_FILE_REBUILD, MainFrame::OnRebuild)
  EVT_MENU(WXAPPFW_FILE_REBUILDCONFIG, MainFrame::OnRebuildConfig)
  EVT_MENU(WXAPPFW_FILE_PROPERTIES, MainFrame::OnStats)

  // Edit menu
#ifdef __USE_CLIPBOARD_FUNCTIONS__
  EVT_MENU(WXAPPFW_EDIT_COPY, MainFrame::OnCopy)
  EVT_MENU(WXAPPFW_EDIT_CUT, MainFrame::OnCut)
  EVT_MENU(WXAPPFW_EDIT_PASTE, MainFrame::OnPaste)
  EVT_MENU(WXAPPFW_CLEAR_CLIPBOARD, MainFrame::OnClearClipboard)
#endif
  EVT_MENU(WXAPPFW_EDIT_PREFERENCES, MainFrame::OnPreferences)

  // 10/25/2026: Clipboard clearing, idle auto-lock and password masking
  EVT_TIMER(WXAPPFW_CLIPBOARD_TIMER, MainFrame::OnClipboardTimer)
  EVT_TIMER(WXAPPFW_IDLE_TIMER, MainFrame::OnIdleTimer)
  EVT_MENU(WXAPPFW_VIEW_SHOW_PASSWORDS, MainFrame::OnToggleShowPasswords)
  EVT_TIMER(WXAPPFW_RESEAL_TIMER, MainFrame::OnResealTimer)
  EVT_MENU(WXAPPFW_FILE_UPGRADE, MainFrame::OnUpgrade)

  // Database menu
  EVT_MENU(WXAPPFW_DATABASE_ADD, MainFrame::OnAddObject)
  EVT_MENU(WXAPPFW_DATABASE_DELETE, MainFrame::OnDeleteObject)
  EVT_MENU(WXAPPFW_DATABASE_CHANGE, MainFrame::OnChangeObject)
  EVT_MENU(WXAPPFW_DATABASE_FIND, MainFrame::OnFindObject)
  EVT_MENU(WXAPPFW_DATABASE_DISPLAY, MainFrame::OnDisplay)

  // Print menu
#if defined (__USE_APP_TEXT_PRINTING__)
  EVT_MENU(WXAPPFW_PRINT_ASPRINT, MainFrame::OnASPrint)
#endif

#ifdef __USE_APP_HTM_PRINTING__
  EVT_MENU(WXAPPFW_PRINT_HTMPRINT, MainFrame::OnHTMPrint)
#endif
  
#ifdef __USE_MSW_PRINTING__
  EVT_MENU(WXAPPFW_PRINT_MSWPRINT, MainFrame::OnMSWPrint)
  EVT_MENU(WXAPPFW_PRINT_MSWPREVIEW, MainFrame::OnMSWPrintPreview)
  EVT_MENU(WXAPPFW_PRINT_MSWPAGESETUP, MainFrame::OnMSWPrintPageSetup)
  EVT_MENU(WXAPPFW_PRINT_MSWSETUP, MainFrame::OnMSWPrintSetup)
#endif

#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  EVT_MENU(WXAPPFW_PRINT_PSPRINT, MainFrame::OnPSPrint)
  EVT_MENU(WXAPPFW_PRINT_PSPAGESETUP, MainFrame::OnPSPrintPageSetup)
  EVT_MENU(WXAPPFW_PRINT_PSSETUP, MainFrame::OnPSPrintSetup)
#endif

  // View menu
  EVT_MENU(WXAPPFW_VIEW_CLEAR, MainFrame::OnClear)
  EVT_MENU(WXAPPFW_VIEW_STATUS, MainFrame::OnStatus)
  EVT_MENU(WXAPPFW_VIEW_CLOSE, MainFrame::OnCloseStatusWin)
  EVT_MENU(WXAPPFW_VIEW_ROWLABELS, MainFrame::ToggleRowLabels)
  EVT_MENU(WXAPPFW_VIEW_COLLABELS, MainFrame::ToggleColLabels)
  EVT_MENU(WXAPPFW_VIEW_TOOLBAR, MainFrame::OnToggleToolbar)
  EVT_MENU(WXAPPFW_VIEW_AUTOSIZE, MainFrame::AutoSize)
  EVT_MENU(WXAPPFW_VIEW_DISABLE_AUTOSIZE, MainFrame::DisableAutoSize)
  EVT_MENU(WXAPPFW_VIEW_CELLOVERFLOW, MainFrame::CellOverflow)
  EVT_MENU(WXAPPFW_VIEW_GRIDLINES, MainFrame::ToggleGridLines)
  EVT_MENU(WXAPPFW_VIEW_GRIDLINECOLOR, MainFrame::SetGridLineColor)
  EVT_MENU(WXAPPFW_VIEW_GRIDBACKGROUNDCOLOR, MainFrame::SetGridBackgroundColor)
  EVT_MENU(WXAPPFW_VIEW_GRIDTEXTCOLOR, MainFrame::SetGridTextColor)
  EVT_MENU(WXAPPFW_VIEW_SKIPLINES, MainFrame::ViewSkipLines)
  EVT_MENU(WXAPPFW_VIEW_SKIPLINECOLOR, MainFrame::SetSkipLineColor)
  EVT_MENU(WXAPPFW_VIEW_GRIDLABELBACKGROUNDCOLOR, MainFrame::SetGridLabelBackgroundColor)
  EVT_MENU(WXAPPFW_VIEW_GRIDLABELTEXTCOLOR, MainFrame::SetGridLabelTextColor)
  EVT_MENU(WXAPPFW_VIEW_HYPERLINKCOLOR, MainFrame::SetHyperlinkColor)
  EVT_MENU(WXAPPFW_VIEW_GRIDLABELFONT, MainFrame::SetGridLabelFont)
  EVT_MENU(WXAPPFW_VIEW_GRIDTEXTFONT, MainFrame::SetGridTextFont)
  EVT_MENU(WXAPPFW_VIEW_HYPERLINKFONT, MainFrame::SetHyperlinkFont)

  // Help Menu
  EVT_MENU(WXAPPFW_HELP_ABOUT, MainFrame::OnAbout)

  // User Menu 
  EVT_MENU(WXAPPFW_USERS_ADD, MainFrame::OnAddUsers)
  EVT_MENU(WXAPPFW_USERS_REMOVE, MainFrame::OnRemoveUsers)
  EVT_MENU(WXAPPFW_USERS_LIST, MainFrame::OnListUsers)

  // 10/27/2026: Undo/redo and the Security menu
  EVT_MENU(WXAPPFW_EDIT_UNDO, MainFrame::OnUndo)
  EVT_MENU(WXAPPFW_EDIT_REDO, MainFrame::OnRedo)
  EVT_UPDATE_UI(WXAPPFW_EDIT_UNDO, MainFrame::OnUpdateUndo)
  EVT_UPDATE_UI(WXAPPFW_EDIT_REDO, MainFrame::OnUpdateRedo)
  EVT_MENU(WXAPPFW_SECURITY_ACCESS_LOG, MainFrame::OnAccessLog)
  EVT_MENU(WXAPPFW_SECURITY_IN_FILE_LOG, MainFrame::OnInFileLog)
  EVT_MENU(WXAPPFW_SECURITY_SELF_DESTRUCT, MainFrame::OnSelfDestruct)
  EVT_MENU(WXAPPFW_SECURITY_SHOW_LOG, MainFrame::OnShowAccessLog)
  EVT_MENU(WXAPPFW_SECURITY_VERIFY_LOG, MainFrame::OnVerifyAccessLog)
  // 10/27/2026: Phase 3 main window
  EVT_MENU(WXAPPFW_FILE_LOCK, MainFrame::OnLock)
  EVT_MENU(WXAPPFW_RECORD_COPY_USER, MainFrame::OnCopyUser)
  EVT_MENU(WXAPPFW_RECORD_COPY_PASSWORD, MainFrame::OnCopyPassword)
  EVT_MENU(WXAPPFW_RECORD_OPEN_LINK, MainFrame::OnOpenLink)
  EVT_MENU(WXAPPFW_EDIT_FIND_FOCUS, MainFrame::OnFindFocus)
  EVT_MENU(WXAPPFW_VIEW_DETAILS, MainFrame::OnToggleDetails)
  EVT_MENU(WXAPPFW_TOOLS_CMDBUILD, MainFrame::OnCommandBuilder)
  EVT_MENU(WXAPPFW_TOOLS_GENERATE, MainFrame::OnGeneratePassword)
  EVT_TIMER(WXAPPFW_STATUS_TIMER, MainFrame::OnStatusTimer)
  EVT_UPDATE_UI(WXAPPFW_RECORD_COPY_USER, MainFrame::OnUpdateNeedsRecord)
  EVT_UPDATE_UI(WXAPPFW_RECORD_COPY_PASSWORD, MainFrame::OnUpdateNeedsRecord)
  EVT_UPDATE_UI(WXAPPFW_RECORD_OPEN_LINK, MainFrame::OnUpdateNeedsRecord)
  EVT_UPDATE_UI(WXAPPFW_DATABASE_CHANGE, MainFrame::OnUpdateNeedsRecord)
  EVT_UPDATE_UI(WXAPPFW_DATABASE_DELETE, MainFrame::OnUpdateNeedsRecord)
  EVT_UPDATE_UI(WXAPPFW_DATABASE_ADD, MainFrame::OnUpdateNeedsDatabase)
  EVT_UPDATE_UI(WXAPPFW_FILE_LOCK, MainFrame::OnUpdateNeedsDatabase)
  EVT_UPDATE_UI(WXAPPFW_PRINT_MSWPRINT, MainFrame::OnUpdateNeedsDatabase)

  EVT_MENU(WXAPPFW_HELP_ONLINE, MainFrame::OnOpenOnlineHelp)

END_EVENT_TABLE()
// --------------------------------------------------------------

// --------------------------------------------------------------
// Main Frame
// --------------------------------------------------------------
MainFrame::MainFrame(wxWindow *parent,
		     const wxWindowID id,
		     const wxString& title,
		     const wxPoint& pos,
		     const wxSize& size,
		     const long style)
  : wxMDIParentFrame(parent, id, title, pos, size,
		     style | wxNO_FULL_REPAINT_ON_RESIZE)
{
  // Set the frame icon
  SetIcons(EPDBAppIcons()); // 10/27/2026
  SetTitle(progcfg->ProgramName.c_str());

  // 10/25/2026: Timers that clear copied values from the clipboard and
  // close open databases when the program is idle
  status_timer = 0;
  clipboard_timer = new wxTimer(this, WXAPPFW_CLIPBOARD_TIMER);
  idle_timer = new wxTimer(this, WXAPPFW_IDLE_TIMER);
  last_activity = time(0);
  if(progcfg->auto_lock_minutes > 0) idle_timer->Start(15000);
  reseal_timer = new wxTimer(this, WXAPPFW_RESEAL_TIMER);
  reseal_timer->Start(2000);
  
  // 10/27/2026: Phase 3 menus. Every earlier item and accelerator is
  // kept; the Database menu is now Records and the Print items are in
  // the File menu. Icons are SVGs drawn at the screen DPI.
  wxMenu *file_menu = new wxMenu;
  MenuItem(file_menu, WXAPPFW_FILE_NEW, "&New Database...\tCtrl+N", "Create a new database", "new_db");
  MenuItem(file_menu, WXAPPFW_FILE_OPEN, "&Open Database...\tCtrl+O", "Unlock and open an existing database", "open");
  MenuItem(file_menu, WXAPPFW_FILE_LOCK, "&Lock\tCtrl+Shift+L", "Close every open database and wipe the keys", "lock");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_IMPORT, "&Import CSV File...", "Import a CSV file into this database");
  file_menu->Append(WXAPPFW_FILE_EXPORT, "&Export To CSV File...", "Export this database to a CSV file");
  file_menu->Append(WXAPPFW_FILE_TEMPLATE, "Create CSV &Template...", "Create an import/export CSV template");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_BACKUP, "&Backup Database...", "Create a backup copy of this database");
  file_menu->Append(WXAPPFW_FILE_MERGE, "&Merge Database...", "Merge an existing database with this database");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_COMPARE, "Te&st Database", "Test the current database");
  file_menu->Append(WXAPPFW_FILE_REBUILD, "&Rebuild Database", "Rebuild the current database");
  file_menu->Append(WXAPPFW_FILE_REBUILDCONFIG, "Rebuild Database &Config", "Rebuild the current database config header");
  file_menu->Append(WXAPPFW_FILE_UPGRADE, "&Upgrade Database Format...",
		    "Upgrade the current database to format v2 with stronger encryption");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_PROPERTIES, "View Database Stat&s", "Display the current database statistics");
  file_menu->AppendSeparator();
#ifdef __USE_MSW_PRINTING__
  MenuItem(file_menu, WXAPPFW_PRINT_MSWPRINT, "&Print...\tCtrl+P", "Print the database to a selected printer", "print");
  file_menu->Append(WXAPPFW_PRINT_MSWPREVIEW, "Print Pre&view", "Preview the document before printing");
  file_menu->Append(WXAPPFW_PRINT_MSWPAGESETUP, "Pa&ge Setup...", "Setup the page options");
  file_menu->Append(WXAPPFW_PRINT_MSWSETUP, "Printer Setu&p...", "Setup the printer options");
#endif
#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  file_menu->Append(WXAPPFW_PRINT_PSPRINT, "Print to Postscript Printer", "Print the database to a postscript printer");
  file_menu->Append(WXAPPFW_PRINT_PSPAGESETUP, "Postscript Page Setup", "Setup the postscript page options");
  file_menu->Append(WXAPPFW_PRINT_PSSETUP, "Line Printer Setup", "Setup postscript line printer options");
#endif
#if defined (__USE_APP_TEXT_PRINTING__)
  file_menu->Append(WXAPPFW_PRINT_ASPRINT, "Print to text file", "Print the database to a text file");
#endif
#ifdef __USE_APP_HTM_PRINTING__
  file_menu->Append(WXAPPFW_PRINT_HTMPRINT, "Print to HTML file", "Print the database to an HTML file");
#endif
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_APP_EXIT, "E&xit\tCtrl+Q", "Exit this program");

  // Edit menu. 10/27/2026: Ctrl+Shift+Z also redoes.
  wxMenu *edit_menu = new wxMenu;
  MenuItem(edit_menu, WXAPPFW_EDIT_UNDO, "&Undo\tCtrl+Z", "Undo the last record change", "undo");
  MenuItem(edit_menu, WXAPPFW_EDIT_REDO, "&Redo\tCtrl+Y", "Redo the last undone record change", "redo");
  edit_menu->AppendSeparator();
#ifdef __USE_CLIPBOARD_FUNCTIONS__
  MenuItem(edit_menu, WXAPPFW_EDIT_COPY, "&Copy Cell\tCtrl+C", "Copy cell to clipboard", "copy");
  edit_menu->Append(WXAPPFW_EDIT_CUT, "Cu&t Cell\tCtrl+X", "Cut cell and copy to clipboard");
  edit_menu->Append(WXAPPFW_EDIT_PASTE, "&Paste Cell\tCtrl+V", "Paste cell from clipboard");
  edit_menu->AppendSeparator();
  edit_menu->Append(WXAPPFW_CLEAR_CLIPBOARD, "C&lear Clipboard\tCtrl+L", "Clear all text from the clipboard");
  edit_menu->AppendSeparator();
#endif
  MenuItem(edit_menu, WXAPPFW_EDIT_FIND_FOCUS, "&Search Records\tCtrl+F", "Move to the search box", "find");
  edit_menu->AppendSeparator();
  MenuItem(edit_menu, WXAPPFW_EDIT_PREFERENCES, "Pr&eferences...", "Edit user preferences", "preferences");

  // Records menu (was Database)
  wxMenu *database_menu = new wxMenu;
  MenuItem(database_menu, WXAPPFW_DATABASE_FIND, "&Find...\tAlt+F", "Find an object in the database", "find");
  database_menu->AppendSeparator();
  MenuItem(database_menu, WXAPPFW_DATABASE_ADD, "&Add Record...\tAlt+A", "Add a record to the database", "add_record");
  MenuItem(database_menu, WXAPPFW_DATABASE_CHANGE, "&Edit Record...\tAlt+C", "Change the selected record", "edit");
  MenuItem(database_menu, WXAPPFW_DATABASE_DELETE, "&Delete Record\tAlt+D", "Delete the selected record", "delete");
  database_menu->AppendSeparator();
  MenuItem(database_menu, WXAPPFW_RECORD_COPY_USER, "Copy &Username\tCtrl+Shift+U", "Copy the username of the selected record", "copy_user");
  MenuItem(database_menu, WXAPPFW_RECORD_COPY_PASSWORD, "Copy &Password\tCtrl+Shift+C", "Copy the password of the selected record", "password");
  MenuItem(database_menu, WXAPPFW_RECORD_OPEN_LINK, "Open &Website Link\tCtrl+Shift+O", "Open the website link of the selected record", "open_link");
  database_menu->AppendSeparator();
  database_menu->Append(WXAPPFW_DATABASE_DISPLAY, "Dis&play Records...", "Show the records one at a time");

  // View menu
  wxMenu *view_menu = new wxMenu;
  view_menu->AppendCheckItem(WXAPPFW_VIEW_DETAILS, "Record &Details Pane", "Show the details of the selected record");
  view_menu->AppendCheckItem(WXAPPFW_VIEW_TOOLBAR, "&Toolbar", "Toggle the toolbar on and off");
  view_menu->AppendCheckItem(WXAPPFW_VIEW_SHOW_PASSWORDS, "Show &passwords", "Show the password columns in clear text");
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_STATUS, "View &Status Window", "View the status window");
  view_menu->Append(WXAPPFW_VIEW_CLOSE, "Close Status Window", "Close the status window");
  view_menu->Append(WXAPPFW_VIEW_CLEAR, "Clear Status Window", "Clear the status window");
  view_menu->AppendSeparator();
  wxMenu *grid_menu = new wxMenu;
  grid_menu->Append(WXAPPFW_VIEW_ROWLABELS, "View Row Labels", "View the database row labels");
  grid_menu->Append(WXAPPFW_VIEW_COLLABELS, "View Column Labels", "View the database column labels");
  grid_menu->Append(WXAPPFW_VIEW_AUTOSIZE, "Auto size cells", "Auto size cells to fit content");
  grid_menu->Append(WXAPPFW_VIEW_DISABLE_AUTOSIZE, "Disable auto size", "Disable cell auto size when database is opened");
  grid_menu->Append(WXAPPFW_VIEW_CELLOVERFLOW, "Toggle cell overflow", "Allow cell content to overflow to next cell");
  grid_menu->AppendSeparator();
  grid_menu->Append(WXAPPFW_VIEW_GRIDLINES, "Toggle grid lines", "View database grid lines");
  grid_menu->Append(WXAPPFW_VIEW_GRIDLINECOLOR, "Grid line color...", "Set the grid line color lines");
  grid_menu->Append(WXAPPFW_VIEW_GRIDBACKGROUNDCOLOR, "Grid background color...", "Set the grid background color for all cells");
  grid_menu->Append(WXAPPFW_VIEW_GRIDLABELBACKGROUNDCOLOR, "Grid label background color...", "Set the grid label background color");
  grid_menu->Append(WXAPPFW_VIEW_GRIDLABELTEXTCOLOR, "Grid label text color...", "Set the grid label text color");
  grid_menu->Append(WXAPPFW_VIEW_GRIDLABELFONT, "Grid label font...", "Set the grid label font");
  grid_menu->AppendSeparator();
  grid_menu->Append(WXAPPFW_VIEW_GRIDTEXTCOLOR, "Grid text color...", "Set the grid text color for all cells");
  grid_menu->Append(WXAPPFW_VIEW_GRIDTEXTFONT, "Grid text font...", "Set the grid text font for all cells");
  grid_menu->AppendSeparator();
  grid_menu->Append(WXAPPFW_VIEW_SKIPLINES, "Toggle skip lines", "View database skip lines");
  grid_menu->Append(WXAPPFW_VIEW_SKIPLINECOLOR, "Skip line color...", "Set the skip line color lines");
  grid_menu->AppendSeparator();
  grid_menu->Append(WXAPPFW_VIEW_HYPERLINKCOLOR, "Hyperlink text color...", "Set the hyperlink text color for all cells");
  grid_menu->Append(WXAPPFW_VIEW_HYPERLINKFONT, "Hyperlink text font...", "Set the hyperlink text font for all cells");
  view_menu->AppendSubMenu(grid_menu, "&Grid Appearance");

  wxMenu *users_menu = new wxMenu;
#ifdef __ENABLE_SMART_CARD__
  MenuItem(users_menu, WXAPPFW_USERS_ADD, "&Add User...", "Add user access with a public RSA key or a smart card cert", "users");
#else
  MenuItem(users_menu, WXAPPFW_USERS_ADD, "&Add User...", "Add user access with a public RSA key", "users");
#endif // __ENABLE_SMART_CARD__
  users_menu->Append(WXAPPFW_USERS_REMOVE, "&Remove User...", "Remove the access of a user");
  users_menu->Append(WXAPPFW_USERS_LIST, "&List Users", "Display the users with access");

  // 10/27/2026: Security menu
  wxMenu *security_menu = new wxMenu;
  security_menu->AppendCheckItem(WXAPPFW_SECURITY_ACCESS_LOG, "&Access Log",
				 "Log opens and changes to the per-user access log");
  security_menu->Append(WXAPPFW_SECURITY_IN_FILE_LOG, "&In-File Access Log On/Off...",
			"Keep the last opens and changes, encrypted, in the open database file");
  security_menu->Append(WXAPPFW_SECURITY_SHOW_LOG, "&Show Access Log", "List the access log entries of the open database");
  security_menu->Append(WXAPPFW_SECURITY_VERIFY_LOG, "&Verify Access Log", "Check the access logs of the open database");
  security_menu->AppendSeparator();
  MenuItem(security_menu, WXAPPFW_SECURITY_SELF_DESTRUCT, "Self-&Destruct Settings...",
	   "Destroy the records of the open database after too many failed opens", "security");

  // 10/27/2026: Tools menu
  wxMenu *tools_menu = new wxMenu;
  MenuItem(tools_menu, WXAPPFW_TOOLS_CMDBUILD, "&Command Builder...",
	   "Build epdb_auth_tool command lines for scripts and automation", "terminal");
  MenuItem(tools_menu, WXAPPFW_TOOLS_GENERATE, "&Generate Password...", "Make a random password", "generate");

  wxMenu *help_menu = new wxMenu;
  MenuItem(help_menu, WXAPPFW_HELP_ONLINE, "Online &Help", "Open the online help in the web browser", "help");
  help_menu->Append(WXAPPFW_HELP_ABOUT, "&About\tF1");

  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(edit_menu, "&Edit");
  menu_bar->Append(database_menu, "&Records");
  menu_bar->Append(view_menu, "&View");
  menu_bar->Append(users_menu, "&Users");
  menu_bar->Append(security_menu, "&Security");
  menu_bar->Append(tools_menu, "&Tools");
  menu_bar->Append(help_menu, "&Help");
  SetMenuBar(menu_bar);
  GetMenuBar()->Check(WXAPPFW_SECURITY_ACCESS_LOG, progcfg->access_log != 0);
  GetMenuBar()->Check(WXAPPFW_VIEW_DETAILS, progcfg->show_details != 0);
  GetMenuBar()->Check(WXAPPFW_VIEW_SHOW_PASSWORDS, progcfg->show_passwords != 0);

  // 10/27/2026: Ctrl+Shift+Z also redoes
  wxAcceleratorEntry accel_entries[1];
  accel_entries[0].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'Z', WXAPPFW_EDIT_REDO);
  wxAcceleratorTable accel(1, accel_entries);
  SetAcceleratorTable(accel);

  // Set the menu defaults
  if(progcfg->ViewToolBar == 1) {
    GetMenuBar()->Check(WXAPPFW_VIEW_TOOLBAR, TRUE);
  }
  else {
    GetMenuBar()->Check(WXAPPFW_VIEW_TOOLBAR, FALSE);
  }

  // Panel, label and text fonts. 10/27/2026: The system GUI font, the
  // status window uses a monospace font of the same size.
  labelFont = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  itemFont = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  textWindowFont = new wxFont(EPDBMonoFont());
  hyperlinkFont = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  gridlineColor = new wxColour(0, 0, 0);
  cellFontColor = new wxColour(0, 0, 0);
  textreadonlyColor = new wxColour(105, 105, 105);
  hyperlinkColor = new wxColour(0, 0, 255);

#if wxUSE_STATUSBAR
  // 10/27/2026: Message, how the database was unlocked, records, format
  // and the auto-lock countdown
  CreateStatusBar(5);
  int widths[5] = { -3, -3, FromDIP(110), FromDIP(80), FromDIP(150) };
  GetStatusBar()->SetStatusWidths(5, widths);
  SetStatusText(progcfg->ProgramName.c_str());
  status_timer = new wxTimer(this, WXAPPFW_STATUS_TIMER);
  status_timer->Start(1000);
#endif // wxUSE_STATUSBAR

  sframe = InitStatusFrame(this, textWindowFont);
  InitStatusWin();

  spanel = InitStatusPanel(this);
  apanel = InitAddPanel(this);
  cpanel = InitChangePanel(this);
  dpanel = InitDisplayPanel(this);

  fpanel = InitFindPanel(this);
  new_db_panel = InitNewDatabasePanel(this);

  add_user_panel = InitAdduserPanel(this);
    
#ifdef __APP_DEBUG_VERSION__
  debug_log << "InitEditTabPanel()" << "\n" << flush;
#endif
  e_tab_panel = InitEditTabPanel(this);

  unlock_dlg = new UnlockDialog(this);

#ifdef __USE_MSW_PRINTING__
  mswpage = InitMSWPagePanel(this);
#endif

#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  pspage = InitPSPagePanel(this);
  lppanel = InitLinePrinterPanel(this);
#endif

  // Create the toolbar
  m_tbar = 0;
  m_smallToolbar = FALSE;
  m_horzToolbar = TRUE;
  m_rows = 1;
  m_nPrint = 1;

  if(progcfg->ViewToolBar == 1) {
    RecreateToolbar();
  }
  Bind(wxEVT_DPI_CHANGED, &MainFrame::OnDPIChanged, this);
}

wxMenuItem *MainFrame::MenuItem(wxMenu *menu, int id, const wxString &text, const wxString &help, const char *icon)
// 10/27/2026: A menu item with an SVG icon
{
  wxMenuItem *item = new wxMenuItem(menu, id, text, help);
  if(icon) item->SetBitmap(EPDBIcon(icon, EPDB_MENU_ICON_DIP));
  menu->Append(item);
  return item;
}

void MainFrame::OnDPIChanged(wxDPIChangedEvent &event)
// 10/27/2026: Moved to a monitor with another DPI: the bitmap bundles
// redraw themselves, the status bar widths are in DIPs
{
  event.Skip();
  if(GetStatusBar()) {
    int widths[5] = { -3, -3, FromDIP(110), FromDIP(80), FromDIP(150) };
    GetStatusBar()->SetStatusWidths(5, widths);
  }
}

void MainFrame::InitStatusWin()
{
  statusWin = sframe->status_win;
  *(statusWin) << "Database Engine: Idle" << "\n";

#ifdef __APP_DEBUG_VERSION__
  *(statusWin) << "\n";
  *(statusWin) << "Debug output:" << "\n";
  *(statusWin) << "ARG0 = " << progcfg->arg0_dir.c_str() << "\n";
  *(statusWin) << "Working DIR = " << progcfg->workDir.c_str() << "\n";
  *(statusWin) << "Home DIR = " << progcfg->homeDir.c_str() << "\n";
  *(statusWin) << "Doc DIR = " << progcfg->docDir.c_str() << "\n";
  *(statusWin) << "Data DIR = " << progcfg->dataDir.c_str() << "\n";
  *(statusWin) << "Log DIR = " << progcfg->logDir.c_str() << "\n";
  *(statusWin) << "Default log file = " << progcfg->default_logFile.c_str() << "\n";
  *(statusWin) << "Log file = " << progcfg->logFile.c_str() << "\n";
  *(statusWin) << "Default Config file = " << progcfg->default_cfgFile.c_str() << "\n";
  *(statusWin) << "Config file = " << progcfg->cfgFile.c_str() << "\n";
  *(statusWin) << "Default history file = " << progcfg->historyFile.c_str() << "\n";
  *(statusWin) << "History file = " << progcfg->historyFile.c_str() << "\n";
  *(statusWin) << "ENV setting = " << progcfg->envSetting.c_str() << "\n";
  *(statusWin) << "\n";
  
#if defined (__WIN32__)
  // Windows environmental variable list used by this application
  gxString app_data;
  CheckEnvVar("APPDATA", app_data);
  gxString home_share;
  CheckEnvVar("HOMESHARE", home_share);
  gxString home_drive;
  CheckEnvVar("HOMEDRIVE", home_drive);
  gxString home_path;
  CheckEnvVar("HOMEPATH", home_path);
  gxString system_drive;
  CheckEnvVar("SystemDrive", system_drive);
  gxString user_profile;
  CheckEnvVar("USERPROFILE", user_profile);

  *(statusWin) << "Debug output:" << "\n";
  *(statusWin) << "Env setting for APPDATA = " << app_data.c_str() << "\n";
  *(statusWin) << "Env setting for HOMESHARE = " << home_share.c_str() << "\n";
  *(statusWin) << "Env setting for HOMEDRIVE = " << home_drive.c_str() << "\n";
  *(statusWin) << "Env setting for HOMEPATH = " << home_path.c_str() << "\n";
  *(statusWin) << "Env setting for SystemDrive = " << system_drive.c_str() << "\n";
  *(statusWin) << "Env setting for USERPROFILE = " << user_profile.c_str() << "\n";
  *(statusWin) << "\n";
#endif // __WIN32__

#endif // __APP_DEBUG_VERSION__

}

MainFrame *GetMainFrame(void)
{
  return frame;
}

// --------------------------------------------------------------
// Event handlers
// --------------------------------------------------------------
void MainFrame::OnCloseWindow(wxCloseEvent& event)
// Define the behaviour for the frame closing. This event handler
// is called when the user has tried to close a a frame or dialog
// box using the window manager (X) or system menu (Windows). Must
// delete all frames except for the main one. 
{
  /* 
  // TODO: If grid is dirty save the changes to the cell being edited
  // NOTE: Do not need this code in this application 
  // because there are no files to save.
  if(event.CanVeto() && (progcfg->num_child_frames > 0)) {
    wxString msg;
    msg.Printf("%d windows still open, close anyhow?", 
	       progcfg->num_child_frames);
    if(wxMessageBox(msg, "Please confirm",
		    wxICON_QUESTION | wxYES_NO) != wxYES) {
      event.Veto();
      return;
    }
  }
  // event.Skip();
  */

  if(dpanel->PanelIsOpen()) {
    dpanel->Show(FALSE);
  }

  // 10/27/2026: Keep the window and details pane sizes, in DIPs
  if(!IsMaximized() && !IsIconized()) {
    wxSize sz = ToDIP(GetSize());
    GUISaveConfigValue("MainWindowWidthDIP", wxString::Format("%d", sz.x).utf8_str());
    GUISaveConfigValue("MainWindowHeightDIP", wxString::Format("%d", sz.y).utf8_str());
  }
  for(gxListNode<CryptDBDocument *> *p = progcfg->child_frame_list.GetHead(); p; p = p->next) p->data->SaveSplitterPosition();
  GUISaveConfigValue("DetailsPaneWidthDIP", wxString::Format("%d", progcfg->details_width_dip).utf8_str());
  if(status_timer) {
    status_timer->Stop();
    delete status_timer;
    status_timer = 0;
  }

  // 10/25/2026: Stop the timers, remove a value this program copied from
  // the clipboard and wipe the database keys
  if(clipboard_timer) {
    clipboard_timer->Stop();
    delete clipboard_timer;
    clipboard_timer = 0;
  }
  if(idle_timer) {
    idle_timer->Stop();
    delete idle_timer;
    idle_timer = 0;
  }
  if(reseal_timer) {
    reseal_timer->Stop();
    delete reseal_timer;
    reseal_timer = 0;
  }
  ClearClipboardIfOurs();
  DBStringConfig::crypt_key.Clear(1);
  if(progcfg) progcfg->global_dbparms.crypt_key.Clear(1);

  // Clean up heap space
  if(labelFont) delete labelFont;
  if(itemFont) delete itemFont;
  if(textWindowFont) delete textWindowFont;
  if(textreadonlyColor) delete textreadonlyColor;
  if(hyperlinkFont) delete hyperlinkFont;
  if(hyperlinkColor) delete hyperlinkColor;
  if(cellFontColor) delete cellFontColor;
  if(progcfg) delete progcfg;

  if(sframe) sframe->Destroy();

#ifdef __USE_MSW_PRINTING__
  if(mswpage) mswpage->Destroy();
#endif

#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  if(pspage) pspage->Destroy();
  if(lppanel) lppanel->Destroy();
#endif

  // Destroy the main window
  Destroy();
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
  // TRUE is to force the frame to close
  Close(TRUE);
}

void MainFrame::OnOpenOnlineHelp(wxCommandEvent& WXUNUSED(event))
{
  LaunchURL(progcfg->online_help_page, this, 0); // 10/25/2026: Default browser
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
// 10/27/2026: wxAboutBox with the program icon, the same build,
// encryption and smart card information as before
{
  wxAboutDialogInfo info;
  info.SetName(progcfg->ProgramName.c_str());
  info.SetVersion(progcfg->VersionString.c_str());
  gxString d;
  d << "Build: " << progcfg->build_type << ", released " << progcfg->release_date << "\n"
    << "Designed for " << progcfg->Platform << "\n"
    << "GUI framework: " << (const char *)wxGetLibraryVersionInfo().GetVersionString().utf8_str() << "\n\n"
    << "Encryption engine: " << OPENSSL_VERSION_TEXT << "\n";
  char vbuf[32];
  snprintf(vbuf, sizeof(vbuf), "0x%lx", (unsigned long)OPENSSL_VERSION_NUMBER);
  d << "OpenSSL version number: " << vbuf << "\n";
#ifdef __ENABLE_SMART_CARD__
  d << "\nSmart card enabled for " << (SC_uses_engine() ? SC_get_default_engine_ID() : "PKCS#11") << "\n"
    << "Smart card module: " << progcfg->SC_modulePath.c_str();
#else
  d << "\nSmart card support is not enabled";
#endif
  info.SetDescription(wxString::FromUTF8(d.c_str()));
  info.SetCopyright(wxString::FromUTF8((gxString("Copyright (c) ") + progcfg->produced_by + " " + progcfg->copyright_dates).c_str()));
  info.SetWebSite(progcfg->default_url.c_str());
  info.SetLicence("GNU Lesser General Public License, version 3.0 or later");
  info.SetIcon(wxIcon(EPDBAppIconBundle(64).GetIcon(FromDIP(wxSize(64, 64)))));
  wxAboutBox(info, this);
}

void MainFrame::OnAddUsers(wxCommandEvent& event)
{
  gxString sbuf;
  DatabaseUserAuth db_auth;
  int rv;
  
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  add_user_panel->ShowPanel();

  if(add_user_panel->IsOK() && !GUIConfirmClearUndo(child_frame, "Adding a user")) return;
  if(add_user_panel->IsOK()) {
    if(add_user_panel->use_rsa_key) {
      db_auth.f = child_frame->DBParms()->pod->OpenDataFile();
      rv = db_auth.AddRSAKeyToStaticArea(child_frame->DBParms()->crypt_key,
					 add_user_panel->public_key, add_user_panel->public_key_len, add_user_panel->rsa_key_username);
      if(rv != 0) {
	sbuf << clear << "ERROR: Cannot add public RSA key " << db_auth.err.c_str();
	ProgramError->Message(sbuf.c_str());
	return;
      }

      sbuf << clear << "Added RSA key access for user " << add_user_panel->rsa_key_username;
      ProgramError->Message(sbuf.c_str());
      GUILog(child_frame, "add-user", gxString(), 1, add_user_panel->rsa_key_username.c_str());
    }
#ifdef __ENABLE_SMART_CARD__
    if(add_user_panel->use_smartcard) {
      db_auth.f = child_frame->DBParms()->pod->OpenDataFile();
       rv = db_auth.AddSmartCardCertToStaticArea(&add_user_panel->sc, add_user_panel->use_cert_file,
						 child_frame->DBParms()->crypt_key, add_user_panel->smartcard_username);
       if(rv != 0) {
	 sbuf << clear << "ERROR: Cannot add smart card cert " << db_auth.err.c_str();
	 ProgramError->Message(sbuf.c_str());
	 return;
       }
       sbuf << clear << "Added smart card access for user " << add_user_panel->smartcard_username; // 10/25/2026: Was rsa_key_username
      ProgramError->Message(sbuf.c_str());
      GUILog(child_frame, "add-user", gxString(), 1, add_user_panel->smartcard_username.c_str());
    }
#endif // __ENABLE_SMART_CARD__
    
  }
  
  return;
}

void MainFrame::OnRemoveUsers(wxCommandEvent& event)
// 10/27/2026: Through the core (EPDBRemoveUserInPlace()): the v2 database
// MAC is updated, the last RSA or smart card user is only removed after
// opening with the password or key file, and the change is logged.
{
  gxString sbuf;
  DatabaseUserAuth db_auth;
  
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  db_auth.f = child_frame->DBParms()->pod->OpenDataFile();

  frame->SetStatusText("Checking database file for authorized users...");
  if(db_auth.LoadStaticDataBlocks() != 0) {
    sbuf << clear << db_auth.err.c_str();
    ProgramError->Message(sbuf.c_str());
    return;
  }

  gxListNode<StaticDataBlock> *list_ptr = db_auth.static_block_list.GetHead();
  if(!list_ptr) {
#ifdef __ENABLE_SMART_CARD__
    ProgramError->Message("No authorized RSA or smartcard users found in encrypted DB file");
#else
    ProgramError->Message("No authorized RSA users found in encrypted DB file");
#endif // __ENABLE_SMART_CARD__
    return;
  }

  // 10/27/2026: Pick the user from the list of the database's users
  wxArrayString choices;
  std::vector<gxString> names;
  for(list_ptr = db_auth.static_block_list.GetHead(); list_ptr; list_ptr = list_ptr->next) {
    int t = list_ptr->data.block_header.block_type;
    if(t != 1 && t != 2) continue;
    int seen = 0;
    for(size_t k = 0; k < names.size(); k++) if(names[k] == list_ptr->data.username) seen = 1;
    if(seen) continue;
    names.push_back(list_ptr->data.username);
    wxString kinds;
    for(gxListNode<StaticDataBlock> *q = db_auth.static_block_list.GetHead(); q; q = q->next) {
      if(!(q->data.username == list_ptr->data.username)) continue;
      int qt = q->data.block_header.block_type;
      if(qt != 1 && qt != 2) continue;
      if(!kinds.IsEmpty()) kinds << ", ";
      kinds << (qt == 1 ? "RSA key" : "smart card");
    }
    choices.Add(wxString::FromUTF8(list_ptr->data.username.c_str()) + "  (" + kinds + ")");
  }
  if(choices.IsEmpty()) {
    ProgramError->Message("No authorized RSA or smartcard users found in encrypted DB file");
    return;
  }
  wxSingleChoiceDialog dialog(this, "Choose the user whose access is removed. A backup copy of the database is kept.",
			      "Remove User", choices);
  dialog.SetIcons(EPDBAppIcons());
  if(dialog.ShowModal() != wxID_OK || dialog.GetSelection() < 0) return;
  gxString username = names[dialog.GetSelection()];
  int count = 0, found = 0;
  for(list_ptr = db_auth.static_block_list.GetHead(); list_ptr; list_ptr = list_ptr->next) {
    if(list_ptr->data.block_header.block_type != 1 && list_ptr->data.block_header.block_type != 2) continue;
    count++;
    if(list_ptr->data.username == username) found++;
  }
  if(!found) {
    sbuf << clear << "User " << username << " has no RSA key or smart card access to this database";
    ProgramError->Message(sbuf.c_str());
    return;
  }
  // A database made with RSA keys or smart cards only has a random
  // password nobody knows
  if(count == found && gui_last_auth.auth != "password" && gui_last_auth.auth != "key-file") {
    sbuf << clear << username << " is the last RSA key or smart card user. The database may have no password "
	 << "anyone knows, so removing it could leave a database nothing can open.\n\n"
	 << "Open the database with its password or key file to remove this user.";
    ProgramError->Message(sbuf.c_str());
    GUILog(child_frame, "remove-user", gxString(), 0, "refused, last user");
    return;
  }
  if(!GUIConfirmClearUndo(child_frame, "Removing a user")) return;
  gxString err;
  if(EPDBRemoveUserInPlace(child_frame->DBParms()->pod->OpenDataFile(), child_frame->DBParms()->crypt_key,
			   0, username, err) != 0) {
    sbuf << clear << "ERROR: User " << username << " was not removed\n" << err;
    ProgramError->Message(sbuf.c_str());
    GUILog(child_frame, "remove-user", gxString(), 0, username.c_str());
    return;
  }
  child_frame->DBParms()->v2_dirty = 0; // Sealed by EPDBRemoveUserInPlace()
  GUILog(child_frame, "remove-user", gxString(), 1, username.c_str());
  sbuf << clear << "Access for user " << username << " was removed.\n\n"
       << "Removing a user does not revoke a database key the user may already have recovered, "
       << "or copies of the database made before. To give the database a new master key run:\n"
       << "epdb_auth_tool --rekey " << child_frame->DBParms()->data_file;
  ProgramError->Message(sbuf.c_str());
}

#include <wx/numdlg.h>
// ----------------------------------------------------------- //
// 10/27/2026: Undo/redo and the Security menu. The work is done by the
// core, see epdb_gui_hooks.cpp.
// ----------------------------------------------------------- //
void MainFrame::OnUndo(wxCommandEvent& event)
{
  GUIUndo(ActiveChild(), 0);
}

void MainFrame::OnRedo(wxCommandEvent& event)
{
  GUIUndo(ActiveChild(), 1);
}

void MainFrame::OnUpdateUndo(wxUpdateUIEvent& event)
{
  CryptDBDocument *doc = ActiveChild();
  int can = doc && doc->undo_stack.CanUndo();
  event.Enable(can != 0);
  gxString label = "Undo";
  if(can) label << " " << doc->undo_stack.UndoLabel();
  label << "\tCtrl-Z";
  event.SetText(label.c_str());
}

void MainFrame::OnUpdateRedo(wxUpdateUIEvent& event)
{
  CryptDBDocument *doc = ActiveChild();
  int can = doc && doc->undo_stack.CanRedo();
  event.Enable(can != 0);
  gxString label = "Redo";
  if(can) label << " " << doc->undo_stack.RedoLabel();
  label << "\tCtrl-Y";
  event.SetText(label.c_str());
}

void MainFrame::OnAccessLog(wxCommandEvent& event)
{
  progcfg->access_log = event.IsChecked() ? 1 : 0;
  GUISaveConfigValue("AccessLog", progcfg->access_log ? "1" : "0");
  gxString m;
  gxString f = progcfg->access_log_file.is_null() ? FC_default_log_file(".encrypted_password_database") : progcfg->access_log_file;
  m << "Access log " << (progcfg->access_log ? "on: " : "off: ") << f;
  *(statusWin) << m.c_str() << "\n";
}

void MainFrame::OnInFileLog(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  CryptDBDocument *doc = ActiveChild();
  if(!doc) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  if(!EPDBIsKeyset(doc->DBParms()->crypt_key)) {
    ProgramError->Message("The in-file access log needs a v2 database. Upgrade it with File > Upgrade Database Format.");
    return;
  }
  gxDatabase *f = doc->GetPOD()->OpenDataFile();
  EPDBHeaderExt x;
  EPDBReadExt(f, x);
  int on = (x.flags & EPDB_EXT_LOG_IN_FILE) != 0;
  gxString q;
  q << "The in-file access log is " << (on ? "on" : "off") << " for this database. It keeps the last "
    << EPDB_RING_ENTRIES << " opens and changes, encrypted, in the database file.\n\nTurn it "
    << (on ? "off" : "on") << "?";
  if(wxMessageBox(q.c_str(), "In-File Access Log", wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) != wxYES) return;
  gxString err;
  int rv = 0;
  if(!on) {
    gxList<FCRingEntry> entries;
    unsigned long long written = 0;
    if(EPDBRingRead(f, doc->DBParms()->crypt_key, entries, written, err) != 0) {
      err.Clear();
      rv = EPDBRingInit(f, doc->DBParms()->crypt_key, err);
    }
    x.flags |= EPDB_EXT_LOG_IN_FILE;
  }
  else {
    x.flags &= ~EPDB_EXT_LOG_IN_FILE;
  }
  if(rv == 0) rv = EPDBWriteExt(f, x, doc->DBParms()->crypt_key, err);
  doc->DBParms()->v2_dirty = 0;
  if(rv != 0) {
    ProgramError->Message(err.c_str());
    return;
  }
  GUILog(doc, on ? "access-log-off" : "access-log-on", gxString(), 1);
  *(statusWin) << "In-file access log " << (on ? "off" : "on") << " for " << doc->DBParms()->data_file.c_str() << "\n";
}

void MainFrame::OnSelfDestruct(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  CryptDBDocument *doc = ActiveChild();
  if(!doc) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  if(!EPDBIsKeyset(doc->DBParms()->crypt_key)) {
    ProgramError->Message("Self-destruct needs a v2 database. Upgrade it with File > Upgrade Database Format.");
    return;
  }
  gxDatabase *f = doc->GetPOD()->OpenDataFile();
  FCControlBlock c;
  EPDBReadControl(f, c);
  // 10/27/2026: A real dialog with wrapped text, n is 0 for off
  SelfDestructDialog sd(this, c.SelfDestruct(), c.limit);
  if(sd.ShowModal() != wxID_OK) return; // Canceled
  long n = sd.IsOn() ? (long)sd.Limit() : 0;
  if(!GUIConfirmClearUndo(doc, "Changing self-destruct")) return;
  EPDBHeaderExt x;
  EPDBReadExt(f, x);
  if(n == 0) {
    c.flags &= ~FC_CTL_SELF_DESTRUCT;
    x.flags &= ~EPDB_EXT_SELF_DESTRUCT;
  }
  else {
    c.flags |= FC_CTL_SELF_DESTRUCT;
    c.limit = (unsigned)n;
    x.flags |= EPDB_EXT_SELF_DESTRUCT;
    x.sd_limit = (unsigned)n;
  }
  c.failed = 0;
  gxString err;
  int rv = EPDBWriteControl(f, c, err);
  if(rv == 0) rv = EPDBWriteExt(f, x, doc->DBParms()->crypt_key, err);
  doc->DBParms()->v2_dirty = 0;
  if(rv != 0) {
    ProgramError->Message(err.c_str());
    return;
  }
  gxString detail;
  if(n) detail << "self-destruct on, " << n << " attempts";
  else detail << "self-destruct off";
  GUILog(doc, "settings", gxString(), 1, detail.c_str());
  *(statusWin) << "Self-destruct: " << detail.c_str() << "\n";
}

static void GUIShowLog(MainFrame *mf, int verify)
{
  if(!TestDatabase(0, 1, 1)) return;
  CryptDBDocument *doc = mf->ActiveChild();
  if(!doc) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  MemoryBuffer &key = doc->DBParms()->crypt_key;
  if(!EPDBIsKeyset(key)) {
    ProgramError->Message("The access logs of a v1 database have no tags to verify. Upgrade it with File > Upgrade Database Format.");
    return;
  }
  mf->sframe->status_win->Clear();
  mf->sframe->Show(TRUE);
  wxTextCtrl *w = frame->statusWin;
  gxString err;
  gxDatabase *f = doc->GetPOD()->OpenDataFile();
  EPDBHeaderExt x;
  int bad = 0;
  if(EPDBReadExt(f, x) && (x.flags & EPDB_EXT_LOG_IN_FILE)) {
    gxList<FCRingEntry> entries;
    unsigned long long written = 0;
    int r = EPDBRingRead(f, key, entries, written, err);
    gxString hdr;
    hdr << "In-file access log: " << (unsigned long)written << " entries written, the last " << EPDB_RING_ENTRIES << " are kept\n";
    *w << hdr.c_str();
    for(gxListNode<FCRingEntry> *p = entries.GetHead(); p && !verify; p = p->next) {
      char stamp[32];
      time_t t = (time_t)p->data.when;
      strftime(stamp, sizeof(stamp), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
      gxString ln;
      ln << (unsigned long)p->data.seq << "  " << stamp << "  " << FC_action_name(p->data.action) << "  "
	 << FC_auth_name(p->data.auth) << "  " << (p->data.result ? "ok" : "fail") << "\n";
      *w << ln.c_str();
    }
    if(r != 0) { *w << "TAMPERED: " << err.c_str() << "\n"; bad = 1; }
    else if(verify) *w << "OK: in-file access log verified\n";
  }
  else {
    *w << "The in-file access log is off for this database\n";
  }
  gxString log_file = progcfg->access_log_file.is_null() ? FC_default_log_file(".encrypted_password_database") : progcfg->access_log_file;
  if(!futils_exists(log_file.c_str())) {
    *w << "No per-user access log at " << log_file.c_str() << "\n";
  }
  else if(verify) {
    unsigned char lk[FC_LOG_KEY_LEN];
    FC_log_key(EPDBKeysetKey(key, EPDB_KEY_MASTER), EPDB_V2_KEY_LEN, EPDBKeysetID(key), EPDB_V2_ID_LEN, lk);
    FCLogVerifyResult r;
    int lrv = FC_log_verify(log_file.c_str(), FC_hex(EPDBKeysetID(key), EPDB_V2_ID_LEN).c_str(), lk, r, err);
    OPENSSL_cleanse(lk, sizeof(lk));
    if(lrv < 0) *w << "ERROR: " << err.c_str() << "\n";
    else {
      gxString ln;
      ln << "Per-user access log " << log_file << ": " << r.id_lines << " entries, " << r.tagged << " tags verified\n";
      *w << ln.c_str();
      if(lrv != 0) {
	ln << clear << "TAMPERED: first problem at line " << r.first_bad_line << ": " << r.first_bad_reason << "\n";
	*w << ln.c_str();
	bad = 1;
      }
      else *w << "OK: per-user access log verified\n";
    }
  }
  else {
    gxString id = FC_hex(EPDBKeysetID(key), EPDB_V2_ID_LEN), tag;
    tag << "\tid=" << id << "\t";
    *w << "Per-user access log " << log_file.c_str() << ":\n";
    FILE *fp = fopen(log_file.c_str(), "rb");
    char line[8192];
    while(fp && fgets(line, sizeof(line), fp)) if(strstr(line, tag.c_str())) *w << line;
    if(fp) fclose(fp);
  }
  if(verify && bad) wxMessageBox("The access log check found changed, removed or inserted entries. See the status window.",
				 "Access Log", wxOK | wxICON_WARNING);
}

void MainFrame::OnShowAccessLog(wxCommandEvent& event)
{
  GUIShowLog(this, 0);
}

void MainFrame::OnVerifyAccessLog(wxCommandEvent& event)
{
  GUIShowLog(this, 1);
}

void MainFrame::OnListUsers(wxCommandEvent& event)
{
  gxString sbuf;
  DatabaseUserAuth db_auth;
  int rv;
  
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  sframe->status_win->Clear();
  sframe->Show(TRUE); // Show the status window
  

  db_auth.f = child_frame->DBParms()->pod->OpenDataFile();

  *(frame->statusWin) << "Checking database file for authorized users" << "\n";
  if(db_auth.LoadStaticDataBlocks() != 0) {
    *(frame->statusWin) << "ERROR: " << db_auth.err.c_str() << "\n";
    return;
  }

  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Authorized users stats" << "\n";
  *(frame->statusWin) << "----------------------" << "\n";
  
  sbuf << clear << db_auth.static_data_size;
  *(frame->statusWin) << "Static auth data size: " << sbuf.c_str() << "\n";
  sbuf << clear <<  db_auth.static_data_bytes_used;
  *(frame->statusWin) << "Static auth bytes used: " << sbuf.c_str() << "\n";
  sbuf << clear << db_auth.num_static_data_blocks;
  *(frame->statusWin) << "Number of static blocks: " << sbuf.c_str() << "\n";

  gxListNode<StaticDataBlock> *list_ptr = db_auth.static_block_list.GetHead();
  if(!list_ptr) {
    *(frame->statusWin) << "\n";
#ifdef __ENABLE_SMART_CARD__
    *(frame->statusWin) << "INFO: No authorized RSA or smartcard users found in encrypted DB file" << "\n";
#else
    *(frame->statusWin) << "INFO: No authorized RSA users found in encrypted DB file" << "\n";    
#endif // __ENABLE_SMART_CARD__
  }
  else {
    *(frame->statusWin) << "\n";
    *(frame->statusWin) << "Authorized users list" << "\n";
    *(frame->statusWin) << "---------------------" << "\n";
    gxString access_type = "Unknown";
    while(list_ptr) {
      if(list_ptr->data.block_header.block_type == 1) access_type = "RSA key";
      if(list_ptr->data.block_header.block_type == 2) access_type = "Smart Card";
      *(frame->statusWin) << "Username: " << list_ptr->data.username.c_str() << " Access: " << access_type.c_str() << "\n";
      list_ptr = list_ptr->next;
    }
  }
  
  return;
}

#ifdef __USE_CLIPBOARD_FUNCTIONS__
void MainFrame::OnClearClipboard(wxCommandEvent& event)
// 10/25/2026: Clear the clipboard instead of copying an empty string
// to it, and do not flush it, which kept the data after the program
// exited.
{
  if(!wxTheClipboard->Open()) {
    ProgramError->Message("Failed to open clipboard\n");
    return;
  }
  wxTheClipboard->Clear();
  wxTheClipboard->Close();
  clipboard_value.Clear();
  if(clipboard_timer) clipboard_timer->Stop();
}

#endif // __USE_CLIPBOARD_FUNCTIONS__

int MainFrame::CopyToClipboard(const wxString &value)
// 10/25/2026: Copy a value to the clipboard and clear it again after
// progcfg->clipboard_clear_seconds. Returns 1 if successful.
{
  if(!wxTheClipboard->Open()) {
    ProgramError->Message("Failed to open clipboard\n");
    return 0;
  }
  if(!wxTheClipboard->SetData(new wxTextDataObject(value))) {
    ProgramError->Message("Failed to put text on clipboard\n");
    wxTheClipboard->Close();
    return 0;
  }
  wxTheClipboard->Close();
  clipboard_value = value;
  if(clipboard_timer && progcfg->clipboard_clear_seconds > 0) {
    clipboard_timer->StartOnce(progcfg->clipboard_clear_seconds * 1000);
  }
  return 1;
}

void MainFrame::ClearClipboardIfOurs()
// 10/25/2026: Clear the clipboard if it still holds the value this
// program copied, so a value the user copied since is left alone.
{
  if(clipboard_value.IsEmpty()) return;
  if(wxTheClipboard->Open()) {
    int ours = 0;
    if(wxTheClipboard->IsSupported(wxDF_UNICODETEXT) || wxTheClipboard->IsSupported(wxDF_TEXT)) {
      wxTextDataObject data;
      if(wxTheClipboard->GetData(data)) ours = (data.GetText() == clipboard_value);
    }
    if(ours) wxTheClipboard->Clear();
    wxTheClipboard->Close();
  }
  clipboard_value.Clear();
}

void MainFrame::OnClipboardTimer(wxTimerEvent& WXUNUSED(event))
{
  ClearClipboardIfOurs();
}

void MainFrame::OnIdleTimer(wxTimerEvent& WXUNUSED(event))
// 10/25/2026: Close the open databases after the idle time
{
  if(progcfg->auto_lock_minutes <= 0) return;
  if(progcfg->child_frame_list.IsEmpty()) return;
  if((time(0) - last_activity) < (time_t)(progcfg->auto_lock_minutes * 60)) return;
  gxString msg;
  msg << clear << "Closed the open databases after " << progcfg->auto_lock_minutes << " minutes with no activity";
  LockAllDatabases(msg.c_str());
}

void MainFrame::LockAllDatabases(const char *reason)
// 10/25/2026: Close every open database, hide the panels that show
// database entries, clear the status window and the clipboard, and
// wipe the keys. The databases must be opened again with the password
// or key.
{
  if(apanel) apanel->Show(FALSE);
  if(cpanel) cpanel->Show(FALSE);
  if(fpanel) fpanel->Show(FALSE);
  if(dpanel && dpanel->PanelIsOpen()) dpanel->ClosePanel();

  // Close the child frames, the list changes as each one is deleted
  gxList<CryptDBDocument *> to_close;
  gxListNode<CryptDBDocument *> *ptr = progcfg->child_frame_list.GetHead();
  while(ptr) {
    to_close.Add(ptr->data);
    ptr = ptr->next;
  }
  ptr = to_close.GetHead();
  while(ptr) {
    ptr->data->Close(TRUE);
    ptr = ptr->next;
  }

  ClearClipboardIfOurs();
  DBStringConfig::crypt_key.Clear(1);
  progcfg->global_dbparms.crypt_key.Clear(1);
  if(statusWin) statusWin->Clear();
  if(reason && statusWin) *(statusWin) << reason << "\n";
}

void MainFrame::OnResealTimer(wxTimerEvent& WXUNUSED(event))
// 10/25/2026: Seal the v2 databases this program changed
{
  gxListNode<CryptDBDocument *> *ptr = progcfg->child_frame_list.GetHead();
  while(ptr) {
    ResealDocument(ptr->data);
    ptr = ptr->next;
  }
}

void MainFrame::OnUpgrade(wxCommandEvent& WXUNUSED(event))
// 10/25/2026: Upgrade the active database to format v2. The database is
// closed, upgraded with a backup copy first, and must be opened again.
{
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  if(EPDBIsKeyset(child_frame->DBParms()->crypt_key)) {
    ProgramError->Message("This database already uses database format v2");
    return;
  }
  if(child_frame->DBParms()->admin_rights == 0) {
    ProgramError->Message("The database is open read-only and cannot be upgraded");
    return;
  }
  gxString data_file = child_frame->DBParms()->data_file;
  gxString mbuf;
  mbuf << clear << "Upgrade " << data_file.c_str() << " to database format v2?\n\n"
       << "Format v2 uses a stronger password key derivation (PBKDF2-SHA256, 600000 iterations) "
       << "and checks the whole database for changes made outside this program.\n\n"
       << "A backup copy of the database is made first. The database is closed for the upgrade "
       << "and must be opened again afterwards.\n\n"
       << "Versions of this program from before 10/25/2026 cannot open a v2 database.";
  if(wxMessageBox(mbuf.c_str(), "Upgrade Database Format",
		  wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_QUESTION, this) != wxYES) {
    return;
  }

  if(!GUIConfirmClearUndo(child_frame, "The upgrade")) return; // 10/27/2026
  // The v1 key is the database secret
  MemoryBuffer secret(child_frame->DBParms()->crypt_key);
  ResealDocument(child_frame);
  child_frame->GetPOD()->Close();
  child_frame->Close(TRUE);

  gxString backup_file, err;
  ::wxBeginBusyCursor();
  int rv = EPDBUpgrade(data_file.c_str(), secret, backup_file, err);
  ::wxEndBusyCursor();
  secret.Clear(1);
  if(rv != 0) {
    *(statusWin) << "Upgrade of " << data_file.c_str() << " failed: " << err.c_str() << "\n";
    mbuf << clear << "The upgrade failed and the database was not changed.\n\n" << err.c_str();
    ProgramError->Message(mbuf.c_str());
    GUILog(data_file.c_str(), "upgrade", gxString(), 0, "not changed", 0);
    return;
  }
  GUILog(data_file.c_str(), "upgrade", gxString(), 1, "v1 to v2", 0);
  gxString index_backup = ReplaceFileExtension(backup_file, ".ehx");
  *(statusWin) << "Upgraded " << data_file.c_str() << " to database format v2" << "\n";
  *(statusWin) << "The v1 database was saved to " << backup_file.c_str() << " and "
	       << index_backup.c_str() << "\n";
  mbuf << clear << "The database was upgraded to format v2.\n\n"
       << "The original database was saved to:\n" << backup_file.c_str() << "\n" << index_backup.c_str()
       << "\n\nOpen the database again to use it.";
  ProgramError->Message(mbuf.c_str());
}

void MainFrame::OnToggleShowPasswords(wxCommandEvent& event)
// 10/25/2026: Show or mask the password columns in every open grid
{
  progcfg->show_passwords = event.IsChecked() ? 1 : 0;
  gxListNode<CryptDBDocument *> *ptr = progcfg->child_frame_list.GetHead();
  while(ptr) {
    if(ptr->data->GridFrame() && ptr->data->GridFrame()->m_grid) {
      ptr->data->GridFrame()->m_grid->ForceRefresh();
    }
    ptr = ptr->next;
  }
}

#ifdef __USE_CLIPBOARD_FUNCTIONS__
void MainFrame::OnCopy(wxCommandEvent& event)
{
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  child_frame->GridFrame()->UpdateGridPosition();
  
  wxString clipData = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row,
								     child_frame->GridFrame()->dbparms.current_col);
  CopyToClipboard(clipData);
}

void MainFrame::OnCut(wxCommandEvent& event)
{
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  /* TODO: Fix the mutli select dectection
  wxArrayInt r_sels = child_frame->GridFrame()->m_grid->GetSelectedRows();
  wxArrayInt c_sels = child_frame->GridFrame()->m_grid->GetSelectedCols();
  if(r_sels.size() > 1) {
    ProgramError->Message("Cut cell can only cut a single cell\n");
    return;
  }
  if(c_sels.size() > 1) {
    ProgramError->Message("Cut cell can only cut a single cell\n");
    return;
  }
  */
  
  gxString ebuf;
  INFOHOG infohog(child_frame->GetPOD());

  child_frame->GridFrame()->UpdateGridPosition();
  
  if(child_frame->GridFrame()->dbparms.current_col == 0) {
    ProgramError->Message("The key memeber cannot be blank\n");
    return;
  }
  
  wxString clipData = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row,
								     child_frame->GridFrame()->dbparms.current_col);
  if(!CopyToClipboard(clipData)) return;

  INFOHOG_t key(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0));
  infohog.SetMember(&key, sizeof(key), 0);
  if(!infohog.FindObject()) {
    ebuf << clear << "Could not find item: " 
	 << child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0)
	 << " in the database";
    ProgramError->Message(ebuf.c_str());
    return;
  }
  
  if(!infohog.ReadObject()) {
    ebuf << clear << "Error reading item: " 
	 << child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0)
	 << " in the database";
    ProgramError->Message(ebuf.c_str());
    return;
  }
  
  GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
  gxString undo_name = (const char *)child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0).c_str();
  GUIUndoBefore(child_frame, undo_name, undo_mark);
  wxString blank_text = "";
  child_frame->GridFrame()->m_grid->SetCellValue(child_frame->GridFrame()->dbparms.current_row,
						 child_frame->GridFrame()->dbparms.current_col,
						 blank_text);

  INFOHOG_t memeber_update(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row,
									  child_frame->GridFrame()->dbparms.current_col));
  infohog.ChangeMember(&memeber_update, sizeof(memeber_update), child_frame->GridFrame()->dbparms.current_col), 0;
  
  child_frame->GetPOD()->Flush(); 
  GUIUndoAfter(undo_mark, undo_name, "update-record");
  return;
}

void MainFrame::OnPaste(wxCommandEvent& event)
{
  gxString ebuf;
  
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  /* TODO: Fix the mutli select dectection
  wxArrayInt r_sels = child_frame->GridFrame()->m_grid->GetSelectedRows();
  wxArrayInt c_sels = child_frame->GridFrame()->m_grid->GetSelectedCols();
  if(r_sels.size() > 1) {
    ProgramError->Message("Paste cell can only paste a single cell\n");
    return;
  }
  if(c_sels.size() > 1) {
    ProgramError->Message("Paste cell can only paste a single cell\n");
    return;
  }
  */
  
  if(!wxTheClipboard->Open()) {
    ProgramError->Message("Failed to open clipboard\n");
    return;
  }

  child_frame->GridFrame()->UpdateGridPosition();

  INFOHOG infohog(child_frame->GetPOD());
  
  if(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0) == "") {
    wxTextDataObject new_data;
    wxTheClipboard->GetData(new_data);
    
    child_frame->GridFrame()->m_grid->SetCellValue(child_frame->GridFrame()->dbparms.current_row,
						   child_frame->GridFrame()->dbparms.current_col,
						   new_data.GetText());
    INFOHOG_t new_key(new_data.GetText());
    infohog.SetMember(&new_key, sizeof(new_key), 0);
    int exists = infohog.FindObject();
    if(exists) {
      ProgramError->Message("Item: ", new_data.GetText().c_str(), 
			    "\nalready exists in the database");
      wxTheClipboard->Close();
      return;
    }
    GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
    gxString undo_name = (const char *)new_data.GetText().c_str();
    GUIUndoBefore(child_frame, undo_name, undo_mark);
    if(!infohog.WriteObject()) {
      ProgramError->Message("Could not add object to the database\n");
      wxTheClipboard->Close();
      return;
    }
    else {
      child_frame->GetPOD()->Flush(); // Flush the POD database following each insert
      GUIUndoAfter(undo_mark, undo_name, "add-record");
      statusWin->WriteText("Added entry ");
      statusWin->WriteText(new_data.GetText().c_str());
      statusWin->WriteText(" to the database\n");
    }
    wxTheClipboard->Close();
    return;
  }
  
  wxString sbuf;
  INFOHOG_t key(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0));
  infohog.SetMember(&key, sizeof(key), 0);
  if(!infohog.FindObject()) {
    ebuf << clear << "Could not find item: " 
	 << child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0)
	 << " in the database";
    ProgramError->Message(ebuf.c_str());
    return;
  }
  
  if(!infohog.ReadObject()) {
    ebuf << clear << "Error reading item: " 
	 << child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0)
	 << " in the database";
    ProgramError->Message(ebuf.c_str());
    return;
  }

  wxTextDataObject data;
  wxTheClipboard->GetData(data);
  GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
  gxString undo_name = (const char *)child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0).c_str();
  GUIUndoBefore(child_frame, undo_name, undo_mark);
  
  child_frame->GridFrame()->m_grid->SetCellValue(child_frame->GridFrame()->dbparms.current_row,
						 child_frame->GridFrame()->dbparms.current_col,
						 data.GetText());

  if(child_frame->GridFrame()->dbparms.current_col == 0) {
    INFOHOG_t key_update(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0));
    
    if(!infohog.ChangeKeyMember(key_update, 1)) {
      ebuf << clear << "Error changing key member to : " 
	   << child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0);
      ProgramError->Message(ebuf.c_str());
    }
  }
  else {
    INFOHOG_t memeber_update(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row,
									    child_frame->GridFrame()->dbparms.current_col));
    infohog.ChangeMember(&memeber_update, sizeof(memeber_update), child_frame->GridFrame()->dbparms.current_col), 0;
  }
  
  wxTheClipboard->Close();
  child_frame->GetPOD()->Flush(); 
  GUIUndoAfter(undo_mark, gxString((const char *)child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0).c_str()), "update-record");
  
  return;
}
#endif // __USE_CLIPBOARD_FUNCTIONS__

void MainFrame::OnPreferences(wxCommandEvent& event)
{
  e_tab_panel->ShowPanel();
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
  OpenDatabaseFile(wxString());
}

void MainFrame::OpenDatabaseFile(const wxString &start_file)
// 10/27/2026: The Unlock Database dialog asks for the file and the
// unlock method, and unlocks the database
{
#ifdef __APP_DEBUG_VERSION__
  *(statusWin) << "Opening an existing database" << "\n";
#endif

  gxString work_dir = (const char *)::wxGetCwd().c_str();
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  gxString database_file;
  if(unlock_dlg->ShowPanel(database_file, start_file)) {
    // Make another frame, containing a canvas
    CryptDBDocument *subframe = new CryptDBDocument(frame, progcfg->ProgramName.c_str(),
						    wxPoint(-1, -1), 
						    wxSize(-1, -1),
						    wxDEFAULT_FRAME_STYLE);
    progcfg->num_child_frames++;
    
    // Give it an icon
    subframe->SetIcons(EPDBAppIcons()); // 10/27/2026
    
    // 10/27/2026: The grid goes in the splitter, the details pane right of it
    CryptDBGrid *grid_frame = new CryptDBGrid(subframe, wxDefaultPosition, wxDefaultSize, subframe->splitter);
    subframe->grid_frame = grid_frame;
    subframe->AttachGrid(grid_frame->m_grid);

    subframe->DBParms()->crypt_key = progcfg->global_dbparms.crypt_key;
    DBStringConfig::crypt_key = progcfg->global_dbparms.crypt_key;
    subframe->unlock_description = unlock_dlg->UnlockDescription(); // 10/27/2026

    // Open the database associated with this child frame
    CryptDBDocument *prev_frame = progcfg->active_child_frame; 
    progcfg->active_child_frame = subframe;
    subframe->DBParms()->pod = OpenDatabase(subframe, database_file, 
					    subframe->DBParms()->pod,
					    subframe->DBParms()->db_config,
					    subframe->DBParms()->data_file_extension, 
					    subframe->DBParms()->index_file_extension,
					    1, 1);
    ::wxYield();

    if(!subframe->DBParms()->pod) {
      ProgramError->Message("Error opening the database.");
      progcfg->active_child_frame = prev_frame;
      subframe->Close();
      ::wxSetWorkingDirectory(work_dir.c_str());
      return;
    }

    // Called after database is opened top ensure that 
    // all the Grid parameters are set.
    grid_frame->InitGrid();

    char dest[DBStringLength];
    subframe->SetTitle(subframe->DBParms()->db_config.database_name.c_str(dest));

    subframe->Show(TRUE);
    // NOTE: 11/06/2014: When testing the database after an open the child frame is not set
    // NOTE: 11/06/2014: So the ReloadGrid() call was changed not to test the database
    //subframe->grid_frame->ReloadGrid(1);
    subframe->grid_frame->ReloadGrid(0);
  }
  else {
    ::wxSetWorkingDirectory(work_dir.c_str());
    return;
  }

  // CD to the program's working directory
  ::wxSetWorkingDirectory(work_dir.c_str());
}

void MainFrame::OnCloseStatusWin(wxCommandEvent& WXUNUSED(event))
{
  sframe->Show(FALSE);
}

void MainFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
  statusWin->Clear();
}

void MainFrame::OnStatus(wxCommandEvent& WXUNUSED(event))
{
  sframe->Show(TRUE);
}

void MainFrame::OnNew(wxCommandEvent& event)
{
  gxString work_dir = (const char *)::wxGetCwd().c_str();
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  new_db_panel->ShowPanel();

  if(!new_db_panel->IsOK()) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "No IsOK from db_panel" << "\n" << flush;
#endif
    ::wxSetWorkingDirectory(work_dir.c_str());
    return;
  }

  if(new_db_panel->IsOK()) {
    DBStringConfig::crypt_key = progcfg->global_dbparms.crypt_key;
    
    gxString dbname = new_db_panel->DatabasePath(); // 10/27/2026: The chosen folder and name
    
    gxString data_file = ReplaceFileExtension(dbname, progcfg->global_dbparms.data_file_extension.c_str()); // 10/25/2026
    if(futils_exists(data_file.c_str())) {
      ProgramError->Message("A database with this name already exists \
\nCannot use an existing datbase name");
      ::wxSetWorkingDirectory(work_dir.c_str());
      return;
    }

#ifdef __APP_DEBUG_VERSION__
    debug_log << "Creating new database: " << dbname.c_str() << "\n" << flush;
#endif

    // Make another frame, containing a canvas
    CryptDBDocument *subframe = new CryptDBDocument(frame, progcfg->ProgramName.c_str(),
						    wxPoint(-1, -1), 
						    wxSize(-1, -1),
						    wxDEFAULT_FRAME_STYLE);
    progcfg->num_child_frames++;
    subframe->SetTitle(wxString::FromUTF8(new_db_panel->DatabaseName().c_str()));
    subframe->unlock_description = "New database"; // 10/27/2026
    
    // Give it an icon
    subframe->SetIcons(EPDBAppIcons()); // 10/27/2026
    
    // 10/27/2026: The grid goes in the splitter, the details pane right of it
    CryptDBGrid *grid_frame = new CryptDBGrid(subframe, wxDefaultPosition, wxDefaultSize, subframe->splitter);
    subframe->grid_frame = grid_frame;
    subframe->AttachGrid(grid_frame->m_grid);
    grid_frame->InitGrid(); // Make a blank grid with no entries

    // Open the database associated with this child frame
    int rv;
    gxString sbuf;
    DatabaseUserAuth db_auth;
    CryptDBDocument *prev_frame = progcfg->active_child_frame; 
    progcfg->active_child_frame = subframe;

    subframe->DBParms()->crypt_key = progcfg->global_dbparms.crypt_key;
    subframe->DBParms()->pod = OpenDatabase(subframe, dbname, 
					    subframe->DBParms()->pod,
					    subframe->DBParms()->db_config,
					    subframe->DBParms()->data_file_extension, 
					    subframe->DBParms()->index_file_extension,
					    1, 1);
    ::wxYield();

    if(!subframe->DBParms()->pod) {
      ProgramError->Message("Error creating the database.");
      progcfg->active_child_frame = prev_frame;
      subframe->Close();
      ::wxSetWorkingDirectory(work_dir.c_str());
      return;
    }

    if(new_db_panel->use_rsa_key) {
      db_auth.f = subframe->DBParms()->pod->OpenDataFile();
      rv = db_auth.AddRSAKeyToStaticArea(subframe->DBParms()->crypt_key,
					 new_db_panel->public_key, new_db_panel->public_key_len, new_db_panel->rsa_key_username);
      if(rv != 0) {
	sbuf << clear << "ERROR: Cannot add public RSA key " << db_auth.err.c_str();
	ProgramError->Message(sbuf.c_str());
	progcfg->active_child_frame = prev_frame;
	subframe->Close();
	::wxSetWorkingDirectory(work_dir.c_str());
	// TODO: remove the database file and index file
	return;
      }
    }

#ifdef __ENABLE_SMART_CARD__
    if(new_db_panel->use_smartcard) {
      db_auth.f = subframe->DBParms()->pod->OpenDataFile();
       rv = db_auth.AddSmartCardCertToStaticArea(&new_db_panel->sc, new_db_panel->use_cert_file,
						 subframe->DBParms()->crypt_key, new_db_panel->smartcard_username);
       if(rv != 0) {
	 sbuf << clear << "ERROR: Cannot add smart card cert " << db_auth.err.c_str();
	 ProgramError->Message(sbuf.c_str());
	 progcfg->active_child_frame = prev_frame;
	 subframe->Close();
	 ::wxSetWorkingDirectory(work_dir.c_str());
	 // TODO: remove the database file and index file
	 return;
       }
       sbuf = new_db_panel->sc.cert_id;
       if(progcfg->SC_cert_id != sbuf) { // Update the global SC cert ID
	 progcfg->SC_cert_id = new_db_panel->sc.cert_id;
	 gxConfig CfgData(progcfg->cfgFile.c_str());
	 CfgData.ChangeConfigValue("SC_cert_id", (const char *)new_db_panel->sc.cert_id);
       }
    }
#endif // __ENABLE_SMART_CARD__
    
    subframe->Show(TRUE);
  }

  ::wxSetWorkingDirectory(work_dir.c_str());
}

void MainFrame::OnAddObject(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  apanel->ShowPanel();
}

void MainFrame::OnDisplay(wxCommandEvent& event)
{
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  frame->SetStatusText("Building the display list...");

  BuildVirtualDB(child_frame);

  frame->SetStatusText(" ");

  dllistptr = dllist->GetHead();
  INFOHOG infohog(child_frame->GetPOD());
  infohog.ReadObject(dllistptr->data);
  dpanel->LoadPanel(infohog);
  dpanel->ShowPanel();
}

void MainFrame::OnChangeObject(wxCommandEvent& event)
{
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  INFOHOG infohog(child_frame->GetPOD());
  wxString sbuf;
  INFOHOG_t key(child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0));
  infohog.SetMember(&key, sizeof(key), 0);
  if(!infohog.FindObject()) {
    *(statusWin) << "Could not find item: " 
		 << child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0)
		 << " in the database" << "\n";
    
    return;
  }
  
  if(!infohog.ReadObject()) {
    *(statusWin) << "Error reading item: " << 
      child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0)
		 << " in the database" << "\n";
      return;
  }
  cpanel->LoadPanel(infohog);
  cpanel->ShowPanel();
}

void MainFrame::OnFindObject(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;
  fpanel->ShowPanel();
}

void MainFrame::OnDeleteObject(wxCommandEvent& event)
{
  if(!TestDatabase(1, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  INFOHOG infohog(child_frame->GetPOD());
  child_frame->GridFrame()->UpdateGridPosition();

  INFOHOG_t key(child_frame->GridFrame()->dbparms.current_name);
  infohog.SetMember(&key, sizeof(key), 0);

  int yn;
  
  // Delete range selected items
  if(child_frame->GridFrame()->m_grid->IsSelection()) {
    yn = wxMessageBox("Delete the selected items?\n", "Program Message",
		      wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
    if(yn == wxNO) return;
    child_frame->undo_stack.BeginGroup("Delete the selected items"); // 10/27/2026
    child_frame->GridFrame()->m_grid->BeginBatch();
    for(int n = 0; n < child_frame->GridFrame()->m_grid->GetNumberRows();) {
      if(child_frame->GridFrame()->m_grid->IsInSelection(n , 0)) {
	if(child_frame->GridFrame()->m_grid->GetCellValue(n, 0) != "") {
	  key = child_frame->GridFrame()->m_grid->GetCellValue(n, 0);
	  infohog.SetMember(&key, sizeof(key), 0);
	  GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
	  gxString undo_name = (const char *)child_frame->GridFrame()->m_grid->GetCellValue(n, 0).c_str();
	  GUIUndoBefore(child_frame, undo_name, undo_mark);
	  if(!infohog.DeleteObject()) {
	    *(statusWin) << "Could not find item: "
			 << child_frame->GridFrame()->m_grid->GetCellValue(n, 0)
			 << " in the database" << "\n";
	  }
	  else {

	    // Update the status window
	    *(statusWin) << "Deleted item: " << child_frame->GridFrame()->m_grid->GetCellValue(n, 0)
			 << "\n";
	    GUIUndoAfter(undo_mark, undo_name, "delete-record");
	  }
	}
	child_frame->GridFrame()->m_grid->DeleteRows(n, 1);
      }
      else {
	n++;
      }
    }
    child_frame->GridFrame()->m_grid->EndBatch();
    child_frame->undo_stack.EndGroup();
    child_frame->GridFrame()->dbparms.current_row = 0;
    child_frame->GridFrame()->dbparms.current_col = 0;

    child_frame->GridFrame()->SetResetSkipLines();    

    if(child_frame->GridFrame()->m_grid->GetNumberRows() == 0) // The table is empty
      child_frame->GridFrame()->dbparms.current_name = "";
    else // Update the current name
      child_frame->GridFrame()->dbparms.current_name = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0);

    // Flush the POD database following the batch delete
    child_frame->GetPOD()->Flush(); 

    return;
  }
  
  // Deleting a single item
  if(child_frame->GridFrame()->dbparms.current_name == "") {
    // A blank row was selected
    return;
  }

  wxString s("Delete item: ");
  s += child_frame->GridFrame()->dbparms.current_name;
  s += "\n";
  yn = wxMessageBox(s, "Program Message",
		    wxYES_NO | wxCENTRE | wxICON_QUESTION, this);
  if(yn == wxNO) return;

  if(child_frame->GridFrame()->dbparms.current_name != "") {
    GUIUndoMark undo_mark; // 10/27/2026: Undo and the access log
    gxString undo_name = (const char *)child_frame->GridFrame()->dbparms.current_name.c_str();
    GUIUndoBefore(child_frame, undo_name, undo_mark);
    if(!infohog.DeleteObject()) {
      *(statusWin) << "Could not find item: " << child_frame->GridFrame()->dbparms.current_name
		   << " in the database" << "\n";
      // Do not delete the grid entry if the object does not exist in the database.
       return; 
    }
    else {
      // Update the status window
      *(statusWin) << "Deleted item: " << child_frame->GridFrame()->dbparms.current_name << "\n";
      infohog.Flush(); // Flush the POD database
      GUIUndoAfter(undo_mark, undo_name, "delete-record");
    }
  }

#ifdef __APP_DEBUG_VERSION__
  *(statusWin) << "Current row " << child_frame->GridFrame()->dbparms.current_row << "\n";
#ifdef __WXWIN2_GRID__
  *(statusWin) << "Number of rows " << child_frame->GridFrame()->m_grid->GetRows() << "\n";
#else
  *(statusWin) << "Number of rows " << child_frame->GridFrame()->m_grid->GetNumberRows() << "\n";
#endif
#endif
  
  // Delete the row and update the "current_name" varible
#ifdef __WXWIN2_GRID__
  if(child_frame->GridFrame()->m_grid->GetRows() == 1) { // There is only one row in this table
#else
    if(child_frame->GridFrame()->m_grid->GetNumberRows() == 1) { // There is only one row in this table
#endif
      
    child_frame->GridFrame()->m_grid->DeleteRows(child_frame->GridFrame()->dbparms.current_row);
    child_frame->GridFrame()->dbparms.current_name = "";
    return;
  }
#ifdef __WXWIN2_GRID__
  else if(child_frame->GridFrame()->dbparms.current_row == child_frame->GridFrame()->m_grid->GetRows()-1) { // At the last row
#else
  else if(child_frame->GridFrame()->dbparms.current_row == child_frame->GridFrame()->m_grid->GetNumberRows()-1) { // At the last row
#endif
    child_frame->GridFrame()->m_grid->DeleteRows(child_frame->GridFrame()->dbparms.current_row);
    child_frame->GridFrame()->dbparms.current_row--; // Use the previous name value
    child_frame->GridFrame()->dbparms.current_name = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0);
  }
  else {
    child_frame->GridFrame()->m_grid->DeleteRows(child_frame->GridFrame()->dbparms.current_row);
#ifdef __WXWIN2_GRID__
    if(child_frame->GridFrame()->dbparms.current_row == child_frame->GridFrame()->m_grid->GetRows()-1) {
#else
    if(child_frame->GridFrame()->dbparms.current_row == child_frame->GridFrame()->m_grid->GetNumberRows()-1) {
#endif
      // This is now the last row so use this row as the current name
      child_frame->GridFrame()->dbparms.current_name = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0);
    }
    else if(child_frame->GridFrame()->dbparms.current_row == 0) {
      // This is the first row so use this row as the current name
      child_frame->GridFrame()->dbparms.current_name = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0);
    }
    else {
      // Use the next row in order as the current name
      child_frame->GridFrame()->dbparms.current_name = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row+1, 0);
    }
  }

  child_frame->GridFrame()->SetResetSkipLines();
}

void MainFrame::OnImport(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  ImportFromASCII(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}

void MainFrame::OnExport(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;
  dllist->ClearList();
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  ExportToASCII(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}

void MainFrame::OnTemplate(wxCommandEvent& event)
{
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
  CreateTextTemplate(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}

void MainFrame::OnBackup(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  BackUp(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}

void MainFrame::OnMerge(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  Merge(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}

void MainFrame::OnCompare(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  CompareIndexFile();
}

void MainFrame::OnRebuild(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  RebuildIndexFile(child_frame->DBParms()->index_file,
  		   child_frame->GetPOD(), 1, 1);
}

void MainFrame::OnRebuildConfig(wxCommandEvent& event)
{
  if(!TestDatabase(0, 1, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  *(frame->statusWin) << "\n";
  *(frame->statusWin) << "Rebuilding the database configuration header..." << "\n";

  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;
  gxDatabase *f = child_frame->DBParms()->pod->OpenDataFile();

  gxDatabaseConfig nc = *(db_config);

  wxString sbuf;

  // Reset the config and save the previous hashes
  db_config->Clear();
  db_config->database_name = nc.database_name;
  for(int i = 0; i < NumDataMembers; i++) {
    // NOTE: Use a clean copy of the column names
    // db_config->column_names[i] = nc.column_names[i];
    db_config->column_names[i] = child_frame->DBParms()->dbgrid_labels[i];
  }

  // Write the clean config
  if(!db_config->WriteConfig(f)) {
    sbuf = "Error saving the rebuilt database configuration header header!\n";
    *(frame->statusWin) << sbuf;
    ProgramError->Message(sbuf.c_str());
    return;
  }

  sbuf = "The database configuration header was rebuilt and saved.\n";
  *(frame->statusWin) << sbuf;
  ProgramError->Message(sbuf.c_str());
}

void MainFrame::OnStats(wxCommandEvent& event)
{
  if(!TestDatabase(0, 0, 1)) return;
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  sframe->status_win->Clear();
  sframe->Show(TRUE); // Show the status window
  DisplayDatabaseConfig(child_frame, statusWin);
  DisplayDataFileStats(child_frame->GetPOD(), statusWin);
  DisplayIndexFileStats(child_frame->GetPOD(), statusWin);
}

#ifdef __USE_MSW_PRINTING__
int MainFrame::MSWTestPaperSize()
// Helper functions used to ensure that selected paper size is
// supported by the specified printer. Returns true if the paper
// size is supported or false if the paper size is not supported.
{
  CryptDBDocument *child_frame = ActiveChild();
  MSWPrintingParameters *print_config = &child_frame->print_config;

  wxPaperSize paper_size = print_config->print_data.GetPaperId();
  switch(paper_size) {
    case wxPAPER_LETTER: 
      break;
    case wxPAPER_LEGAL:
      break;
    case wxPAPER_TABLOID:
      break;
    case wxPAPER_A3:
      break;
    case wxPAPER_A4:
      break;
      
    default:
      ProgramError->Message("The selected paper size is not \
supported by this printer\n");
     return 0;
  };

  return 1;
}

void MainFrame::MSWPrint()
{
  if(!MSWTestPaperSize()) return;

  CryptDBDocument *child_frame = ActiveChild();
  MSWPrintingParameters *print_config = &child_frame->print_config;

  mswPrintout printout_init;
  if(!printout_init.InitPrintout()) {
    wxMessageBox("There was an error initializing the printer.\nPerhaps your current \
printer is not set correctly?", "Initializing Printer", wxOK);
    return;
  }

  wxPrintDialogData printDialogData(print_config->print_data);
  wxPrinter printer(&printDialogData);
  mswPrintout printout(progcfg->ProgramName.c_str());
  
  if(!printer.Print(this, &printout, TRUE)) {
    if(wxPrinter::GetLastError() == wxPRINTER_ERROR)
      wxMessageBox("There was a problem printing.\nPerhaps your current \
printer is not set correctly?", "Printing", wxOK);
    else
      wxMessageBox("Print operation canceled", "Printing", wxOK);
  }
  else {
    print_config->print_data = printer.GetPrintDialogData().GetPrintData();
  }
}

void MainFrame::OnMSWPrint(wxCommandEvent& event)
{
  if(!TestDatabase(0, 0, 1)) return;

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  CryptDBDocument *child_frame = ActiveChild();
  MSWPrintingParameters *print_config = &child_frame->print_config;

  print_config->print_list = 0;
  print_config->print_all_entries = 1;
  print_config->print_all_cols = 1;
  MSWPrint();
}

void MainFrame::OnMSWPrintPreview(wxCommandEvent& event)
{
  if(!TestDatabase(0, 0, 1)) return;

  if(dpanel->PanelIsOpen()) {
    dpanel->ClosePanel();
  }

  CryptDBDocument *child_frame = ActiveChild();
  MSWPrintingParameters *print_config = &child_frame->print_config;

  print_config->print_list = 0;
  print_config->print_all_entries = 1;
  print_config->print_all_cols = 1;
  MSWPrintPreview();
}

void MainFrame::MSWPrintPreview()
{
  if(!MSWTestPaperSize()) return;

  CryptDBDocument *child_frame = ActiveChild();
  MSWPrintingParameters *print_config = &child_frame->print_config;

  mswPrintout printout_init;
  if(!printout_init.InitPrintPreview()) {
    wxMessageBox("There was an error initializing the printer.\nPerhaps your current \
printer is not set correctly?", "Initializing Printer", wxOK);
    return;
  }

  wxPrintDialogData printDialogData(print_config->print_data);

  // Pass two printout objects: for preview, and possible printing.
  wxPrintPreview *preview = \
    new wxPrintPreview(new mswPrintout(progcfg->ProgramName.c_str()),
		       new mswPrintout(progcfg->ProgramName.c_str()),
		       &printDialogData);
  
  if(!preview->Ok()) {
    delete preview;
    wxMessageBox("There was a problem previewing.\nPerhaps your current \
printer is not set correctly?", "Previewing", wxOK);
    return;
  }

  // Set the percentage preview zoom to the most accurate screen
  // representation of the page that will be printed.
  preview->SetZoom(100);

  wxPreviewFrame *preview_frame = new wxPreviewFrame(preview, this,
						     progcfg->ProgramName.c_str());
  preview_frame->SetIcons(EPDBAppIcons()); // 10/27/2026
  preview_frame->Centre(wxBOTH);
  preview_frame->Initialize();
  preview_frame->Show(TRUE);
  preview_frame->Maximize(TRUE); // Maximize frame, WIN32 only
}

void MainFrame::OnMSWPrintPageSetup(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  mswpage->ShowPanel();
}

void MainFrame::OnMSWPrintSetup(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }

  MSWPrintingParameters *print_config = &child_frame->print_config;

  // Set the printer paper size and orientation
  print_config->SetPaperSize();
  print_config->SetOrientation();
  
  wxPrintDialogData printDialogData(print_config->print_data);
  wxPrintDialog printerDialog(this, & printDialogData);
  
  // TODO: Is this deprciated
  //  printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
  printerDialog.ShowModal();
  
  print_config->print_data = \
    printerDialog.GetPrintDialogData().GetPrintData();
}
#endif // __USE_MSW_PRINTING__



#ifdef __USE_APP_POSTSCRIPT_PRINTING__
void MainFrame::OnPSPrint(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;
  PostScriptPrint(this);
}

void MainFrame::OnPSPrintPageSetup(wxCommandEvent& event)
{
  pspage->Show(TRUE);
}
  
void MainFrame::OnPSPrintSetup(wxCommandEvent& event)
{
  lppanel->Show(TRUE);
}
#endif // __USE_APP_POSTSCRIPT_PRINTING__

#if defined (__USE_APP_TEXT_PRINTING__)
void MainFrame::OnASPrint(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;
  TextFilePrint(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}
#endif

#ifdef __USE_APP_HTM_PRINTING__
void MainFrame::OnHTMPrint(wxCommandEvent& event)
{
  if(!TestDatabase(1, 0, 1)) return;
  HTMLPrint(this);
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());
}
#endif

void MainFrame::AutoSize(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  child_frame->GridFrame()->m_grid->AutoSizeColumns();
  child_frame->GridFrame()->m_grid->AutoSizeRows();

  // Requires a grid refresh
  child_frame->GridFrame()->RefreshGrid();

  db_config->auto_size = 1;  

  if(!TestDatabase(0, 1, 1)) return;

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteAutoSize(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving auto size value" << "\n";
  }

  for(int i = 0; i < NumDataMembers; i++) {
#ifdef __WXWIN2_GRID__
    int col_width = child_frame->GetGrid()->GetColumnWidth(i);
#else
    int col_width = child_frame->GetGrid()->ToDIP(child_frame->GetGrid()->GetColSize(i)); // 10/27/2026: DIPs
#endif
    db_config->col_sizes[i] = col_width;
  }

  if(!db_config->WriteColSize(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error writing colsize DB config value" << "\n";
  }
}

void MainFrame::DisableAutoSize(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  db_config->auto_size = 0;

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteAutoSize(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving auto size value" << "\n";
  }
}

void MainFrame::CellOverflow(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();

  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  if(db_config->cell_overflow == 1) {
    child_frame->GridFrame()->m_grid->SetDefaultCellOverflow(FALSE);
    db_config->cell_overflow = 0;
  }
  else {
    child_frame->GridFrame()->m_grid->SetDefaultCellOverflow(TRUE);
    db_config->cell_overflow = 1;
  }

  // Requires a grid refresh
  child_frame->GridFrame()->RefreshGrid();

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteCellOverflow(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving cell overflow value" << "\n";
  }
}

void MainFrame::ToggleGridLines(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  if(db_config->view_grid_lines == 1) {
    child_frame->GridFrame()->m_grid->EnableGridLines(0);
    db_config->view_grid_lines = 0;
  }
  else {
    child_frame->GridFrame()->m_grid->EnableGridLines(1);
    db_config->view_grid_lines = 1;
  }

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteViewGridLines(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving view grid line value" << "\n";
  }
}

void MainFrame::SetGridLabelTextColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetGridLabelTextColor(&color)) {
    data.SetColour(color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    child_frame->GridFrame()->SetGridLabelTextColor(&color);

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteGridLabelTextColor(child_frame->DBParms()->pod->OpenDataFile(),
					   &color)) {
      *(frame->statusWin) << "Error saving grid label text color value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::SetGridLineColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetGridLineColor(&color)) {
    data.SetColour(color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    child_frame->GridFrame()->SetGridLineColor(&color);

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteGridLineColor(child_frame->DBParms()->pod->OpenDataFile(), &color)) {
      *(frame->statusWin) << "Error saving grid line color value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::ViewSkipLines(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  if(db_config->view_skip_lines == 1) {
    db_config->view_skip_lines = 0;
    child_frame->GridFrame()->SetGridBackgroundColor();
  }
  else {
    db_config->view_skip_lines = 1;
    child_frame->GridFrame()->SetSkipLines();
  }

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteViewSkipLines(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving view skip lines value" << "\n";
  }
}

void MainFrame::SetSkipLineColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetSkipLineColor(&color)) {
    data.SetColour(color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    db_config->view_skip_lines = 1;
    child_frame->GridFrame()->SetSkipLines(&color);

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteSkipLineColor(child_frame->DBParms()->pod->OpenDataFile(),
				      &color)) {
      *(frame->statusWin) << "Error saving skip line color value" << "\n";
    }
    if(!db_config->WriteViewSkipLines(child_frame->DBParms()->pod->OpenDataFile())) {
      *(frame->statusWin) << "Error saving view skip lines value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::SetGridBackgroundColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetGridBackgroundColor(&color)) {
    data.SetColour(color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    child_frame->GridFrame()->SetGridBackgroundColor(&color);
    child_frame->GridFrame()->SetSkipLines();

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteGridBackgroundColor(child_frame->DBParms()->pod->OpenDataFile(),
					   &color)) {
      *(frame->statusWin) << "Error saving grid background color value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::SetGridTextColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetGridTextColor(&color)) {
    data.SetColour(color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    child_frame->GridFrame()->SetGridTextColor(&color);

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteGridTextColor(child_frame->DBParms()->pod->OpenDataFile(),
				      &color)) {
      *(frame->statusWin) << "Error saving grid text color value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::SetGridLabelBackgroundColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetGridLabelBackgroundColor(&color)) {
    data.SetColour(color);
  }

  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    child_frame->GridFrame()->SetGridLabelBackgroundColor(&color);

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteGridLabelBackgroundColor(child_frame->DBParms()->pod->OpenDataFile(),
						 &color)) {
      *(frame->statusWin) << "Error saving grid label background color value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::SetHyperlinkColor(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxColourData data;
  wxColour color;
  data.SetChooseFull(TRUE);
  LoadCustomColors(&data);

  // Set if user has already selected a color
  if(db_config->GetHyperlinkColor(&color)) {
    data.SetColour(color);
  }
  wxColourDialog dlg(this, &data);
  if(dlg.ShowModal() == wxID_OK) {
    wxColourData retData = dlg.GetColourData();
    color = retData.GetColour();
    child_frame->GridFrame()->SetHyperlinkColor(&color);

    // Make this setting persistent by writing it to the dbconfig
    if(!db_config->WriteHyperlinkColor(child_frame->DBParms()->pod->OpenDataFile(),
						 &color)) {
      *(frame->statusWin) << "Error saving hyperlink color value" << "\n";
    }
    SaveCustomColors(&retData);
  }
}

void MainFrame::SetGridLabelFont(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxFontData data;
  wxFont font;
  wxColour color;
  if(db_config->GetGridLabelFont(&font)) {
    data.SetInitialFont(font);
  }
#if defined (__WIN32__)
  data.EnableEffects(0);
  data.SetAllowSymbols(0);
#endif

  wxFontDialog dialog(this, data);
  if(dialog.ShowModal() == wxID_OK) {
    wxFontData retData = dialog.GetFontData();
    font = retData.GetChosenFont();
    child_frame->GridFrame()->SetGridLabelFont(&font);
    if(!db_config->WriteGridLabelFont(child_frame->DBParms()->pod->OpenDataFile(),
				      &font)) {
      *(frame->statusWin) << "Error saving grid label font value" << "\n";
    }
  }
}

void MainFrame::SetGridTextFont(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxFontData data;
  wxFont font;
  wxColour color;
  if(db_config->GetGridTextFont(&font)) {
    data.SetInitialFont(font);
  }
#if defined (__WIN32__)
  data.EnableEffects(0);
  data.SetAllowSymbols(0);
#endif

  wxFontDialog dialog(this, data);
  if(dialog.ShowModal() == wxID_OK) {
    wxFontData retData = dialog.GetFontData();
    font = retData.GetChosenFont();
    child_frame->GridFrame()->SetGridTextFont(&font);
    if(!db_config->WriteGridTextFont(child_frame->DBParms()->pod->OpenDataFile(),
				     &font)) {
      *(frame->statusWin) << "Error saving grid text font value" << "\n";
    }
  }
}

void MainFrame::SetHyperlinkFont(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  wxFontData data;
  wxFont font;
  wxColour color;
  if(db_config->GetHyperlinkFont(&font)) {
    data.SetInitialFont(font);
  }
#if defined (__WIN32__)
  data.EnableEffects(0);
  data.SetAllowSymbols(0);
#endif

  wxFontDialog dialog(this, data);
  if(dialog.ShowModal() == wxID_OK) {
    wxFontData retData = dialog.GetFontData();
    font = retData.GetChosenFont();
    child_frame->GridFrame()->SetHyperlinkFont(&font);
    if(!db_config->WriteHyperlinkFont(child_frame->DBParms()->pod->OpenDataFile(),
				      &font)) {
      *(frame->statusWin) << "Error saving hyperlink font value" << "\n";
    }
  }
}

void MainFrame::ToggleRowLabels(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  if(db_config->view_row_numbers == 1) {
    child_frame->GridFrame()->m_grid->SetRowLabelSize(0);
    db_config->view_row_numbers = 0;
  }
  else {
    child_frame->GridFrame()->m_grid->SetRowLabelAlignment(wxCENTRE, -1);
    child_frame->GridFrame()->m_grid->SetRowLabelSize(child_frame->DBParms()->row_label_size);
    db_config->view_row_numbers = 1;
  }

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteViewRowNumbers(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving view row numbers value" << "\n";
  }
}

void MainFrame::ToggleColLabels(wxCommandEvent& event)
{
  CryptDBDocument *child_frame = ActiveChild();
  if(!child_frame) {
    ProgramError->Message("No database is currently open\n");
    return;
  }
  gxDatabaseConfig *db_config = &child_frame->DBParms()->db_config;

  if(db_config->view_labels == 1) {
    child_frame->GridFrame()->m_grid->SetColLabelSize(0);
    db_config->view_labels = 0;
  }
  else {
    child_frame->GridFrame()->m_grid->SetColLabelSize(child_frame->DBParms()->column_label_size);
    db_config->view_labels = 1;
  }

  // Make this setting persistent by writing it to the dbconfig
  if(!db_config->WriteViewLabels(child_frame->DBParms()->pod->OpenDataFile())) {
    *(frame->statusWin) << "Error saving view column labels value" << "\n";
  }
}

void MainFrame::OnToggleToolbar(wxCommandEvent& WXUNUSED(event))
{
  wxToolBar *tbar = GetToolBar();

  if(!tbar) {
    RecreateToolbar();
    gxConfig CfgData(progcfg->cfgFile.c_str());      
    CfgData.ChangeConfigValue("ViewToolBar", "TRUE");
  }
  else {
    delete tbar;
    SetToolBar(NULL);
    gxConfig CfgData(progcfg->cfgFile.c_str());      
    CfgData.ChangeConfigValue("ViewToolBar", "FALSE");
  }
}
// --------------------------------------------------------------

// ----------------------------------------------------------- //
// 10/27/2026: Phase 3 main window commands
// ----------------------------------------------------------- //
void MainFrame::OnLock(wxCommandEvent& WXUNUSED(event))
{
  if(progcfg->child_frame_list.IsEmpty()) return;
  LockAllDatabases("Locked: the open databases were closed and their keys wiped");
  SetStatusText("Locked", 0);
}

static CryptDBDocument *EPDBSelectedRecord(MainFrame *mf, int &row)
{
  row = -1;
  CryptDBDocument *doc = mf->ActiveChild();
  if(!doc || !doc->GridFrame()) return 0;
  row = doc->CurrentRecordRow();
  return row >= 0 ? doc : 0;
}

void MainFrame::OnCopyUser(wxCommandEvent& WXUNUSED(event))
{
  int row;
  CryptDBDocument *doc = EPDBSelectedRecord(this, row);
  if(!doc) return;
  if(CopyToClipboard(doc->GetGrid()->GetCellValue(row, 1))) SetStatusText("Username copied to the clipboard", 0);
}

void MainFrame::OnCopyPassword(wxCommandEvent& WXUNUSED(event))
{
  int row;
  CryptDBDocument *doc = EPDBSelectedRecord(this, row);
  if(!doc) return;
  if(CopyToClipboard(doc->GetGrid()->GetCellValue(row, 2))) {
    wxString m = "Password copied to the clipboard";
    if(progcfg->clipboard_clear_seconds > 0) m << wxString::Format(", cleared in %d seconds", progcfg->clipboard_clear_seconds);
    SetStatusText(m, 0);
  }
}

void MainFrame::OnOpenLink(wxCommandEvent& WXUNUSED(event))
{
  int row;
  CryptDBDocument *doc = EPDBSelectedRecord(this, row);
  if(!doc) return;
  wxString url = doc->GetGrid()->GetCellValue(row, 6);
  url.Trim().Trim(false);
  if(url.IsEmpty()) {
    SetStatusText("The record has no website link", 0);
    return;
  }
  gxString u = (const char *)url.utf8_str();
  LaunchURL(u, this);
}

void MainFrame::OnFindFocus(wxCommandEvent& WXUNUSED(event))
// The search box of the active database, or the Find dialog
{
  CryptDBDocument *doc = ActiveChild();
  if(doc && doc->search_ctrl) {
    doc->search_ctrl->SetFocus();
    doc->search_ctrl->SelectAll();
    return;
  }
  wxCommandEvent e(wxEVT_MENU, WXAPPFW_DATABASE_FIND);
  ProcessEvent(e);
}

void MainFrame::OnToggleDetails(wxCommandEvent& event)
{
  progcfg->show_details = event.IsChecked() ? 1 : 0;
  GUISaveConfigValue("ShowDetailsPane", progcfg->show_details ? "1" : "0");
  for(gxListNode<CryptDBDocument *> *p = progcfg->child_frame_list.GetHead(); p; p = p->next) {
    p->data->ShowDetails(progcfg->show_details);
  }
}

void MainFrame::OnCommandBuilder(wxCommandEvent& event)
{
  CommandBuilderDialog dlg(this, ActiveChild(), event.GetInt() == 1);
  dlg.ShowModal();
}

void MainFrame::OnGeneratePassword(wxCommandEvent& WXUNUSED(event))
{
  GeneratePasswordDialog dlg(this, 0);
  dlg.ShowModal();
}

void MainFrame::UpdateStatusFields()
{
  if(!GetStatusBar() || GetStatusBar()->GetFieldsCount() < 5) return;
  CryptDBDocument *doc = ActiveChild();
  if(!doc || !doc->GridFrame() || !doc->DBParms()->pod) {
    SetStatusText("No database open", 1);
    SetStatusText("", 2);
    SetStatusText("", 3);
    SetStatusText("", 4);
    return;
  }
  SetStatusText(doc->unlock_description, 1);
  int n = doc->GridFrame()->NumRecords();
  SetStatusText(wxString::Format("%d record%s", n, n == 1 ? "" : "s"), 2);
  SetStatusText(EPDBIsKeyset(doc->DBParms()->crypt_key) ? "Format v2" : "Format v1 (upgrade)", 3);
  if(progcfg->auto_lock_minutes > 0) {
    long left = (long)(progcfg->auto_lock_minutes * 60) - (long)(time(0) - last_activity);
    if(left < 0) left = 0;
    SetStatusText(wxString::Format("Auto-lock in %ld:%02ld", left / 60, left % 60), 4);
  }
  else {
    SetStatusText("Auto-lock off", 4);
  }
}

void MainFrame::OnStatusTimer(wxTimerEvent& WXUNUSED(event))
{
  UpdateStatusFields();
}

void MainFrame::OnUpdateNeedsRecord(wxUpdateUIEvent& event)
{
  int row;
  event.Enable(EPDBSelectedRecord(this, row) != 0);
}

void MainFrame::OnUpdateNeedsDatabase(wxUpdateUIEvent& event)
{
  CryptDBDocument *doc = ActiveChild();
  event.Enable(doc != 0 && doc->GridFrame() != 0);
}

int MainFrame::DatabaseIsOpen(gxString &dbname)
{
  gxListNode<CryptDBDocument *> *ptr = progcfg->child_frame_list.GetHead();

  while(ptr) {
#if defined (__WIN32__) 
    // NOTE: Under Windows the file open dialog box will
    // mix upper and lower case file and dir names.
    gxString sbuf1 = ptr->data->DBParms()->data_file;
    gxString sbuf2 = dbname;
    sbuf1.ToUpper(); sbuf2.ToUpper();
    if(sbuf1 == sbuf2) return 1;
#else 
    if(ptr->data->DBParms()->data_file == dbname) return 1;
#endif
    ptr = ptr->next;
  } 
  return 0;
}

CryptDBDocument *MainFrame::get_active_child()
{
  wxMDIChildFrame *child_frame = GetActiveChild();
  gxListNode<CryptDBDocument *> *ptr = progcfg->child_frame_list.GetHead();

  while(ptr) {
    wxMDIChildFrame *cf_ptr = (wxMDIChildFrame *)ptr->data;
    if(child_frame == cf_ptr) {
      DBStringConfig::crypt_key = ptr->data->DBParms()->crypt_key;
      return ptr->data;
    }
    ptr = ptr->next;
  } 
  return 0;
}

CryptDBDocument *MainFrame::set_active_child()
{
  return progcfg->active_child_frame = get_active_child();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

