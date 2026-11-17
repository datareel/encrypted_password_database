#!/bin/bash
# 10/31/2026: Crash tests for crash-safe saves (src/epdb_journal.cpp).
#
# Usage: ./run_journal_tests.sh [rounds] [old_build_dir]
#
# Run after building epdb_auth_tool with make in this directory. Builds
# epdb_test_db and epdb_journal_test, makes test databases in a new
# directory under ~/tmp and never uses any other database.
#
#  1. GUI-style saves (add, change, delete, config, access log, MAC in
#     each save) killed with kill -9 at random moments, "rounds" times
#     (default 60). After each kill the database is opened, which
#     finishes an interrupted save, and checked: MAC, index lists
#     exactly the data records, index order, every record decrypts,
#     and the records, config and log are the state after the last
#     reported save or the one after it, nothing in between.
#  2. epdb_auth_tool --add-record (temporary files renamed over the
#     database) killed at random moments: the database holds the
#     records before or after the change and passes its integrity check.
#  3. The journal file is mode 600 during a save and removed after it.
#  4. A database with an unfinished save that cannot be written opens
#     read-only with a warning, and no journal file is made.
#  5. With an old build directory (a console/linux directory with an
#     older epdb_auth_tool and epdb_test_db): a database made by the old
#     build opens unchanged, and a database saved by this build opens in
#     the old build.
#  6. The time of a save, with and without the journal.
#  7. Writer lock: while one program has the database open for writing,
#     a second read-write open and a record change are refused with
#     "open in another program", a read-only open works, and after a
#     kill -9 of the writer the next open gets the lock and finishes
#     its save.
#  8. A journal that another user could have written is not replayed:
#     the journal's directory writable by the group, or a journal that
#     names another database's files. The database opens read-only with
#     a warning and is not changed.
NEW_DIR=$(cd $(dirname $0) && pwd)
ROUNDS=${1:-60}
OLD_DIR=$2
MAKEFILE=makefile
if [ -f /etc/redhat-release ] && grep -qi Coughlan /etc/redhat-release; then MAKEFILE=makefile_rhel10; fi
testDIR=${HOME}/tmp/epdb_journal_testing_$(date +%Y_%m_%d_%H%M%S)
logFILE=${testDIR}/epdb_journal_testing.log
TOOL=${NEW_DIR}/epdb_auth_tool
JT=${NEW_DIR}/epdb_journal_test
TESTDB=${NEW_DIR}/epdb_test_db
PW='Crash-Test-Passw0rd!'

mkdir -p ${testDIR} || exit 1
cd ${testDIR} || exit 1
passed=0
failed=0
pass() { echo "Passed: $1" | tee -a ${logFILE}; passed=$((passed+1)); }
fail() { echo "FAILED: $1" | tee -a ${logFILE}; failed=$((failed+1)); }
check() { if [ "$1" -eq 0 ]; then pass "$2"; else fail "$2"; fi; }
# A journal holds an unfinished save while its header has the magic number
pending() { [ -f "$1" ] && [ "$(head -c 8 "$1" | tr -d '\000')" = "GXDWAL01" ]; }
rand_ms() { echo $(( (RANDOM % ($2 - $1 + 1)) + $1 )); }
sleep_ms() { sleep $(awk -v m=$1 'BEGIN{printf "%.3f", m/1000}'); }

if [ ! -x ${TOOL} ]; then
    echo "Build epdb_auth_tool first: make -f ${MAKEFILE}"
    exit 1
fi
for p in epdb_test_db epdb_journal_test; do
    (cd ${NEW_DIR} && make -f ${MAKEFILE} PROJECT=${p} >/dev/null 2>&1)
    if [ ! -x ${NEW_DIR}/${p} ]; then
	echo "Cannot build ${p}"
	exit 1
    fi
done
printf '%s\n' "${PW}" > pw.txt
chmod 600 pw.txt
A="--password-file=pw.txt --non-interactive"

echo "Testing in ${testDIR}" | tee -a ${logFILE}

# 1. GUI-style saves killed at random moments
${TESTDB} create crash.ehd "${PW}" 20 >>${logFILE} 2>&1
check $? "create the test database"
start=0
bad=0
jnl_seen=0
recovered=0
jnl_left=0
for r in $(seq 1 ${ROUNDS}); do
    ${JT} writer crash.ehd "${PW}" ${start} > acked.txt 2>>${logFILE} &
    pid=$!
    # The open (key derivation) takes about 0.4 s, the saves follow
    sleep_ms $(rand_ms 300 1500)
    kill -9 ${pid} 2>/dev/null
    wait ${pid} 2>/dev/null
    pending crash.ehd.jnl && jnl_seen=$((jnl_seen+1))
    acked=$(tail -1 acked.txt 2>/dev/null)
    acked=${acked:-${start}}
    res=$(${JT} verify crash.ehd "${PW}" ${acked} 2>&1)
    echo "round ${r}: start ${start} acked ${acked}: ${res}" | tr '\n' ' ' >> ${logFILE}
    echo >> ${logFILE}
    echo "${res}" | grep -q "finished a save" && recovered=$((recovered+1))
    found=$(echo "${res}" | grep '^PASS' | awk '{print $2}')
    if [ -z "${found}" ]; then
	bad=$((bad+1))
	echo "round ${r}: ${res}" | tee -a ${logFILE}
	break
    fi
    start=${found}
    # 11/06/2026: verify opens for writing and closes cleanly: no journal
    # is left, also when the writer was killed between saves (an empty
    # journal)
    [ -f crash.ehd.jnl ] && jnl_left=$((jnl_left+1))
done
check ${bad} "${ROUNDS} kill -9 rounds of GUI-style saves: every reported save is there, no half save (${start} saves, ${jnl_seen} kills stopped a save after it reached the journal and the open finished ${recovered})"
check ${jnl_left} "no journal file is left after the open that follows a kill (${jnl_left} of ${ROUNDS} rounds left one)"

# 2. epdb_auth_tool record changes (temporary files renamed over the
# database) killed at random moments
${TESTDB} create cli.ehd "${PW}" 20 >>${logFILE} 2>&1
before=$(${TOOL} ${A} --db-list cli.ehd 2>/dev/null | wc -l)
# Time of one change, so the kills fall anywhere in it
t0=$(date +%s%N)
printf 'username=u0\n' | ${TOOL} ${A} --add-record=Cli0 cli.ehd >>${logFILE} 2>&1
t_ms=$(( ($(date +%s%N) - t0) / 1000000 ))
before=$((before+1))
bad=0
left=0
for r in $(seq 1 20); do
    printf 'username=u%s\npassword=p%s\n' ${r} ${r} | ${TOOL} ${A} --add-record=Cli${r} cli.ehd >>${logFILE} 2>&1 &
    pid=$!
    sleep_ms $(rand_ms $((t_ms / 3)) $((t_ms * 11 / 10)))
    kill -9 ${pid} 2>/dev/null
    wait ${pid} 2>/dev/null
    if [ -f cli.change.ehd -o -f cli.change.ehx ] && [ ! -f cli.change.ehd.swap ]; then
	# Killed before the files were replaced: the original is unchanged,
	# the user removes the temporary files the tool names
	rm -f cli.change.ehd cli.change.ehx cli.change.ehd.swap.tmp
	left=$((left+1))
    fi
    out=$(${TOOL} ${A} --db-list cli.ehd 2>>${logFILE})
    rv=$?
    n=$(echo "${out}" | grep -c .)
    if [ ${rv} -ne 0 ] || { [ ${n} -ne ${before} ] && [ ${n} -ne $((before+1)) ]; }; then
	bad=$((bad+1))
	echo "cli round ${r}: exit ${rv}, ${n} records, ${before} before" | tee -a ${logFILE}
	break
    fi
    before=${n}
done
check ${bad} "20 kill -9 rounds of epdb_auth_tool --add-record (${t_ms} ms each): the database opens and holds the records before or after the change (${left} kills before the files were replaced)"
[ ! -f cli.change.ehd.swap ]
check $? "no unfinished file replacement is left after an open"

# 2b. The two states a crash can leave while the temporary files are
# renamed (epdb_swap_files() in db_auth.cpp), made by hand: the marker
# is written and nothing renamed yet, or only the index is renamed.
# The next open must finish the replacement.
${TESTDB} create sw.ehd "${PW}" 20 >>${logFILE} 2>&1
cp sw.ehd old_sw.ehd; cp sw.ehx old_sw.ehx
printf 'username=sw\n' | ${TOOL} ${A} --add-record=Swapped sw.ehd >>${logFILE} 2>&1
cp sw.ehd new_sw.ehd; cp sw.ehx new_sw.ehx
for state in marker index; do
    cp old_sw.ehd sw.ehd
    if [ ${state} = marker ]; then cp old_sw.ehx sw.ehx; cp new_sw.ehx sw.change.ehx; else cp new_sw.ehx sw.ehx; fi
    cp new_sw.ehd sw.change.ehd
    echo "EPDB file replacement in progress" > sw.change.ehd.swap
    ${TOOL} ${A} --db-list sw.ehd > sw_list.txt 2>>${logFILE}
    rv=$?
    [ ${rv} -eq 0 ] && cmp -s sw.ehd new_sw.ehd && cmp -s sw.ehx new_sw.ehx && grep -q Swapped sw_list.txt && \
	[ ! -f sw.change.ehd ] && [ ! -f sw.change.ehx ] && [ ! -f sw.change.ehd.swap ]
    check $? "a replacement stopped at the ${state} step is finished on the next open"
done

# 3. Journal file mode and removal
${TESTDB} create mode.ehd "${PW}" 20 >>${logFILE} 2>&1
${JT} writer mode.ehd "${PW}" 0 > /dev/null 2>>${logFILE} &
pid=$!
mode=""
for i in $(seq 1 400); do
    if [ -f mode.ehd.jnl ]; then mode=$(stat -c %a mode.ehd.jnl 2>/dev/null); [ -n "${mode}" ] && break; fi
    sleep_ms 2
done
kill -9 ${pid} 2>/dev/null
wait ${pid} 2>/dev/null
[ "${mode}" = "600" ]
check $? "the journal file is mode 600 during a save (${mode})"
${JT} verify mode.ehd "${PW}" 0 >/dev/null 2>&1 || ${JT} verify mode.ehd "${PW}" 1 >/dev/null 2>&1
[ ! -f mode.ehd.jnl ]
check $? "no journal file is left after the database is opened and closed"
${TOOL} ${A} --db-list mode.ehd > /dev/null 2>>${logFILE}
[ ! -f mode.ehd.jnl ]
check $? "a read-only open makes no journal file"

# 4. An unfinished save in a database that cannot be written
${TESTDB} create ro.ehd "${PW}" 20 >>${logFILE} 2>&1
for i in $(seq 1 50); do
    ${JT} writer ro.ehd "${PW}" 0 > /dev/null 2>>${logFILE} &
    pid=$!
    sleep_ms $(rand_ms 400 1500)
    kill -9 ${pid} 2>/dev/null
    wait ${pid} 2>/dev/null
    pending ro.ehd.jnl && break
    rm -f ro.ehd ro.ehx ro.ehd.jnl
    ${TESTDB} create ro.ehd "${PW}" 20 >>${logFILE} 2>&1
done
if pending ro.ehd.jnl; then
    chmod a-w ro.ehd ro.ehx ro.ehd.jnl
    sum_before=$(cat ro.ehd ro.ehx ro.ehd.jnl | sha256sum)
    ${TOOL} ${A} --check-version ro.ehd > ro_out.txt 2>&1
    ${TOOL} ${A} --db-list ro.ehd >> ro_out.txt 2>&1
    sum_after=$(cat ro.ehd ro.ehx ro.ehd.jnl | sha256sum)
    grep -q "waiting to be finished" ro_out.txt && [ "${sum_before}" = "${sum_after}" ]
    check $? "a read-only database with an unfinished save warns and is not changed"
    chmod u+w ro.ehd ro.ehx ro.ehd.jnl
    ${TOOL} ${A} --db-list ro.ehd > /dev/null 2>ro_out2.txt
    rv=$?
    [ ${rv} -eq 0 ] && grep -q "finished a save" ro_out2.txt && [ ! -f ro.ehd.jnl ]
    check $? "the save is finished when the database can be written again"
else
    fail "could not stop a save part way to test a read-only database"
fi

# 5. Old builds
if [ -n "${OLD_DIR}" ]; then
    OLD_TOOL=${OLD_DIR}/epdb_auth_tool
    ${OLD_DIR}/epdb_test_db create old.ehd "${PW}" 20 >>${logFILE} 2>&1
    sum_before=$(cat old.ehd old.ehx | sha256sum)
    ${TOOL} ${A} --db-list old.ehd > new_list.txt 2>>${logFILE}
    rv=$?
    ${OLD_TOOL} ${A} --db-list old.ehd > old_list.txt 2>>${logFILE}
    sum_after=$(cat old.ehd old.ehx | sha256sum)
    [ ${rv} -eq 0 ] && cmp -s new_list.txt old_list.txt && [ "${sum_before}" = "${sum_after}" ]
    check $? "a database made by the old build lists the same and is not changed by an open"
    ${JT} writer old.ehd "${PW}" 0 > acked.txt 2>>${logFILE} &
    pid=$!
    sleep_ms 1500 # After the open (key derivation), some saves
    kill -9 ${pid} 2>/dev/null
    wait ${pid} 2>/dev/null
    acked=$(tail -1 acked.txt 2>/dev/null)
    [ -n "${acked}" ] && ${JT} verify old.ehd "${PW}" ${acked} >>${logFILE} 2>&1
    check $? "this build saves to a database made by the old build"
    ${OLD_TOOL} ${A} --db-list old.ehd > old_list2.txt 2>>${logFILE}
    rv=$?
    ${TOOL} ${A} --db-list old.ehd > new_list2.txt 2>>${logFILE}
    [ ${rv} -eq 0 ] && cmp -s new_list2.txt old_list2.txt && [ $(grep -c . old_list2.txt) -gt 20 ]
    check $? "the old build opens a database saved by this build, with the same records ($(grep -c . old_list2.txt) listed)"
    ${OLD_TOOL} ${A} --db-list crash.ehd > /dev/null 2>>${logFILE}
    check $? "the old build opens the database used for the kill -9 rounds"
fi

# 7. Writer lock
${TESTDB} create lock.ehd "${PW}" 20 >>${logFILE} 2>&1
${JT} writer lock.ehd "${PW}" 0 > acked.txt 2>>${logFILE} &
pid=$!
for i in $(seq 1 300); do [ -s acked.txt ] && break; sleep_ms 10; done
out=$(${JT} verify lock.ehd "${PW}" 0 2>&1)
echo "${out}" | grep -q "open in another program"
check $? "a second read-write open is refused while a writer has the database open"
# 11/17/2026: 50 tries while the writer saves nonstop. Each one must be
# refused as locked, never with an integrity error from reading a save
# in progress (2026.105 took the writer lock after its integrity check)
lock_ok=0
for i in $(seq 1 50); do
    printf 'username=x\n' | ${TOOL} ${A} --add-record=Second lock.ehd > lock_out.txt 2>&1
    rv=$?
    [ ${rv} -eq 4 ] && grep -q "open in another program" lock_out.txt && lock_ok=$((lock_ok + 1))
done
[ ${lock_ok} -eq 50 ]
check $? "epdb_auth_tool --add-record is refused (exit 4) while a writer has the database open (${lock_ok} of 50)"
${TOOL} ${A} --check-version lock.ehd >>${logFILE} 2>&1
check $? "a read-only open works while a writer has the database open"
kill -0 ${pid} 2>/dev/null
check $? "the writer kept running"
kill -9 ${pid} 2>/dev/null
wait ${pid} 2>/dev/null
acked=$(tail -1 acked.txt 2>/dev/null)
out=$(${JT} verify lock.ehd "${PW}" ${acked:-0} 2>&1)
echo "${out}" | grep -q '^PASS'
check $? "after kill -9 of the writer the lock is free and the next open finishes its save (${out})"
printf 'username=x\n' | ${TOOL} ${A} --add-record=After lock.ehd >>${logFILE} 2>&1
check $? "a record change works after the writer is gone"

# 8. Journals that are not trusted
make_pending() { # $1 = database name; leaves a journal with a save in it
    for i in $(seq 1 60); do
	rm -f $1.ehd $1.ehx $1.ehd.jnl
	${TESTDB} create $1.ehd "${PW}" 20 >>${logFILE} 2>&1
	${JT} writer $1.ehd "${PW}" 0 > /dev/null 2>>${logFILE} &
	pid=$!
	sleep_ms $(rand_ms 400 1500)
	kill -9 ${pid} 2>/dev/null
	wait ${pid} 2>/dev/null
	pending $1.ehd.jnl && return 0
    done
    return 1
}
mkdir -p shared
if (cd shared && make_pending gw); then
    chmod g+w shared
    sum_before=$(cat shared/gw.ehd shared/gw.ehx | sha256sum)
    ${TOOL} ${A} --db-list shared/gw.ehd > gw_out.txt 2>&1
    sum_after=$(cat shared/gw.ehd shared/gw.ehx | sha256sum)
    grep -q "can be written by other users" gw_out.txt && [ "${sum_before}" = "${sum_after}" ] && pending shared/gw.ehd.jnl
    check $? "a journal in a directory the group can write is not replayed, the database is not changed"
    chmod g-w shared
    ${TOOL} ${A} --db-list shared/gw.ehd > /dev/null 2>gw_out2.txt
    grep -q "finished a save" gw_out2.txt && [ ! -f shared/gw.ehd.jnl ]
    check $? "it is replayed once only the owner can write the directory"
else
    fail "could not stop a save part way for the directory test"
fi
if make_pending other; then
    ${TESTDB} create victim.ehd "${PW}" 20 >>${logFILE} 2>&1
    cp other.ehd.jnl victim.ehd.jnl
    sum_before=$(cat victim.ehd victim.ehx other.ehd other.ehx | sha256sum)
    ${TOOL} ${A} --db-list victim.ehd > v_out.txt 2>&1
    sum_after=$(cat victim.ehd victim.ehx other.ehd other.ehx | sha256sum)
    grep -q "names files that are not this database" v_out.txt && [ "${sum_before}" = "${sum_after}" ]
    check $? "a journal that names another database's files is not replayed, no file is changed"
else
    fail "could not stop a save part way for the foreign journal test"
fi

# 6. Time of a save
for m in 0 2 1; do
    ${TESTDB} create t${m}.ehd "${PW}" 20 >>${logFILE} 2>&1
    ${JT} time t${m}.ehd "${PW}" 200 ${m} | tee -a ${logFILE}
done

echo "${passed} passed, ${failed} failed" | tee -a ${logFILE}
echo "Logged output to: ${logFILE}"
[ ${failed} -eq 0 ]
