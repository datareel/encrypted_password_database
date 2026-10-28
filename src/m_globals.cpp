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

This file contains global definitions for the given project. It
was created as an add-on to centralize all the global elements
used in throughout the source code.

NOTE: This file is indended to be used for elements that are
external to the wxWindow library.
*/
// ----------------------------------------------------------- //   
#include "m_globals.h"

// --------------------------------------------------------------
// External global variable initialization
// --------------------------------------------------------------

// --------------------------------------------------------------

// --------------------------------------------------------------
// Data structure member definitions
// --------------------------------------------------------------

// --------------------------------------------------------------


// --------------------------------------------------------------
// Standalone Functions
// --------------------------------------------------------------
#if defined (__UNIX__)
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#if defined (__LINUX__)
#include <sys/prctl.h>
#endif
#else
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

void SecureProcessSetup()
// 10/25/2026: Called at program start. Files and directories this
// program creates (exports, backups, config files, databases) can only
// be read by the user, and the process does not write core dumps, which
// would hold decrypted passwords and keys.
{
#if defined (__UNIX__)
  umask(077);
  struct rlimit rl;
  rl.rlim_cur = rl.rlim_max = 0;
  setrlimit(RLIMIT_CORE, &rl);
#if defined (__LINUX__)
  prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);
#endif
#endif
}

void SecureWipe(gxString &s)
// 10/25/2026: Overwrite and clear a string that held a secret
{
  if(s.length() > 0) OPENSSL_cleanse((void *)s.GetSPtr(), s.length());
  s.Clear(1);
}

// --------------------------------------------------------------

int IsHyperlink(const gxString &s)
// 10/25/2026: Only http and https URLs are links. Any text with "://"
// or "mailto:", UNC paths and Windows paths were links, and opening
// one could run a program from the database contents.
{
  gxString sbuf(s);
  sbuf.TrimLeading(' ');
  if(sbuf.length() < 8) return 0;
  if(CaseICmp(gxString(sbuf.c_str(), 7), "http://") == 0) return sbuf.length() > 7;
  if(CaseICmp(gxString(sbuf.c_str(), 8), "https://") == 0) return sbuf.length() > 8;
  return 0;
}


int CSVFormulaPrefixLength(const gxString &value)
// 10/25/2026: Returns the number of leading quote characters if the
// value, after them, starts with a character spreadsheets read as a
// formula (= + - @), or -1 if it does not.
{
  unsigned i = 0;
  while(i < value.length() && value[i] == '\'') i++;
  if(i >= value.length()) return -1;
  char c = value[i];
  if(c == '=' || c == '+' || c == '-' || c == '@') return (int)i;
  return -1;
}

void EscapeCSVFormula(gxString &value)
// 10/25/2026: Prefix a quote so spreadsheets show the value as text
// instead of running it as a formula. Values that already start with
// quotes before a formula character get one more, so the import can
// remove exactly one and get the stored value back.
{
  if(CSVFormulaPrefixLength(value) >= 0) value.InsertAt(0, "'");
}

void UnescapeCSVFormula(gxString &value)
// 10/25/2026: Undo EscapeCSVFormula() when importing
{
  if(CSVFormulaPrefixLength(value) > 0) value.DeleteAt(0, 1);
}

void AppendCSVField(gxString &out, const gxString &value, char delimiter, char filter)
// 10/25/2026: Append one export field. The filter character is removed,
// formulas are escaped and fields with the delimiter are quoted.
// 10/27/2026: Line breaks are kept: a field with the delimiter, a double
// quote or a line break is quoted with its quotes doubled (RFC 4180, as
// spreadsheets read it). CR LF is written as LF.
{
  gxString v(value);
  if(filter) v.FilterChar(filter);
  v.ReplaceString("\r\n", "\n");
  v.ReplaceChar('\r', '\n');
  EscapeCSVFormula(v);
  gxString d;
  d << delimiter;
  if((v.Find(d) != -1) || (v.Find("\"") != -1) || (v.Find("\n") != -1)) {
    v.ReplaceString("\"", "\"\"");
    out << "\"" << v << "\"";
  }
  else out << v;
  SecureWipe(v);
}

int ParseCSVRecord(const gxString &rec, gxString *fields, unsigned max_fields, unsigned &num,
		   int &open, int &rfc)
// 10/27/2026: Splits one CSV record, which can span lines. A quote at the
// start of a field opens a quoted field, "" in it is one quote, and a
// quote before a comma or the end of the record closes it. Any other
// quote is kept, as files written before quotes were doubled have them.
// Unquoted fields have their leading and trailing spaces removed.
// open is set if the record ends inside a quoted field, rfc if a quoted
// field has a line break or a doubled quote. Returns 0, or -1 if the
// record ends inside a quoted field.
{
  const char *p = rec.c_str();
  unsigned len = rec.length(), pos = 0;
  num = 0;
  open = 0;
  rfc = 0;
  for(;;) {
    gxString f;
    if(pos < len && p[pos] == '"') {
      pos++;
      int closed = 0;
      while(pos < len) {
	if(p[pos] == '"') {
	  if(pos + 1 < len && p[pos+1] == '"') { f << '"'; pos += 2; rfc = 1; continue; }
	  if(pos + 1 >= len || p[pos+1] == ',') { pos++; closed = 1; break; }
	  f << '"'; pos++; // A quote inside an old style quoted field
	  continue;
	}
	if(p[pos] == '\n') rfc = 1;
	f << p[pos++];
      }
      if(!closed) {
	open = 1;
	SecureWipe(f);
	return -1;
      }
    }
    else {
      while(pos < len && p[pos] != ',') f << p[pos++];
      f.TrimLeadingSpaces();
      f.TrimTrailingSpaces();
    }
    if(num < max_fields) fields[num] = f;
    num++;
    SecureWipe(f);
    if(pos < len && p[pos] == ',') { pos++; continue; }
    break;
  }
  return 0;
}

int WritePrivateFile(const char *fname, const char *data, unsigned len, gxString &err)
// 10/25/2026: Write data to a new file only the user can read and
// write. An existing file is removed first (the file dialog asked to
// overwrite it) and a symbolic link is not followed. On failure no file
// is left behind. Returns 0 if successful.
{
  err.Clear();
#if defined (__UNIX__)
  struct stat st;
  if(lstat(fname, &st) == 0) { // Also removes a symbolic link, not its target
    if(unlink(fname) != 0) {
      err << clear << "Error overwriting existing file " << fname;
      return 1;
    }
  }
#else
  if(futils_exists(fname)) {
    if(futils_remove(fname) != 0) {
      err << clear << "Error overwriting existing file " << fname;
      return 1;
    }
  }
#endif
#if defined (__UNIX__)
  int fd = open(fname, O_WRONLY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC, 0600);
#else
  int fd = _open(fname, _O_WRONLY|_O_CREAT|_O_EXCL|_O_BINARY, _S_IREAD|_S_IWRITE);
#endif
  if(fd < 0) {
    err << clear << "Error creating file " << fname;
    return 1;
  }
  unsigned written = 0;
  while(written < len) {
#if defined (__UNIX__)
    ssize_t n = write(fd, data+written, len-written);
#else
    int n = _write(fd, data+written, len-written);
#endif
    if(n <= 0) break;
    written += (unsigned)n;
  }
#if defined (__UNIX__)
  int sync_ok = (fsync(fd) == 0);
  int close_ok = (close(fd) == 0);
#else
  int sync_ok = 1;
  int close_ok = (_close(fd) == 0);
#endif
  if(written != len || !sync_ok || !close_ok) {
    futils_remove(fname);
    err << clear << "Error writing file " << fname;
    return 1;
  }
  return 0;
}

int CheckPrivateKeyFile(const char *fname, gxString &err)
// 10/25/2026: A private RSA key or AES key file must be a regular file
// owned by the user that other users cannot read or write, like ssh
// requires for private keys. A symbolic link is checked by its target.
// Returns 0 if the file is good, or 1 with err set.
{
  err.Clear();
#if defined (__UNIX__)
  struct stat st;
  if(stat(fname, &st) != 0) {
    err << clear << "Cannot read key file " << fname;
    return 1;
  }
  if(!S_ISREG(st.st_mode)) {
    err << clear << "Key file " << fname << " is not a regular file";
    return 1;
  }
  if(st.st_uid != geteuid()) {
    err << clear << "Key file " << fname << " is not owned by the current user";
    return 1;
  }
  if(st.st_mode & (S_IRWXG|S_IRWXO)) {
    err << clear << "Key file " << fname << " can be read or written by other users, run chmod 600 " << fname;
    return 1;
  }
#endif
  return 0;
}

gxString ReplaceFileExtension(const gxString &fname, const char *ext)
// 10/25/2026: Replace the extension of the file name part of a path.
// Paths used to be cut at the last "." anywhere, so "dir.d/mydb" opened
// "dir.ehd" instead of "dir.d/mydb.ehd".
{
  gxString path(fname);
  int sep = -1;
  for(unsigned i = 0; i < path.length(); i++) {
    if(path[i] == '/' || path[i] == '\\') sep = (int)i;
  }
  int dot = -1;
  for(unsigned i = (unsigned)(sep+1); i < path.length(); i++) {
    if(path[i] == '.') dot = (int)i;
  }
  if(dot > sep + 1) path.DeleteAt((unsigned)dot, path.length() - (unsigned)dot);
  path << ext;
  return path;
}

// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
