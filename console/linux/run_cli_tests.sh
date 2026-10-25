#!/bin/bash
# Regression tests for epdb_auth_tool.
#
# Builds epdb_test_db, creates a database with a known password, and
# checks epdb_auth_tool output, exit codes and its handling of damaged
# or crafted database files.
#
# Usage: ./run_cli_tests.sh [old_build_dir]
#
# Run after building epdb_auth_tool with make in this directory. With
# an old build directory (a console/linux directory with an older
# epdb_auth_tool), also checks the old build reads databases made and
# changed by this build.

NEW_DIR=$(cd $(dirname $0) && pwd)
TOOL=${NEW_DIR}/epdb_auth_tool
TESTDB=${NEW_DIR}/epdb_test_db
OLD_DIR=$1
OLD=${OLD_DIR}/epdb_auth_tool
testDIR=${HOME}/tmp/epdb_cli_testing_$(date +%Y_%m_%d_%H%M%S)
logFILE=${testDIR}/epdb_cli_testing.log
PW='Known-Pass#2026'
WRONG='Wrong-Pass#2026'
passed=0
failed=0

if [ ! -f ${TOOL} ]; then
    echo "ERROR: Missing ${TOOL}, run make first"
    exit 1
fi
if ! command -v python3 >/dev/null 2>&1; then
    echo "ERROR: python3 is needed to build the test files"
    exit 1
fi

if [ ! -f ${TESTDB} ] || [ ${NEW_DIR}/../epdb_test_db.cpp -nt ${TESTDB} ]; then
    MAKEFILE=makefile
    if [ -f /etc/redhat-release ] && grep -qi Coughlan /etc/redhat-release; then MAKEFILE=makefile_rhel10; fi
    (cd ${NEW_DIR} && make -f ${MAKEFILE} PROJECT=epdb_test_db >/dev/null 2>&1)
    if [ ! -f ${TESTDB} ]; then
	echo "ERROR: Cannot build ${TESTDB}"
	exit 1
    fi
fi

mkdir -p ${testDIR}
cd ${testDIR}
cat /dev/null > ${logFILE}

pass() { echo "Passed: $1" | tee -a ${logFILE}; passed=$((passed+1)); }
fail() { echo "FAILED: $1" | tee -a ${logFILE}; failed=$((failed+1)); }
check() { if [ "$1" -eq 0 ]; then pass "$2"; else fail "$2"; fi; }

echo "Starting tests at $(date)" | tee -a ${logFILE}

${TESTDB} create known.ehd "${PW}" 12 >>${logFILE} 2>&1
check $? "create the test database (format v2)"
${TESTDB} create-v1 known_v1.ehd "${PW}" 12 >>${logFILE} 2>&1
check $? "create a format v1 test database"
${TESTDB} dump known.ehd "${PW}" > dump.txt 2>>${logFILE}
[ $? -eq 0 ] && [ $(wc -l < dump.txt) -eq 12 ]
check $? "dump the test database"

${TESTDB} selftest ${testDIR} >>${logFILE} 2>&1
check $? "helper self test: links, CSV formula escaping, private files, process setup"

# Key check and exit codes
${TOOL} --password="${PW}" --db-list known.ehd > list.txt 2>>${logFILE}
[ $? -eq 0 ] && [ $(wc -l < list.txt) -eq 12 ]
check $? "list with the right password"
${TOOL} --password="${WRONG}" --db-list known.ehd > out.txt 2>err.txt
rv=$?
[ ${rv} -eq 1 ] && [ ! -s out.txt ] && grep -q -E "Error decrypting database hash|not correct" err.txt
check $? "list with the wrong password fails with no output"
${TOOL} --password="${WRONG}" --db-find-key='Entry 003' known.ehd > out.txt 2>/dev/null
[ $? -eq 1 ] && [ ! -s out.txt ]
check $? "find with the wrong password fails with no output"
${TOOL} --password="${PW}" --db-find-key='Entry 003' known.ehd > out.txt 2>/dev/null
[ $? -eq 0 ] && [ $(wc -l < out.txt) -eq 1 ] && grep -q "^Entry 003" out.txt
check $? "find one record"
${TOOL} --password="${PW}" --db-find-key='nomatch' known.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "No objects found" err.txt
check $? "find with no match exits 1, message on stderr"
${TOOL} --password="${PW}" --db-list missing.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ]
check $? "missing database file exits 1"
printf '%s\n' "${PW}" | ${TOOL} --password --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && [ $(wc -l < out.txt) -eq 12 ] && ! grep -q "Password:" out.txt
check $? "password prompt and echo go to stderr"
timeout 20 ${TOOL} --db-list known.ehd < /dev/null > out.txt 2>/dev/null
[ $? -eq 1 ]
check $? "password prompt at end of input exits 1"

# Secrets from files and file descriptors (M9)
umask 077
printf '%s\n' "${PW}" > pw.txt
${TOOL} --password-file=pw.txt --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "--password-file reads the password"
chmod 644 pw.txt
${TOOL} --password-file=pw.txt --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "chmod 600" err.txt
check $? "--password-file refuses a file other users can read"
chmod 600 pw.txt; ln -sf pw.txt pw_link.txt
${TOOL} --password-file=pw_link.txt --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ]
check $? "--password-file refuses a symbolic link"
${TOOL} --password-fd=3 --db-list known.ehd 3<pw.txt > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "--password-fd reads the password"
${TOOL} --password="${PW}" --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt && grep -q "WARNING: --password=value" err.txt
check $? "--password=value still works and warns"
openssl genrsa -aes256 -passout pass:rsa-pass-2026 -out rsa_priv.pem 2048 >/dev/null 2>&1
openssl rsa -in rsa_priv.pem -passin pass:rsa-pass-2026 -pubout -out rsa_pub.pem >/dev/null 2>&1
cp known.ehd rsa.ehd; cp known.ehx rsa.ehx
${TOOL} --password-file=pw.txt --add-rsa-key=rsa_pub.pem --add-username=tester rsa.ehd >>${logFILE} 2>&1
check $? "add an RSA user"
printf 'rsa-pass-2026\n' > pp.txt
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase-file=pp.txt --db-list rsa.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "--rsa-key-passphrase-file opens with the RSA key"
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase-fd=4 --db-list rsa.ehd 4<pp.txt > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "--rsa-key-passphrase-fd opens with the RSA key"
# The tool waits at the RSA passphrase prompt while its arguments are checked
(sleep 3; echo wrong) | ${TOOL} --password=SECRETVALUE123 --rsa-key=rsa_priv.pem --rsa-key-username=tester --db-list rsa.ehd >/dev/null 2>&1 &
tpid=$!
sleep 1
if [ -r /proc/${tpid}/cmdline ]; then
    tr '\0' ' ' < /proc/${tpid}/cmdline > cmdline.txt
    ! grep -q SECRETVALUE123 cmdline.txt && grep -q -- "--password=xxxx" cmdline.txt
    check $? "--password=value is removed from the process arguments"
else
    fail "--password=value is removed from the process arguments (process ended early)"
fi
wait ${tpid} 2>/dev/null
${TOOL} "" --password-file=pw.txt --db-list known.ehd > out.txt 2>/dev/null &
tpid=$!
sleep 5
if kill -0 ${tpid} 2>/dev/null; then kill ${tpid}; fail "an empty argument does not hang"; else pass "an empty argument does not hang"; fi
wait ${tpid} 2>/dev/null

# Output formats (M11)
${TOOL} --password-file=pw.txt --db-find-key='Entry 004' known.ehd > out.txt 2>/dev/null
rv=$?
python3 -I -c "import csv,sys; r=list(csv.reader(open('out.txt',newline=''),delimiter='\t')); sys.exit(0 if len(r)==1 and len(r[0])==8 and r[0][2]=='P@ss\tw0rd,4\"q' else 1)"
[ ${rv} -eq 0 ] && [ $? -eq 0 ]
check $? "default output quotes a field with a tab and doubles its quotes"
${TOOL} --password-file=pw.txt --csv --db-list known.ehd > out.csv 2>/dev/null
rv=$?
python3 -I - out.csv dump.txt <<'PYEOF'
import csv,sys
rows=list(csv.reader(open(sys.argv[1],newline='')))
def unesc(f):
    if f=='<none>': return ''
    return f.encode('latin-1').decode('unicode_escape').replace('\\|','|')
dump=[[unesc(x) for x in l.rstrip('\n').split('|')] for l in open(sys.argv[2])]
sys.exit(0 if rows==dump else 1)
PYEOF
[ ${rv} -eq 0 ] && [ $? -eq 0 ]
check $? "--csv output parses back to the stored values"
${TOOL} --password-file=pw.txt --field=3 --db-find-key='Entry 004' known.ehd > out.txt 2>/dev/null
[ $? -eq 0 ] && [ "$(cat out.txt)" = 'P@ss	w0rd,4"q' ]
check $? "--field prints one value as stored"
${TOOL} --password-file=pw.txt --field=5 --db-find-key='Entry 004' known.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "use --null" err.txt
check $? "--field refuses a value with a newline without --null"
${TOOL} --password-file=pw.txt --field=5 --null --db-find-key='Entry 004' known.ehd > out.txt 2>/dev/null
[ $? -eq 0 ] && [ "$(tr '\0\n' '#~' < out.txt)" = 'Q1? "blue"~Q2? 42#' ]
check $? "--field --null prints a value with a newline"

# Key verification for RSA and unset database names (M1, M2)
${TESTDB} create other.ehd "${WRONG}" 2 >>${logFILE} 2>&1
printf '%s\n' "${WRONG}" > pw2.txt
${TOOL} --password-file=pw2.txt --add-rsa-key=rsa_pub.pem --add-username=tester other.ehd >>${logFILE} 2>&1
check $? "add the same RSA user to a second database"
# Copy the second database's auth area, which wraps its key, over the first
cp rsa.ehd swap.ehd; cp rsa.ehx swap.ehx
python3 -I -c "
a=bytearray(open('swap.ehd','rb').read()); b=open('other.ehd','rb').read()
o=86+65536; a[o:o+65536]=b[o:o+65536]; open('swap.ehd','wb').write(a)"
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase-file=pp.txt --db-list swap.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "does not hold this database's key" err.txt
check $? "RSA key block holding another database's key is rejected"
# Add the other database's block as a second block for the same user
cp rsa.ehd dup.ehd; cp rsa.ehx dup.ehx
python3 -I -c "
import struct
a=bytearray(open('dup.ehd','rb').read()); b=open('other.ehd','rb').read(); o=86+65536
def blen(d):
    v=struct.unpack_from('<7I',d,o); return 60+v[5]+32+v[6]
la=blen(a); lb=blen(b); a[o+la:o+la+lb]=b[o:o+lb]; open('dup.ehd','wb').write(a)"
${TOOL} --list-users dup.ehd 2>/dev/null | grep -c "Username: tester" > count.txt
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase-file=pp.txt --db-list dup.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ "$(cat count.txt)" = "2" ] && grep -q "more than one RSA key block" err.txt
check $? "a second RSA key block for the same user is rejected"
cp known_v1.ehd nohash.ehd; cp known_v1.ehx nohash.ehx
python3 -I -c "d=bytearray(open('nohash.ehd','rb').read()); d[90]=0; open('nohash.ehd','wb').write(d)"
${TOOL} --password-file=pw2.txt --db-config nohash.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "not set" err.txt
check $? "a database name that was never set does not accept any password"
cp known.ehd nohash2.ehd; cp known.ehx nohash2.ehx
python3 -I -c "d=bytearray(open('nohash2.ehd','rb').read()); d[90]=0; open('nohash2.ehd','wb').write(d)"
${TOOL} --password-file=pw.txt --db-config nohash2.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "not set" err.txt
check $? "a v2 database name that was never set is refused with the right password"
${TOOL} --password-file=pw2.txt --db-config nohash2.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ]
check $? "a v2 database name that was never set is refused with another password"

# Group 6: OAEP blocks, key file permissions, usernames, file names
cp known.ehd oaep.ehd; cp known.ehx oaep.ehx
${TESTDB} addrsa oaep.ehd "${PW}" rsa_pub.pem oaepuser oaep >>${logFILE} 2>&1
check $? "add an RSA user block with OAEP padding"
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=oaepuser --rsa-key-passphrase-file=pp.txt --db-list oaep.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open with an OAEP RSA user block"
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase-file=pp.txt --db-list rsa.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "PKCS#1 v1.5 RSA user blocks still open"
cp rsa_priv.pem loose_priv.pem; chmod 644 loose_priv.pem
${TOOL} --rsa-key=loose_priv.pem --rsa-key-username=tester --rsa-key-passphrase-file=pp.txt --db-list rsa.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "chmod 600" err.txt
check $? "a private RSA key file other users can read is refused"
head -c 64 /dev/urandom > aes.key; chmod 644 aes.key
${TOOL} --key=aes.key --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "chmod 600" err.txt
check $? "an AES key file other users can read is refused"
LONGNAME=$(head -c 400 /dev/zero | tr '\0' 'u')
cp known.ehd longname.ehd; cp known.ehx longname.ehx
${TOOL} --password-file=pw.txt --add-rsa-key=rsa_pub.pem --add-username=${LONGNAME} longname.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "username must be" err.txt
check $? "an add-user name that is too long is refused"
mkdir -p dir.d; cp known.ehd dir.d/mydb.ehd; cp known.ehx dir.d/mydb.ehx
cp known.ehd dir.d/mydb; cp known.ehd dir.ehd; cp known.ehx dir.ehx
${TOOL} --password-file=pw.txt --db-list dir.d/mydb.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "a path with a dot in a directory name opens the right file"
${TOOL} --password-file=pw.txt --db-list dir.d/mydb > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "does not end in .ehd" err.txt
check $? "a database name without .ehd is refused, not changed to dir.ehd"
cp known.ehd loop.ehd; cp known.ehx loop.ehx
python3 -I -c "
import struct
x=bytearray(open('loop.ehx','rb').read()); r=struct.unpack_from('<q',x,86+20)[0]
struct.pack_into('<q',x,r+4,r); open('loop.ehx','wb').write(x)"
timeout 30 ${TOOL} --password-file=pw.txt --db-list loop.ehd > out.txt 2>err.txt
rv=$?; [ ${rv} -eq 1 ]
check $? "a B-tree node that points to itself is an error, not a loop (rc=${rv})"
timeout 30 ${TOOL} --password-file=pw.txt --db-find-key='Entry 003' loop.ehd > out.txt 2>err.txt
rv=$?; [ ${rv} -eq 1 ]
check $? "a search of a looping B-tree stops with an error (rc=${rv})"

# Group 7: database format v2 and the upgrade
cmp_dump() { ${TESTDB} dump $1 "$2" > d.txt 2>>${logFILE} && cmp -s d.txt $3; }
${TESTDB} create-v1 up.ehd "${PW}" 12 >>${logFILE} 2>&1
${TESTDB} dump up.ehd "${PW}" > up_before.txt 2>>${logFILE}
cmp -s up_before.txt dump.txt
check $? "a v1 database has the same records as a v2 database"
cp up.ehd up_orig.ehd; cp up.ehx up_orig.ehx
${TOOL} --password-file=pw.txt --add-rsa-key=rsa_pub.pem --add-username=tester up.ehd >>${logFILE} 2>&1
cp up.ehd up_rsa.ehd; cp up.ehx up_rsa.ehx
${TOOL} --password-file=pw.txt --upgrade-db up.ehd > out.txt 2>err.txt
rv=$?; backup=$(sed -n 's/^The v1 database was saved to \([^ ]*\) and .*/\1/p' err.txt)
[ ${rv} -eq 0 ] && [ -n "${backup}" ] && [ -f "${backup}" ]
check $? "--upgrade-db upgrades and reports the backup file"
cmp -s "${backup}" up_rsa.ehd && cmp -s "${backup%.ehd}.ehx" up_rsa.ehx
check $? "the backup files are identical to the database before the upgrade"
[ "$(stat -c %a ${backup})" = "600" ] && [ "$(stat -c %a ${backup%.ehd}.ehx)" = "600" ]
check $? "the backup files can only be read by the user"
cmp_dump up.ehd "${PW}" up_before.txt
check $? "every record is the same after the upgrade"
! ls up.v2-upgrade.* >/dev/null 2>&1
check $? "no temporary files are left after the upgrade"
${TOOL} --password-file=pw.txt --db-list up.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "epdb_auth_tool lists the upgraded database"
${TOOL} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase-file=pp.txt --db-list up.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "an RSA user added before the upgrade opens the v2 database"
openssl genrsa -out rsa2_priv.pem 2048 >/dev/null 2>&1; chmod 600 rsa2_priv.pem
openssl rsa -in rsa2_priv.pem -pubout -out rsa2_pub.pem >/dev/null 2>&1
${TOOL} --password-file=pw.txt --add-rsa-key=rsa2_pub.pem --add-username=newuser up.ehd >>${logFILE} 2>&1
${TOOL} --rsa-key=rsa2_priv.pem --rsa-key-username=newuser --db-list up.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "an RSA user added to the v2 database (OAEP) opens it"
${TOOL} --password-file=pw.txt --upgrade-db up.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "already a v2" err.txt
check $? "--upgrade-db refuses a v2 database"
cp up_orig.ehd bad.ehd; cp up_orig.ehx bad.ehx
python3 -I -c "
import struct
d=bytearray(open('bad.ehd','rb').read()); i=d.find(struct.pack('<II',8,355)); i=d.find(struct.pack('<II',8,355),i+1)
struct.pack_into('<I',d,i+8+40,0x41414141); open('bad.ehd','wb').write(d)"
cp bad.ehd bad_before.ehd
${TOOL} --password-file=pw.txt --upgrade-db bad.ehd > out.txt 2>err.txt
rv=$?
[ ${rv} -eq 1 ] && cmp -s bad.ehd bad_before.ehd && ! ls bad.v1-backup-* bad.v2-upgrade.* >/dev/null 2>&1
check $? "a failed upgrade leaves the database unchanged and no backup or temporary files"
${TOOL} --password-file=pw2.txt --upgrade-db up_orig.ehd > out.txt 2>&1
[ $? -eq 1 ] && ! ls up_orig.v1-backup-* >/dev/null 2>&1
check $? "an upgrade with the wrong password changes nothing"
# Tampering a v2 database is detected
cp known.ehd t1.ehd; cp known.ehx t1.ehx
python3 -I -c "
import struct
d=bytearray(open('t1.ehd','rb').read()); i=d.find(struct.pack('<II',8,355)); i=d.find(struct.pack('<I',355),i+8+355)
d[i+4+200]^=1; open('t1.ehd','wb').write(d)"
${TOOL} --password-file=pw.txt --db-list t1.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "integrity check failed" err.txt
check $? "a changed byte in a v2 database is detected"
cp known.ehd t2.ehd; cp known.ehx t2.ehx
python3 -I -c "
import struct
d=bytearray(open('t2.ehd','rb').read()); a=d.find(struct.pack('<II',8,355)); b=d.find(struct.pack('<II',8,355),a+8)
x=d[a+8:a+8+355]; y=d[b+8:b+8+355]; d[a+8:a+8+355]=y; d[b+8:b+8+355]=x; open('t2.ehd','wb').write(d)"
${TOOL} --password-file=pw.txt --db-list t2.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "integrity check failed" err.txt
check $? "swapping the key fields of two v2 records is detected"
cp known.ehd t3.ehd; cp known.ehx t3.ehx
python3 -I -c "
import struct
x=bytearray(open('t3.ehx','rb').read()); r=struct.unpack_from('<q',x,86+20)[0]
lc=struct.unpack_from('<q',x,r+4)[0]
n=struct.unpack_from('<i',x,lc)[0]; struct.pack_into('<i',x,lc,n-1); open('t3.ehx','wb').write(x)"
${TOOL} --password-file=pw.txt --db-list t3.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "index file does not match" err.txt
check $? "an entry removed from the v2 index is detected"
${TOOL} --password-file=pw.txt --reseal-db t3.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && grep -q "WARNING: Resealed" err.txt
check $? "--reseal-db re-seals with a warning"
${TOOL} --password-file=pw.txt --db-list t3.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && grep -q "index file does not match" err.txt
check $? "re-sealing does not hide an entry removed from the index"
${TOOL} --password-file=pw2.txt --db-list known.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "not correct" err.txt
check $? "the wrong password is refused for a v2 database"
if [ "${OLD_DIR}" != "" ]; then
    ${OLD} --password="${PW}" --db-config known.ehd > out.txt 2>&1
    [ $? -ne 0 ]
    check $? "the old build refuses --db-config on a v2 database"
    ${OLD} --rsa-key=rsa_priv.pem --rsa-key-username=tester --rsa-key-passphrase=rsa-pass-2026 --db-list up.ehd > out.txt 2>err.txt
    [ $? -ne 0 ] && grep -q "No authorized RSA users" err.txt
    check $? "the old build refuses an RSA open of an upgraded v2 database"
    ${OLD} --password="${PW}" --db-list up_orig.ehd > out.txt 2>>${logFILE}
    rv=$?; sed 's/""/"/g' list.txt > list_old_quoting.txt
    [ ${rv} -eq 0 ] && cmp -s out.txt list_old_quoting.txt
    check $? "the old build still lists a v1 database made by this build"
fi

# Damaged and crafted files must fail cleanly, never crash
cp known.ehd t.ehd; cp known.ehx t.ehx
tamper() { # tamper name python_statement, on a v1 database to test the parsing
    cp known_v1.ehd $1.ehd; cp known_v1.ehx $1.ehx
    python3 -I -c "import struct,sys
d=bytearray(open('$1.ehd','rb').read()); x=bytearray(open('$1.ehx','rb').read())
$2
open('$1.ehd','wb').write(d); open('$1.ehx','wb').write(x)"
}
# Offsets: config DBString database_name crypt_len at 90, auth area
# block header at 86+65536, B-tree header at 86 and root node in .ehx.
tamper auth "struct.pack_into('<IIIIIII',d,86+65536,2023103,0xFEFE,1,0,0,256,6000)"
${TOOL} --list-users auth.ehd > /dev/null 2>&1
rv=$?; [ ${rv} -eq 1 ]
check $? "auth area with a bad username length is rejected (rc=${rv})"
tamper cryptlen "struct.pack_into('<I',d,90,200000)"
${TOOL} --password="${PW}" --db-list cryptlen.ehd > /dev/null 2>&1
rv=$?; [ ${rv} -eq 1 ]
check $? "config string with a bad crypt_len is rejected (rc=${rv})"
tamper keycount "r=struct.unpack_from('<q',x,86+20)[0]; struct.pack_into('<i',x,r,5000)"
${TOOL} --password="${PW}" --db-list keycount.ehd > /dev/null 2>&1
rv=$?; [ ${rv} -eq 1 ]
check $? "B-tree node with a bad key count is rejected (rc=${rv})"
tamper keysize "struct.pack_into('<i',x,86+4,40)"
${TOOL} --password="${PW}" --db-list keysize.ehd > /dev/null 2>&1
rv=$?; [ ${rv} -eq 1 ]
check $? "B-tree header with a bad key size is rejected (rc=${rv})"
tamper member "i=d.find(struct.pack('<II',8,355)); struct.pack_into('<I',d,i+4,0xF0000000)"
${TOOL} --password="${PW}" --db-list member.ehd > out.txt 2>/dev/null
rv=$?; [ ${rv} -eq 1 ] && [ $(wc -l < out.txt) -eq 11 ]
check $? "record with a bad member length is skipped and exits 1 (rc=${rv})"
tamper field "i=d.find(struct.pack('<II',8,355)); struct.pack_into('<I',d,i+8+40,0x41414141)"
${TOOL} --password="${PW}" --db-list field.ehd > out.txt 2>err.txt
rv=$?; [ ${rv} -eq 1 ] && grep -q "Cannot decrypt" err.txt
check $? "record field that fails to decrypt stops the export (rc=${rv})"

if [ "${OLD_DIR}" != "" ]; then
    ${OLD} --password="${PW}" --db-list known_v1.ehd > old_list.txt 2>>${logFILE}
    rv=$?
    # The old build does not double quotes inside quoted fields
    sed 's/""/"/g' list.txt > list_old_quoting.txt
    [ ${rv} -eq 0 ] && cmp -s old_list.txt list_old_quoting.txt
    check $? "old build lists the database the same, apart from quote doubling"
fi

echo "" | tee -a ${logFILE}
echo "${passed} passed, ${failed} failed" | tee -a ${logFILE}
echo "Logged output to: ${logFILE}"
if [ ${failed} -ne 0 ]; then exit 1; fi
exit 0
