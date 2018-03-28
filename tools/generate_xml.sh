#!/bin/sh


# USAGE:
# ./generate_xml.sh <data model> <scripts path> <product class> <device protocol> <model name> <software version>
# If the input arguments are empty, then use the default values:
	

# VARIABLES ####################################################################################################
obj_look_obj_child_list=""
obj_look_param_child_list=""
obj_look_father_list=""
param_look_father_list=""


# FUNCTIONS ####################################################################################################
set_node_name() { 
	echo ${1}
}

set_obj_object_child() { 
	echo "${1},${2}"
}

set_obj_object_line() {
	echo "object, ${1}, root, ${2}"
}

set_obj_param_child() { 
	echo "${1},${2}"
}
set_obj_param_line() { 
	echo "parameter, ${1}, root, ${2}"
}

set_obj_instance_line(){
	echo "instance, , root, ${1}"
}

set_objs_child_instance_name(){
	echo "${1}.${2}"
}

set_prms_child_instance_name(){
	echo "${1}.${2}"
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

#Tree.txt Generation ####################################
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
			childs_obj=`echo $obj | awk -F ":" '{print $2}'`
			if [ "$childs_obj" == "$table_name" ]; then  #I found mum
				father_name=`echo $obj | awk -F ":" '{print $1}'`
				o_found="1"
				break
			fi
		done
		for param in $obj_look_param_child_list; do
			childs_params=`echo $param | awk -F ":" '{print $2}'`
			if [ "$childs_params" == "$table_name" ]; then  #I found mum
				father_name=`echo $param | awk -F ":" '{print $1}'`
				p_found="1"
				break
			fi
		done
	
		######## Create Childs list
		while IFS=, read -r f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11; do
			name=`echo ${f1//{} | sed 's/^"\(.*\)"$/\1/'`
			permission=${f2// &}
			type=${f3// }
			multiinstance=${f6// }
			if [ "$multiinstance" != "NULL" ]; then
				instance="true"
			else
				instance="false"
			fi
			if [ "$name" == "(char *)&dmroot"  ]; then
				name=${ROOT_PATH}
			fi
			if [ "$o_found" == "1" ]; then
				name=`set_obj_object_child "$father_name" "$name"`
				oname=`set_obj_object_line $instance "$name"`
				echo "$oname," >> $TREE_TXT
#				if [ "$instance" == "true" ]; then
#					objinst=`set_obj_instance_line $name`
#					echo $objinst >> $TREE_TXT
#				fi
				#tree=`add_item_to_list "$oname" "$tree"`
			fi
			if [ "$p_found" == "1" ]; then
				name=`set_obj_param_child "$father_name" "$name"`
				otype=`get_param_type $type`
				pname=`set_obj_param_line "$otype" "$name"`
				#tree=`add_item_to_list "$pname" "$tree"`
				echo $pname >> $TREE_TXT
			fi
			if [ -n "$str" ]; then
				child_objects=${f9// }
				child_parameters=${f10// }
				obj_name=${name}
				#Add the actual object to the list of objects looking for their children objects ########
				if [ "$child_objects" != "NULL" ]; then
					new_item=${obj_name}":"${child_objects}
					obj_look_obj_child_list=`add_item_to_list "$new_item" "$obj_look_obj_child_list"`
				fi
				#Add the actual object to the list of objects looking for their children parameters #######
				if [ "$child_parameters" != "NULL" ]; then
					new_item=${obj_name}":"${child_parameters}
					obj_look_param_child_list=`add_item_to_list "$new_item" "$obj_look_param_child_list"`
				fi
			fi
		done <<<"`sed -n $line_number',/{0}/p' $file | sed -e '/{0}/d' | sed -e '/^{/!d'`"
		
		######### Remove object from list of object looking there childs
		for obj in $obj_look_obj_child_list; do
			childs_obj=`echo $obj | awk -F ":" '{print $2}'`
			if [ "$childs_obj" == "$table_name" ]; then  #I found mum
				obj_look_obj_child_list=`remove_item_from_list "$obj" "$obj_look_obj_child_list"`
				break
			fi
		done
		
		######### Remove object from list of object looking there childs
		for param in $obj_look_param_child_list; do
			childs_params=`echo $param | awk -F ":" '{print $2}'`
			if [ "$childs_params" == "$table_name" ]; then  #I found mum
				obj_look_param_child_list=`remove_item_from_list "$param" "$obj_look_param_child_list"`
				break
			fi
		done
	done
}
#XML Generation Functions ####################################
xml_open_tag_object() {
	local objn="$1"
	local isarray="$2"
	local level="$3"
	local h_child="$4"
	local sp1=0 sp2=0
	let sp1=8+4*$level
	let sp2=$sp1+4
	printf "%${sp1}s"; echo "<parameter>"
	printf "%${sp2}s"; echo "<parameterName>$objn</parameterName>"
	printf "%${sp2}s"; echo "<parameterType>object</parameterType>"
	printf "%${sp2}s"; echo "<array>$isarray</array>"
	if [ -n "$h_child" -a "$h_child" != "0" ]; then
		printf "%${sp2}s"; echo "<parameters>"
	fi
}

xml_close_tag_object() {
	local level="$1"
	local h_child="$2"
	local sp1=0 sp2=0
	let sp1=8+4*$level
	let sp2=$sp1+4
	if [ -n "$h_child" -a "$h_child" != "0" ]; then
		printf "%${sp2}s"; echo "</parameters>"
	fi
	printf "%${sp1}s"; echo "</parameter>"
}

xml_add_parameter() {
	local paramn="$1"
	local type="$2"
	local level="$3"
	local sp1=0 sp2=0
	let sp1=8+4*$level
	let sp2=$sp1+4

	printf "%${sp1}s"; echo "<parameter>"
	printf "%${sp2}s"; echo "<parameterName>$paramn</parameterName>"
	printf "%${sp2}s"; echo "<parameterType>$type</parameterType>"
	printf "%${sp1}s"; echo "</parameter>"
}


xml_write_line() {
	local level="$1"
	local parent="$2"
	local path="$3"
	local line=""
	
	local LINES=`grep "$path[^,]\+$\|$path[^,]\+,$" tmp.txt`

	for line in $LINES; do
		local p=`echo "$line" | cut -d, -f$((level+2))`
		[ "$p" != "$parent" ] && continue
		local param=`echo "$line" | cut -d, -f$((level+3))`
		[ "$param" = "" ] && continue
		local node=`echo "$line" | cut -d, -f1`
		if [ "$node" = "object" ]; then
			local isarray=`echo "$line" | cut -d, -f2`
			let cnt_obj++
			local has_child=`grep "$path$param,[a-zA-Z0-9_,]\+$" tmp.txt |wc -l`;
			xml_open_tag_object "$param" "$isarray" "$level" "$has_child"
			xml_write_line "$((level+1))" "$param" "$path$param,"
			xml_close_tag_object "$level" "$has_child"
		elif [ "$node" = "parameter" ]; then
			local type=`echo "$line" | cut -d, -f2`
			let cnt_param++
			xml_add_parameter "$param" "$type" "$level"
		fi
	done
}

gen_data_model_xml_file() {
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
echo "<deviceType xmlns=\"urn:dslforum-org:hdm-0-0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"urn:dslforum-org:hdm-0-0 deviceType.xsd\">"
echo "    <protocol>$DEVICE_PROTOCOL</protocol>"
echo "    <manufacturer>Inteno</manufacturer>"
echo "    <manufacturerOUI>002207</manufacturerOUI>"
echo "    <productClass>$PRODUCT_CLASS</productClass>"
echo "    <modelName>$MODEL_NAME</modelName>" 
echo "    <softwareVersion>$SOFTWARE_VERSION</softwareVersion>"
echo "    <dataModel>"
echo "        <attributes>"
echo "            <attribute>"
echo "                <attributeName>notification</attributeName>"
echo "                <attributeType>int</attributeType>"
echo "                <minValue>0</minValue>"
echo "                <maxValue>2</maxValue>"
echo "            </attribute>"
echo "            <attribute>"
echo "                <attributeName>accessList</attributeName>"
echo "                <attributeType>string</attributeType>"
echo "                <array>true</array>"
echo "                <attributeLength>64</attributeLength>"
echo "            </attribute>"
echo "            <attribute>"
echo "                <attributeName>visibility</attributeName>"
echo "                <attributeType>string</attributeType>"
echo "                <array>true</array>"
echo "                <attributeLength>64</attributeLength>"
echo "            </attribute>"
echo "        </attributes>"
echo "        <parameters>"
xml_write_line "1" "root" "root,"
echo "        </parameters>"
echo "    </dataModel>"
echo "</deviceType>"
}




# MAIN ####################################################################################################
# CONSTANTS ######
CURRENT_PATH=`pwd`
OUT_STREAM="tmp.txt"
ROOT_FILE="root.c"
TREE_TXT=$CURRENT_PATH"/"$OUT_STREAM

DM_98="tr098"
DM_181="tr181"
DM_PATH=${2:-"$(pwd)/../dm/dmtree/"}
PRODUCT_CLASS=${3:-"DG301-W7P2U"}
DEVICE_PROTOCOL=${4:-"DEVICE_PROTOCOL_DSLFTR069v1"}
MODEL_NAME=${5:-"DG301-W7P2U"}
SOFTWARE_VERSION=${6:-"1.2.3.4B"}

SCRIPTS_PATH_COMMON=${DM_PATH}/"common/"
cnt_obj=0
cnt_param=0
### GEN TR098 TREE ####
DATA_MODEL=$DM_98
SCRIPTS_PATH=${DM_PATH}/${DATA_MODEL}
DIR_LIST="$SCRIPTS_PATH_COMMON $SCRIPTS_PATH"
XML_OUT_STREAM_098="inteno_tr098.xml"
ROOT_PATH="InternetGatewayDevice"
####

echo "Start Generation of TR098..."
echo "Please wait..."
rm -rf $OUT_STREAM
rm -rf $XML_OUT_STREAM_098
echo "object,false,root,$ROOT_PATH," > $OUT_STREAM
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

sort -k 4 $OUT_STREAM > tmp2.txt
cat tmp2.txt | tr -d "[:blank:]" > $OUT_STREAM

gen_data_model_xml_file > $XML_OUT_STREAM_098
cnt_obj=`grep -c "object," tmp.txt`
cnt_param=`grep -c "parameter," tmp.txt`
echo "Number of TR098 objects is $cnt_obj"
echo "Number of TR098 parameters is $cnt_param"
echo "End Of TR098 Generation"

### GEN TR181 TREE ##################################
cnt_obj=0
cnt_param=0
DATA_MODEL=$DM_181
SCRIPTS_PATH=${DM_PATH}/${DATA_MODEL}
DIR_LIST="$SCRIPTS_PATH_COMMON $SCRIPTS_PATH"
XML_OUT_STREAM_181="inteno_tr181.xml"
ROOT_PATH="Device"
########

echo "Start Generation of TR0181..."
echo "Please wait..."
rm -rf $OUT_STREAM
rm -rf $XML_OUT_STREAM_181
echo "object,false,root,$ROOT_PATH," > $OUT_STREAM
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

sort -k 4 $OUT_STREAM > tmp2.txt
cat tmp2.txt | tr -d "[:blank:]" > $OUT_STREAM
rm -rf tmp2.txt
gen_data_model_xml_file > $XML_OUT_STREAM_181
cnt_obj=`grep -c "object," tmp.txt`
cnt_param=`grep -c "parameter," tmp.txt`
echo "Number of TR181 objects is $cnt_obj"
echo "Number of TR181 parameters is $cnt_param"
echo "End Of TR181 Generation"
rm -rf tmp_.txt
