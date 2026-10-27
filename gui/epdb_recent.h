// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Header File Name: epdb_recent.h
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
*/
// ----------------------------------------------------------- //   
#ifndef __EPDB_RECENT_HPP__
#define __EPDB_RECENT_HPP__

#include "app_defs.h"

const int EPDB_MAX_RECENT_FILES = 10;

struct EPDBRecentFile
{
  wxString path;
  wxString method;      // password, key-file, rsa-key or smartcard
  wxString key_file;    // Key file or private RSA key file name
  wxString username;    // RSA key or smart card username
  wxString slot;        // Smart card slot
  wxString cert_id;     // Smart card cert ID
};

void EPDBRecentLoad(std::vector<EPDBRecentFile> &files);
int EPDBRecentFind(const wxString &path, EPDBRecentFile &out);
// Moves the file to the top of the list
void EPDBRecentRemember(const EPDBRecentFile &file);

#endif // __EPDB_RECENT_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
