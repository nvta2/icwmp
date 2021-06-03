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
		exit $ret
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

function build_cwmp()
{
	COV_CFLAGS='-g -O0'
	#COV_CFLAGS='-g -O0 -fprofile-arcs -ftest-coverage'
	#COV_LDFLAGS='--coverage'

	make clean >/dev/null 2>&1

	# compile obuspa
	autoreconf -i
	./configure CFLAGS="$COV_CFLAGS" LDFLAGS="$COV_LDFLAGS" --enable-acs=multi --enable-debug
	make
	check_ret $?
}
