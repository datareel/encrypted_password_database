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
  EVT_TEXT_ENTER(ID_OPENDATABASE_TEXTCONTROL1, 
		 OpenDatabasePanel::OnTextControl1Enter)
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
}
  
OpenDatabasePanel::~OpenDatabasePanel()
{
  if(ok_btn) delete ok_btn;
  if(cancel_btn) delete cancel_btn;
  if(password_label) delete password_label;
  if(password_input) delete password_input;
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
  if(password_input->GetValue().IsNull()) {
    password_input->Clear();
    ProgramError->Message("You must enter a password for this database");   
    is_ok = 0;
    return 0;
  }

  gxString pass_buf = (const char *)password_input->GetValue();
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
  
  gxDatabase f;
  gxDatabaseError rv = f.Open(curr_fname.c_str(), gxDBASE_READONLY); 
  if(rv != gxDBASE_NO_ERROR) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "ERROR - Cannot open " << curr_fname.c_str() << "\n" << flush;
    debug_log << gxDatabaseExceptionMessage(rv) << "\n" << flush;
#endif
    ProgramError->Message("Error opening specified database");   
    is_ok = 0;
    password_input->Clear();
    pass_buf.Clear();
    return 0;
  }
  gxDatabaseConfig dbconfig;
  if(!dbconfig.ReadConfig(&f)) {
    ProgramError->Message("Error reading database config");   
    is_ok = 0;
    password_input->Clear();
    pass_buf.Clear();
    f.Close();
    return 0;
  }
  f.Close();

  char *dup = new char[DBStringLength];
  memmove(dup, &dbconfig.database_name, DBStringLength);
  if(dup[0] == 0) {
    ProgramError->Message("Invalid password hash in database config");   
    is_ok = 0;
    password_input->Clear();
    pass_buf.Clear();
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
			    (const unsigned char *)pass_buf.c_str(), 
			    pass_buf.length());
  if(crypt_error != AES_NO_ERROR) {
#ifdef __APP_DEBUG_VERSION__
    debug_log << "Error decrypting SPPC BL" << "\n" << flush; 
    debug_log << AES_err_string(crypt_error) << "\n" << flush; 
#endif
    delete[] dup;
    dup = 0;
    if(crypt_error == AES_ERROR_BAD_SECRET) {
      ProgramError->Message("The password you entered is not correct \
\nCannot open database");   
    }
    else { 
      ProgramError->Message("Error decrypting password hash \
\nCannot open database");   
    }
    is_ok = 0;
    password_input->Clear();
    pass_buf.Clear();
    return 0;
  }

  delete[] dup;
  dup = 0;
  progcfg->global_dbparms.passwd = pass_buf;
  is_ok = 1;
  password_input->Clear();
  pass_buf.Clear();

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

OpenDatabasePanel *InitOpenDatabasePanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=350; int height=165;
  int button_ypos = 100;

  OpenDatabasePanel *panel = new OpenDatabasePanel(parent,
						   OPENDATABASE_PANEL_ID,
						   (char *)"Open Database",
						   xpos, ypos, width, height,
				   wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
						   (char*)"Open Database");
  
  if(!panel) return 0;

  panel->password_label = new wxStaticText(panel, -1,
					   "Password",
					   wxPoint(11, 15));
  
  panel->password_input = new wxTextCtrl(panel, ID_OPENDATABASE_TEXTCONTROL1,
					 "",
					 wxPoint(11, 39),
					 wxSize(250,25),
					 wxTE_PROCESS_ENTER|wxTE_PASSWORD);
  
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
