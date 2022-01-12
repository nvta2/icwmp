#!/bin/bash

function cleanup()
{
	echo ""
}

function check_ret()
{
	ret=$1
	if [ "$ret" -ne 0 ]; then
		echo "Validation of last command failed, ret(${ret})"
		exit $ret
	fi

}

function error_on_zero()
{
	ret=$1
	if [ "$ret" -eq 0 ]; then
		echo "Validation of last command failed, ret(${ret})"
		exit 1
	fi

}

function exec_cmd()
{
	echo "executing $@"
	$@ >/dev/null 2>&1

	if [ $? -ne 0 ]; then
		echo "Failed to execute $@"
		exit 1
	fi
}

function configure_genieacs()
{
	sleep 3
	echo "create a new user"
	curl -X POST 'http://localhost:3000/init' -H "Content-Type: application/json" --data '{"users": true, "presets": true, "filters": true, "device": true, "index": true, "overview": true}' >/dev/null 2>&1
	check_ret $?

	echo "delete the default provision inform"
	curl -X DELETE 'http://localhost:7557/provisions/inform' >/dev/null 2>&1
	check_ret $?

	echo "add a new provision inform"
	curl -X PUT 'http://localhost:7557/provisions/inform' --data-binary '@/builds/iopsys/icwmp/test/acs/connection_request_auth' >/dev/null 2>&1
	check_ret $?

	#echo "get the supported provisions"
	#curl -X GET 'http://localhost:7557/provisions/'
	#check_ret $?

	echo "upload firmware image to genieacs server"
	exec_cmd dd if=/dev/zero of=/tmp/firmware_v1.0.bin bs=25MB count=1
	echo "Valid" > /tmp/firmware_v1.0.bin
	curl -X PUT 'http://localhost:7557/files/firmware_v1.0.bin' --data-binary '@/tmp/firmware_v1.0.bin' --header "fileType: 1 Firmware Upgrade Image" --header "oui: XXX" --header "productClass: FirstClass" --header "version: 000000001" >/dev/null 2>&1
	check_ret $?

	exec_cmd ubus call tr069 inform
	sleep 5
	echo "## Restarting cwmp client ##"
	supervisorctl restart icwmpd
	exec_cmd ubus wait_for tr069
}

function configure_acs_url()
{
	uci set cwmp.acs.url="http://127.0.0.1:7547"
	uci set cwmp.cpe.interface="lo"
	uci commit cwmp
	echo "Current ACS URL=$url"
}

function check_cwmp_status()
{
	status=`ubus call tr069 status | jq -r ".cwmp.status"`
	if [ $status != "up" ]; then
		echo "icwmpd is not started correctly, (the current status=$status)"
		exit 1
	fi
}

function clean_icwmp()
{
	if [ -f Makefile ]; then
		exec_cmd make maintainer-clean
		exec_cmd make -C test/cmocka clean
		find -name '*.gcda' -exec rm {} -fv \;
		find -name '*.gcno' -exec rm {} -fv \;
		find -name '*.gcov' -exec rm {} -fv \;
		find -name '*.deps' -exec rm {} -rfv \;
		find -name '*.so' -exec rm {} -fv \;
		rm -f *.o *.log *.xml firmware_v1.0.bin
		rm -rf report
	fi
}

function build_icwmp()
{
	#COV_CFLAGS='-g -O0'
	COV_CFLAGS='-fprofile-arcs -ftest-coverage'
	COV_LDFLAGS='--coverage'

	# clean icwmp
	clean_icwmp

	mkdir -p /var/state
	mkdir -p /var/run
	mkdir -p /var/run/icwmpd

	# compile icwmp
	autoreconf -i >/dev/null 2>&1
	./configure CFLAGS="$COV_CFLAGS" LDFLAGS="$COV_LDFLAGS" --enable-acs=multi --enable-debug >/dev/null 2>&1
	make >/dev/null 2>&1
	check_ret $?
}
