// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File Name: epdb_widgets.h
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

10/27/2026: Small controls shared by the dialogs: a password field with
show and hide, and a password strength meter.
*/
// ----------------------------------------------------------- //   
#ifndef __EPDB_WIDGETS_HPP__
#define __EPDB_WIDGETS_HPP__

#include "app_defs.h"

// A masked text field with an eye button to show the value. GTK cannot
// change wxTE_PASSWORD on a shown control, so a masked and a plain
// control are swapped. The plain one is cleared whenever it is hidden.
class PasswordEntry : public wxPanel
{
public:
  PasswordEntry(wxWindow *parent, wxWindowID id, int show_button = 1, long extra_style = 0);

public:
  wxString GetValue() const;
  void SetValue(const wxString &v);
  void Clear();
  int IsEmpty() const { return GetValue().IsEmpty(); }
  void SetMaxLength(unsigned long len);
  void SetEditable(bool editable);
  void Reveal(int show);
  int IsRevealed() const { return revealed; }
  wxTextCtrl *Ctrl() const { return revealed ? plain : masked; }
  virtual void SetFocus() wxOVERRIDE { Ctrl()->SetFocus(); }
  virtual bool Enable(bool enable = true) wxOVERRIDE;

private:
  void OnToggle(wxCommandEvent &event);
  void OnText(wxCommandEvent &event);
  void OnEnter(wxCommandEvent &event);

private:
  wxTextCtrl *masked;
  wxTextCtrl *plain;
  wxBitmapButton *eye;
  int revealed;
  int syncing;
};

// 0 to 100 and a word for a password: length and character kinds, with
// common patterns (one kind only, repeats, sequences) counted down
int EPDBPasswordScore(const wxString &pw, wxString &word);

class PasswordStrength : public wxPanel
{
public:
  PasswordStrength(wxWindow *parent);
  void Update(const wxString &pw);

private:
  wxGauge *gauge;
  wxStaticText *label;
};

// A text field and a Browse button for a file
wxSizer *EPDBFileRow(wxWindow *parent, wxTextCtrl *&text, wxButton *&browse, wxWindowID browse_id,
		     const wxString &value = wxString());

#endif // __EPDB_WIDGETS_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
