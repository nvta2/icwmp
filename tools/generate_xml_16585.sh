#!/bin/sh


# USAGE:
# ./generate_xml.sh <data model> <scripts path> <product class> <device protocol> <model name> <software version>
# If the input arguments are empty, then use the default values:
DATA_MODEL=${1:-"tr098"}
SCRIPTS_PATH=${2:-"$(pwd)/../dm/dmtree/"}
cnt_obj=0
cnt_param=0
SCRIPTS_PATH_COMMON=${SCRIPTS_PATH}/"common/"
SCRIPTS_PATH=${SCRIPTS_PATH}/${DATA_MODEL}
if [ "$DATA_MODEL" == "tr098" ]; then
	ROOT_PATH="InternetGatewayDevice."
	DM_HEAD="InternetGatewayDevice-1.8"
	DM_FILE="tr-098-1-8-0-full.xml"
	DM_VERSION="tr-098-1-8-0"
	version="1.0"
else
	ROOT_PATH="Device."
	DM_HEAD="InternetGatewayDevice-2.12"
	DM_FILE="tr-181-2-12-0-cwmp-full.xml"
	DM_VERSION="tr-181-2-12-0"
	version="2.0"
fi
# Funtcions
# Check if object of parameters or get none in case of object contains an intance
is_object() {
	str=$1
	function=`echo $str |cut -d, -f1`
	if [ "$function" == "DMOBJECT" ]; then	
		check=`echo $str |grep "\.[012345689]*\.,\$\|\.#\w*\.,\$"`
		if [ "$check" == "" ]; then
			echo "object"
		else
			echo "instance"
		fi
	else
		echo "parameter"
	fi
}

#	Check if object contains list of instances
is_array() {
	object=$1
	check=`cat script_list.txt |grep "$object[012345689]*\.,\$\|$object#\w*\.,\$"`
	if [ "$check" == "" ]; then
		echo "false"
	else
		echo "true"
	fi
}

is_read_write(){
	str=$1
	rw=`echo $str |cut -d, -f3`
	if [ "$rw" == "0" ]; then
		echo "readOnly"
	else
		echo "readWrite"
	fi
}

get_param_type() {
	str=$1
	type=`echo $str |cut -d, -f4`
	type=${type#*xsd:}
	type=${type:-string}
	[ $type = 0 ] && type=""
	type=${type:-string}
	echo $type
}

is_forced_notif() {
	str=$1
	fn=`echo $str |cut -d, -f7`
	if [ "$fn" == "0" ]; then
		echo "canDeny"
	else
		if [ "$fn" == "2" ]; then
			echo "Active"
		else
			if [ "$fn" == "1" ]; then
				echo "Passive"
			else
				echo ""
			fi
		fi
	fi
}

xml_open_tag_object() {
	local level="$1"
	local objn="$2"
	local permission="$3"
	local sp1=0 sp2=0
	let sp1=8+4*$level
	let sp2=$sp1+4
	printf "%${sp1}s"; echo "<object name=\"$objn\" access=\"$permission\" minEntries=\"\" maxEntries=\"\" dmr:version=\"$version\">"
}

xml_close_tag_object() {
	local level="$1"
	local sp1=0 sp2=0
	let sp1=8+4*$level
	let sp2=$sp1+4
	printf "%${sp1}s"; echo "</object>"
}

xml_add_parameter() {
	local level="$1"
	local paramn="$2"
	local type="$3"
	local access="$4"
	local fnf="$5"
	local sp1=0 sp2=0
	let sp1=8+4*$level
	let sp2=$sp1+4
	let sp3=$sp2+4
	if [ "$fnf" == "canDeny"  ]; then
		printf "%${sp1}s"; echo "<parameter name=\"$paramn\" access=\"$access\" activeNotify=\"canDeny\" dmr:version=\"$version\">"
	else
		printf "%${sp1}s"; echo "<parameter name=\"$paramn\" access=\"$access\" dmr:version=\"$version\">"
	fi		
	printf "%${sp2}s"; echo "<description>parameter $paramn</description>"
	printf "%${sp2}s"; echo "<syntaxe>"
	printf "%${sp3}s"; echo "<$type></$type>"
	printf "%${sp2}s"; echo "</syntaxe>"
	printf "%${sp1}s"; echo "</parameter>"
}


add_dm_xml() {
	local line=""
	local objects=`grep "object" tmp.txt |wc -l`
	object_lines=`grep "object" tmp.txt`
	for line in $object_lines; do
		let cnt_obj++
		local objname=`echo "$line" | cut -d, -f3`
		local permission=`echo "$line" |cut -d, -f2`
		xml_open_tag_object "1" "$objname" "$permission" 
		local param_list=`grep "parameter.*,$objname[a-zA-Z0-9_]\+$" tmp.txt`
		for pl in $param_list; do
			local type=`echo "$pl" |cut -d, -f3`
			local param=`echo "$pl" |rev |cut -d. -f1 |rev`
			local permission=`echo "$pl" |cut -d, -f2`
			local fnotif=`echo "$pl" |cut -d, -f4`
			let cnt_param++
			xml_add_parameter "2" "$param" "$type" "$permission" "fnotif"
		done
		xml_close_tag_object "1"
	done
}

gen_data_model_xml_file() {
	echo "<?xml version=\"$version\" encoding=\"UTF-8\"?>"
	echo "<dm:document xmlns:dm=\"urn:broadband-forum-org:cwmp:datamodel-1-6\" xmlns:dmr=\"urn:broadband-forum-org:cwmp:datamodel-report-0-1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:broadband-forum-org:cwmp:datamodel-1-6 http://www.broadband-forum.org/cwmp/cwmp-datamodel-1-6.xsd urn:broadband-forum-org:cwmp:datamodel-report-0-1 http://www.broadband-forum.org/cwmp/cwmp-datamodel-report.xsd\" spec=\"urn:broadband-forum-org:$DM_VERSION\" file=\"$DM_FILE\">"
	echo "	<description>$DEVICE_PROTOCOL</description>"
	echo "	<dataType></dataType>"
	echo "	<model name=\"$DEVICE_PROTOCOL\">"
	echo "		<object name=\"$ROOT_PATH\" access=\"readOnly\" minEntries=\"1\" maxEntries=\"1\" dmr:version=\"$version\">"
	echo "			<description>"
	echo "				The top-level object for an Internet Gateway Device."
	echo "			</description>"
	echo "		</object>"
	add_dm_xml
	echo "	</model>"
	echo "</dm:document>"
}

echo "Start Generation of inteno.xml"

EXEC_PATH=`pwd`
SCRIPTS_LIST_FILE="$EXEC_PATH/script_list.txt"
echo "" > $SCRIPTS_LIST_FILE
# Extract object and parameter list from scripts
list=`ls $SCRIPTS_PATH |grep -v "common"`
cd $SCRIPTS_PATH

cat `echo $list` | grep "DMOBJECT\|DMPARAM" |grep -v "^\\s*#" |sed 's/^[ \t]*//' |awk -F '[(,)]' '{type=""; { if($1=="DMOBJECT") { type = "object"; obj_name = $2; print $1 " " $2 " " $4 " " $5;} else {if($7 ~ /xsd:/) {print $1 " " obj_name$2 " " $4 " " $7 " " $8 " " $9 " " $10;} else { print $1 " " obj_name$2 " " $4 " " str " " $8 " " $9 " " $10;}} } }' | awk '{print $1","$2","$3","$4","$5","$6","$7}' | sort -t, -k2 | sed -e "s|.\%|.#|g" | sed -e "s|\"||g" | sed -e "s|DMROOT|$ROOT_PATH|g" > $SCRIPTS_LIST_FILE

list=`ls $SCRIPTS_PATH_COMMON |grep -v "common"`
cd $SCRIPTS_PATH_COMMON

cat `echo $list` | grep "DMOBJECT\|DMPARAM" |grep -v "^\\s*#" |sed 's/^[ \t]*//' |awk -F '[(,)]' '{type=""; { if($1=="DMOBJECT") { type = "object"; obj_name = $2; print $1 " " $2 " " $4 " " $5;} else {if($7 ~ /xsd:/) {print $1 " " obj_name$2 " " $4 " " $7 " " $8 " " $9 " " $10;} else {print $1 " " obj_name$2 " " $4 " " $8 " " $9 " " $10;}} } }' | awk '{print $1","$2","$3","$4","$5","$6","$7}' | sort -t, -k2 | sed -e "s|.\%|.#|g" | sed -e "s|\"||g" | sed -e "s|DMROOT|$ROOT_PATH|g" >> $SCRIPTS_LIST_FILE
cd $EXEC_PATH
rm -rf tmp.txt
while read line
do
	test=`is_object "$line"`
	name=`echo "$line" |cut -d, -f2`
	case "$test" in
		"object" )
			permission=`is_read_write "$line"`
			str="object,$permission,$name"
			;;
		"instance" )
			str="instance,,$name"
			;;
		"parameter" )
			permission=`is_read_write "$line"`
			fnotif=`is_forced_notif "$line"`
			type=`get_param_type "$line"`
			str="parameter,$permission, $type,$fnotif,$name"
			;;
	esac
	echo "$str" >> tmp.txt
done <$SCRIPTS_LIST_FILE
#Remove instances from lines
cont=1
while [ "$cont" != "" ]; do
	sed -ri 's/\.[0-9]+//' tmp.txt
	cont=`grep "\.[0-9]+" tmp.txt`
done
cont=1
while [ "$cont" != "" ]; do
	sed -ri 's/\.(#)[^.]+//' tmp.txt
	cont=`grep "\.#" tmp.txt`
done

#remove spaces from each line
cont=1
while [ "$cont" != "" ]; do
	sed -ri 's/ //' tmp.txt
	cont=`grep "\.[0-9]+" tmp.txt`
done

#Remove duplicated lines
awk '!a[$0]++' tmp.txt > tmp2.txt
mv tmp2.txt tmp.txt

gen_data_model_xml_file > inteno_$DATA_MODEL.xml

rm -rf tmp.txt
rm -rf $SCRIPTS_LIST_FILE

echo "Number of objects is $cnt_obj"
echo "Number of parameters is $cnt_param"

echo "End Of Generation"
