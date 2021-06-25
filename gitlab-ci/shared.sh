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
	exec_cmd dd if=/dev/zero of=/builds/iopsys/icwmp/firmware_v1.0.bin bs=25MB count=1
	curl -X PUT 'http://localhost:7557/files/firmware_v1.0.bin' --data-binary '@/builds/iopsys/icwmp/firmware_v1.0.bin' --header "fileType: 1 Firmware Upgrade Image" --header "oui: XXX" --header "productClass: FirstClass" --header "version: 000000001" >/dev/null 2>&1
	check_ret $?
}

function configure_acs_url()
{
	url="http://`hostname -i`:7547"
	uci set cwmp.acs.url=$url
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
		rm -f *.o *.log *.xml vgcore.* firmware_v1.0.bin
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

	# compile icwmp
	autoreconf -i >/dev/null 2>&1
	./configure CFLAGS="$COV_CFLAGS" LDFLAGS="$COV_LDFLAGS" --enable-acs=multi --enable-debug >/dev/null 2>&1
	make >/dev/null 2>&1
	check_ret $?
}