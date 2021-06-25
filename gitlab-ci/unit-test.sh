#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

echo "Starting dependent services"
supervisorctl status all
supervisorctl update
supervisorctl restart all
supervisorctl stop icwmpd
ubus wait_for usp.raw
supervisorctl status all

echo "Clean cmocka"
make clean -C test/cmocka/
make uninstall -C test/cmocka

echo "Compiling libicmwp"
make libicwmp -C test/cmocka

echo "Installing libicwmp"
make install -C test/cmocka
ldconfig

echo "Running the unit test cases"
make unit-test -C test/cmocka/
check_ret $?

echo "Stop dependent services"
supervisorctl stop all
supervisorctl status

#report part
#GitLab-CI output
gcovr -r . 2> /dev/null #throw away stderr
# Artefact
gcovr -r . 2> /dev/null --xml -o ./unit-test-coverage.xml
date +%s > timestamp.log

echo "Unit test PASS"
