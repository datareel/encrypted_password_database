// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
// Copyright (c) 1999-2023 DataReel Software Development
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

Database fixed string class that can be used with encrypted strings.
*/
// ----------------------------------------------------------- //   
#include "m_globals.h"
#include "dbstring.h"

// Initialize DBString Global Variables
char DBStringNULLPtr::DBStringNUllChar = '\0';
char DBStringNULLPtr::DBStringNUllStr[1] = { '\0' };
int DBStringConfig::DBStringCaseCompare = 1;
char DBStringConfig::mode = 3; // Use 256-bit encryption by default
int DBStringConfig::AES_error_level = 0;
MemoryBuffer DBStringConfig::crypt_key;

DBStringConfig::DBStringConfig() 
{
  
}

DBStringConfig::~DBStringConfig()
{

}

void DBString::Clear()
{
  AES_fillrand((unsigned char *)sptr, DBStringLength);
  sptr[0] = 0;
}

char *DBString::GetString() const
// Returns a null terminated C string or a null pointer.
{
  DBStringConfig::AES_error_level = 0;
  
  if(sptr[0] == 0) return DBStringNULLPtr::DBStringNUllStr;

  // The sptr pointer cannot be modified during
  // a DBString::Copy() call.
  char *dup = new char[DBStringLength];
  memmove(dup, sptr, DBStringLength);
  

  int crypt_error;
  CryptoHeader eh;
  
  // Get the length of the encrytpted buffer
  memmove(&eh, dup, sizeof(eh));
  unsigned cryptLen = eh.crypt_len;
  
  // Realign the encrypted buffer
  memmove(dup, (dup+sizeof(eh)), cryptLen);
  
  // Decrypt the compressed string
  crypt_error = AES_Decrypt(dup, &cryptLen, 
			    (const unsigned char*)DBStringConfig::crypt_key.m_buf(), 
			    DBStringConfig::crypt_key.length());
  if(crypt_error != AES_NO_ERROR) {
    DBStringConfig::AES_error_level = crypt_error; 
    delete[] dup;
    dup = 0;
    return 0;
  }
    
  Bytef *dest = 0;;
  uLong destLen;  
  
  // Retrieve the compression header
  CompressHeader ch;
  memmove(&ch, dup, sizeof(ch));

  // Uncompress the string
  dest = UnCompressString(dest, 
			  &destLen, 
			  (const Bytef *)(dup+sizeof(ch)), 
			  (uLong)ch.compress_len);

  // Free the duplicate sptr string
  delete[] dup;
  dup = 0;

  if(!dest) return 0;
  
  // Null terminate the string
  dest[destLen] = 0;

  return (char *)dest;
}

int DBString::SetString(const char *s, unsigned bytes)
// Set the string value for this object. This function will truncate 
// the number of bytes requested if the number of bytes exceeds maximum
// fixed string length. Returns true if successful or false if the 
// string was truncated. NOTE: The DBString class guarantees that each
// string object is unique by storing a unique copy of the string with
// each object. This ensures that DBString objects can be safely copy
// constructed, assigned, resized, and deleted by multiple threads.
// Multiple threads accessing shared memory segments must be handled
// by the application.
{
  if(!s) { // Check for null pointers
    return 0; 
  }

  DBStringConfig::AES_error_level = 0;  

  // Calculate the length of this string if no bytes size is specified
  if(bytes == 0) bytes = strlen(s);
  int truncate = 0;  

  Clear(); // Reset the current string in case compression or encryption fails
  Bytef *dest = 0;
  uLong destLen;
  
  // Compress the string
  dest = CompressString(dest, &destLen, (Bytef *)s, (uLong)bytes);
  if(!dest) return 0;
  
  // The compressed string will not fit so we have to truncate it
  if(destLen > (DBStringLength-cytpro_overhead)) {
    bytes = (DBStringLength-cytpro_overhead);
    truncate = 1;
    if(dest) delete[] dest;
    dest = CompressString(dest, &destLen, (Bytef *)s, (uLong)bytes);
    if(!dest) {
      Clear(); // Null the entire string
      return 0;
    }
  }

  // The string will not fit
  if(destLen > (DBStringLength-cytpro_overhead)) {
    Clear(); // Null the entire string
    if(dest) delete[] dest;
    dest = 0;
    return 0;
  }

  if(!dest) return 0;

  // Store the compression header and set the compressed string
  CompressHeader ch(destLen);

  // Copy the compression header and realign the dest buffer
  memmove((dest+sizeof(ch)), dest, destLen);
  memmove(dest, &ch, sizeof(ch));

  // Encrypt the string
  unsigned cryptLen = destLen+sizeof(ch);
  int crypt_error;
  crypt_error = AES_Encrypt((char *)dest, &cryptLen, 
			    (const unsigned char*)DBStringConfig::crypt_key.m_buf(), 
			    DBStringConfig::crypt_key.length(), 
			    DBStringConfig::mode);
  if(crypt_error != AES_NO_ERROR) {
    DBStringConfig::AES_error_level = crypt_error;
    if(dest) delete[] dest;
    dest = 0;
    return 0;
  }

  // Store the compression header and encrypted string
  CryptoHeader eh(cryptLen);
  memmove(sptr, &eh, sizeof(eh));
  memmove((sptr+sizeof(eh)), dest, cryptLen);

  delete[] dest;
  dest = 0;

  // Fill the remaining bytes with random characters
  if(cryptLen < DBStringLength) {
    unsigned bytes_left = DBStringLength - (cryptLen+sizeof(eh));
    AES_fillrand((unsigned char *)(sptr+(cryptLen+sizeof(eh))), bytes_left);
  }
  return truncate == 0;
}

void DBString::Copy(const DBString &s)
// Function used to copy DBString objects.
{
  Clear();
  char *dest = (char *)s.GetString();
  if(!dest) return;
  if(dest[0] == 0) return;
  SetString(dest);
  delete dest;
}

GXDLCODE_API int Compare(const DBString &a, const DBString &b)
{
  int rv;
  char *dest1 = (char *)a.GetString();
  if(!dest1) return 0;
  if(dest1[0] == 0) return 0;
  char *dest2 = (char *)b.GetString();
  if(!dest2) {
    delete[] dest1;
    return 0;
  }
  if(dest2[0] == 0) {
    delete[] dest1;
    return 0;
  }
  rv = StringANCompare(dest1, dest2, DBStringConfig::DBStringCaseCompare);
  delete[] dest1;
  delete[] dest2;
  return rv;
}

GXDLCODE_API int operator==(const DBString &a, const DBString &b) 
{
  return Compare(a, b) == 0;
}

GXDLCODE_API int operator<(const DBString &a, const DBString &b) 
{
  return Compare(a, b) < 0;
}

GXDLCODE_API int operator>(const DBString &a, const DBString &b) 
{
  return Compare(a, b) > 0;
}

GXDLCODE_API int operator!=(const DBString &a, const DBString &b) 
{
  return Compare(a, b) != 0;
}

unsigned DBString::length() 
{ 
  char *dest = GetString();
  if(!dest) return 0;
  if(dest[0] == 0) return 0;
  int len = strlen(dest);
  delete[] dest;
  return len;
}

char *DBString::c_str(char *sbuf) 
// Returns a null terminated string representing this object's
// ASCII value. NOTE: This function assumes that the proper
// amount of memory has been allocated for the "sbuf" string
// buffer. The string buffer "sbuf" is redundantly returned by
// this function.
{ 
  if(!sbuf) return DBStringNULLPtr::DBStringNUllStr;
  strcpy(sbuf, DBStringNULLPtr::DBStringNUllStr); // Clear the output buffer

  char *dest = GetString();
  if(!dest) {
    strcpy(sbuf, DBStringNULLPtr::DBStringNUllStr);
    return sbuf;
  }
  if(dest[0] == 0) {
    strcpy(sbuf, DBStringNULLPtr::DBStringNUllStr);
    return sbuf;
  }
  strcpy(sbuf, dest);
  delete[] dest;

  return sbuf; 
}

int DBString::is_null() 
{ 
  char *dest = GetString();
  if(!dest) return 1;
  return dest[0] == 0; 
} 

int DBString::is_not_null() 
{ 
  return is_null() != 1;
}
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
