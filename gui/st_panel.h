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

Status panel
*/
// ----------------------------------------------------------- // 
#ifndef __ST_PANEL_HPP__
#define __ST_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define STATUS_PANEL_ID 110
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Status panel IDs 
  ID_STATUS_CANCEL = 111
};
// --------------------------------------------------------------

class StatusPanel : public wxDialog
{
public:
  StatusPanel(wxWindow *parent, wxWindowID id, char *title,
	   int xpos, int ypos, int width, int height,
	   long style = wxDEFAULT_DIALOG_STYLE,
	      char* name = (char *)"dialogBox");
  ~StatusPanel();
  
public: // Event handlers
  void OnCancel(wxCommandEvent &event);

public:
  void WriteMessage(const wxString &message);
  void Close();
  void ResetPanel();
  void ShowPanel();

public:
  wxStaticBox *control_label; // Outlines the control elements
  wxGauge *status_gauge;      // Progress gauge
  wxTextCtrl *status_win;     // Status windows
  wxButton *cancel_btn;       // Cancel button
  wxFont *text_font;          // Font used in the status window
  int cancel_event; // True if user cancels the event this panel is monitoring
  
private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
StatusPanel *InitStatusPanel(wxWindow *parent);

#endif // __ST_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
