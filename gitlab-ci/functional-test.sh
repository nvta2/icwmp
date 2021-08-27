#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

date +%s > timestamp.log
echo "Compiling icmwp"
build_icwmp

echo "Starting dependent services"
supervisorctl status all
supervisorctl update
supervisorctl restart all
supervisorctl stop icwmpd
supervisorctl status all

echo "Configuring genieacs"
configure_genieacs

echo "Configuring ACS URL"
configure_acs_url

echo "Starting icwmpd deamon"
supervisorctl start icwmpd
sleep 5

echo "Checking cwmp status"
check_cwmp_status

[ -f funl-test-result.log ] && rm -f funl-test-result.log

echo "## Running script verification of functionalities ##"
echo > ./funl-test-result.log
echo > ./funl-test-debug.log
test_num=0
for test in `ls -I "common.sh" -I "verify_custom_notifications.sh" test/script/`; do
	test_num=$(( test_num + 1 ))
	./test/script/${test}
	if [ "$?" -eq 0 ]; then
		echo "ok ${test_num} - ${test}" >> ./funl-test-result.log
	else
		echo "not ok ${test_num} - ${test}" >> ./funl-test-result.log
	fi
done

echo "Stop all services"
supervisorctl stop icwmpd

cp test/files/etc/config/users /etc/config/
cp test/files/etc/config/wireless /etc/config/

echo "Verify Custom notifications"
./test/script/verify_custom_notifications.sh
if [ "$?" -eq 0 ]; then
	echo "ok - verify_custom_notifications" >> ./funl-test-result.log
else
	echo "not ok - verify_custom_notifications" >> ./funl-test-result.log
fi

test_num=$(( test_num + 1 ))
echo "1..${test_num}" >> ./funl-test-result.log

# Artefact
gcovr -r . 2> /dev/null --xml -o ./funl-test-coverage.xml
#GitLab-CI output
gcovr -r . 2> /dev/null

cp ./memory-report.xml ./funl-test-memory-report.xml

#report part
exec_cmd tap-junit --input ./funl-test-result.log --output report

echo "Checking memory leaks..."
grep -q "<kind>UninitCondition</kind>" memory-report.xml
error_on_zero $?

grep -q "<kind>Leak_PossiblyLost</kind>" memory-report.xml
error_on_zero $?

grep -q "<kind>Leak_DefinitelyLost</kind>" memory-report.xml
error_on_zero $?

grep -q "<kind>Leak_StillReachable</kind>" memory-report.xml
error_on_zero $?

echo "Functional test :: PASS"
