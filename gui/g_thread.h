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

Thread safe classes and functions designed for use in 
multithreaded GUI applications.

NOTE: On UNIX systems threads are only supported under X11R6 and higher.
All multithreaded applications must use GUI lock inside of any thread 
running that manipulates a GUI object. The GUI lock ensures that the 
application's thread will not perform any multithreaded operation at 
the same time the GUI is performing a multithreaded operation. "Xlib: 
unexpected async reply" errors will be generated if the application 
makes any X calls from more then one thread at a time. Within wxWindows 
always use the wxMutexGuiEnter() and wxMutexGuiLeave() functions to 
ensure that only one thread at time can access this GUI object.

NOTE: If a thread is not manipulating any GUI objects a GUI lock should 
not be use. All calls made to any function using wxMutexGuiEnter() and 
wxMutexGuiLeave() must be made within a running thread or the application
will hang.

NOTE: It is not safe to call the ::wxYield() function within a 
running thread under GTK but has no affect under WIN32. If yield is
called the application will hang with the following error message:
GLib-WARNING **: g_main_iterate(): main loop already active in another thread

NOTE: A g_main_iterate() error will occur under GTK within a running 
thread if a dialog box is opened in the thread execute function or 
any other function called by the running thread.
*/
// ----------------------------------------------------------- // 
#ifndef __G_THREAD_HPP__
#define __G_THREAD_HPP__ 

#include "app_defs.h"

class GUISerialLock
{
public:
  GUISerialLock() { num_locks = 0; }
  ~GUISerialLock() { if(num_locks) Unlock(); }

public:
  int Lock();
  int Unlock();
  int IsLocked() { return num_locks > 0; }
  int NumLocks() { return num_locks; }

public: // Overloaded operators
  int operator!() const { return num_locks == 0; }
  int operator!() { return num_locks == 0; }
  operator const int () const { return num_locks > 0; }
  operator int () { return num_locks > 0; }

private:
  int num_locks;    // Total number of GUI locks
};

class GUIOStream
{
public:
  GUIOStream() { stream = 0; }
  ~GUIOStream() { }

public:
  void WriteTSText();
  void WriteText();
  void SetStream(wxTextCtrl *s) { stream = s; }
  wxTextCtrl *GetStream() { return stream; }
  UString *GetSBuf() { return &sbuf; }

public:
  GUIOStream &operator<<(GUIOStream & (*_f)(GUIOStream&));
  GUIOStream &operator<<(const UString &s) { 
    sbuf << s; return *this; 
  }
  GUIOStream &operator<<(const wxString &s) { 
    sbuf << s.wc_str(); return *this; 
  }
  GUIOStream &operator<<(const char *s) {
    sbuf << s; return *this;
  }
  GUIOStream &operator<<(const unsigned char *s) { 
    sbuf << (const char *)s; return *this;
  }
  GUIOStream &operator<<(const char c) {
    sbuf << c; return *this;
  }
  GUIOStream &operator<<(const unsigned char c) { 
    sbuf << (const char)c; return *this;
  }
  GUIOStream &operator<<(const long val) {
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const unsigned long val) {
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const int val) { 
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const unsigned int val) { 
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const short val) { 
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const unsigned short val) {
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const float val) { 
    sbuf << val; return *this;
  }
  GUIOStream &operator<<(const double val) {
    sbuf << val; return *this;
  }

private:
  wxTextCtrl *stream;
  UString sbuf;
};

inline GUIOStream &tswrite(GUIOStream &s) { s.WriteTSText(); return s; }
inline GUIOStream &write(GUIOStream &s) { s.WriteText(); return s; }
inline GUIOStream &clear(GUIOStream &s) { s.GetSBuf()->Clear(); return s; }

// Global objects
extern GUIOStream gui_output_stream;
extern GUIOStream *gui_stream;
extern GUISerialLock gui_serial_lock;
extern GUISerialLock *gui_lock;

#endif // __G_THREAD_HPP__
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
