#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

date +%s > timestamp.log
echo "Compiling icmwp"
build_cwmp

echo "Starting dependent services"
supervisorctl status all
supervisorctl update
supervisorctl restart all
exec_cmd ubus wait_for usp.raw tr069
supervisorctl status all

echo "Running the api test cases"
ubus-api-validator -f ./test/api/json/tr069.validation.json > ./api-result.log
check_ret $?

echo "Stop all services"
supervisorctl stop all

# Artefact
#gcovr -r . --xml -o ./api-test-coverage.xml
#GitLab-CI output
#gcovr -r .
#report part
exec_cmd tap-junit --input ./api-result.log --output report

echo "Checking memory leaks ..."
grep -q "<kind>UninitCondition</kind>" memory-report.xml
check_ret $?

grep -q "<kind>Leak_PossiblyLost</kind>" memory-report.xml
check_ret $?

grep -q "<kind>Leak_DefinitelyLost</kind>" memory-report.xml
check_ret $?

echo "Functional API test PASS"
