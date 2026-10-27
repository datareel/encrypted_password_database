// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 10/27/2026
// Date Last Modified: 10/27/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- //
// ---------- Include File Description and Details  ---------- //
// ----------------------------------------------------------- //
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
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

10/27/2026: Unlock Database dialog, see unlock_dlg.h. The password, key
file, RSA key and smart card code is the code of the Open Database
panel (open_panel.cpp, removed) with the same checks and PIN
safeguards: the PIN is sent at most once, never empty or cut short,
not at all after a failed card login in this run, and only after a
Yes to the warning when the card is low on tries or has one try left.
*/
// ----------------------------------------------------------- //
#include "app_defs.h"

enum {
  ID_UNLOCK_FILE = 6201,
  ID_UNLOCK_BROWSE_DB,
  ID_UNLOCK_METHOD,
  ID_UNLOCK_BROWSE_KEY,
  ID_UNLOCK_BROWSE_RSA,
  ID_UNLOCK_BROWSE_MODULE,
  ID_UNLOCK_PICKER
};

static const char *epdb_method_names[] = { "password", "key-file", "rsa-key", "smartcard" };

wxBEGIN_EVENT_TABLE(UnlockDialog, wxDialog)
  EVT_BUTTON(wxID_OK, UnlockDialog::OnOK)
  EVT_BUTTON(wxID_CANCEL, UnlockDialog::OnCancel)
  EVT_CLOSE(UnlockDialog::OnClose)
  EVT_BUTTON(ID_UNLOCK_BROWSE_DB, UnlockDialog::OnBrowseDatabase)
  EVT_BUTTON(ID_UNLOCK_BROWSE_KEY, UnlockDialog::OnBrowseKey)
  EVT_BUTTON(ID_UNLOCK_BROWSE_RSA, UnlockDialog::OnBrowseRSAKey)
  EVT_BUTTON(ID_UNLOCK_BROWSE_MODULE, UnlockDialog::OnBrowseModule)
  EVT_RADIOBOX(ID_UNLOCK_METHOD, UnlockDialog::OnMethod)
  EVT_TEXT(ID_UNLOCK_FILE, UnlockDialog::OnFileChanged)
  EVT_COMBOBOX(ID_UNLOCK_FILE, UnlockDialog::OnFileChanged)
  EVT_TEXT_ENTER(wxID_ANY, UnlockDialog::OnOK)
  EVT_UPDATE_UI(wxID_OK, UnlockDialog::OnUpdateOK)
  EVT_DPI_CHANGED(UnlockDialog::OnDPIChanged)
wxEND_EVENT_TABLE()

UnlockDialog::UnlockDialog(wxWindow *parent)
  : wxDialog(parent, wxID_ANY, "Unlock Database", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  is_ok = 0;
  open_failed = 0;
  method = EPDB_UNLOCK_PASSWORD;
  for(int i = 0; i < 4; i++) pages[i] = 0;
  SetIcons(EPDBAppIcons());

  const int pad = FromDIP(10);
  const int gap = FromDIP(6);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);

  // The database file
  top->Add(EPDBHeading(this, "Database"), 0, wxLEFT | wxRIGHT | wxTOP, pad);
  wxBoxSizer *frow = new wxBoxSizer(wxHORIZONTAL);
  file_input = new wxComboBox(this, ID_UNLOCK_FILE, "", wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_DROPDOWN);
  file_input->SetMinSize(wxSize(FromDIP(380), -1));
  file_input->SetToolTip("The database file, or choose a recent one");
  frow->Add(file_input, 1, wxALIGN_CENTER_VERTICAL);
  frow->Add(new wxButton(this, ID_UNLOCK_BROWSE_DB, "Browse..."), 0, wxLEFT | wxALIGN_CENTER_VERTICAL, gap);
  top->Add(frow, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

  // One unlock method at a time
  wxString methods[4] = { "Password", "Key file", "Private key", "Smart card" };
#ifdef __ENABLE_SMART_CARD__
  int num_methods = 4;
#else
  int num_methods = 3;
#endif
  method_box = new wxRadioBox(this, ID_UNLOCK_METHOD, "Unlock with", wxDefaultPosition, wxDefaultSize,
			      num_methods, methods, num_methods, wxRA_SPECIFY_COLS);
  top->Add(method_box, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

  // One page per method, only the chosen one is shown so the dialog is
  // only as tall as that page
  wxBoxSizer *pages_sizer = new wxBoxSizer(wxVERTICAL);
  wxWindow *book = this;

  // Password page
  wxPanel *p = new wxPanel(book);
  wxBoxSizer *ps = new wxBoxSizer(wxVERTICAL);
  ps->Add(new wxStaticText(p, wxID_ANY, "Database password"), 0, wxBOTTOM, FromDIP(3));
  password_input = new PasswordEntry(p, wxID_ANY);
  ps->Add(password_input, 0, wxEXPAND);
  p->SetSizer(ps);
  pages[EPDB_UNLOCK_PASSWORD] = p;

  // Key file page
  p = new wxPanel(book);
  ps = new wxBoxSizer(wxVERTICAL);
  ps->Add(new wxStaticText(p, wxID_ANY, "Symmetric key file"), 0, wxBOTTOM, FromDIP(3));
  wxButton *b;
  ps->Add(EPDBFileRow(p, key_input, b, ID_UNLOCK_BROWSE_KEY), 0, wxEXPAND);
  ps->Add(EPDBNote(p, "The key file must be readable only by you.", 0), 0, wxTOP, FromDIP(4));
  p->SetSizer(ps);
  pages[EPDB_UNLOCK_KEY_FILE] = p;

  // Private RSA key page
  p = new wxPanel(book);
  wxFlexGridSizer *g = new wxFlexGridSizer(2, FromDIP(6), FromDIP(8));
  g->AddGrowableCol(1, 1);
  g->Add(new wxStaticText(p, wxID_ANY, "Username"), 0, wxALIGN_CENTER_VERTICAL);
  rsa_username_input = new wxTextCtrl(p, wxID_ANY, progcfg->USERNAME.c_str());
  g->Add(rsa_username_input, 1, wxEXPAND);
  g->Add(new wxStaticText(p, wxID_ANY, "Private key file"), 0, wxALIGN_CENTER_VERTICAL);
  g->Add(EPDBFileRow(p, rsa_key_input, b, ID_UNLOCK_BROWSE_RSA), 1, wxEXPAND);
  g->Add(new wxStaticText(p, wxID_ANY, "Key passphrase"), 0, wxALIGN_CENTER_VERTICAL);
  rsa_passphrase_input = new PasswordEntry(p, wxID_ANY);
  g->Add(rsa_passphrase_input, 1, wxEXPAND);
  g->AddSpacer(0);
  g->Add(EPDBNote(p, "Leave the passphrase empty if the key has none."), 0);
  p->SetSizer(g);
  pages[EPDB_UNLOCK_RSA_KEY] = p;

#ifdef __ENABLE_SMART_CARD__
  // Smart card page
  p = new wxPanel(book);
  ps = new wxBoxSizer(wxVERTICAL);
  ps->Add(new wxStaticText(p, wxID_ANY, "Smart card or token"), 0, wxBOTTOM, FromDIP(3));
  sc_picker = new SmartCardPicker(p, ID_UNLOCK_PICKER);
  ps->Add(sc_picker, 1, wxEXPAND);
  sc_pin_label = new wxStaticText(p, wxID_ANY, "PIN");
  ps->Add(sc_pin_label, 0, wxTOP, FromDIP(8));
  sc_pin_input = new PasswordEntry(p, wxID_ANY, 0);
  sc_pin_input->SetMaxLength(SC_max_pin_size - 1);
  ps->Add(sc_pin_input, 0, wxEXPAND | wxTOP, FromDIP(3));
  sc_hint = EPDBNote(p, "The PIN is sent to the card once. After a wrong PIN no more PINs are sent "
		     "until the program is restarted, so the card is not locked.", 460);
  ps->Add(sc_hint, 0, wxTOP, FromDIP(4));
  sc_advanced = new wxCollapsiblePane(p, wxID_ANY, "Advanced", wxDefaultPosition, wxDefaultSize,
				      wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE);
  wxWindow *ap = sc_advanced->GetPane();
  wxFlexGridSizer *ag = new wxFlexGridSizer(2, FromDIP(6), FromDIP(8));
  ag->AddGrowableCol(1, 1);
  ag->Add(new wxStaticText(ap, wxID_ANY, "Username"), 0, wxALIGN_CENTER_VERTICAL);
  sc_username_input = new wxTextCtrl(ap, wxID_ANY, progcfg->USERNAME.c_str());
  ag->Add(sc_username_input, 1, wxEXPAND);
  ag->Add(new wxStaticText(ap, wxID_ANY, "Cert ID"), 0, wxALIGN_CENTER_VERTICAL);
  sc_keyid_input = new wxTextCtrl(ap, wxID_ANY, progcfg->SC_cert_id.c_str());
  sc_keyid_input->SetToolTip("Used for a card that shows no certificate before login. "
			     "The cert ID of the chosen certificate is used otherwise.");
  ag->Add(sc_keyid_input, 1, wxEXPAND);
  ag->Add(new wxStaticText(ap, wxID_ANY, "PKCS#11 module"), 0, wxALIGN_CENTER_VERTICAL);
  ag->Add(EPDBFileRow(ap, sc_module_input, b, ID_UNLOCK_BROWSE_MODULE, progcfg->SC_modulePath.c_str()), 1, wxEXPAND);
  ap->SetSizer(ag);
  ps->Add(sc_advanced, 0, wxEXPAND | wxTOP, FromDIP(4));
  p->SetSizer(ps);
  pages[EPDB_UNLOCK_SMART_CARD] = p;
  sc_picker->Bind(EPDB_EVT_CARD_CHANGED, &UnlockDialog::OnCardChanged, this);
  sc_advanced->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent &) { Refit(); });
#endif

  for(int i = 0; i < 4; i++) {
    if(!pages[i]) continue;
    pages_sizer->Add(pages[i], 1, wxEXPAND);
    pages[i]->Hide();
  }
  top->Add(pages_sizer, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

  message = new wxStaticText(this, wxID_ANY, "");
  message->SetForegroundColour(EPDBWarningColour());
  top->Add(message, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

  wxStdDialogButtonSizer *btns = new wxStdDialogButtonSizer();
  wxButton *ok = new wxButton(this, wxID_OK, "Unlock");
  ok->SetDefault();
  btns->AddButton(ok);
  btns->AddButton(new wxButton(this, wxID_CANCEL));
  btns->Realize();
  top->Add(btns, 0, wxEXPAND | wxALL, pad);
  SetEscapeId(wxID_CANCEL);

  SetSizer(top);
  SelectMethod(EPDB_UNLOCK_PASSWORD);
}

UnlockDialog::~UnlockDialog()
{
  ClearSecrets();
}

void UnlockDialog::ClearSecrets()
{
  password_input->Clear();
  rsa_passphrase_input->Clear();
#ifdef __ENABLE_SMART_CARD__
  sc_pin_input->Clear();
#endif
}

void UnlockDialog::Refit()
// Fits the dialog to the shown page, keeping a width the user chose
{
  int w = GetSize().x;
  SetMinSize(wxDefaultSize);
  Layout();
  wxSize best = GetSizer()->ComputeFittingWindowSize(this);
  SetMinSize(best);
  SetSize(wxSize(wxMax(best.x, w), best.y));
}

void UnlockDialog::OnDPIChanged(wxDPIChangedEvent &event)
{
  event.Skip();
  CallAfter([this]() { Refit(); });
}

int UnlockDialog::ShowPanel(gxString &fname, const wxString &start_file)
{
  is_ok = 0;
  ClearSecrets();
  message->SetLabel("");
  file_input->Clear();
  std::vector<EPDBRecentFile> recent;
  EPDBRecentLoad(recent);
  for(size_t i = 0; i < recent.size(); i++) file_input->Append(recent[i].path);
  if(!start_file.IsEmpty()) file_input->SetValue(start_file);
  else if(!recent.empty()) file_input->SetValue(recent[0].path);
  LoadFileSettings();
  CentreOnParent();
  int rv = ShowModal();
#ifdef __ENABLE_SMART_CARD__
  sc_picker->StopPolling();
#endif
  ClearSecrets();
  if(rv == wxID_OK && is_ok) {
    fname = curr_fname;
    return 1;
  }
  is_ok = 0;
  return 0;
}

void UnlockDialog::LoadFileSettings()
// Selects the method last used for this file and fills in its names
{
  wxString path = file_input->GetValue();
  EPDBRecentFile r;
  int m = method;
  if(!path.IsEmpty() && EPDBRecentFind(path, r)) {
    for(int i = 0; i < 4; i++) if(r.method == epdb_method_names[i]) m = i;
    if(m == EPDB_UNLOCK_KEY_FILE && !r.key_file.IsEmpty()) key_input->ChangeValue(r.key_file);
    if(m == EPDB_UNLOCK_RSA_KEY) {
      if(!r.key_file.IsEmpty()) rsa_key_input->ChangeValue(r.key_file);
      if(!r.username.IsEmpty()) rsa_username_input->ChangeValue(r.username);
    }
#ifdef __ENABLE_SMART_CARD__
    if(m == EPDB_UNLOCK_SMART_CARD) {
      if(!r.username.IsEmpty()) sc_username_input->ChangeValue(r.username);
      if(!r.cert_id.IsEmpty()) sc_keyid_input->ChangeValue(r.cert_id);
    }
#endif
  }
#ifndef __ENABLE_SMART_CARD__
  if(m == EPDB_UNLOCK_SMART_CARD) m = EPDB_UNLOCK_PASSWORD;
#endif
  SelectMethod(m);
}

void UnlockDialog::SelectMethod(int m)
{
  method = m;
  method_box->SetSelection(m);
  for(int i = 0; i < 4; i++) if(pages[i]) pages[i]->Show(i == m);
#ifdef __ENABLE_SMART_CARD__
  if(m == EPDB_UNLOCK_SMART_CARD) {
    // Hints for choosing the card: the card users of this database and
    // the card used the last time, see EPDBChooseCard()
    EPDBCardHints hints;
    wxString path = file_input->GetValue();
    EPDBRecentFile r;
    int have_recent = EPDBRecentFind(path, r) && r.method == "smartcard";
    hints.saved_slot = have_recent ? std::string(r.slot.utf8_str()) : std::string(progcfg->SC_slot.c_str());
    hints.saved_cert_id = have_recent && !r.cert_id.IsEmpty() ? std::string(r.cert_id.utf8_str()) :
      std::string(progcfg->SC_cert_id.c_str());
    hints.username = std::string(sc_username_input->GetValue().utf8_str());
    if(!path.IsEmpty() && wxFileExists(path) && EPDBReadCardUsers(path.utf8_str(), hints.users) == 0) {
      hints.have_users = 1;
      // One card user: use that name
      if(hints.users.size() == 1 && !have_recent) sc_username_input->ChangeValue(wxString::FromUTF8(hints.users[0].username.c_str()));
    }
    sc_picker->SetModulePath(sc_module_input->GetValue());
    sc_picker->SetHints(hints);
    sc_picker->Rescan(1);
    sc_picker->StartPolling();
    wxCommandEvent e;
    OnCardChanged(e);
    if(SC_login_blocked()) {
      message->SetLabel("A smart card login failed earlier. No more PINs are sent until the program is restarted.");
    }
  }
  else {
    sc_picker->StopPolling();
  }
#endif
  Refit();
  if(m == EPDB_UNLOCK_PASSWORD) password_input->SetFocus();
  else if(m == EPDB_UNLOCK_KEY_FILE) key_input->SetFocus();
  else if(m == EPDB_UNLOCK_RSA_KEY) {
    // 10/27/2026: The first empty field, the passphrase when the names
    // are remembered, so typing does not replace the selected key path
    if(rsa_username_input->GetValue().IsEmpty()) rsa_username_input->SetFocus();
    else if(rsa_key_input->GetValue().IsEmpty()) rsa_key_input->SetFocus();
    else rsa_passphrase_input->SetFocus();
  }
#ifdef __ENABLE_SMART_CARD__
  else sc_pin_input->SetFocus();
#endif
}

void UnlockDialog::OnMethod(wxCommandEvent &WXUNUSED(event))
{
  // Leave nothing typed for another method behind
  ClearSecrets();
  message->SetLabel("");
  SelectMethod(method_box->GetSelection());
}

void UnlockDialog::OnFileChanged(wxCommandEvent &WXUNUSED(event))
{
  if(!IsShown()) return;
  wxString path = file_input->GetValue();
  EPDBRecentFile r;
  if(EPDBRecentFind(path, r)) LoadFileSettings();
#ifdef __ENABLE_SMART_CARD__
  else if(method == EPDB_UNLOCK_SMART_CARD) SelectMethod(method);
#endif
}

void UnlockDialog::OnCardChanged(wxCommandEvent &WXUNUSED(event))
{
#ifdef __ENABLE_SMART_CARD__
  if(sc_picker->HasChoice()) sc_pin_label->SetLabel("PIN for " + sc_picker->ChoiceName());
  else sc_pin_label->SetLabel("PIN (choose the card first)");
  sc_pin_input->Enable(sc_picker->HasChoice() && !SC_login_blocked());
#endif
}

void UnlockDialog::OnUpdateOK(wxUpdateUIEvent &event)
{
  int ok = !file_input->GetValue().IsEmpty();
#ifdef __ENABLE_SMART_CARD__
  if(method == EPDB_UNLOCK_SMART_CARD) ok = ok && sc_picker->HasChoice() && !SC_login_blocked();
#endif
  event.Enable(ok != 0);
}

void UnlockDialog::OnBrowseDatabase(wxCommandEvent &WXUNUSED(event))
{
  gxDatabaseParms def_parms;
  wxString wild = wxString("Encrypted databases (*") + def_parms.data_file_extension.c_str() + ")|*" +
    def_parms.data_file_extension.c_str() + "|All files|*";
  wxString dir = progcfg->docDir.c_str();
  if(!file_input->GetValue().IsEmpty()) dir = wxFileName(file_input->GetValue()).GetPath();
  wxFileDialog dialog(this, "Open an existing database", dir, "", wild, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) {
    file_input->SetValue(dialog.GetPath());
    LoadFileSettings();
  }
}

void UnlockDialog::OnBrowseKey(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog dialog(this, "Open symmetric encryption key file", progcfg->docDir.c_str(), "",
		      "Key files (*.key)|*.key|All files|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) key_input->SetValue(dialog.GetPath());
}

void UnlockDialog::OnBrowseRSAKey(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog dialog(this, "Open private RSA key file", progcfg->docDir.c_str(), "",
		      "PEM files (*.pem)|*.pem|All files|*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) rsa_key_input->SetValue(dialog.GetPath());
}

void UnlockDialog::OnBrowseModule(wxCommandEvent &WXUNUSED(event))
{
#ifdef __ENABLE_SMART_CARD__
#ifdef __WXMSW__
  wxString wild = "PKCS#11 modules (*.dll)|*.dll|All files|*";
#else
  wxString wild = "PKCS#11 modules (*.so)|*.so|All files|*";
#endif
  wxFileDialog dialog(this, "Smart card PKCS#11 module", wxFileName(sc_module_input->GetValue()).GetPath(), "",
		      wild, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(dialog.ShowModal() == wxID_OK) {
    sc_module_input->SetValue(dialog.GetPath());
    sc_picker->SetModulePath(dialog.GetPath());
    sc_picker->Rescan(1);
  }
#endif
}

void UnlockDialog::OnOK(wxCommandEvent &WXUNUSED(event))
{
  if(!TestInput()) return;
  EndModal(wxID_OK);
}

void UnlockDialog::OnCancel(wxCommandEvent &WXUNUSED(event))
{
  is_ok = 0;
  ClearSecrets();
  EndModal(wxID_CANCEL);
}

void UnlockDialog::OnClose(wxCloseEvent &WXUNUSED(event))
// The window X cancels, it never tries to unlock
{
  is_ok = 0;
  ClearSecrets();
  EndModal(wxID_CANCEL);
}

int UnlockDialog::TestInput()
// 10/27/2026: Self-destruct and the access log around the open. With
// self-destruct on, a destroyed or read-only database is refused before
// any credential is tried, and each failed open is counted in the file.
{
  message->SetLabel("");
  is_ok = 0;
  gxDatabaseParms def_parms;
  wxString path = file_input->GetValue();
  path.Trim().Trim(false);
  if(path.IsEmpty()) {
    message->SetLabel("Choose the database file");
    return 0;
  }
  curr_fname = (const char *)path.utf8_str();
  curr_fname = ReplaceFileExtension(curr_fname, def_parms.data_file_extension.c_str()); // 10/25/2026
  if(!futils_exists(curr_fname.c_str())) {
    message->SetLabel("The database file does not exist");
    return 0;
  }
  if(frame->DatabaseIsOpen(curr_fname)) {
    message->SetLabel("The database is already open");
    return 0;
  }

  open_failed = 0;
  if(!GUISelfDestructPrecheck(curr_fname.c_str())) {
    ClearSecrets();
    return 0;
  }
  int rv = 0;
  switch(method) {
    case EPDB_UNLOCK_PASSWORD: rv = PasswordOpenDatabase(); break;
    case EPDB_UNLOCK_KEY_FILE: rv = AESKeyOpenDatabase(); break;
    case EPDB_UNLOCK_RSA_KEY: rv = RSAOpenDatabase(); break;
#ifdef __ENABLE_SMART_CARD__
    case EPDB_UNLOCK_SMART_CARD: rv = SmartCardOpenDatabase(); break;
#endif
  }
  if(open_failed) GUIFailedOpen(curr_fname.c_str(), open_failed);
  else if(rv && is_ok) GUIGoodOpen(curr_fname.c_str(), progcfg->global_dbparms.crypt_key);
  open_failed = 0;
  if(rv && is_ok) Remember();
  return rv && is_ok;
}

void UnlockDialog::Remember()
{
  EPDBRecentFile r;
  r.path = wxString::FromUTF8(curr_fname.c_str());
  r.method = epdb_method_names[method];
  if(method == EPDB_UNLOCK_KEY_FILE) r.key_file = key_input->GetValue();
  if(method == EPDB_UNLOCK_RSA_KEY) {
    r.key_file = rsa_key_input->GetValue();
    r.username = rsa_username_input->GetValue();
  }
#ifdef __ENABLE_SMART_CARD__
  if(method == EPDB_UNLOCK_SMART_CARD) {
    r.username = sc_username_input->GetValue();
    r.slot = progcfg->SC_slot.c_str();
    r.cert_id = progcfg->SC_cert_id.c_str();
  }
#endif
  EPDBRecentRemember(r);
}

int UnlockDialog::PasswordOpenDatabase()
// 10/25/2026: Wipe the password copy and clear the input on every path
{
  gxString pass_buf = (const char *)password_input->GetValue().utf8_str();
  password_input->Clear();
  gxString sbuf;
  if(pass_buf.length() < 8) {
    SecureWipe(pass_buf);
    message->SetLabel("The password should be at least 8 characters long");
    return 0;
  }

  gxDatabase f;
  gxDatabaseError rv = f.Open(curr_fname.c_str(), gxDBASE_READONLY);
  if(rv != gxDBASE_NO_ERROR) {
    SecureWipe(pass_buf);
    sbuf << clear << "Error opening database" << "\n" << gxDatabaseExceptionMessage(rv);
    ProgramError->Message(sbuf.c_str());
    return 0;
  }
  gxDatabaseConfig dbconfig;
  if(!dbconfig.ReadConfig(&f)) {
    SecureWipe(pass_buf);
    ProgramError->Message("Error reading database config");
    f.Close();
    return 0;
  }
  f.Close();

  // 10/25/2026: Check the password. For a v2 database it is turned into
  // the database keyset.
  MemoryBuffer secret, keyset;
  secret.Cat(pass_buf.c_str(), pass_buf.length());
  SecureWipe(pass_buf);
  gxString uerr;
  int aes_error = 0;
  int urv = 0;
  if(f.Open(curr_fname.c_str(), gxDBASE_READONLY) != gxDBASE_NO_ERROR) {
    uerr << clear << "Error opening database";
    urv = -1;
  }
  else {
    urv = EPDBUnlock(&f, secret, keyset, uerr, aes_error);
    f.Close();
  }
  secret.Clear(1);
  gui_last_auth.auth = "password";
  gui_last_auth.slot.Clear();
  if(urv != 0) {
    if(aes_error) open_failed = "The password did not open the database"; // 10/27/2026
    message->SetLabel(aes_error ? wxString("The password did not open the database") :
		      wxString::FromUTF8(uerr.c_str()));
    if(!aes_error) {
      sbuf << clear << "Error decrypting database hash with password provided" << "\n" << uerr.c_str();
      ProgramError->Message(sbuf.c_str());
    }
    return 0;
  }
  DBStringConfig::crypt_key = keyset;
  keyset.Clear(1);
  progcfg->global_dbparms.crypt_key = DBStringConfig::crypt_key;
  unlock_description = "Unlocked with password";
  is_ok = 1;
  return 1;
}

int UnlockDialog::AESKeyOpenDatabase()
{
  wxString kf = key_input->GetValue();
  if(kf.IsEmpty() || !futils_exists(kf.utf8_str()) || !futils_isfile(kf.utf8_str())) {
    message->SetLabel("The key file does not exist or cannot be read");
    return 0;
  }

  gxString sbuf, ebuf;
  MemoryBuffer key;
  // 10/25/2026: Refuse a key file other users can read
  if(CheckPrivateKeyFile(kf.utf8_str(), ebuf) != 0) {
    ProgramError->Message(ebuf.c_str());
    return 0;
  }
  if(read_key_file(kf.utf8_str(), key, ebuf) != 0) {
    ProgramError->Message(ebuf.c_str());
    return 0;
  }

  gxDatabase f;
  gxDatabaseError rv = f.Open(curr_fname.c_str(), gxDBASE_READONLY);
  if(rv != gxDBASE_NO_ERROR) {
    key.Clear(1);
    sbuf << clear << "Error opening database" << "\n" << gxDatabaseExceptionMessage(rv);
    ProgramError->Message(sbuf.c_str());
    return 0;
  }
  gxDatabaseConfig dbconfig;
  if(!dbconfig.ReadConfig(&f)) {
    key.Clear(1);
    ProgramError->Message("Error reading database config");
    f.Close();
    return 0;
  }
  f.Close();

  // 10/25/2026: Check the key. For a v2 database it is turned into the
  // database keyset.
  MemoryBuffer keyset;
  gxString uerr;
  int aes_error = 0;
  int urv = 0;
  if(f.Open(curr_fname.c_str(), gxDBASE_READONLY) != gxDBASE_NO_ERROR) {
    uerr << clear << "Error opening database";
    urv = -1;
  }
  else {
    urv = EPDBUnlock(&f, key, keyset, uerr, aes_error);
    f.Close();
  }
  key.Clear(1);
  gui_last_auth.auth = "key-file";
  gui_last_auth.slot.Clear();
  if(urv != 0) {
    if(aes_error) open_failed = "The key file did not open the database"; // 10/27/2026
    message->SetLabel(aes_error ? wxString("The key file did not open the database") :
		      wxString::FromUTF8(uerr.c_str()));
    if(!aes_error) {
      sbuf << clear << "Error decrypting database hash with key provided" << "\n" << uerr.c_str();
      ProgramError->Message(sbuf.c_str());
    }
    return 0;
  }
  DBStringConfig::crypt_key = keyset;
  progcfg->global_dbparms.crypt_key = keyset;
  keyset.Clear(1);
  unlock_description = "Unlocked with key file";
  is_ok = 1;
  return 1;
}

int UnlockDialog::RSAOpenDatabase()
// 10/25/2026: Wipe the private key and passphrase on every return path
{
  char private_key[RSA_max_keybuf_len];
  gxString pbuf;
  int rv = RSAOpenDatabase(private_key, pbuf);
  OPENSSL_cleanse(private_key, sizeof(private_key));
  SecureWipe(pbuf);
  rsa_passphrase_input->Clear();
  return rv;
}

int UnlockDialog::RSAOpenDatabase(char private_key[RSA_max_keybuf_len], gxString &pbuf)
{
  int rv;
  unsigned private_key_len = 0;
  int has_passphrase = 0;
  char *passphrase = 0;
  gxString rsa_key_username;
  gxString sbuf;
  wxString kf = rsa_key_input->GetValue();

  if(kf.IsEmpty() || !futils_exists(kf.utf8_str()) || !futils_isfile(kf.utf8_str())) {
    message->SetLabel("The private RSA key file does not exist or cannot be read");
    return 0;
  }

  // 10/25/2026: Refuse a private key file other users can read
  if(CheckPrivateKeyFile(kf.utf8_str(), sbuf) != 0) {
    ProgramError->Message(sbuf.c_str());
    return 0;
  }
  rv = RSA_read_key_file(kf.utf8_str(), private_key, RSA_max_keybuf_len, &private_key_len, &has_passphrase);
  if(rv != RSA_NO_ERROR) {
    sbuf << clear << "Error reading private key file " << RSA_err_string(rv);
    ProgramError->Message(sbuf.c_str());
    return 0;
  }

  if(rsa_username_input->GetValue().IsEmpty()) {
    message->SetLabel("Enter the username of the private RSA key");
    return 0;
  }
  rsa_key_username = (const char *)rsa_username_input->GetValue().utf8_str();

  if(has_passphrase) {
    if(rsa_passphrase_input->GetValue().IsEmpty()) {
      message->SetLabel("This key has a passphrase, enter it");
      return 0;
    }
    pbuf = (const char *)rsa_passphrase_input->GetValue().utf8_str();
    passphrase = (char *)pbuf.GetSPtr();
  }

  gxDatabase f;
  gxDatabaseError dberr = f.Open(curr_fname.c_str(), gxDBASE_READONLY);
  if(dberr != gxDBASE_NO_ERROR) {
    sbuf << clear << "Error opening database" << "\n" << gxDatabaseExceptionMessage(dberr);
    ProgramError->Message(sbuf.c_str());
    return 0;
  }

  DatabaseUserAuth db_auth;
  db_auth.f = &f; // Set the file pointer

  rv = db_auth.DecryptWithRSAKey(private_key, private_key_len, rsa_key_username, passphrase);
  gui_last_auth.auth = "rsa-key";
  gui_last_auth.slot = rsa_key_username;
  if(rv != 0) {
    open_failed = "The RSA key did not open the database"; // 10/27/2026
    ProgramError->Message(db_auth.err.c_str());
    return 0;
  }
  f.Close();

  progcfg->global_dbparms.crypt_key = DBStringConfig::crypt_key;
  unlock_description = wxString("Unlocked with private key of ") + wxString::FromUTF8(rsa_key_username.c_str());
  is_ok = 1;
  return 1;
}

#ifdef __ENABLE_SMART_CARD__
int UnlockDialog::SmartCardOpenDatabase()
{
  int rv;
  gxString sbuf;
  SmartCardOB sc;
  gxString smartcard_username;

  // 10/27/2026: The card is chosen in the picker. A locked card or a key
  // that is not RSA cannot be chosen.
  SC_TokenInfo token;
  SC_CertInfo cert;
  if(sc_picker->GetChoice(token, cert) != 0) {
    sc_pin_input->Clear();
    message->SetLabel("Choose the smart card");
    return 0;
  }
  if(SC_login_blocked()) {
    sc_pin_input->Clear();
    message->SetLabel("A smart card login failed earlier. No more PINs are sent until the program is restarted.");
    return 0;
  }
  gxString cert_id = cert.id_hex.empty() ? gxString((const char *)sc_keyid_input->GetValue().utf8_str()) :
    gxString(cert.id_hex.c_str());
  cert_id.TrimLeading(' '); cert_id.TrimTrailing(' ');
  if(cert_id.is_null()) {
    sc_pin_input->Clear();
    message->SetLabel("Enter the cert ID under Advanced, the card shows no certificate");
    return 0;
  }
  if(cert_id.length() >= SC_max_cert_id_size) {
    sc_pin_input->Clear();
    message->SetLabel("The smart card cert ID is too long");
    return 0;
  }
  if(sc_username_input->GetValue().IsEmpty()) {
    sc_pin_input->Clear();
    message->SetLabel("Enter the smart card username under Advanced");
    return 0;
  }
  wxString module = sc_module_input->GetValue();
  if(module.length() >= SC_max_path_size) {
    sc_pin_input->Clear();
    message->SetLabel("The PKCS#11 module path is too long");
    return 0;
  }

  smartcard_username = (const char *)sc_username_input->GetValue().utf8_str();
  sc.SetEnginePath(progcfg->SC_enginePath.c_str());
  sc.SetModulePath((char *)(const char *)module.utf8_str());
  sc.SetEngineID(progcfg->SC_engine_ID.c_str());
  // 10/26/2026: Use only the card in this PKCS#11 slot
  sc.SetSlot(token.slot_id);
  gxString slot_str = (const char *)EPDBSlotString(token.slot_id).utf8_str();
  sc.SetCertID(cert_id.c_str());

  sbuf = (const char *)sc_pin_input->GetValue().utf8_str();
  sc_pin_input->Clear();
  // 10/26/2026: Never send an empty or cut short PIN, it uses up a card
  // PIN try. The PIN is not kept after this function returns.
  if(sbuf.is_null() || sbuf.length() >= SC_max_pin_size) {
    sbuf.Clear(1);
    message->SetLabel("The smart card PIN is empty or too long, the PIN was not sent to the card");
    return 0;
  }
  sc.SetPin(sbuf.c_str());
  sbuf.Clear(1);

  gxDatabase f;
  gxDatabaseError dberr = f.Open(curr_fname.c_str(), gxDBASE_READONLY);
  if(dberr != gxDBASE_NO_ERROR) {
    OPENSSL_cleanse(sc.pin, sizeof(sc.pin));
    sbuf << clear << "Error opening database" << "\n" << gxDatabaseExceptionMessage(dberr);
    ProgramError->Message(sbuf.c_str());
    return 0;
  }

  DatabaseUserAuth db_auth;
  db_auth.f = &f; // Set the file pointer

  int sc_was_blocked = SC_login_blocked(); // 10/27/2026: For the failed open count
  rv = db_auth.DecryptWithSmartcard(&sc, smartcard_username);
  if(rv != 0 && (sc.error_level == SC_ERROR_PIN_COUNT_LOW || sc.error_level == SC_ERROR_PIN_FINAL_TRY)) {
    // 10/26/2026: The PIN was not sent. Ask first, the default is Cancel.
    gxString warn;
    if(sc.error_level == SC_ERROR_PIN_FINAL_TRY) {
      warn << clear << "The smart card reports ONE PIN try is left. A wrong PIN will LOCK the card.\n\n"
	   << "Check the PIN tries left (for example in ActivClient, open the card and view its smart card info) and be sure of the PIN.\n\n"
	   << "Send the PIN to the card?";
    }
    else {
      warn << clear << "The smart card reports it is low on PIN tries (a wrong PIN was entered before).\n\n"
	   << "Check the PIN tries left (for example in ActivClient, open the card and view its smart card info) and be sure of the PIN.\n\n"
	   << "Send the PIN to the card?";
    }
    wxMessageDialog dialog(this, warn.c_str(), "Smart Card PIN Warning",
			   wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
    dialog.SetYesNoLabels("Send the PIN", wxID_CANCEL);
    if(dialog.ShowModal() == wxID_YES) {
      sc.allow_pin_count_low = 1;
      if(sc.error_level == SC_ERROR_PIN_FINAL_TRY) sc.allow_pin_final_try = 1;
      rv = db_auth.DecryptWithSmartcard(&sc, smartcard_username);
    }
    else {
      OPENSSL_cleanse(sc.pin, sizeof(sc.pin));
      message->SetLabel("The smart card PIN was not sent");
      return 0;
    }
  }
  OPENSSL_cleanse(sc.pin, sizeof(sc.pin));
  gui_last_auth.auth = "smartcard";
  gui_last_auth.slot = smartcard_username;
  if(rv != 0) {
    // 10/27/2026: A wrong PIN, or a card key that does not open the
    // database, is a failed open for self-destruct. A PIN that was not
    // sent is not.
    if((!sc_was_blocked && SC_login_blocked()) || sc.error_level == SC_ERROR_PIN_INCORRECT ||
       db_auth.err.Find("does not hold") != -1 || db_auth.err.Find("authentication failed") != -1) {
      open_failed = "The smart card did not open the database";
    }
    gxString msg;
    msg << clear << db_auth.err.c_str();
    if(SC_login_blocked()) {
      msg << "\n\nNo more smart card logins will be made until the program is restarted.";
      sc_pin_input->Enable(false);
    }
    ProgramError->Message(msg.c_str());
    wxCommandEvent e;
    OnCardChanged(e);
    return 0;
  }

  f.Close();

  sbuf = sc.cert_id;
  if(progcfg->SC_cert_id != sbuf) { // Update the global SC cert ID
    progcfg->SC_cert_id = sc.cert_id;
    gxConfig CfgData(progcfg->cfgFile.c_str());
    CfgData.ChangeConfigValue("SC_cert_id", (const char *)sc.cert_id);
  }
  if(progcfg->SC_slot != slot_str) { // 10/26/2026: And the slot
    progcfg->SC_slot = slot_str;
    gxConfig CfgData(progcfg->cfgFile.c_str());
    CfgData.ChangeConfigValue("SC_slot", slot_str.c_str());
  }
  if(module != wxString(progcfg->SC_modulePath.c_str()) && wxFileExists(module)) {
    progcfg->SC_modulePath = (const char *)module.utf8_str();
    gxConfig CfgData(progcfg->cfgFile.c_str());
    CfgData.ChangeConfigValue("SC_modulePath", progcfg->SC_modulePath.c_str());
  }

  progcfg->global_dbparms.crypt_key = DBStringConfig::crypt_key;
  unlock_description = "Unlocked with smart card " + sc_picker->ChoiceName();
  is_ok = 1;
  return 1;
}
#endif // __ENABLE_SMART_CARD__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
