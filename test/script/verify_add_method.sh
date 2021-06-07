#!/bin/bash

source ./test/script/common.sh
source ./gitlab-ci/shared.sh

TEST_NAME="ADD RPC Method"

echo "Running: $TEST_NAME"

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "addObject","objectName":"Device.Users.User"}' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "AddObject"
obj_instance=$(print_tag_value "cwmp:AddObjectResponse" "InstanceNumber")
status=$(print_tag_value "cwmp:AddObjectResponse" "Status")
if [ "$obj_instance" != "2" -o $status != "1" ]; then
	echo "Error: Add Object Method doesn't work correctly, current_value($obj_instance) expected_value(2)" >> ./funl-test-debug.log
	exit 1
fi

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "getParameterValues", "parameterNames": ["Device.Users.User.2"] }' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "GetParameterValues"

echo "PASS: $TEST_NAME"
