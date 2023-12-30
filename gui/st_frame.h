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

Status frame
*/
// ----------------------------------------------------------- // 
#ifndef __ST_FRAME_HPP__
#define __ST_FRAME_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define STATUS_FRAME_ID 1311
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------

// --------------------------------------------------------------

class StatusFrame : public wxFrame
{
public:
  StatusFrame(wxWindow* parent = NULL, wxWindowID id = -1,
	      const wxString& title = "wxWin App",
	      const wxPoint& pos = wxDefaultPosition,
	      const wxSize& size = wxDefaultSize,
	      long style = wxDEFAULT_FRAME_STYLE);
  
  ~StatusFrame();

public: // Event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));

public: // Helper functions
  void WriteMessage(const wxString &message);

public:
  wxTextCtrl *status_win;     // Status windows

private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
StatusFrame *InitStatusFrame(wxWindow* parent, wxFont *textWindowFont);

#endif // __ST_FRAME_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
