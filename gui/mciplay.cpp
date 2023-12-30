// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2024 DataReel Software Development
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

WIN32/UNIX function used to play multi-media files.

Under WIN32 this function  uses the Windows Media Control 
Interface function to play multi-media files.

Required WIN32 Libraries:
user32.lib
winmm.lib
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

#include "wx/process.h"

void MCIPlay(const gxString &fname)
{
#if defined (__WIN32__)
  gxString sbuf;
  sbuf << clear << "OPEN " << fname << " ALIAS MM_FILE";
  mciSendString(sbuf.c_str(), NULL, 0, NULL);
  mciSendString("PLAY MM_FILE WAIT", NULL, 0, NULL);
  mciSendString("CLOSE MM_FILE", NULL, 0, NULL);
#elif defined (__LINUX__)
  gxString command("play");
  command << " " << fname;
  wxExecute(command.c_str());
#endif
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

