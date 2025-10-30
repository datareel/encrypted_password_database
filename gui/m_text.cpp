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

Define  a text ctrl which allows to call different wxTextCtrl
functions interactively.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(wxappfwTextWin, wxTextCtrl)
  EVT_KEY_DOWN(wxappfwTextWin::OnKeyDown)
  EVT_KEY_UP(wxappfwTextWin::OnKeyUp)
  EVT_CHAR(wxappfwTextWin::OnChar)
  EVT_TEXT(-1, wxappfwTextWin::OnText)
  EVT_MOUSE_EVENTS(wxappfwTextWin::OnMouseEvent)
END_EVENT_TABLE()

void wxappfwTextWin::OnMouseEvent(wxMouseEvent& event)
{
  if(!event.Moving()) {
    wxString msg;
    if(event.Entering()) {
      // Mouse entered the window
    }
    else if(event.Leaving()) {
      // Mouse left the window
    }
    else {
      // Click event
      wxString button;
      bool dbl, up;
      if(event.LeftDown() || event.LeftUp() || event.LeftDClick()) {
	button = "Left";
	dbl = event.LeftDClick();
	up = event.LeftUp();
      }
      else if(event.MiddleDown() || event.MiddleUp() || event.MiddleDClick()) {
	button = "Middle";
	dbl = event.MiddleDClick();
	up = event.MiddleUp();
      }
      else if(event.RightDown() || event.RightUp() || event.RightDClick()) {
	button = "Right";
	dbl = event.RightDClick();
	up = event.RightUp();
      }
      else {
	// Unknown mouse event
	return;
      }
    }
  }

  event.Skip();
}

void wxappfwTextWin::OnText(wxCommandEvent& event)
{
  wxappfwTextWin *win = (wxappfwTextWin *)event.GetEventObject();
  const wxChar *data = (const wxChar *)(win->GetClientData());
  if(data) {
    // Text changed in control
  }
  else {
    // Text changed in some control
  }
}

void wxappfwTextWin::OnChar(wxKeyEvent& event)
{
  event.Skip();
}

void wxappfwTextWin::OnKeyUp(wxKeyEvent& event)
{
  event.Skip();
}

void wxappfwTextWin::OnKeyDown(wxKeyEvent& event)
{
  event.Skip();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
