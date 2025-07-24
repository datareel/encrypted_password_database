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

Add user panel
*/
// ----------------------------------------------------------- // 
#ifndef __ADD_USER_PANEL_HPP__
#define __ADD_USER_PANEL_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// Window IDs
// --------------------------------------------------------------
#define ADDUSER_PANEL_ID 22730
// --------------------------------------------------------------

// --------------------------------------------------------------
// Identification for event handlers, controls, and menu commands
// --------------------------------------------------------------
enum { // Page setup panel IDs 
  ID_ADDUSER_OK = 22731,
  ID_ADDUSER_CANCEL,
  ID_ADDUSER_SC_CERT_BROWSE,
  ID_ADDUSER_RSA_KEY_BROWSE,

  ID_ADDUSER_TEXTCONTROL_RSA_USERNAME,
  ID_ADDUSER_TEXTCONTROL_RSA_KEY,
  ID_ADDUSER_TEXTCONTROL_SC_USERNAME,
  ID_ADDUSER_TEXTCONTROL_SC_CERT
};
// --------------------------------------------------------------

class AdduserPanel : public wxDialog
{
public:
  AdduserPanel(wxWindow *parent, wxWindowID id, char *title,
		   int xpos, int ypos, int width, int height,
		   long style = wxDEFAULT_DIALOG_STYLE,
		   char* name = (char *)"dialogBox");
  ~AdduserPanel();
  
public: // Event handlers
  // Frame event handlers
  void OnCloseWindow(wxCloseEvent& WXUNUSED(event));

  // Button event handlers
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnRSAKeyFileBrowse(wxCommandEvent &event);
  void OnTextControlRSAKeyFileEnter(wxCommandEvent &event);
  void OnTextControlRSAUsernameEnter(wxCommandEvent &event);

#ifdef __ENABLE_SMART_CARD__
  void OnTextControlSCCertFileEnter(wxCommandEvent &event);
  void OnCertFileBrowse(wxCommandEvent &event);
  void OnTextControlSCUsernameEnter(wxCommandEvent &event);
#endif // __ENABLE_SMART_CARD__
  
public: // Memeber functions
  int CheckPassword();
  void ShowPanel();
  int TestInput();
  int IsOK() { return is_ok == 1; }

public: // Control objects
  wxStaticBox *rsa_key_box;
  wxStaticText *rsa_username_label;
  wxTextCtrl *rsa_username_input;
  wxStaticText *rsa_key_label;
  wxTextCtrl *rsa_key_input;
  wxButton *rsa_browse;

#ifdef __ENABLE_SMART_CARD__
  wxStaticBox *sc_box;
  wxStaticText *sc_username_label;
  wxTextCtrl *sc_username_input;
  wxStaticText *sc_cert_label;
  wxTextCtrl *sc_cert_input;
  wxButton *sc_browse;
#endif // __ENABLE_SMART_CARD__
  
  wxButton *ok_btn;
  wxButton *cancel_btn;
  
private: // Data members
  int is_ok;

public:
  int use_rsa_key;
  gxString rsa_key_username;
  char public_key[RSA_max_keybuf_len];
  unsigned public_key_len = 0;

#ifdef __ENABLE_SMART_CARD__
  int use_cert_file;
  int use_smartcard;
  SmartCardOB sc;
  gxString smartcard_username;
#endif // __ENABLE_SMART_CARD__

private:
  DECLARE_EVENT_TABLE()
};

// Standalone panel initialization function
AdduserPanel *InitAdduserPanel(wxWindow *parent);

#endif // __ADD_USER_PANEL_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
