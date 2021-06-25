#!/bin/bash

source ./test/script/common.sh
source ./gitlab-ci/shared.sh

TEST_NAME="SET RPC Method"

echo "Running: $TEST_NAME"

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "getParameterValues", "parameterNames": ["Device.Users.User.1.Enable"] }' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "GetParameterValues"
param_value_before=$(print_tag_value "cwmp:GetParameterValuesResponse" "Value xsi:type=\"xsd:boolean\"")
if [ "$param_value_before" != "1" ]; then
	echo "Error: Default value of 'Device.Users.User.1.Enable' is wrong, current_value($param_value_before) expected_value(1)" >> ./funl-test-debug.log
	exit 1
fi

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "setParameterValues", "parameterValues": [["Device.Users.User.1.Enable",false]]}' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "SetParameterValues"
get_status=$(print_tag_value "cwmp:SetParameterValuesResponse" "Status")
if [ "$get_status" != "1" ]; then
	echo "Error: Set Value doesn't work correctly" >> ./funl-test-debug.log
	exit 1
fi

remove_icwmp_log
curl $connection_request_path -X POST --data '{"name": "getParameterValues", "parameterNames": ["Device.Users.User.1.Enable"] }' >/dev/null 2>&1
check_ret $?
sleep 2
check_session "GetParameterValues"
param_value_after=$(print_tag_value "cwmp:GetParameterValuesResponse" "Value xsi:type=\"xsd:boolean\"")
if [ "$param_value_after" != "0" ]; then
	echo "Error: the value of 'Device.Users.User.1.Enable' is wrong, current_value($param_value_after) expected_value(0)" >> ./funl-test-debug.log
	exit 1
fi

echo "PASS: $TEST_NAME"
