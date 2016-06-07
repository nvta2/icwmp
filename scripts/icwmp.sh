#!/bin/sh
# Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
# Copyright (C) 2013 Inteno Broadband Technology AB
#  Author Mohamed Kallel <mohamed.kallel@pivasoftware.com>
#  Author Ahmed Zribi <ahmed.zribi@pivasoftware.com>

. /usr/share/libubox/jshn.sh
#TODO help message

NEW_LINE='\n'
CWMP_PROMPT="icwmp>"
UCI_GET="/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} get -q"
UCI_SHOW="/sbin/uci ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} show -q"
TMP_SET_VALUE="/tmp/.tmp_set_value"
TMP_SET_NOTIFICATION="/tmp/.tmp_set_notification"


# Fault codes
FAULT_CPE_NO_FAULT="0"
FAULT_CPE_INTERNAL_ERROR="2"
FAULT_CPE_DOWNLOAD_FAILURE="10"
FAULT_CPE_UPLOAD_FAILURE="11"
FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED="18"

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
			action="set_value"
		else
			__arg1="$2"
			__arg2="$3"
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
	du_download)
		__arg1="$2"
		__arg2="$3"
		__arg3="$4"
		action="du_download"
		;;
	upload)
		__arg1="$2"
		__arg2="$3"
		__arg3="$4"
		__arg4="$5"
		action="upload"
		;;
	factory_reset)
		action="factory_reset"
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
		__arg1="$3"
		__arg2="$4"
		action="add_object"
		;;
	delete)
		__arg1="$3"
		__arg2="$4"
		action="delete_object"
		;;
	inform)
		action="inform"
		;;
	allow_cr_ip)
		action="allow_cr_ip"
		__arg1="$2"
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
	if [ "$action" = "get_value" -o "$action" = "get_notification" ]; then
		/usr/sbin/icwmpd -m 1 $action "$__arg1"
	fi

	if [ "$action" = "get_name" ]; then
		/usr/sbin/icwmpd -m 1 get_name "$__arg1" "$__arg2"
	fi

	if [ "$action" = "set_value" ]; then
		json_init
		json_add_string "parameter" "$__arg1"
		json_add_string "value" "$__arg2"
		json_dump >> $TMP_SET_VALUE
		json_close_object
	fi

	if [ "$action" = "set_notification" ]; then
		json_init
		json_add_string "parameter" "$__arg1"
		json_add_string "value" "$__arg2"
		json_dump >> $TMP_SET_NOTIFICATION
		json_close_object
	fi

	if [ "$action" = "apply_value" ]; then
		local svargs="-m 1 set_value \"$__arg1\""
		local svp svv
		while read line; do
			json_init
			json_load "$line"
			json_get_var svp parameter
			json_get_var svv value
			svargs="$svargs \"$svp\" \"$svv\""
		done < $TMP_SET_VALUE
		eval "/usr/sbin/icwmpd $svargs"
		rm -f $TMP_SET_VALUE
	fi

	if [ "$action" = "apply_notification" ]; then
		local snargs="-m 1 set_notification"
		local snp snv
		while read line; do
			json_init
			json_load "$line"
			json_get_var snp parameter
			json_get_var snv value
			snargs="$snargs \"$snp\" \"$snv\""
		done < $TMP_SET_NOTIFICATION
		eval "/usr/sbin/icwmpd $snargs"
		rm -f $TMP_SET_NOTIFICATION
	fi


	if [ "$action" = "add_object" -o "$action" = "delete_object" ]; then
		/usr/sbin/icwmpd -m 1 get_value "$__arg2" "$__arg1"		
	fi

	if [ "$action" = "inform" ]; then
		/usr/sbin/icwmpd -m 1 "inform"
	fi

	if [ "$action" = "du_download" ]; then
		local fault_code="9000"
		if [ "$__arg2" = "" -o "$__arg3" = "" ];then
			wget -O /tmp/icwmp_du_download "$__arg1" 2> /dev/null
			if [ "$?" != "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		else
			local url="http://$__arg2:$__arg3@`echo $__arg1|sed 's/http:\/\///g'`"
			wget -O /tmp/icwmp_du_download "$url" 2> /dev/null
			if [ "$?" != "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		fi
		mv /tmp/icwmp_du_download /tmp/du_change_state.ipk 2> /dev/null
		icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"			
	fi
	if [ "$action" = "du_uninstall" ]; then
		/bin/opkg remove "$__arg1" 2> /dev/null
		if [ "$?" != "0" ];then
			let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
			icwmp_fault_output "" "$fault_code"
			return 1
		fi
		icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"			
	fi
	if [ "$action" = "download" ]; then
		local fault_code="9000"
		if [ "$__arg4" = "" -o "$__arg5" = "" ];then
			wget -O /tmp/icwmp_download "$__arg1" 2> /dev/null
			if [ "$?" != "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		else
			local url="http://$__arg4:$__arg5@`echo $__arg1|sed 's/http:\/\///g'`"
			wget -O /tmp/icwmp_download "$url" 2> /dev/null
			if [ "$?" != "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		fi

		local flashsize="`icwmp_check_flash_size`" #ALZ
		local filesize=`ls -l /tmp/icwmp_download | awk '{ print $5 }'`
		if [ $flashsize -gt 0 -a $flashsize -lt $__arg2 ]; then
			let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
			rm /tmp/icwmp_download 2> /dev/null
			icwmp_fault_output "" "$fault_code"
		else
			if [ "$__arg3" = "1" ];then
				mv /tmp/icwmp_download /tmp/firmware_upgrade_image 2> /dev/null
				(icwmp_check_image)
				if [ "$?" = "0" ];then
					if [ $flashsize -gt 0 -a $filesize -gt $flashsize ];then
						let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED
						rm /tmp/firmware_upgrade_image 2> /dev/null
						icwmp_fault_output "" "$fault_code"
					else
						rm /tmp/firmware_upgrade_image_last_valid 2> /dev/null
						mv /tmp/firmware_upgrade_image /tmp/firmware_upgrade_image_last_valid 2> /dev/null
						icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
					fi
				else
					let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAIL_FILE_CORRUPTED
					rm /tmp/firmware_upgrade_image 2> /dev/null
					icwmp_fault_output "" "$fault_code"
				fi
			elif [ "$__arg3" = "2" ];then
				mv /tmp/icwmp_download /tmp/web_content.ipk 2> /dev/null
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
			elif [ "$__arg3" = "3" ];then
				if [ "$__arg6" != "0" ]; then
					local tmp="/etc/vendor_configuration_file_${__arg6}.cfg"
					mv /tmp/icwmp_download "$tmp" 2> /dev/null
				else
					mv /tmp/icwmp_download /tmp/vendor_configuration_file.cfg 2> /dev/null
				fi
				icwmp_fault_output "" "$FAULT_CPE_NO_FAULT"
			else
				let fault_code=$fault_code+$FAULT_CPE_DOWNLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				rm /tmp/icwmp_download 2> /dev/null
			fi
		fi
	fi
	if [ "$action" = "upload" ]; then
		local fault_code="9000"
		if [ "$__arg3" = "" -o "$__arg4" = "" ];then
			curl -T /etc/config/cwmp "$__arg1" 2> /dev/null
			if [ "$?" != "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		else
			curl -T /etc/config/cwmp -u $__arg3:$__arg4 "$__arg1" 2> /dev/null
			if [ "$?" != "0" ];then
				let fault_code=$fault_code+$FAULT_CPE_UPLOAD_FAILURE
				icwmp_fault_output "" "$fault_code"
				return 1
			fi
		fi		
	fi
	if [ "$action" = "apply_download" ]; then
		case "$__arg1" in
			1) icwmp_apply_firmware ;;
			2)
				if [ "$__arg2" != "0" ]; then 
					icwmp_apply_web_content $__arg2
				else
					icwmp_apply_web_content
				fi
			;;
			3) 
				if [ "$__arg2" != "0" ]; then 
					icwmp_apply_vendor_configuration $__arg2
				else
					icwmp_apply_vendor_configuration
				fi
			;;
		esac
	fi

	if [ "$action" = "apply_du_download" ]; then
		case "$__arg1" in
			install)	icwmp_install_package ;;
			update)		icwmp_update_package ;;
		esac
	fi
	if [ "$action" = "factory_reset" ]; then
		jffs2_mark_erase "rootfs_data"
		sync
		ACTION=add INTERFACE=resetbutton /sbin/hotplug-call button
		reboot
	fi

	if [ "$action" = "reboot" ]; then
		sync
		reboot
	fi

	if [ "$action" = "allow_cr_ip" ]; then
		local port=`$UCI_GET cwmp.cpe.port`
		local if_wan=`$UCI_GET cwmp.cpe.default_wan_interface`
		local zone=`$UCI_SHOW firewall | grep "firewall\.@zone\[[0-9]\+\]\.network=.*$if_wan" | head -1 | cut -f2 -d.`
		local zone_name=`$UCI_GET firewall.$zone.name`
		[ "$zone_name" = "" ] && return
		# update iptables rule
		sed -i "s,^.*Open ACS port.*,iptables -I zone_${zone_name}_input -p tcp -s $__arg1 --dport $port -j ACCEPT -m comment --comment=\"Open ACS port\",g" /etc/firewall.cwmp
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
					json_get_var __arg6 id
					action="download"
					;;
				du_download)
					json_get_var __arg1 url
					json_get_var __arg2 user
					json_get_var __arg3 pass
					action="du_download"
					;;
				upload)
					json_get_var __arg1 url
					json_get_var __arg2 type
					json_get_var __arg3 user
					json_get_var __arg4 pass
					action="upload"
					;;
				factory_reset)
					action="factory_reset"
					;;
				reboot)
					action="reboot"
					;;
				apply)
					if [ "$action" = "notification" ]; then
						action="apply_notification"
					elif [ "$action" = "value" ]; then
						json_get_var __arg1 arg
						action="apply_value"
					elif [ "$action" = "download" ]; then
						json_get_var __arg1 arg
						json_get_var __arg2 id
						action="apply_download"
					elif [ "$action" = "du_download" ]; then
						json_get_var __arg1 arg
						action="apply_du_download"
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
					action="delete_object"
					;;
				inform)
					action="inform"
					;;
				allow_cr_ip)
					action="allow_cr_ip"
					json_get_var __arg1 arg
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

