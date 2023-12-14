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

#ifdef __USE_DB_ENCRYPTION__

BEGIN_EVENT_TABLE(OpenDatabasePanel, wxDialog)
  EVT_CLOSE(OpenDatabasePanel::OnCloseWindow)
  EVT_BUTTON (ID_OPENDATABASE_OK, OpenDatabasePanel::OnOK)
  EVT_BUTTON (ID_OPENDATABASE_CANCEL, OpenDatabasePanel::OnCancel)
  EVT_BUTTON (ID_OPENDATABASE_BROWSE, OpenDatabasePanel::OnBrowse)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL1, OpenDatabasePanel::OnTextControl1Enter)
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL1, OpenDatabasePanel::OnTextControl1Enter)
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
}

void OpenDatabasePanel::ShowPanel(gxString &fname)
{
  password_input->Clear();
  curr_fname = fname;

  is_ok = 0;

  ShowModal();
}

int OpenDatabasePanel::TestInput()
{
  use_key = 0;
  use_password = 0;
  use_rsa_key = 0;
  use_smartcard = 0;
  gxString sbuf;
  
  if(password_input->GetValue().IsNull() && key_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You must use a password or a key file to open this database");   
    is_ok = 0;
    return 0;
  }

  if(!password_input->GetValue().IsNull() && !key_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You only one method at a time to open this database");   
    is_ok = 0;
    return 0;
  }

  // Set the logic here to determine with open method to use
  if(!key_input->GetValue().IsNull() && password_input->GetValue().IsNull()) use_key = 1;
  if(key_input->GetValue().IsNull() && !password_input->GetValue().IsNull()) use_password = 1;
  
  if(use_key) {
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

    progcfg->global_dbparms.crypt_key = key;
  
    password_input->Clear();
    key_input->Clear();
    use_key = 1;
  }

  if(password_input->GetValue().IsNull() && !use_key && !use_rsa_key && !use_smartcard) {
    password_input->Clear();
    ProgramError->Message("You must enter a password for this database");   
    is_ok = 0;
    return 0;
  }

  if(use_password) {
    gxString pass_buf = (const char *)password_input->GetValue();
    if(pass_buf.length() < 8) { 
      ProgramError->Message("Your password should be at least 8 characters long");
      is_ok = 0;
      return 0;
    }
    password_input->Clear();
    key_input->Clear();
    progcfg->global_dbparms.crypt_key.Clear();
    progcfg->global_dbparms.crypt_key.Cat(pass_buf.c_str(), pass_buf.length());
    pass_buf.Clear(1);
  }

  gxDatabase f;
  gxDatabaseError rv = f.Open(curr_fname.c_str(), gxDBASE_READONLY); 
  if(rv != gxDBASE_NO_ERROR) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "ERROR - Cannot open " << curr_fname.c_str() << "\n" << flush;
    debug_log << gxDatabaseExceptionMessage(rv) << "\n" << flush;
#endif
    ProgramError->Message("Error opening specified database");   
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

  char *dup = new char[DBStringLength];
  memmove(dup, &dbconfig.database_name, DBStringLength);
  if(dup[0] == 0) {
    ProgramError->Message("Invalid password hash in database config");   
    is_ok = 0;
    delete[] dup;
    return 0;
  }

  int crypt_error;
  CryptoHeader eh;

  // Get the length of the encrytpted buffer
  memmove(&eh, dup, sizeof(eh));
  unsigned cryptLen = eh.crypt_len;
    
  // Realign the encrypted buffer
  memmove(dup, (dup+sizeof(eh)), cryptLen);

  // Decrypt the compressed string
  crypt_error = AES_Decrypt(dup, &cryptLen, 
			    (const unsigned char *)progcfg->global_dbparms.crypt_key.m_buf(), 
			    progcfg->global_dbparms.crypt_key.length());
  if(crypt_error != AES_NO_ERROR) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "Error decrypting password hash" << "\n" << flush; 
    debug_log << AES_err_string(crypt_error) << "\n" << flush; 
#endif
    delete[] dup;
    dup = 0;
    if(crypt_error == AES_ERROR_BAD_SECRET) {
      if(use_password) {
	ProgramError->Message("The password you entered is not correct\nCannot open database");
      }
      else if(use_key) {
	ProgramError->Message("The key you used is not vaild\nCannot open database");
      }
      else {
	ProgramError->Message("Cannot open database with the credentials you provided");
      }
    }
    else {
      sbuf << clear << "Error decrypting password hash " << AES_err_string(crypt_error) << "\nCannot open database";
      ProgramError->Message(sbuf.c_str());
    }
    is_ok = 0;
    password_input->Clear();
    return 0;
  }

  delete[] dup;
  dup = 0;
  is_ok = 1;

  return 1;
}

void OpenDatabasePanel::OnTextControl1Enter(wxCommandEvent &event)
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

void OpenDatabasePanel::OnBrowse(wxCommandEvent &event)
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

void OpenDatabasePanel::OnTextControl2Enter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

OpenDatabasePanel *InitOpenDatabasePanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=550;
  int button_ypos = 450;

  OpenDatabasePanel *panel = new OpenDatabasePanel(parent,
						   OPENDATABASE_PANEL_ID,
						   (char *)"Open Database",
						   xpos, ypos, width, height,
				   wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
						   (char*)"Open Database");
  
  if(!panel) return 0;

  panel->password_box = new wxStaticBox(panel, -1, "Use Password to open", wxPoint(9, 47), wxSize(270,95));
  
  panel->password_label = new wxStaticText(panel, -1,
					   "Password",
					   wxPoint(15, 69));
  
  panel->password_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL1,
					 "",
					 wxPoint(15, 99),
					 wxSize(250,25),
					 wxTE_PROCESS_ENTER|wxTE_PASSWORD);

  panel->key_box = new wxStaticBox(panel, -1, "Use Symmetric Encryption Key", wxPoint(9, 240), wxSize(270,135));
  panel->key_label = new wxStaticText(panel, -1,
				      "File Name",
				      wxPoint(15, 266));
  
  panel->key_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL2,
				    "",
				    wxPoint(15, 292),
				    wxSize(250, 25)),

  panel->browse = panel->cancel_btn = new wxButton(panel, ID_OPENDATABASE_BROWSE, "Browse",
						   wxPoint(15, 330),
						   wxSize(75, 25));
  
  panel->ok_btn = new wxButton(panel, ID_OPENDATABASE_OK, "OK",
			       wxPoint(17, button_ypos),
			       wxSize(46, 25));
  
  panel->cancel_btn = new wxButton(panel, ID_OPENDATABASE_CANCEL, "Cancel",
				   wxPoint(83, button_ypos),
				   wxSize(56, 25));

  panel->Show(FALSE);
  
  return panel;
}

#endif // __USE_DB_ENCRYPTION__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
