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
  EVT_BUTTON (ID_NEWDATABASE_BROWSE, NewDatabasePanel::OnBrowse)
  EVT_TEXT_ENTER(ID_NEWDATABASE_TEXTCONTROL1, NewDatabasePanel::OnTextControl1Enter)
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

#ifdef __USE_DB_ENCRYPTION__
  password_label = confirm_password_label = 0;
  password_input = confirm_password_input = 0;
#endif
}
  
NewDatabasePanel::~NewDatabasePanel()
{
  if(name_label) delete name_label; 
  if(name_input) delete name_input;
  if(ok_btn) delete ok_btn;
  if(cancel_btn) delete cancel_btn;

#ifdef __USE_DB_ENCRYPTION__
  if(password_label) delete password_label;
  if(confirm_password_label) delete confirm_password_label;
  if(password_input) delete password_input;
  if(confirm_password_input) delete confirm_password_input;
  if(password_box) delete password_box;
  if(key_box) delete key_box;
  if(key_label) delete key_label;
  if(key_input) delete key_input;
  if(browse) delete browse;
#endif
}

void NewDatabasePanel::ShowPanel()
{
  name_input->Clear();

#ifdef __USE_DB_ENCRYPTION__
  password_input->Clear();
  confirm_password_input->Clear();
#endif

  is_ok = 0;

  ShowModal();
}

void NewDatabasePanel::OnTextControl1Enter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

int NewDatabasePanel::TestInput()
{
  gxString sbuf;

#ifdef __APP_DEBUG_VERSION__
    debug_log << "NewDatabasePanel::TestInput() call" << "\n" << flush;
#endif

  if(name_input->GetValue().IsNull()) {
    ProgramError->Message("You must enter a name for this database");   
    is_ok = 0;
    return 0;
  }

#ifdef __USE_DB_ENCRYPTION__
  if(password_input->GetValue().IsNull() && key_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You use password or a key file to create this database");   
    is_ok = 0;
    return 0;
  }


  if(!password_input->GetValue().IsNull() && !key_input->GetValue().IsNull()) {
    password_input->Clear();
    key_input->Clear();
    ProgramError->Message("You can use use a password or a key file to create this database");   
    is_ok = 0;
    return 0;
  }

  if(!key_input->GetValue().IsNull()) {
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
    confirm_password_input->Clear();
    key_input->Clear();
    is_ok = 1;
    return 1;
  }

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
      // Our encryption requires a min of 8 character passwords
      // NOTE: These values must match the x_panel values
      if(pass_buf.length() == 0) {
	pass_buf = "lkjasdlkjalskdjlaksjda0198213354.0s23)()87123+_+_76&^(*@#!@#+-*213shasjdsjasjdSUISJ#*@#MJC";
      }
      else if(pass_buf.length() == 1) {
	pass_buf << "LKLKSJADJISD--911kkasd0&(*^A9721ln243809sllNSDFasfd><?>AS<POF-0982130asoiasoi^&#781q932101982)XZXWcxz";
      }
      else if(pass_buf.length() == 2) {
	pass_buf << "jlahs890723LKJSD)*&A3lki809asudlnasd908738jlq908asd78ashdoasi70nc*(&S(*ASDAy1qlj32eb497acnb a98s7ds2da";
      }
      else if(pass_buf.length() == 3) {
	pass_buf << "LKJASLMD9312871283lns87*(&SA1oljd98w,m S(A&^D832knasd9q6914bk^(*&yt(knjasbdia&^871KJ9((^klj2q39476bdc";
      }
      else if(pass_buf.length() == 4) {
	pass_buf << "LKASD()903214ksadf809781l23907saodJKL(AS*DASD*(*A&^H#@(*AS&^DL#97213lsdf899(^(AS^(A^SD&A^D(*G#Bkbmbr98";
      }
      else if(pass_buf.length() == 5) {
	pass_buf << "-098324kjasd0-q7983LKJSAD)(*D0831ljkhAs)(90a7wdokjh)*(OIHASD80-231y4o1hd908AszND128209183091Hn 08aqy08";
      }
      else if(pass_buf.length() == 6) {
	pass_buf << "k*ASKDAN312091u09kmasd0aq8903081238akjOI)A(*AS,129812679(*D(*Aws1m23980asd08aYW08y123SDLHAJ2381908j)((";
      }
      else if(pass_buf.length() == 7) {
	pass_buf << "JLKAS*(761298*(*@#&&&Q@&KJbU^79862KBADSUIYAKJSB9872*(&^&^(&s^dasdKBJQ23H4981Yk*s^d&(*sEB 2139786k89&W,";
      }
      else {
	// We have 8 or more characters
      }
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

  progcfg->global_dbparms.passwd = pass_buf;
  
  password_input->Clear();
  confirm_password_input->Clear();
  pass_buf.Clear();
#endif

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

void NewDatabasePanel::OnBrowse(wxCommandEvent &WXUNUSED(event))
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
#ifdef __USE_DB_ENCRYPTION__
  int xpos=50; int ypos=50; int width=350; int height=550;
  int button_ypos = 450;
#else
  int xpos=50; int ypos=50; int width=350; int height=165;
  int button_ypos = 100;
#endif

  NewDatabasePanel *panel = new NewDatabasePanel(parent,
						 NEWDATABASE_PANEL_ID,
						 (char *)"Create New Database",
						 xpos, ypos, width, height,
			 wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
						 (char *)"Create New Database");
  
  if(!panel) return 0;
  panel->name_label = new wxStaticText(panel, -1,
				       "Database name",
				       wxPoint(11, 13));
  
  panel->name_input = new wxTextCtrl(panel, -1,
				     "",
				     wxPoint(11, 37),
				     wxSize(250,25));
  

#ifdef __USE_DB_ENCRYPTION__

  panel->password_box = new wxStaticBox(panel, -1, "Use Password", wxPoint(9, 73), wxSize(270,145));
  
  panel->password_label = new wxStaticText(panel, -1,
					   "Password",
					   wxPoint(15, 95));
  
  panel->password_input = new wxTextCtrl(panel, -1,
					 "",
					 wxPoint(15, 121),
					 wxSize(250,25),
					 wxTE_PASSWORD);
  
  panel->confirm_password_label = new wxStaticText(panel, -1,
						   "Confirm Password",
						   wxPoint(15, 149));
  
  panel->confirm_password_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL1,
						 "",
						 wxPoint(15, 175),
						 wxSize(250, 25),
						 wxTE_PROCESS_ENTER|wxTE_PASSWORD);

  panel->key_box = new wxStaticBox(panel, -1, "Use Symmetric Encryption Key", wxPoint(9, 240), wxSize(270,135));
  panel->key_label = new wxStaticText(panel, -1,
				      "File Name",
				      wxPoint(15, 266));
  
  panel->key_input = new wxTextCtrl(panel, ID_NEWDATABASE_TEXTCONTROL1,
				    "",
				    wxPoint(15, 292),
				    wxSize(250, 25)),

  panel->browse = panel->cancel_btn = new wxButton(panel, ID_NEWDATABASE_BROWSE, "Browse",
						   wxPoint(15, 330),
						   wxSize(75, 25));

#endif

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
