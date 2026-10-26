#!/bin/bash
# Smart card tests for epdb_auth_tool.
#
# Adds smart card access to password databases, from the card and from
# exported PEM and DER cert files, and checks the card opens them.
# Works with a real card or a SoftHSM2 software token.
#
# Usage: SC_PIN=pin SC_CERT=cert.pem ./run_smartcard_tests.sh
#        SC_LIST_ONLY=1 ./run_smartcard_tests.sh
#
#   SC_PIN     Smart card PIN (required unless SC_LIST_ONLY=1)
#   SC_CERT    PEM cert exported from the card for the cert ID (required
#              unless SC_LIST_ONLY=1)
#   SC_ID      Smart card cert ID, default 01
#   SC_LABEL   Label of the same key and cert, tests label_ IDs (optional)
#   SC_MODULE  PKCS#11 module, default /usr/lib64/opensc-pkcs11.so
#   SC_EC_ID   ID of an EC key on the card, tests that it is refused (optional)
#   SC_EC_CERT PEM cert of an EC key, tests that it is refused (optional)
#   SC_LIST_ONLY=1  List the reader slots, token flags and objects with
#              pkcs11-tool and exit. Never logs in, no PIN is used.
#   SC_TEST_WRONG_PIN=1  Also run the PIN safeguard tests: wrong PINs,
#              low/final/locked PIN flags and C_Login counts. Refused
#              unless the token is a SoftHSM2 token, the tests run on a
#              throwaway copy of the SoftHSM2 token directory.
#
# Safety with a real card (CAC and PIV cards lock after 3 wrong PINs):
# * The script stops before any login if the token reports a locked PIN,
#   one try left or low tries, or if more than one card is present.
# * The first card test is a single open. If the card refuses the PIN or
#   any card login fails, the script stops at once: the PIN is never sent
#   again by this script. Check the PIN tries left before running again.
# * Wrong PINs are only ever sent to SoftHSM2 tokens.
#
# For a YubiKey, slot 9D is cert ID 03. For a CAC card with OpenSC the
# Key Management (encryption) cert is ID 03.

NEW_DIR=$(cd $(dirname $0) && pwd)
TOOL=${NEW_DIR}/epdb_auth_tool
TESTDB=${NEW_DIR}/epdb_test_db
testDIR=${HOME}/tmp/epdb_smartcard_testing_$(date +%Y_%m_%d_%H%M%S)
logFILE=${testDIR}/epdb_smartcard_testing.log
PW='Known-Pass#2026'
MODULE=${SC_MODULE:-/usr/lib64/opensc-pkcs11.so}
SPY=/usr/lib64/pkcs11-spy.so
PIN_ERRORS='PIN is incorrect|PIN is locked|login failed|PIN not sent|PIN error|PIN is empty|PIN may be wrong'
passed=0
failed=0

if ! command -v pkcs11-tool >/dev/null 2>&1; then
    echo "ERROR: pkcs11-tool is needed (dnf install opensc)"
    exit 1
fi
if [ ! -f "${MODULE}" ]; then
    echo "ERROR: PKCS#11 module ${MODULE} not found, set SC_MODULE"
    exit 1
fi

# Token information without a login
TOKEN_INFO=$(pkcs11-tool --module "${MODULE}" --list-token-slots 2>&1)
NUM_TOKENS=$(echo "${TOKEN_INFO}" | grep -c "token label")
IS_SOFTHSM=0
if echo "${TOKEN_INFO}" | grep -i "token manufacturer" | grep -qi "SoftHSM"; then IS_SOFTHSM=1; fi

if [ "${SC_LIST_ONLY}" = "1" ]; then
    echo "${TOKEN_INFO}"
    echo ""
    echo "Objects that can be read without a PIN:"
    pkcs11-tool --module "${MODULE}" --list-objects 2>&1
    echo ""
    echo "No login was made and no PIN was used."
    exit 0
fi

if [ -z "${SC_PIN}" ] || [ -z "${SC_CERT}" ] || [ ! -f "${SC_CERT}" ]; then
    echo "ERROR: Set SC_PIN and SC_CERT, see the usage at the top of $0"
    exit 1
fi
if [ "${SC_TEST_WRONG_PIN}" = "1" ] && [ ${IS_SOFTHSM} -ne 1 ]; then
    echo "ERROR: SC_TEST_WRONG_PIN=1 only runs with a SoftHSM2 token, never with a real card"
    exit 1
fi
if [ ${NUM_TOKENS} -ne 1 ]; then
    echo "${TOKEN_INFO}"
    echo "ERROR: Found ${NUM_TOKENS} tokens, insert exactly one card (and no other cards or YubiKeys)"
    exit 1
fi
if echo "${TOKEN_INFO}" | grep -i "token flags" | grep -qiE "PIN locked|final try|count low"; then
    echo "${TOKEN_INFO}" | grep -i "token flags"
    echo "ERROR: The card reports a locked PIN, one PIN try left or low PIN tries. No PIN was sent."
    echo "       Check the PIN tries left (for example in ActivClient, open the card and view its smart card info) and fix that first."
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
    # 10/26/2026: The default makefile is RHEL 10, use the RHEL 8 or 9
    # makefile on those releases
    MAKEFILE=makefile
    RHEL_VERSION=$( (. /etc/os-release 2>/dev/null; echo ${VERSION_ID%%.*}) )
    if [ "${RHEL_VERSION}" = "8" ] || [ "${RHEL_VERSION}" = "9" ]; then MAKEFILE=makefile_rhel${RHEL_VERSION}; fi
    (cd ${NEW_DIR} && make -f ${MAKEFILE} PROJECT=epdb_test_db >/dev/null 2>&1)
    if [ ! -f ${TESTDB} ]; then
	echo "ERROR: Cannot build ${TESTDB}"
	exit 1
    fi
fi
SC_CERT=$(cd $(dirname ${SC_CERT}) && pwd)/$(basename ${SC_CERT})
[ -n "${SC_EC_CERT}" ] && SC_EC_CERT=$(cd $(dirname ${SC_EC_CERT}) && pwd)/$(basename ${SC_EC_CERT})
SC="--smartcard-cert-id=${SC_ID:-01} --smartcard-provider=${MODULE}"

mkdir -p ${testDIR}
cd ${testDIR}
cat /dev/null > ${logFILE}
umask 077
printf '%s\n' "${SC_PIN}" > pin.txt
printf '%s\n' "${PW}" > pw.txt
PIN="--smartcard-pin-file=pin.txt"
trap 'rm -f ${testDIR}/pin.txt ${testDIR}/badpin.txt ${testDIR}/longpin.txt ${testDIR}/tabpin.txt ${testDIR}/pw.txt' EXIT

pass() { echo "Passed: $1" | tee -a ${logFILE}; passed=$((passed+1)); }
fail() { echo "FAILED: $1" | tee -a ${logFILE}; failed=$((failed+1)); }
check() { if [ "$1" -eq 0 ]; then pass "$2"; else fail "$2"; fi; }

card() {
    # Runs epdb_auth_tool with the smart card, stdout to out.txt and
    # stderr to err.txt. Stops the script on any PIN or login error, so
    # the PIN is never sent to the card again.
    "${TOOL}" "$@" > out.txt 2> err.txt
    local rv=$?
    cat err.txt >> ${logFILE}
    if grep -qE "${PIN_ERRORS}" err.txt; then
	cat err.txt
	fail "smart card PIN or login error"
	echo "STOPPING: no more smart card logins are made. Check the PIN and the PIN tries left."
	echo "${passed} passed, ${failed} failed"
	echo "Logged output to: ${logFILE}"
	exit 2
    fi
    return ${rv}
}

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
check $? "add smart card access from the card (no PIN used)"
# The first PIN use: a single open. card() stops the script if it fails.
card --smartcard --smartcard-username=card ${SC} ${PIN} --db-list a.ehd
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open the v2 database with the smart card (first PIN use)"
if [ ${failed} -ne 0 ]; then
    echo "STOPPING after the first failure, no more smart card tests are run"
    exit 1
fi
${TOOL} --list-users a.ehd > out.txt 2>>${logFILE}
grep -q "card" out.txt && grep -qi "smart card" out.txt
check $? "--list-users shows the smart card user"
card --smartcard --smartcard-username=card ${SC} ${PIN} --db-find-key='Entry 003' a.ehd
[ $? -eq 0 ] && [ $(wc -l < out.txt) -eq 1 ] && grep -q "^Entry 003" out.txt
check $? "find a record with the smart card"
${TOOL} --password-file=pw.txt --db-list a.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "the password still opens the database after adding a card"
card --smartcard --smartcard-username=nobody ${SC} ${PIN} --db-list a.ehd
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "does not have access" err.txt
check $? "unknown smart card user is refused before any login"
${TOOL} --password-file=pw.txt --add-smartcard-cert ${SC} --add-username=card a.ehd > out.txt 2>err.txt
[ $? -ne 0 ] && grep -q "already exists" err.txt
check $? "a second card block for the same user is refused"

cp known.ehd b.ehd; cp known.ehx b.ehx
${TOOL} --password-file=pw.txt --add-smartcard-cert-file=${SC_CERT} --add-username=pemfile b.ehd >>${logFILE} 2>&1
check $? "add smart card access from a PEM cert file"
card --smartcard --smartcard-username=pemfile ${SC} ${PIN} --db-list b.ehd
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open the PEM cert file access with the smart card"
openssl x509 -in ${SC_CERT} -outform DER -out cert.cer 2>>${logFILE}
${TOOL} --password-file=pw.txt --add-smartcard-cert=cert.cer --add-username=derfile b.ehd >>${logFILE} 2>&1
check $? "add smart card access from a DER .cer file (old --add-smartcard-cert=file form)"
card --smartcard --smartcard-username=derfile ${SC} ${PIN} --db-list b.ehd
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
card --smartcard --smartcard-username=card ${SC} ${PIN} --db-list v1.ehd
[ $? -eq 0 ] && cmp -s out.txt list_v1.txt
check $? "open the v1 database with the smart card"
cp v1.ehd up.ehd; cp v1.ehx up.ehx
${TOOL} --password-file=pw.txt --upgrade-db up.ehd >>${logFILE} 2>&1
card --smartcard --smartcard-username=card ${SC} ${PIN} --db-list up.ehd
[ $? -eq 0 ] && cmp -s out.txt list_v1.txt
check $? "the v1 card block opens the database after --upgrade-db"

if [ -n "${SC_LABEL}" ]; then
    card --smartcard --smartcard-username=card "--smartcard-cert-id=label_${SC_LABEL}" --smartcard-provider=${MODULE} ${PIN} --db-list a.ehd
    [ $? -eq 0 ] && cmp -s out.txt list.txt
    check $? "open with a label_ cert ID"
fi
URI_ID=$(printf '%s' "${SC_ID:-01}" | sed 's/\(..\)/%\1/g')
card --smartcard --smartcard-username=card "--smartcard-cert-id=pkcs11:id=${URI_ID}" --smartcard-provider=${MODULE} ${PIN} --db-list a.ehd
[ $? -eq 0 ] && cmp -s out.txt list.txt
check $? "open with a pkcs11: URI cert ID"
card --smartcard --smartcard-username=card ${SC} ${PIN} --smartcard-provider=/no/such/module.so --db-list a.ehd
[ $? -eq 1 ] && [ ! -s out.txt ] && grep -q "does not exist" err.txt
check $? "a missing PKCS#11 module is refused"

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

if [ "${SC_TEST_WRONG_PIN}" = "1" ] && [ ${IS_SOFTHSM} -eq 1 ]; then
    # PIN safeguard tests, SoftHSM2 only. They run on a throwaway copy
    # of the token directory. C_Login calls are counted with the OpenSC
    # pkcs11-spy module, and p11_flag_shim.so adds the low, final try and
    # locked PIN flags that SoftHSM2 does not set itself.
    if [ ! -f ${SPY} ]; then
	fail "PIN safeguard tests need ${SPY} (opensc)"
    else
	TOKDIR=$(awk -F= '/^ *directories.tokendir/ { gsub(/ /, "", $2); print $2 }' "${SOFTHSM2_CONF:-/etc/softhsm2.conf}")
	mkdir -p ${testDIR}/tokens
	cp -a ${TOKDIR}/. ${testDIR}/tokens/
	printf 'directories.tokendir = %s\nobjectstore.backend = file\nlog.level = ERROR\n' "${testDIR}/tokens" > ${testDIR}/softhsm2.conf
	export SOFTHSM2_CONF=${testDIR}/softhsm2.conf
	SHIM=${testDIR}/p11_flag_shim.so
	gcc -shared -fPIC -I${NEW_DIR}/../../fcrypt/include -o ${SHIM} ${NEW_DIR}/p11_flag_shim.c -ldl >>${logFILE} 2>&1
	SPYSC="--smartcard-cert-id=${SC_ID:-01} --smartcard-provider=${SPY}"
	logins() { if [ -f spy.log ]; then grep -c "^[0-9]*: C_Login$" spy.log; else echo 0; fi; }
	spyrun() {
	    # spyrun flags args... : runs through pkcs11-spy -> shim -> SoftHSM2
	    local flags=$1; shift
	    rm -f spy.log
	    PKCS11SPY=${SHIM} PKCS11SPY_OUTPUT=${testDIR}/spy.log EPDB_SHIM_MODULE=${MODULE} EPDB_SHIM_FLAGS=${flags} \
		"${TOOL}" "$@" > out.txt 2> err.txt
	    local rv=$?
	    cat err.txt >> ${logFILE}
	    echo "C_Login calls: $(logins)" >> ${logFILE}
	    return ${rv}
	}
	printf 'wrong-%s\n' "${SC_PIN}" > badpin.txt
	printf 'x%.0s' $(seq 1 40) > longpin.txt; echo >> longpin.txt
	printf '%s\t\n' "${SC_PIN}" > tabpin.txt
	cp a.ehd a2.ehd; cp a.ehx a2.ehx

	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd
	[ $? -eq 0 ] && cmp -s out.txt list.txt && [ "$(logins)" = "1" ]
	check $? "exactly one C_Login for one database open"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd a2.ehd
	[ $? -eq 0 ] && [ "$(logins)" = "2" ]
	check $? "one C_Login per database for two databases"
	spyrun 40000 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd
	[ $? -ne 0 ] && [ ! -s out.txt ] && [ "$(logins)" = "0" ] && grep -q "PIN is locked" err.txt
	check $? "locked PIN flag: no PIN is sent"
	spyrun 20000 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd
	[ $? -ne 0 ] && [ ! -s out.txt ] && [ "$(logins)" = "0" ] && grep -q "ONE PIN try is left" err.txt && grep -q "smartcard-allow-final-try" err.txt
	check $? "final try PIN flag: no PIN is sent without --smartcard-allow-final-try"
	spyrun 20000 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --smartcard-allow-final-try --db-list a.ehd
	[ $? -eq 0 ] && cmp -s out.txt list.txt && [ "$(logins)" = "1" ]
	check $? "final try PIN flag: --smartcard-allow-final-try sends the PIN once"
	spyrun 10000 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd < /dev/null
	[ $? -ne 0 ] && [ ! -s out.txt ] && [ "$(logins)" = "0" ] && grep -q "smartcard-allow-pin-count-low" err.txt
	check $? "low PIN tries flag: no PIN is sent when not interactive"
	spyrun 10000 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --smartcard-allow-pin-count-low --db-list a.ehd
	[ $? -eq 0 ] && cmp -s out.txt list.txt && [ "$(logins)" = "1" ]
	check $? "low PIN tries flag: --smartcard-allow-pin-count-low sends the PIN once"
	printf '\n' | spyrun 0 --smartcard --smartcard-username=card ${SPYSC} --db-list a.ehd
	[ $? -ne 0 ] && [ "$(logins)" = "0" ] && grep -q "empty" err.txt
	check $? "an empty PIN at the prompt is not sent"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} --smartcard-pin-file=longpin.txt --db-list a.ehd
	[ $? -ne 0 ] && [ "$(logins)" = "0" ] && grep -q "longer than" err.txt
	check $? "a PIN that does not fit is not cut short or sent"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} --smartcard-pin-file=tabpin.txt --db-list a.ehd
	[ $? -ne 0 ] && [ "$(logins)" = "0" ] && grep -q "control character" err.txt
	check $? "a PIN with a control character is not sent"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} --smartcard-pin-file=badpin.txt --db-list a.ehd a2.ehd
	[ $? -ne 0 ] && [ ! -s out.txt ] && [ "$(logins)" = "1" ] && [ $(grep -c "PIN is incorrect" err.txt) -eq 1 ] && grep -q "ONE PIN try was used" err.txt
	check $? "a wrong PIN: one C_Login for two databases, the run stops"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd < /dev/null
	[ $? -ne 0 ] && [ "$(logins)" = "0" ] && grep -q "low on PIN tries" err.txt
	check $? "after a wrong PIN SoftHSM2 reports low tries: the next run does not send the PIN"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --smartcard-allow-pin-count-low --db-list a.ehd
	[ $? -eq 0 ] && cmp -s out.txt list.txt && [ "$(logins)" = "1" ]
	check $? "the right PIN works with --smartcard-allow-pin-count-low after a wrong PIN"
	# A second token: an unknown cert ID must not be guessed
	SOFTHSM_UTIL=$(dirname ${MODULE})/../../bin/softhsm2-util
	[ -x ${SOFTHSM_UTIL} ] || SOFTHSM_UTIL=softhsm2-util
	${SOFTHSM_UTIL} --module ${MODULE} --init-token --free --label second --so-pin 87654321 --pin 123456 >>${logFILE} 2>&1
	spyrun 0 --smartcard --smartcard-username=card --smartcard-cert-id=7e --smartcard-provider=${SPY} ${PIN} --db-list a.ehd
	[ $? -ne 0 ] && [ "$(logins)" = "0" ] && grep -q "more than one card" err.txt
	check $? "two cards and an unknown cert ID: no PIN is sent to a guessed card"
	spyrun 0 --smartcard --smartcard-username=card ${SPYSC} ${PIN} --db-list a.ehd
	[ $? -eq 0 ] && cmp -s out.txt list.txt && [ "$(logins)" = "1" ]
	check $? "two cards: the card with the cert ID is used, one C_Login"
    fi
fi

echo ""
echo "${passed} passed, ${failed} failed"
echo "Logged output to: ${logFILE}"
[ ${failed} -eq 0 ]
