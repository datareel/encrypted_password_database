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

Add User For Database Access. 10/27/2026: Rewritten with sizers: one
way at a time, an RSA public key, the smart card in the reader (from the
card picker, no PIN) or an exported smart card cert file.
*/
// ----------------------------------------------------------- // 
#ifndef __ADD_USER_PANEL_HPP__
#define __ADD_USER_PANEL_HPP__

#include "app_defs.h"

#define ADDUSER_PANEL_ID 760

class AdduserPanel : public wxDialog
{
public:
  AdduserPanel(wxWindow *parent);
  ~AdduserPanel();

public: // Event handlers
  void OnCloseWindow(wxCloseEvent &event);
  void OnOK(wxCommandEvent &event);
  void OnCancel(wxCommandEvent &event);
  void OnMethod(wxCommandEvent &event);
  void OnRSAKeyFileBrowse(wxCommandEvent &event);
#ifdef __ENABLE_SMART_CARD__
  void OnCertFileBrowse(wxCommandEvent &event);
#endif // __ENABLE_SMART_CARD__

public: // Member functions
  void ShowPanel();
  int TestInput();
  int IsOK() { return is_ok == 1; }
  void SelectMethod(int m);

public: // Control objects
  wxRadioBox *method_box;
  wxTextCtrl *username_input;
  wxPanel *pages[3];
  wxTextCtrl *rsa_key_input;
#ifdef __ENABLE_SMART_CARD__
  SmartCardPicker *sc_picker;
  wxTextCtrl *sc_cert_input;
#endif // __ENABLE_SMART_CARD__
  wxStaticText *message;

private: // Data members
  int is_ok;
  int method;

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
