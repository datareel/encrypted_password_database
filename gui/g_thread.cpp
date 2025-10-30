// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
// Copyright (c) 2001-2025 DataReel Software Development
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

Thread safe classes and functions designed for use in 
multithreaded GUI applications.
*/
// ----------------------------------------------------------- // 
#include "app_defs.h"

GUIOStream gui_output_stream;
GUIOStream *gui_stream = &gui_output_stream;
GUISerialLock gui_serial_lock;
GUISerialLock *gui_lock = &gui_serial_lock;

int GUISerialLock::Lock() 
// Function used to lock a GUI object with in a running thread to prevent 
// multiple threads from accessing the same object. Returns a true if
// the object was locked or false if the a lock could not be obtained
// for the object.
{
#if defined(__WXGTK__) || defined(__WXMOTIF__)
  // ********** Critical Section ******************* //
  num_locks++;
  wxMutexGuiEnter();
  // ********** Critical Section ******************* //
#endif
  return 1;
}

int GUISerialLock::Unlock()
// Function used to unlock a GUI object after a running thread no longer 
// needs to access the object. Returns a true if the object was unlocked 
// or false if the object could not be unlocked.
{
#if defined(__WXGTK__) || defined(__WXMOTIF__)
  if(num_locks > 0) num_locks--;
  wxMutexGuiLeave();  
#endif
  return 1;
}

GUIOStream& GUIOStream::operator<<(GUIOStream & (*_f)(GUIOStream&))
{
  (*_f)(*(this));
  return *(this);
}

void GUIOStream::WriteTSText() 
// Thread safe write function used only within running 
// threads to write a thread save message using a GUI 
// object.
{
  if(!stream) return;
  if(!gui_lock->Lock()) return; 
  *(stream) << sbuf.c_str(); 
  gui_lock->Unlock();
}

void GUIOStream::WriteText() 
// Unlocked write fucntion used outside of running threads. 
{
  if(!stream) return;
  *(stream) << sbuf.c_str(); 
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //


