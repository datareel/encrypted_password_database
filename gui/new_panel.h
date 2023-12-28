// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
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

Application panel
*/
// ----------------------------------------------------------- // 
#ifndef __NEW_PANEL_HPP__
#define __NEW_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define NEWDATABASE_PANEL_ID 730
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Page setup panel IDs 
  ID_NEWDATABASE_OK = 731,
  ID_NEWDATABASE_CANCEL,
  ID_NEWDATABASE_KEY_BROWSE,
  ID_NEWDATABASE_RSA_KEY_BROWSE,
  ID_NEWDATABASE_TEXTCONTROL_CONFIRM_PASSWORD,
  ID_NEWDATABASE_TEXTCONTROL_KEY_FILE,
  ID_NEWDATABASE_TEXTCONTROL_RSA_USERNAME,
  ID_NEWDATABASE_TEXTCONTROL_RSA_KEY,
  ID_NEWDATABASE_TEXTCONTROL_SC_USERNAME,
  ID_NEWDATABASE_TEXTCONTROL_SC_CERT_ID
};
// --------------------------------------------------------------

class NewDatabasePanel : public wxDialog
{
public:
  NewDatabasePanel(wxWindow *parent, wxWindowID id, char *title,
		   int xpos, int ypos, int width, int height,
		   long style = wxDEFAULT_DIALOG_STYLE,
		   char* name = (char *)"dialogBox");
  ~NewDatabasePanel();
  
public: // Event handlers
  // Frame event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));

  // Button event handlers
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnKeyFileBrowse(wxCommandEvent &event);
  void OnRSAKeyFileBrowse(wxCommandEvent &event);
  void OnTextControlPasswordEnter(wxCommandEvent &event);
  void OnTextControlKeyFileEnter(wxCommandEvent &event);
  void OnTextControlRSAKeyFileEnter(wxCommandEvent &event);
  void OnTextControlRSAUsernameEnter(wxCommandEvent &event);
  void OnTextControlSCUsernameEnter(wxCommandEvent &event);
  void OnTextControlSCCertIDEnter(wxCommandEvent &event);

public: // Memeber functions
  int CheckPassword();
  void ShowPanel();
  int TestInput();
  int IsOK() { return is_ok == 1; }

public: // Control objects
  wxStaticText *name_label;
  wxTextCtrl *name_input;
  wxStaticBox *password_box;
  wxStaticBox *key_box;
  wxStaticText *password_label;
  wxStaticText *confirm_password_label;
  wxTextCtrl *password_input;
  wxTextCtrl *confirm_password_input;
  wxStaticText *key_label;
  wxTextCtrl *key_input;
  wxButton *browse;
  wxStaticBox *rsa_key_box;
  wxStaticText *rsa_username_label;
  wxTextCtrl *rsa_username_input;
  wxStaticText *rsa_key_label;
  wxTextCtrl *rsa_key_input;
  wxButton *rsa_browse;
  wxStaticBox *sc_box;
  wxStaticText *sc_username_label;
  wxTextCtrl *sc_username_input;
  wxStaticText *sc_keyid_label;
  wxTextCtrl *sc_keyid_input;
  wxCheckBox *sc_use_checkbox;
  
  wxButton *ok_btn;
  wxButton *cancel_btn;
  
private: // Data members
  int is_ok;

public:
  int use_key;
  int use_password;
  int use_rsa_key;
  int use_smartcard;
  gxString rsa_key_username;
  char public_key[RSA_max_keybuf_len];
  unsigned public_key_len = 0;
  SmartCardOB sc;
  gxString smartcard_username;
  int use_cert_file;
  
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
