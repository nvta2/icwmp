#!/bin/bash

source ./test/script/common.sh
source ./gitlab-ci/shared.sh

TEST_NAME="Custom Notifications"

echo "Running: $TEST_NAME"

echo "Install notification json files"
exec_cmd mkdir -p /etc/icwmpd
exec_cmd cp test/files/etc/icwmpd/custom_notification* /etc/icwmpd

#
# Test a valid custom notification json file
#
rm /var/log/icwmpd.log
uci delete cwmp.@notifications[0]
uci commit cwmp

exec_cmd uci set cwmp.cpe.json_custom_notify_file="/etc/icwmpd/custom_notification_valid.json"
uci commit cwmp

supervisorctl start icwmpd
sleep 5

notif1=`uci get cwmp.@notifications[0].active | grep "Device.Users."`
if [[ $notif1 != *"Device.Users."* ]]; then
	echo "FAIL: active notifications list doesn't contain Device.Users. parameter"
	exit 1
fi
notif2=`uci get cwmp.@notifications[0].passive | grep "Device.WiFi.SSID.1.SSID"`
if [[ $notif2 != *"Device.WiFi.SSID.1.SSID"* ]]; then
	echo "FAIL: active notifications list doesn't contain Device.WiFi.SSID.1.SSID parameter"
	exit 1
fi

supervisorctl stop icwmpd
rm /etc/icwmpd/.icwmpd_notify

echo "PASS test valid custom notification json file"

#
# Test custom notification invalid json file
#
rm /var/log/icwmpd.log
uci delete cwmp.@notifications[0]
uci commit cwmp

exec_cmd uci set cwmp.cpe.json_custom_notify_file="/etc/icwmpd/custom_notification_invalid_json.json"
uci commit cwmp

supervisorctl start icwmpd
sleep 5

notif1=`uci get cwmp.@notifications[0].active | grep "Device.Users."`
if [[ $notif1 == *"Device.Users."* ]]; then
	echo "FAIL: the json file is invalid, the active notifcation list shouldn't contain Device.Users. parameter"
	exit 1
fi
notif2=`uci get cwmp.@notifications[0].passive | grep "Device.WiFi.SSID.1.SSID"`
if [[ $notif2 == *"Device.WiFi.SSID.1.SSID"* ]]; then
	echo "FAIL: the json file is invalid, the active notifcation list shouldn't contain Device.WiFi.SSID.1.SSID parameter"
	exit 1
fi

logfile=`cat /var/log/icwmpd.log`
if [[ $logfile != *"[WARNING] The file /etc/icwmpd/custom_notification_invalid_json.json is not a valid JSON file"* ]]; then
	echo "FAIL: Log file doesn't contain a WARNING that the file /etc/icwmpd/custom_notification_invalid_json.json is not valid."
	exit 1
fi 

echo "PASS test custom notification invalid json file"
supervisorctl stop icwmpd
rm /etc/icwmpd/.icwmpd_notify

#
# Test custom notification json file containing forced active notification
#
rm /var/log/icwmpd.log
uci delete cwmp.@notifications[0]
uci commit cwmp

exec_cmd uci set cwmp.cpe.json_custom_notify_file="/etc/icwmpd/custom_notification_forced.json"
uci commit cwmp

supervisorctl start icwmpd
sleep 5

notif1=`uci get cwmp.@notifications[0].active | grep "Device.Users."`
if [[ $notif1 != *"Device.Users."* ]]; then
	echo "FAIL: active notifications list doesn't contain Device.Users. parameter"
	exit 1
fi
notif2=`uci get cwmp.@notifications[0].passive | grep "Device.DeviceInfo.ProvisioningCode"`
if [[ $notif2 == *"Device.DeviceInfo.ProvisioningCode"* ]]; then
	echo "FAIL: passive notifications list contains Device.DeviceInfo.ProvisioningCode while it's a forced active notification paramter"
	exit 1
fi

logfile=`cat /var/log/icwmpd.log`
if [[ $logfile != *"[WARNING] This parameter Device.DeviceInfo.ProvisioningCode is forced notification parameter, can't be changed"* ]]; then
	echo "FAIL: Device.DeviceInfo.ProvisioningCode is forced notification parameter, can't be changed"
	exit 1
fi 

echo "PASS test custom notification json file containing forced active notification"
supervisorctl stop icwmpd
rm /etc/icwmpd/.icwmpd_notify

#
# Test custom notification json file containing invalid parameter path
#
rm /var/log/icwmpd.log
uci delete cwmp.@notifications[0]
uci commit cwmp

exec_cmd uci set cwmp.cpe.json_custom_notify_file="/etc/icwmpd/custom_notification_invalid_parameter.json"
uci commit cwmp

supervisorctl start icwmpd
sleep 5

notif1=`uci get cwmp.@notifications[0].active | grep "Device.Users."`
if [[ $notif1 != *"Device.Users."* ]]; then
	echo "FAIL: active notifications list doesn't contain Device.Users. parameter"
	exit 1
fi
notif2=`uci get cwmp.@notifications[0].passive | grep "Device.WiFi.SSID.1.SD"`
if [[ $notif2 == *"Device.WiFi.SSID.1.SD"* ]]; then
	echo "FAIL: passive notifications list contains Device.WiFi.SSID.1.SD while it's a wrong parameter path"
	exit 1
fi

logfile=`cat /var/log/icwmpd.log`
if [[ $logfile != *"[WARNING] The parameter Device.WiFi.SSID.1.SD is wrong path"* ]]; then
	echo "FAIL: Log file should contain WARNING that Device.WiFi.SSID.1.SD is wrong parameter path."
	exit 1
fi 

echo "PASS test custom notification json file containing invalid parameter path"
supervisorctl stop icwmpd
rm /etc/icwmpd/.icwmpd_notify

echo "PASS: $TEST_NAME"