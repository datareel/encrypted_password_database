// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
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
#endif
  EVT_MENU(WXAPPFW_EDIT_PREFERENCES, MainFrame::OnPreferences)

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
  SetIcon(wxICON(wxappfw));
  SetTitle(progcfg->ProgramName.c_str());
  
  // Make the menubar bars
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(WXAPPFW_FILE_NEW, "New Database", 
		    "Create a new database");
  file_menu->Append(WXAPPFW_FILE_OPEN, "Open Database",
		   "Open an existing database");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_IMPORT, "Import Database",
		   "Import a text file into this database");
  file_menu->Append(WXAPPFW_FILE_EXPORT, "Export Database",
		   "Export this database to a text file");
  file_menu->Append(WXAPPFW_FILE_TEMPLATE, "Create DB Text Template",
		   "Create an import/export database text template");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_BACKUP, "Backup Database",
		   "Create a backup copy of this database");
  file_menu->Append(WXAPPFW_FILE_MERGE, "Merge Database",
		   "Merge an existing database with this database");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_COMPARE, "Test Database",
		   "Test the current database");
  file_menu->Append(WXAPPFW_FILE_REBUILD, "Rebuild Database",
		   "Rebuild the current database");
  file_menu->Append(WXAPPFW_FILE_REBUILDCONFIG, "Rebuild Database Config",
		   "Rebuild the current database config header");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_FILE_PROPERTIES, "View Database Stats",
		   "Display the current database statistics");
  file_menu->AppendSeparator();
  file_menu->Append(WXAPPFW_APP_EXIT, "Exit", "Exit this program");

  // Edit menu
  wxMenu *edit_menu = new wxMenu("", wxMENU_TEAROFF);
#ifdef __USE_CLIPBOARD_FUNCTIONS__
  edit_menu->Append(WXAPPFW_EDIT_COPY, "Copy Cell", 
		    "Copy cell to clipboard");
  edit_menu->Append(WXAPPFW_EDIT_CUT, "Cut Cell", 
		    "Cut cell and copy to clipboard");
  edit_menu->Append(WXAPPFW_EDIT_PASTE, "Paste Cell", 
		    "Paste cell from clipboard");
  edit_menu->AppendSeparator();
#endif
  edit_menu->Append(WXAPPFW_EDIT_PREFERENCES, "Preferences",
		    "Edit user preferences");

  // Database menu
  wxMenu *database_menu = new wxMenu("", wxMENU_TEAROFF);
  database_menu->Append(WXAPPFW_DATABASE_FIND, "&Find\tAlt-F",
			"Find an object in the database");
  database_menu->AppendSeparator();
  database_menu->Append(WXAPPFW_DATABASE_ADD, "&Add\tAlt-A",
			"Add an object to the database");
  database_menu->Append(WXAPPFW_DATABASE_CHANGE, "&Change\tAlt-C",
			"Change selected table item");
  database_menu->Append(WXAPPFW_DATABASE_DELETE, "&Delete\tAlt-D",
			"Delete selected table item");
  database_menu->AppendSeparator();
  database_menu->Append(WXAPPFW_DATABASE_DISPLAY, "Display",
			"Display the database");

  // View menu
  wxMenu *view_menu = new wxMenu("", wxMENU_TEAROFF);
  view_menu->Append(WXAPPFW_VIEW_STATUS, "View Status Window",
		    "View the status window");
  view_menu->Append(WXAPPFW_VIEW_CLOSE, "Close Status Window",
		    "Close the status window");
  view_menu->Append(WXAPPFW_VIEW_CLEAR, "Clear Status Window",
		    "Clear the status window");
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_TOOLBAR, "View Toolbar",
		    "Toggle the toolbar on and off", TRUE);
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_ROWLABELS, "View Row Labels",
		    "View the database row labels");
  view_menu->Append(WXAPPFW_VIEW_COLLABELS, "View Column Labels",
		    "View the database column labels");
  view_menu->Append(WXAPPFW_VIEW_AUTOSIZE, "Auto size cells",
		    "Auto size cells to fit content");
  view_menu->Append(WXAPPFW_VIEW_DISABLE_AUTOSIZE, "Disable auto size",
		    "Disable cell auto size when database is opened");
  view_menu->Append(WXAPPFW_VIEW_CELLOVERFLOW, "Toggle cell overflow",
		    "Allow cell content to overflow to next cell");
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_GRIDLINES, "Toggle grid lines",
		    "View database grid lines");
  view_menu->Append(WXAPPFW_VIEW_GRIDLINECOLOR, "Grid line color",
		    "Set the grid line color lines");
  view_menu->Append(WXAPPFW_VIEW_GRIDBACKGROUNDCOLOR, "Grid background color",
		    "Set the grid background color for all cells");
  view_menu->Append(WXAPPFW_VIEW_GRIDLABELBACKGROUNDCOLOR, "Grid label background color",
		    "Set the grid label background color");
  view_menu->Append(WXAPPFW_VIEW_GRIDLABELTEXTCOLOR, "Grid label text color",
		    "Set the grid label text color");
  view_menu->Append(WXAPPFW_VIEW_GRIDLABELFONT, "Grid label font",
		    "Set the grid label font");
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_GRIDTEXTCOLOR, "Grid text color",
		    "Set the grid text color for all cells");
  view_menu->Append(WXAPPFW_VIEW_GRIDTEXTFONT, "Grid text font",
		    "Set the grid text font for all cells");
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_SKIPLINES, "Toggle skip lines",
		    "View database skip lines");
  view_menu->Append(WXAPPFW_VIEW_SKIPLINECOLOR, "Skip line color",
		    "Set the skip line color lines");
  view_menu->AppendSeparator();
  view_menu->Append(WXAPPFW_VIEW_HYPERLINKCOLOR, "Hyperlink text color",
		    "Set the hyperlink text color for all cells");
  view_menu->Append(WXAPPFW_VIEW_HYPERLINKFONT, "Hyperlink text font",
		    "Set the hyperlink text font for all cells");

  // Print menu
  wxMenu *print_menu = new wxMenu("", wxMENU_TEAROFF);
#ifdef __USE_MSW_PRINTING__
  print_menu->Append(WXAPPFW_PRINT_MSWPRINT, "Print",
		     "Print the database to a selected printer");
  print_menu->Append(WXAPPFW_PRINT_MSWPREVIEW, "Preview",
		     "Preview the document before printing");
  print_menu->Append(WXAPPFW_PRINT_MSWPAGESETUP, "Page Setup",
		     "Setup the page options");
  print_menu->Append(WXAPPFW_PRINT_MSWSETUP, "Printer Setup",
		     "Setup the printer options");
#endif // __USE_MSW_PRINTING__

#ifdef __USE_APP_POSTSCRIPT_PRINTING__
  print_menu->AppendSeparator();
  print_menu->Append(WXAPPFW_PRINT_PSPRINT, "Print to Postscript Printer",
		     "Print the database to a postscript printer");
  print_menu->Append(WXAPPFW_PRINT_PSPAGESETUP, "Postscript Page Setup",
		     "Setup the postscript page options");
  print_menu->Append(WXAPPFW_PRINT_PSSETUP, "Line Printer Setup",
		     "Setup postscript line printer options");
  print_menu->AppendSeparator();
#endif // __USE_POSTSCRIPT_PRINTING__

#if defined (__USE_APP_TEXT_PRINTING__)
  // Text file and HTML printer options
  print_menu->Append(WXAPPFW_PRINT_ASPRINT, "Print to text file",
		     "Print the database to a text file");
  print_menu->AppendSeparator();
#endif

#ifdef __USE_APP_HTM_PRINTING__
  print_menu->Append(WXAPPFW_PRINT_HTMPRINT, "Print to HTML file",
		     "Print the database to an HTML file");
#endif

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(WXAPPFW_HELP_ABOUT, "&About\tF1");

  wxMenu *users_menu = new wxMenu;
  users_menu->Append(WXAPPFW_USERS_ADD, "Add Users", "Add user access with public RSA key or smart card cert");
  users_menu->Append(WXAPPFW_USERS_REMOVE, "Remove Users", "Removed user access");
  users_menu->Append(WXAPPFW_USERS_LIST, "List Users", "Display list of users with access");
  
  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, "File");
  menu_bar->Append(edit_menu, "Edit");
  menu_bar->Append(database_menu, "Database");
  menu_bar->Append(view_menu, "View");
  menu_bar->Append(print_menu, "Print");
  menu_bar->Append(users_menu, "Users");
  menu_bar->Append(help_menu, "Help");
  
  // Associate the menu bar with the frame
  SetMenuBar(menu_bar);

  // Set the menu defaults
  if(progcfg->ViewToolBar == 1) {
    GetMenuBar()->Check(WXAPPFW_VIEW_TOOLBAR, TRUE);
  }
  else {
    GetMenuBar()->Check(WXAPPFW_VIEW_TOOLBAR, FALSE);
  }

  // Panel, label and text fonts
  labelFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
  itemFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
  textWindowFont = new wxFont(10, wxMODERN, wxNORMAL, wxNORMAL);
  hyperlinkFont = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
  gridlineColor = new wxColour(0, 0, 0);
  cellFontColor = new wxColour(0, 0, 0);
  textreadonlyColor = new wxColour(105, 105, 105);
  hyperlinkColor = new wxColour(0, 0, 255);

#if wxUSE_STATUSBAR
  // Create a status bar (by default with 1 pane only)
  CreateStatusBar(2);
  SetStatusText(progcfg->ProgramName.c_str());
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

  open_db_panel = InitOpenDatabasePanel(this);

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
}

void MainFrame::InitStatusWin()
{
  statusWin = sframe->status_win;
  *(statusWin) << "Database Engine: Idle" << "\n";
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

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  gxString msg;

  msg << clear << progcfg->ProgramName << " " << progcfg->VersionString << "\n"
      << "Copyright (c) " <<  progcfg->produced_by << " " << progcfg->copyright_dates << "\n"
      << "Produced by: " << progcfg->produced_by << "\n"
      <<  progcfg->default_url << "\n"
      << "\n"
      << "Release date: " << progcfg->release_date << "\n"
      << "\n"
      << "Designed for " << progcfg->Platform << "\n"
      << "GUI Framework: " << wxGetLibraryVersionInfo().GetVersionString() << "\n"
      << "\n"
      << "Encryption engine: Openssl" << "\n"
      << "Version string: " << OPENSSL_VERSION_TEXT << "\n"
      << "Version number: 0x" << hex << OPENSSL_VERSION_NUMBER << "\n"
#ifdef __ENABLE_SMART_CARD__
      << "\n"
      << "Smart card enabled for " << SC_get_default_engine_ID() << "\n"
#else
      << "\n"
      << "Smart card support is not enabled" << "\n"
#endif
      << "\n";



  wxMessageBox(msg.c_str(), "About Box", wxOK | wxICON_INFORMATION, this);
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
    }

    if(add_user_panel->use_smartcard) {
      db_auth.f = child_frame->DBParms()->pod->OpenDataFile();
       rv = db_auth.AddSmartCardCertToStaticArea(&add_user_panel->sc, add_user_panel->use_cert_file,
						 child_frame->DBParms()->crypt_key, add_user_panel->smartcard_username);
       if(rv != 0) {
	 sbuf << clear << "ERROR: Cannot add smart card cert " << db_auth.err.c_str();
	 ProgramError->Message(sbuf.c_str());
	 return;
       }
       sbuf << clear << "Added smart card access for user " << add_user_panel->rsa_key_username;
      ProgramError->Message(sbuf.c_str());
    }
  }
  
  return;
}

void MainFrame::OnRemoveUsers(wxCommandEvent& event)
{
  gxString sbuf;
  DatabaseUserAuth db_auth;
  int rv;
  unsigned i;
  
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
    ProgramError->Message("No authorized RSA or smartcard users found in encrypted DB file");
    return;
  }

  wxTextEntryDialog dialog(this, "Enter the username to remove", "Remove database access for user", "", wxOK | wxCANCEL);
  if(dialog.ShowModal() == wxID_OK) {

    if(dialog.GetValue().IsNull()) {
      ProgramError->Message("You must enter a username");
      return;
    }
    sbuf = (const char *)dialog.GetValue();
    for(i = 0; i < NUM_DB_AUTH_TYPES; i++) { // Check for all user auth types
      list_ptr = db_auth.static_block_list.GetHead();
      while(list_ptr) {
	if(list_ptr->data.username == sbuf) break;
	list_ptr = list_ptr->next;
      }
      db_auth.static_block_list.Remove(list_ptr);
    }
    db_auth.UpdateStaticData();
    if(db_auth.WriteStaticDataArea() != 0) {
      sbuf << clear << "ERROR: User " << (const char *)dialog.GetValue() << " was not removed" << "\n" << db_auth.err;
      ProgramError->Message(sbuf.c_str());
    }
    else {
      sbuf << clear << "Access for User " << (const char *)dialog.GetValue() << " was removed";
      ProgramError->Message(sbuf.c_str());
    }
  }
  else {
    sbuf << clear << "User " << (const char *)dialog.GetValue() << " was not removed";
    ProgramError->Message(sbuf.c_str());
  }
  
  return;
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
    *(frame->statusWin) << "INFO: No authorized RSA or smartcard users found in encrypted DB file" << "\n";
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
void MainFrame::OnCopy(wxCommandEvent& event)
{
  ProgramError->Message("This feature has not be fully implemented yet.");
  return;

  // NOTE: Example code to read some text
  // wxClipboard cb;
  // wxClipboard *wxTheClipboard = &cb;
  // if(wxTheClipboard->Open()) {
  //  if(wxTheClipboard->IsSupported( wxDF_TEXT )) {
  //    wxTextDataObject data;
  //    wxTheClipboard->GetData( data );
  //    wxMessageBox( data.GetText() );
  //  }  
  //  wxTheClipboard->Close();
  // }
}

void MainFrame::OnCut(wxCommandEvent& event)
{
  ProgramError->Message("This feature has not be fully implemented yet.");
  return;
}

void MainFrame::OnPaste(wxCommandEvent& event)
{
  ProgramError->Message("This feature has not be fully implemented yet.");
  return;

  // NOTE: Example code to write some text to the clipboard
  //if (wxTheClipboard->Open())
  // {
  // This data objects are held by the clipboard, 
  // so do not delete them in the app.
  //  wxTheClipboard->SetData( new wxTextDataObject("Some text") );
  //  wxTheClipboard->Close();
  // }
}
#endif // __USE_CLIPBOARD_FUNCTIONS__

void MainFrame::OnPreferences(wxCommandEvent& event)
{
  e_tab_panel->ShowPanel();
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
#ifdef __APP_DEBUG_VERSION__
  *(statusWin) << "Opening an existing database" << "\n";
#endif

  gxString work_dir = (const char *)::wxGetCwd().c_str();
  ::wxSetWorkingDirectory(progcfg->docDir.c_str());

  gxDatabaseParms def_parms;

  wxString sbuf("*");
  sbuf += def_parms.data_file_extension.c_str();
  wxFileDialog dialog(this, "Open an existing database:",
		      progcfg->docDir.c_str(), "",
		      sbuf,
		      wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  
  if(dialog.ShowModal() == wxID_OK) {
    // Set the config file name of the programs main database
    gxString database_file((const char *)dialog.GetPath().c_str());
    database_file.DeleteAfterLastIncluding(".");
    database_file << def_parms.data_file_extension;

    if(DatabaseIsOpen(database_file)) {
      ProgramError->Message("The database is already open");
      ::wxSetWorkingDirectory(work_dir.c_str());
      return;
    }

    if(!futils_exists(database_file.c_str())) {
      ProgramError->Message("The database file does not exist");
      ::wxSetWorkingDirectory(work_dir.c_str());
      return;
    }

    // OpenDatabasePanel code is in the x_panel.h and x_panel.cpp modules
    open_db_panel->ShowPanel(database_file);

    if(!open_db_panel->IsOK()) {
      ::wxSetWorkingDirectory(work_dir.c_str());
      return;
    }

    // Make another frame, containing a canvas
    CryptDBDocument *subframe = new CryptDBDocument(frame, progcfg->ProgramName.c_str(),
						    wxPoint(-1, -1), 
						    wxSize(-1, -1),
						    wxDEFAULT_FRAME_STYLE);
    progcfg->num_child_frames++;
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("doc"));
#else
    subframe->SetIcon(wxIcon(doc_xpm));
#endif
    
    int width, height;
    subframe->GetClientSize(&width, &height);
    CryptDBGrid *grid_frame = new CryptDBGrid(subframe, 
					      wxPoint(0, 0), 
					      wxSize(width, height));
    subframe->grid_frame = grid_frame;

    subframe->DBParms()->crypt_key = progcfg->global_dbparms.crypt_key;
    DBStringConfig::crypt_key = progcfg->global_dbparms.crypt_key;

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
    if(!futils_exists(dialog.GetPath())) {
      ProgramError->Message("The database file does not exist");
    }
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

    gxString dbname = (const char *)new_db_panel->name_input->GetValue().c_str();
    
    gxString data_file = dbname;
    data_file.DeleteAfterLastIncluding(".");
    data_file += progcfg->global_dbparms.data_file_extension;
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
    subframe->SetTitle((const char *)new_db_panel->name_input->GetValue().c_str());
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("doc"));
#else
    subframe->SetIcon(wxIcon(doc_xpm));
#endif
    
    int width, height;
    subframe->GetClientSize(&width, &height);
    CryptDBGrid *grid_frame = new CryptDBGrid(subframe, 
					      wxPoint(0, 0), 
					      wxSize(width, height));
    subframe->grid_frame = grid_frame;
    grid_frame->InitGrid(); // Make a blank grid with no entries

    // Open the database associated with this child frame
    int rv;
    gxString sbuf;
    DatabaseUserAuth db_auth;
    CryptDBDocument *prev_frame = progcfg->active_child_frame; 
    progcfg->active_child_frame = subframe;

    subframe->DBParms()->crypt_key = progcfg->global_dbparms.crypt_key;
    DBStringConfig::crypt_key = progcfg->global_dbparms.crypt_key;

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
    child_frame->GridFrame()->m_grid->BeginBatch();
    for(int n = 0; n < child_frame->GridFrame()->m_grid->GetNumberRows();) {
      if(child_frame->GridFrame()->m_grid->IsInSelection(n , 0)) {
	if(child_frame->GridFrame()->m_grid->GetCellValue(n, 0) != "") {
	  key = child_frame->GridFrame()->m_grid->GetCellValue(n, 0);
	  infohog.SetMember(&key, sizeof(key), 0);
	  if(!infohog.DeleteObject()) {
	    *(statusWin) << "Could not find item: "
			 << child_frame->GridFrame()->m_grid->GetCellValue(n, 0)
			 << " in the database" << "\n";
	  }
	  else {

	    // Update the status window
	    *(statusWin) << "Deleted item: " << child_frame->GridFrame()->m_grid->GetCellValue(n, 0)
			 << "\n";
	  }
	}
	child_frame->GridFrame()->m_grid->DeleteRows(n, 1);
      }
      else {
	n++;
      }
    }
    child_frame->GridFrame()->m_grid->EndBatch();
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
    }
  }

#ifdef __APP_DEBUG_VERSION__
  *(statusWin) << "Current row " << child_frame->GridFrame()->dbparms.current_row << "\n";
  *(statusWin) << "Number of rows " << child_frame->GridFrame()->m_grid->GetRows() << "\n";
#endif
  
  // Delete the row and update the "current_name" varible
  if(child_frame->GridFrame()->m_grid->GetRows() == 1) { // There is only one row in this table
    child_frame->GridFrame()->m_grid->DeleteRows(child_frame->GridFrame()->dbparms.current_row);
    child_frame->GridFrame()->dbparms.current_name = "";
    return;
  }
  else if(child_frame->GridFrame()->dbparms.current_row == child_frame->GridFrame()->m_grid->GetRows()-1) { // At the last row
    child_frame->GridFrame()->m_grid->DeleteRows(child_frame->GridFrame()->dbparms.current_row);
    child_frame->GridFrame()->dbparms.current_row--; // Use the previous name value
    child_frame->GridFrame()->dbparms.current_name = child_frame->GridFrame()->m_grid->GetCellValue(child_frame->GridFrame()->dbparms.current_row, 0);
  }
  else {
    child_frame->GridFrame()->m_grid->DeleteRows(child_frame->GridFrame()->dbparms.current_row);
    if(child_frame->GridFrame()->dbparms.current_row == child_frame->GridFrame()->m_grid->GetRows()-1) {
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
    wxMessageBox("There was a initializing the printer.\nPerhaps your current \
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
    wxMessageBox("There was a initializing the printer.\nPerhaps your current \
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
  preview_frame->SetIcon(wxICON(wxappfw));
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
    int col_width = child_frame->GetGrid()->GetColumnWidth(i);
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

