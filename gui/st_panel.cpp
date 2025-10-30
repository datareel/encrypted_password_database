// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2025 DataReel Software Development
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

Status panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(StatusPanel, wxDialog)
  EVT_BUTTON    (ID_STATUS_CANCEL, StatusPanel::OnCancel)
END_EVENT_TABLE()

StatusPanel::StatusPanel(wxWindow *parent, wxWindowID id, char *title,
		     int xpos, int ypos, int width, int height,
		     long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  status_gauge = 0;
  status_win = 0;
  cancel_btn = 0;
  cancel_event = 0; 
  control_label = 0;
  text_font = 0;
}
  
StatusPanel::~StatusPanel()
{
  if(control_label) delete control_label;
  if(status_gauge) delete status_gauge;
  if(status_win) delete status_win;
  if(cancel_btn) delete cancel_btn;
  if(text_font) delete text_font;
}

void StatusPanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  cancel_event++;
  ResetPanel();
  Show(FALSE);
}

void StatusPanel::WriteMessage(const wxString &message)
{
  if(!status_win) return; // Prevent program crash is not initialized
  *(status_win) << message << "\n";
}

void StatusPanel::Close()
{
  ResetPanel();
  Show(FALSE);
}

void StatusPanel::ResetPanel()
{
  cancel_event = 0;

  // Prevent program crash is not initialized
  if(!status_gauge) return;
  if(!status_win) return;

  status_gauge->SetValue(0);
  status_win->Clear();
}

StatusPanel *InitStatusPanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=320;
  StatusPanel *spanel = new StatusPanel(parent, STATUS_PANEL_ID,
					(char *)"Database Event Window",
					xpos, ypos, width, height,
			wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
					(char *)"Database Event Window");

  if(!spanel) return 0;
  
  spanel->control_label = new wxStaticBox(spanel, -1,
					  "Progress Monitor",
					  wxPoint(7, 12),
					  wxSize(330, 230));

  spanel->status_gauge = new wxGauge(spanel, -1, 1000, wxPoint(16, 30),
				     wxSize(310, 44),
				     wxGA_HORIZONTAL|wxNO_BORDER);
  spanel->status_gauge->SetBackgroundColour(*wxBLUE);
  spanel->status_gauge->SetForegroundColour(*wxGREEN);

  spanel->status_win = new wxTextCtrl(spanel, -1, "",
				      wxPoint(16, 82),
				      wxSize(310, 150),
				      wxTE_MULTILINE|wxTE_READONLY);

  spanel->text_font = new wxFont(10, wxSWISS, wxNORMAL, wxNORMAL);
  spanel->status_win->SetFont(*(spanel->text_font));
  
  // NOTE: 11/06/2014: Changed Cancel to Close
  spanel->cancel_btn = new wxButton(spanel, ID_STATUS_CANCEL, "Close",
				    wxPoint(142, 255),
				    wxSize(56, 25));

  spanel->Show(FALSE);

  return spanel;
}

void StatusPanel::ShowPanel()
{
  ShowModal();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
