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

Add user panel
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

BEGIN_EVENT_TABLE(AdduserPanel, wxDialog)
  EVT_CLOSE(AdduserPanel::OnCloseWindow)
  EVT_BUTTON (ID_ADDUSER_OK, AdduserPanel::OnOK)
  EVT_BUTTON (ID_ADDUSER_CANCEL, AdduserPanel::OnCancel)
  EVT_BUTTON (ID_ADDUSER_SC_CERT_BROWSE, AdduserPanel::OnCertFileBrowse)
  EVT_BUTTON (ID_ADDUSER_RSA_KEY_BROWSE, AdduserPanel::OnRSAKeyFileBrowse)
  EVT_TEXT_ENTER(ID_ADDUSER_TEXTCONTROL_RSA_USERNAME, AdduserPanel::OnTextControlRSAKeyFileEnter)
  EVT_TEXT_ENTER(ID_ADDUSER_TEXTCONTROL_RSA_KEY, AdduserPanel::OnTextControlRSAUsernameEnter)
  EVT_TEXT_ENTER(ID_ADDUSER_TEXTCONTROL_SC_USERNAME, AdduserPanel::OnTextControlSCUsernameEnter)
  EVT_TEXT_ENTER(ID_ADDUSER_TEXTCONTROL_SC_CERT, AdduserPanel::OnTextControlSCCertFileEnter)
END_EVENT_TABLE()

AdduserPanel::AdduserPanel(wxWindow *parent, wxWindowID id,
				   char *title,
				   int xpos, int ypos, int width, int height,
				   long style, char* name) : 
  wxDialog(parent, id, title, wxPoint(xpos, ypos), wxSize(width, height),
	   style, name)
  
{
  rsa_key_box = 0;
  rsa_username_label = 0;
  rsa_username_input = 0;
  rsa_key_label = 0;
  rsa_key_input = 0;
  rsa_browse = 0;
  sc_box = 0;
  sc_username_label = 0;
  sc_username_input = 0;
  sc_cert_label = 0;
  sc_cert_input = 0;
  sc_browse = 0;
  ok_btn = 0;
  cancel_btn = 0;
  
  is_ok = 0;
  use_rsa_key = 0;
  use_smartcard = 0;
  use_cert_file = 1;
  memset(public_key, 0, sizeof(public_key));
}
  
AdduserPanel::~AdduserPanel()
{
  if(rsa_key_box) delete rsa_key_box;
  if(rsa_username_label) delete rsa_username_label;
  if(rsa_username_input) delete rsa_username_input;
  if(rsa_key_label) delete rsa_key_label;
  if(rsa_key_input) delete rsa_key_input;
  if(rsa_browse) delete rsa_browse;
  if(sc_box) delete sc_box;
  if(sc_username_label) delete sc_username_label;
  if(sc_username_input) delete sc_username_input;
  if(sc_cert_label) delete sc_cert_label;
  if(sc_cert_input) delete sc_cert_input;
  if(sc_browse) delete sc_browse;
  if(ok_btn) delete ok_btn;
  if(cancel_btn) delete cancel_btn;

  memset(public_key, 0, sizeof(public_key));
}

void AdduserPanel::ShowPanel()
{
  is_ok = 0;
  ShowModal();
}

void AdduserPanel::OnRSAKeyFileBrowse(wxCommandEvent &event)
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

void AdduserPanel::OnCertFileBrowse(wxCommandEvent &event)
{
  wxFileDialog dialog(this, "Open smart card exported cert file:",
		      progcfg->docDir.c_str(), "",
		      "*.pem",
		      wxFD_OPEN|wxFD_FILE_MUST_EXIST);
  
  if(dialog.ShowModal() == wxID_OK) {
    sc_cert_input->Clear();
    sc_cert_input->AppendText(dialog.GetPath());
  }
}

void AdduserPanel::OnTextControlRSAKeyFileEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void AdduserPanel::OnTextControlRSAUsernameEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void AdduserPanel::OnTextControlSCUsernameEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void AdduserPanel::OnTextControlSCCertFileEnter(wxCommandEvent &event)
{
  if(!TestInput()) return;  
  Show(FALSE);
}

int AdduserPanel::TestInput()
{
  return 1;
}

void AdduserPanel::OnOK(wxCommandEvent &WXUNUSED(event))
{
  if(!TestInput()) return;  
  Show(FALSE);
}

void AdduserPanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  is_ok = 0;
  Show(FALSE);
}

void AdduserPanel::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  if(!TestInput()) return;  
  Show(FALSE);
}

AdduserPanel *InitAdduserPanel(wxWindow *parent)
{
  int xpos=50; int ypos=50; int width=450; int height=580;
  int button_ypos = 480;

  AdduserPanel *panel = new AdduserPanel(parent, ADDUSER_PANEL_ID,
					 (char *)"Add User For Database Access",
					 xpos, ypos, width, height,
					 wxRAISED_BORDER|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU,
					 (char *)"Add User For Database Access");
  if(!panel) return 0;

  panel->rsa_key_box = new wxStaticBox(panel, -1, "Add Users Public RSA Key", wxPoint(9, 13), wxSize(370,200));
  panel->rsa_username_label = new wxStaticText(panel, -1, "RSA Key Username", wxPoint(15, 43));
  panel->rsa_username_input = new wxTextCtrl(panel, ID_ADDUSER_TEXTCONTROL_RSA_USERNAME, 
					     "", wxPoint(15, 65), wxSize(250, 25));
  panel->rsa_key_label = new wxStaticText(panel, -1, "Public Key File Name", wxPoint(15, 101));
  panel->rsa_key_input = new wxTextCtrl(panel, ID_ADDUSER_TEXTCONTROL_RSA_KEY, 
					"", wxPoint(15, 127), wxSize(350, 25));
  panel->rsa_browse = new wxButton(panel, ID_ADDUSER_RSA_KEY_BROWSE, "Browse",
				   wxPoint(15, 173), wxSize(75, 25));

  panel->sc_box = new wxStaticBox(panel, -1, "Add Users Smart Card Cert", wxPoint(9, 245), wxSize(370,195));
  panel->sc_username_label = new wxStaticText(panel, -1, "Smart Card Username", wxPoint(15, 271));
  panel->sc_username_input = new wxTextCtrl(panel, ID_ADDUSER_TEXTCONTROL_SC_USERNAME,
					    "", wxPoint(15, 297), wxSize(250,25));
  panel->sc_cert_label = new wxStaticText(panel, -1, "Exported Smart Card Cert File", wxPoint(15, 333));
  panel->sc_cert_input = new wxTextCtrl(panel, ID_ADDUSER_TEXTCONTROL_SC_CERT, 
					"", wxPoint(15, 360), wxSize(350, 25));
  panel->sc_browse =  new wxButton(panel, ID_ADDUSER_SC_CERT_BROWSE, "Browse", wxPoint(15, 401), wxSize(75, 25));

  
  panel->ok_btn = new wxButton(panel, ID_ADDUSER_OK, "OK",
			       wxPoint(17, button_ypos),
			       wxSize(46, 25));
  
  panel->cancel_btn = new wxButton(panel, ID_ADDUSER_CANCEL, "Cancel",
				   wxPoint(83, button_ypos),
				   wxSize(56, 25));

  panel->Show(FALSE);
  
  return panel;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
