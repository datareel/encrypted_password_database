// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
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

BEGIN_EVENT_TABLE(NewDatabasePanel, wxDialog)
  EVT_CLOSE(NewDatabasePanel::OnCloseWindow)
  EVT_BUTTON (ID_NEWDATABASE_OK, NewDatabasePanel::OnOK)
  EVT_BUTTON (ID_NEWDATABASE_CANCEL, NewDatabasePanel::OnCancel)
  EVT_BUTTON (ID_NEWDATABASE_KEY_BROWSE, NewDatabasePanel::OnKeyFileBrowse)
  EVT_BUTTON (ID_NEWDATABASE_RSA_KEY_BROWSE, NewDatabasePanel::OnRSAKeyFileBrowse)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL_CONFIRM_PASSWORD, NewDatabasePanel::OnTextControlPasswordEnter)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL_KEY_FILE, NewDatabasePanel::OnTextControlKeyFileEnter)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL_RSA_USERNAME, NewDatabasePanel::OnTextControlRSAKeyFileEnter)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL_RSA_KEY, NewDatabasePanel::OnTextControlRSAUsernameEnter)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL_SC_USERNAME, NewDatabasePanel::OnTextControlSCUsernameEnter)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL_SC_CERT_ID, NewDatabasePanel::OnTextControlSCCertIDEnter)
END_EVENT_TABLE()

NewDatabasePanel::NewDatabasePanel(wxWindow *parent, wxWindowID id,
				   char *title,
				   int xpos, int ypos, int width, int height,
				   long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  name_label = 0; 
  name_input = 0;
  ok_btn = cancel_btn = 0;
  is_ok = 0;
  password_label = confirm_password_label = 0;
  password_input = confirm_password_input = 0;
  password_box = 0;
  key_box = 0;
  key_label = 0;
  key_input = 0;
  browse = 0;
  rsa_key_box = 0;
  rsa_username_label = 0;
  rsa_username_input = 0;
  rsa_key_label = 0;
  rsa_key_input = 0;
  rsa_browse = 0;
  sc_box = 0;
  sc_username_label = 0;
  sc_username_input = 0;
  sc_keyid_label = 0;
  sc_keyid_input = 0;
  sc_use_checkbox = 0;

  use_key = 0;
  use_password = 0;
  use_rsa_key = 0;
  use_smartcard = 0;
  use_cert_file = 0;
  
  memset(public_key, 0, sizeof(public_key));
  memset(random_key, 0, sizeof(random_key));
}
  
NewDatabasePanel::~NewDatabasePanel()
{
  if(name_label) delete name_label; 
  if(name_input) delete name_input;
  if(ok_btn) delete ok_btn;
  if(cancel_btn) delete cancel_btn;
  if(password_label) delete password_label;
  if(confirm_password_label) delete confirm_password_label;
  if(password_input) delete password_input;
  if(confirm_password_input) delete confirm_password_input;
  if(password_box) delete password_box;
  if(key_box) delete key_box;
  if(key_label) delete key_label;
  if(key_input) delete key_input;
  if(browse) delete browse;
  if(rsa_key_box) delete rsa_key_box;
  if(rsa_username_label) delete rsa_username_label;
  if(rsa_username_input) delete rsa_username_input;
  if(rsa_key_label) delete rsa_key_label;
  if(rsa_key_input) delete rsa_key_input;
  if(rsa_browse) delete rsa_browse;
  if(sc_box) delete sc_box;
  if(sc_username_label) delete sc_username_label;
  if(sc_username_input) delete sc_username_input;
  if(sc_keyid_label) delete sc_keyid_label;
  if(sc_keyid_input) delete sc_keyid_input;
  if(sc_use_checkbox) delete sc_use_checkbox;

  memset(public_key, 0, sizeof(public_key));
  memset(random_key, 0, sizeof(random_key));
}

void NewDatabasePanel::ShowPanel()
{
  name_input->Clear();
  password_input->Clear();
  confirm_password_input->Clear();
  is_ok = 0;

  ShowModal();
}

void NewDatabasePanel::OnKeyFileBrowse(wxCommandEvent &event)
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

void NewDatabasePanel::OnRSAKeyFileBrowse(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Open RSA public key file:",
		      progcfg->docDir.c_str(), "",
		      "*.pem",
		      wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  
  if(dialog.ShowModal() == wxID_OK) {
    rsa_key_input->Clear();
    rsa_key_input->AppendText(dialog.GetPath());
  }
}

void NewDatabasePanel::OnTextControlPasswordEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void NewDatabasePanel::OnTextControlKeyFileEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void NewDatabasePanel::OnTextControlRSAKeyFileEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void NewDatabasePanel::OnTextControlRSAUsernameEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void NewDatabasePanel::OnTextControlSCUsernameEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void NewDatabasePanel::OnTextControlSCCertIDEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

int NewDatabasePanel::CheckPassword()
{
  gxString sbuf;
  
  if(password_input->GetValue().IsNull()) {
    password_input->Clear();
    ProgramError->Message("You must enter a password for this database");   
    is_ok = 0;
    return 0;
  }

  if(password_input->GetValue() != confirm_password_input->GetValue()) {
    password_input->Clear();
    confirm_password_input->Clear();
    ProgramError->Message("Your passwords do not match");
    is_ok = 0;
    return 0;
  }

  gxString pass_buf = (const char *)password_input->GetValue();
  if(progcfg->password_length) {
    if(pass_buf.length() < (unsigned)progcfg->min_password_length) {
      password_input->Clear();
      confirm_password_input->Clear();
      pass_buf.Clear();
      sbuf << clear << "Your password does not meet the minimum security requirement!"
	   << "\n" << "Your password must be at least " 
	   << progcfg->min_password_length << " characters long.";
      ProgramError->Message(sbuf.c_str());
      is_ok = 0;
      return 0;
    }
  }
  else {
    if(pass_buf.length() < 8) { 
      ProgramError->Message("Your password must be at least 8 characters long");
      is_ok = 0;
      return 0;
    }
  }

  if(progcfg->complex_passwords) {
    int is_complex = 0;
    int has_spec = 0;
    int has_num = 0;
    int has_alpha = 0;
    int has_big_alpha = 0;
    char spec_array[32] = { '~', '`', '@', '#', '$', '%', '^', '&', '*', 
			    '(', ')', '-', '_', '+', '=', '{', '[', '}', 
			    ']', '|', '\\', ':', ';', '\"', ',', '<', 
			    '\'', '.', '>', '?', '/', '!' };
    char num_array[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    char alpha_array[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
			     'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 
			     'u', 'v', 'w', 'x', 'y', 'z' };
    char big_alpha_array[26] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 
				 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 
				 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
    unsigned i, j;
    for(i = 0; i < pass_buf.length(); i++) {
      for(j = 0; j < sizeof(spec_array); j++) {
	if(pass_buf[i] == spec_array[j]) {
	  has_spec++;
	  break;
	} 
      }
      if(has_spec) break;
    }
    for(i = 0; i < pass_buf.length(); i++) {
      for(j = 0; j < sizeof(num_array); j++) {
	if(pass_buf[i] == num_array[j]) {
	  has_num++;
	  break;
	} 
      }
      if(has_num) break;
    }
    for(i = 0; i < pass_buf.length(); i++) {
      for(j = 0; j < sizeof(alpha_array); j++) {
	if(pass_buf[i] == alpha_array[j]) {
	  has_alpha++;
	  break;
	} 
      }
      if(has_alpha) break;
    }
    for(i = 0; i < pass_buf.length(); i++) {
      for(j = 0; j < sizeof(big_alpha_array); j++) {
	if(pass_buf[i] == big_alpha_array[j]) {
	  has_big_alpha++;
	  break;
	} 
      }
      if(has_big_alpha) break;
    }
    if((has_spec) && (has_num) && 
       (has_alpha) && (has_big_alpha)) is_complex = 1;
    if(!is_complex) {
      password_input->Clear();
      confirm_password_input->Clear();
      pass_buf.Clear();
      ProgramError->Message("Your password does not meet the minimum security requirement! \
       \n\nYour password must contain a combination of upper and lower case letters \
       \nnumbers, and special characters (meaning punctuation marks or symbol keys)");
      is_ok = 0;
      return 0;
    }
  }

  // Set the password for this database
  progcfg->global_dbparms.crypt_key.Clear(1);
  progcfg->global_dbparms.crypt_key.Cat(pass_buf.c_str(), pass_buf.length());
  
  password_input->Clear();
  confirm_password_input->Clear();
  pass_buf.Clear(1);
  return 1;
}


int NewDatabasePanel::TestInput()
{
  use_key = 0;
  use_password = 0;
  use_rsa_key = 0;
  use_smartcard = 0;

  gxString sbuf;
  int rv;
  
#ifdef __APP_DEBUG_VERSION__
    debug_log << "NewDatabasePanel::TestInput() call" << "\n" << flush;
#endif

  if(name_input->GetValue().IsNull()) {
    ProgramError->Message("You must enter a name for this database");   
    is_ok = 0;
    return 0;
  }
  
  if(password_input->GetValue().IsNull() && key_input->GetValue().IsNull()
     && rsa_key_input->GetValue().IsNull() && !sc_use_checkbox->IsChecked()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You must use a password, key, or smart card to create this database");
    is_ok = 0;
    return 0;
  }

  if(!password_input->GetValue().IsNull() && !key_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You cannot both password and symmetric key to create this database");   
    is_ok = 0;
    return 0;
  }

  if(!rsa_key_input->GetValue().IsNull()) {
    use_rsa_key = 1;
    
    if(!futils_exists(rsa_key_input->GetValue().c_str()) || !futils_isfile(rsa_key_input->GetValue().c_str())) {
      ProgramError->Message("The public RSA key file does not exist or cannot be read");   
      is_ok = 0;
      return 0;
    }
   
    rv = RSA_read_key_file(rsa_key_input->GetValue().c_str(), public_key, sizeof(public_key), &public_key_len);
    if(rv != RSA_NO_ERROR) {
      sbuf << clear << "Error reading public key file " << RSA_err_string(rv);
      ProgramError->Message(sbuf.c_str());
      is_ok = 0;
      return 0;
    }
  
    if(rsa_username_input->GetValue().IsNull()) {
      ProgramError->Message("You must provide a user name for the public RSA key");   
      is_ok = 0;
      return 0;
    }
    
    rsa_key_username = (const char *)rsa_username_input->GetValue();

    if(password_input->GetValue().IsNull() && key_input->GetValue().IsNull()) {
      // Use a random key if no password or symmectric are provided with RSA
      AES_fillrand(random_key, sizeof(random_key));

      // Set the random key for this database
      progcfg->global_dbparms.crypt_key.Clear(1);
      progcfg->global_dbparms.crypt_key.Cat(random_key, sizeof(random_key));

    }
  }

  if(sc_use_checkbox->IsChecked()) {
    use_smartcard = 1;
    if(sc_keyid_input->GetValue().IsNull()) {
      ProgramError->Message("You must use provide a cert ID for the smart card");   
      is_ok = 0;
      return 0;
    }
    
      
    if(sc_username_input->GetValue().IsNull()) {
      ProgramError->Message("You must provide a user name for the smart card");   
      is_ok = 0;
      return 0;
    }

    smartcard_username = (const char *)sc_username_input->GetValue();
    sc.SetEnginePath(progcfg->SC_enginePath.c_str());
    sc.SetModulePath(progcfg->SC_modulePath.c_str()); 
    sc.SetEngineID(progcfg->SC_engine_ID.c_str());
    sbuf = (const char *)sc_keyid_input->GetValue();
    sc.SetCertID(sbuf.c_str());      
    use_cert_file = 0;
    
    if(password_input->GetValue().IsNull() && key_input->GetValue().IsNull()) {
      // Use a random key if no password or symmectric are provided with RSA
      AES_fillrand(random_key, sizeof(random_key));
      
      // Set the random key for this database
      progcfg->global_dbparms.crypt_key.Clear(1);
      progcfg->global_dbparms.crypt_key.Cat(random_key, sizeof(random_key));
    }
    
  }

  if(!key_input->GetValue().IsNull()) {
    use_key = 1;
    
    if(!futils_exists(key_input->GetValue().c_str()) || !futils_isfile(key_input->GetValue().c_str())) {
      ProgramError->Message("The key file does not exist or cannot be read");   
      is_ok = 0;
      return 0;
    }
    
    gxString ebuf;
    MemoryBuffer key;
    if(read_key_file(key_input->GetValue().c_str(), key, ebuf) != 0) {
      ProgramError->Message(ebuf.c_str());   
      is_ok = 0;
      return 0;
    }

    // set the key
    progcfg->global_dbparms.crypt_key = key;
  
    password_input->Clear();
    confirm_password_input->Clear();
    key_input->Clear();
    is_ok = 1;
    return 1;
  }

  if(!password_input->GetValue().IsNull()) {
    use_password = 1;
    if(!CheckPassword()) {
      is_ok = 0;
      return 0;
      
    }
  }

  
  is_ok = 1;

#ifdef __APP_DEBUG_VERSION__
    debug_log << " Returning from NewDatabasePanel::TestInput() call with no errors" 
	      << "\n" << flush;
#endif

  return 1;
}

void NewDatabasePanel::OnOK(wxCommandEvent &WXUNUSED(event))
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void NewDatabasePanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  is_ok = 0;
  Show(FALSE);
}

void NewDatabasePanel::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
// Define the behaviour for the dialog closing when the user has 
// tried to close a dialog box using the window manager (X) or 
// system menu (Windows).
{
  if(!TestInput()) return;  
  Show(FALSE);
}

NewDatabasePanel *InitNewDatabasePanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=1000;
  int button_ypos = 900;

  NewDatabasePanel *panel = new NewDatabasePanel(parent,
						 NEWDATABASE_PANEL_ID,
						 (char *)"Create New Database",
						 xpos, ypos, width, height,
			 wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
						 (char *)"Create New Database");
  
  if(!panel) return 0;
  panel->name_label = new wxStaticText(panel, -1, "Name for New Database", wxPoint(11, 13));
  panel->name_input = new wxTextCtrl(panel, -1, "", wxPoint(11, 37), wxSize(250,25));
  
  panel->password_box = new wxStaticBox(panel, -1, "Create Using Password", wxPoint(9, 93), wxSize(270,150));
  panel->password_label = new wxStaticText(panel, -1, "Password", wxPoint(15, 115));
  panel->password_input = new wxTextCtrl(panel, -1, "", wxPoint(15, 141), wxSize(250,25), wxTE_PASSWORD);
  panel->confirm_password_label = new wxStaticText(panel, -1, "Confirm Password", wxPoint(15, 179));
  panel->confirm_password_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL_CONFIRM_PASSWORD,
						 "", wxPoint(15, 205), wxSize(250, 25), wxTE_PROCESS_ENTER|wxTE_PASSWORD);
  
  panel->key_box = new wxStaticBox(panel, -1, "Use Symmetric Encryption Key", wxPoint(9, 270), wxSize(270,135));
  panel->key_label = new wxStaticText(panel, -1, "File Name", wxPoint(15, 296));
  panel->key_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL_KEY_FILE,
				    "", wxPoint(15, 322), wxSize(250, 25));
  panel->browse =  new wxButton(panel, ID_NEWDATABASE_KEY_BROWSE, "Browse",
				wxPoint(15, 360), wxSize(75, 25));
  
  panel->rsa_key_box = new wxStaticBox(panel, -1, "Use Public RSA Key", wxPoint(9, 440), wxSize(270,200));
  panel->rsa_username_label = new wxStaticText(panel, -1, "RSA Key Username", wxPoint(15, 470));
  panel->rsa_username_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL_RSA_USERNAME, 
					     progcfg->USERNAME.c_str(), wxPoint(15, 492), wxSize(250, 25));
  panel->rsa_key_label = new wxStaticText(panel, -1, "Public Key File Name", wxPoint(15, 528));
  panel->rsa_key_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL_RSA_KEY, 
					"", wxPoint(15, 554), wxSize(250, 25));
  panel->rsa_browse = new wxButton(panel, ID_NEWDATABASE_RSA_KEY_BROWSE, "Browse",
				   wxPoint(15, 600), wxSize(75, 25));

  panel->sc_box = new wxStaticBox(panel, -1, "Smart Card", wxPoint(9, 672), wxSize(270,180));
  panel->sc_username_label = new wxStaticText(panel, -1, "Smart Card Username", wxPoint(15, 698));
  panel->sc_username_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL_SC_USERNAME,
					    progcfg->USERNAME.c_str(), wxPoint(15, 724), wxSize(250,25));
  
  panel->sc_keyid_label = new wxStaticText(panel, -1, "Smart Card Cert ID", wxPoint(15, 755));
  panel->sc_keyid_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL_SC_CERT_ID, 
					 progcfg->SC_cert_id.c_str(), wxPoint(15, 776), wxSize(250, 25));
  panel->sc_use_checkbox = new wxCheckBox(panel, -1, "Use Smart Card", wxPoint(15, 812), wxSize(250, 25));
  panel->sc_use_checkbox->SetValue(false);
  
  panel->ok_btn = new wxButton(panel, ID_NEWDATABASE_OK, "OK",
			       wxPoint(17, button_ypos),
			       wxSize(46, 25));
  
  panel->cancel_btn = new wxButton(panel, ID_NEWDATABASE_CANCEL, "Cancel",
				   wxPoint(83, button_ypos),
				   wxSize(56, 25));

  panel->Show(FALSE);
  
  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
