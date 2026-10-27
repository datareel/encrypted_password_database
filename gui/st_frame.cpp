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

Status frame
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(StatusFrame, wxFrame)
  EVT_CLOSE(StatusFrame::OnCloseWindow)
END_EVENT_TABLE()

StatusFrame::StatusFrame(wxWindow* parent, wxWindowID id,
			 const wxString& title,
			 const wxPoint& pos, const wxSize& size, long style):
  wxFrame(parent, id, title, pos, size, style)
{
#ifdef __WXMAC__
  // Reqired to allow the about menu relocation, since ABOUT is
  // not the default ID of the about menu
  wxApp::s_macAboutMenuItemId = Minimal_About;
#endif
  
  SetIcons(EPDBAppIcons()); // 10/27/2026
  status_win = 0;
}
  
StatusFrame::~StatusFrame() 
{
  if(status_win) delete status_win;
}

// --------------------------------------------------------------
// Event handlers
// --------------------------------------------------------------
void StatusFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
// Define the behaviour for the frame closing. This event handler
// is called when the user has tried to close a a frame or dialog
// box using the window manager (X) or system menu (Windows). Must
// delete all frames except for the main one. 
{
  Show(FALSE);
}
// --------------------------------------------------------------

void StatusFrame::WriteMessage(const wxString &message)
{
  if(!status_win) return; // Prevent program crash is not initialized
  *(status_win) << "\n" << message;
}

StatusFrame *InitStatusFrame(wxWindow* parent, wxFont *textWindowFont)
// 10/27/2026: Sized in DIPs, the text fills the frame
{
  gxString frame_name = progcfg->ProgramName;
  frame_name << " Status Window";
  StatusFrame *sframe = new StatusFrame(parent, STATUS_FRAME_ID,
					(const wxString)frame_name.c_str(),
					wxDefaultPosition, wxDefaultSize,
					wxDEFAULT_FRAME_STYLE|wxFRAME_FLOAT_ON_PARENT);
  if(!sframe) return 0;
  sframe->status_win = new wxTextCtrl(sframe, -1, "", wxDefaultPosition, wxDefaultSize,
				      wxTE_MULTILINE|wxTE_READONLY);
  sframe->status_win->SetFont(*(textWindowFont));
  wxBoxSizer *s = new wxBoxSizer(wxVERTICAL);
  s->Add(sframe->status_win, 1, wxEXPAND);
  sframe->SetSizer(s);
  sframe->SetSize(sframe->FromDIP(wxSize(560, 360)));
  sframe->Show(FALSE);
  return sframe;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
