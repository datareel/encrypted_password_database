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

10/27/2026: Smart card picker, see sc_picker.h. Nothing here logs in to
a card or takes a PIN.
*/
// ----------------------------------------------------------- //
#include "app_defs.h"

#ifdef __ENABLE_SMART_CARD__

wxDEFINE_EVENT(EPDB_EVT_CARD_CHANGED, wxCommandEvent);

const int ID_SCPICKER_LIST = 6101;
const int ID_SCPICKER_RESCAN = 6102;
const int ID_SCPICKER_POLL = 6103;
const int EPDB_CARD_POLL_MS = 2500;

int EPDBReadCardUsers(const char *data_file, std::vector<EPDBCardUser> &users)
{
  users.clear();
  gxDatabase f;
  if(f.Open(data_file, gxDBASE_READONLY) != gxDBASE_NO_ERROR) return -1;
  DatabaseUserAuth db_auth;
  db_auth.f = &f;
  int rv = db_auth.LoadStaticDataBlocks();
  if(rv == 0) {
    for(gxListNode<StaticDataBlock> *p = db_auth.static_block_list.GetHead(); p; p = p->next) {
      if(p->data.block_header.block_type != 2) continue; // 2 is a smart card user
      EPDBCardUser u;
      u.username = p->data.username.c_str();
      u.key_bits = (int)p->data.block_header.ciphertext_len * 8;
      users.push_back(u);
    }
  }
  f.Close();
  return rv == 0 ? 0 : -1;
}

wxString EPDBSlotString(unsigned long slot_id)
{
  if(slot_id < 16) return wxString::Format("%lu", slot_id);
  return wxString::Format("0x%lx", slot_id);
}

wxString EPDBTokenStatus(const SC_TokenInfo &t)
{
  if(!t.token_initialized) return "Not initialized";
  if(t.user_pin_locked) return "PIN locked";
  if(t.user_pin_final_try) return "One PIN try left";
  if(t.user_pin_count_low) return "Few tries left";
  if(t.user_pin_to_be_changed) return "PIN must be changed";
  return "Ready";
}

static std::string EPDBNormalCertID(const std::string &id)
// "id_01", "01" and "1:01" compare as "01". Other forms are kept.
{
  std::string s = id;
  size_t colon = s.find(':');
  if(colon != std::string::npos && s.compare(0, 7, "pkcs11:") != 0) s = s.substr(colon + 1);
  if(s.compare(0, 3, "id_") == 0) s = s.substr(3);
  for(size_t i = 0; i < s.size(); i++) s[i] = (char)tolower((unsigned char)s[i]);
  return s;
}

void EPDBCardRows(const std::vector<SC_TokenInfo> &tokens, const EPDBCardHints &hints,
		  std::vector<EPDBCardRow> &rows)
{
  rows.clear();
  for(size_t t = 0; t < tokens.size(); t++) {
    const SC_TokenInfo &tk = tokens[t];
    if(!tk.token_initialized) continue; // An empty SoftHSM slot or a new card
    wxString tstatus = EPDBTokenStatus(tk);
    int locked = tk.user_pin_locked;
    int have = 0;
    for(size_t c = 0; c < tk.certs.size(); c++) {
      const SC_CertInfo &ci = tk.certs[c];
      EPDBCardRow r;
      r.token = (int)t;
      r.cert = (int)c;
      r.usable = (!locked && ci.is_rsa);
      if(!ci.is_rsa && ci.key_bits) r.status = "Unsupported key (not RSA)";
      else r.status = tstatus;
      if(hints.have_users && r.usable) {
	r.may_open = 0;
	for(size_t u = 0; u < hints.users.size(); u++) {
	  if(ci.key_bits == 0 || hints.users[u].key_bits == ci.key_bits) r.may_open = 1;
	}
	if(!r.may_open) r.status = "No user in this database";
      }
      rows.push_back(r);
      have++;
    }
    if(!have) {
      // Some cards show no objects before login
      EPDBCardRow r;
      r.token = (int)t;
      r.cert = -1;
      r.usable = !locked;
      r.status = locked ? tstatus : wxString("No certificate shown");
      rows.push_back(r);
    }
  }
}

int EPDBChooseCard(const std::vector<SC_TokenInfo> &tokens, const std::vector<EPDBCardRow> &rows,
		   const EPDBCardHints &hints, wxString &why)
{
  why.Clear();
  std::vector<int> candidates;
  for(size_t i = 0; i < rows.size(); i++) {
    if(rows[i].usable && rows[i].may_open && rows[i].cert >= 0) candidates.push_back((int)i);
  }
  if(hints.have_users && hints.users.empty()) {
    why = "This database has no smart card users";
    return -1;
  }
  std::string want_id = EPDBNormalCertID(hints.saved_cert_id);
  unsigned long want_slot = 0;
  int have_slot = !hints.saved_slot.empty() && SC_parse_slot(hints.saved_slot.c_str(), &want_slot);

  // The card and cert used the last time
  if(have_slot && !want_id.empty()) {
    std::vector<int> m;
    for(size_t i = 0; i < candidates.size(); i++) {
      const EPDBCardRow &r = rows[candidates[i]];
      if(tokens[r.token].slot_id == want_slot && EPDBNormalCertID(tokens[r.token].certs[r.cert].id_hex) == want_id) {
	m.push_back(candidates[i]);
      }
    }
    if(m.size() == 1) {
      why = "The card used the last time";
      return m[0];
    }
  }
  // The saved cert ID on exactly one card
  if(!want_id.empty()) {
    std::vector<int> m;
    for(size_t i = 0; i < candidates.size(); i++) {
      const EPDBCardRow &r = rows[candidates[i]];
      if(EPDBNormalCertID(tokens[r.token].certs[r.cert].id_hex) == want_id) m.push_back(candidates[i]);
    }
    if(m.size() == 1) {
      why = "The only card with cert ID " + wxString(hints.saved_cert_id.c_str());
      return m[0];
    }
    if(m.size() > 1) {
      why = wxString::Format("Cert ID %s is on %d cards, choose the card", hints.saved_cert_id.c_str(), (int)m.size());
      return -1;
    }
  }
  if(candidates.size() == 1) {
    why = "The only card that can open this database";
    return candidates[0];
  }
  if(candidates.size() > 1) why = "More than one card could open this database, choose the card";
  else if(rows.empty()) why = "No smart card found";
  else why = "No card can be used, see the status";
  return -1;
}

// ----------------------------------------------------------- //
// SmartCardPicker
// ----------------------------------------------------------- //
wxBEGIN_EVENT_TABLE(SmartCardPicker, wxPanel)
  EVT_BUTTON(ID_SCPICKER_RESCAN, SmartCardPicker::OnRescan)
  EVT_TIMER(ID_SCPICKER_POLL, SmartCardPicker::OnPoll)
  EVT_LIST_ITEM_SELECTED(ID_SCPICKER_LIST, SmartCardPicker::OnSelected)
  EVT_LIST_ITEM_DESELECTED(ID_SCPICKER_LIST, SmartCardPicker::OnDeselected)
wxEND_EVENT_TABLE()

SmartCardPicker::SmartCardPicker(wxWindow *parent, wxWindowID id)
  : wxPanel(parent, id), poll_timer(this, ID_SCPICKER_POLL)
{
  selected_row = -1;
  filling = 0;
  sel_valid = 0;
  sel_slot = 0;
  module_path = progcfg->SC_modulePath.c_str();

  wxBoxSizer *top = new wxBoxSizer(wxVERTICAL);
  list = new wxListCtrl(this, ID_SCPICKER_LIST, wxDefaultPosition, FromDIP(wxSize(-1, 110)),
			wxLC_REPORT | wxLC_SINGLE_SEL);
  list->AppendColumn("Card", wxLIST_FORMAT_LEFT, FromDIP(130));
  list->AppendColumn("Slot", wxLIST_FORMAT_LEFT, FromDIP(90));
  list->AppendColumn("Cert ID", wxLIST_FORMAT_LEFT, FromDIP(70));
  list->AppendColumn("Key", wxLIST_FORMAT_LEFT, FromDIP(80));
  list->AppendColumn("Status", wxLIST_FORMAT_LEFT, FromDIP(150));
  top->Add(list, 1, wxEXPAND);

  wxBoxSizer *row = new wxBoxSizer(wxHORIZONTAL);
  note = EPDBNote(this, "");
  row->Add(note, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, FromDIP(8));
  rescan_btn = new wxButton(this, ID_SCPICKER_RESCAN, "Rescan");
  rescan_btn->SetBitmap(EPDBIcon("rescan", EPDB_MENU_ICON_DIP));
  row->Add(rescan_btn, 0, wxALIGN_CENTER_VERTICAL);
  top->Add(row, 0, wxEXPAND | wxTOP, FromDIP(6));
  SetSizer(top);
}

SmartCardPicker::~SmartCardPicker()
{
  poll_timer.Stop();
}

std::string SmartCardPicker::Signature(const std::vector<SC_TokenInfo> &t) const
{
  std::string s;
  char buf[64];
  for(size_t i = 0; i < t.size(); i++) {
    snprintf(buf, sizeof(buf), "%lu/%lx/", t[i].slot_id, t[i].flags);
    s += buf + t[i].token_label + "/" + t[i].serial + ":";
    for(size_t c = 0; c < t[i].certs.size(); c++) s += t[i].certs[c].id_hex + ",";
    s += ";";
  }
  return s;
}

void SmartCardPicker::Rescan(int auto_select)
{
  std::vector<SC_TokenInfo> t;
  std::string err;
  wxBusyCursor busy;
  int rv = SC_list_tokens(module_path.utf8_str(), t, err);
  last_signature = Signature(t);
  tokens = t;
  Fill();
  if(rv != 0) {
    note->SetLabel(wxString::FromUTF8(err.c_str()));
    note->SetForegroundColour(EPDBWarningColour());
  }
  if(auto_select && rv == 0) {
    wxString why;
    int pick = EPDBChooseCard(tokens, rows, hints, why);
    if(pick >= 0) {
      filling = 1;
      list->SetItemState(pick, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
			 wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
      list->EnsureVisible(pick);
      filling = 0;
      selected_row = pick;
      Remember();
    }
    note->SetLabel(why);
    note->SetForegroundColour(pick >= 0 ? EPDBMutedTextColour() : EPDBWarningColour());
  }
  Layout();
  Notify();
}

void SmartCardPicker::Fill()
{
  // The selected card stays selected while it is present
  EPDBCardRows(tokens, hints, rows);
  filling = 1;
  list->DeleteAllItems();
  selected_row = -1;
  for(size_t i = 0; i < rows.size(); i++) {
    const EPDBCardRow &r = rows[i];
    const SC_TokenInfo &tk = tokens[r.token];
    wxString name = wxString::FromUTF8(tk.token_label.c_str());
    if(name.IsEmpty()) name = wxString::FromUTF8(tk.model.c_str());
    long item = list->InsertItem((long)i, name);
    list->SetItem(item, 1, EPDBSlotString(tk.slot_id));
    if(r.cert >= 0) {
      const SC_CertInfo &ci = tk.certs[r.cert];
      list->SetItem(item, 2, wxString::FromUTF8(ci.id_hex.c_str()));
      wxString key;
      if(ci.key_bits) key = wxString::Format("%s %d", ci.is_rsa ? "RSA" : "EC", ci.key_bits);
      else key = ci.is_rsa ? "RSA" : "Unknown";
      list->SetItem(item, 3, key);
    }
    list->SetItem(item, 4, r.status);
    if(!r.usable || !r.may_open) list->SetItemTextColour(item, EPDBMutedTextColour());
    if(sel_valid && r.usable && r.cert >= 0 && tk.slot_id == sel_slot && tk.certs[r.cert].id_hex == sel_id) {
      list->SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      selected_row = (int)i;
    }
  }
  filling = 0;
  if(rows.empty()) {
    note->SetLabel("No smart card found. Insert a card, it is found automatically.");
    note->SetForegroundColour(EPDBWarningColour());
  }
  else if(selected_row < 0) {
    note->SetLabel("Choose the card");
    note->SetForegroundColour(EPDBMutedTextColour());
  }
}

void SmartCardPicker::StartPolling()
{
  poll_timer.Start(EPDB_CARD_POLL_MS);
}

void SmartCardPicker::StopPolling()
{
  poll_timer.Stop();
}

void SmartCardPicker::OnPoll(wxTimerEvent &WXUNUSED(event))
// Reads the readers again, no login. The list is only redrawn when a
// card was inserted, removed or changed.
{
  if(!IsShownOnScreen()) return;
  std::vector<SC_TokenInfo> t;
  std::string err;
  if(SC_list_tokens(module_path.utf8_str(), t, err) != 0) return;
  std::string sig = Signature(t);
  if(sig == last_signature) return;
  int had_none = rows.empty() || selected_row < 0;
  last_signature = sig;
  tokens = t;
  Fill();
  if(had_none && selected_row < 0) {
    // A card was inserted: preselect it with the same rules
    wxString why;
    int pick = EPDBChooseCard(tokens, rows, hints, why);
    if(pick >= 0) {
      filling = 1;
      list->SetItemState(pick, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
      filling = 0;
      selected_row = pick;
      Remember();
    }
    if(!rows.empty()) {
      note->SetLabel(why);
      note->SetForegroundColour(pick >= 0 ? EPDBMutedTextColour() : EPDBWarningColour());
    }
  }
  Layout();
  Notify();
}

void SmartCardPicker::OnRescan(wxCommandEvent &WXUNUSED(event))
{
  Rescan(selected_row < 0);
}

void SmartCardPicker::OnSelected(wxListEvent &event)
{
  if(filling) return;
  int i = (int)event.GetIndex();
  if(i < 0 || i >= (int)rows.size()) return;
  if(!rows[i].usable) {
    // Locked cards and keys that are not RSA cannot be chosen
    filling = 1;
    list->SetItemState(i, 0, wxLIST_STATE_SELECTED);
    filling = 0;
    note->SetLabel("This card cannot be used: " + rows[i].status);
    note->SetForegroundColour(EPDBWarningColour());
    selected_row = -1;
    Notify();
    return;
  }
  selected_row = i;
  Remember();
  if(!rows[i].may_open) {
    note->SetLabel("The key size of this card matches no smart card user of this database");
    note->SetForegroundColour(EPDBWarningColour());
  }
  else {
    note->SetLabel("");
  }
  Notify();
}

void SmartCardPicker::OnDeselected(wxListEvent &WXUNUSED(event))
{
  if(filling) return;
  selected_row = -1;
  sel_valid = 0;
  Notify();
}

void SmartCardPicker::Remember()
{
  sel_valid = 0;
  if(selected_row < 0 || selected_row >= (int)rows.size() || rows[selected_row].cert < 0) return;
  sel_slot = tokens[rows[selected_row].token].slot_id;
  sel_id = tokens[rows[selected_row].token].certs[rows[selected_row].cert].id_hex;
  sel_valid = 1;
}

void SmartCardPicker::Notify()
{
  wxCommandEvent e(EPDB_EVT_CARD_CHANGED, GetId());
  e.SetEventObject(this);
  ProcessWindowEvent(e);
}

int SmartCardPicker::HasChoice() const
{
  return (selected_row >= 0 && selected_row < (int)rows.size() && rows[selected_row].usable);
}

int SmartCardPicker::GetChoice(SC_TokenInfo &token, SC_CertInfo &cert) const
{
  if(!HasChoice()) return -1;
  const EPDBCardRow &r = rows[selected_row];
  token = tokens[r.token];
  cert = (r.cert >= 0) ? tokens[r.token].certs[r.cert] : SC_CertInfo();
  return 0;
}

wxString SmartCardPicker::ChoiceName() const
{
  if(!HasChoice()) return wxString();
  const SC_TokenInfo &tk = tokens[rows[selected_row].token];
  wxString name = wxString::FromUTF8(tk.token_label.c_str());
  if(name.IsEmpty()) name = wxString::FromUTF8(tk.model.c_str());
  return name + " (slot " + EPDBSlotString(tk.slot_id) + ")";
}

#endif // __ENABLE_SMART_CARD__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
