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

10/27/2026: Tools > Command Builder, see cmdbuild_dlg.h
*/
// ----------------------------------------------------------- //
#include "app_defs.h"

enum {
  ID_CMDB_COPY = 6501,
  ID_CMDB_SAVE,
  ID_CMDB_TEST
};

// When each option row is shown
enum {
  CMDB_ALWAYS = 0, CMDB_DB, CMDB_AUTH, CMDB_PW, CMDB_KEY, CMDB_RSA, CMDB_RSA_PASS, CMDB_SC, CMDB_SC_CARD,
  CMDB_RECORD, CMDB_FIELD, CMDB_RECINPUT, CMDB_RECFILE, CMDB_EXPORT, CMDB_IMPORT, CMDB_MERGE,
  CMDB_MERGE_PW, CMDB_MERGE_KEY, CMDB_NEWPW, CMDB_GEN, CMDB_USER, CMDB_USERKEY, CMDB_FROMCARD,
  CMDB_REMOVE, CMDB_REKEY, CMDB_DRYRUN
};

static const char *cmdb_fields[NumDataMembers] = {
  "title", "username", "password", "previous-password", "security-questions",
  "account-information", "website-link", "notes"
};

// ----------------------------------------------------------- //
// SecretSourcePanel
// ----------------------------------------------------------- //
SecretSourcePanel::SecretSourcePanel(wxWindow *parent, const wxString &what) : wxPanel(parent, wxID_ANY)
{
  wxBoxSizer *s = new wxBoxSizer(wxHORIZONTAL);
  source = new wxChoice(this, wxID_ANY);
  source->Append("From a file only you can read");
  source->Append("Piped from AWS Secrets Manager");
  source->Append("Piped from another command");
  source->SetSelection(0);
  source->SetToolTip("Where the " + what + " comes from. It is never put on the command line.");
  s->Add(source, 0, wxALIGN_CENTER_VERTICAL);
  value = new wxTextCtrl(this, wxID_ANY, "");
  value->SetMinSize(wxSize(FromDIP(220), -1));
  s->Add(value, 1, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(6));
  browse = new wxButton(this, wxID_ANY, "Browse...");
  s->Add(browse, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(6));
  SetSizer(s);
  source->Bind(wxEVT_CHOICE, &SecretSourcePanel::OnSource, this);
  browse->Bind(wxEVT_BUTTON, &SecretSourcePanel::OnBrowse, this);
  value->Bind(wxEVT_TEXT, [this](wxCommandEvent &e) {
    wxCommandEvent c(wxEVT_TEXT, GetId());
    c.SetEventObject(this);
    ProcessWindowEvent(c);
    e.Skip(false);
  });
  wxCommandEvent e;
  OnSource(e);
}

void SecretSourcePanel::OnSource(wxCommandEvent &WXUNUSED(event))
{
  int s = source->GetSelection();
  browse->Show(s == EPDB_SECRET_FILE);
  if(s == EPDB_SECRET_FILE) value->SetHint("File that holds it on the first line");
  else if(s == EPDB_SECRET_PIPE_AWS) value->SetHint("Secret ID, such as epdb/password");
  else value->SetHint("Command that prints it");
  Layout();
  wxCommandEvent c(wxEVT_CHOICE, GetId());
  c.SetEventObject(this);
  ProcessWindowEvent(c);
}

void SecretSourcePanel::OnBrowse(wxCommandEvent &WXUNUSED(event))
{
  wxFileDialog dialog(this, "File only you can read", progcfg->homeDir.c_str(), "", "All files|*", wxFD_OPEN);
  if(dialog.ShowModal() == wxID_OK) value->SetValue(dialog.GetPath());
}

void SecretSourcePanel::Get(EPDBCmdSecret &s) const
{
  s = EPDBCmdSecret();
  s.source = source->GetSelection();
  std::string v = (const char *)value->GetValue().utf8_str();
  if(s.source == EPDB_SECRET_FILE) s.file = v;
  else if(s.source == EPDB_SECRET_PIPE_AWS) s.aws_id = v;
  else s.command = v;
}

void SecretSourcePanel::SetFile(const wxString &file)
{
  source->SetSelection(EPDB_SECRET_FILE);
  value->ChangeValue(file);
}

// ----------------------------------------------------------- //
// CommandBuilderDialog
// ----------------------------------------------------------- //
wxWindow *CommandBuilderDialog::AddRow(const wxString &label, wxWindow *ctrl, int group)
{
  wxStaticText *l = new wxStaticText(opts, wxID_ANY, label);
  grid->Add(l, 0, wxALIGN_CENTER_VERTICAL);
  grid->Add(ctrl, 1, wxEXPAND);
  row_labels.push_back(l);
  row_ctrls.push_back(ctrl);
  row_groups.push_back(group);
  return ctrl;
}

wxWindow *CommandBuilderDialog::FileRow(wxTextCtrl *&text, const wxString &value, int save)
{
  wxPanel *p = new wxPanel(opts);
  wxBoxSizer *s = new wxBoxSizer(wxHORIZONTAL);
  text = new wxTextCtrl(p, wxID_ANY, value);
  text->SetMinSize(wxSize(FromDIP(260), -1));
  s->Add(text, 1, wxALIGN_CENTER_VERTICAL);
  wxButton *b = new wxButton(p, wxID_ANY, "Browse...");
  s->Add(b, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(6));
  p->SetSizer(s);
  wxTextCtrl *t = text;
  b->Bind(wxEVT_BUTTON, [this, t, save](wxCommandEvent &) {
    wxFileDialog dialog(this, "Choose a file", wxFileName(t->GetValue()).GetPath(), "", "All files|*",
			save ? wxFD_SAVE : wxFD_OPEN);
    if(dialog.ShowModal() == wxID_OK) t->SetValue(dialog.GetPath());
  });
  return p;
}

CommandBuilderDialog::CommandBuilderDialog(wxWindow *parent, CryptDBDocument *doc, int prefill_record)
  : wxDialog(parent, wxID_ANY, "Command Builder", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{
  building = 1;
  last_ok = 0;
  SetIcons(EPDBAppIcons());
  const int pad = FromDIP(10);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *head = new wxBoxSizer(wxHORIZONTAL);
  head->Add(new wxStaticBitmap(this, wxID_ANY, EPDBIcon("terminal", 32)), 0, wxRIGHT, FromDIP(10));
  wxBoxSizer *ht = new wxBoxSizer(wxVERTICAL);
  ht->Add(EPDBHeading(this, "Build an epdb_auth_tool command"), 0);
  ht->Add(EPDBNote(this, "Pick an operation and its options. The command updates as you type. Passwords, PINs "
		   "and passphrases are never put on the command line: they are read from a file only you "
		   "can read, or piped in.", 560), 0, wxTOP, FromDIP(2));
  head->Add(ht, 1);
  top->Add(head, 0, wxEXPAND | wxALL, pad);

  wxBoxSizer *mid = new wxBoxSizer(wxHORIZONTAL);
  opts = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
  opts->SetScrollRate(0, FromDIP(10));
  grid = new wxFlexGridSizer(2, FromDIP(6), FromDIP(8));
  grid->AddGrowableCol(1, 1);

  operation = new wxChoice(opts, wxID_ANY);
  for(int i = 0; i < EPDB_OP_COUNT; i++) operation->Append(EPDBCmdOperationName(i));
  operation->SetSelection(EPDB_OP_DB_INFO);
  AddRow("Operation", operation, CMDB_ALWAYS);
  AddRow("Database", FileRow(database, "", 0), CMDB_DB);
  tool = new wxTextCtrl(opts, wxID_ANY, ToolPath());
  AddRow("epdb_auth_tool", tool, CMDB_ALWAYS);

  auth = new wxChoice(opts, wxID_ANY);
  auth->Append("Password");
  auth->Append("Key file");
  auth->Append("Private RSA key");
#ifdef __ENABLE_SMART_CARD__
  auth->Append("Smart card");
#endif
  auth->SetSelection(0);
  AddRow("Unlock with", auth, CMDB_AUTH);
  password = new SecretSourcePanel(opts, "database password");
  AddRow("Password", password, CMDB_PW);
  AddRow("Key file", FileRow(key_file, "", 0), CMDB_KEY);
  AddRow("Private key", FileRow(rsa_key, "", 0), CMDB_RSA);
  rsa_user = new wxTextCtrl(opts, wxID_ANY, progcfg->USERNAME.c_str());
  AddRow("Key username", rsa_user, CMDB_RSA);
  rsa_has_pass = new wxCheckBox(opts, wxID_ANY, "The private key has a passphrase");
  AddRow("", rsa_has_pass, CMDB_RSA);
  rsa_pass = new SecretSourcePanel(opts, "private key passphrase");
  AddRow("Passphrase", rsa_pass, CMDB_RSA_PASS);
  sc_user = new wxTextCtrl(opts, wxID_ANY, progcfg->USERNAME.c_str());
  AddRow("Card username", sc_user, CMDB_SC);
  sc_cert = new wxTextCtrl(opts, wxID_ANY, progcfg->SC_cert_id.c_str());
  AddRow("Cert ID", sc_cert, CMDB_SC_CARD);
  sc_slot = new wxTextCtrl(opts, wxID_ANY, progcfg->SC_slot.c_str());
  sc_slot->SetHint("Empty for any");
  AddRow("Slot", sc_slot, CMDB_SC_CARD);
  sc_provider = new wxTextCtrl(opts, wxID_ANY, progcfg->SC_modulePath.c_str());
  AddRow("PKCS#11 module", sc_provider, CMDB_SC_CARD);
  sc_pin = new SecretSourcePanel(opts, "smart card PIN");
  AddRow("PIN", sc_pin, CMDB_SC);

  record = new wxTextCtrl(opts, wxID_ANY, "");
  AddRow("Record title", record, CMDB_RECORD);
  field = new wxChoice(opts, wxID_ANY);
  for(int i = 0; i < NumDataMembers; i++) field->Append(cmdb_fields[i]);
  field->SetSelection(2);
  AddRow("Field", field, CMDB_FIELD);
  record_source = new wxChoice(opts, wxID_ANY);
  record_source->Append("A file only you can read (--record-file)");
  record_source->Append("stdin");
  record_source->SetSelection(0);
  AddRow("Record values", record_source, CMDB_RECINPUT);
  AddRow("Values file", FileRow(record_file, "", 0), CMDB_RECFILE);
  export_format = new wxChoice(opts, wxID_ANY);
  export_format->Append("csv");
  export_format->Append("json");
  export_format->SetSelection(0);
  AddRow("Format", export_format, CMDB_EXPORT);
  AddRow("Output file", FileRow(output_file, "", 1), CMDB_EXPORT);
  input_format = new wxChoice(opts, wxID_ANY);
  input_format->Append("detect");
  input_format->Append("text");
  input_format->Append("json");
  input_format->Append("csv");
  input_format->SetSelection(0);
  AddRow("Input format", input_format, CMDB_IMPORT);
  AddRow("Other database", FileRow(merge_db, "", 0), CMDB_MERGE);
  merge_auth = new wxChoice(opts, wxID_ANY);
  merge_auth->Append("Password");
  merge_auth->Append("Key file");
  merge_auth->SetSelection(0);
  AddRow("Other unlock", merge_auth, CMDB_MERGE);
  merge_pass = new SecretSourcePanel(opts, "password of the other database");
  AddRow("Other password", merge_pass, CMDB_MERGE_PW);
  AddRow("Other key file", FileRow(merge_key, "", 0), CMDB_MERGE_KEY);
  merge_policy = new wxChoice(opts, wxID_ANY);
  merge_policy->Append("fail");
  merge_policy->Append("skip");
  merge_policy->Append("replace");
  merge_policy->SetSelection(0);
  AddRow("In both", merge_policy, CMDB_MERGE);
  new_pass = new SecretSourcePanel(opts, "new database password");
  AddRow("New password", new_pass, CMDB_NEWPW);
  gen_length = new wxSpinCtrl(opts, wxID_ANY, "20", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 8, 256, 20);
  AddRow("Length", gen_length, CMDB_GEN);
  gen_symbols = new wxCheckBox(opts, wxID_ANY, "Use punctuation and symbols");
  AddRow("", gen_symbols, CMDB_GEN);
  user_name = new wxTextCtrl(opts, wxID_ANY, "");
  AddRow("Username", user_name, CMDB_USER);
  user_from_card = new wxCheckBox(opts, wxID_ANY, "Read the cert from the card in the reader (no PIN)");
  user_from_card->SetValue(true);
  AddRow("", user_from_card, CMDB_FROMCARD);
  AddRow("Key or cert file", FileRow(user_key, "", 0), CMDB_USERKEY);
  remove_type = new wxChoice(opts, wxID_ANY);
  remove_type->Append("Any");
  remove_type->Append("rsa");
  remove_type->Append("smartcard");
  remove_type->SetSelection(0);
  AddRow("Access to remove", remove_type, CMDB_REMOVE);
  AddRow("Rekey input file", FileRow(rekey_file, "", 0), CMDB_REKEY);

  wxPanel *gp = new wxPanel(opts);
  wxWrapSizer *gs = new wxWrapSizer(wxHORIZONTAL);
  non_interactive = new wxCheckBox(gp, wxID_ANY, "--non-interactive");
  non_interactive->SetValue(true);
  json = new wxCheckBox(gp, wxID_ANY, "--json");
  quiet = new wxCheckBox(gp, wxID_ANY, "--quiet");
  dry_run = new wxCheckBox(gp, wxID_ANY, "--dry-run");
  gs->Add(non_interactive, 0, wxRIGHT | wxBOTTOM, FromDIP(10));
  gs->Add(json, 0, wxRIGHT | wxBOTTOM, FromDIP(10));
  gs->Add(quiet, 0, wxRIGHT | wxBOTTOM, FromDIP(10));
  gs->Add(dry_run, 0, wxRIGHT | wxBOTTOM, FromDIP(10));
  gp->SetSizer(gs);
  AddRow("Options", gp, CMDB_ALWAYS);
  lock_timeout = new wxSpinCtrl(opts, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 3600, 0);
  lock_timeout->SetToolTip("Seconds to wait for another writer, 0 for the default");
  AddRow("Lock timeout", lock_timeout, CMDB_DB);
  opts->SetSizer(grid);
  mid->Add(opts, 1, wxEXPAND | wxRIGHT, pad);

  // The command for each shell
  wxBoxSizer *right = new wxBoxSizer(wxVERTICAL);
  shells = new wxNotebook(this, wxID_ANY);
  const char *names[3] = { "Bash (Linux)", "PowerShell (Windows)", "cmd.exe (Windows)" };
  for(int i = 0; i < 3; i++) {
    out[i] = new wxTextCtrl(shells, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
			    wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP);
    out[i]->SetFont(EPDBMonoFont());
    shells->AddPage(out[i], names[i]);
  }
#ifdef __WXMSW__
  shells->SetSelection(1);
#endif
  shells->SetMinSize(FromDIP(wxSize(520, 240)));
  right->Add(shells, 1, wxEXPAND);
  error = new wxStaticText(this, wxID_ANY, "");
  error->SetForegroundColour(EPDBWarningColour());
  right->Add(error, 0, wxEXPAND | wxTOP, FromDIP(4));
  notes = EPDBNote(this, "");
  right->Add(notes, 0, wxEXPAND | wxTOP, FromDIP(4));
  right->Add(EPDBHeading(this, "Exit codes", 0), 0, wxTOP, FromDIP(8));
  wxStaticText *codes = new wxStaticText(this, wxID_ANY, EPDBCmdExitCodes());
  codes->SetFont(EPDBMonoFont());
  right->Add(codes, 0, wxTOP, FromDIP(2));
  mid->Add(right, 1, wxEXPAND);
  top->Add(mid, 1, wxEXPAND | wxLEFT | wxRIGHT, pad);

  wxBoxSizer *b = new wxBoxSizer(wxHORIZONTAL);
  wxButton *copy = new wxButton(this, ID_CMDB_COPY, "Copy");
  copy->SetBitmap(EPDBIcon("copy", EPDB_MENU_ICON_DIP));
  b->Add(copy, 0);
  b->Add(new wxButton(this, ID_CMDB_SAVE, "Save as Script..."), 0, wxLEFT, FromDIP(6));
  wxButton *test = new wxButton(this, ID_CMDB_TEST, "Test with --dry-run");
  test->SetToolTip("Run the command with --dry-run on a copy of the database. Not for smart cards or piped secrets.");
  b->Add(test, 0, wxLEFT, FromDIP(6));
  b->AddStretchSpacer();
  b->Add(new wxButton(this, wxID_CANCEL, "Close"), 0);
  top->Add(b, 0, wxEXPAND | wxALL, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizer(top);

  Prefill(doc, prefill_record);
  Bind(wxEVT_TEXT, &CommandBuilderDialog::OnChange, this);
  Bind(wxEVT_CHOICE, &CommandBuilderDialog::OnChange, this);
  Bind(wxEVT_CHECKBOX, &CommandBuilderDialog::OnChange, this);
  Bind(wxEVT_SPINCTRL, [this](wxSpinEvent &) { Rebuild(); });
  Bind(wxEVT_BUTTON, &CommandBuilderDialog::OnCopy, this, ID_CMDB_COPY);
  Bind(wxEVT_BUTTON, &CommandBuilderDialog::OnSave, this, ID_CMDB_SAVE);
  Bind(wxEVT_BUTTON, &CommandBuilderDialog::OnTest, this, ID_CMDB_TEST);
  Bind(wxEVT_UPDATE_UI, &CommandBuilderDialog::OnUpdateTest, this, ID_CMDB_TEST);
  building = 0;
  ShowRows();
  Rebuild();

  // Fit the screen work area
  wxRect area = wxDisplay(wxDisplay::GetFromWindow(parent) == wxNOT_FOUND ? 0u :
			  (unsigned)wxDisplay::GetFromWindow(parent)).GetClientArea();
  wxSize best = GetBestSize();
  // 10/27/2026: On a narrow screen the command goes below the options
  // instead of beside them
  if(best.x > area.width) {
    mid->SetOrientation(wxVERTICAL);
    shells->SetMinSize(wxSize(-1, FromDIP(140)));
    opts->SetMinSize(wxSize(-1, FromDIP(160)));
    Layout();
    best = GetBestSize();
  }
  SetSize(wxSize(wxMin(wxMax(best.x, FromDIP(1100)), area.width), wxMin(wxMax(best.y, FromDIP(640)), area.height)));
  SetMinSize(wxSize(wxMin(FromDIP(600), area.width), wxMin(FromDIP(400), area.height)));
  CentreOnParent();
}

wxString CommandBuilderDialog::ToolPath() const
// The epdb_auth_tool next to this program
{
  wxFileName fn(wxStandardPaths::Get().GetExecutablePath());
#ifdef __WXMSW__
  fn.SetFullName("epdb_auth_tool.exe");
#else
  fn.SetFullName("epdb_auth_tool");
#endif
  return fn.GetFullPath();
}

void CommandBuilderDialog::Prefill(CryptDBDocument *doc, int prefill_record)
// The open database, and the unlock method used to open it
{
  if(!doc || !doc->GridFrame()) return;
  database->ChangeValue(wxString::FromUTF8(doc->DBParms()->data_file.c_str()));
  if(gui_last_auth.auth == "key-file") auth->SetSelection(EPDB_AUTH_KEY_FILE);
  else if(gui_last_auth.auth == "rsa-key") {
    auth->SetSelection(EPDB_AUTH_RSA_KEY);
    rsa_user->ChangeValue(wxString::FromUTF8(gui_last_auth.slot.c_str()));
  }
#ifdef __ENABLE_SMART_CARD__
  else if(gui_last_auth.auth == "smartcard") {
    auth->SetSelection(EPDB_AUTH_SMART_CARD);
    sc_user->ChangeValue(wxString::FromUTF8(gui_last_auth.slot.c_str()));
  }
#endif
  EPDBRecentFile r;
  if(EPDBRecentFind(database->GetValue(), r)) {
    if(r.method == "key-file") key_file->ChangeValue(r.key_file);
    if(r.method == "rsa-key") rsa_key->ChangeValue(r.key_file);
  }
  if(prefill_record) {
    int row = doc->CurrentRecordRow();
    if(row >= 0) {
      record->ChangeValue(doc->GetGrid()->GetCellValue(row, 0));
      operation->SetSelection(EPDB_OP_GET_FIELD);
      field->SetSelection(2);
    }
  }
}

void CommandBuilderDialog::ShowRows()
{
  int op = operation->GetSelection();
  int a = auth->GetSelection();
  int needs_auth = EPDBCmdNeedsAuth(op);
  int recinput = (op == EPDB_OP_ADD_RECORD || op == EPDB_OP_UPDATE_RECORD || op == EPDB_OP_IMPORT);
  for(size_t i = 0; i < row_groups.size(); i++) {
    int show = 1;
    switch(row_groups[i]) {
      case CMDB_DB: show = (op != EPDB_OP_GENERATE_PASSWORD); break;
      case CMDB_AUTH: show = needs_auth; break;
      case CMDB_PW: show = needs_auth && a == EPDB_AUTH_PASSWORD; break;
      case CMDB_KEY: show = needs_auth && a == EPDB_AUTH_KEY_FILE; break;
      case CMDB_RSA: show = needs_auth && a == EPDB_AUTH_RSA_KEY; break;
      case CMDB_RSA_PASS: show = needs_auth && a == EPDB_AUTH_RSA_KEY && rsa_has_pass->GetValue(); break;
      case CMDB_SC: show = needs_auth && a == EPDB_AUTH_SMART_CARD; break;
      case CMDB_SC_CARD: show = (needs_auth && a == EPDB_AUTH_SMART_CARD) ||
			   (op == EPDB_OP_ADD_SMARTCARD_USER && user_from_card->GetValue()); break;
      case CMDB_RECORD: show = (op <= EPDB_OP_GET_FIELD); break;
      case CMDB_FIELD: show = (op == EPDB_OP_GET_FIELD); break;
      case CMDB_RECINPUT: show = recinput; break;
      case CMDB_RECFILE: show = recinput && record_source->GetSelection() == 0; break;
      case CMDB_EXPORT: show = (op == EPDB_OP_EXPORT); break;
      case CMDB_IMPORT: show = (op == EPDB_OP_IMPORT); break;
      case CMDB_MERGE: show = (op == EPDB_OP_MERGE_FROM); break;
      case CMDB_MERGE_PW: show = (op == EPDB_OP_MERGE_FROM && merge_auth->GetSelection() == 0); break;
      case CMDB_MERGE_KEY: show = (op == EPDB_OP_MERGE_FROM && merge_auth->GetSelection() == 1); break;
      case CMDB_NEWPW: show = (op == EPDB_OP_CHANGE_PASSWORD); break;
      case CMDB_GEN: show = (op == EPDB_OP_GENERATE_PASSWORD); break;
      case CMDB_USER: show = (op == EPDB_OP_ADD_RSA_USER || op == EPDB_OP_ADD_SMARTCARD_USER || op == EPDB_OP_REMOVE_USER); break;
      case CMDB_USERKEY: show = (op == EPDB_OP_ADD_RSA_USER) || (op == EPDB_OP_ADD_SMARTCARD_USER && !user_from_card->GetValue()); break;
      case CMDB_FROMCARD: show = (op == EPDB_OP_ADD_SMARTCARD_USER); break;
      case CMDB_REMOVE: show = (op == EPDB_OP_REMOVE_USER); break;
      case CMDB_REKEY: show = (op == EPDB_OP_REKEY); break;
    }
    row_labels[i]->Show(show != 0);
    row_ctrls[i]->Show(show != 0);
  }
  dry_run->Enable(EPDBCmdSupportsDryRun(op) != 0);
  opts->Layout();
  opts->FitInside();
}

void CommandBuilderDialog::Collect(EPDBCmdOptions &o)
{
  o = EPDBCmdOptions();
  o.tool = (const char *)tool->GetValue().utf8_str();
  o.database = (const char *)database->GetValue().utf8_str();
  o.operation = operation->GetSelection();
  o.auth = auth->GetSelection();
  password->Get(o.password);
  o.key_file = (const char *)key_file->GetValue().utf8_str();
  o.rsa_key_file = (const char *)rsa_key->GetValue().utf8_str();
  o.rsa_username = (const char *)rsa_user->GetValue().utf8_str();
  o.rsa_has_passphrase = rsa_has_pass->GetValue() ? 1 : 0;
  rsa_pass->Get(o.rsa_passphrase);
  o.sc_username = (const char *)sc_user->GetValue().utf8_str();
  o.sc_cert_id = (const char *)sc_cert->GetValue().utf8_str();
  o.sc_slot = (const char *)sc_slot->GetValue().utf8_str();
  o.sc_provider = (const char *)sc_provider->GetValue().utf8_str();
  sc_pin->Get(o.sc_pin);
  o.record_name = (const char *)record->GetValue().utf8_str();
  o.field_name = cmdb_fields[field->GetSelection() < 0 ? 2 : field->GetSelection()];
  o.record_from_file = (record_source->GetSelection() == 0);
  o.record_file = (const char *)record_file->GetValue().utf8_str();
  o.export_format = (const char *)export_format->GetStringSelection().utf8_str();
  o.output_file = (const char *)output_file->GetValue().utf8_str();
  o.input_format = input_format->GetSelection() > 0 ? std::string(input_format->GetStringSelection().utf8_str()) : std::string();
  o.merge_database = (const char *)merge_db->GetValue().utf8_str();
  o.merge_auth = merge_auth->GetSelection() == 1 ? EPDB_AUTH_KEY_FILE : EPDB_AUTH_PASSWORD;
  merge_pass->Get(o.merge_password);
  o.merge_key_file = (const char *)merge_key->GetValue().utf8_str();
  o.merge_policy = (const char *)merge_policy->GetStringSelection().utf8_str();
  new_pass->Get(o.new_password);
  o.generate_length = gen_length->GetValue();
  o.generate_symbols = gen_symbols->GetValue() ? 1 : 0;
  o.user_name = (const char *)user_name->GetValue().utf8_str();
  o.user_key_file = (const char *)user_key->GetValue().utf8_str();
  o.user_from_card = user_from_card->GetValue() ? 1 : 0;
  o.remove_user_type = remove_type->GetSelection() > 0 ? std::string(remove_type->GetStringSelection().utf8_str()) : std::string();
  o.rekey_file = (const char *)rekey_file->GetValue().utf8_str();
  o.non_interactive = non_interactive->GetValue() ? 1 : 0;
  o.json = json->GetValue() ? 1 : 0;
  o.quiet = quiet->GetValue() ? 1 : 0;
  o.dry_run = dry_run->GetValue() ? 1 : 0;
  o.lock_timeout = lock_timeout->GetValue();
}

void CommandBuilderDialog::Rebuild()
{
  if(building) return;
  EPDBCmdOptions o;
  Collect(o);
  last_ok = 1;
  wxString err_text;
  for(int sh = 0; sh < 3; sh++) {
    std::string err;
    if(EPDBBuildCommand(o, sh, last[sh], err) != 0) {
      last_ok = 0;
      out[sh]->ChangeValue(wxString::FromUTF8(((sh == EPDB_SHELL_CMD ? "REM " : "# ") +
						std::string("Complete the options: ") + err + "\n").c_str()));
      if(err_text.IsEmpty()) err_text = wxString::FromUTF8(err.c_str());
      continue;
    }
    out[sh]->ChangeValue(wxString::FromUTF8(EPDBCommandScript(last[sh], sh).c_str()));
  }
  error->SetLabel(err_text);
  wxString n;
  if(last_ok) {
    for(size_t i = 0; i < last[0].notes.size(); i++) n << wxString::FromUTF8(last[0].notes[i].c_str()) << "\n";
  }
  notes->SetLabel(n);
  notes->Wrap(FromDIP(520));
  Layout();
}

void CommandBuilderDialog::OnChange(wxCommandEvent &event)
{
  event.Skip();
  if(building) return;
  ShowRows();
  Rebuild();
}

void CommandBuilderDialog::OnCopy(wxCommandEvent &WXUNUSED(event))
{
  int sh = shells->GetSelection();
  if(!last_ok) return;
  // The command line only, ready to paste; it holds no secret
  if(wxTheClipboard->Open()) {
    wxTheClipboard->SetData(new wxTextDataObject(wxString::FromUTF8(last[sh].command.c_str())));
    wxTheClipboard->Close();
  }
}

void CommandBuilderDialog::OnSave(wxCommandEvent &WXUNUSED(event))
{
  int sh = shells->GetSelection();
  if(!last_ok) return;
  const char *ext[3] = { "sh", "ps1", "bat" };
  wxString wild = wxString::Format("Script (*.%s)|*.%s|All files|*", ext[sh], ext[sh]);
  wxFileDialog dialog(this, "Save the script", progcfg->homeDir.c_str(), wxString::Format("epdb_command.%s", ext[sh]),
		      wild, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if(dialog.ShowModal() != wxID_OK) return;
  std::string text = EPDBCommandScript(last[sh], sh);
  if(sh != EPDB_SHELL_BASH) {
    // Windows line endings for PowerShell and batch files
    std::string crlf;
    for(size_t i = 0; i < text.size(); i++) {
      if(text[i] == '\n') crlf += "\r\n";
      else crlf += text[i];
    }
    text = crlf;
    // A UTF-8 BOM, so Windows PowerShell 5.1 reads the script as UTF-8
    if(sh == EPDB_SHELL_POWERSHELL) text = "\xef\xbb\xbf" + text;
  }
  gxString err;
  if(WritePrivateFile(dialog.GetPath().utf8_str(), text.data(), (unsigned)text.size(), err) != 0) {
    ProgramError->Message(err.c_str());
    return;
  }
#if defined (__UNIX__)
  chmod(dialog.GetPath().utf8_str(), 0700); // Only the user can read and run it
#endif
}

void CommandBuilderDialog::OnUpdateTest(wxUpdateUIEvent &event)
// Only operations with --dry-run, never with a smart card PIN or a
// secret from a pipe, and only with the record values in a file
{
  EPDBCmdOptions o;
  Collect(o);
  int ok = last_ok && EPDBCmdSupportsDryRun(o.operation) && o.auth != EPDB_AUTH_SMART_CARD &&
    o.password.source == EPDB_SECRET_FILE && o.merge_password.source == EPDB_SECRET_FILE &&
    (!o.rsa_has_passphrase || o.rsa_passphrase.source == EPDB_SECRET_FILE);
  if(o.operation == EPDB_OP_ADD_RECORD || o.operation == EPDB_OP_UPDATE_RECORD || o.operation == EPDB_OP_IMPORT) {
    ok = ok && o.record_from_file;
  }
  if(o.operation == EPDB_OP_REKEY) ok = ok && !o.rekey_file.empty();
  event.Enable(ok != 0);
}

void CommandBuilderDialog::OnTest(wxCommandEvent &WXUNUSED(event))
{
  EPDBCmdOptions o;
  Collect(o);
  if(o.auth == EPDB_AUTH_SMART_CARD) return; // Never test with a card
  o.dry_run = 1;
  o.non_interactive = 1;
  EPDBCmdResult r;
  std::string err;
  if(EPDBBuildCommand(o, EPDB_SHELL_BASH, r, err) != 0) {
    ProgramError->Message(err.c_str());
    return;
  }
  // Run the program directly with the arguments, not through a shell
  std::vector<wxString> argv_s;
  for(size_t i = 0; i < r.args.size(); i++) argv_s.push_back(wxString::FromUTF8(r.args[i].c_str()));
  std::vector<const wchar_t *> argv;
  for(size_t i = 0; i < argv_s.size(); i++) argv.push_back(argv_s[i].wc_str());
  argv.push_back(0);
  wxProcess proc;
  proc.Redirect();
  long rc;
  {
    wxBusyCursor busy;
    rc = wxExecute(&argv[0], wxEXEC_SYNC | wxEXEC_HIDE_CONSOLE, &proc);
  }
  wxString text;
  if(rc == -1) {
    text = "The program could not be started: " + argv_s[0];
  }
  else {
    text = wxString::Format("Exit code %ld\n\n", rc);
    wxInputStream *streams[2] = { proc.GetInputStream(), proc.GetErrorStream() };
    for(int k = 0; k < 2; k++) {
      if(!streams[k]) continue;
      wxStringOutputStream so;
      streams[k]->Read(so);
      text << so.GetString();
    }
  }
  wxDialog d(this, wxID_ANY, "Test with --dry-run", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
  wxBoxSizer *ds = new wxBoxSizer(wxVERTICAL);
  ds->Add(EPDBNote(&d, "The command ran on a copy of the database with --dry-run, the database was not changed."),
	  0, wxALL, FromDIP(10));
  wxTextCtrl *t = new wxTextCtrl(&d, wxID_ANY, text, wxDefaultPosition, FromDIP(wxSize(600, 300)),
				 wxTE_MULTILINE | wxTE_READONLY);
  t->SetFont(EPDBMonoFont());
  ds->Add(t, 1, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(10));
  ds->Add(d.CreateStdDialogButtonSizer(wxOK), 0, wxEXPAND | wxALL, FromDIP(10));
  d.SetSizerAndFit(ds);
  d.CentreOnParent();
  d.ShowModal();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
