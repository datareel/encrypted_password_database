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
// 10/27/2026: Sizers, the system GUI font
{
  StatusPanel *spanel = new StatusPanel(parent, STATUS_PANEL_ID, (char *)"Database Event Window", -1, -1, -1, -1,
					wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, (char *)"Database Event Window");
  if(!spanel) return 0;
  spanel->SetIcons(EPDBAppIcons());
  const int pad = spanel->FromDIP(12);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  top->Add(EPDBHeading(spanel, "Progress", 0), 0, wxLEFT | wxRIGHT | wxTOP, pad);
  spanel->status_gauge = new wxGauge(spanel, -1, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
  top->Add(spanel->status_gauge, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
  spanel->status_win = new wxTextCtrl(spanel, -1, "", wxDefaultPosition, spanel->FromDIP(wxSize(400, 160)),
				      wxTE_MULTILINE | wxTE_READONLY);
  spanel->text_font = new wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
  spanel->status_win->SetFont(*spanel->text_font);
  top->Add(spanel->status_win, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
  wxBoxSizer *b = new wxBoxSizer(wxHORIZONTAL);
  b->AddStretchSpacer();
  spanel->cancel_btn = new wxButton(spanel, ID_STATUS_CANCEL, "Close");
  b->Add(spanel->cancel_btn, 0);
  top->Add(b, 0, wxEXPAND | wxALL, pad);
  spanel->SetEscapeId(ID_STATUS_CANCEL);
  spanel->SetSizerAndFit(top);
  spanel->Show(FALSE);
  return spanel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
