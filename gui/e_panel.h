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

Edit panel
*/
// ----------------------------------------------------------- // 
#ifndef __E_PANEL_HPP__
#define __E_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define EDIT_TAB_PANEL_ID 990
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Main panel IDs 
  ID_EDITTABPANEL_ACCEPT = 991,
  ID_EDITTABPANEL_CLOSE,
  ID_EDITTABPANEL_CANCEL,
  ID_EDITTABPANEL_DEFAULT,
  ID_EDITTABPANEL_DIRECTORY,
  ID_EDITTABPANEL_NOTEBOOK
};
// --------------------------------------------------------------

class EditTabPanel: public wxDialog
{
public:
  EditTabPanel(wxWindow *parent, wxWindowID id, char *title,
	       int xpos, int ypos, int width, int height,
	       long style = wxDEFAULT_DIALOG_STYLE,
	       char* name = (char *)"dialogBox");
  ~EditTabPanel();

public: // Event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));
  void OnAccept(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnDefault(wxCommandEvent &event);
  void OnBrowseDirectory(wxCommandEvent &event);

public: // Helper functions
  void LoadPanel();
  void ShowPanel();
  void ClosePanel();

private: // Internal processing functions
  int CommitChanges();
  int HasChanged();
  void Init();

public:
  // Tab member functions
  wxNotebook* notebook;

  // Panel conrol buttons
  wxButton *accept_btn;
  wxButton *close_btn;
  wxButton *cancel_btn;
  wxButton *default_btn;

  // Tab panels
  wxPanel *panel1;

  // Database directory controls
  wxStaticText *directory_label;
  wxTextCtrl *directory_input;
  wxButton *directory_btn;

#ifdef __USE_DB_ENCRYPTION__
  // Security controls
  wxPanel *panel2;
  wxStaticText *ekey_label;
  wxRadioBox *mode_radio_box;
#endif

  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
EditTabPanel *InitEditTabPanel(wxWindow *parent);

#endif // __E_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
