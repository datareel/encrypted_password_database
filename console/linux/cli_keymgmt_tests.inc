# 10/27/2026: Tests for key management, records, access logs, version
# check, self-destruct and the automation options. Sourced by
# run_cli_tests.sh, which sets TOOL, TESTDB, PW, OLD and the pass, fail
# and check functions, and runs in a scratch directory.

echo "" | tee -a ${logFILE}
echo "Key management, records, logs, self-destruct and automation" | tee -a ${logFILE}

# Fixtures: password file, two RSA users (no passphrase), a log file
printf '%s\n' "${PW}" > k_pw.txt; chmod 600 k_pw.txt
printf '%s\n' "${WRONG}" > k_bad.txt; chmod 600 k_bad.txt
for u in alice bob; do
    openssl genrsa -out k_${u}.pem 2048 >/dev/null 2>&1
    openssl rsa -in k_${u}.pem -pubout -out k_${u}.pub >/dev/null 2>&1
    chmod 600 k_${u}.pem
done
KLOG=${testDIR}/k_access.log
A="--password-file=k_pw.txt --non-interactive"
newdb() { rm -f $1.ehd $1.ehx; ${TESTDB} create $1.ehd "${PW}" ${2:-3} >>${logFILE} 2>&1; }
dbsum() { cat $1.ehd $1.ehx | sha256sum; }

# ---- Secrets from stdin, a file descriptor and a file ----
newdb k1
printf '%s\n' "${PW}" | ${TOOL} --password-fd=0 --db-list k1.ehd > out.txt 2>>${logFILE}
check $? "password from stdin with --password-fd=0"
${TOOL} --password-fd=5 --db-list k1.ehd 5<k_pw.txt > out.txt 2>>${logFILE}
check $? "password from file descriptor 5"
grep -c "" out.txt | grep -q '^3$'
check $? "the listing has every record"
printf 'username=u1\npassword=Secret-Value-1\nnotes=a\\nb\n' > k_rec.txt; chmod 600 k_rec.txt
${TOOL} ${A} --add-record=FromFile --record-file=k_rec.txt k1.ehd >>${logFILE} 2>&1
check $? "record fields from --record-file"
${TOOL} ${A} --add-record=FromFD --record-fd=6 k1.ehd 6<k_rec.txt >>${logFILE} 2>&1
check $? "record fields from --record-fd"
printf 'username=u3\n' | ${TOOL} ${A} --add-record=FromStdin k1.ehd >>${logFILE} 2>&1
check $? "record fields from stdin"
ps_out=$( (printf 'password=Secret-PS-Check\n' | ${TOOL} ${A} --add-record=PsCheck k1.ehd >/dev/null 2>&1 &) ; sleep 0.2; ps -eo args | grep -c "Secret-PS-Check" )
[ "${ps_out}" -le 1 ]
check $? "a record value never appears in argv"

# ---- Records ----
${TOOL} ${A} --get-field=FromFile:password k1.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && [ "$(cat out.txt)" = "Secret-Value-1" ]
check $? "--get-field writes only the value"
${TOOL} ${A} --get-field=FromFile:notes k1.ehd > out.txt 2>>${logFILE}
printf 'a\nb' | cmp -s - out.txt
check $? "an escaped newline in a value is stored as a newline"
printf 'username=changed\n' | ${TOOL} ${A} --update-record=fromfile k1.ehd >>${logFILE} 2>&1
${TOOL} ${A} --show-record=FromFile --json k1.ehd > out.json 2>>${logFILE}
python3 -I -c "import json,sys; r=json.load(open('out.json')); sys.exit(0 if r['username']=='changed' and r['password']=='Secret-Value-1' else 1)"
check $? "update changes only the fields given (record name not case sensitive)"
printf 'title=Renamed\nnotes=\n' | ${TOOL} ${A} --update-record=FromFile k1.ehd >>${logFILE} 2>&1
${TOOL} ${A} --get-field=Renamed:notes k1.ehd > out.txt 2>>${logFILE}
[ $? -eq 0 ] && [ ! -s out.txt ]
check $? "title= renames and an empty value clears a field"
printf 'username=x\n' | ${TOOL} ${A} --add-record=Renamed k1.ehd >>${logFILE} 2>&1
[ $? -ne 0 ]
check $? "adding a record that exists is refused"
printf 'username=x\n' | ${TOOL} ${A} --update-record=NoSuchRecord k1.ehd >>${logFILE} 2>&1
[ $? -eq 3 ]
check $? "updating a missing record exits 3"
printf 'colour=x\n' | ${TOOL} ${A} --update-record=Renamed k1.ehd >>${logFILE} 2>&1
[ $? -eq 1 ]
check $? "an unknown field is refused"
python3 -I -c "print('notes=' + 'x'*2000)" | ${TOOL} ${A} --update-record=Renamed k1.ehd >>${logFILE} 2>&1
[ $? -ne 0 ]
check $? "a value that does not fit a field is refused"
before=$(dbsum k1)
${TOOL} ${A} --delete-record=Renamed k1.ehd </dev/null >>${logFILE} 2>&1
[ $? -ne 0 ] && [ "$(dbsum k1)" = "${before}" ]
check $? "delete without --yes and no terminal is refused, nothing changed"
${TOOL} ${A} --delete-record=Renamed --dry-run k1.ehd >>${logFILE} 2>&1
[ $? -eq 0 ] && [ "$(dbsum k1)" = "${before}" ]
check $? "--dry-run delete changes nothing"
${TOOL} ${A} --delete-record=Renamed --yes k1.ehd >>${logFILE} 2>&1
${TOOL} ${A} --show-record=Renamed k1.ehd >/dev/null 2>&1
[ $? -eq 3 ]
check $? "delete with --yes removes the record"
for auth in rsa; do
    ${TOOL} ${A} --add-rsa-key=k_alice.pub --add-username=alice k1.ehd >>${logFILE} 2>&1
    printf 'username=viarsa\n' | ${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --add-record=ViaRSA k1.ehd >>${logFILE} 2>&1
    check $? "add a record authenticated with an RSA key"
    ${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --delete-record=ViaRSA --yes k1.ehd >>${logFILE} 2>&1
    check $? "delete a record authenticated with an RSA key"
done

# ---- JSON input and output ----
newdb kj 0
python3 -I -c "
import json
recs=[{'title':'R%03d'%i,'username':'u%d'%i,'password':'p\"w\\\\%d'%i,'notes':'line1\nline2'} for i in range(200)]
open('k_bulk.json','w').write(json.dumps(recs))"
chmod 600 k_bulk.json
${TOOL} ${A} --import --record-file=k_bulk.json kj.ehd >>${logFILE} 2>&1
${TOOL} ${A} --db-list --json kj.ehd > out.json 2>>${logFILE}
python3 -I -m json.tool out.json >/dev/null && python3 -I -c "import json,sys; sys.exit(0 if len(json.load(open('out.json')))==200 else 1)"
check $? "bulk JSON import of 200 records, --db-list --json is valid JSON"
${TOOL} ${A} --export=json --quiet kj.ehd > k_exp.json 2>>${logFILE}
newdb kj2 0
chmod 600 k_exp.json
${TOOL} ${A} --import --record-file=k_exp.json kj2.ehd >>${logFILE} 2>&1
${TOOL} ${A} --export=json --quiet kj2.ehd | cmp -s - k_exp.json
check $? "JSON export, import and export again round-trips exactly"
before=$(dbsum kj)
printf '[{"title":"A1"},{"title":"A2",}]' | ${TOOL} ${A} --import kj.ehd >>${logFILE} 2>&1
rv=$?; [ ${rv} -eq 1 ] && [ "$(dbsum kj)" = "${before}" ]
check $? "malformed JSON is refused and the database is unchanged"
printf '[{"title":"New1"},{"title":"R005"}]' | ${TOOL} ${A} --import kj.ehd >>${logFILE} 2>&1
[ $? -ne 0 ] && [ "$(dbsum kj)" = "${before}" ]
check $? "a batch with one bad record changes nothing (all or nothing)"
printf '{"title":"D","title":"E"}' | ${TOOL} ${A} --add-record=D kj.ehd >>${logFILE} 2>&1
[ $? -eq 1 ]
check $? "duplicate JSON keys are refused"
printf '{"title":"D","colour":"red"}' | ${TOOL} ${A} --add-record=D kj.ehd >>${logFILE} 2>&1
[ $? -eq 1 ]
check $? "unknown JSON fields are refused"
python3 -I -c "print('[' + ','.join(['{\"title\":\"T%07d\",\"notes\":\"%s\"}' % (i, 'x'*200) for i in range(5000)]) + ']')" | ${TOOL} ${A} --import kj.ehd >>${logFILE} 2>&1
[ $? -eq 1 ] && [ "$(dbsum kj)" = "${before}" ]
check $? "input over the size cap is refused"
printf '[{"title":"S1"}]' | ${TOOL} ${A} --import --dry-run kj.ehd >>${logFILE} 2>&1
[ $? -eq 0 ] && [ "$(dbsum kj)" = "${before}" ]
check $? "--dry-run import changes nothing"
for opt in "--db-info" "--list-users" "--show-record=R001"; do
    ${TOOL} ${A} ${opt} --json kj.ehd 2>>${logFILE} | python3 -I -m json.tool >/dev/null
    check $? "${opt} --json is valid JSON"
done
${TOOL} --check-version --json kj.ehd known_v1.ehd 2>>${logFILE} | python3 -I -m json.tool >/dev/null
check $? "--check-version --json is valid JSON"
${TOOL} ${A} --export --output-file=k_exp.csv kj.ehd >>${logFILE} 2>&1
[ $(stat -c %a k_exp.csv) = "600" ] && [ $(grep -c "^R" k_exp.csv) -eq 200 ]
check $? "CSV export to --output-file, a file only you can read"
newdb kc 0
chmod 600 k_exp.csv
${TOOL} ${A} --import --record-file=k_exp.csv --input-format=csv kc.ehd >>${logFILE} 2>&1
${TOOL} ${A} --get-field=R007:password kc.ehd > out.txt 2>>${logFILE}
[ "$(cat out.txt)" = 'p"w\7' ]
check $? "CSV import of the GUI export format"
${TOOL} ${A} --merge-from=kj.ehd --merge-policy=skip kc.ehd > out.txt 2>&1
grep -q "0 added, 0 replaced, 200 skipped" out.txt
check $? "--merge-from with --merge-policy=skip"
${TOOL} --generate-password=32 --password-symbols > out.txt
[ $(tr -d '\n' < out.txt | wc -c) -eq 32 ]
check $? "--generate-password writes a password of the length asked"

# ---- Exit codes, non-interactive, quiet, locking ----
${TOOL} --password-file=k_bad.txt --db-list kj.ehd >/dev/null 2>&1
[ $? -eq 2 ]
check $? "wrong password exits 2"
${TOOL} ${A} --db-list nosuch.ehd >/dev/null 2>&1
[ $? -eq 3 ]
check $? "missing database exits 3"
${TOOL} --non-interactive --db-list kj.ehd </dev/null >/dev/null 2>err.txt
[ $? -eq 1 ] && grep -q "non-interactive" err.txt
check $? "--non-interactive refuses to prompt for a password"
${TOOL} ${A} --quiet --db-list kj.ehd 2>err.txt >/dev/null
[ $? -eq 0 ] && [ ! -s err.txt ]
check $? "--quiet writes nothing to stderr on success"
${TOOL} ${A} --db-list known_v1.ehd 2>err.txt >out.txt
grep -q "v1" err.txt && ! grep -q "WARNING" out.txt
check $? "errors and warnings on stderr, output on stdout"
( flock -x 9; sleep 4 ) 9>kj.ehd.lock &
sleep 0.5
printf 'username=l\n' | ${TOOL} ${A} --lock-timeout=1 --add-record=Locked kj.ehd >>${logFILE} 2>&1
[ $? -eq 4 ]
check $? "a held lock makes a writer exit 4 after --lock-timeout"
wait
newdb kw 0
for i in $(seq 1 10); do
    printf 'username=w%d\n' $i | ${TOOL} ${A} --add-record=W$i kw.ehd >>${logFILE} 2>&1 &
done
wait
n=$(${TOOL} ${A} --db-list kw.ehd 2>/dev/null | wc -l)
${TOOL} ${A} --db-list kw.ehd >/dev/null 2>&1
[ $? -eq 0 ] && [ ${n} -eq 10 ]
check $? "10 concurrent writers: every record added and the database checks"

# ---- Remove user ----
newdb kr
${TOOL} ${A} --add-rsa-key=k_alice.pub --add-username=alice kr.ehd >>${logFILE} 2>&1
${TOOL} ${A} --add-rsa-key=k_bob.pub --add-username=bob kr.ehd >>${logFILE} 2>&1
before=$(dbsum kr)
${TOOL} ${A} --remove-user=bob --dry-run kr.ehd >>${logFILE} 2>&1
[ $? -eq 0 ] && [ "$(dbsum kr)" = "${before}" ]
check $? "--remove-user --dry-run changes nothing"
${TOOL} ${A} --remove-user=bob kr.ehd > out.txt 2>&1
[ $? -eq 0 ] && grep -q "does not revoke" out.txt && grep -q -- "--rekey" out.txt
check $? "remove an RSA user, with the note about revoking and --rekey"
${TOOL} --rsa-key=k_bob.pem --rsa-key-username=bob --non-interactive --db-list kr.ehd >/dev/null 2>&1
[ $? -eq 2 ]
check $? "the removed RSA user can no longer open the database"
ls kr.remove-user-backup-*.ehd >/dev/null 2>&1
check $? "a backup copy is kept"
${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --remove-user=alice kr.ehd >>${logFILE} 2>&1
[ $? -ne 0 ] && ${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --db-list kr.ehd >/dev/null 2>&1
check $? "the last RSA user cannot be removed when opened with an RSA key"
${TOOL} ${A} --remove-user=nobody kr.ehd >>${logFILE} 2>&1
[ $? -eq 3 ]
check $? "removing an unknown user exits 3"

# ---- Rekey ----
newdb kk
${TOOL} ${A} --add-rsa-key=k_alice.pub --add-username=alice kk.ehd >>${logFILE} 2>&1
${TOOL} ${A} --add-rsa-key=k_bob.pub --add-username=bob kk.ehd >>${logFILE} 2>&1
before=$(dbsum kk)
${TOOL} ${A} --rekey kk.ehd </dev/null > out.txt 2>&1
rv=$?; [ ${rv} -ne 0 ] && grep -q "rsa:alice=" out.txt && grep -q "rsa:bob=" out.txt && [ "$(dbsum kk)" = "${before}" ]
check $? "rekey without public keys lists the missing slots and changes nothing"
printf 'rsa:alice=k_alice.pub\nrsa:bob=k_bob.pub\n' | ${TOOL} ${A} --rekey --dry-run kk.ehd >>${logFILE} 2>&1
[ $? -eq 0 ] && [ "$(dbsum kk)" = "${before}" ]
check $? "--rekey --dry-run changes nothing"
cp kk.ehd kk_pre.ehd; cp kk.ehx kk_pre.ehx
${TOOL} ${A} --remove-user=bob kk.ehd >>${logFILE} 2>&1
printf 'rsa:alice=k_alice.pub\n' | ${TOOL} ${A} --rekey kk.ehd >>${logFILE} 2>&1
check $? "rekey keeping the password and rewrapping the RSA user"
${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --db-list kk.ehd >/dev/null 2>&1
check $? "the kept RSA user opens the rekeyed database"
${TOOL} ${A} --db-list kk.ehd >/dev/null 2>&1
check $? "the password still opens the rekeyed database"
# Bob's old key block, from before the remove, put back in the rekeyed file
python3 -I -c "
pre=open('kk_pre.ehd','rb').read(); new=bytearray(open('kk.ehd','rb').read())
o=86+65536; new[o:o+65536]=pre[o:o+65536]; open('kk_bob.ehd','wb').write(new)"
cp kk.ehx kk_bob.ehx
${TOOL} --rsa-key=k_bob.pem --rsa-key-username=bob --non-interactive --db-list kk_bob.ehd >/dev/null 2>&1
[ $? -ne 0 ]
check $? "after remove and rekey an old key block of the removed user does not open the database"
printf 'new-password=Another-Pass#77\nrsa:alice=k_alice.pub\n' | ${TOOL} ${A} --rekey kk.ehd >>${logFILE} 2>&1
${TOOL} ${A} --db-list kk.ehd >/dev/null 2>&1; old_rc=$?
printf 'Another-Pass#77\n' > k_pw2.txt; chmod 600 k_pw2.txt
${TOOL} --password-file=k_pw2.txt --db-list kk.ehd >/dev/null 2>&1
[ $? -eq 0 ] && [ ${old_rc} -eq 2 ]
check $? "rekey with new-password: the old password fails, the new one opens"
rm -f k_gen.txt
printf 'rsa:alice=k_alice.pub\n' | ${TOOL} --password-file=k_pw2.txt --non-interactive --rekey --rekey-new-password-file=k_gen.txt kk.ehd > out.txt 2>&1
${TOOL} --password-file=k_gen.txt --db-list kk.ehd >/dev/null 2>&1
[ $? -eq 0 ] && [ $(stat -c %a k_gen.txt) = "600" ] && ! grep -q "$(cat k_gen.txt)" out.txt
check $? "a generated password goes only to --rekey-new-password-file and opens the database"
printf 'rsa:alice=k_alice.pub\ndrop-password=yes\n' | ${TOOL} --password-file=k_gen.txt --non-interactive --rekey kk.ehd >>${logFILE} 2>&1
${TOOL} --password-file=k_gen.txt --db-list kk.ehd >/dev/null 2>&1; pw_rc=$?
${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --db-list kk.ehd >/dev/null 2>&1
[ $? -eq 0 ] && [ ${pw_rc} -eq 2 ]
check $? "drop-password: only the RSA user opens the database"
printf 'drop-rsa:alice=yes\n' | ${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --rekey kk.ehd >>${logFILE} 2>&1
[ $? -ne 0 ] && ${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --db-list kk.ehd >/dev/null 2>&1
check $? "a rekey that would leave nothing to open the database is refused"
newdb kx
before=$(dbsum kx)
FCRYPT_TEST_CRASH_BEFORE_RENAME=1 ${TOOL} ${A} --rekey kx.ehd </dev/null >>${logFILE} 2>&1
rv=$?
[ ${rv} -eq 137 ] && [ "$(dbsum kx)" = "${before}" ] && ${TOOL} ${A} --db-list kx.ehd >/dev/null 2>&1
check $? "a rekey killed before the rename leaves the original database working (rc=${rv})"
rm -f kx.rekey.ehd kx.rekey.ehx

# ---- Access logs ----
newdb kl
rm -f ${KLOG}
${TOOL} ${A} --access-log=${KLOG} --access-log-in-file=on kl.ehd >>${logFILE} 2>&1
printf 'username=loguser\npassword=Log-Secret-Value\n' | ${TOOL} ${A} --access-log=${KLOG} --add-record=LogRec kl.ehd >>${logFILE} 2>&1
${TOOL} --password-file=k_bad.txt --non-interactive --access-log=${KLOG} --db-list kl.ehd >/dev/null 2>&1
${TOOL} ${A} --access-log=${KLOG} --get-field=LogRec:password kl.ehd >/dev/null 2>&1
${TOOL} ${A} --access-log=${KLOG} --verify-access-log kl.ehd > out.txt 2>&1
check $? "the per-user log and the in-file log verify"
grep -q "action=add-record" ${KLOG} && grep -q "result=fail" ${KLOG} && grep -q "record=LogRec" ${KLOG}
check $? "the log has the open failure, the change and the record name"
! grep -q -e "Log-Secret-Value" -e "${PW}" -e "loguser" ${KLOG} && ! strings kl.ehd | grep -q "loguser"
check $? "the logs hold no password and no record value"
[ $(stat -c %a ${KLOG}) = "600" ]
check $? "the log file is mode 600"
${TOOL} ${A} --access-log=${KLOG} --show-access-log --json kl.ehd > out.json 2>>${logFILE} || true
${TOOL} ${A} --access-log=${KLOG} --show-access-log kl.ehd > out.txt 2>>${logFILE}
grep -q "add-record" out.txt
check $? "--show-access-log lists the in-file entries"
cp ${KLOG} k_log_orig
sed -i '2s/pid=/pid=9/' ${KLOG}
${TOOL} ${A} --access-log=${KLOG} --verify-access-log kl.ehd > out.txt 2>&1
[ $? -ne 0 ] && grep -q "TAMPERED" out.txt
check $? "--verify-access-log catches an edited per-user log line"
cp k_log_orig ${KLOG}
cp kl.ehd kl_t.ehd; cp kl.ehx kl_t.ehx
python3 -I -c "d=bytearray(open('kl_t.ehd','rb').read()); o=86+49472+64+96+20; d[o]^=1; open('kl_t.ehd','wb').write(d)"
${TOOL} ${A} --verify-access-log kl_t.ehd > out.txt 2>&1
[ $? -ne 0 ] && grep -q "TAMPERED: in-file" out.txt
check $? "--verify-access-log catches a changed in-file log entry"
chmod 444 kl.ehd
rm -f k_fallback.log
HOME_SAVE=${HOME}; export HOME=${testDIR}/khome; mkdir -p ${HOME}
${TOOL} ${A} --db-list kl.ehd >/dev/null 2>&1
grep -q "in-file log unavailable (read-only)" ${HOME}/.encrypted_password_database/logs/access.log
check $? "in-file logging on a read-only file falls back to the per-user log, marked"
[ $(stat -c %a ${HOME}/.encrypted_password_database/logs) = "700" ]
check $? "the per-user log directory is mode 700"
export HOME=${HOME_SAVE}
chmod 644 kl.ehd
printf 'rsa:none=x\n' > /dev/null
${TOOL} ${A} --access-log=${KLOG} --rekey kl.ehd </dev/null >>${logFILE} 2>&1
${TOOL} ${A} --access-log=${KLOG} --verify-access-log kl.ehd > out.txt 2>&1
check $? "the logs still verify after a rekey (carried over to the new key)"

# ---- v1 check ----
${TOOL} --check-version known.ehd > out.txt 2>&1
check $? "--check-version on a v2 database exits 0"
${TOOL} --check-version known.ehd known_v1.ehd > out.txt 2>&1
[ $? -eq 5 ] && grep -q "known_v1.ehd: v1" out.txt && grep -q -- "--upgrade-db" out.txt
check $? "--check-version reports v1, the upgrade command and exits 5"
${TOOL} --check-version k_pw.txt >/dev/null 2>&1
[ $? -eq 7 ]
check $? "--check-version of a file that is not a database exits 7"
${TOOL} --password="${PW}" --db-list known_v1.ehd 2>err.txt >/dev/null
grep -q "WARNING: known_v1.ehd uses EPDB v1" err.txt && grep -q -- "--upgrade-db" err.txt
check $? "opening a v1 database warns on stderr and names the upgrade command"

# ---- Self-destruct ----
newdb ks
${TOOL} ${A} --self-destruct=on ks.ehd >>${logFILE} 2>&1
for i in 1 2 3 4; do ${TOOL} --password-file=k_bad.txt --non-interactive --db-list ks.ehd >/dev/null 2>err_$i.txt; done
grep -q "4 of 5 attempts used" err_4.txt && grep -q "ONE ATTEMPT LEFT" err_4.txt
check $? "self-destruct counts failed opens and warns at one attempt left"
${TOOL} ${A} --db-list ks.ehd >/dev/null 2>&1
${TOOL} ${A} --db-info --json ks.ehd 2>/dev/null | python3 -I -c "import json,sys; sys.exit(0 if json.load(sys.stdin)['failed_opens']==0 else 1)"
check $? "a good open resets the failed open counter"
chmod 444 ks.ehd
${TOOL} ${A} --db-list ks.ehd > /dev/null 2>err.txt
[ $? -eq 7 ] && grep -q "No credential was tried" err.txt
check $? "with self-destruct on a read-only database is refused before any credential"
chmod 644 ks.ehd
${TOOL} ${A} --add-rsa-key=k_alice.pub --add-username=alice ks.ehd >>${logFILE} 2>&1
${TOOL} ${A} --add-rsa-key=k_bob.pub --add-username=bob ks.ehd >>${logFILE} 2>&1
${TOOL} ${A} --remove-user=bob ks.ehd >>${logFILE} 2>&1
printf 'rsa:alice=k_alice.pub\n' | ${TOOL} ${A} --rekey ks.ehd >>${logFILE} 2>&1
! ls ks.*backup* >/dev/null 2>&1
check $? "with self-destruct on, remove-user and rekey keep no backup copy"
for i in 1 2 3 4 5; do ${TOOL} --password-file=k_bad.txt --non-interactive --db-list ks.ehd >/dev/null 2>err.txt; rv=$?; done
[ ${rv} -eq 6 ] && grep -q "destroyed" err.txt
check $? "the fifth failed open destroys the database (exit 6, default limit 5)"
${TOOL} ${A} --db-list ks.ehd >/dev/null 2>err.txt
[ $? -eq 6 ] && grep -q "destroyed" err.txt
check $? "a destroyed database is reported as destroyed, even with the right password"
${TOOL} --check-version ks.ehd > out.txt 2>&1
[ $? -eq 6 ] && grep -q "destroyed" out.txt
check $? "--check-version reports destroyed"
! strings ks.ehd ks.ehx | grep -q -e "user1" -e "Entry 00"
check $? "the destroyed files hold no record text"
newdb ks2
${TOOL} ${A} --self-destruct=on --self-destruct-attempts=2 ks2.ehd >>${logFILE} 2>&1
${TOOL} --password-file=k_bad.txt --non-interactive --db-list ks2.ehd >/dev/null 2>&1
${TOOL} --password-file=k_bad.txt --non-interactive --db-list ks2.ehd >/dev/null 2>&1
[ $? -eq 6 ]
check $? "self-destruct with a limit of 2 destroys at the second failed open"
newdb ks3
${TOOL} ${A} --self-destruct=on --self-destruct-attempts=3 ks3.ehd >>${logFILE} 2>&1
${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --db-list ks3.ehd >/dev/null 2>&1
${TOOL} ${A} --db-info --json ks3.ehd 2>/dev/null | python3 -I -c "import json,sys; sys.exit(0 if json.load(sys.stdin)['failed_opens']==0 else 1)"
${TOOL} --rsa-key=k_alice.pem --rsa-key-username=alice --non-interactive --db-list ks3.ehd 2>&1 | grep -q "attempts used"
check $? "a wrong RSA key counts as a failed open"

# ---- Undo/redo model the GUI uses ----
newdb ku 0
${TESTDB} undotest ku.ehd "${PW}" > out.txt 2>&1
[ $? -eq 0 ] && grep -q "UNDOTEST PASS" out.txt && ${TOOL} ${A} --db-list ku.ehd >/dev/null 2>&1
check $? "undo/redo model: add, rename, delete, redo, groups, depth cap, the database still checks"

# ---- AWS KMS / Secrets Manager pipe syntax, with a local stand-in ----
fake_kms() { printf '%s' "${PW}" | base64; }
fake_kms | base64 -d | ${TOOL} --password-fd=0 --non-interactive --db-list known.ehd >/dev/null 2>&1
check $? "a KMS style pipe (base64 -d into --password-fd=0) opens the database"

# ---- 2026.102 reads the new files ----
# COMPAT_DIR is a console/linux directory with a 2026.102 (or other v2
# capable) epdb_auth_tool. OLD_DIR above is for builds older than v2.
if [ "${COMPAT_DIR}" != "" ]; then
    COMPAT=${COMPAT_DIR}/epdb_auth_tool
    for db in kr kk_pre kl ks3 kj; do
	[ -f ${db}.ehd ] || continue
	${COMPAT} --password="${PW}" --db-list ${db}.ehd >/dev/null 2>>${logFILE}
	check $? "the 2026.102 build opens ${db}.ehd written by this build"
    done
    ${COMPAT} --rsa-key=k_alice.pem --rsa-key-username=alice --db-list kk.ehd >/dev/null 2>>${logFILE}
    check $? "the 2026.102 build opens a rekeyed, password-less database with the kept RSA user"
fi
