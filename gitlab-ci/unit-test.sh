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
sleep 3
supervisorctl status all

echo "Compiling icmwp"
autoreconf -i
./configure --enable-acs=multi --enable-acs=hdm --enable-icwmp-test
make clean
make
check_ret $?

echo "Install libicwmp"
cp bin/.libs/libicwmp.a /usr/lib
cp bin/.libs/libicwmp.so* /usr/lib
mkdir /usr/include/libicwmp
cp inc/*.h /usr/include/libicwmp

echo "Running the unit test cases"
make clean -C test/cmocka/
make unit-test -C test/cmocka/
check_ret $?

echo "Stop dependent services"
supervisorctl stop all
supervisorctl status

#report part
#GitLab-CI output
gcovr -r .
# Artefact
gcovr -r . --xml -o ./unit-test-coverage.xml
date +%s > timestamp.log

echo "Unit test PASS"
