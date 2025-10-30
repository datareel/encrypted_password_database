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

Find panel
*/
// ----------------------------------------------------------- // 
#ifndef __F_PANEL_HPP__
#define __F_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define FIND_PANEL_ID 2841
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Add panel IDs 
  ID_FIND_SEARCH = 2842,
  ID_FIND_CLOSE,
  ID_FIND_CHOICEBOX1,
  ID_FIND_STATICTEXT1,
  ID_FIND_STATICTEXT2,
  ID_FIND_TEXTCONTROL1
};
// --------------------------------------------------------------

class FindPanel : public wxDialog
{
public:
  FindPanel(wxWindow *parent, wxWindowID id, char *title,
	   int xpos, int ypos, int width, int height,
	   long style = wxDEFAULT_DIALOG_STYLE,
	    char* name = (char *)"dialogBox");
  ~FindPanel();
  
public: // Event handlers
  void OnSearch(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnTextControl1Enter(wxCommandEvent &event);

public: // Helper functions
  void StartSearch();
  void ShowPanel();

public:
  wxTextCtrl *db_input;              // Text input
  wxStaticBox *key_name_label;       // Key data member label
  wxStaticText *search_choice_lable; // Choice box lable
  wxChoice *search_choicebox;        // Choice box
  wxButton *search_btn;              // Search button
  wxButton *close_btn;               // Close button

  CryptDBDocument *child_frame;

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
FindPanel *InitFindPanel(wxWindow *parent);

#endif // __F_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
