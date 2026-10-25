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
check $? "create the test database"
${TESTDB} dump known.ehd "${PW}" > dump.txt 2>>${logFILE}
[ $? -eq 0 ] && [ $(wc -l < dump.txt) -eq 12 ]
check $? "dump the test database"

# Key check and exit codes
${TOOL} --password="${PW}" --db-list known.ehd > list.txt 2>>${logFILE}
[ $? -eq 0 ] && [ $(wc -l < list.txt) -eq 12 ]
check $? "list with the right password"
${TOOL} --password="${WRONG}" --db-list known.ehd > out.txt 2>err.txt
rv=$?
[ ${rv} -eq 1 ] && [ ! -s out.txt ] && grep -q "Error decrypting database hash" err.txt
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

# Damaged and crafted files must fail cleanly, never crash
cp known.ehd t.ehd; cp known.ehx t.ehx
tamper() { # tamper name python_statement
    cp known.ehd $1.ehd; cp known.ehx $1.ehx
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
    OLD=${OLD_DIR}/epdb_auth_tool
    ${OLD} --password="${PW}" --db-list known.ehd > old_list.txt 2>>${logFILE}
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
