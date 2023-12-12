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

Cryptography code  
*/
// ----------------------------------------------------------- //   
#ifndef __M_CRYPTO_HPP__
#define __M_CRYPTO_HPP__

#include "app_defs.h"

#ifdef __USE_DB_ENCRYPTION__

// String compression functions
GXDLCODE_API uLong CalcStringCompressLen(uLong len);
GXDLCODE_API Bytef *CompressString(Bytef *dest, uLong *destLen, 
				   const Bytef *s, uLong bytes);
GXDLCODE_API Bytef *UnCompressString(Bytef *dest, uLong *destLen, 
				     const Bytef *s, uLong bytes);

#endif // __USE_DB_ENCRYPTION__

#endif // __M_CRYPTO_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //


