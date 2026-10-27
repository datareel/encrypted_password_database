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

10/27/2026: Small dialogs, see misc_dlg.h
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

enum {
  ID_GENPW_GENERATE = 6401,
  ID_GENPW_COPY,
  ID_GENPW_USE
};

GeneratePasswordDialog::GeneratePasswordDialog(wxWindow *parent, PasswordEntry *t)
  : wxDialog(parent, wxID_ANY, "Generate Password", wxDefaultPosition, wxDefaultSize,
	     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
  target = t;
  SetIcons(EPDBAppIcons());
  const int pad = FromDIP(10);
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxFlexGridSizer *g = new wxFlexGridSizer(2, FromDIP(6), FromDIP(8));
  g->AddGrowableCol(1, 1);
  g->Add(new wxStaticText(this, wxID_ANY, "Length"), 0, wxALIGN_CENTER_VERTICAL);
  length = new wxSpinCtrl(this, wxID_ANY, "20", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 8, 256, 20);
  g->Add(length, 0);
  g->AddSpacer(0);
  symbols = new wxCheckBox(this, wxID_ANY, "Use punctuation and symbols");
  g->Add(symbols, 0);
  g->Add(new wxStaticText(this, wxID_ANY, "Password"), 0, wxALIGN_CENTER_VERTICAL);
  result = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  result->SetFont(EPDBMonoFont());
  result->SetMinSize(wxSize(FromDIP(320), -1));
  g->Add(result, 1, wxEXPAND);
  g->AddSpacer(0);
  strength = new PasswordStrength(this);
  g->Add(strength, 1, wxEXPAND);
  top->Add(g, 1, wxEXPAND | wxALL, pad);
  top->Add(EPDBNote(this, "Made with the program's cryptographic random number generator, every character "
		    "kind is used at least once.", 380), 0, wxLEFT | wxRIGHT, pad);

  wxBoxSizer *b = new wxBoxSizer(wxHORIZONTAL);
  wxButton *gen = new wxButton(this, ID_GENPW_GENERATE, "Generate");
  gen->SetBitmap(EPDBIcon("generate", EPDB_MENU_ICON_DIP));
  b->Add(gen, 0);
  wxButton *copy = new wxButton(this, ID_GENPW_COPY, "Copy");
  copy->SetBitmap(EPDBIcon("copy", EPDB_MENU_ICON_DIP));
  b->Add(copy, 0, wxLEFT, FromDIP(6));
  b->AddStretchSpacer();
  if(target) {
    wxButton *use = new wxButton(this, ID_GENPW_USE, "Use Password");
    use->SetDefault();
    b->Add(use, 0, wxLEFT, FromDIP(6));
  }
  b->Add(new wxButton(this, wxID_CANCEL, target ? "Cancel" : "Close"), 0, wxLEFT, FromDIP(6));
  top->Add(b, 0, wxEXPAND | wxALL, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizerAndFit(top);
  SetMinSize(GetSize());

  Bind(wxEVT_BUTTON, &GeneratePasswordDialog::OnGenerate, this, ID_GENPW_GENERATE);
  Bind(wxEVT_BUTTON, &GeneratePasswordDialog::OnCopy, this, ID_GENPW_COPY);
  Bind(wxEVT_BUTTON, &GeneratePasswordDialog::OnUse, this, ID_GENPW_USE);
  length->Bind(wxEVT_SPINCTRL, [this](wxSpinEvent &) { Generate(); });
  symbols->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent &) { Generate(); });
  Generate();
  CentreOnParent();
}

GeneratePasswordDialog::~GeneratePasswordDialog()
{
  result->Clear();
}

void GeneratePasswordDialog::Generate()
{
  gxString pw;
  if(EPDBGeneratePassword((unsigned)length->GetValue(), symbols->GetValue() ? 1 : 0, pw) != 0) {
    result->ChangeValue("");
    ProgramError->Message("Cannot generate a password");
    return;
  }
  result->ChangeValue(wxString::FromUTF8(pw.c_str()));
  strength->Update(result->GetValue());
  SecureWipe(pw);
}

void GeneratePasswordDialog::OnGenerate(wxCommandEvent &WXUNUSED(event))
{
  Generate();
}

void GeneratePasswordDialog::OnCopy(wxCommandEvent &WXUNUSED(event))
{
  frame->CopyToClipboard(result->GetValue());
}

void GeneratePasswordDialog::OnUse(wxCommandEvent &WXUNUSED(event))
{
  if(target) target->SetValue(result->GetValue());
  result->Clear();
  EndModal(wxID_OK);
}

SelfDestructDialog::SelfDestructDialog(wxWindow *parent, int on, unsigned limit)
  : wxDialog(parent, wxID_ANY, "Self-Destruct Settings")
{
  SetIcons(EPDBAppIcons());
  const int pad = FromDIP(12);
  const int wrap = 420;
  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *head = new wxBoxSizer(wxHORIZONTAL);
  head->Add(new wxStaticBitmap(this, wxID_ANY, EPDBIcon("security", 32)), 0, wxRIGHT, FromDIP(10));
  wxBoxSizer *texts = new wxBoxSizer(wxVERTICAL);
  texts->Add(EPDBHeading(this, "Destroy the records after too many failed opens"), 0, wxBOTTOM, FromDIP(6));
  wxStaticText *t = new wxStaticText(this, wxID_ANY,
				     "Self-destruct destroys every record of this database after the number of failed "
				     "opens below. It only counts guesses made through this program and epdb_auth_tool: "
				     "anyone with a copy of the file can guess offline or reset the counter.");
  t->Wrap(FromDIP(wrap));
  texts->Add(t, 0, wxBOTTOM, FromDIP(6));
  wxStaticText *w = new wxStaticText(this, wxID_ANY,
				     "A destroyed database cannot be recovered except from your own backups. "
				     "With self-destruct on, a read-only copy is not opened.");
  w->SetForegroundColour(EPDBWarningColour());
  w->Wrap(FromDIP(wrap));
  texts->Add(w, 0);
  head->Add(texts, 1);
  top->Add(head, 0, wxALL, pad);

  wxFlexGridSizer *g = new wxFlexGridSizer(2, FromDIP(6), FromDIP(8));
  enable = new wxCheckBox(this, wxID_ANY, "Self-destruct on");
  enable->SetValue(on != 0);
  g->Add(enable, 0, wxALIGN_CENTER_VERTICAL);
  g->AddSpacer(0);
  g->Add(new wxStaticText(this, wxID_ANY, "Failed opens allowed"), 0, wxALIGN_CENTER_VERTICAL);
  attempts = new wxSpinCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS,
			    1, FC_SD_MAX_ATTEMPTS, on ? (int)limit : FC_SD_DEFAULT_ATTEMPTS);
  g->Add(attempts, 0);
  top->Add(g, 0, wxLEFT | wxRIGHT, pad);
  attempts->Enable(on != 0);
  enable->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent &) { attempts->Enable(enable->GetValue()); });

  wxStdDialogButtonSizer *btns = new wxStdDialogButtonSizer();
  btns->AddButton(new wxButton(this, wxID_OK, "Save"));
  btns->AddButton(new wxButton(this, wxID_CANCEL));
  btns->Realize();
  top->Add(btns, 0, wxEXPAND | wxALL, pad);
  SetEscapeId(wxID_CANCEL);
  SetSizerAndFit(top);
  CentreOnParent();
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
