#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

date +%s > timestamp.log
echo "Compiling icmwp"
build_icwmp

echo "Starting all services"
supervisorctl update
supervisorctl status all
supervisorctl restart all
supervisorctl status all
exec_cmd ubus wait_for usp.raw tr069

echo "Configuring genieacs"
configure_genieacs

echo "Checking cwmp status"
check_cwmp_status

mkdir -p /var/run/icwmpd

[ -f funl-test-result.log ] && rm -f funl-test-result.log

echo "## Running script verification of functionalities ##"
echo > ./funl-test-result.log
echo > ./funl-test-debug.log
test_num=0
for test in `cat test/script/test_seq.txt`; do
	test_num=$(( test_num + 1 ))
	./test/script/${test}
	if [ "$?" -eq 0 ]; then
		echo "ok ${test_num} - ${test}" >> ./funl-test-result.log
	else
		echo "not ok ${test_num} - ${test}" >> ./funl-test-result.log
	fi
done

echo "Stop all services"
supervisorctl stop all

#echo "Verify Custom notifications"
#./test/script/verify_custom_notifications.sh
#if [ "$?" -eq 0 ]; then
#	echo "ok - verify_custom_notifications" >> ./funl-test-result.log
#else
#	echo "not ok - verify_custom_notifications" >> ./funl-test-result.log
#fi
#
#test_num=$(( test_num + 1 ))
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
