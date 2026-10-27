// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File Name: misc_dlg.h
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

10/27/2026: Small sizer-based dialogs: Generate Password and the
Self-Destruct Settings.
*/
// ----------------------------------------------------------- //   
#ifndef __MISC_DLG_HPP__
#define __MISC_DLG_HPP__

#include "app_defs.h"

// Makes a random password with EPDBGeneratePassword(). With a target
// field, Use puts it in the field.
class GeneratePasswordDialog : public wxDialog
{
public:
  GeneratePasswordDialog(wxWindow *parent, PasswordEntry *target);
  ~GeneratePasswordDialog();

private:
  void Generate();
  void OnGenerate(wxCommandEvent &event);
  void OnCopy(wxCommandEvent &event);
  void OnUse(wxCommandEvent &event);

private:
  wxSpinCtrl *length;
  wxCheckBox *symbols;
  wxTextCtrl *result;
  PasswordStrength *strength;
  PasswordEntry *target;
};

// Self-destruct on or off and the number of failed opens allowed
class SelfDestructDialog : public wxDialog
{
public:
  SelfDestructDialog(wxWindow *parent, int on, unsigned limit);
  int IsOn() const { return enable->GetValue() ? 1 : 0; }
  unsigned Limit() const { return (unsigned)attempts->GetValue(); }

private:
  wxCheckBox *enable;
  wxSpinCtrl *attempts;
};

#endif // __MISC_DLG_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
