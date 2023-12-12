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

Line printer setup panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

#ifdef __USE_POSTSCRIPT_PRINTING__

BEGIN_EVENT_TABLE(LinePrinterPanel, wxDialog)
  EVT_BUTTON    (ID_LINEPRINTER_PRINT, LinePrinterPanel::OnPrint)
  EVT_BUTTON    (ID_LINEPRINTER_CANCEL, LinePrinterPanel::OnCancel)
  EVT_BUTTON    (ID_LINEPRINTER_CLOSE, LinePrinterPanel::OnClose)
  EVT_BUTTON    (ID_LINEPRINTER_DEFAULT, LinePrinterPanel::OnDefault)
END_EVENT_TABLE()

LinePrinterPanel::LinePrinterPanel(wxWindow *parent, wxWindowID id,
				   char *title,
				   int xpos, int ypos, int width, int height,
				   long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  lp_command_label = lp_device_label = lp_tempfile_label = 0;
  lp_command_input = lp_device_input = lp_tempfile_input = 0;
  lp_printtofile_ckbox = 0;
  print_btn = close_btn = cancel_btn = default_btn = 0;
}
  
LinePrinterPanel::~LinePrinterPanel()
{
  if(lp_command_label) delete lp_command_label;
  if(lp_device_label) delete lp_device_label;
  if(lp_tempfile_label) delete lp_tempfile_label;
  if(lp_command_input) delete lp_command_input;
  if(lp_device_input) delete lp_device_input;
  if(lp_tempfile_input) delete lp_tempfile_input;
  if(lp_printtofile_ckbox) delete lp_printtofile_ckbox;
  if(print_btn) delete print_btn;
  if(close_btn) delete close_btn;
  if(cancel_btn) delete cancel_btn;
  if(default_btn) delete default_btn;
}

void LinePrinterPanel::OnClose(wxCommandEvent &WXUNUSED(event))
{
  Show(FALSE);
}

void LinePrinterPanel::ShowPanel()
{
  ShowModal();

  // TODO/NOTE: Must implement in all wxWin 2.6.X code
  // 02/07/2010: Not working on Linux version, so convert to ShowModal() call
  // 02/07/2010: Need to retest in Windows version
  // #if wxCHECK_VERSION(2, 6, 0)
  // ShowModal();
  // #else
  // Show(TRUE);
  // #endif
}

void LinePrinterPanel::OnPrint(wxCommandEvent &WXUNUSED(event))
{

}

void LinePrinterPanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{

}

void LinePrinterPanel::OnDefault(wxCommandEvent &WXUNUSED(event))
{

}

LinePrinterPanel *InitLinePrinterPanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=365;
  LinePrinterPanel *lppanel = new LinePrinterPanel(parent,
						   LINEPRINTER_PANEL_ID,
						   (char *)"Line Printer Setup",
						   xpos, ypos, width, height,
		    wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
						   (char *)"Line Printer Setup");

  if(!lppanel) return 0;

  lppanel->lp_command_label = new wxStaticText(lppanel, -1,
					       "Line printer command",
					       wxPoint(11, 25));
  
  lppanel->lp_command_input = new wxTextCtrl(lppanel, -1,
					     lpConfig->lpCommand,
					     wxPoint(11, 39),
					     wxSize(250,25));
  
  lppanel->lp_device_label = new wxStaticText(lppanel, -1,
					      "Redirect device name",
					      wxPoint(11, 74));
  
  lppanel->lp_device_input = new wxTextCtrl(lppanel, -1,
					    lpConfig->lpCommand,
					     wxPoint(11, 39),
					     wxSize(250,25));
  
  lppanel->lp_device_label = new wxStaticText(lppanel, -1,
					      "Redirect device name",
					      wxPoint(11, 74));
  
  lppanel->lp_device_input = new wxTextCtrl(lppanel, -1,
					    lpConfig->redirectDevice,
					    wxPoint(11, 88),
					    wxSize(250, 25));
  
  lppanel->lp_tempfile_label = new wxStaticText(lppanel, -1,
						"Temp file name",
						wxPoint(11, 123));
  
  lppanel->lp_tempfile_input = new wxTextCtrl(lppanel, -1,
					      lpConfig->tempFile,
					      wxPoint(11, 137),
					      wxSize(250, 25));
					      
					      
  lppanel->lp_printtofile_ckbox = new wxCheckBox(lppanel, -1,
						 "Print to file",
						 wxPoint(11, 171),
						 wxSize(250, 25));
  
  
  lppanel->print_btn = new wxButton(lppanel, ID_LINEPRINTER_PRINT, "Print",
				    wxPoint(17, 300),
				    wxSize(46, 25));

  lppanel->cancel_btn = new wxButton(lppanel, ID_LINEPRINTER_CANCEL, "Cancel",
				     wxPoint(83, 300),
				     wxSize(46, 25));

  lppanel->close_btn = new wxButton(lppanel, ID_LINEPRINTER_CLOSE, "Close",
				    wxPoint(149, 300),
				    wxSize(46, 25));


  lppanel->default_btn = new wxButton(lppanel, ID_LINEPRINTER_DEFAULT,
				      "Default",
				      wxPoint(215, 300),
				      wxSize(46, 25));

  lppanel->ShowModal(); 
  lppanel->Show(FALSE);

  return lppanel;
}

#endif // __USE_POSTSCRIPT_PRINTING__

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
