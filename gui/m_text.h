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

Define a new text object that shares the same properties in all
the frames and dialogs.
*/
// ----------------------------------------------------------- // 
#ifndef __M_TEXT_HPP__
#define __M_TEXT_HPP__

#include "app_defs.h"

class wxappfwTextWin: public wxTextCtrl
{
public:
  wxappfwTextWin(wxWindow* parent, wxWindowID id, char *value,
	      int x=-1, int y=-1, int width=-1, int height=-1,
	      long style=0):
    wxTextCtrl(parent, id, value, wxPoint(x, y), wxSize(width, height), style)
  {

  }

  wxappfwTextWin(wxWindow* parent, wxWindowID id, wxString &value,
	      int x=-1, int y=-1, int width=-1, int height=-1,
	      long style=0):
    wxTextCtrl(parent, id, value, wxPoint(x, y), wxSize(width, height), style)
  {

  }

public:
  void OnKeyDown(wxKeyEvent& event);
  void OnKeyUp(wxKeyEvent& event);
  void OnChar(wxKeyEvent& event);
  void OnText(wxCommandEvent& event);
  void OnMouseEvent(wxMouseEvent& event);

private:
  DECLARE_EVENT_TABLE()
};

#endif // __M_TEXT_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
