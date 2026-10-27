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

10/27/2026: Record form, see record_form.h
*/
// ----------------------------------------------------------- //
#include "app_defs.h"

static unsigned EPDBFieldBytes(const wxString &v)
{
  wxScopedCharBuffer b = v.utf8_str();
  return (unsigned)b.length();
}

RecordFieldCtrl::RecordFieldCtrl(wxWindow *parent, int m, int ro) : wxPanel(parent, wxID_ANY)
{
  member = m;
  read_only = ro;
  limiting = 0;
  text = 0;
  pw = 0;
  counter = 0;
  wxBoxSizer *s = new wxBoxSizer(wxVERTICAL);
  if(IsPasswordField(m)) {
    pw = new PasswordEntry(this, wxID_ANY);
    pw->SetEditable(!ro);
    s->Add(pw, 0, wxEXPAND);
    pw->Bind(wxEVT_TEXT, &RecordFieldCtrl::OnText, this);
  }
  else {
    long style = ro ? wxTE_READONLY : 0;
    if(IsMultiLineField(m)) style |= wxTE_MULTILINE;
    text = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, style);
    if(IsMultiLineField(m)) text->SetMinSize(wxSize(FromDIP(260), text->GetCharHeight() * 4 + FromDIP(10)));
    else text->SetMinSize(wxSize(FromDIP(260), -1));
    s->Add(text, 1, wxEXPAND);
    text->Bind(wxEVT_TEXT, &RecordFieldCtrl::OnText, this);
  }
  if(!ro && (IsMultiLineField(m) || m == 0)) {
    counter = EPDBNote(this, "");
    counter->SetMinSize(wxSize(counter->GetTextExtent("255 / 255").x + FromDIP(4), -1));
    s->Add(counter, 0, wxALIGN_RIGHT | wxTOP, FromDIP(1));
  }
  SetSizer(s);
  UpdateCounter();
}

wxString RecordFieldCtrl::GetValue() const
{
  wxString v = pw ? pw->GetValue() : text->GetValue();
  v.Replace("\r\n", "\n"); // 10/27/2026: The same bytes on Windows and Linux
  return v;
}

void RecordFieldCtrl::SetValue(const wxString &v)
{
  limiting = 1;
  if(pw) pw->SetValue(v);
  else text->ChangeValue(v);
  last_ok = v;
  limiting = 0;
  UpdateCounter();
}

void RecordFieldCtrl::Clear()
{
  limiting = 1;
  if(pw) pw->Clear();
  else text->Clear();
  last_ok.Clear();
  limiting = 0;
  UpdateCounter();
}

void RecordFieldCtrl::Reveal(int show)
{
  if(pw) pw->Reveal(show);
}

void RecordFieldCtrl::UpdateCounter()
{
  if(!counter) return;
  unsigned n = EPDBFieldBytes(GetValue());
  counter->SetLabel(wxString::Format("%u / %u", n, EPDB_FIELD_MAX_BYTES));
  counter->SetForegroundColour(n >= EPDB_FIELD_MAX_BYTES ? EPDBWarningColour() : EPDBMutedTextColour());
}

void RecordFieldCtrl::OnText(wxCommandEvent &event)
// Refuses input past the field size: the last value that fit is put back
{
  if(limiting) return;
  wxString v = GetValue();
  if(EPDBFieldBytes(v) > EPDB_FIELD_MAX_BYTES) {
    limiting = 1;
    long ip = text ? text->GetInsertionPoint() : 0;
    if(pw) pw->SetValue(last_ok);
    else {
      text->ChangeValue(last_ok);
      text->SetInsertionPoint(wxMin(ip, (long)last_ok.length()));
    }
    limiting = 0;
    wxBell();
  }
  else {
    last_ok = v;
  }
  UpdateCounter();
  // The dialog sees the change as this control's text event
  wxCommandEvent e(wxEVT_TEXT, GetId());
  e.SetEventObject(this);
  ProcessWindowEvent(e);
  event.Skip(false);
}

// ----------------------------------------------------------- //
// RecordForm
// ----------------------------------------------------------- //
RecordForm::RecordForm()
{
  title_label = 0;
  labels = 0;
  inputs = 0;
  strength = 0;
  generate = 0;
  autofill = 0;
  read_only = 0;
}

void RecordForm::Build(wxWindow *p, wxSizer *sizer, RecordFieldCtrl *in[], wxStaticText *lb[], int ro)
{
  inputs = in;
  labels = lb;
  read_only = ro;
  const int gap = p->FromDIP(6);
  for(int i = 0; i < MaxPanelMembers; i++) {
    inputs[i] = new RecordFieldCtrl(p, i, ro);
    inputs[i]->SetMaxSize(wxSize(-1, -1));
  }
  title_label = new wxStaticText(p, wxID_ANY, progcfg->global_dbparms.dbgrid_labels[0].c_str());
  for(int i = 1; i < MaxPanelMembers; i++) {
    labels[i-1] = new wxStaticText(p, wxID_ANY, progcfg->global_dbparms.dbgrid_labels[i].c_str());
  }

  // Title across the top, then two columns, Notes across the bottom
  wxGridBagSizer *g = new wxGridBagSizer(gap / 2, p->FromDIP(14));
  int row = 0;
  g->Add(title_label, wxGBPosition(row++, 0), wxGBSpan(1, 2));
  g->Add(inputs[0], wxGBPosition(row++, 0), wxGBSpan(1, 2), wxEXPAND | wxBOTTOM, gap);
  // Username | Website link
  g->Add(labels[0], wxGBPosition(row, 0));
  g->Add(labels[5], wxGBPosition(row++, 1));
  g->Add(inputs[1], wxGBPosition(row, 0), wxDefaultSpan, wxEXPAND | wxBOTTOM, gap);
  g->Add(inputs[6], wxGBPosition(row++, 1), wxDefaultSpan, wxEXPAND | wxBOTTOM, gap);
  // Password | Previous password
  g->Add(labels[1], wxGBPosition(row, 0));
  g->Add(labels[2], wxGBPosition(row++, 1));
  wxBoxSizer *pwrow = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *pwline = new wxBoxSizer(wxHORIZONTAL);
  pwline->Add(inputs[2], 1, wxALIGN_CENTER_VERTICAL);
  if(!ro) {
    generate = new wxButton(p, wxID_ANY, "Generate");
    generate->SetBitmap(EPDBIcon("generate", EPDB_MENU_ICON_DIP));
    generate->SetToolTip("Make a random password");
    pwline->Add(generate, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, gap);
    generate->Bind(wxEVT_BUTTON, &RecordForm::OnGenerate, this);
  }
  pwrow->Add(pwline, 0, wxEXPAND);
  if(!ro) {
    strength = new PasswordStrength(p);
    pwrow->Add(strength, 0, wxEXPAND | wxTOP, p->FromDIP(3));
    inputs[2]->Bind(wxEVT_TEXT, &RecordForm::OnPasswordText, this);
  }
  g->Add(pwrow, wxGBPosition(row, 0), wxDefaultSpan, wxEXPAND | wxBOTTOM, gap);
  g->Add(inputs[3], wxGBPosition(row++, 1), wxDefaultSpan, wxEXPAND | wxBOTTOM, gap);
  // Security questions | Account information
  g->Add(labels[3], wxGBPosition(row, 0));
  g->Add(labels[4], wxGBPosition(row++, 1));
  g->Add(inputs[4], wxGBPosition(row, 0), wxDefaultSpan, wxEXPAND | wxBOTTOM, gap);
  g->Add(inputs[5], wxGBPosition(row, 1), wxDefaultSpan, wxEXPAND | wxBOTTOM, gap);
  g->AddGrowableRow(row++, 1);
  // Notes
  g->Add(labels[6], wxGBPosition(row++, 0), wxGBSpan(1, 2));
  g->Add(inputs[7], wxGBPosition(row, 0), wxGBSpan(1, 2), wxEXPAND);
  g->AddGrowableRow(row, 1);
  g->AddGrowableCol(0, 1);
  g->AddGrowableCol(1, 1);
  sizer->Add(g, 1, wxEXPAND);
}

void RecordForm::SetLabels(CryptDBDocument *doc)
{
  char dest[DBStringLength];
  for(int i = 0; i < MaxPanelMembers; i++) {
    gxString name;
    if(doc && doc->GridFrame()) name = doc->DBParms()->db_config.GetColName(i, dest);
    if(name.is_null()) name = progcfg->global_dbparms.dbgrid_labels[i];
    wxString n = wxString::FromUTF8(name.c_str());
    if(i == 0) title_label->SetLabel(n);
    else labels[i-1]->SetLabel(n);
  }
  for(int i = 0; i < MaxPanelMembers; i++) inputs[i]->Reveal(0);
}

void RecordForm::OnGenerate(wxCommandEvent &WXUNUSED(event))
{
  GeneratePasswordDialog dlg(inputs[2]->GetParent(), inputs[2]->Password());
  dlg.ShowModal();
}

void RecordForm::SetOriginalPassword(const wxString &pwv, const wxString &prev)
{
  original_password = pwv;
  original_previous = prev;
  autofill = 1;
  if(strength) strength->Update(pwv);
}

void RecordForm::OnPasswordText(wxCommandEvent &event)
{
  event.Skip();
  wxString v = inputs[2]->GetValue();
  if(strength) strength->Update(v);
  // The old password becomes the previous password, unless the user
  // already changed the previous password
  if(autofill && !original_password.IsEmpty()) {
    if(inputs[3]->GetValue() == original_previous || inputs[3]->GetValue() == original_password) {
      inputs[3]->SetValue(v == original_password ? original_previous : original_password);
    }
  }
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
