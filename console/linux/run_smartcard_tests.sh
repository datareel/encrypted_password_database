#!/bin/bash
# Smart card tests for epdb_auth_tool.
#
# Adds smart card access to password databases, from the card and from
# exported PEM and DER cert files, and checks the card opens them.
# Works with a real card or a software token such as SoftHSM2.
#
# Usage: SC_PIN=pin SC_CERT=cert.pem ./run_smartcard_tests.sh
#
#   SC_PIN     Smart card PIN (required). Use a test card, a wrong PIN
#              uses up the card's PIN tries.
#   SC_CERT    PEM cert exported from the card for the cert ID (required)
#   SC_ID      Smart card cert ID, default 01
#   SC_LABEL   Label of the same key and cert, tests label_ IDs (optional)
#   SC_MODULE  PKCS#11 module, default /usr/lib64/opensc-pkcs11.so
#   SC_EC_ID   ID of an EC key on the card, tests that it is refused (optional)
#   SC_EC_CERT PEM cert of an EC key, tests that it is refused (optional)
#   SC_TEST_WRONG_PIN=1  Also test a wrong PIN. This uses up one PIN try,
#              only use it with a software token or a card you can reset.
#
# For a YubiKey, slot 9D is cert ID 03. For a CAC card with OpenSC the
# Key Management (encryption) cert is ID 03.

NEW_DIR=$(cd $(dirname $0) && pwd)
TOOL=${NEW_DIR}/epdb_auth_tool
TESTDB=${NEW_DIR}/epdb_test_db
testDIR=${HOME}/tmp/epdb_smartcard_testing_$(date +%Y_%m_%d_%H%M%S)
logFILE=${testDIR}/epdb_smartcard_testing.log
PW='Known-Pass#2026'
passed=0
failed=0

if [ -z "${SC_PIN}" ] || [ -z "${SC_CERT}" ] || [ ! -f "${SC_CERT}" ]; then
    echo "ERROR: Set SC_PIN and SC_CERT, see the usage at the top of $0"
    exit 1
fi
if [ ! -f ${TOOL} ]; then
    echo "ERROR: Missing ${TOOL}, run make first"
    exit 1
fi
if ! ${TOOL} --version 2>&1 | grep -q "Smart card enabled"; then
    echo "ERROR: ${TOOL} was built without smart card support"
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
SC_CERT=$(cd $(dirname ${SC_CERT}) && pwd)/$(basename ${SC_CERT})
[ -n "${SC_EC_CERT}" ] && SC_EC_CERT=$(cd $(dirname ${SC_EC_CERT}) && pwd)/$(basename ${SC_EC_CERT})
MODULE=${SC_MODULE:-/usr/lib64/opensc-pkcs11.so}
SC="--smartcard-cert-id=${SC_ID:-01} --smartcard-provider=${MODULE}"

mkdir -p ${testDIR}
cd ${testDIR}
cat /dev/null > ${logFILE}
umask 077
printf '%s\n' "${SC_PIN}" > pin.txt
printf '%s\n' "${PW}" > pw.txt
PIN="--smartcard-pin-file=pin.txt"

pass() { echo "Passed: $1" | tee -a ${logFILE}; passed=$((passed+1)); }
fail() { echo "FAILED: $1" | tee -a ${logFILE}; failed=$((failed+1)); }
check() { if [ "$1" -eq 0 ]; then pass "$2"; else fail "$2"; fi; }

echo "Starting tests at $(date)" | tee -a ${logFILE}
echo "Module ${MODULE}, cert ID ${SC_ID:-01}" | tee -a ${logFILE}

${TESTDB} create known.ehd "${PW}" 12 >>${logFILE} 2>&1
check $? "create a format v2 test database"
${TESTDB} create-v1 known_v1.ehd "${PW}" 12 >>${logFILE} 2>&1
check $? "create a format v1 test database"
${TOOL} --password-file=pw.txt --db-list known.ehd > list.txt 2>>${logFILE}
[ $? -eq 0 ] && [ $(wc -l < list.txt) -eq 12 ]
check $? "list with the password"

cp known.ehd a.ehd; cp known.ehx a.ehx
${TOOL} --password-file=pw.txt --add-smartcard-cert ${SC} --add-username=card a.ehd >>${logFILE} 2>&1
check $? "add smart card access from the card"
${TOOL} --list-users a.ehd > out.txt 2>>${logFILE}
grep -q "card" out.txt && grep -qi "smart card" out.txt
check $? "--list-users shows the smart card user"
${TOOL} --smartcard --smartcard-username=card ${SC} ${PIN} --db-list a.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open the v2 database with the smart card"
${TOOL} --smartcard --smartcard-username=card ${SC} ${PIN} --db-find-key='Entry 003' a.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && [ $(wc -l < out.txt) -eq 1 ] && grep -q "^Entry 003" out.txt
check $? "find a record with the smart card"
${TOOL} --password-file=pw.txt --db-list a.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "the password still opens the database after adding a card"
${TOOL} --smartcard --smartcard-username=nobody ${SC} ${PIN} --db-list a.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "does not have access" err.txt
check $? "unknown smart card user is refused"
${TOOL} --password-file=pw.txt --add-smartcard-cert ${SC} --add-username=card a.ehd > out.txt 2>err.txt
[ $? -ne 0 ] && grep -q "already exists" err.txt
check $? "a second card block for the same user is refused"
printf '%s\n' "${SC_PIN}" | ${TOOL} --smartcard --smartcard-username=card ${SC} --db-list a.ehd > out.txt 2>err.txt
[ $? -eq 0 ] && cmp -s out.txt list.txt && grep -q "Smart card PIN:" err.txt
check $? "PIN prompt, prompt on stderr"

cp known.ehd b.ehd; cp known.ehx b.ehx
${TOOL} --password-file=pw.txt --add-smartcard-cert-file=${SC_CERT} --add-username=pemfile b.ehd >>${logFILE} 2>&1
check $? "add smart card access from a PEM cert file"
${TOOL} --smartcard --smartcard-username=pemfile ${SC} ${PIN} --db-list b.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open the PEM cert file access with the smart card"
openssl x509 -in ${SC_CERT} -outform DER -out cert.cer 2>>${logFILE}
${TOOL} --password-file=pw.txt --add-smartcard-cert=cert.cer --add-username=derfile b.ehd >>${logFILE} 2>&1
check $? "add smart card access from a DER .cer file (old --add-smartcard-cert=file form)"
${TOOL} --smartcard --smartcard-username=derfile ${SC} ${PIN} --db-list b.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open the DER cert file access with the smart card"
head -c 600 /dev/urandom > junk.cer
${TOOL} --password-file=pw.txt --add-smartcard-cert-file=junk.cer --add-username=junk b.ehd > out.txt 2>err.txt
[ $? -ne 0 ] && grep -q "not a PEM or DER" err.txt
check $? "a cert file that is not a cert is refused"

cp known_v1.ehd v1.ehd; cp known_v1.ehx v1.ehx
${TOOL} --password-file=pw.txt --db-list v1.ehd > list_v1.txt 2>>${logFILE}
${TOOL} --password-file=pw.txt --add-smartcard-cert ${SC} --add-username=card v1.ehd >>${logFILE} 2>&1
check $? "add smart card access to a v1 database (PKCS#1 v1.5 block)"
${TOOL} --smartcard --smartcard-username=card ${SC} ${PIN} --db-list v1.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list_v1.txt
check $? "open the v1 database with the smart card"
cp v1.ehd up.ehd; cp v1.ehx up.ehx
${TOOL} --password-file=pw.txt --upgrade-db up.ehd >>${logFILE} 2>&1 && \
    ${TOOL} --smartcard --smartcard-username=card ${SC} ${PIN} --db-list up.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list_v1.txt
check $? "the v1 card block opens the database after --upgrade-db"

if [ -n "${SC_LABEL}" ]; then
    ${TOOL} --smartcard --smartcard-username=card "--smartcard-cert-id=label_${SC_LABEL}" --smartcard-provider=${MODULE} ${PIN} --db-list a.ehd > out.txt 2>>${logFILE}
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "open with a label_ cert ID"
fi
URI_ID=$(printf '%s' "${SC_ID:-01}" | sed 's/\(..\)/%\1/g')
${TOOL} --smartcard --smartcard-username=card "--smartcard-cert-id=pkcs11:id=${URI_ID}" --smartcard-provider=${MODULE} ${PIN} --db-list a.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open with a pkcs11: URI cert ID"
${TOOL} --smartcard --smartcard-username=card ${SC} ${PIN} --smartcard-provider=/no/such/module.so --db-list a.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "does not exist" err.txt
check $? "a missing PKCS#11 module is refused"
${TOOL} --smartcard --smartcard-username=card --smartcard-cert-id=7e --smartcard-provider=${MODULE} ${PIN} --db-list a.ehd > out.txt 2>err.txt
[ $? -eq 1 ] && [ ! -s out.txt ]
check $? "a cert ID that is not on the card fails"

if [ -n "${SC_EC_CERT}" ]; then
    ${TOOL} --password-file=pw.txt --add-smartcard-cert-file=${SC_EC_CERT} --add-username=ec b.ehd > out.txt 2>err.txt
    [ $? -ne 0 ] && grep -q "EC keys are not supported" err.txt
    check $? "an EC cert file is refused with a clear message"
fi
if [ -n "${SC_EC_ID}" ]; then
    ${TOOL} --password-file=pw.txt --add-smartcard-cert --smartcard-cert-id=${SC_EC_ID} --smartcard-provider=${MODULE} --add-username=ec b.ehd > out.txt 2>err.txt
    [ $? -ne 0 ] && grep -q "EC keys are not supported" err.txt
    check $? "an EC key on the card is refused with a clear message"
fi

if [ "${SC_TEST_WRONG_PIN}" = "1" ]; then
    printf 'wrong-%s\n' "${SC_PIN}" > badpin.txt
    cp a.ehd a2.ehd; cp a.ehx a2.ehx
    ${TOOL} --smartcard --smartcard-username=card ${SC} --smartcard-pin-file=badpin.txt --db-list a.ehd a2.ehd > out.txt 2>err.txt
    rv=$?
    [ ${rv} -ne 0 ] && [ ! -s out.txt ] && grep -q "PIN is incorrect" err.txt && [ $(grep -c "PIN is incorrect" err.txt) -eq 1 ]
    check $? "a wrong PIN is refused and not tried on the next file"
    ${TOOL} --smartcard --smartcard-username=card ${SC} ${PIN} --db-list a.ehd > out.txt 2>>${logFILE}
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "the right PIN works after a wrong PIN"
fi

rm -f pin.txt badpin.txt pw.txt
echo ""
echo "${passed} passed, ${failed} failed"
echo "Logged output to: ${logFILE}"
[ ${failed} -eq 0 ]
