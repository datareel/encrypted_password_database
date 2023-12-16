// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Header File
// Compiler Used: MSVC, GCC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 12/10/2023
// Copyright (c) 2001-2023 DataReel Software Development
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

Encrypted database auth functions
*/
// ----------------------------------------------------------- //   
#ifndef __DB_AUTH_HPP__
#define __DB_AUTH_HPP__

#include "m_globals.h"

class DatabaseUserAuth
{
public:
  DatabaseUserAuth();
    ~DatabaseUserAuth();

public:
  int LoadStaticDataBlocks();  
  int UpdateStaticData();
  int WriteStaticDataArea();
  int AddRSAKeyToStaticArea(const MemoryBuffer &secret,
			    char public_key[], unsigned public_key_len,
			    const gxString &rsa_key_username, char *passphrase = 0);
  int AddSmartCardCertToStaticArea(SmartCardOB *sc, int use_cert_file,
				   const MemoryBuffer &secret, const gxString &smartcard_cert_username);
  int DecryptWithRSAKey(char private_key[], unsigned private_key_len,
			const gxString &rsa_key_username, char *passphrase = 0);
  int DecryptWithSmartcard(SmartCardOB *sc, const gxString &smartcard_cert_username);
  
public:
  gxList<StaticDataBlock> static_block_list;
  int ERROR_LEVEL;
  gxString err;
  gxDatabase *f;
  unsigned char *static_data;
  unsigned static_data_size;
  unsigned static_data_bytes_used;
  unsigned num_static_data_blocks;
};

#endif // __DB_AUTH_HPP__
// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
