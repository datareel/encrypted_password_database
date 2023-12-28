// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 09/20/1999
// Date Last Modified: 12/10/2023
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

Compression code  
*/
// ----------------------------------------------------------- // 
#include "m_globals.h"
#include "m_compress.h"

GXDLCODE_API uLong CalcStringCompressLen(uLong len)
{
  return ((len * 2) + (12+128));
  // NOTE: The following is from the zlib documentation
  // return (((.1/100 * len) * len) +12);
}

GXDLCODE_API Bytef *CompressString(Bytef *dest, uLong *destLen, 
				   const Bytef *s, uLong bytes)
{
  int compress_len = CalcStringCompressLen(bytes);
  dest = new Bytef[compress_len];
  *(destLen) = compress_len;
  memset(dest, 0, *(destLen));

  // The compression level must be Z_DEFAULT_COMPRESSION, 
  // or between 0 and 9: 1 gives best speed, 9 gives best compression, 
  // 0 gives no compression at all (the input data is simply copied a 
  // block at a time).
  int level = 1;
  int rv = compress2(dest, destLen, (const Bytef *)s, (uLong)bytes, level);
  int has_error = 0;
  if(rv != Z_OK) {
    // Try to recover from the compression error
    switch(rv) {
      case Z_MEM_ERROR:
	// There was not enough memory
	if(dest) delete[] dest;
	compress_len = CalcStringCompressLen((bytes+(bytes/2)));
	dest = new Bytef[compress_len];
	*(destLen) = compress_len;
	memset(dest, 0, *(destLen));
	if(compress2(dest, destLen, (const Bytef *)s, (uLong)bytes, level) != Z_OK) {
	  has_error = 1;
	}
	break;
      case Z_BUF_ERROR:
	// There was not enough room in the output buffer
	if(dest) delete[] dest;
	compress_len = CalcStringCompressLen(bytes*2);
	dest = new Bytef[compress_len];
	*(destLen) = compress_len;
	memset(dest, 0, *(destLen));
	if(compress2(dest, destLen, (const Bytef *)s, (uLong)bytes, level) != Z_OK) {
	  has_error = 1;
	}
	break;
      case Z_STREAM_ERROR:
	// The level parameter is invalid.  
	has_error = 1;
	break;
      default: 
	has_error = 1;
	break;
    }
  }

  if(has_error) { // Could not recover from compression error
    if(dest) delete[] dest;
    dest = 0;
  }
  return dest;
} 

GXDLCODE_API Bytef *UnCompressString(Bytef *dest, uLong *destLen, 
				     const Bytef *s, uLong bytes)
{
  dest = new Bytef[CalcStringCompressLen(bytes)];
  *(destLen) = CalcStringCompressLen(bytes);

  int rv = uncompress(dest, destLen, (const Bytef *)s, (uLong)bytes);
  int has_error = 0;
  if(rv != Z_OK) {
    // Try to recover from the compression error
    switch(rv) {
      case Z_MEM_ERROR:
	// There was not enough memory
	if(dest) delete[] dest;
	dest = new Bytef[CalcStringCompressLen((bytes+(bytes/2)))];
	*(destLen) = CalcStringCompressLen((bytes+(bytes/2)));
	if(uncompress(dest, destLen, (const Bytef *)s, (uLong)bytes) != Z_OK) {
	  has_error = 1;
	}
	break;
      case Z_BUF_ERROR:
	// There was not enough room in the output buffer
	if(dest) delete[] dest;
	dest = new Bytef[CalcStringCompressLen(bytes*2)];
	*(destLen) = CalcStringCompressLen(bytes*2);
	if(uncompress(dest, destLen, (const Bytef *)s, (uLong)bytes) != Z_OK) {
	  has_error = 1;
	}
	break;
      case Z_STREAM_ERROR:
	// The level parameter is invalid.  
	has_error = 1;
	break;
      default: 
	has_error = 1;
	break;
    }
  }

  if(has_error) { // Could not recover from compression error
    if(dest) delete[] dest;
    dest = 0;
  }
  return dest;
} 
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //

