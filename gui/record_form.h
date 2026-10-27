// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File Name: record_form.h
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

10/27/2026: The record form shared by the Add, Edit and Display record
dialogs: Title across the top, two columns below (Username and Website,
Password and Previous password, Security questions and Account
information) and Notes across the bottom. Security questions, Account
information and Notes are multi-line. Every field holds at most
EPDB_FIELD_MAX_BYTES bytes (UTF-8, newlines count), shown by a counter.
*/
// ----------------------------------------------------------- //
#ifndef __RECORD_FORM_HPP__
#define __RECORD_FORM_HPP__

#include "app_defs.h"

const unsigned EPDB_FIELD_MAX_BYTES = 255; // DBStringLength without the overhead

// One record field: a text field, a multi-line text field or a password
// field. It has the wxTextCtrl calls the record dialogs use.
class RecordFieldCtrl : public wxPanel
{
public:
  RecordFieldCtrl(wxWindow *parent, int member, int read_only);

public:
  wxString GetValue() const;   // CRLF line ends are returned as LF
  void SetValue(const wxString &v);
  void ChangeValue(const wxString &v) { SetValue(v); }
  void Clear();
  void Reveal(int show);
  PasswordEntry *Password() { return pw; }
  wxStaticText *Counter() { return counter; }
  int Member() const { return member; }

private:
  void OnText(wxCommandEvent &event);
  void UpdateCounter();

private:
  int member;
  int read_only;
  int limiting;
  wxTextCtrl *text;
  PasswordEntry *pw;
  wxStaticText *counter;
  wxString last_ok; // The last value within the limit
};

class RecordForm
{
public:
  RecordForm();

public:
  // Adds the form to sizer. inputs[] and labels[] are the dialog's
  // field arrays (labels are indexed by member - 1, as before).
  void Build(wxWindow *parent, wxSizer *sizer, RecordFieldCtrl *inputs[], wxStaticText *labels[],
	     int read_only);
  // Column names of the database, set before each show
  void SetLabels(CryptDBDocument *doc);
  void OnGenerate(wxCommandEvent &event);
  void OnPasswordText(wxCommandEvent &event);
  // Edit dialog: the old password goes to Previous password when the
  // password is changed and Previous password was not edited
  void SetOriginalPassword(const wxString &pw, const wxString &prev);

public:
  wxStaticText *title_label;
  wxStaticText **labels;
  RecordFieldCtrl **inputs;
  PasswordStrength *strength;
  wxButton *generate;
  wxString original_password;
  wxString original_previous;
  int autofill;
  int read_only;
};

#endif // __RECORD_FORM_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
