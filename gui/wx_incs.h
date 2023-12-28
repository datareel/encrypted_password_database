// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2024 DataReel Software Development
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

Include files need to compile with the wxWindows GUI library 
version 2.X
*/
// ----------------------------------------------------------- //   
#ifndef __WX_INCS_HPP__
#define __WX_INCS_HPP__

#include "app_defs.h"

// --------------------------------------------------------------
// wxWindows 2.X Headers
// --------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "wxappfw.cpp"
    #pragma interface "wxappfw.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// Standard wxWindows headers for all platforms
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <wx/toolbar.h>
#include <wx/grid.h>
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/grid.h"
#include "wx/generic/gridctrl.h"
#include <wx/image.h>
#include <wx/clipbrd.h>
#include <wx/notebook.h>
#include <wx/dcps.h>

// MDI includes
#include <wx/mdi.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/docview.h>
#include <wx/cmdproc.h>
#include <wx/docview.h>
#include <wx/txtstrm.h>

// define this to 1 to use wxToolBarSimple instead of the native one
#define USE_GENERIC_TBAR 0

// Define this to use XPMs everywhere (by default, BMPs are used under Win)
// BMPs use less space, but aren't compiled into the executable on other platforms
#ifdef __WXMSW__
#define USE_XPM_BITMAPS 0
#else
#define USE_XPM_BITMAPS 1
#endif

#if USE_GENERIC_TBAR
#if !wxUSE_TOOLBAR_SIMPLE
#error wxToolBarSimple is not compiled in, set wxUSE_TOOLBAR_SIMPLE \
               to 1 in setup.h and recompile the library.
#else
#include <wx/tbarsmpl.h>
#endif
#endif // USE_GENERIC_TBAR

#if USE_XPM_BITMAPS && defined(__WXMSW__) && !wxUSE_XPM_IN_MSW
#error You need to enable XPM support to use XPM bitmaps with toolbar!
#endif // USE_XPM_BITMAPS

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

// --------------------------------------------------------------

#endif  // __WX_INCS_HPP__ 
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
