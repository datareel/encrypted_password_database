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

This file contains global definitions for the given project. It
was created as an add-on to centralize all the global elements
used in throughout the source code.

NOTE: This file is indended to be used for elements that are
external to the wxWindow library.
*/
// ----------------------------------------------------------- //   
#ifndef __M_GLOBALS_HPP__
#define __M_GLOBALS_HPP__

// --------------------------------------------------------------
// Conditional Directives
// --------------------------------------------------------------

// --------------------------------------------------------------

// --------------------------------------------------------------
// Include Files
// --------------------------------------------------------------
// DataReel include files
#include "gxdlcode.h"

#include "gxthread.h"
#include "gxcond.h"
#include "gxmutex.h"
#include "ehandler.h"
#include "gxconfig.h"
#include "gxstring.h"
#include "logfile.h"
#include "futils.h"
#include "dfileb.h"
#include "systime.h"
#include "gxdstats.h"
#include "gxbstree.h"
#include "bstreei.h"
#include "infohog.h"
#include "strutil.h"

// ZLIB include files
#include <zlib.h>

// Fcrypt include files
#include "cryptdb.h"
#include "aesdb.h"
#include "rsadb.h"
#include "smart_card.h"

#include "db_auth.h"
#include "dbstring.h"
#include "m_compress.h"
#include "m_dbase.h"
#include "as2print.h"

const unsigned DB_AUTH_STATIC_AREA_SIZE = 65536;

// --------------------------------------------------------------

// --------------------------------------------------------------
// Type Definitions
// --------------------------------------------------------------

// --------------------------------------------------------------

// --------------------------------------------------------------
// Constants
// --------------------------------------------------------------
// Thread constants
const int DISPLAY_THREAD_RETRIES = 255;
const int LOGFILE_THREAD_RETRIES = 255;
const int DATABASE_THREAD_RETRIES = 255;
const int BITMAP_THREAD_RETRIES = 255;
const int METER_THREAD_RETRIES = 255;
const int STATBAR_THREAD_RETRIES = 255;
const int MAX_PROGRESS_METER_COUNT = 11;
// --------------------------------------------------------------

// --------------------------------------------------------------
// Enumerations
// --------------------------------------------------------------

// --------------------------------------------------------------

// --------------------------------------------------------------
// Data Structures
// --------------------------------------------------------------

// --------------------------------------------------------------

// --------------------------------------------------------------
// Standalone Functions
// --------------------------------------------------------------

// --------------------------------------------------------------

#endif  // __M_GLOBALS_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
