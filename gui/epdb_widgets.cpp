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

10/27/2026: Shared controls, see epdb_widgets.h
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

PasswordEntry::PasswordEntry(wxWindow *parent, wxWindowID id, int show_button, long extra_style)
  : wxPanel(parent, id)
{
  revealed = 0;
  syncing = 0;
  wxBoxSizer *s = new wxBoxSizer(wxHORIZONTAL);
  masked = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
			  wxTE_PASSWORD | wxTE_PROCESS_ENTER | extra_style);
  plain = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | extra_style);
  plain->Hide();
  s->Add(masked, 1, wxALIGN_CENTER_VERTICAL);
  s->Add(plain, 1, wxALIGN_CENTER_VERTICAL);
  eye = 0;
  if(show_button) {
    eye = new wxBitmapButton(this, wxID_ANY, EPDBIcon("eye", EPDB_MENU_ICON_DIP));
    eye->SetToolTip("Show the value");
    s->Add(eye, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(4));
    eye->Bind(wxEVT_BUTTON, &PasswordEntry::OnToggle, this);
  }
  masked->Bind(wxEVT_TEXT, &PasswordEntry::OnText, this);
  plain->Bind(wxEVT_TEXT, &PasswordEntry::OnText, this);
  masked->Bind(wxEVT_TEXT_ENTER, &PasswordEntry::OnEnter, this);
  plain->Bind(wxEVT_TEXT_ENTER, &PasswordEntry::OnEnter, this);
  SetSizer(s);
}

wxString PasswordEntry::GetValue() const
{
  return Ctrl()->GetValue();
}

void PasswordEntry::SetValue(const wxString &v)
{
  syncing = 1;
  Ctrl()->ChangeValue(v);
  syncing = 0;
}

void PasswordEntry::Clear()
{
  syncing = 1;
  masked->Clear();
  plain->Clear();
  syncing = 0;
}

void PasswordEntry::SetMaxLength(unsigned long len)
{
  masked->SetMaxLength(len);
  plain->SetMaxLength(len);
}

void PasswordEntry::SetEditable(bool editable)
{
  masked->SetEditable(editable);
  plain->SetEditable(editable);
}

bool PasswordEntry::Enable(bool enable)
{
  masked->Enable(enable);
  plain->Enable(enable);
  if(eye) eye->Enable(enable);
  return wxPanel::Enable(enable);
}

void PasswordEntry::Reveal(int show)
{
  if((show != 0) == (revealed != 0)) return;
  wxString v = GetValue();
  syncing = 1;
  wxTextCtrl *from = Ctrl();
  revealed = show;
  wxTextCtrl *to = Ctrl();
  to->ChangeValue(v);
  from->Clear();
  syncing = 0;
  from->Hide();
  to->Show();
  if(eye) {
    eye->SetBitmap(EPDBIcon(revealed ? "eye_off" : "eye", EPDB_MENU_ICON_DIP));
    eye->SetToolTip(revealed ? "Hide the value" : "Show the value");
  }
  Layout();
  v.Clear();
}

void PasswordEntry::OnToggle(wxCommandEvent &WXUNUSED(event))
{
  Reveal(!revealed);
  Ctrl()->SetFocus();
}

void PasswordEntry::OnText(wxCommandEvent &event)
{
  if(syncing) return;
  // Pass the text change to the parent as this control's event
  wxCommandEvent e(wxEVT_TEXT, GetId());
  e.SetEventObject(this);
  e.SetString(wxString());
  ProcessWindowEvent(e);
  event.Skip(false);
}

void PasswordEntry::OnEnter(wxCommandEvent &WXUNUSED(event))
{
  wxCommandEvent e(wxEVT_TEXT_ENTER, GetId());
  e.SetEventObject(this);
  ProcessWindowEvent(e);
}

int EPDBPasswordScore(const wxString &pw, wxString &word)
{
  size_t n = pw.length();
  int lower = 0, upper = 0, digit = 0, other = 0;
  int repeats = 0, sequences = 0;
  for(size_t i = 0; i < n; i++) {
    wxUniChar c = pw[i];
    if(c >= 'a' && c <= 'z') lower = 1;
    else if(c >= 'A' && c <= 'Z') upper = 1;
    else if(c >= '0' && c <= '9') digit = 1;
    else other = 1;
    if(i > 0 && pw[i] == pw[i-1]) repeats++;
    if(i > 0 && (int)pw[i].GetValue() == (int)pw[i-1].GetValue() + 1) sequences++;
  }
  int pool = (lower ? 26 : 0) + (upper ? 26 : 0) + (digit ? 10 : 0) + (other ? 33 : 0);
  double bits = 0;
  if(pool > 0) bits = (double)(n - repeats/2 - sequences/2) * log((double)pool) / log(2.0);
  int score = (int)(bits * 100.0 / 90.0); // 90 bits is full
  if(score > 100) score = 100;
  if(score < 0) score = 0;
  if(n == 0) word = "";
  else if(n < 8) word = "Too short";
  else if(bits < 40) word = "Weak";
  else if(bits < 60) word = "Fair";
  else if(bits < 80) word = "Good";
  else word = "Strong";
  return score;
}

PasswordStrength::PasswordStrength(wxWindow *parent) : wxPanel(parent, wxID_ANY)
{
  wxBoxSizer *s = new wxBoxSizer(wxHORIZONTAL);
  gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, FromDIP(wxSize(120, 8)));
  label = EPDBNote(this, "");
  s->Add(gauge, 1, wxALIGN_CENTER_VERTICAL);
  s->Add(label, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, FromDIP(8));
  label->SetMinSize(wxSize(GetTextExtent("Too short").x + FromDIP(4), -1));
  SetSizer(s);
}

void PasswordStrength::Update(const wxString &pw)
{
  wxString word;
  int score = EPDBPasswordScore(pw, word);
  gauge->SetValue(score);
  label->SetLabel(word);
}

wxSizer *EPDBFileRow(wxWindow *parent, wxTextCtrl *&text, wxButton *&browse, wxWindowID browse_id,
		     const wxString &value)
{
  wxBoxSizer *s = new wxBoxSizer(wxHORIZONTAL);
  text = new wxTextCtrl(parent, wxID_ANY, value);
  text->SetMinSize(wxSize(parent->FromDIP(260), -1));
  browse = new wxButton(parent, browse_id, "Browse...");
  s->Add(text, 1, wxALIGN_CENTER_VERTICAL);
  s->Add(browse, 0, wxLEFT | wxALIGN_CENTER_VERTICAL, parent->FromDIP(6));
  return s;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
