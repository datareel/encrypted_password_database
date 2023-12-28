// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2024 DataReel Software Development
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

Application panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(OpenDatabasePanel, wxDialog)
  EVT_CLOSE(OpenDatabasePanel::OnCloseWindow)
  EVT_BUTTON (ID_OPENDATABASE_OK, OpenDatabasePanel::OnOK)
  EVT_BUTTON (ID_OPENDATABASE_CANCEL, OpenDatabasePanel::OnCancel)
  EVT_BUTTON (ID_OPENDATABASE_KEY_BROWSE, OpenDatabasePanel::OnKeyBrowse)
  EVT_BUTTON (ID_OPENDATABASE_RSA_BROWSE, OpenDatabasePanel::OnRSAKeyBrowse)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_PASSWORD, OpenDatabasePanel::OnTextControlPasswordEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_KEY_FILE, OpenDatabasePanel::OnTextControlKeyFileEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_RSA_USERNAME, OpenDatabasePanel::OnTextControlRSAKeyFileEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_RSA_KEY, OpenDatabasePanel::OnTextControlRSAUsernameEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_RSA_PASSPHRASE, OpenDatabasePanel::OnTextControlRSAPassPhraseEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_SC_USERNAME, OpenDatabasePanel::OnTextControlSCUsernameEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_SC_PIN, OpenDatabasePanel::OnTextControlSCPinEnter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL_SC_CERT_ID, OpenDatabasePanel::OnTextControlSCCertIDEnter)
END_EVENT_TABLE()
OpenDatabasePanel::OpenDatabasePanel(wxWindow *parent, wxWindowID id,
				   char *title,
				   int xpos, int ypos, int width, int height,
				   long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  ok_btn = cancel_btn = 0;
  is_ok = 0;
  password_label = 0;
  password_input = 0;
  password_box = 0;
  key_box = 0;
  key_label = 0;
  key_input = 0;
  browse = 0;
  rsa_key_box = 0;
  rsa_passphrase_label = 0;
  rsa_passphrase_input = 0;
  rsa_username_label = 0;
  rsa_username_input = 0;
  rsa_key_label = 0;
  rsa_key_input = 0;
  rsa_browse = 0;
  sc_box = 0;
  sc_pin_label = 0;
  sc_username_label = 0;
  sc_username_input = 0;
  sc_pin_input = 0;
  sc_keyid_label = 0;
  sc_keyid_input = 0;

  use_key = 0;
  use_password = 0;
  use_rsa_key = 0;
  use_smartcard = 0;
}
  
OpenDatabasePanel::~OpenDatabasePanel()
{
  if(ok_btn) delete ok_btn;
  if(cancel_btn) delete cancel_btn;
  if(password_label) delete password_label;
  if(password_input) delete password_input;
  if(password_box) delete password_box;
  if(key_box) delete key_box;
  if(key_label) delete key_label;
  if(key_input) delete key_input;
  if(browse) delete browse;
  if(rsa_key_box) delete rsa_key_box;
  if(rsa_passphrase_label) delete rsa_passphrase_label;
  if(rsa_passphrase_input) delete rsa_passphrase_input;
  if(rsa_username_label) delete rsa_username_label;
  if(rsa_username_input) delete rsa_username_input;
  if(rsa_key_label) delete rsa_key_label;
  if(rsa_key_input) delete rsa_key_input;
  if(rsa_browse) delete rsa_browse;
  if(sc_box) delete sc_box;
  if(sc_pin_label) delete sc_pin_label;
  if(sc_username_label) delete sc_username_label;
  if(sc_username_input) delete sc_username_input;
  if(sc_pin_input) delete sc_pin_input;
  if(sc_keyid_label) delete sc_keyid_label;
  if(sc_keyid_input) delete sc_keyid_input;
}

void OpenDatabasePanel::ShowPanel(gxString &fname)
{
  password_input->Clear();
  curr_fname = fname;

  is_ok = 0;

  ShowModal();
}

int OpenDatabasePanel::SmartCardOpenDatabase()
{
  int rv;
  gxString sbuf;
  SmartCardOB sc;
  gxString smartcard_username;

  use_smartcard = 1;
  if(sc_keyid_input->GetValue().IsNull()) {
    ProgramError->Message("You must provide a cert ID for the smart card");   
    is_ok = 0;
    return 0;
  }
    
  if(sc_username_input->GetValue().IsNull()) {
    ProgramError->Message("You must provide a user name for the smart card");   
    is_ok = 0;
    return 0;
  }

  if(sc_pin_input->GetValue().IsNull()) {
    ProgramError->Message("You must input your smart card PIN number");   
    is_ok = 0;
    return 0;
  }

  smartcard_username = (const char *)sc_username_input->GetValue();
  sc.SetEnginePath(progcfg->SC_enginePath.c_str());
  sc.SetModulePath(progcfg->SC_modulePath.c_str()); 
  sc.SetEngineID(progcfg->SC_engine_ID.c_str());
  
  sbuf = (const char *)sc_keyid_input->GetValue();
  sc.SetCertID(sbuf.c_str());      
  sbuf = (const char *)sc_pin_input->GetValue();
  sc_pin_input->Clear();
  sc.SetPin(sbuf.c_str());
  sbuf.Clear(1);
  
  gxDatabase f;
  gxDatabaseError dberr = f.Open(curr_fname.c_str(), gxDBASE_READONLY); 
  if(dberr != gxDBASE_NO_ERROR) {
    sbuf << clear << "Error opening database" << "\n" <<   gxDatabaseExceptionMessage(dberr); 
    ProgramError->Message(sbuf.c_str());   
    is_ok = 0;
    return 0;
  }
  
  DatabaseUserAuth db_auth;
  db_auth.f = &f; // Set the file pointer

  rv = db_auth.DecryptWithSmartcard(&sc, smartcard_username);
  if(rv != 0) {
    ProgramError->Message(db_auth.err.c_str());   
    is_ok = 0;
    return 0;
  }
  
  f.Close();

  sbuf = sc.cert_id;
  if(progcfg->SC_cert_id != sbuf) { // Update the global SC cert ID
    progcfg->SC_cert_id = sc.cert_id;
    gxConfig CfgData(progcfg->cfgFile.c_str());
    CfgData.ChangeConfigValue("SC_cert_id", (const char *)sc.cert_id);
  }
  
  progcfg->global_dbparms.crypt_key = DBStringConfig::crypt_key;
  is_ok = 1;
  return 1;
}

int OpenDatabasePanel::RSAOpenDatabase()
{
  int rv;
  char private_key[RSA_max_keybuf_len];
  unsigned private_key_len = 0;
  int has_passphrase = 0;
  char *passphrase = 0;
  gxString rsa_key_username;
  gxString sbuf, pbuf;
  
  if(!futils_exists(rsa_key_input->GetValue().c_str()) || !futils_isfile(rsa_key_input->GetValue().c_str())) {
    ProgramError->Message("The private RSA key file does not exist or cannot be read");   
    is_ok = 0;
    return 0;
  }
   
  rv = RSA_read_key_file(rsa_key_input->GetValue().c_str(), private_key, sizeof(private_key), &private_key_len, &has_passphrase);
  if(rv != RSA_NO_ERROR) {
    sbuf << clear << "Error reading private key file " << RSA_err_string(rv);
    ProgramError->Message(sbuf.c_str());
    is_ok = 0;
    return 0;
  }
  
  if(rsa_username_input->GetValue().IsNull()) {
    ProgramError->Message("You must provide a user name for the private RSA key");   
    is_ok = 0;
    return 0;
  }

  rsa_key_username = (const char *)rsa_username_input->GetValue();
  
  if(has_passphrase) {
    if(rsa_passphrase_input->GetValue().IsNull()) {
      ProgramError->Message("You must enter the private key passphrase");   
      is_ok = 0;
      return 0;
    }

    pbuf = (const char *)rsa_passphrase_input->GetValue();
    passphrase = (char *)pbuf.GetSPtr();
  }
  
  gxDatabase f;
  gxDatabaseError dberr = f.Open(curr_fname.c_str(), gxDBASE_READONLY); 
  if(dberr != gxDBASE_NO_ERROR) {
    sbuf << clear << "Error opening database" << "\n" <<   gxDatabaseExceptionMessage(dberr); 
    ProgramError->Message(sbuf.c_str());   
    is_ok = 0;
    return 0;
  }

  DatabaseUserAuth db_auth;
  db_auth.f = &f; // Set the file pointer

  rv = db_auth.DecryptWithRSAKey(private_key,  private_key_len, rsa_key_username, passphrase);
  if(rv != 0) {
    ProgramError->Message(db_auth.err.c_str());   
    is_ok = 0;
    return 0;
  }
  
  f.Close();

  progcfg->global_dbparms.crypt_key = DBStringConfig::crypt_key;
  is_ok = 1;
  return 1;
}

int OpenDatabasePanel::AESKeyOpenDatabase()
{
  if(!futils_exists(key_input->GetValue().c_str()) || !futils_isfile(key_input->GetValue().c_str())) {
    ProgramError->Message("The key file does not exist or cannot be read");   
    is_ok = 0;
    return 0;
  }
  
  gxString sbuf, ebuf;
  MemoryBuffer key;
  if(read_key_file(key_input->GetValue().c_str(), key, ebuf) != 0) {
    ProgramError->Message(ebuf.c_str());   
    is_ok = 0;
    return 0;
  }
  
  progcfg->global_dbparms.crypt_key = key;
  key_input->Clear();

  gxDatabase f;
  gxDatabaseError rv = f.Open(curr_fname.c_str(), gxDBASE_READONLY); 
  if(rv != gxDBASE_NO_ERROR) {
    sbuf << clear << "Error opening database" << "\n" <<   gxDatabaseExceptionMessage(rv); 
    ProgramError->Message(sbuf.c_str());   
    is_ok = 0;
    return 0;
  }
  gxDatabaseConfig dbconfig;
  if(!dbconfig.ReadConfig(&f)) {
    ProgramError->Message("Error reading database config");   
    is_ok = 0;
    f.Close();
    return 0;
  }
  f.Close();

  DatabaseUserAuth db_auth;
  db_auth.f = &f; // Set the file pointer

  DBStringConfig::crypt_key = key;
  char *s = dbconfig.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    sbuf << clear << "Error decrypting database hash with key provided" << "\n" << AES_err_string(DBStringConfig::AES_error_level);
    ProgramError->Message(sbuf.c_str());   
    is_ok = 0;
    return 0;
  }
  if(!s) {
    ProgramError->Message("Error decrypting database hash with key provided");   
    is_ok = 0;
    return 0;
  }

  progcfg->global_dbparms.crypt_key = key;
  key.Clear(1);
  is_ok = 1;
  return 1;
}

int OpenDatabasePanel::PasswordOpenDatabase()
{
  gxString sbuf;
  gxString pass_buf = (const char *)password_input->GetValue();
  if(pass_buf.length() < 8) { 
    ProgramError->Message("Your password should be at least 8 characters long");
    is_ok = 0;
    return 0;
    password_input->Clear();
    progcfg->global_dbparms.crypt_key.Clear();
    progcfg->global_dbparms.crypt_key.Cat(pass_buf.c_str(), pass_buf.length());
    pass_buf.Clear(1);
  }

  gxDatabase f;
  gxDatabaseError rv = f.Open(curr_fname.c_str(), gxDBASE_READONLY); 
  if(rv != gxDBASE_NO_ERROR) {
    sbuf << clear << "Error opening database" << "\n" <<   gxDatabaseExceptionMessage(rv); 
    ProgramError->Message(sbuf.c_str());   
    is_ok = 0;
    return 0;
  }
  gxDatabaseConfig dbconfig;
  if(!dbconfig.ReadConfig(&f)) {
    ProgramError->Message("Error reading database config");   
    is_ok = 0;
    f.Close();
    return 0;
  }
  f.Close();
  
  DatabaseUserAuth db_auth;
  db_auth.f = &f; // Set the file pointer

  DBStringConfig::crypt_key.Clear();
  DBStringConfig::crypt_key.Cat(pass_buf.c_str(), pass_buf.length());

  char *s = dbconfig.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    sbuf << clear << "Error decrypting database hash with password provided" << "\n" << AES_err_string(DBStringConfig::AES_error_level);
    ProgramError->Message(sbuf.c_str());   
    is_ok = 0;
    return 0;
  }
  if(!s) {
    ProgramError->Message("Error decrypting database hash with password provided");   
    is_ok = 0;
    return 0;
  }

  progcfg->global_dbparms.crypt_key = DBStringConfig::crypt_key;
  is_ok = 1;
  return 1;
}

int OpenDatabasePanel::TestInput()
{
  use_key = 0;
  use_password = 0;
  use_rsa_key = 0;
  use_smartcard = 0;

  if(password_input->GetValue().IsNull() && key_input->GetValue().IsNull()
     && rsa_key_input->GetValue().IsNull() && sc_pin_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You must use a password, key, or smart card to open this database");
    is_ok = 0;
    return 0;
  }

  if(!password_input->GetValue().IsNull() && !key_input->GetValue().IsNull()
     && !rsa_key_input->GetValue().IsNull() && !sc_pin_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You only one method at a time to open this database");   
    is_ok = 0;
    return 0;
  }

  // Set the logic here to determine with open method to use
  if(!key_input->GetValue().IsNull() && password_input->GetValue().IsNull()
     && rsa_key_input->GetValue().IsNull() && sc_pin_input->GetValue().IsNull()) use_key = 1;
  if(!password_input->GetValue().IsNull() && key_input->GetValue().IsNull()
     && rsa_key_input->GetValue().IsNull() && sc_pin_input->GetValue().IsNull()) use_password = 1;
  if(!rsa_key_input->GetValue().IsNull() &&  password_input->GetValue().IsNull() 
     && key_input->GetValue().IsNull() && sc_pin_input->GetValue().IsNull()) use_rsa_key = 1;
  if(!sc_pin_input->GetValue().IsNull() && rsa_key_input->GetValue().IsNull()
     && key_input->GetValue().IsNull() && password_input->GetValue().IsNull()) use_smartcard = 1;

  if(use_rsa_key) {
    return RSAOpenDatabase();
  }

  if(use_smartcard) {
    return SmartCardOpenDatabase();
  }
  
  if(use_key) {
    return AESKeyOpenDatabase();
  }

  if(password_input->GetValue().IsNull() && !use_key && !use_rsa_key && !use_smartcard) {
    password_input->Clear();
    ProgramError->Message("You must enter a password for this database");   
    is_ok = 0;
    return 0;
  }

  if(use_password) {
    return PasswordOpenDatabase();
  }

  is_ok = 1;
  return 1;
}

void OpenDatabasePanel::OnTextControlPasswordEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlKeyFileEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlRSAKeyFileEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlRSAUsernameEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlRSAPassPhraseEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlSCUsernameEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlSCPinEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnTextControlSCCertIDEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnOK(wxCommandEvent &WXUNUSED(event))
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  is_ok = 0;
  Show(FALSE);
}

void OpenDatabasePanel::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
// Define the behaviour for the dialog closing when the user has 
// tried to close a dialog box using the window manager (X) or 
// system menu (Windows).
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void OpenDatabasePanel::OnKeyBrowse(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Open symmetric encryption key file:",
		      progcfg->docDir.c_str(), "",
		      "*.key",
		      wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  
  if(dialog.ShowModal() == wxID_OK) {
    key_input->Clear();
    key_input->AppendText(dialog.GetPath());
  }
}

void OpenDatabasePanel::OnRSAKeyBrowse(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Open private RSA key file:",
		      progcfg->docDir.c_str(), "",
		      "*.pem",
		      wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  
  if(dialog.ShowModal() == wxID_OK) {
    rsa_key_input->Clear();
    rsa_key_input->AppendText(dialog.GetPath());
  }
}

OpenDatabasePanel *InitOpenDatabasePanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=900;
  int button_ypos = 800;

  OpenDatabasePanel *panel = new OpenDatabasePanel(parent,
						   OPENDATABASE_PANEL_ID,
						   (char *)"Open Database",
						   xpos, ypos, width, height,
				   wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
						   (char*)"Open Database");
  
  if(!panel) return 0;

  panel->password_box = new wxStaticBox(panel, -1, "Use Password to open", wxPoint(9, 17), wxSize(270,95));
  
  panel->password_label = new wxStaticText(panel, -1, "Password", wxPoint(15, 40));
  panel->password_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_PASSWORD,
					 "",
					 wxPoint(15, 69),
					 wxSize(250,25),
					 wxTE_PROCESS_ENTER|wxTE_PASSWORD);

  panel->key_box = new wxStaticBox(panel, -1, "Use Symmetric Encryption Key", wxPoint(9, 125), wxSize(270,135));
  panel->key_label = new wxStaticText(panel, -1, "Key File Name", wxPoint(15, 161));
  
  panel->key_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_KEY_FILE, 
				    "", wxPoint(15, 187), wxSize(250, 25)),

  panel->browse = panel->cancel_btn = new wxButton(panel, ID_OPENDATABASE_KEY_BROWSE, "Browse",
						   wxPoint(15, 220), wxSize(75, 25));


  panel->rsa_key_box = new wxStaticBox(panel, -1, "Use Private RSA Key", wxPoint(9, 280), wxSize(270,270));
  
  panel->rsa_username_label = new wxStaticText(panel, -1, "RSA Key Username", wxPoint(15, 310));
  panel->rsa_username_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_RSA_USERNAME, 
					     progcfg->USERNAME.c_str(), wxPoint(15, 332), wxSize(250, 25)),

  panel->rsa_key_label = new wxStaticText(panel, -1, "Private Key File Name", wxPoint(15, 368));
  panel->rsa_key_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_RSA_KEY, 
					"", wxPoint(15, 394), wxSize(250, 25)),

  panel->rsa_browse = panel->cancel_btn = new wxButton(panel, ID_OPENDATABASE_RSA_BROWSE, "Browse",
						       wxPoint(15, 430), wxSize(75, 25));
  panel->rsa_passphrase_label = new wxStaticText(panel, -1, "Private Key Passphrase", wxPoint(15, 466));
  panel->rsa_passphrase_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_RSA_PASSPHRASE,
					      "",
					      wxPoint(15, 492),
					      wxSize(250,25),
					      wxTE_PROCESS_ENTER|wxTE_PASSWORD);

  panel->sc_box = new wxStaticBox(panel, -1, "Use Smart Card", wxPoint(9, 572), wxSize(270,200));
  panel->sc_username_label = new wxStaticText(panel, -1, "Smart Card Username", wxPoint(15, 598));
  panel->sc_username_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_SC_USERNAME,
					    progcfg->USERNAME.c_str(), wxPoint(15, 624), wxSize(250,25));

  panel->sc_pin_label = new wxStaticText(panel, -1, "Smart Card PIN", wxPoint(15, 655));
  panel->sc_pin_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_SC_PIN,
				       "",
				       wxPoint(15, 676),
				       wxSize(250,25),
				       wxTE_PROCESS_ENTER|wxTE_PASSWORD);
  panel->sc_keyid_label = new wxStaticText(panel, -1, "Smart Card Cert ID", wxPoint(15, 707));
  panel->sc_keyid_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL_SC_CERT_ID, 
					 progcfg->SC_cert_id.c_str(), wxPoint(15, 728), wxSize(250, 25)),
  
  panel->ok_btn = new wxButton(panel, ID_OPENDATABASE_OK, "OK",
			       wxPoint(17, button_ypos),
			       wxSize(46, 25));
  
  panel->cancel_btn = new wxButton(panel, ID_OPENDATABASE_CANCEL, "Cancel",
				   wxPoint(83, button_ypos),
				   wxSize(56, 25));

  panel->Show(FALSE);
  
  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
