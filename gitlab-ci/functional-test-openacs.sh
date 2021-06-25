#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

# install required packages for functional test OpenACS
exec_cmd apt install -y perl -MCPAN -e 'install WWW::Mechanize'

date +%s > timestamp.log
echo "Compiling icmwp"
build_icwmp

echo "Starting dependent services"
supervisorctl status all
supervisorctl update
supervisorctl restart all
supervisorctl stop icwmpd
supervisorctl status all

echo "Configuring OpenACS URL"
configure_openacs_url

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
for test in `ls test/script/openacs/`; do
	test_num=$(( test_num + 1 ))
	./test/script/openacs/${test}
	if [ "$?" -eq 0 ]; then
		echo "ok ${test_num} - ${test}" >> ./funl-test-result.log
	else
		echo "not ok ${test_num} - ${test}" >> ./funl-test-result.log
	fi
done

echo "1..${test_num}" >> ./funl-test-result.log

echo "Stop all services"
supervisorctl stop icwmpd

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
