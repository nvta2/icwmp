#!/bin/bash

source ./test/script/common.sh
source ./gitlab-ci/shared.sh

TEST_NAME="DOWNLOAD RPC Method"

echo "Running: $TEST_NAME"

remove_icwmp_log
curl $connection_request_path -X POST  --data '{"name": "download", "file": "firmware_v1.0.bin"}' >/dev/null 2>&1
check_ret $?
sleep 10
check_session "Download"
sent_command_key=$(print_tag_value "cwmp:Download" "CommandKey")
status=$(print_tag_value "cwmp:DownloadResponse" "Status")
if [ "$status" != "1" ]; then
	echo "Error: Download Method doesn't work correctly, current_value($status) expected_value(1)" >> ./funl-test-debug.log
	exit 1
fi

rm /etc/icwmpd/.dm_enabled_notify
remove_icwmp_log
echo "Restarting icwmpd in order to apply the new firmware"  >> ./funl-test-debug.log
supervisorctl restart icwmpd  >> ./funl-test-debug.log
sleep 5
check_session "TransferComplete"
received_command_key=$(print_tag_value "cwmp:TransferComplete" "CommandKey")
if [ "$sent_command_key" != "$received_command_key" ]; then
	echo "Error: Download Method && The received command key is not conform with the sent command key, current_value($received_command_key) expected_value($sent_command_key)" >> ./funl-test-debug.log
	exit 1
fi

start_time=$(print_tag_value "cwmp:TransferComplete" "StartTime")
if [ -z "$start_time" ]; then
	echo "Error: Download Method && StartTime must not be empty" >> ./funl-test-debug.log
	exit 1
fi

complete_time=$(print_tag_value "cwmp:TransferComplete" "CompleteTime")
if [ -z "$complete_time" ]; then
	echo "Error: Download Method && CompleteTime must not be empty" >> ./funl-test-debug.log
	exit 1
fi

fault_code=$(print_tag_value "FaultStruct" "FaultCode")
if [ "$fault_code" != "0" ]; then
	echo "Error: Download Method && FaultCode is not correct, current_value($fault_code) expected_value(0)" >> ./funl-test-debug.log
	exit 1
fi

fault_string=$(print_tag_value "FaultStruct" "FaultString")
if [[ ! -z "$fault_string" ]]; then
	echo "Error: Download Method && FaultString must be empty, current_value($fault_string) expected_value()" >> ./funl-test-debug.log
	exit 1
fi

echo "Checking received events ..." >> ./funl-test-debug.log
grep -q "<EventCode>7 TRANSFER COMPLETE</EventCode>" $icwmp_log_file
check_ret $?

grep -q "<EventCode>M Download</EventCode>" $icwmp_log_file
check_ret $?

echo "PASS: $TEST_NAME"
