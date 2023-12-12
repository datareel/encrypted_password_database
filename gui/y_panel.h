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

Application panel
*/
// ----------------------------------------------------------- // 
#ifndef __Y_PANEL_HPP__
#define __Y_PANEL_HPP__

#include "app_defs.h"

#ifdef __USE_DB_ENCRYPTION__
// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define OPENDATABASE_PANEL_ID 9845
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Page setup panel IDs 
  ID_OPENDATABASE_OK = 9846,
  ID_OPENDATABASE_CANCEL,
  ID_OPENDATABASE_TEXTCONTROL1
};
// --------------------------------------------------------------

class OpenDatabasePanel : public wxDialog
{
public:
  OpenDatabasePanel(wxWindow *parent, wxWindowID id, char *title,
		   int xpos, int ypos, int width, int height,
		   long style = wxDEFAULT_DIALOG_STYLE,
		    char* name = (char *)"dialogBox");
  ~OpenDatabasePanel();
  
public: // Event handlers
  // Frame event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));

  // Button event handlers
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnTextControl1Enter(wxCommandEvent &event);

public: // Memeber functions
  void ShowPanel(gxString &fname);
  int TestInput();
  int IsOK() { return is_ok == 1; }

public: // Control objects
  wxStaticText *password_label;
  wxTextCtrl *password_input;

  wxButton *ok_btn;
  wxButton *cancel_btn;
  
private: // Data members
  int is_ok;
  gxString curr_fname;

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
OpenDatabasePanel *InitOpenDatabasePanel(wxWindow *parent);

#endif //__USE_DB_ENCRYPTION__

#endif // __Y_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
