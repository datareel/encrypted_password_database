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

Line printer setup panel
*/
// ----------------------------------------------------------- // 
#ifndef __LINE_PRINTER_PANEL_HPP__
#define __LINE_PRINTER_PANEL_HPP__

#include "app_defs.h"

#ifdef __USE_POSTSCRIPT_PRINTING__

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define LINEPRINTER_PANEL_ID 9821
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Page setup panel IDs 
  ID_LINEPRINTER_PRINT = 9822,
  ID_LINEPRINTER_CANCEL,
  ID_LINEPRINTER_CLOSE,
  ID_LINEPRINTER_DEFAULT
};
// --------------------------------------------------------------

class LinePrinterPanel : public wxDialog
{
public:
  LinePrinterPanel(wxWindow *parent, wxWindowID id, char *title,
		   int xpos, int ypos, int width, int height,
		   long style = wxDEFAULT_DIALOG_STYLE,
		   char* name = (char *)"dialogBox");
  ~LinePrinterPanel();
  
public: // Event handlers
  void OnPrint(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnClose(wxCommandEvent &event);
  void OnDefault(wxCommandEvent &event);

public: // Helper Functions
  void ShowPanel();

public:
  wxStaticText *lp_command_label;
  wxStaticText *lp_device_label;
  wxStaticText *lp_tempfile_label;
  wxTextCtrl *lp_command_input;
  wxTextCtrl *lp_device_input;
  wxTextCtrl *lp_tempfile_input;
  wxCheckBox *lp_printtofile_ckbox;
  wxButton *print_btn;
  wxButton *close_btn;
  wxButton *cancel_btn;
  wxButton *default_btn;

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
LinePrinterPanel *InitLinePrinterPanel(wxWindow *parent);

#endif // __USE_POSTSCRIPT_PRINTING__

#endif // __LINE_PRINTER_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
