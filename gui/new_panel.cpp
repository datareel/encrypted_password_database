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

Create Database dialog, see new_panel.h. 10/27/2026: Rewritten with
sizers. The password rules (CheckPassword) are the same as before.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

enum {
  ID_NEWDB_BROWSE_FOLDER = 731,
  ID_NEWDB_BROWSE_KEY,
  ID_NEWDB_BROWSE_RSA,
  ID_NEWDB_GENERATE
};

BEGIN_EVENT_TABLE(NewDatabasePanel, wxDialog)
  EVT_CLOSE(NewDatabasePanel::OnCloseWindow)
  EVT_BUTTON(wxID_OK, NewDatabasePanel::OnOK)
  EVT_BUTTON(wxID_CANCEL, NewDatabasePanel::OnCancel)
  EVT_BUTTON(ID_NEWDB_BROWSE_FOLDER, NewDatabasePanel::OnBrowseFolder)
  EVT_BUTTON(ID_NEWDB_BROWSE_KEY, NewDatabasePanel::OnKeyFileBrowse)
  EVT_BUTTON(ID_NEWDB_BROWSE_RSA, NewDatabasePanel::OnRSAKeyFileBrowse)
  EVT_BUTTON(ID_NEWDB_GENERATE, NewDatabasePanel::OnGenerate)
  EVT_TEXT(wxID_ANY, NewDatabasePanel::OnChange)
  EVT_CHECKBOX(wxID_ANY, NewDatabasePanel::OnChange)
END_EVENT_TABLE()

static wxPanel *EPDBSection(wxWindow *parent, wxSizer *top, wxCheckBox *&check, const wxString &title,
			    const char *icon, const wxString &note)
// A checkable section: an icon, a check box title and a note, then an
// indented panel for its fields
{
  wxBoxSizer *h = new wxBoxSizer(wxHORIZONTAL);
  h->Add(new wxStaticBitmap(parent, wxID_ANY, EPDBIcon(icon, 20)), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, parent->FromDIP(6));
  check = new wxCheckBox(parent, wxID_ANY, title);
  check->SetFont(check->GetFont().Bold());
  h->Add(check, 0, wxALIGN_CENTER_VERTICAL);
  if(!note.IsEmpty()) h->Add(EPDBNote(parent, note), 1, wxLEFT | wxALIGN_CENTER_VERTICAL, parent->FromDIP(8));
  top->Add(h, 0, wxEXPAND | wxTOP, parent->FromDIP(10));
  wxPanel *p = new wxPanel(parent);
  top->AddSpacer(parent->FromDIP(4));
  top->Add(p, 0, wxEXPAND | wxLEFT, parent->FromDIP(28));
  return p;
}

NewDatabasePanel::NewDatabasePanel(wxWindow *parent)
  : wxDialog(parent, NEWDATABASE_PANEL_ID, "Create Database", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  is_ok = 0;
  use_key = use_password = use_rsa_key = use_cert_file = 0;
#ifdef __ENABLE_SMART_CARD__
  use_smartcard = 0;
#endif
  memset(public_key, 0, sizeof(public_key));
  memset(random_key, 0, sizeof(random_key));
  SetIcons(EPDBAppIcons());

  const int pad = FromDIP(12);
  const int gap = FromDIP(6);
  wxBoxSizer *outer = new wxBoxSizer(wxVERTICAL);
  wxScrolledWindow *sw = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
  sw->SetScrollRate(0, FromDIP(10));
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);

  // Name and folder
  top->Add(EPDBHeading(sw, "New database"), 0);
  wxFlexGridSizer *g = new wxFlexGridSizer(2, gap, FromDIP(8));
  g->AddGrowableCol(1, 1);
  g->Add(new wxStaticText(sw, wxID_ANY, "Name"), 0, wxALIGN_CENTER_VERTICAL);
  name_input = new wxTextCtrl(sw, wxID_ANY, "");
  name_input->SetMinSize(wxSize(FromDIP(320), -1));
  g->Add(name_input, 1, wxEXPAND);
  g->Add(new wxStaticText(sw, wxID_ANY, "Folder"), 0, wxALIGN_CENTER_VERTICAL);
  wxBoxSizer *fr = new wxBoxSizer(wxHORIZONTAL);
  folder_input = new wxTextCtrl(sw, wxID_ANY, progcfg->docDir.c_str());
  fr->Add(folder_input, 1, wxALIGN_CENTER_VERTICAL);
  fr->Add(new wxButton(sw, ID_NEWDB_BROWSE_FOLDER, "Browse..."), 0, wxLEFT | wxALIGN_CENTER_VERTICAL, gap);
  g->Add(fr, 1, wxEXPAND);
  g->AddSpacer(0);
  path_label = EPDBNote(sw, "");
  g->Add(path_label, 1, wxEXPAND);
  top->Add(g, 0, wxEXPAND | wxTOP, gap);

  top->Add(new wxStaticLine(sw), 0, wxEXPAND | wxTOP, FromDIP(12));
  top->Add(EPDBHeading(sw, "Protect with"), 0, wxTOP, FromDIP(8));
  top->Add(EPDBNote(sw, "Any of the ways checked below can unlock the database.", 0), 0, wxTOP, FromDIP(2));

  // Master password
  password_panel = EPDBSection(sw, top, password_check, "Master password", "password", "");
  wxFlexGridSizer *pg = new wxFlexGridSizer(2, gap, FromDIP(8));
  pg->AddGrowableCol(1, 1);
  pg->Add(new wxStaticText(password_panel, wxID_ANY, "Password"), 0, wxALIGN_CENTER_VERTICAL);
  wxBoxSizer *pr = new wxBoxSizer(wxHORIZONTAL);
  password_input = new PasswordEntry(password_panel, wxID_ANY);
  pr->Add(password_input, 1, wxALIGN_CENTER_VERTICAL);
  wxButton *gen = new wxButton(password_panel, ID_NEWDB_GENERATE, "Generate");
  gen->SetBitmap(EPDBIcon("generate", EPDB_MENU_ICON_DIP));
  pr->Add(gen, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, gap);
  pg->Add(pr, 1, wxEXPAND);
  pg->Add(new wxStaticText(password_panel, wxID_ANY, "Confirm"), 0, wxALIGN_CENTER_VERTICAL);
  confirm_password_input = new PasswordEntry(password_panel, wxID_ANY);
  pg->Add(confirm_password_input, 1, wxEXPAND);
  pg->AddSpacer(0);
  strength = new PasswordStrength(password_panel);
  pg->Add(strength, 1, wxEXPAND);
  password_panel->SetSizer(pg);
  password_check->SetValue(true);

  // Key file, instead of a password
  key_panel = EPDBSection(sw, top, key_check, "Key file", "key_file", "");
  wxBoxSizer *ks = new wxBoxSizer(wxVERTICAL);
  wxButton *b;
  ks->Add(EPDBFileRow(key_panel, key_input, b, ID_NEWDB_BROWSE_KEY), 0, wxEXPAND);
  key_note = EPDBNote(key_panel, "");
  ks->Add(key_note, 0, wxTOP, FromDIP(3));
  key_panel->SetSizer(ks);

  // RSA public key of a user
  rsa_panel = EPDBSection(sw, top, rsa_check, "RSA public key", "rsa_key", "");
  wxFlexGridSizer *rg = new wxFlexGridSizer(2, gap, FromDIP(8));
  rg->AddGrowableCol(1, 1);
  rg->Add(new wxStaticText(rsa_panel, wxID_ANY, "Username"), 0, wxALIGN_CENTER_VERTICAL);
  rsa_username_input = new wxTextCtrl(rsa_panel, wxID_ANY, progcfg->USERNAME.c_str());
  rg->Add(rsa_username_input, 1, wxEXPAND);
  rg->Add(new wxStaticText(rsa_panel, wxID_ANY, "Public key file"), 0, wxALIGN_CENTER_VERTICAL);
  rg->Add(EPDBFileRow(rsa_panel, rsa_key_input, b, ID_NEWDB_BROWSE_RSA), 1, wxEXPAND);
  rsa_panel->SetSizer(rg);

#ifdef __ENABLE_SMART_CARD__
  // Smart card or token, only its public key is read: no PIN
  sc_panel = EPDBSection(sw, top, sc_check, "Smart card or token", "smart_card", "No PIN is needed to add a card");
  wxBoxSizer *ss = new wxBoxSizer(wxVERTICAL);
  sc_picker = new SmartCardPicker(sc_panel);
  ss->Add(sc_picker, 0, wxEXPAND);
  wxBoxSizer *su = new wxBoxSizer(wxHORIZONTAL);
  su->Add(new wxStaticText(sc_panel, wxID_ANY, "Username"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
  sc_username_input = new wxTextCtrl(sc_panel, wxID_ANY, progcfg->USERNAME.c_str());
  su->Add(sc_username_input, 1, wxALIGN_CENTER_VERTICAL);
  ss->Add(su, 0, wxEXPAND | wxTOP, gap);
  sc_panel->SetSizer(ss);
  sc_picker->Bind(EPDB_EVT_CARD_CHANGED, [this](wxCommandEvent &) { UpdateState(); });
#endif

  sw->SetSizer(top);
  outer->Add(sw, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

  // Summary and buttons
  outer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
  summary = new wxStaticText(this, wxID_ANY, "");
  outer->Add(summary, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
  message = new wxStaticText(this, wxID_ANY, "");
  message->SetForegroundColour(EPDBWarningColour());
  outer->Add(message, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, FromDIP(4));
  wxStdDialogButtonSizer *btns = new wxStdDialogButtonSizer();
  wxButton *ok = new wxButton(this, wxID_OK, "Create");
  ok->SetDefault();
  btns->AddButton(ok);
  btns->AddButton(new wxButton(this, wxID_CANCEL));
  btns->Realize();
  outer->Add(btns, 0, wxEXPAND | wxALL, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizer(outer);

  // As tall as the content, but within the screen work area
  sw->FitInside();
  wxSize best = outer->ComputeFittingWindowSize(this);
  wxSize vbest = sw->GetVirtualSize();
  wxRect area = wxDisplay(wxDisplay::GetFromWindow(parent) == wxNOT_FOUND ? 0u :
			  (unsigned)wxDisplay::GetFromWindow(parent)).GetClientArea();
  int h = wxMin(best.y + vbest.y - sw->GetClientSize().y, area.height - FromDIP(40));
  SetSize(wxSize(wxMax(best.x, FromDIP(560)), wxMax(h, FromDIP(420))));
  SetMinSize(wxSize(FromDIP(520), wxMin(FromDIP(420), area.height)));
  UpdateState();
}

NewDatabasePanel::~NewDatabasePanel()
{
  memset(public_key, 0, sizeof(public_key));
  memset(random_key, 0, sizeof(random_key));
}

gxString NewDatabasePanel::DatabaseName() const
{
  wxString n = name_input->GetValue();
  n.Trim().Trim(false);
  return gxString((const char *)n.utf8_str());
}

gxString NewDatabasePanel::DatabasePath() const
{
  wxString n = name_input->GetValue();
  n.Trim().Trim(false);
  if(n.IsEmpty()) return gxString();
  wxFileName fn(folder_input->GetValue(), n);
  return gxString((const char *)fn.GetFullPath().utf8_str());
}

void NewDatabasePanel::UpdateState()
// Enables the sections and shows the summary of the unlock methods
{
  int pw = password_check->GetValue();
  // A database has a password or a key file, not both
  key_check->Enable(!pw);
  if(pw && key_check->GetValue()) key_check->SetValue(false);
  key_note->SetLabel(pw ? "A database uses a password or a key file, not both.\nClear Master password to use a key file."
		     : "The key file must be readable only by you.");
  password_panel->Enable(pw);
  key_panel->Enable(key_check->GetValue());
  rsa_panel->Enable(rsa_check->GetValue());
#ifdef __ENABLE_SMART_CARD__
  sc_panel->Enable(sc_check->GetValue());
  if(sc_check->GetValue() && sc_picker->NumCards() == 0 && IsShown()) {
    sc_picker->Rescan(1);
  }
#endif
  if(pw) strength->Update(password_input->GetValue());

  gxString full = DatabasePath();
  gxDatabaseParms def_parms;
  if(!full.is_null()) path_label->SetLabel("Creates " + wxString::FromUTF8(ReplaceFileExtension(full, def_parms.data_file_extension.c_str()).c_str()));
  else path_label->SetLabel("");

  wxString s = "Unlock with: ";
  wxArrayString ways;
  if(pw) ways.Add("the master password");
  if(key_check->GetValue()) ways.Add("the key file");
  if(rsa_check->GetValue()) ways.Add("the private key of " + rsa_username_input->GetValue());
#ifdef __ENABLE_SMART_CARD__
  if(sc_check->GetValue()) ways.Add("smart card " + (sc_picker->HasChoice() ? sc_picker->ChoiceName() : wxString("(choose a card)")));
#endif
  if(ways.IsEmpty()) s = "Check at least one way to unlock the database.";
  else s << wxJoin(ways, ',');
  s.Replace(",", ", ");
  if(!pw && !key_check->GetValue() && !ways.IsEmpty()) {
    s << "\nNo password or key file: only these users can open it, there is no password to recover with.";
  }
  summary->SetLabel(s);
  summary->Wrap(FromDIP(520));
  Layout();
}

void NewDatabasePanel::OnChange(wxCommandEvent &event)
{
  event.Skip();
  if(IsShown()) UpdateState();
}

void NewDatabasePanel::ShowPanel()
{
  name_input->Clear();
  password_input->Clear();
  confirm_password_input->Clear();
  message->SetLabel("");
  is_ok = 0;
  UpdateState();
  name_input->SetFocus();
  CentreOnParent();
  ShowModal();
#ifdef __ENABLE_SMART_CARD__
  sc_picker->StopPolling();
#endif
  password_input->Clear();
  confirm_password_input->Clear();
}

void NewDatabasePanel::OnBrowseFolder(wxCommandEvent &WXUNUSED(event))
{
  wxDirDialog dialog(this, "Folder for the new database", folder_input->GetValue(), wxDD_DEFAULT_STYLE);
  if(dialog.ShowModal() == wxID_OK) folder_input->SetValue(dialog.GetPath());
}

void NewDatabasePanel::OnKeyFileBrowse(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog dialog(this, "Symmetric encryption key file", progcfg->docDir.c_str(), "",
		      "Key files (*.key)|*.key|All files|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) key_input->SetValue(dialog.GetPath());
}

void NewDatabasePanel::OnRSAKeyFileBrowse(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog dialog(this, "RSA public key file", progcfg->docDir.c_str(), "",
		      "PEM files (*.pem)|*.pem|All files|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) rsa_key_input->SetValue(dialog.GetPath());
}

void NewDatabasePanel::OnGenerate(wxCommandEvent &WXUNUSED(event))
{
  GeneratePasswordDialog dlg(this, password_input);
  if(dlg.ShowModal() == wxID_OK) {
    confirm_password_input->SetValue(password_input->GetValue());
    password_input->Reveal(1);
    UpdateState();
  }
}

int NewDatabasePanel::CheckPassword()
{
  gxString sbuf;
  
  if(password_input->GetValue().IsEmpty()) {
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

  gxString pass_buf = (const char *)password_input->GetValue().utf8_str();
  if(progcfg->password_length) {
    if(pass_buf.length() < (unsigned)progcfg->min_password_length) {
      password_input->Clear();
      confirm_password_input->Clear();
      SecureWipe(pass_buf);
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
      // 10/25/2026: Clear the inputs and the copy, as on the other paths
      password_input->Clear();
      confirm_password_input->Clear();
      SecureWipe(pass_buf);
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
      SecureWipe(pass_buf);
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
  SecureWipe(pass_buf);
  return 1;
}


int NewDatabasePanel::TestInput()
{
  use_key = 0;
  use_password = 0;
  use_rsa_key = 0;
  use_cert_file = 0;
#ifdef __ENABLE_SMART_CARD__
  use_smartcard = 0;
#endif // __ENABLE_SMART_CARD__
  message->SetLabel("");

  gxString sbuf;
  int rv;

  if(DatabaseName().is_null()) {
    message->SetLabel("Enter a name for the new database");
    return 0;
  }
  if(!wxDirExists(folder_input->GetValue())) {
    message->SetLabel("The folder does not exist");
    return 0;
  }
  gxDatabaseParms def_parms;
  gxString data_file = ReplaceFileExtension(DatabasePath(), def_parms.data_file_extension.c_str());
  if(futils_exists(data_file.c_str())) {
    message->SetLabel("A database with this name already exists in the folder");
    return 0;
  }

  int pw = password_check->GetValue();
  int kf = key_check->GetValue() && !pw;
  int rsa = rsa_check->GetValue();
#ifdef __ENABLE_SMART_CARD__
  int card = sc_check->GetValue();
#else
  int card = 0;
#endif
  if(!pw && !kf && !rsa && !card) {
    message->SetLabel("Check at least one way to unlock the database");
    return 0;
  }

  if(rsa) {
    use_rsa_key = 1;
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
    if(rsa_username_input->GetValue().IsEmpty()) {
      message->SetLabel("Enter the username of the public RSA key");
      return 0;
    }
    rsa_key_username = (const char *)rsa_username_input->GetValue().utf8_str();
  }

#ifdef __ENABLE_SMART_CARD__
  if(card) {
    // 10/27/2026: The card chosen in the picker; only its public key is
    // read, no PIN
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
    if(sc_username_input->GetValue().IsEmpty()) {
      message->SetLabel("Enter the smart card username");
      return 0;
    }
    use_smartcard = 1;
    smartcard_username = (const char *)sc_username_input->GetValue().utf8_str();
    sc.SetEnginePath(progcfg->SC_enginePath.c_str());
    sc.SetModulePath((char *)(const char *)sc_picker->GetModulePath().utf8_str());
    sc.SetEngineID(progcfg->SC_engine_ID.c_str());
    sc.SetSlot(token.slot_id);
    sc.SetCertID((char *)cert.id_hex.c_str());
    card_name = sc_picker->ChoiceName();
  }
#endif // __ENABLE_SMART_CARD__

  if(!pw && !kf) {
    // Use a random key if no password or symmetric key is given with RSA or a smart card
    AES_fillrand(random_key, sizeof(random_key));
    progcfg->global_dbparms.crypt_key.Clear(1);
    progcfg->global_dbparms.crypt_key.Cat(random_key, sizeof(random_key));
    memset(random_key, 0, sizeof(random_key));
  }

  if(kf) {
    use_key = 1;
    wxString f = key_input->GetValue();
    if(f.IsEmpty() || !futils_exists(f.utf8_str()) || !futils_isfile(f.utf8_str())) {
      message->SetLabel("The key file does not exist or cannot be read");
      return 0;
    }
    gxString ebuf;
    MemoryBuffer key;
    // 10/25/2026: Refuse a key file other users can read
    if(CheckPrivateKeyFile(f.utf8_str(), ebuf) != 0) {
      ProgramError->Message(ebuf.c_str());
      return 0;
    }
    if(read_key_file(f.utf8_str(), key, ebuf) != 0) {
      ProgramError->Message(ebuf.c_str());
      return 0;
    }
    progcfg->global_dbparms.crypt_key = key;
    key.Clear(1);
  }

  if(pw) {
    use_password = 1;
    if(!CheckPassword()) return 0;
  }

  is_ok = 1;
  return 1;
}

void NewDatabasePanel::OnOK(wxCommandEvent &WXUNUSED(event))
{
  if(!TestInput()) return;
  EndModal(wxID_OK);
}

void NewDatabasePanel::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  is_ok = 0;
  EndModal(wxID_CANCEL);
}

void NewDatabasePanel::OnCloseWindow(wxCloseEvent &WXUNUSED(event))
// The window X cancels, it never creates a database
{
  is_ok = 0;
  EndModal(wxID_CANCEL);
}

NewDatabasePanel *InitNewDatabasePanel(wxWindow *parent)
{
  return new NewDatabasePanel(parent);
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
