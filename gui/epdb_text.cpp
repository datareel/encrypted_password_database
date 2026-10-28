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

10/27/2026: Recent databases and the unlock method last used for each,
kept in recent_files.txt next to the config file (only the user can
read it). It holds file names, the method, the card slot, cert ID and
usernames, never a password, key, PIN or passphrase.

10/27/2026: Database text conversion, see epdb_text.h
*/
// ----------------------------------------------------------- //
#include "app_defs.h"
#include <clocale>

void EPDBSetUTF8Locale()
{
#if defined (__WIN32__)
  // The manifest sets the process code page to UTF-8 (Windows 10 1903
  // and later), the C runtime locale follows it
  setlocale(LC_CTYPE, ".UTF8");
#else
  if(!setlocale(LC_CTYPE, "C.UTF-8")) setlocale(LC_CTYPE, "en_US.UTF-8");
#endif
}

static const wxMBConv &EPDBLegacyConv()
{
#if defined (__WIN32__)
  // The user's ANSI code page, not the process code page, which the
  // manifest sets to UTF-8
  static wxCSConv *conv = 0;
  if(!conv) {
    UINT cp = 0;
    if(GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER,
		       (LPWSTR)&cp, sizeof(cp) / sizeof(WCHAR)) == 0 || cp == 0 || cp == CP_UTF8) cp = 1252;
    conv = new wxCSConv(wxString::Format("cp%u", cp));
    if(!conv->IsOk()) {
      delete conv;
      conv = new wxCSConv(wxFONTENCODING_CP1252);
    }
  }
  return *conv;
#else
  return wxConvISO8859_1;
#endif
}

wxString EPDBFromDB(const char *s)
{
  if(!s || !*s) return wxString();
  wxString w = wxString::FromUTF8(s);
  if(w.IsEmpty()) w = wxString(s, EPDBLegacyConv());
  return w;
}

gxString EPDBToDB(const wxString &s)
{
  gxString g;
  wxScopedCharBuffer b = s.utf8_str();
  if(b.length() > 0) g.InsertAt(0, b.data(), b.length());
  return g;
}

int EPDBFindTitle(INFOHOG &ih, const wxString &title)
{
  gxString u = EPDBToDB(title);
  INFOHOG_t key(u.c_str());
  ih.SetMember(&key, sizeof(key), 0);
  if(ih.FindObject()) return 1;
  wxCharBuffer legacy = title.mb_str(EPDBLegacyConv());
  if(!legacy.data() || !*legacy.data() || u == gxString(legacy.data())) return 0;
  INFOHOG_t key2(legacy.data());
  ih.SetMember(&key2, sizeof(key2), 0);
  if(ih.FindObject()) return 1;
  ih.SetMember(&key, sizeof(key), 0);
  return 0;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
