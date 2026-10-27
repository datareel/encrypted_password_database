// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 07/23/2025
// Copyright (c) 2001-2025 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
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

Add User For Database Access, see add_user_panel.h. 10/27/2026:
Rewritten with sizers, the checks are the same as before.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

enum {
  ID_ADDUSER_METHOD = 761,
  ID_ADDUSER_RSA_BROWSE,
  ID_ADDUSER_CERT_BROWSE
};

enum { EPDB_ADDUSER_RSA = 0, EPDB_ADDUSER_CARD, EPDB_ADDUSER_CERT_FILE };

BEGIN_EVENT_TABLE(AdduserPanel, wxDialog)
  EVT_CLOSE(AdduserPanel::OnCloseWindow)
  EVT_BUTTON(wxID_OK, AdduserPanel::OnOK)
  EVT_BUTTON(wxID_CANCEL, AdduserPanel::OnCancel)
  EVT_RADIOBOX(ID_ADDUSER_METHOD, AdduserPanel::OnMethod)
  EVT_BUTTON(ID_ADDUSER_RSA_BROWSE, AdduserPanel::OnRSAKeyFileBrowse)
#ifdef __ENABLE_SMART_CARD__
  EVT_BUTTON(ID_ADDUSER_CERT_BROWSE, AdduserPanel::OnCertFileBrowse)
#endif
END_EVENT_TABLE()

AdduserPanel::AdduserPanel(wxWindow *parent)
  : wxDialog(parent, ADDUSER_PANEL_ID, "Add User", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  is_ok = 0;
  method = EPDB_ADDUSER_RSA;
  use_rsa_key = 0;
  memset(public_key, 0, sizeof(public_key));
#ifdef __ENABLE_SMART_CARD__
  use_cert_file = 0;
  use_smartcard = 0;
#endif
  SetIcons(EPDBAppIcons());
  const int pad = FromDIP(12);
  const int gap = FromDIP(6);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *head = new wxBoxSizer(wxHORIZONTAL);
  head->Add(new wxStaticBitmap(this, wxID_ANY, EPDBIcon("users", 32)), 0, wxRIGHT, FromDIP(10));
  head->Add(EPDBNote(this, "The user can then open this database with their own private RSA key or smart card. "
		     "Only the public key is read, no PIN is needed.", 420), 1, wxALIGN_CENTER_VERTICAL);
  top->Add(head, 0, wxEXPAND | wxALL, pad);

  wxBoxSizer *ur = new wxBoxSizer(wxHORIZONTAL);
  ur->Add(new wxStaticText(this, wxID_ANY, "Username"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
  username_input = new wxTextCtrl(this, wxID_ANY, "");
  ur->Add(username_input, 1, wxALIGN_CENTER_VERTICAL);
  top->Add(ur, 0, wxEXPAND | wxLEFT | wxRIGHT, pad);

  wxString methods[3] = { "RSA public key", "Smart card in the reader", "Exported smart card cert" };
#ifdef __ENABLE_SMART_CARD__
  int n = 3;
#else
  int n = 1;
#endif
  method_box = new wxRadioBox(this, ID_ADDUSER_METHOD, "Add with", wxDefaultPosition, wxDefaultSize,
			      n, methods, n, wxRA_SPECIFY_COLS);
  top->Add(method_box, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

  wxButton *b;
  for(int i = 0; i < 3; i++) pages[i] = 0;
  pages[EPDB_ADDUSER_RSA] = new wxPanel(this);
  wxBoxSizer *ps = new wxBoxSizer(wxVERTICAL);
  ps->Add(new wxStaticText(pages[0], wxID_ANY, "Public key file (PEM)"), 0, wxBOTTOM, FromDIP(3));
  ps->Add(EPDBFileRow(pages[0], rsa_key_input, b, ID_ADDUSER_RSA_BROWSE), 0, wxEXPAND);
  pages[0]->SetSizer(ps);
#ifdef __ENABLE_SMART_CARD__
  pages[EPDB_ADDUSER_CARD] = new wxPanel(this);
  ps = new wxBoxSizer(wxVERTICAL);
  sc_picker = new SmartCardPicker(pages[1]);
  ps->Add(sc_picker, 1, wxEXPAND);
  pages[1]->SetSizer(ps);
  pages[EPDB_ADDUSER_CERT_FILE] = new wxPanel(this);
  ps = new wxBoxSizer(wxVERTICAL);
  ps->Add(new wxStaticText(pages[2], wxID_ANY, "Cert file exported from the card (PEM, .cer or .crt)"), 0, wxBOTTOM, FromDIP(3));
  ps->Add(EPDBFileRow(pages[2], sc_cert_input, b, ID_ADDUSER_CERT_BROWSE), 0, wxEXPAND);
  pages[2]->SetSizer(ps);
#endif
  for(int i = 0; i < 3; i++) {
    if(!pages[i]) continue;
    top->Add(pages[i], 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
    pages[i]->Hide();
  }
  message = new wxStaticText(this, wxID_ANY, "");
  message->SetForegroundColour(EPDBWarningColour());
  top->Add(message, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, gap);
  wxStdDialogButtonSizer *btns = new wxStdDialogButtonSizer();
  wxButton *ok = new wxButton(this, wxID_OK, "Add User");
  ok->SetDefault();
  btns->AddButton(ok);
  btns->AddButton(new wxButton(this, wxID_CANCEL));
  btns->Realize();
  top->Add(btns, 0, wxEXPAND | wxALL, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizer(top);
  SelectMethod(EPDB_ADDUSER_RSA);
}

AdduserPanel::~AdduserPanel()
{
  memset(public_key, 0, sizeof(public_key));
}

void AdduserPanel::SelectMethod(int m)
{
  method = m;
  method_box->SetSelection(m);
  for(int i = 0; i < 3; i++) if(pages[i]) pages[i]->Show(i == m);
#ifdef __ENABLE_SMART_CARD__
  if(m == EPDB_ADDUSER_CARD) {
    sc_picker->Rescan(0);
    sc_picker->StartPolling();
  }
  else {
    sc_picker->StopPolling();
  }
#endif
  int w = GetSize().x;
  SetMinSize(wxDefaultSize);
  Layout();
  wxSize best = GetSizer()->ComputeFittingWindowSize(this);
  SetMinSize(best);
  SetSize(wxSize(wxMax(best.x, w), best.y));
}

void AdduserPanel::OnMethod(wxCommandEvent &WXUNUSED(event))
{
  message->SetLabel("");
  SelectMethod(method_box->GetSelection());
}

void AdduserPanel::ShowPanel()
{
  is_ok = 0;
  message->SetLabel("");
  username_input->Clear();
  rsa_key_input->Clear();
#ifdef __ENABLE_SMART_CARD__
  sc_cert_input->Clear();
#endif
  SelectMethod(method);
  CentreOnParent();
  username_input->SetFocus();
  ShowModal();
#ifdef __ENABLE_SMART_CARD__
  sc_picker->StopPolling();
#endif
}

void AdduserPanel::OnRSAKeyFileBrowse(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog dialog(this, "RSA public key file", progcfg->docDir.c_str(), "",
		      "PEM files (*.pem)|*.pem|All files|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) rsa_key_input->SetValue(dialog.GetPath());
}

#ifdef __ENABLE_SMART_CARD__
void AdduserPanel::OnCertFileBrowse(wxCommandEvent &WXUNUSED(event))
{
  // 10/26/2026: Windows exports certs as .cer files, PEM or DER
  wxFileDialog dialog(this, "Smart card exported cert file", progcfg->docDir.c_str(), "",
		      "Cert files (*.pem;*.cer;*.crt)|*.pem;*.cer;*.crt|All files|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) sc_cert_input->SetValue(dialog.GetPath());
}
#endif // __ENABLE_SMART_CARD__

int AdduserPanel::TestInput()
{
  use_rsa_key = 0;
#ifdef __ENABLE_SMART_CARD__
  use_smartcard = 0;
  use_cert_file = 0;
#endif
  gxString sbuf;
  int rv;
  message->SetLabel("");
  wxString user = username_input->GetValue();
  user.Trim().Trim(false);
  if(user.IsEmpty()) {
    message->SetLabel("Enter the username");
    return 0;
  }

  if(method == EPDB_ADDUSER_RSA) {
    wxString f = rsa_key_input->GetValue();
    if(f.IsEmpty() || !futils_exists(f.utf8_str()) || !futils_isfile(f.utf8_str())) {
      message->SetLabel("The public RSA key file does not exist or cannot be read");
      return 0;
    }
    rv = RSA_read_key_file(f.utf8_str(), public_key, sizeof(public_key), &public_key_len);
    if(rv != RSA_NO_ERROR) {
      sbuf << clear << "Error reading public key file " << RSA_err_string(rv);
      ProgramError->Message(sbuf.c_str());
      return 0;
    }
    use_rsa_key = 1;
    rsa_key_username = (const char *)user.utf8_str();
  }
#ifdef __ENABLE_SMART_CARD__
  else if(method == EPDB_ADDUSER_CARD) {
    // 10/26/2026: Read the public key from the card in the reader, no PIN
    SC_TokenInfo token;
    SC_CertInfo cert;
    if(sc_picker->GetChoice(token, cert) != 0 || cert.id_hex.empty()) {
      message->SetLabel("Choose the smart card certificate");
      return 0;
    }
    if(!cert.is_rsa) {
      message->SetLabel("The smart card key must be an RSA key");
      return 0;
    }
    if(cert.id_hex.length() >= SC_max_cert_id_size) {
      message->SetLabel("The smart card cert ID is too long");
      return 0;
    }
    sc.SetEnginePath(progcfg->SC_enginePath.c_str());
    sc.SetModulePath((char *)(const char *)sc_picker->GetModulePath().utf8_str());
    sc.SetEngineID(progcfg->SC_engine_ID.c_str());
    sc.SetSlot(token.slot_id);
    sc.SetCertID((char *)cert.id_hex.c_str());
    use_smartcard = 1;
    smartcard_username = (const char *)user.utf8_str();
  }
  else {
    wxString f = sc_cert_input->GetValue();
    if(f.IsEmpty() || !futils_exists(f.utf8_str()) || !futils_isfile(f.utf8_str())) {
      message->SetLabel("The smart card cert file does not exist or cannot be read");
      return 0;
    }
    // 10/26/2026: PEM or DER cert files
    if(EPDBReadSmartCardCertFile(&sc, f.utf8_str(), sbuf) != 0) {
      ProgramError->Message(sbuf.c_str());
      return 0;
    }
    use_smartcard = 1;
    use_cert_file = 1;
    smartcard_username = (const char *)user.utf8_str();
  }
#endif // __ENABLE_SMART_CARD__
  is_ok = 1;
  return 1;
}

void AdduserPanel::OnOK(wxCommandEvent &WXUNUSED(event))
{
  if(!TestInput()) return;
  EndModal(wxID_OK);
}

void AdduserPanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  is_ok = 0;
  EndModal(wxID_CANCEL);
}

void AdduserPanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event))
// The window X cancels, it never adds a user
{
  is_ok = 0;
  EndModal(wxID_CANCEL);
}

AdduserPanel *InitAdduserPanel(wxWindow *parent)
{
  return new AdduserPanel(parent);
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
