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

10/27/2026: Recent databases, see epdb_recent.h
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

static wxString EPDBRecentFileName()
{
  wxFileName fn(wxString::FromUTF8(progcfg->cfgFile.c_str()));
  fn.SetFullName("recent_files.txt");
  return fn.GetFullPath();
}

void EPDBRecentLoad(std::vector<EPDBRecentFile> &files)
// One file per line: path, method, key file, username, slot and cert
// ID, separated by tabs
{
  files.clear();
  wxFile f;
  wxString name = EPDBRecentFileName();
  if(!wxFileExists(name) || !f.Open(name)) return;
  wxString text;
  f.ReadAll(&text, wxConvUTF8);
  wxStringTokenizer lines(text, "\n");
  while(lines.HasMoreTokens() && (int)files.size() < EPDB_MAX_RECENT_FILES) {
    wxString line = lines.GetNextToken();
    line.Trim();
    if(line.IsEmpty() || line[0] == '#') continue;
    wxArrayString v = wxSplit(line, '\t', 0);
    if(v.size() < 1 || v[0].IsEmpty()) continue;
    EPDBRecentFile r;
    r.path = v[0];
    if(v.size() > 1) r.method = v[1];
    if(v.size() > 2) r.key_file = v[2];
    if(v.size() > 3) r.username = v[3];
    if(v.size() > 4) r.slot = v[4];
    if(v.size() > 5) r.cert_id = v[5];
    files.push_back(r);
  }
}

int EPDBRecentFind(const wxString &path, EPDBRecentFile &out)
{
  std::vector<EPDBRecentFile> files;
  EPDBRecentLoad(files);
  for(size_t i = 0; i < files.size(); i++) {
    if(files[i].path == path) {
      out = files[i];
      return 1;
    }
  }
  return 0;
}

static wxString EPDBRecentClean(const wxString &s)
{
  wxString c(s);
  c.Replace("\t", " ");
  c.Replace("\n", " ");
  c.Replace("\r", " ");
  return c;
}

void EPDBRecentRemember(const EPDBRecentFile &file)
{
  std::vector<EPDBRecentFile> files;
  EPDBRecentLoad(files);
  wxString text = "# Encrypted Password Database recent files: path, unlock method, key file, username, slot, cert ID\n";
  text << EPDBRecentClean(file.path) << "\t" << EPDBRecentClean(file.method) << "\t" << EPDBRecentClean(file.key_file)
       << "\t" << EPDBRecentClean(file.username) << "\t" << EPDBRecentClean(file.slot) << "\t"
       << EPDBRecentClean(file.cert_id) << "\n";
  int n = 1;
  for(size_t i = 0; i < files.size() && n < EPDB_MAX_RECENT_FILES; i++) {
    if(files[i].path == file.path) continue;
    text << EPDBRecentClean(files[i].path) << "\t" << files[i].method << "\t" << files[i].key_file << "\t"
	 << files[i].username << "\t" << files[i].slot << "\t" << files[i].cert_id << "\n";
    n++;
  }
  wxScopedCharBuffer buf = text.utf8_str();
  gxString err;
  WritePrivateFile(EPDBRecentFileName().utf8_str(), buf.data(), (unsigned)buf.length(), err);
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
