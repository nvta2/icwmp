#!/bin/bash

connection_request_path="http://localhost:7557/devices/XXX-FirstClass-000000001/tasks?timeout=3000&connection_request"
icwmp_log_file="/var/log/icwmpd.log"
last_req=0

valid_request()
{
	message="$1"
	title="$2"
	message_line=`grep -n "$message" $icwmp_log_file | awk -F ":" 'BEGIN { ORS=" " }; {print $1}'`
	if [[ -z $message_line ]]; then
		echo "Error: No $title Message found" >> ./funl-test-debug.log
		exit 1
	fi
	second_instance=`echo $message_line | awk '{print $2}'`
	if [[ ! -z $second_instance ]]; then
		echo "Error: There are multiple $title Message" >> ./funl-test-debug.log
		exit 1
	fi
	if [ $last_req -ge $message_line ]; then
		echo "Error: Failed session wrong order of requests" >> ./funl-test-debug.log
		exit 1
	fi
	last_req=$message_line
}

function check_session()
{
	last_req=0
	rpc_method=$1
	echo "Check RPC Method: $rpc_method" >> ./funl-test-debug.log
	
	echo "Check Inform" >> ./funl-test-debug.log
	valid_request "<cwmp:Inform>" "Inform"
	valid_request "<cwmp:InformResponse>" "InformResponse"

	#echo "Check Empty message" >> ./funl-test-debug.log
	#valid_request "Send empty message to the ACS" "Empty"
	
	echo "Check RPC method" >> ./funl-test-debug.log
	valid_request "<cwmp:$rpc_method>" "$rpc_method"
	valid_request "<cwmp:${rpc_method}Response>" "${rpc_method}Response"
	
	echo "Check End Session" >> ./funl-test-debug.log
	valid_request "Receive HTTP 204 No Content" "End Session"
	
	echo "Tests pass" >> ./funl-test-debug.log
}

function remove_icwmp_log()
{
	echo "" > $icwmp_log_file
}

function print_tag_value()
{
	rpc_method=$1
	tag=$2
	xml_data=`awk -v start="<"$rpc_method">" -v end="</"$rpc_method">" '$0~start,$1~end' $icwmp_log_file`
	tag_value=`grep -oPm1 "(?<=<$tag>)[^<]+" <<< "$xml_data"`
	echo $tag_value
}
