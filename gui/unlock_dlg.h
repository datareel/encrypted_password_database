// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File Name: unlock_dlg.h
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

10/27/2026: Unlock Database dialog, replaces the Open Database panel.
The user picks one unlock method and only its fields are shown, so the
methods cannot be mixed. The unlock itself is done by the core
(EPDBUnlock, DatabaseUserAuth); this dialog only collects the input.
*/
// ----------------------------------------------------------- //
#ifndef __UNLOCK_DLG_HPP__
#define __UNLOCK_DLG_HPP__

#include "app_defs.h"

enum EPDBUnlockMethod {
  EPDB_UNLOCK_PASSWORD = 0,
  EPDB_UNLOCK_KEY_FILE,
  EPDB_UNLOCK_RSA_KEY,
  EPDB_UNLOCK_SMART_CARD
};

class PasswordEntry;
class SmartCardPicker;

class UnlockDialog : public wxDialog
{
public:
  UnlockDialog(wxWindow *parent);
  ~UnlockDialog();

public:
  // Shows the dialog. Returns 1 with the database file name if the
  // database was unlocked (progcfg->global_dbparms.crypt_key is set).
  int ShowPanel(gxString &fname, const wxString &start_file = wxString());
  int IsOK() { return is_ok == 1; }
  wxString UnlockDescription() const { return unlock_description; }

private: // Event handlers
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnClose(wxCloseEvent &event);
  void OnBrowseDatabase(wxCommandEvent &event);
  void OnBrowseKey(wxCommandEvent &event);
  void OnBrowseRSAKey(wxCommandEvent &event);
  void OnBrowseModule(wxCommandEvent &event);
  void OnMethod(wxCommandEvent &event);
  void OnFileChanged(wxCommandEvent &event);
  void OnCardChanged(wxCommandEvent &event);
  void OnUpdateOK(wxUpdateUIEvent &event);
  void OnDPIChanged(wxDPIChangedEvent &event);

private: // Unlock methods, each returns 1 if the database was unlocked
  int TestInput();
  int PasswordOpenDatabase();
  int AESKeyOpenDatabase();
  int RSAOpenDatabase();
  int RSAOpenDatabase(char private_key[RSA_max_keybuf_len], gxString &pbuf);
#ifdef __ENABLE_SMART_CARD__
  int SmartCardOpenDatabase();
#endif
  void SelectMethod(int method);
  void LoadFileSettings();
  void Remember();
  void ClearSecrets();
  void Refit();

private:
  wxComboBox *file_input;
  wxRadioBox *method_box;
  wxPanel *pages[4]; // One page per unlock method
  PasswordEntry *password_input;
  wxTextCtrl *key_input;
  wxTextCtrl *rsa_username_input;
  wxTextCtrl *rsa_key_input;
  PasswordEntry *rsa_passphrase_input;
#ifdef __ENABLE_SMART_CARD__
  SmartCardPicker *sc_picker;
  wxStaticText *sc_pin_label;
  PasswordEntry *sc_pin_input;
  wxStaticText *sc_hint;
  wxCollapsiblePane *sc_advanced;
  wxTextCtrl *sc_username_input;
  wxTextCtrl *sc_keyid_input;
  wxTextCtrl *sc_module_input;
#endif
  wxStaticText *message;
  gxString curr_fname;
  const char *open_failed; // 10/27/2026: Set when a credential was tried and refused
  int is_ok;
  int method;
  wxString unlock_description; // For the status bar, such as "smart card CAC (slot 1)"

  wxDECLARE_EVENT_TABLE();
};

#endif // __UNLOCK_DLG_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
