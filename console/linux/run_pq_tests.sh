#!/bin/bash
# 11/18/2026: Tests for the release 2026.107 quantum-resistant (hybrid)
# users: an RSA key or a smart card plus the user's ML-KEM-1024 key.
#
# Usage: ./run_pq_tests.sh [old_build_dir]
#
# Run after building epdb_auth_tool with make in this directory (the
# script builds epdb_test_db). Makes its test databases in a new
# directory under ~/tmp and never uses any other database.
#
#   old_build_dir  a console/linux directory with an older epdb_auth_tool
#                  (2026.106 or older): checks that it still opens the
#                  database with the password and a plain RSA user, refuses
#                  the hybrid user, and keeps hybrid users when it adds a
#                  user.
#   SC_MODULE, SC_PIN, SC_CERT  a SoftHSM2 token with an RSA key and its
#                  cert (cert ID SC_ID, default 01) for the smart card +
#                  ML-KEM-1024 checks. Never a real card: the script sends
#                  a PIN. Without them the smart card checks are skipped.

NEW_DIR=$(cd $(dirname $0) && pwd)
TOOL=${NEW_DIR}/epdb_auth_tool
TESTDB=${NEW_DIR}/epdb_test_db
OLD_DIR=$1
OLD=${OLD_DIR:+${OLD_DIR}/epdb_auth_tool}
testDIR=${HOME}/tmp/epdb_pq_testing_$(date +%Y_%m_%d_%H%M%S)
logFILE=${testDIR}/epdb_pq_testing.log
PW='Known-Pass#2026'
passed=0
failed=0

check() {
    if [ $1 -eq 0 ]; then echo "Passed: $2"; passed=$((passed+1))
    else echo "FAILED: $2"; failed=$((failed+1)); fi
    echo "check $1: $2" >> ${logFILE}
}

if [ ! -x ${TOOL} ]; then echo "ERROR: build epdb_auth_tool first"; exit 1; fi
if [ ! -x ${TESTDB} ]; then
    # The makefile of this RHEL release (RHEL 8 builds with the openssl3 headers)
    V=$(. /etc/os-release 2>/dev/null; echo ${VERSION_ID%%.*})
    MK=makefile_rhel${V}; [ -f ${NEW_DIR}/${MK} ] || MK=makefile
    (cd ${NEW_DIR} && make -f ${MK} PROJECT=epdb_test_db > /dev/null 2>&1)
fi
if [ ! -x ${TESTDB} ]; then echo "ERROR: cannot build epdb_test_db"; exit 1; fi
mkdir -p ${testDIR} && chmod 700 ${testDIR} && cd ${testDIR} || exit 1
echo "Testing in ${testDIR}"

printf '%s' "${PW}" > pw.txt
printf 'rsa-pass-2026' > pp.txt
printf 'pq-pass-2026-long' > pqpp.txt
printf 'pq-pass-wrong-long' > pqbad.txt
chmod 600 pw.txt pp.txt pqpp.txt pqbad.txt
${TESTDB} create db.ehd "${PW}" 12 >> ${logFILE} 2>&1
check $? "create the test database"
${TOOL} --password-file=pw.txt --db-list db.ehd > list.txt 2>> ${logFILE}
openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -aes-256-cbc -pass pass:rsa-pass-2026 -out alice_priv.pem > /dev/null 2>&1
openssl pkey -in alice_priv.pem -passin pass:rsa-pass-2026 -pubout -out alice_pub.pem > /dev/null 2>&1
openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out carl_priv.pem > /dev/null 2>&1
openssl pkey -in carl_priv.pem -pubout -out carl_pub.pem > /dev/null 2>&1
chmod 600 alice_priv.pem carl_priv.pem

ALICE="--rsa-key=alice_priv.pem --rsa-key-username=alice --rsa-key-passphrase-file=pp.txt"

# ---- Keys
${TOOL} --generate-pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt >> ${logFILE} 2>&1
[ $? -eq 0 ] && [ -s alice_pq.pem ] && [ -s alice_pq.pem.pub.pem ] && [ "$(stat -c %a alice_pq.pem)" = "600" ]
check $? "--generate-pq-key writes the private key (mode 600) and file.pub.pem"
grep -q "ENCRYPTED PRIVATE KEY" alice_pq.pem
check $? "the ML-KEM private key file is encrypted with the passphrase"
${TOOL} --generate-pq-key=carl_pq.pem --pq-key-passphrase-file=pqpp.txt >> ${logFILE} 2>&1
${TOOL} --generate-pq-key=nopass_pq.pem --non-interactive > /dev/null 2>&1 < /dev/null
[ $? -ne 0 ] && [ ! -e nopass_pq.pem ]
check $? "--generate-pq-key without a passphrase is refused with --non-interactive"

# ---- Add and open
${TOOL} --password-file=pw.txt --add-rsa-key=alice_pub.pem --add-username=alice --add-pq-public-key=alice_pq.pem.pub.pem db.ehd >> ${logFILE} 2>&1
check $? "add alice as an RSA key + ML-KEM-1024 user"
${TOOL} --list-users db.ehd | grep -q "Username: alice Access: RSA key + ML-KEM-1024"
check $? "--list-users shows the hybrid user"
${TOOL} --password-file=pw.txt --add-rsa-key=carl_pub.pem --add-username=carl db.ehd >> ${logFILE} 2>&1
check $? "add carl as a plain RSA user"
${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list db.ehd > out.txt 2>> ${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "alice opens the database with her RSA key and ML-KEM key"
${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-fd=4 --db-list db.ehd 4<pqpp.txt > out.txt 2>> ${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "--pq-key-passphrase-fd"
${TOOL} ${ALICE} --non-interactive --db-list db.ehd > out.txt 2> err.txt
[ $? -ne 0 ] && [ ! -s out.txt ] && grep -q "give the user's ML-KEM key file" err.txt
check $? "alice without --pq-key is refused with a clear message"
${TOOL} ${ALICE} --pq-key=carl_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list db.ehd > out.txt 2> err.txt
[ $? -eq 2 ] && [ ! -s out.txt ] && grep -q "authentication failed" err.txt
check $? "alice with another user's ML-KEM key is refused (exit 2)"
${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqbad.txt --db-list db.ehd > out.txt 2> err.txt
[ $? -ne 0 ] && [ ! -s out.txt ] && grep -q "wrong passphrase" err.txt
check $? "a wrong ML-KEM key passphrase is refused"
${TOOL} --rsa-key=carl_priv.pem --rsa-key-username=alice --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list db.ehd > out.txt 2>> ${logFILE}
[ $? -ne 0 ] && [ ! -s out.txt ]
check $? "another RSA key with alice's ML-KEM key is refused"
${TOOL} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --password-file=pw.txt --db-list db.ehd > out.txt 2> err.txt
[ $? -ne 0 ] && grep -q "together with --rsa-key or --smartcard" err.txt
check $? "--pq-key without --rsa-key or --smartcard is refused"
${TOOL} --rsa-key=carl_priv.pem --rsa-key-username=carl --db-list db.ehd > out.txt 2>> ${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "the plain RSA user still opens the database"
${TOOL} --password-file=pw.txt --add-rsa-key=carl_pub.pem --add-username=carl --add-pq-public-key=carl_pq.pem.pub.pem db.ehd > /dev/null 2>&1
[ $? -ne 0 ] && [ $(${TOOL} --list-users db.ehd | grep -c "Username: carl") -eq 1 ]
check $? "a user cannot get a second (hybrid) RSA block"
${TOOL} --password-file=pw.txt --add-rsa-key=alice_pub.pem --add-username=dan --require-pq db.ehd > /dev/null 2>&1
[ $? -ne 0 ] && ! ${TOOL} --list-users db.ehd | grep -q "Username: dan"
check $? "--require-pq refuses to add an RSA-only user"
${TOOL} --password-file=pw.txt --db-info --json db.ehd | grep -q '"quantum_resistant_users": 1'
check $? "--db-info --json counts the quantum-resistant users"

# ---- Rekey
cp db.ehd rk.ehd; cp db.ehx rk.ehx
printf 'password=%s\nrsa:alice=alice_pub.pem\nrsa:carl=carl_pub.pem\n' "${PW}" | ${TOOL} --password-file=pw.txt --rekey rk.ehd > /dev/null 2> err.txt
[ $? -ne 0 ] && grep -q "is a quantum-resistant user" err.txt && cmp -s db.ehd rk.ehd
check $? "a rekey without pq= for a hybrid user is refused, nothing changed"
printf 'password=%s\nrsa:alice=alice_pub.pem\nrsa:carl=carl_pub.pem\n' "${PW}" | ${TOOL} --password-file=pw.txt --require-pq --rekey rk.ehd > /dev/null 2>&1
[ $? -ne 0 ] && cmp -s db.ehd rk.ehd
check $? "--require-pq refuses a rekey that keeps an RSA-only user"
printf 'password=%s\nrsa:alice=alice_pub.pem,pq=alice_pq.pem.pub.pem\nrsa:carl=carl_pub.pem,pq=carl_pq.pem.pub.pem\n' "${PW}" | \
    ${TOOL} --password-file=pw.txt --require-pq --rekey rk.ehd >> ${logFILE} 2>&1
check $? "rekey keeps alice and moves carl to RSA key + ML-KEM-1024"
[ $(${TOOL} --list-users rk.ehd | grep -c "Access: RSA key + ML-KEM-1024") -eq 2 ]
check $? "both users are hybrid after the rekey"
${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list rk.ehd > out.txt 2>> ${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "alice opens the rekeyed database"
${TOOL} --rsa-key=carl_priv.pem --rsa-key-username=carl --pq-key=carl_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list rk.ehd > out.txt 2>> ${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "carl opens the rekeyed database as a hybrid user"
${TOOL} --rsa-key=carl_priv.pem --rsa-key-username=carl --db-list rk.ehd > out.txt 2>> ${logFILE}
[ $? -ne 0 ] && [ ! -s out.txt ]
check $? "carl's RSA key alone no longer opens it"

# ---- Remove
cp db.ehd rm.ehd; cp db.ehx rm.ehx
${TOOL} --password-file=pw.txt --remove-user=alice --yes rm.ehd >> ${logFILE} 2>&1
[ $? -eq 0 ] && ! ${TOOL} --list-users rm.ehd | grep -q "Username: alice"
check $? "--remove-user removes the hybrid user"
${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list rm.ehd > out.txt 2>> ${logFILE}
[ $? -ne 0 ] && [ ! -s out.txt ]
check $? "the removed hybrid user is refused"

# ---- Tampering
cp db.ehd tm.ehd; cp db.ehx tm.ehx
python3 - tm.ehd <<'PY' >> ${logFILE} 2>&1
import sys
d = bytearray(open(sys.argv[1], 'rb').read())
i = d.find(b'\x00\x00\x00\x01\x00\x00\x04\x00')   # hybrid block: version 1, ML-KEM-1024
if i < 0: sys.exit(1)
d[i + 20 + 300] ^= 1                                # a byte of the RSA / ML-KEM ciphertexts
open(sys.argv[1], 'wb').write(d)
PY
if [ $? -eq 0 ]; then
    ${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list tm.ehd > out.txt 2>> ${logFILE}
    [ $? -ne 0 ] && [ ! -s out.txt ]
    check $? "a changed byte in the hybrid block is refused"
else
    check 1 "find the hybrid block to change a byte"
fi

# ---- Smart card + ML-KEM-1024 (SoftHSM2 only)
if [ -n "${SC_MODULE}" ] && [ -n "${SC_PIN}" ] && [ -n "${SC_CERT}" ]; then
    printf '%s' "${SC_PIN}" > pin.txt; chmod 600 pin.txt
    SC="--smartcard --smartcard-provider=${SC_MODULE} --smartcard-cert-id=${SC_ID:-01} --smartcard-username=erin --smartcard-pin-file=pin.txt"
    cp db.ehd sc.ehd; cp db.ehx sc.ehx
    ${TOOL} --password-file=pw.txt --add-smartcard-cert-file=${SC_CERT} --add-username=erin --add-pq-public-key=carl_pq.pem.pub.pem sc.ehd >> ${logFILE} 2>&1
    [ $? -eq 0 ] && ${TOOL} --list-users sc.ehd | grep -q "Username: erin Access: Smart Card + ML-KEM-1024"
    check $? "add erin as a Smart Card + ML-KEM-1024 user (cert file, no PIN)"
    ${TOOL} ${SC} --pq-key=carl_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list sc.ehd > out.txt 2>> ${logFILE}
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "erin opens the database with the card and the ML-KEM key"
    ${TOOL} ${SC} --non-interactive --db-list sc.ehd > out.txt 2> err.txt
    [ $? -ne 0 ] && grep -q "give the user's ML-KEM key file" err.txt
    check $? "erin without --pq-key is refused before the card is used"
    printf 'wrongpin99' > badpin.txt; chmod 600 badpin.txt
    ${TOOL} ${SC/pin.txt/badpin.txt} --pq-key=carl_pq.pem --pq-key-passphrase-file=pqbad.txt --db-list sc.ehd > out.txt 2> err.txt
    [ $? -ne 0 ] && grep -q "wrong passphrase" err.txt && ! pkcs11-tool --module ${SC_MODULE} -T 2>/dev/null | grep -q "count low"
    check $? "a wrong ML-KEM passphrase stops before any PIN is sent"
else
    echo "Skipped: smart card + ML-KEM-1024 checks (set SC_MODULE, SC_PIN and SC_CERT for a SoftHSM2 token)"
fi

# ---- Older releases
if [ -n "${OLD}" ] && [ -x "${OLD}" ]; then
    ${OLD} --password-file=pw.txt --db-list db.ehd > out.txt 2>> ${logFILE}
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "the old build opens a database with hybrid users with the password"
    ${OLD} --rsa-key=carl_priv.pem --rsa-key-username=carl --db-list db.ehd > out.txt 2>> ${logFILE}
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "the old build opens it with a plain RSA user"
    ${OLD} ${ALICE} --db-list db.ehd > out.txt 2>> ${logFILE}
    [ $? -ne 0 ] && [ ! -s out.txt ]
    check $? "the old build refuses the hybrid user"
    cp db.ehd old.ehd; cp db.ehx old.ehx
    openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out dan_priv.pem > /dev/null 2>&1; openssl pkey -in dan_priv.pem -pubout -out dan_pub.pem > /dev/null 2>&1
    ${OLD} --password-file=pw.txt --add-rsa-key=dan_pub.pem --add-username=dan old.ehd >> ${logFILE} 2>&1
    ${TOOL} ${ALICE} --pq-key=alice_pq.pem --pq-key-passphrase-file=pqpp.txt --db-list old.ehd > out.txt 2>> ${logFILE}
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "a user added by the old build keeps the hybrid user"
fi

echo
echo "${passed} passed, ${failed} failed"
echo "Logged output to: ${logFILE}"
[ ${failed} -eq 0 ]
