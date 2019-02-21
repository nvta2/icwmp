#!/bin/sh

# USAGE:
# ./generate_xml.sh


############################################ VARIABLES #########################################################
CURRENT_PATH=`pwd`
OUT_STREAM="tmp.txt"
ROOT_FILE="root.c"
TREE_TXT=$CURRENT_PATH"/"$OUT_STREAM
DM_PATH=${2:-"$(pwd)/../dm/dmtree/"}
SCRIPTS_PATH_COMMON=${DM_PATH}/"common/"
obj_look_obj_child_list=""
obj_look_param_child_list=""

############################################ FUNCTIONS #########################################################
set_obj_object_child() { 
	echo "${1}.${2}"
}

set_obj_object_line() {
	echo "object, ${1}, , , , root, ${2}"
}

set_obj_param_child() { 
	echo "${1}.${2}"
}

set_obj_param_line() {
	echo "parameter, ${1}, ${2}, ${3}, ${4}, root, ${5}"
}

get_param_type(){
	ptype=$1
	case "$ptype" in
		"DMT_STRING" )
			echo "string"
			;;
		"DMT_UNINT" )
			echo "unsignedInt"
			;;
		"DMT_TIME" )
			echo "dateTime"
			;;
		"DMT_BOOL" )
			echo "boolean"
			;;
		"DMT_LONG" )
			echo "long"
			;;
		"DMT_INT" )
			echo "int"
			;;
		"DMT_HEXBIN" )
			echo "hexbin"
			;;
	esac
	
}

get_leaf_obj_line_number(){
	if [ "$1" !=  "root.c" ]; then
		echo `grep -nE DMOBJ\|DMLEAF $1 | grep -v UPNP |cut -f1 -d: | tr "\n" " "`
	else
		if [ $DATA_MODEL == "tr098" ]; then
			echo `grep -nE DMOBJ\|DMLEAF $1 |grep "098" |grep -v UPNP | cut -f1 -d: | tr "\n" " "`
		else
			echo `grep -nE DMOBJ\|DMLEAF $1 |grep "181" |grep -v UPNP | cut -f1 -d: | tr "\n" " "`
		fi
	fi
}

add_item_to_list(){
	item="$1"
	list="$2"
	length=${#list}
	if [ $length == 0 ]; then
		list="$item"
	else
		list="$list $item"
	fi
	echo "$list"
}

remove_item_from_list(){
	item="$1"
	list="$2"
	new_list=""
	for i in $list; do
		if [ "$i" == "$item" ]; then
			continue
		fi
		new_list=`add_item_to_list "$i" "$new_list"`
	done
	echo "$new_list"
}

is_with_instance () {
	local obj=$1
	local inst=`echo $obj | rev | cut -d'.' -f 2 | rev`
	if [ "$inst" == "i" ]; then
		echo "1"
	else
		echo "0"
	fi
}

file_filter(){
	file=$1
	sort -k 7 $file > tmp2.txt
	cat tmp2.txt | tr -d "[:blank:]" > $file
	rm -rf tmp2.txt
	sed 's/,,,,/,/g' $file > tmp3.txt
	mv tmp3.txt $file
	sed 's/CUSTOM_PREFIX"/X_IOPSYS_EU_/g' $file > tmp3.txt
	mv tmp3.txt $file
	sed 's/"//' $file > tmp3.txt
	mv tmp3.txt $file
	sed 's/"././g' $file > tmp3.txt
	mv tmp3.txt $file
	local obl=""
	local objects=`grep "object" $file |wc -l`
	local object_lines=`grep "object" $file`
	for obl in $object_lines; do
		local objname=`echo "$obl" | cut -d, -f3`
		local inst=`is_with_instance $objname`
		if [ "$inst" == "1" ]; then
			sed -ri "/$prev_obj$/d" $file
			continue
		fi
		prev_obj=$obl
	done
	sed -ri '/^\s*$/d' $file
	sed -ri 's/\.i\./\.\{i\}\./g' $file
}

################# Tree.txt Generation ####################
gen_dm_tree(){
	file=$1
	#Get line number of lines containing Object or Param
	leaf_obj_line=`get_leaf_obj_line_number "$file"`

	for line_number in $leaf_obj_line; do
		#Get table name
		table_name=`sed -n $line_number'p' $file | cut -d' ' -f2 | tr -d []`
		str=`sed -n $line_number'p' $file | grep "DMOBJ"`
		parameters_list=""
		objects_list=""
		o_found="0"
		p_found="0"

		######## Before looking for childs Look to father
		for obj in $obj_look_obj_child_list; do
			multiinst_obj=`echo $obj | awk -F ":" '{print $2}'`
			childs_obj=`echo $obj | awk -F ":" '{print $3}'`
			if [ "$childs_obj" == "$table_name" ]; then  #I found mum
				if [ "$multiinst_obj" != "NULL" ]; then
					tmp=`echo $obj | awk -F ":" '{print $1}'`
					father_name="${tmp}.i"
				else
					father_name=`echo $obj | awk -F ":" '{print $1}'`
				fi
				o_found="1"
				break
			fi
		done
		for param in $obj_look_param_child_list; do
			multiinst_params=`echo $param | awk -F ":" '{print $2}'`
			childs_params=`echo $param | awk -F ":" '{print $3}'`
			if [ "$childs_params" == "$table_name" ]; then  #I found mum
				if [ "$multiinst_params" != "NULL" ]; then
					tmp=`echo $param | awk -F ":" '{print $1}'`
					father_name="${tmp}.i"
				else
					father_name=`echo $param | awk -F ":" '{print $1}'`
				fi
				p_found="1"
				break
			fi
		done

		######## Create Childs list
		while IFS=, read -r f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11; do
			name=`echo ${f1//{} | sed 's/^"\(.*\)"$/\1/'`
			permission=${f2// &}
			type=${f3// }
			browse=${f6// }

			if [ "$permission" == "DMWRITE" ]; then
				instance="readWrite"
			else
				instance="readOnly"
			fi

			if [ "$name" == "(char *)&dmroot"  ]; then
				name=${ROOT_PATH}
			fi

			if [ "$o_found" == "1" ]; then
				name=`set_obj_object_child "$father_name" "$name"`
				oname=`set_obj_object_line $instance "$name"`
				if [ "$browse" != "NULL" ]; then
					echo "$oname.{i}." >> $TREE_TXT
				else
					echo "$oname." >> $TREE_TXT
				fi
			fi

			if [ "$p_found" == "1" ]; then
				forced_inform=${f6// &}
				active_notify=`echo ${f7//&} | tr --delete }`
				[ "$forced_inform" == "DMFINFRM" ] && forced_inform="true" || forced_inform="false"
				[ "$active_notify" == "DMACTIVE" ] && active_notify="Active" || active_notify=""
				name=`set_obj_param_child "$father_name" "$name"`
				otype=`get_param_type $type`
				pname=`set_obj_param_line "$instance" "$otype" "$active_notify" "$forced_inform" "$name"`
				echo $pname >> $TREE_TXT
			fi

			if [ -n "$str" ]; then
				child_objects=${f9// }
				child_parameters=${f10// }
				obj_name=${name}
				#Add the actual object to the list of objects looking for their children objects ########
				if [ "$child_objects" != "NULL" ]; then
					new_item=${obj_name}":"${browse}":"${child_objects}
					obj_look_obj_child_list=`add_item_to_list "$new_item" "$obj_look_obj_child_list"`
				fi
				#Add the actual object to the list of objects looking for their children parameters #######
				if [ "$child_parameters" != "NULL" ]; then
					new_item=${obj_name}":"${browse}":"${child_parameters}
					obj_look_param_child_list=`add_item_to_list "$new_item" "$obj_look_param_child_list"`
				fi
			fi

		done <<<"`sed -n $line_number',/{0}/p' $file | sed -e '/{0}/d' | sed -e '/^{/!d'`"
		
		######### Remove object from list of object looking there childs
		for obj in $obj_look_obj_child_list; do
			childs_obj=`echo $obj | awk -F ":" '{print $3}'`
			if [ "$childs_obj" == "$table_name" ]; then  #I found mum
				obj_look_obj_child_list=`remove_item_from_list "$obj" "$obj_look_obj_child_list"`
				break
			fi
		done
		
		######### Remove object from list of object looking there childs
		for param in $obj_look_param_child_list; do
			childs_params=`echo $param | awk -F ":" '{print $3}'`
			if [ "$childs_params" == "$table_name" ]; then  #I found mum
				obj_look_param_child_list=`remove_item_from_list "$param" "$obj_look_param_child_list"`
				break
			fi
		done
	done
}

################################# XML Generation Functions ######################################"
xml_open_tag_object() {
	local level="$1"
	local objn="$2"
	local permission="$3"
	local sp1=0 sp2=0
	let sp1=4+4*$level
	let sp2=$sp1+4
	printf "%${sp1}s"; echo "<object name=\"$objn\" access=\"$permission\" minEntries=\"0\" maxEntries=\"20\">"
}

xml_close_tag_object() {
	local level="$1"
	local sp1=0 sp2=0
	let sp1=4+4*$level
	let sp2=$sp1+4
	printf "%${sp1}s"; echo "</object>"
}

xml_add_parameter() {
	local level="$1"
	local paramn="$2"
	local type="$3"
	local access="$4"
	local fnf="$5"
	local fif="$6"
	local sp1=0 sp2=0
	let sp1=4+4*$level
	let sp2=$sp1+4
	let sp3=$sp2+4
	[ "$fnf" == "Active" ] && activenotif="activeNotify=\"forceEnabled\"" || activenotif=""
	[ "$fif" == "true"  ] && forcedinform="forcedInform=\"true\"" || forcedinform=""

	if [[ -z "$activenotif" && -z "$forcedinform" ]]; then
		printf "%${sp1}s"; echo "<parameter name=\"$paramn\" access=\"$access\">"
	elif [[ -z "$activenotif" && -n "$forcedinform" ]]; then
		printf "%${sp1}s"; echo "<parameter name=\"$paramn\" access=\"$access\" $forcedinform>"
	elif [[ -n "$activenotif" && -z "$forcedinform" ]]; then
		printf "%${sp1}s"; echo "<parameter name=\"$paramn\" access=\"$access\" $activenotif>"
	else
		printf "%${sp1}s"; echo "<parameter name=\"$paramn\" access=\"$access\" $activenotif $forcedinform>"
	fi
	printf "%${sp2}s"; echo "<description>parameter $paramn</description>"
	printf "%${sp2}s"; echo "<syntax>"
	printf "%${sp3}s"; echo "<$type></$type>"
	printf "%${sp2}s"; echo "</syntax>"
	printf "%${sp1}s"; echo "</parameter>"
}

add_dm_xml() {
	file=$1
	local line=""
	object_lines=`grep "object" $file`
	for line in $object_lines; do
		let cnt_obj++
		local objname=`echo "$line" | cut -d, -f4`
		local permission=`echo "$line" |cut -d, -f2`
		xml_open_tag_object "1" "$objname" "$permission" 
		local param_list=`grep "parameter.*,$objname[a-zA-Z0-9_]\+$" $file`
		for pl in $param_list; do
			local type=`echo "$pl" |cut -d, -f3`
			local param=`echo "$pl" |rev |cut -d. -f1 |rev`
			local permission=`echo "$pl" |cut -d, -f2`
			local fnotif=`echo "$pl" |cut -d, -f4`
			local finform=`echo "$pl" |cut -d, -f5`
			let cnt_param++
			xml_add_parameter "2" "$param" "$type" "$permission" "$fnotif" "$finform"
		done
		xml_close_tag_object "1"
	done
}

gen_data_model_xml_file() {
	echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
	echo "<dm:document xmlns:dm=\"urn:broadband-forum-org:cwmp:datamodel-1-6\" xmlns:dmr=\"urn:broadband-forum-org:cwmp:datamodel-report-0-1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:broadband-forum-org:cwmp:datamodel-1-6 http://www.broadband-forum.org/cwmp/cwmp-datamodel-1-6.xsd urn:broadband-forum-org:cwmp:datamodel-report-0-1 http://www.broadband-forum.org/cwmp/cwmp-datamodel-report.xsd\" spec=\"urn:broadband-forum-org:$DM_VERSION\" file=\"$DM_FILE\">"
	echo "	<model name=\"$model_name\">"
	echo "		<object name=\"$ROOT_PATH\" access=\"readOnly\" minEntries=\"1\" maxEntries=\"1\">"
	echo "			<description>"
	echo "				The top-level for $DATA_MODEL"
	echo "			</description>"
	echo "		</object>"
	add_dm_xml $OUT_STREAM
	echo "	</model>"
	echo "</dm:document>"
}

############################################### MAIN ######################################################
cnt_obj=1
cnt_param=0
DATA_MODEL="tr098"
SCRIPTS_PATH=${DM_PATH}/${DATA_MODEL}
DIR_LIST="$SCRIPTS_PATH_COMMON $SCRIPTS_PATH"
ROOT_PATH="InternetGatewayDevice"
DM_HEAD="$ROOT_PATH-1.8"
DM_FILE="tr-098-1-8-0-full.xml"
DM_VERSION="tr-098-1-8-0-cwmp"
model_name="$ROOT_PATH:1.4"
XML_OUT_STREAM_BBF_098="iopsys_bbf_tr098.xml"
############## GEN TR098 TREE ##############
echo "Start Generation of TR098..."
echo "Please wait..."
rm -rf $OUT_STREAM
rm -rf $XML_OUT_STREAM_BBF_098
cd "$SCRIPTS_PATH_COMMON"
gen_dm_tree $ROOT_FILE
for dir in $DIR_LIST; do
	cd $dir
	files=`ls *.c |grep -v $ROOT_FILE`
	for file in $files; do
		gen_dm_tree "$file"
	done
done
cd $CURRENT_PATH
file_filter $OUT_STREAM
gen_data_model_xml_file > $XML_OUT_STREAM_BBF_098

echo "Number of TR098 objects is $cnt_obj"
echo "Number of TR098 parameters is $cnt_param"
echo "End of TR098 Generation"
echo "=============================="
##################################################################################################
cnt_obj=1
cnt_param=0
DATA_MODEL="tr181"
SCRIPTS_PATH=${DM_PATH}/${DATA_MODEL}
DIR_LIST="$SCRIPTS_PATH_COMMON $SCRIPTS_PATH"
ROOT_PATH="Device"
DM_HEAD="$ROOT_PATH-2.12"
DM_FILE="tr-181-2-12-0-cwmp-full.xml"
DM_VERSION="tr-181-2-12-0-cwmp"
model_name="$ROOT_PATH:2.12"
XML_OUT_STREAM_BBF_181="iopsys_bbf_tr181.xml"
############## GEN TR181 TREE ##############
echo "Start Generation of TR181..."
echo "Please wait..."
rm -rf $OUT_STREAM
rm -rf $XML_OUT_STREAM_BBF_181
cd "$SCRIPTS_PATH_COMMON"
gen_dm_tree $ROOT_FILE
for dir in $DIR_LIST; do
	cd $dir
	files=`ls *.c |grep -v $ROOT_FILE`
	for file in $files; do
		gen_dm_tree "$file"
	done
done
cd $CURRENT_PATH
file_filter $OUT_STREAM
gen_data_model_xml_file > $XML_OUT_STREAM_BBF_181

echo "Number of TR181 objects is $cnt_obj"
echo "Number of TR181 parameters is $cnt_param"
echo "End of TR181 Generation"
rm -rf $OUT_STREAM
