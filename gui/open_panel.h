// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 07/23/2025
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

Application panel
*/
// ----------------------------------------------------------- // 
#ifndef __OPEN_PANEL_HPP__
#define __OPEN_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define OPENDATABASE_PANEL_ID 9845
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Page setup panel IDs 
  ID_OPENDATABASE_OK = 9846,
  ID_OPENDATABASE_CANCEL,
  ID_OPENDATABASE_KEY_BROWSE,
  ID_OPENDATABASE_RSA_BROWSE,
  ID_OPENDATABASE_TEXTCONTROL_PASSWORD,
  ID_OPENDATABASE_TEXTCONTROL_KEY_FILE,
  ID_OPENDATABASE_TEXTCONTROL_RSA_USERNAME,
  ID_OPENDATABASE_TEXTCONTROL_RSA_KEY,
  ID_OPENDATABASE_TEXTCONTROL_RSA_PASSPHRASE,
#ifdef __ENABLE_SMART_CARD__
  ID_OPENDATABASE_TEXTCONTROL_SC_USERNAME,
  ID_OPENDATABASE_TEXTCONTROL_SC_PIN,
  ID_OPENDATABASE_TEXTCONTROL_SC_CERT_ID
#endif // __ENABLE_SMART_CARD__ 
};
// --------------------------------------------------------------

class OpenDatabasePanel : public wxDialog
{
public:
  OpenDatabasePanel(wxWindow *parent, wxWindowID id, char *title,
		   int xpos, int ypos, int width, int height,
		   long style = wxDEFAULT_DIALOG_STYLE,
		    char* name = (char *)"dialogBox");
  ~OpenDatabasePanel();
  
public: // Event handlers
  // Frame event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));

  // Button event handlers
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnKeyBrowse(wxCommandEvent &event);
  void OnRSAKeyBrowse(wxCommandEvent &event);
  void OnTextControlPasswordEnter(wxCommandEvent &event);
  void OnTextControlKeyFileEnter(wxCommandEvent &event);
  void OnTextControlRSAKeyFileEnter(wxCommandEvent &event);
  void OnTextControlRSAUsernameEnter(wxCommandEvent &event);
  void OnTextControlRSAPassPhraseEnter(wxCommandEvent &event);

#ifdef __ENABLE_SMART_CARD__
  void OnTextControlSCUsernameEnter(wxCommandEvent &event);
  void OnTextControlSCPinEnter(wxCommandEvent &event);
  void OnTextControlSCCertIDEnter(wxCommandEvent &event);
#endif // __ENABLE_SMART_CARD__   
  
public: // Memeber functions
  void ShowPanel(gxString &fname);
  int TestInput();
  int RSAOpenDatabase();

#ifdef __ENABLE_SMART_CARD__
  int SmartCardOpenDatabase();
#endif // __ENABLE_SMART_CARD__

  int AESKeyOpenDatabase();
  int PasswordOpenDatabase();
  int IsOK() { return is_ok == 1; }

public: // Control objects
  wxStaticText *password_label;
  wxTextCtrl *password_input;
  wxButton *ok_btn;
  wxButton *cancel_btn;
  wxStaticBox *password_box;
  wxStaticBox *key_box;
  wxStaticText *key_label;
  wxTextCtrl *key_input;
  wxButton *browse;
  wxStaticBox *rsa_key_box;
  wxStaticText *rsa_passphrase_label;
  wxTextCtrl *rsa_passphrase_input;
  wxStaticText *rsa_username_label;
  wxTextCtrl *rsa_username_input;
  wxStaticText *rsa_key_label;
  wxTextCtrl *rsa_key_input;
  wxButton *rsa_browse;

#ifdef __ENABLE_SMART_CARD__
  wxStaticBox *sc_box;
  wxStaticText *sc_pin_label;
  wxStaticText *sc_username_label;
  wxTextCtrl *sc_username_input;
  wxTextCtrl *sc_pin_input;
  wxStaticText *sc_keyid_label;
  wxTextCtrl *sc_keyid_input;
#endif // __ENABLE_SMART_CARD__

private: // Data members
  int is_ok;
  gxString curr_fname;
  int use_key;
  int use_password;
  int use_rsa_key;

#ifdef __ENABLE_SMART_CARD__
  int use_smartcard;
#endif // __ENABLE_SMART_CARD__ 

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
OpenDatabasePanel *InitOpenDatabasePanel(wxWindow *parent);

#endif // __OPEN_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
