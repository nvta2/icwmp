#!/bin/sh
# Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
# Copyright (C) 2013-2019 iopsys Software Solutions AB
#  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
#  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>


. /usr/share/libubox/jshn.sh
#TODO help message

NEW_LINE='\n'
CWMP_PROMPT="icwmp>"
UCI_GET="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} get"
UCI_SHOW="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} show"
UCI_IMPORT="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} import"
UCI_EXPORT="/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} export"
TMP_SET_VALUE="/tmp/.tmp_set_value"
TMP_SET_NOTIFICATION="/tmp/.tmp_set_notification"


# Fault codes
FAULT_CPE_NO_FAULT="0"
FAULT_CPE_INTERNAL_ERROR="2"
FAULT_CPE_DOWNLOAD_FAILURE="10"
FAULT_CPE_UPLOAD_FAILURE="11"
FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER="15"
FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED="18"
FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION="19"

ICWMP_DOWNLOAD_FILE="/tmp/icwmp_download"

FIRMWARE_UPGRADE_IMAGE="/tmp/firmware.bin"

for ffile in `ls /usr/share/icwmp/functions/`; do
. /usr/share/icwmp/functions/$ffile
done

	
case "$1" in
	set)
		if [ "$2" = "notification" ]; then
			__arg1="$3"
			__arg2="$4"
			__arg3="$5"
			action="set_notification"
		elif [ "$2" = "value" ]; then
			__arg1="$3"
			__arg2="$4"
			__arg3="$5"
			action="set_value"
		else
			__arg1="$2"
			__arg2="$3"
			__arg3="$4"
			action="set_value"
		fi
		;;
	get)
		if [ "$2" = "notification" ]; then
			__arg1="$3"
			action="get_notification"
		elif [ "$2" = "value" ]; then
			__arg1="$3"
			action="get_value"
		elif [ "$2" = "name" ]; then
			__arg1="$3"
			__arg2="$4"
			action="get_name"
		else
			__arg1="$2"
			action="get_value"
		fi
		;;
	download)
		__arg1="$2"
		__arg2="$3"
		__arg3="$4"
		__arg4="$5"
		__arg5="$6"
		action="download"
		;;
	du_install)
		__arg1="$2"
		__arg2="$3"
		__arg3="$4"
		__arg4="$5"
		__arg5="$6"
		action="du_install"
		;;
	du_update)
		__arg1="$2"
		__arg2="$3"
		__arg3="$4"
		__arg4="$5"
		action="du_update"
		;;
	du_uninstall)
		__arg1="$2"
		__arg2="$3"
		action="du_uninstall"
		;;
	upload)
		__arg1="$2"
		__arg2="$3"
		__arg3="$4"
		__arg4="$5"
		__arg5="$6"
		action="upload"
		;;
	factory_reset)
		action="factory_reset"
		;;
	factory_reset_soft)
		action="factory_reset_soft"
		;;
	reboot)
		action="reboot"
		;;
	apply)
		if [ "$2" = "notification" ]; then
			action="apply_notification"
		elif [ "$2" = "value" ]; then
			__arg1="$3"
			action="apply_value"
		elif [ "$2" = "download" ]; then
			__arg1="$3"
			action="apply_download"
		else
			__arg1="$2"
			action="apply_value"
		fi
		;;
	add)
		__arg1="$2"
		__arg2="$3"
		action="add_object"
		;;
	delete)
		__arg1="$2"
		__arg2="$3"
		action="del_object"
		;;
	inform)
		action="inform"
		;;
	allow_cr_ip)
		action="allow_cr_ip"
		__arg1="$2"
		__arg2="$3"
		;;
	json_continuous_input)
		action="json_continuous_input"
		;;
	end)
		echo "$CWMP_PROMPT"
		;;
	exit)
		exit 0
	;;
esac

if [ -z "$action" ]; then
	echo invalid action \'$1\'
	exit 1
fi

handle_action() {
	local fault_code=$FAULT_CPE_NO_FAULT
	if [ "$action" = "get_value" ]; then
		 ubus call usp.raw get "{'path':'$__arg1','proto':'cwmp'}"
	fi
	
	if [ "$action" = "get_notification" ]; then
		ubus call usp.raw getm_attributes "{'paths':['$__arg1'],'proto':'cwmp'}"
	fi

	if [ "$action" = "get_name" ]; then
		if [ $__arg2 -eq 1 ]; then
			nextlevel=true
		else
			nextlevel=false
		fi
		ubus call usp.raw object_names "{'path':'$__arg1','proto':'cwmp','next-level':$nextlevel}"
	fi

	if [ "$action" = "set_value" ]; then
		transaction_start_ret=`ubus call usp.raw transaction_start '{"app":"cwmp"}'`
		if [ `echo $transaction_start_ret | jsonfilter -e @.status` = false ];then
			echo "Not able to start a transaction "
			exit 0
		fi
		transaction_id=`echo $transaction_start_ret | jsonfilter -e @.transaction_id`
		set_res=`ubus call usp.raw set "{'path':'$__arg1','value':'$__arg2','proto':'cwmp','key':'$__arg3','transaction_id':$transaction_id}"`
		if [ -z `echo $set_res | jsonfilter -e @.fault` ]; then
			if [ `echo $set_res | jsonfilter -e @.status` = true ]; then
				ubus call usp.raw transaction_commit "{'transaction_id':$transaction_id}" &> /dev/null
			else
				ubus call usp.raw transaction_abort "{'transaction_id':$transaction_id}" &> /dev/null
			fi
		else
			ubus call usp.raw transaction_abort "{'transaction_id':$transaction_id}" &> /dev/null
		fi 
		echo $set_res
		exit 0
	fi

	if [ "$action" = "set_notification" ]; then
		transaction_start_ret=`ubus call usp.raw transaction_start '{"app":"cwmp"}'`
		if [ `echo $transaction_start_ret | jsonfilter -e @.status` = false ];then
			echo "Not able to start a transaction "
			exit 0
		fi
		transaction_id=`echo $transaction_start_ret | jsonfilter -e @.transaction_id`
		set_notif=`ubus call usp.raw setm_attributes "{'paths':[{'path':'$__arg1','notify-type':'$__arg2','notify':'1'}], 'transaction_id': $transaction_id}"`
		if [ -z `echo $set_notif | jsonfilter -e @.fault` ]; then
			if [ -z `echo $set_notif | jsonfilter -e @.parameters[0].fault` ]; then
				ubus call usp.raw transaction_commit "{'transaction_id':$transaction_id}" &> /dev/null
			else
				ubus call usp.raw transaction_abort "{'transaction_id':$transaction_id}" &> /dev/null
			fi
		else
			ubus call usp.raw transaction_abort "{'transaction_id':$transaction_id}" &> /dev/null
		fi 
		echo $set_notif
		exit 0
	fi

	if [ "$action" = "add_object" -o "$action" = "del_object" ]; then
		transaction_start_ret=`ubus call usp.raw transaction_start '{"app":"cwmp"}'`
		if [ `echo $transaction_start_ret | jsonfilter -e @.status` = false ];then
			echo "Not able to start a transaction "
			exit 0
		fi
		transaction_id=`echo $transaction_start_ret | jsonfilter -e @.transaction_id`
		adddel_obj_res=`ubus call usp.raw $action "{'path':'$__arg1','key':'$__arg2','proto':'cwmp','instance_mode':0, 'transaction_id':$transaction_id}"`
		if [ -z `echo $adddel_obj_res | jsonfilter -e @.fault` ]; then
			if [ -z `echo $adddel_obj_res | jsonfilter -e @.parameters[0].fault` ]; then
				ubus call usp.raw transaction_commit "{'transaction_id':$transaction_id}" &> /dev/null
			else
				ubus call usp.raw transaction_abort "{'transaction_id':$transaction_id}" &> /dev/null
			fi
		else
			ubus call usp.raw transaction_abort "{'transaction_id':$transaction_id}" &> /dev/null
		fi 
		echo $adddel_obj_res
		exit 0
	fi

	if [ "$action" = "inform" ]; then
		/usr/sbin/icwmpd -m 1 "inform"
	fi

	if [ "$action" = "du_install" ]; then
		local fault_code="9000"
		ubus_args=`echo {\"url\":\"$__arg1\",\"uuid\":\"$__arg2\",\"username\":\"$__arg3\",\"password\":\"$__arg4\",\"environment\":\"$__arg5\"}`
		output=`ubus -t 3 call swmodules du_install $ubus_args`
		if [ "$output" != "" ];then
			json_init
			json_load "$output"
			json_get_var status status
			if [ "$status" == "0" ];then
				json_get_var error error
				if [ "$error" == "Download" ];then
					let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				else
					let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED
				fi
				icwmp_fault_output "" "$fault_code"
				return 1
			else
				json_get_var name name
				json_get_var uuid uuid
				json_get_var version version
				json_get_var environment environment
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT" "$name" "$version" "$uuid" "$environment"
			fi
		fi
	fi
	
	if [ "$action" = "du_update" ]; then
		local fault_code="9000"
		ubus_args=`echo {\"uuid\":\"$__arg1\",\"url\":\"$__arg2\",\"username\":\"$__arg3\",\"password\":\"$__arg4\"}`
		output=`ubus -t 3 call swmodules du_update $ubus_args`
		if [ "$output" != "" ];then
			json_init
			json_load "$output"
			json_get_var status status
			if [ "$status" == "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			else
				json_get_var name name
				json_get_var uuid uuid
				json_get_var version version
				json_get_var environment environment
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT" "$name" "$version" "$uuid" "$environment"
			fi
		fi
	fi
	
	if [ "$action" = "du_uninstall" ]; then
		local fault_code="9000"
		ubus_args=`echo {\"name\":\"$__arg1\",\"environment\":\"$__arg2\"}`
		output=`ubus -t 3 call swmodules du_uninstall $ubus_args`
		if [ "$output" != "" ];then
			json_init
			json_load "$output"
			json_get_var status status
			if [ "$status" == "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			else
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
			fi
		fi
	fi
	
	if [ "$action" = "download" ]; then
		local fault_code="9000"
		if [ "$__arg4" = "" -o "$__arg5" = "" ];then
			if [ "$__arg7" != "" ];then
				resp=$(curl --fail --capath $__arg7 --write-out %{http_code} --silent -o $ICWMP_DOWNLOAD_FILE $__arg1)
			elif [ ${__arg1:0:8} = https:// ];then
				resp=`curl --fail --write-out %{http_code} --silent -k --connect-timeout 10 --retry 1 -o $ICWMP_DOWNLOAD_FILE $__arg1`
			else
				resp=`curl --write-out %{http_code} --silent --connect-timeout 10 --retry 1 -o $ICWMP_DOWNLOAD_FILE $__arg1`
			fi
			if [ "$resp" == "404" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER
				icwmp_fault_output "" "$fault_code"
				return 1
			elif [ "$resp" == "401" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION
				icwmp_fault_output "" "$fault_code"
				return 1
			elif [ "$resp" != "200" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		else
			local url=`echo "$__arg1" | sed -e "s@://@://$__arg4:$__arg5\@@g"`
			if [ "$__arg7" != "" ];then
				resp=$(curl --fail --capath $__arg7 -u $__arg4:$__arg5 --anyauth --write-out %{http_code} --silent -o $ICWMP_DOWNLOAD_FILE $__arg1)
			elif [ ${__arg1:0:8} = https:// ];then
				resp=`curl --fail -u $__arg4:$__arg5 --anyauth --write-out %{http_code} --silent -k --connect-timeout 10 --retry 1 -o $ICWMP_DOWNLOAD_FILE $__arg1`
			else
				resp=`curl --fail -u $__arg4:$__arg5 --anyauth --write-out %{http_code} --silent --connect-timeout 10 --retry 1 -o $ICWMP_DOWNLOAD_FILE $__arg1`
			fi

			resp=`echo $resp| awk '{print $NF}'`
			if [ "$resp" == "404" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_CONTACT_SERVER
				icwmp_fault_output "" "$fault_code"
				return 1
			elif [ "$resp" == "401" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_AUTHENTICATION
				icwmp_fault_output "" "$fault_code"
				return 1
			elif [ "$resp" != "200" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		fi

		local flashsize=256000000 #flashsize="`icwmp_check_flash_size`"
		local filesize=`ls -l $ICWMP_DOWNLOAD_FILE | awk '{ print $5 }'`
		if [ $flashsize -gt 0 -a $flashsize -lt $__arg2 ]; then
			let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
			rm $ICWMP_DOWNLOAD_FILE 2> /dev/null
			icwmp_fault_output "" "$fault_code"
		else
			if [ "$__arg3" = "1 Firmware Upgrade Image" ];then
				mv $ICWMP_DOWNLOAD_FILE $FIRMWARE_UPGRADE_IMAGE 2> /dev/null
				(icwmp_check_image)
				if [ "$?" = "0" ];then
					if [ $flashsize -gt 0 -a $filesize -gt $flashsize ];then
						let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED
						rm -f $FIRMWARE_UPGRADE_IMAGE
						icwmp_fault_output "" "$fault_code"
					else
						icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
					fi
				else
					let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED
					rm -f $FIRMWARE_UPGRADE_IMAGE
					icwmp_fault_output "" "$fault_code"
				fi
			elif [ "$__arg3" = "2 Web Content" ];then
				mv $ICWMP_DOWNLOAD_FILE /tmp/web_content.ipk 2> /dev/null
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
			elif [ "$__arg3" = "3 Vendor Configuration File" ];then
				if [ "$__arg6" != "" ]; then
					local tmp="/etc/vendor_configuration_file_${__arg6}.cfg"
					mv $ICWMP_DOWNLOAD_FILE "$tmp" 2> /dev/null
				else
					mv $ICWMP_DOWNLOAD_FILE /tmp/vendor_configuration_file.cfg 2> /dev/null
				fi
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
			elif [ "$__arg3" = "6 Stored Firmware Image" ]; then
				mv $ICWMP_DOWNLOAD_FILE /tmp/owsd-repeater-control-cert.pem 2> /dev/null
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
			else
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				rm $ICWMP_DOWNLOAD_FILE 2> /dev/null
			fi
		fi
	fi
	
	if [ "$action" = "upload" ]; then
		local fault_code="9000"
		local flname=""
		case $__arg2 in
			"1"*|"3"*)
				if [ "$__arg5" = "" ];then
					__arg5="all_configs"
					$UCI_EXPORT > "/tmp/${__arg5}"
				else 
					$UCI_EXPORT "$__arg5" > "/tmp/${__arg5}"
				fi
				if [ "$__arg3" = "" -o "$__arg4" = "" ];then
					if [ ${__arg1:0:4} = http ];then
						resp=`curl -I -T "/tmp/${__arg5}" "$__arg1" 2>&1 | awk '/^HTTP/{print $2}' | awk '!/100/'`
						if [ "$resp" != "200" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					else
						curl -T "/tmp/${__arg5}" "$__arg1" &> /dev/null
						if [ "$?" != "0" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					fi
				else
					if [ ${__arg1:0:4} = http ];then
						resp=`curl -I -T "/tmp/${__arg5}" -u $__arg3:$__arg4 "$__arg1" 2>&1 | awk '/^HTTP/{print $2}' | awk '!/100/'`
						if [ "$resp" != "200" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					else
						curl -T "/tmp/${__arg5}" -u $__arg3:$__arg4 "$__arg1" &> /dev/null
						if [ "$?" != "0" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					fi
				fi
			;;
			"2"*|"4"*)
				flname=`$UCI_GET system.@system[0].log_file`
				if [ "$flname" = "" ];then
					let fault_code=$fault_code+$FAULT_CPE_INTERNAL_ERROR
					icwmp_fault_output "" "$fault_code"
					return 1
				fi
				if [ "$__arg3" = "" -o "$__arg4" = "" ];then
					if [ ${__arg1:0:4} = http ];then
						resp=`curl -I -T "$flname" "$__arg1" 2>&1 | awk '/^HTTP/{print $2}' | awk '!/100/'`
						if [ "$resp" != "200" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					else
						curl -T "$flname" "$__arg1" &> /dev/null
						if [ "$?" != "0" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					fi
				else
					if [ ${__arg1:0:4} = http ];then
						resp=`curl -I -T "$flname" -u $__arg3:$__arg4 "$__arg1" 2>&1 | awk '/^HTTP/{print $2}' | awk '!/100/'`
						if [ "$resp" != "200" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					else
						curl -T "$flname" -u $__arg3:$__arg4 "$__arg1" &> /dev/null
						if [ "$?" != "0" ];then
							let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
							icwmp_fault_output "" "$fault_code"
							return 1
						fi
					fi
				fi
			;;
		esac
	fi
	
	if [ "$action" = "apply_download" ]; then
		case "$__arg1" in
			"1 Firmware Upgrade Image")
				icwmp_apply_firmware
			;;
			"2 Web Content")
				if [ "$__arg2" != "0" ]; then 
					icwmp_apply_web_content $__arg2
				else
					icwmp_apply_web_content
				fi
			;;
			"3 Vendor Configuration File")
				if [ "$__arg2" != "" ]; then 
					icwmp_apply_vendor_configuration $__arg2
				else
					icwmp_apply_vendor_configuration
				fi
			;;
			"6 Stored Firmware Image")
				icwmp_apply_ca_ssl_certificate_key
			;;
		esac
	fi

	if [ "$action" = "factory_reset" ]; then
		/sbin/defaultreset
	fi
	
	if [ "$action" = "factory_reset_soft" ]; then
		/sbin/defaultreset
	fi
	
	if [ "$action" = "reboot" ]; then
		sync
		uci set cwmp.acs.ParameterKey="$commandKey"
		uci commit cwmp
		reboot
	fi

	if [ "$action" = "allow_cr_ip" ]; then
		local port=`$UCI_GET cwmp.cpe.port`
		local if_wan=`$UCI_GET cwmp.cpe.default_wan_interface`
		local zone=`$UCI_SHOW firewall | grep "firewall\.@zone\[[0-9]\+\]\.network=.*$if_wan" | head -1 | cut -f2 -d.`
		local zone_name=`$UCI_GET firewall.$zone.name`
		if [ "$zone_name" = "" ]; then
			zone_name=icwmp	
		fi
		sed -i "s,^port=.*,port=${port},g" /etc/firewall.cwmp
		sed -i "s,^zone_name=.*,zone_name=${zone_name},g" /etc/firewall.cwmp	
		# update iptables rule
		if [ "$__arg2" != "1" ]; then 
			sed -i "s,^.*iptables.*Open ACS port.*,iptables -I zone_${zone_name}_input -p tcp -s $__arg1 --dport $port -j ACCEPT -m comment --comment=\"Open ACS port\",g" /etc/firewall.cwmp			
		else
			sed -i "s,^.*iptables.*Open ACS port.*,ip6tables -I zone_${zone_name}_input -p tcp -s $__arg1 --dport $port -j ACCEPT -m comment --comment=\"Open ACS port\",g" /etc/firewall.cwmp			
		fi
		fw3 reload
	fi
	
	if [ "$action" = "json_continuous_input" ]; then
		echo "$CWMP_PROMPT"
		while read CMD; do
			[ -z "$CMD" ] && continue
			result=""
			json_init
			json_load "$CMD"
			json_get_var command command
			json_get_var action action
			json_get_var arg arg
			case "$command" in
				set)
					if [ "$action" = "notification" ]; then
						json_get_var __arg1 parameter
						json_get_var __arg2 value
						json_get_var __arg3 change
						action="set_notification"
					elif [ "$action" = "value" ]; then
						json_get_var __arg1 parameter
						json_get_var __arg2 value
						action="set_value"
					else
						json_get_var __arg1 parameter
						json_get_var __arg2 value
						action="set_value"
					fi
					;;
				get)
					if [ "$action" = "notification" ]; then
						json_get_var __arg1 parameter
						action="get_notification"
					elif [ "$action" = "value" ]; then
						json_get_var __arg1 parameter
						action="get_value"
					elif [ "$action" = "name" ]; then
						json_get_var __arg1 parameter
						json_get_var __arg2 next_level
						action="get_name"
					else
						json_get_var __arg1 parameter
						action="get_value"
					fi
					;;
				download)
					json_get_var __arg1 url
					json_get_var __arg2 size
					json_get_var __arg3 type
					json_get_var __arg4 user
					json_get_var __arg5 pass
					json_get_var __arg6 ids
					json_get_var __arg7 cert_path
					action="download"
					;;
				du_install)
					json_get_var __arg1 url
					json_get_var __arg2 uuid
					json_get_var __arg3 user
					json_get_var __arg4 pass
					json_get_var __arg5 env
					action="du_install"
					;;
				du_update)
					json_get_var __arg1 uuid
					json_get_var __arg2 url
					json_get_var __arg3 user
					json_get_var __arg4 pass
					action="du_update"
					;;
				du_uninstall)
					json_get_var __arg1 name
					json_get_var __arg2 env
					action="du_uninstall"
					;;
				upload)
					json_get_var __arg1 url
					json_get_var __arg2 type
					json_get_var __arg3 user
					json_get_var __arg4 pass
					json_get_var __arg5 name
					action="upload"
					;;
				factory_reset)
					action="factory_reset"
					;;
				factory_reset_soft)
					action="factory_reset_soft"
					;;
				reboot)
					action="reboot"
					commandKey="$arg"
					;;
				apply)
					if [ "$action" = "notification" ]; then
						action="apply_notification"
					elif [ "$action" = "value" ]; then
						json_get_var __arg1 arg
						action="apply_value"
					elif [ "$action" = "download" ]; then
						json_get_var __arg1 arg
						json_get_var __arg2 ids
						action="apply_download"
					else
						json_get_var __arg1 arg
						action="apply_value"
					fi
					;;
				add)
					json_get_var __arg1 parameter
					json_get_var __arg2 parameter_key
					action="add_object"
					;;
				delete)
					json_get_var __arg1 parameter
					json_get_var __arg2 parameter_key
					action="del_object"
					;;
				inform)
					action="inform"
					;;
				allow_cr_ip)
					action="allow_cr_ip"
					json_get_var __arg1 arg
					json_get_var __arg2 ipv6
					;;
				end)
					echo "$CWMP_PROMPT"
					;;
				exit)
					exit 0
					;;
				*)
					continue
					;;
			esac
			handle_action
		done
	
		exit 0;
	fi
}

handle_action 2> /dev/null

