// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 07/24/2023
// Copyright (c) 2001-2025 DataReel Software Development
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

Create Database dialog. 10/27/2026: Rewritten with sizers: the name and
folder of the new database, then the ways it can be unlocked (master
password or key file, RSA public keys, smart cards), with a summary.
*/
// ----------------------------------------------------------- // 
#ifndef __NEW_PANEL_HPP__
#define __NEW_PANEL_HPP__

#include "app_defs.h"

#define NEWDATABASE_PANEL_ID 730

class NewDatabasePanel : public wxDialog
{
public:
  NewDatabasePanel(wxWindow *parent);
  ~NewDatabasePanel();

public: // Event handlers
  void OnCloseWindow(wxCloseEvent &event);
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnBrowseFolder(wxCommandEvent &event);
  void OnKeyFileBrowse(wxCommandEvent &event);
  void OnRSAKeyFileBrowse(wxCommandEvent &event);
  void OnGenerate(wxCommandEvent &event);
  void OnChange(wxCommandEvent &event);

public: // Member functions
  int CheckPassword();
  void ShowPanel();
  int TestInput();
  int IsOK() { return is_ok == 1; }
  gxString DatabasePath() const; // Folder and name, without the extension
  gxString DatabaseName() const;
  void UpdateState();

public: // Control objects
  wxTextCtrl *name_input;
  wxTextCtrl *folder_input;
  wxStaticText *path_label;
  wxCheckBox *password_check;
  wxPanel *password_panel;
  PasswordEntry *password_input;
  PasswordEntry *confirm_password_input;
  PasswordStrength *strength;
  wxCheckBox *key_check;
  wxPanel *key_panel;
  wxTextCtrl *key_input;
  wxStaticText *key_note;
  wxCheckBox *rsa_check;
  wxPanel *rsa_panel;
  wxTextCtrl *rsa_username_input;
  wxTextCtrl *rsa_key_input;
#ifdef __ENABLE_SMART_CARD__
  wxCheckBox *sc_check;
  wxPanel *sc_panel;
  SmartCardPicker *sc_picker;
  wxTextCtrl *sc_username_input;
#endif // __ENABLE_SMART_CARD__
  wxStaticText *summary;
  wxStaticText *message;

private: // Data members
  int is_ok;

public:
  int use_key;
  int use_password;
  int use_rsa_key;
  gxString rsa_key_username;
  char public_key[RSA_max_keybuf_len];
  unsigned public_key_len = 0;
  int use_cert_file;
#ifdef __ENABLE_SMART_CARD__
  int use_smartcard;
  SmartCardOB sc;
  gxString smartcard_username;
  wxString card_name;
#endif // __ENABLE_SMART_CARD__
  // Use a random key if no password or symmectric are provided with RSA or smartcard
  unsigned char random_key[128];

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
NewDatabasePanel *InitNewDatabasePanel(wxWindow *parent);

#endif // __NEW_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
