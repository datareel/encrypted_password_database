// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File Name: details_pane.h
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/27/2026
// Date Last Modified: 10/27/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
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

10/27/2026: Record details pane, right of the records grid. Shows the
selected record with copy buttons for the username and password (the
clipboard is cleared again after the Preferences time), a show button
for the password, and an open button for the website link.
*/
// ----------------------------------------------------------- //   
#ifndef __DETAILS_PANE_HPP__
#define __DETAILS_PANE_HPP__

#include "app_defs.h"

class CryptDBDocument;
class PasswordEntry;

class DetailsPane : public wxScrolledWindow
{
public:
  DetailsPane(wxWindow *parent, CryptDBDocument *doc);

public:
  void ShowRow(int row);   // -1 shows nothing
  int Row() const { return row; }
  void UpdateLabels();     // After a column rename
  void UpdateClipboardNote();

private:
  void OnCopyUser(wxCommandEvent &event);
  void OnCopyPassword(wxCommandEvent &event);
  void OnOpenLink(wxCommandEvent &event);
  void OnEdit(wxCommandEvent &event);
  void OnCommand(wxCommandEvent &event);
  wxTextCtrl *ReadOnlyText(int multiline);

private:
  CryptDBDocument *doc;
  int row;
  wxStaticText *title;
  wxStaticText *labels[NumDataMembers];
  wxTextCtrl *values[NumDataMembers]; // Not used for the password columns
  PasswordEntry *password;
  PasswordEntry *previous_password;
  wxStaticText *clip_note;
  wxStaticText *empty_note;
  wxBoxSizer *body;
  wxPanel *content;
};

#endif // __DETAILS_PANE_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
