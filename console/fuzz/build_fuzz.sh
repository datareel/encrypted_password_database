#!/bin/bash
# build_fuzz.sh OUT - 10/31/2026: build the record import fuzz targets in
# console/fuzz (fuzz_import.cpp) with clang, AddressSanitizer and
# UndefinedBehaviorSanitizer, into OUT. Needs clang, compiler-rt,
# openssl-devel and zlib-devel. It rebuilds the objects in console/linux
# with clang, so run it on a copy of the source tree.
set -e
OUT=${1:?usage: build_fuzz.sh OUT}
HERE=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$OUT"
SAN="-fsanitize=address,undefined -fno-sanitize-recover=undefined -g -O1"
cd "$HERE/../linux"
make -f makefile_rhel10 clean > /dev/null 2>&1 || true
make -f makefile_rhel10 -j"$(nproc)" CPP="clang++ $SAN -fsanitize=fuzzer-no-link" > "$OUT/build.log" 2>&1
OBJS=$(ls *.o | grep -v '^epdb_auth_tool\.o$')
DEFS=$(grep -o -- '-D[A-Za-z0-9_]*' "$OUT/build.log" | sort -u | tr '\n' ' ')
INC="-I$HERE/../../include -I$HERE/../../src -I$HERE/.. -I$HERE/../../fcrypt/3plibs/datareel/include -I$HERE/../../fcrypt/include"
for t in CSV JSON NV; do
  clang++ $SAN -fsanitize=fuzzer $DEFS -DFUZZ_$t $INC "$HERE/fuzz_import.cpp" $OBJS \
    -lssl -lcrypto -lz -lpthread -o "$OUT/fuzz_$(echo $t | tr A-Z a-z)"
done
echo "built $OUT/fuzz_csv $OUT/fuzz_json $OUT/fuzz_nv"
