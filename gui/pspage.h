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

Page setup panel
*/
// ----------------------------------------------------------- // 
#ifndef __PS_PAGE_PANEL_HPP__
#define __PS_PAGE_PANEL_HPP__

#include "app_defs.h"

#ifdef __USE_APP_POSTSCRIPT_PRINTING__

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define PSPAGE_PANEL_ID 981110
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Page setup panel IDs 
  ID_PSPAGE_ACCEPT = 981111,
  ID_PSPAGE_CANCEL,
  ID_PSPAGE_CLOSE,
  ID_PSPAGE_DEFAULT
};
// --------------------------------------------------------------

class PSPagePanel : public wxDialog
{
public:
  PSPagePanel(wxWindow *parent, wxWindowID id, char *title,
	   int xpos, int ypos, int width, int height,
	   long style = wxDEFAULT_DIALOG_STYLE,
	   char* name = "dialogBox");
  ~PSPagePanel();
  
public: // Event handlers
  void OnAccept(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnDefault(wxCommandEvent &event);

public:
  wxStaticText *lpp_label;
  wxStaticText *cell_label;
  wxStaticText *fs_label;
  wxStaticText *lm_label;
  wxStaticText *orientation_label;
  wxStaticText *font_label;
  wxStaticText *paper_label;
  wxStaticText *doc_name_label;
  wxSlider *lpp_slider;
  wxSlider *cell_slider;
  wxSlider *fs_slider;
  wxSlider *lm_slider;
  wxChoice *orientation_choice;
  wxChoice *font_choice;
  wxChoice *paper_choice;
  wxTextCtrl *doc_name_input;
  wxButton *accept_btn;
  wxButton *close_btn;
  wxButton *cancel_btn;
  wxButton *default_btn;

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
PSPagePanel *InitPSPagePanel(wxWindow *parent);

#endif // __USE_APP_POSTSCRIPT_PRINTING__

#endif // __PS_PAGE_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
