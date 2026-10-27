// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File Name: cmdbuild_dlg.h
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

10/27/2026: Tools > Command Builder. Shows the epdb_auth_tool command
for an operation, for bash, PowerShell and cmd.exe, updated as the
options change. The command is made by EPDBBuildCommand(), which never
puts a secret on the command line.
*/
// ----------------------------------------------------------- //
#ifndef __CMDBUILD_DLG_HPP__
#define __CMDBUILD_DLG_HPP__

#include "app_defs.h"

class CryptDBDocument;

// Where a secret comes from: a protected file, AWS Secrets Manager or
// another command. The secret itself is never asked for.
class SecretSourcePanel : public wxPanel
{
public:
  SecretSourcePanel(wxWindow *parent, const wxString &what);
  void Get(EPDBCmdSecret &s) const;
  void SetFile(const wxString &file);

private:
  void OnSource(wxCommandEvent &event);
  void OnBrowse(wxCommandEvent &event);

private:
  wxChoice *source;
  wxTextCtrl *value;
  wxButton *browse;
};

class CommandBuilderDialog : public wxDialog
{
public:
  CommandBuilderDialog(wxWindow *parent, CryptDBDocument *doc, int prefill_record);

private:
  wxWindow *AddRow(const wxString &label, wxWindow *ctrl, int group);
  wxWindow *FileRow(wxTextCtrl *&text, const wxString &value, int save);
  void Prefill(CryptDBDocument *doc, int prefill_record);
  void ShowRows();
  void Rebuild();
  void Collect(EPDBCmdOptions &o);
  wxString ToolPath() const;
  void OnChange(wxCommandEvent &event);
  void OnCopy(wxCommandEvent &event);
  void OnSave(wxCommandEvent &event);
  void OnTest(wxCommandEvent &event);
  void OnUpdateTest(wxUpdateUIEvent &event);

private:
  wxScrolledWindow *opts;
  wxFlexGridSizer *grid;
  std::vector<wxWindow *> row_labels;
  std::vector<wxWindow *> row_ctrls;
  std::vector<int> row_groups;

  wxChoice *operation;
  wxTextCtrl *database;
  wxTextCtrl *tool;
  wxChoice *auth;
  SecretSourcePanel *password;
  wxTextCtrl *key_file;
  wxTextCtrl *rsa_key;
  wxTextCtrl *rsa_user;
  wxCheckBox *rsa_has_pass;
  SecretSourcePanel *rsa_pass;
  wxTextCtrl *sc_user;
  wxTextCtrl *sc_cert;
  wxTextCtrl *sc_slot;
  wxTextCtrl *sc_provider;
  SecretSourcePanel *sc_pin;
  wxTextCtrl *record;
  wxChoice *field;
  wxChoice *record_source;
  wxTextCtrl *record_file;
  wxChoice *export_format;
  wxTextCtrl *output_file;
  wxChoice *input_format;
  wxTextCtrl *merge_db;
  wxChoice *merge_auth;
  SecretSourcePanel *merge_pass;
  wxTextCtrl *merge_key;
  wxChoice *merge_policy;
  SecretSourcePanel *new_pass;
  wxSpinCtrl *gen_length;
  wxCheckBox *gen_symbols;
  wxTextCtrl *user_name;
  wxTextCtrl *user_key;
  wxCheckBox *user_from_card;
  wxChoice *remove_type;
  wxTextCtrl *rekey_file;
  wxCheckBox *non_interactive;
  wxCheckBox *json;
  wxCheckBox *quiet;
  wxCheckBox *dry_run;
  wxSpinCtrl *lock_timeout;

  wxNotebook *shells;
  wxTextCtrl *out[3];
  wxStaticText *notes;
  wxStaticText *error;
  int building;
  EPDBCmdResult last[3];
  int last_ok;
};

#endif // __CMDBUILD_DLG_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
