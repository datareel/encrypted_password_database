// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// C++ Source Code File
// Compiler Used: clang (libFuzzer)
// Produced By: DataReel Software Development Team
// File Creation Date: 10/31/2026
// Date Last Modified: 10/31/2026
// Copyright (c) 2001-2026 DataReel Software Development
// ----------------------------------------------------------- //
// ------------- Program Description and Details ------------- //
// ----------------------------------------------------------- //
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
USA

10/31/2026: libFuzzer targets for the record import parsers, which read
files and stdin given by the user (epdb_auth_tool --import, --add-record
and the GUI import):

  FUZZ_CSV   EPDBParseCSV()
  FUZZ_JSON  EPDBParseJSONRecords()
  FUZZ_NV    FC_parse_name_values() and EPDBNameValuesToRecord()

Build with console/fuzz/build_fuzz.sh (clang, ASan, UBSan).
*/
// ----------------------------------------------------------- //
#include "gxdlcode.h"

#include <stdint.h>
#include "epdb_core.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
  MemoryBuffer buf;
  buf.Cat(data, (unsigned)size);
  gxString err;
  gxList<EPDBRecord> recs;
#if defined (FUZZ_CSV)
  EPDBParseCSV(buf, recs, err);
#elif defined (FUZZ_JSON)
  EPDBParseJSONRecords(buf, recs, err);
#elif defined (FUZZ_NV)
  gxList<FCNameValue> list;
  if(FC_parse_name_values(buf, list, err) == 0) {
    EPDBRecord rec;
    EPDBNameValuesToRecord(list, 0, rec, err);
  }
#else
#error Define FUZZ_CSV, FUZZ_JSON or FUZZ_NV
#endif
  EPDBWipeRecords(recs);
  return 0;
}
// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
