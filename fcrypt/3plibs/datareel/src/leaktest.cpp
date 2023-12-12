// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File Name: leaktest.cpp
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 11/09/2000 
// Date Last Modified: 06/17/2016
// Copyright (c) 2001-2016 DataReel Software Development
// ----------------------------------------------------------- // 
// ------------- Program Description and Details ------------- // 
// ----------------------------------------------------------- // 
/*
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
 
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  
USA

Code used to monitor memory leaks within an MSVC application.
This program was taken from the Visual C++ Debug Routines
example program. The leak test functions will print an error
message to the console if the end of the application attempts to
overwrite the end of an allocated buffer or if any memory leaks
occur. To use this code the InitLeakTest() function must be called
in the program's main thread of execution. Additionally the
program must be compiled with the /MDd compiler flag the /MTd
compiler flag for multi-threaded applications.
*/
// ----------------------------------------------------------- //  
#include "gxdlcode.h"

#include "leaktest.h"

#ifdef __BCC32__
#pragma warn -8057
#endif

#if defined(__MSVC_DEBUG__)
GXDLCODE_API void OutputHeading(const char * explanation)
// Debug output message heading function.
// Ignore BCC32 warning 8057 explanation is never used
{


  _RPT1(_CRT_WARN, "\n\n%s:\n**************************************\
************************************\n", explanation);
}

GXDLCODE_API void InitLeakTest()
// Initialize debug reporting functions to monitor memory leaks.
{
  // Send all reports to STDOUT
  // NOTE: The _CrtSetReportMode() function is used to define the
  // destination(s) for a specific report type generated by the
  // _CrtDbgReport() function:
  // _CRTDBG_MODE_DEBUG - Writes the message to an output debug string. 
  // _CRTDBG_MODE_FILE - Writes the message to a user-supplied file handle.
  // _CRTDBG_MODE_WNDW - Creates a message box to display the message.
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

  // NOTE: The follow code can be used to send the failures to to both a
  // debug message window and to the stdout or stderr:
  // _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW);
  // _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

  // Retrive the current debug flag settings.
  int debug_flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

  // Set the debug flags
  // _CRTDBG_CHECK_ALWAYS_DF - On: Call _CrtCheckMemory at every allocation and
  // deallocation request.
  // _CRTDBG_LEAK_CHECK_DF - On: Perform automatic leak checking at program
  // exit via a call to _CrtDumpMemoryLeaks
  // _CRTDBG_DELAY_FREE_MEM_DF - On: Keep freed memory blocks in the heap's
  // linked list, assign them the _FREE_BLOCK type, and fill them with the
  // byte value 0xDD
  debug_flag |= _CRTDBG_CHECK_ALWAYS_DF;   // ON
  debug_flag |= _CRTDBG_LEAK_CHECK_DF;     // ON
  debug_flag |= _CRTDBG_DELAY_FREE_MEM_DF; // ON

  // NOTE: Use the following code to turn a debug flag off:
  // debug_flag &= ~_CRTDBG_CHECK_ALWAYS_DF;
  
  _CrtSetDbgFlag(debug_flag);
}

#endif // __MSVC_DEBUG__

#ifdef __BCC32__
#pragma warn .8057
#endif
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

