// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/30/2023
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

Database fixed string class that can be used with encrypted strings.
*/
// ----------------------------------------------------------- //   
#ifndef __GX_DBSTRING_HPP__
#define __GX_DBSTRING_HPP__

#include "m_globals.h"

// This constant sets the string length for all DBString objects.
// 100 bytes of overhead includes space for compress header 
// and compression overhead plus crypto header and 256-bit 
// encryption overhead. 
const unsigned cytpro_overhead = 100; 
const unsigned DBStringLength = 255+cytpro_overhead; // Length plus overhead

const unsigned CryptDBHashSize = DBStringLength;
const unsigned DBBinaryChunkSize = 4096;

struct GXDLCODE_API CompressHeader
{
  CompressHeader() { compress_len = (gxUINT32)0; }
  CompressHeader(uLong len) { compress_len = (gxUINT32)len; }
  ~CompressHeader() { }
    
  gxUINT32 compress_len;
};

struct GXDLCODE_API CryptoHeader
{
  CryptoHeader() { crypt_len = (gxUINT32)0; }
  CryptoHeader(uLong len) { crypt_len = (gxUINT32)len; }
  ~CryptoHeader() { }
    
  gxUINT32 crypt_len;
};

// Null string class used to reserve address space for null strings
class GXDLCODE_API DBStringNULLPtr
{
public:
  DBStringNULLPtr() { }
  ~DBStringNULLPtr() { }

public: // Global null character and string pointers
  static char DBStringNUllChar;
  static char DBStringNUllStr[1];
};

struct GXDLCODE_API DBStringConfig
{
  DBStringConfig();
  ~DBStringConfig();

  static int DBStringCaseCompare;
  static char mode;
  static MemoryBuffer crypt_key;
  static int AES_error_level;
}; 

// Database string class 
class GXDLCODE_API DBString 
{ 
public:
  DBString() { Clear(); }
  DBString(const char *s, unsigned bytes = 0) {
    SetString(s, bytes);
  }
  ~DBString() { }    

  DBString(const DBString &s) { Copy(s); }
  DBString &operator=(const char *s) { SetString(s); return *this; }
  DBString &operator=(const gxString &s) { 
    SetString(s.c_str()); 
    return *this; 
  }
#ifdef __wxWINALL__
  DBString &operator=(const wxString &s) { 
    SetString(s.c_str()); 
    return *this; 
  }
#endif
 DBString &operator=(const DBString &s) {
    if(this != &s) Copy(s); // Prevent self assignment 
    return *this;
  }

public:
  int SetString(const char *s, unsigned bytes = 0);
  char *GetString() const; 
  int SetString(MemoryBuffer &key, int &error_level, const char *s, unsigned bytes = 0);
  char *GetString(MemoryBuffer &key, int &error_level) const; 
  void Clear();
  void Copy(const DBString &s);
  unsigned length();
  char *c_str(char *sbuf);
  int is_null();
  int is_not_null();
  char *GetSPtr() { return sptr; }

public: // Database functions
  size_t SizeOf() { return sizeof(sptr); }

public: // Overloaded operators
  GXDLCODE_API friend int Compare(const DBString &a, const DBString &b);
  GXDLCODE_API friend int operator==(const DBString &a, const DBString &b);
  GXDLCODE_API friend int operator<(const DBString &a, const DBString &b);
  GXDLCODE_API friend int operator>(const DBString &a, const DBString &b);
  GXDLCODE_API friend int operator!=(const DBString &a, const DBString &b);

private: // The DBString structure can have only one data member
  char sptr[DBStringLength]; // Encrypted string
};

#endif  // __GX_DBSTRING_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
