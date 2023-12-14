// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2023 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
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

Encrypted password database auth tool 
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#if defined (__USE_ANSI_CPP__) // Use the ANSI Standard C++ library
#include <iostream>
using namespace std; // Use unqualified names for Standard C++ library
#else // Use the old iostream library by default
#include <iostream.h>
#endif // __USE_ANSI_CPP__

#include "db_auth.h"

#ifdef __MSVC_DEBUG__
#include "leaktest.h"
#endif

#include "gxdbase.h"
#include "gxdstats.h"
#include "memblock.h"
#include "gxlist.h"


int ERROR_LEVEL = 0;
gxString executable_name;

int main(int argc, char **argv)
{
  executable_name = argv[0];

  if(argc < 2) {
    cout << "ERROR: You must supply a file name" << "\n";
    return 1;
  }
  gxString fname;
  gxDatabaseError err;

  fname = argv[1];

  gxDatabase *f = new gxDatabase;
  if(!f) {
    cout << "ERROR: Cannot create database object" << "\n";
    return 1;
  }

  cout << "Opening database file " << fname.c_str() << "\n";
  
  err = f->Open(fname.c_str(),  gxDBASE_READWRITE);
  if(err != gxDBASE_NO_ERROR) {
    cout << "ERROR: " << gxDatabaseExceptionMessage(err) << "\n";
    delete f;
    return 1;
  }

  DatabaseStats(f);
  
  
  return ERROR_LEVEL;
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
