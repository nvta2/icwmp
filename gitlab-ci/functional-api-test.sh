#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

date +%s > timestamp.log
echo "Compiling icmwp"
autoreconf -i
./configure --enable-acs=multi --enable-debug
make clean
make
check_ret $?

echo "Starting dependent services"
supervisorctl status all
supervisorctl update
supervisorctl restart all
ubus wait_for usp.raw
supervisorctl status all

echo "Running the api test cases"
ubus call tr069 status

echo "Stop dependent services"
supervisorctl stop all

#report part
#GitLab-CI output
gcovr -r .
# Artefact
gcovr -r . --xml -o ./api-test-coverage.xml

echo "Checking memory leaks ..."
grep -q "<kind>UninitCondition</kind>" memory-report.xml
check_ret $?

grep -q "<kind>Leak_PossiblyLost</kind>" memory-report.xml
check_ret $?

grep -q "<kind>Leak_DefinitelyLost</kind>" memory-report.xml
check_ret $?

echo "Functional API test PASS"
