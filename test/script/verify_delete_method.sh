#!/bin/bash

source ./test/script/common.sh
source ./gitlab-ci/shared.sh

TEST_NAME="DELETE RPC Method"

echo "Running: $TEST_NAME"

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "deleteObject","objectName":"Device.Users.User.2"}' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "DeleteObject"
status=$(print_tag_value "cwmp:DeleteObjectResponse" "Status")
if [ "$status" != "1" ]; then
	echo "Error: Delete Object Method doesn't work correctly, current_value($status) expected_value(1)" >> ./funl-test-debug.log
	exit 1
fi

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "getParameterValues", "parameterNames": ["Device.Users.User"] }' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "GetParameterValues"
if grep -q "Device.Users.User.2" "$icwmp_log_file"; then
	echo "Error: 'Device.Users.User.2' object is not really deleted" >> ./funl-test-debug.log
	exit 1
fi

echo "PASS: $TEST_NAME"
