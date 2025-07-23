// ------------------------------- //
// -------- start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- // 
// C++ Source Code File
// Compiler Used: MSVC, BCC32, GCC, HPUX aCC, SOLARIS CC
// Produced By: DataReel Software Development Team
// File Creation Date: 06/15/2003
// Date Last Modified: 01/08/2024
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

Encrypted database auth functions
*/
// ----------------------------------------------------------- // 
#include "gxdlcode.h"

#include "m_globals.h"
#include "db_auth.h"

DatabaseUserAuth::DatabaseUserAuth()
{
    f = 0;
    ERROR_LEVEL = 0;
    static_data_size = DB_AUTH_STATIC_AREA_SIZE;
    static_data = 0;
    static_data_bytes_used = 0;
    num_static_data_blocks = 0;
}
  
DatabaseUserAuth::~DatabaseUserAuth()
{
  if(static_data) delete static_data;
}

int DatabaseUserAuth::LoadStaticDataBlocks()
{
  ERROR_LEVEL = 0;
  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }

  if(!static_data) static_data = new unsigned char[static_data_size];

  FAU_t start_of_auth_data = f->FileHeaderSize() + DB_CONFIG_STATIC_AREA_SIZE;
  gxDatabaseError db_error;

  db_error = f->Read(static_data, static_data_size, start_of_auth_data);
  if(db_error != gxDBASE_NO_ERROR) {
    ERROR_LEVEL = db_error;
    err << clear << "Error reading static auth area in DB file " << gxDatabaseExceptionMessage(db_error);
    return ERROR_LEVEL;
  }

  StaticDataBlockHdr static_data_block_header;
  StaticDataBlock static_data_block;
  unsigned block_offset = 0;
  unsigned offset = 0;
  int found_block = 0;
  int end_of_static_data = 0;
  int read_static_area = 1;
  char username_buf[1024];
  char username[1024];

  
  num_static_data_blocks = 0;
  static_data_bytes_used = 0;
  static_block_list.Clear();

  while(read_static_area) {
    if((static_data_size < sizeof(static_data_block_header)) || (offset >= (static_data_size-sizeof(static_data_block_header)))) {
      end_of_static_data = 1;
      read_static_area = 0;
      break;
    }
    
    found_block = 0;
    block_offset = offset;
    while(!found_block) {
      if(block_offset >= (static_data_size-sizeof(static_data_block_header))) {
	end_of_static_data = 1;
	read_static_area = 0;
	break;
      }
      
      static_data_block_header.WipeHeader(); // Clear all existing header data before read
      memmove(&static_data_block_header, static_data+block_offset, sizeof(static_data_block_header));
      block_offset+=sizeof(static_data_block_header);
      if((static_data_block_header.version == STATIC_DATA_BLOCK_VERSION) && (static_data_block_header.checkword == 0xFEFE)) {
	// Found a valid block
	found_block = 1;
	num_static_data_blocks++;
	break;
      }
    }
    if(end_of_static_data) {
      read_static_area = 0;
      break;
    }
    if(found_block) {
      static_data_block.Wipe();
      static_data_block.block_header = static_data_block_header;
      
      offset+=sizeof(static_data_block_header);
      static_data_block.rsa_ciphertext.Cat(static_data+offset, static_data_block_header.ciphertext_len);
      offset+=static_data_block_header.ciphertext_len;
      static_data_block.hmac.Cat(static_data+offset, AES_MAX_HMAC_LEN);
      offset+=AES_MAX_HMAC_LEN;
      memset(username_buf, 0, sizeof(username_buf));
      memset(username, 0, sizeof(username));
      
      if(static_data_block_header.username_len > 0) {
	memmove(username_buf, static_data+offset, static_data_block_header.username_len);
	static_data_block.username_encoded = username_buf;
	gxsBase64Decode(username_buf, username);
	static_data_block.username = username;
      }
      offset+=static_data_block_header.username_len;
      static_block_list.Add(static_data_block);
    }
  }
  if(num_static_data_blocks == 0) offset = 0;
  static_data_bytes_used = offset;
    
  return ERROR_LEVEL;
}

int DatabaseUserAuth::UpdateStaticData()
{
  ERROR_LEVEL = 0;
  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();
  unsigned offset = 0;
  MemoryBuffer mbuf;
  StaticDataBlockHdr static_data_block_header;
  
  AES_fillrand(static_data, static_data_size);

  // If the list is empty we will be clearing the the static auth area
  if(!ptr) return 0;
  
  offset = 0;
  while(ptr) {
    mbuf.Clear(1);
    mbuf.Cat(&ptr->data.block_header, sizeof(static_data_block_header));
    mbuf.Cat(ptr->data.rsa_ciphertext.m_buf(), ptr->data.rsa_ciphertext.length());
    mbuf.Cat(ptr->data.hmac.m_buf(), ptr->data.hmac.length());
    mbuf.Cat(ptr->data.username_encoded.c_str(), ptr->data.username_encoded.length());
    if((static_data_size < mbuf.length()) || (offset > (static_data_size - mbuf.length()))) {
      ERROR_LEVEL = -1;
      err << clear << "Out of free space in static data area";
      mbuf.Clear(1);
      return -1;
    }
    memmove(static_data+offset, mbuf.m_buf(), mbuf.length());
    offset += mbuf.length();
    ptr = ptr->next;
  }

  mbuf.Clear(1);
  return 0;
}

int DatabaseUserAuth::WriteStaticDataArea()
{
  ERROR_LEVEL = 0;

  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }

  FAU_t start_of_auth_data = f->FileHeaderSize() + DB_CONFIG_STATIC_AREA_SIZE;
  gxDatabaseError db_error;

  db_error = f->Write(static_data, static_data_size, start_of_auth_data);
  if(db_error != gxDBASE_NO_ERROR) {
    ERROR_LEVEL = db_error;
    err << clear << "Error writing static auth area to DB file " << gxDatabaseExceptionMessage(db_error);
    return ERROR_LEVEL;
  }

  return 0;
}

int DatabaseUserAuth::AddRSAKeyToStaticArea(const MemoryBuffer &secret,
					    char public_key[], unsigned public_key_len,
					    const gxString &rsa_key_username, char *passphrase)
{
  gxString sbuf;
  gxUINT32 version;
  StaticDataBlockHdr static_data_block_header;
  char username_buf[1024];
  unsigned char rsa_ciphertext[8192];
  unsigned rsa_ciphertext_len;
  int rv;

  ERROR_LEVEL = 0;
  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }
  
  gxDatabaseConfig db_config;
  DBStringConfig::crypt_key = secret;
  db_config.ReadConfig(f);

  char *s = db_config.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    ERROR_LEVEL = DBStringConfig::AES_error_level;
    err << clear << "Error decrypting database hash " << AES_err_string(DBStringConfig::AES_error_level) << "\n";
    return ERROR_LEVEL;
  }
  if(!s) {
    ERROR_LEVEL = -1;
      err << clear << "Database hash is a null value";
    return ERROR_LEVEL;
  }

  memset(rsa_ciphertext, 0, sizeof(rsa_ciphertext));
  RSA_openssl_init();  

  StaticDataBlock static_data_block;
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  AES_init_salt(SALT, sizeof(SALT));
  
  rv = AES_derive_key((const unsigned char*)secret.m_buf(), secret.length(), SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  MemoryBuffer p_secret;
  p_secret.Cat(SALT, AES_MAX_SALT_LEN);
  p_secret.Cat(secret.m_buf(), secret.length());
  
  rv = RSA_public_key_encrypt((const unsigned char *)public_key, public_key_len,
			      p_secret.m_buf(), p_secret.length(),
			      rsa_ciphertext, sizeof(rsa_ciphertext), &rsa_ciphertext_len,
			      RSA_padding, passphrase);
  if(rv != RSA_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "RSA encrypt public key error " << RSA_err_string(rv);
    return ERROR_LEVEL;
  }

  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();

  while(ptr) {
    if((ptr->data.username == rsa_key_username) && (ptr->data.block_header.block_type == 1)) {
      ERROR_LEVEL = 1;
      err << clear << "An RSA key for username " << ptr->data.username << " already exists";
      return ERROR_LEVEL;
    }
    ptr = ptr->next;
  }

  static_data_block_header.FormatHeader();
  unsigned char hash[AES_MAX_HMAC_LEN];
  memset(username_buf, 0, sizeof(username_buf));
  unsigned username_buf_len = 0;
  
  static_data_block_header.block_len = 0;
  static_data_block_header.block_type = 1;
  static_data_block_header.block_status = 1;
  
  static_data_block_header.ciphertext_len = rsa_ciphertext_len;
  static_data_block_header.block_len += rsa_ciphertext_len;
  static_data_block_header.block_len += sizeof(hash);

  gxsBase64Encode(rsa_key_username.c_str(), username_buf, rsa_key_username.length());
  username_buf_len = strlen(username_buf);
  static_data_block_header.username_len = username_buf_len;
  
  static_data_block.block_header = static_data_block_header;
  static_data_block.rsa_ciphertext.Cat(rsa_ciphertext, rsa_ciphertext_len);
  rv = AES_HMAC(KEY, sizeof(KEY), rsa_ciphertext, rsa_ciphertext_len, hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate HMAC for RSA ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }

  static_data_block.hmac.Cat(hash, sizeof(hash));
  static_data_block.username = rsa_key_username;
  static_data_block.username_encoded = username_buf;

  static_block_list.Add(static_data_block);

  if(UpdateStaticData() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  if(WriteStaticDataArea() != 0) return -1;

  return ERROR_LEVEL;
}

#ifdef __ENABLE_SMART_CARD__
int DatabaseUserAuth::AddSmartCardCertToStaticArea(SmartCardOB *sc, int use_cert_file,
						   const MemoryBuffer &secret, const gxString &smartcard_cert_username)
{
  gxString sbuf;
  gxUINT32 version;
  StaticDataBlockHdr static_data_block_header;
  char username_buf[1024];
  unsigned char ciphertext[8192];
  unsigned ciphertext_len;
  int rv;

  ERROR_LEVEL = 0;
  if(!f) {
    ERROR_LEVEL = -1;
    err << clear << "Error database pointer is null for DB auth";
    return ERROR_LEVEL;
  }
  
  gxDatabaseConfig db_config;
  DBStringConfig::crypt_key = secret;
  db_config.ReadConfig(f);

  char *s = db_config.database_name.GetString();
  if(DBStringConfig::AES_error_level != AES_NO_ERROR) {
    ERROR_LEVEL = DBStringConfig::AES_error_level;
    err << clear << "Error decrypting database hash " << AES_err_string(DBStringConfig::AES_error_level) << "\n";
    return ERROR_LEVEL;
  }
  if(!s) {
    ERROR_LEVEL = -1;
      err << clear << "Database hash is a null value";
    return ERROR_LEVEL;
  }

  memset(ciphertext, 0, sizeof(ciphertext));

  StaticDataBlock static_data_block;
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  AES_init_salt(SALT, sizeof(SALT));
  
  rv = AES_derive_key((const unsigned char*)secret.m_buf(), secret.length(), SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  MemoryBuffer p_secret;
  p_secret.Cat(SALT, AES_MAX_SALT_LEN);
  p_secret.Cat(secret.m_buf(), secret.length());

  if(use_cert_file) {
    rv =  SC_cert_file_public_key_encrypt(sc, p_secret.m_buf(), p_secret.length(), ciphertext, sizeof(ciphertext), &ciphertext_len);
  }
  else {
    rv =  SC_public_key_encrypt(sc, p_secret.m_buf(), p_secret.length(), ciphertext, sizeof(ciphertext), &ciphertext_len);
  }
  
  if(rv != 0) {
    ERROR_LEVEL = rv;
    err << clear << "Smart card cert encrypt error " << sc->err_string;
    return ERROR_LEVEL;
  }

  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *ptr = static_block_list.GetHead();

  while(ptr) {
    if((ptr->data.username == smartcard_cert_username) && (ptr->data.block_header.block_type == 2)) {
      ERROR_LEVEL = 1;
      err << clear << "A smart card cert for username " << ptr->data.username << " already exists";
      return ERROR_LEVEL;
    }
    ptr = ptr->next;
  }

  static_data_block_header.FormatHeader();
  unsigned char hash[AES_MAX_HMAC_LEN];
  memset(username_buf, 0, sizeof(username_buf));
  unsigned username_buf_len = 0;
  
  static_data_block_header.block_len = 0;
  static_data_block_header.block_type = 2;
  static_data_block_header.block_status = 1;
  
  static_data_block_header.ciphertext_len = ciphertext_len;
  static_data_block_header.block_len += ciphertext_len;
  static_data_block_header.block_len += sizeof(hash);

  gxsBase64Encode(smartcard_cert_username.c_str(), username_buf, smartcard_cert_username.length());
  username_buf_len = strlen(username_buf);
  static_data_block_header.username_len = username_buf_len;
  
  static_data_block.block_header = static_data_block_header;
  static_data_block.rsa_ciphertext.Cat(ciphertext, ciphertext_len);
  rv = AES_HMAC(KEY, sizeof(KEY), ciphertext, ciphertext_len, hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "Failed to generate HMAC for smart card ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }

  static_data_block.hmac.Cat(hash, sizeof(hash));
  static_data_block.username = smartcard_cert_username;
  static_data_block.username_encoded = username_buf;

  static_block_list.Add(static_data_block);

  if(UpdateStaticData() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  if(WriteStaticDataArea() != 0) return -1;

  return ERROR_LEVEL;
}
#endif // __ENABLE_SMART_CARD__

int DatabaseUserAuth::DecryptWithRSAKey(char private_key[], unsigned private_key_len,
					const gxString &rsa_key_username, char *passphrase)
{
  ERROR_LEVEL = 0;
  int rv;
  unsigned decrypted_data_len = 0;
  unsigned char rsa_decrypted_message[2048];
  unsigned char hash[AES_MAX_HMAC_LEN];
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  MemoryBuffer aes_file_decrypt_secret;
  
  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *list_ptr = static_block_list.GetHead();
  if(!list_ptr) {
    ERROR_LEVEL = -1;
    err << clear << "No authorized RSA users found in encrypted DB file";
    return ERROR_LEVEL;
  }
  
  int found_key = 0;
  while(list_ptr) {
    memset(rsa_decrypted_message, 0, sizeof(rsa_decrypted_message));
    memset(hash, 0, sizeof(hash));
    decrypted_data_len = 0;
    if((rsa_key_username == list_ptr->data.username) && (list_ptr->data.block_header.block_type == 1)) {
      found_key = 1;
      break;
    }
    list_ptr = list_ptr->next;
  }

  if(!found_key) {
    ERROR_LEVEL = -1;
    err << clear << "The private RSA key provided for user " << rsa_key_username << " does not have access to decrypt the database";
    return ERROR_LEVEL;
  }

  if(list_ptr->data.block_header.block_type != 1) {
    ERROR_LEVEL = -1;
    if(list_ptr->data.block_header.block_type == 2) {
      err << clear << "User " << list_ptr->data.username << " access type is smart card";
    }
    else {
      err << clear << "User " << list_ptr->data.username << " access type is not RSA key";
    }
    return ERROR_LEVEL;
    
  }

  rv = RSA_private_key_decrypt((const unsigned char *)private_key, private_key_len,
			       list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(),
			       rsa_decrypted_message, sizeof(rsa_decrypted_message), &decrypted_data_len,
			       RSA_padding, passphrase);
  if(rv != RSA_NO_ERROR) {
    ERROR_LEVEL = rv;
    err << clear << "RSA private key decrypt failed " << RSA_err_string(rv);
    return ERROR_LEVEL;
  }

  memmove(SALT, rsa_decrypted_message, AES_MAX_SALT_LEN);
  aes_file_decrypt_secret.Clear(1);
  aes_file_decrypt_secret.Cat(rsa_decrypted_message+AES_MAX_SALT_LEN, (decrypted_data_len-AES_MAX_SALT_LEN));
     
  rv = AES_derive_key((const unsigned char*)aes_file_decrypt_secret.m_buf(), aes_file_decrypt_secret.length(),
		      SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
      
  rv = AES_HMAC(KEY, sizeof(KEY), list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(), hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate HMAC for RSA ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  if(memcmp(hash, list_ptr->data.hmac.m_buf(), sizeof(hash)) != 0) {
    ERROR_LEVEL = -1;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Message authentication failed bad HMAC for RSA ciphertext";
    return ERROR_LEVEL;
  }

  DBStringConfig::crypt_key = aes_file_decrypt_secret;
  aes_file_decrypt_secret.Clear(1);
  
  return ERROR_LEVEL;
}

#ifdef __ENABLE_SMART_CARD__
int DatabaseUserAuth::DecryptWithSmartcard(SmartCardOB *sc, const gxString &smartcard_cert_username)
{
  ERROR_LEVEL = 0;
  int rv;
  unsigned decrypted_data_len = 0;
  unsigned char rsa_decrypted_message[2048];
  unsigned char hash[AES_MAX_HMAC_LEN];
  unsigned char SALT[AES_MAX_SALT_LEN];
  unsigned char IV[AES_MAX_IV_LEN];
  unsigned char KEY[AES_MAX_KEY_LEN];
  MemoryBuffer aes_file_decrypt_secret;
  
  if(LoadStaticDataBlocks() != 0) return -1;
  if(ERROR_LEVEL != 0) return ERROR_LEVEL;

  gxListNode<StaticDataBlock> *list_ptr = static_block_list.GetHead();
  if(!list_ptr) {
    ERROR_LEVEL = -1;
    err << clear << "No authorized smartcard users found in encrypted DB file";
    return ERROR_LEVEL;
  }

  int found_key = 0;
  while(list_ptr) {
    memset(rsa_decrypted_message, 0, sizeof(rsa_decrypted_message));
    memset(hash, 0, sizeof(hash));
    decrypted_data_len = 0;
    if((smartcard_cert_username == list_ptr->data.username) && (list_ptr->data.block_header.block_type == 2)) {
     found_key = 1;
      break;
    }
    list_ptr = list_ptr->next;
  }

  if(!found_key) {
    ERROR_LEVEL = -1;
    err << clear << "The smart card provided for user " << smartcard_cert_username << " does not have access to decrypt the database";
    return ERROR_LEVEL;
  }
  
  if(list_ptr->data.block_header.block_type != 2) {
    ERROR_LEVEL = -1;
    if(list_ptr->data.block_header.block_type == 1) {
      err << clear << "User " << list_ptr->data.username << " access type is RSA key";
    }
    else {
      err << clear << "User " << list_ptr->data.username << " access type is not smart card";
    }
  }
  
  rv = SC_private_key_decrypt(sc, list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(),
			      rsa_decrypted_message, sizeof(rsa_decrypted_message), &decrypted_data_len);
  if(rv != 0) {
    ERROR_LEVEL = rv;
    err << clear << "Smart card private key decrypt failed for cert ID " << sc->cert_id << " " << sc->err_string;
    return ERROR_LEVEL;
  }
  
  memmove(SALT, rsa_decrypted_message, AES_MAX_SALT_LEN);
  aes_file_decrypt_secret.Clear(1);
  aes_file_decrypt_secret.Cat(rsa_decrypted_message+AES_MAX_SALT_LEN, (decrypted_data_len-AES_MAX_SALT_LEN));
  
  rv = AES_derive_key((const unsigned char*)aes_file_decrypt_secret.m_buf(), aes_file_decrypt_secret.length(),
		      SALT, sizeof(SALT), KEY, sizeof(KEY), IV, sizeof(IV), 1000);
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate derived key " << AES_err_string(rv);
    return rv;
  }
  
  rv = AES_HMAC(KEY, sizeof(KEY), list_ptr->data.rsa_ciphertext.m_buf(), list_ptr->data.rsa_ciphertext.length(), hash, sizeof(hash));
  if(rv != AES_NO_ERROR) {
    ERROR_LEVEL = rv;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Failed to generate HMAC for smart card ciphertext " << AES_err_string(rv);
    return ERROR_LEVEL;
  }
  if(memcmp(hash, list_ptr->data.hmac.m_buf(), sizeof(hash)) != 0) {
    ERROR_LEVEL = -1;
    aes_file_decrypt_secret.Clear(1);
    err << clear << "Message authentication failed bad HMAC for smart card ciphertext";
    return ERROR_LEVEL;
  }
  
  DBStringConfig::crypt_key = aes_file_decrypt_secret;

  aes_file_decrypt_secret.Clear(1);
  return ERROR_LEVEL;
}
#endif // __ENABLE_SMART_CARD__

// ----------------------------------------------------------- // 
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
