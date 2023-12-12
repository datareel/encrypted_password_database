// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
// ----------------------------------------------------------- // 
// ---------- Include File Description and Details  ---------- // 
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

Global macro definitions overrides for the given project.
*/
// ----------------------------------------------------------- //   
#ifndef __APP_DEFAULT_MACROS_HPP__
#define __APP_DEFAULT_MACROS_HPP__

#include "app_defs.h"

// Always Enable application debug code
//#ifndef __APP_DEBUG_VERSION__
//#define __APP_DEBUG_VERSION__
//#endif
// Always Disable application debug code
//#ifdef __APP_DEBUG_VERSION__
//#undef __APP_DEBUG_VERSION__
//#endif

// Disable Beta version options 
#ifdef __BETA_VERSION__
#undef __BETA_VERSION__
#endif

// Disable clipboard options
#ifdef __USE_CLIPBOARD_FUNCTIONS__
#undef __USE_CLIPBOARD_FUNCTIONS__
#endif

// Disable MSW Style help
#ifdef __USE_ONLINE_HELP__
#undef __USE_ONLINE_HELP__
#endif

// Enable MSW Style printing (Windows and Linux)
#ifndef __USE_MSW_PRINTING__
#define __USE_MSW_PRINTING__
#endif

// Enable Database encryption
#ifndef __USE_DB_ENCRYPTION__
#define __USE_DB_ENCRYPTION__
#endif

// Disable Postscript printing options
#ifdef  __USE_APP_POSTSCRIPT_PRINTING__
#undef  __USE_APP_POSTSCRIPT_PRINTING__
#endif

// Disable HTML printing
#ifdef __USE_APP_TEXT_PRINTING__ 
#undef __USE_APP_TEXT_PRINTING__ 
#endif

// Disable text printing
#ifdef __USE_APP_HTM_PRINTING__
#undef __USE_APP_HTM_PRINTING__
#endif

#endif  // __APP_DEFAULT_MACROS_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
