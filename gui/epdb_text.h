// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File Name: epdb_text.h
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

10/27/2026: Text in the database is UTF-8, as epdb_auth_tool writes it.
These helpers convert between the database bytes and wxString. Bytes
that are not valid UTF-8 (written by an older Windows build in the
ANSI code page) are read as ANSI (Latin-1 on Linux), so they still show.
*/
// ----------------------------------------------------------- // 
#ifndef __EPDB_TEXT_HPP__
#define __EPDB_TEXT_HPP__

// Sets the C library character type to UTF-8, so the implicit char*
// conversions of wxString read and write UTF-8 (call once at startup)
void EPDBSetUTF8Locale();

// Database bytes to text: UTF-8, else ANSI or Latin-1
wxString EPDBFromDB(const char *s);

// Text to database bytes: UTF-8
gxString EPDBToDB(const wxString &s);

// Finds the record with this title: the UTF-8 key first, then the
// ANSI or Latin-1 bytes of an older record. Sets member 0 of ih to
// the key that was found. Returns 1 if found.
int EPDBFindTitle(INFOHOG &ih, const wxString &title);

#endif // __EPDB_TEXT_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
