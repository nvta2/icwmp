#!/bin/bash

echo "preparation script"
pwd
source ./gitlab-ci/shared.sh

trap cleanup EXIT
trap cleanup SIGINT

echo "Install lighttpd"
apt-get update
apt-get install -y lighttpd
exec_cmd dd if=/dev/zero of=/builds/iopsys/icwmp/firmware_v1.0.bin bs=25MB count=1
echo "Valid" > /builds/iopsys/icwmp/firmware_v1.0.bin
exec_cmd cp /builds/iopsys/icwmp/firmware_v1.0.bin /var/www/html
exec_cmd dd if=/dev/zero of=/builds/iopsys/icwmp/invalid_firmware_v1.0.bin bs=25MB count=1
echo "Invalid" > /builds/iopsys/icwmp/invalid_firmware_v1.0.bin
exec_cmd cp /builds/iopsys/icwmp/invalid_firmware_v1.0.bin /var/www/html

echo "Install Inform json files"
exec_cmd mkdir -p /etc/icwmpd
exec_cmd cp /builds/iopsys/icwmp/test/files/etc/icwmpd/* /etc/icwmpd

exec_cmd mkdir -p /var/state
exec_cmd cp /builds/iopsys/icwmp/test/files/var/state/cwmp /var/state
mkdir -p /var/state/icwmpd

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

mkdir -p /etc/icwmpd
mkdir -p /etc/config
cp test/files/etc/config/* /etc/config/

mkdir -p /etc/board-db/config/
cp test/files/etc/board-db/config/device /etc/board-db/config/

echo "Display cwmp config: "
cat /etc/config/cwmp 

echo "Running the unit test cases"
make unit-test -C test/cmocka/
check_ret $?

exec_cmd rm -rf /etc/icwmpd/*

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
