#!/bin/bash

source ./test/script/common.sh
source ./gitlab-ci/shared.sh

TEST_NAME="GET RPC Method"

echo "Running: $TEST_NAME"

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "getParameterValues", "parameterNames": ["Device.Users.User.1.Username"] }' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "GetParameterValues"
param_value=$(print_tag_value "cwmp:GetParameterValuesResponse" "Value xsi:type=\"xsd:string\"")
if [ "$param_value" != "user" ]; then
	echo "Error: Default value of 'Device.Users.User.1.Username' is wrong, current_value($param_value) expected_value(user)" >> ./funl-test-debug.log
	exit 1
fi

echo "PASS: $TEST_NAME"
