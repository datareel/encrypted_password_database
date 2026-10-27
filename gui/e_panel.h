// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2025 DataReel Software Development
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

#define EDIT_TAB_PANEL_ID 990

// 10/27/2026: Preferences, rewritten with sizers: File Locations,
// Security (encryption strength, clipboard clearing, auto-lock, access
// log) and Appearance tabs
class EditTabPanel: public wxDialog
{
public:
  EditTabPanel(wxWindow *parent);
  ~EditTabPanel();

public: // Event handlers
  void OnCloseWindow(wxCloseEvent &event);
  void OnAccept(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnDefault(wxCommandEvent &event);
  void OnBrowseDirectory(wxCommandEvent &event);

public: // Helper functions
  void LoadPanel();
  void ShowPanel();

private: // Internal processing functions
  int CommitChanges();

public:
  wxNotebook *notebook;
  wxTextCtrl *directory_input;
  wxRadioBox *mode_radio_box;
  wxSpinCtrl *clipboard_seconds;
  wxSpinCtrl *lock_minutes;
  wxCheckBox *access_log;
  wxCheckBox *show_passwords;
  wxCheckBox *show_details;
  wxCheckBox *show_toolbar;

  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
EditTabPanel *InitEditTabPanel(wxWindow *parent);

#endif // __E_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
