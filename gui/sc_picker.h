// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File Name: sc_picker.h
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

10/27/2026: Smart card picker for the Unlock, Create and Add User
dialogs. Lists the cards with fcrypt SC_list_tokens(), which never logs
in to a card and never takes a PIN, and polls the readers while shown.
EPDBChooseCard() picks the card to preselect; it never picks a card
when more than one could match, so the PIN is never sent to a guessed
card.
*/
// ----------------------------------------------------------- //
#ifndef __SC_PICKER_HPP__
#define __SC_PICKER_HPP__

#include "app_defs.h"

#ifdef __ENABLE_SMART_CARD__

// A smart card user of a database, read from the static area without
// the database key
struct EPDBCardUser
{
  std::string username;
  int key_bits; // Size of the RSA key the database key is wrapped with
};

// Reads the smart card users of a database. Returns 0, or -1 if the
// file cannot be read (the list is then empty).
int EPDBReadCardUsers(const char *data_file, std::vector<EPDBCardUser> &users);

// What is known before the card is chosen
struct EPDBCardHints
{
  EPDBCardHints() { have_users = 0; }
  std::string saved_slot;     // SC_slot from the last good open, "" for none
  std::string saved_cert_id;  // SC_cert_id from the last good open
  std::string username;       // Smart card username
  int have_users;             // 1 if users holds the database's card users
  std::vector<EPDBCardUser> users;
};

// One row of the picker: a card object, or a card that shows no objects
struct EPDBCardRow
{
  EPDBCardRow() { token = -1; cert = -1; usable = 0; may_open = 1; }
  int token;   // Index in the token list
  int cert;    // Index in the token's certs, -1 for a card with none shown
  int usable;  // 0 for locked cards and keys that are not RSA
  int may_open; // 0 if the key size matches no card user of the database
  wxString status;
};

// Makes the rows for a token list
void EPDBCardRows(const std::vector<SC_TokenInfo> &tokens, const EPDBCardHints &hints,
		  std::vector<EPDBCardRow> &rows);

// 10/27/2026: Picks the row to preselect, or -1 to let the user choose.
// Today: a saved slot and cert ID that match one usable row, or the only
// usable row whose key size matches a card user of the database. Later
// this can match the card's public key fingerprint to the user blocks.
int EPDBChooseCard(const std::vector<SC_TokenInfo> &tokens, const std::vector<EPDBCardRow> &rows,
		   const EPDBCardHints &hints, wxString &why);

wxString EPDBTokenStatus(const SC_TokenInfo &t);
wxString EPDBSlotString(unsigned long slot_id);

wxDECLARE_EVENT(EPDB_EVT_CARD_CHANGED, wxCommandEvent);

class SmartCardPicker : public wxPanel
{
public:
  SmartCardPicker(wxWindow *parent, wxWindowID id = wxID_ANY);
  ~SmartCardPicker();

public:
  void SetModulePath(const wxString &path) { module_path = path; }
  wxString GetModulePath() const { return module_path; }
  void SetHints(const EPDBCardHints &h) { hints = h; }
  void Rescan(int auto_select);
  void StartPolling();
  void StopPolling();
  // The chosen card and cert. Returns 0 if a usable card is selected.
  int GetChoice(SC_TokenInfo &token, SC_CertInfo &cert) const;
  int HasChoice() const;
  wxString ChoiceName() const; // "label (slot N)" for the PIN label
  int NumCards() const { return (int)tokens.size(); }

private:
  void OnRescan(wxCommandEvent &event);
  void OnPoll(wxTimerEvent &event);
  void OnSelected(wxListEvent &event);
  void OnDeselected(wxListEvent &event);
  void Fill();
  void Notify();
  void Remember();
  std::string Signature(const std::vector<SC_TokenInfo> &t) const;

private:
  wxListCtrl *list;
  wxButton *rescan_btn;
  wxStaticText *note;
  wxTimer poll_timer;
  wxString module_path;
  EPDBCardHints hints;
  std::vector<SC_TokenInfo> tokens;
  std::vector<EPDBCardRow> rows;
  std::string last_signature;
  int selected_row;
  int filling;
  int sel_valid;          // The last chosen card, kept when the list is read again
  unsigned long sel_slot;
  std::string sel_id;

  wxDECLARE_EVENT_TABLE();
};

#endif // __ENABLE_SMART_CARD__

#endif // __SC_PICKER_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
