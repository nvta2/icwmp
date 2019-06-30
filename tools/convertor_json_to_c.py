#!/usr/bin/python

#      This program is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 2 of the License, or
#      (at your option) any later version.
#
#      Copyright (C) 2019 iopsys Software Solutions AB
#		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>

import os
import sys
import time
import json
from collections import OrderedDict

arrtype = {
"string": "DMT_STRING",
"unsignedInt": "DMT_UNINT",
"unsignedLong": "DMT_UNLONG",
"int": "DMT_INT",
"long": "DMT_LONG",
"boolean": "DMT_BOOL",
"dateTime": "DMT_TIME",
"hexBinary": "DMT_HEXBIN",
"base64": "DMT_BASE64",
}

def removefile( filename ):
	try:
		os.remove(filename)
	except OSError:
		pass

def securemkdir( folder ):
	try:
		os.mkdir(folder)
	except:
		pass

def getlastname( name ):
	lastname = name
	lastname = lastname.replace(".{i}", "")
	namelist = lastname.split('.')
	lastname = namelist[-1]
	if lastname == "":
		lastname = namelist[-2]
	return lastname;

def getname( objname ):
	global model_root_name
	OBJSname = objname
	if (objname.count('.') > 1 and (objname.count('.') != 2 or objname.count('{i}') != 1) ):
		OBJSname = objname.replace(dmroot, "", 1)
	OBJSname = OBJSname.replace("{i}", "")
	OBJSname = OBJSname.replace(".", "")
	if (objname.count('.') == 1):
		model_root_name = OBJSname
		OBJSname = "Root" + OBJSname
		return OBJSname
	if (objname.count('.') == 2 and objname.count('{i}') == 1):
		model_root_name = OBJSname
		OBJSname = "Services" + OBJSname
		return OBJSname
	return OBJSname;

def getoptionparam( value, option ):
	val = "false"
	if isinstance(value,dict):
		for k,v in value.items():
			if k == option:
				return v
	return val

def getargsparam( value ):
	val1 = "false"
	val2 = "false"
	if isinstance(value,dict):
		for k,v in value.items():
			return k, v
	return val1, val2

def getparamtype( value ):
	ptype = None
	paramtype = getoptionparam(value, "type")
	ptype = arrtype.get(paramtype, None)
	if ptype == None:
		ptype = "__NA__"
	return ptype

def objhaschild( value ):
	if isinstance(value,dict):
		for k,v in value.items():
			if isinstance(v,dict):
				for k1,v1 in v.items():
					if k1 == "type" and v1 == "object":
						return 1
	return 0

def objhasparam( value ):
	if isinstance(value,dict):
		for k,v in value.items():
			if isinstance(v,dict):
				for k1,v1 in v.items():
					if k1 == "type" and v1 != "object":
						return 1
	return 0

def get_mapping_param( mappingobj ):
	type = getoptionparam(mappingobj, "type")
	if type == "uci":
		uciobj = getoptionparam(mappingobj, "uci")
		file = getoptionparam(uciobj, "file")
		sectionobj = getoptionparam(uciobj, "section")
		sectiontype = getoptionparam(sectionobj, "type")
		sectionname = getoptionparam(sectionobj, "name")
		sectionindex = getoptionparam(sectionobj, "index")
		optionobj = getoptionparam(uciobj, "option")
		optionname = getoptionparam(optionobj, "name")
		return type, file, sectiontype, sectionname, sectionindex, optionname
	elif type == "ubus":
		ubusobj = getoptionparam(mappingobj, "ubus")
		object = getoptionparam(ubusobj, "object")
		method = getoptionparam(ubusobj, "method")
		argsobj = getoptionparam(ubusobj, "args")
		arg1, arg2 = getargsparam(argsobj)
		key = getoptionparam(ubusobj, "key")
		return type, object, method, arg1, arg2, key
	else:
		cliobj = getoptionparam(mappingobj, "cli")
		command = getoptionparam(cliobj, "command")
		argsobj = getoptionparam(cliobj, "args")
		i = 0
		value = ""
		list_length = len(argsobj)
		while i < list_length:
			if value == "":
				value = "\"" + argsobj[i] + "\", "
			elif i == list_length-1:
				value = value + "\"" + argsobj[i] + "\""
			else:
				value = value + "\"" + argsobj[i] + "\", "
			i += 1
		return type, command, list_length, value, "", ""

def get_mapping_obj( mappingobj ):
	type = getoptionparam(mappingobj, "type")
	uciobj = getoptionparam(mappingobj, "uci")
	file = getoptionparam(uciobj, "file")
	sectionobj = getoptionparam(uciobj, "section")
	sectiontype = getoptionparam(sectionobj, "type")
	dmmapfile = getoptionparam(uciobj, "dmmapfile")
	return type, file, sectiontype, dmmapfile

def printheaderObjCommon( objname ):
	fp = open('./.objparamarray.c', 'a')
	print >> fp, "/* *** %s *** */" % objname
	fp.close()

def cprintheaderOBJS( objname ):
	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "DMOBJ %s[] = {" % ("t" + getname(objname) + "Obj")
	print >> fp,  "/* OBJ, permission, addobj, delobj, checkobj, browseinstobj, forced_inform, notification, nextobj, leaf, linker*/"
	fp.close()

def hprintheaderOBJS( objname ):
	fp = open('./.objparamarray.h', 'a')
	print >> fp,  "extern DMOBJ %s[];" % ("t" + getname(objname) + "Obj")
	fp.close()

def cprinttopfile (fp, filename):
	print >> fp, "/*"
	print >> fp, "*      This program is free software: you can redistribute it and/or modify"
	print >> fp, "*      it under the terms of the GNU General Public License as published by"
	print >> fp, "*      the Free Software Foundation, either version 2 of the License, or"
	print >> fp, "*      (at your option) any later version."
	print >> fp, "*"
	print >> fp, "*      Copyright (C) 2019 iopsys Software Solutions AB"
	print >> fp, "*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>"
	print >> fp, "*/"
	print >> fp, ""
	print >> fp, "#include \"dmcwmp.h\""
	print >> fp, "#include \"dmcommon.h\""
	print >> fp, "#include \"dmuci.h\""
	print >> fp, "#include \"dmubus.h\""
	print >> fp, "#include \"dmjson.h\""
	print >> fp, "#include \"dmentry.h\""
	print >> fp, "#include \"%s.h\"" % filename.lower()
	print >> fp, ""

def hprinttopfile (fp, filename):
	print >> fp, "/*"
	print >> fp, "*      This program is free software: you can redistribute it and/or modify"
	print >> fp, "*      it under the terms of the GNU General Public License as published by"
	print >> fp, "*      the Free Software Foundation, either version 2 of the License, or"
	print >> fp, "*      (at your option) any later version."
	print >> fp, "*"
	print >> fp, "*      Copyright (C) 2019 iopsys Software Solutions AB"
	print >> fp, "*		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>"
	print >> fp, "*/"
	print >> fp, ""
	print >> fp, "#ifndef __%s_H" % filename.upper()
	print >> fp, "#define __%s_H" % filename.upper()
	print >> fp, ""

def hprintfootfile (fp, filename):
	print >> fp, ""
	print >> fp, "#endif //__%s_H" % filename.upper()
	print >> fp, ""

def cprintAddDelObj( faddobj, fdelobj, name, mappingobj, dmobject ):
	fp = open('./.objadddel.c', 'a')
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char **instance)" % faddobj
	print >> fp, "{"
	if mappingobj != "false":
		type, file, sectiontype, dmmapfile = get_mapping_obj(mappingobj)
		if type == "uci":
			print >> fp, "	char *inst, *value, *v;"
			print >> fp, "	struct uci_section *dmmap = NULL, *s = NULL;"
			print >> fp, ""
			print >> fp, "	check_create_dmmap_package(\"%s\");" % dmmapfile
			print >> fp, "	inst = get_last_instance_icwmpd(\"%s\", \"%s\", \"%s\");" % (dmmapfile, sectiontype, name+"instance")
			print >> fp, "	dmuci_add_section_and_rename(\"%s\", \"%s\", &s, &value);" % (file, sectiontype)
			print >> fp, "	//dmuci_set_value_by_section(s, \"option\", \"value\");"
			print >> fp, ""
			print >> fp, "	dmuci_add_section_icwmpd(\"%s\", \"%s\", &dmmap, &v);" % (dmmapfile, sectiontype)
			print >> fp, "	dmuci_set_value_by_section(dmmap, \"section_name\", section_name(s));"
			print >> fp, "	*instance = update_instance_icwmpd(dmmap, inst, \"%s\");" % (name+"instance")
	else:
		print >> fp, "	//TODO"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)" % fdelobj
	print >> fp, "{"
	if mappingobj != "false":
		if type == "uci":
			print >> fp, "	struct uci_section *s = NULL, *ss = NULL, *dmmap_section= NULL;"
			print >> fp, "	int found = 0;"
			print >> fp, ""
	print >> fp, "	switch (del_action) {"
	if mappingobj != "false":
		if type == "uci":
			print >> fp, "		case DEL_INST:"
			print >> fp, "			get_dmmap_section_of_config_section(\"%s\", \"%s\", section_name((struct uci_section *)data), &dmmap_section);" % (dmmapfile, sectiontype)
			print >> fp, "			if(dmmap_section != NULL)"
			print >> fp, "				dmuci_delete_by_section(dmmap_section, NULL, NULL);"
			print >> fp, "			dmuci_delete_by_section((struct uci_section *)data, NULL, NULL);"
			print >> fp, "			break;"
			print >> fp, "		case DEL_ALL:"
			print >> fp, "			uci_foreach_sections(\"%s\", \"%s\", s) {" % (file, sectiontype)
			print >> fp, "				if (found != 0){"
			print >> fp, "					get_dmmap_section_of_config_section(\"%s\", \"%s\", section_name(ss), &dmmap_section);" % (dmmapfile, sectiontype)
			print >> fp, "					if(dmmap_section != NULL)"
			print >> fp, "						dmuci_delete_by_section(dmmap_section, NULL, NULL);"
			print >> fp, "					dmuci_delete_by_section(ss, NULL, NULL);"
			print >> fp, "				}"
			print >> fp, "				ss = s;"
			print >> fp, "				found++;"
			print >> fp, "			}"
			print >> fp, "			if (ss != NULL) {"
			print >> fp, "				get_dmmap_section_of_config_section(\"%s\", \"%s\", section_name(ss), &dmmap_section);" % (dmmapfile, sectiontype)
			print >> fp, "				if(dmmap_section != NULL)"
			print >> fp, "					dmuci_delete_by_section(dmmap_section, NULL, NULL);"
			print >> fp, "				dmuci_delete_by_section(ss, NULL, NULL);"
			print >> fp, "			}"
			print >> fp, "			break;"
	else:
		print >> fp, "		case DEL_INST:"
		print >> fp, "			//TODO"
		print >> fp, "			break;"
		print >> fp, "		case DEL_ALL:"
		print >> fp, "			//TODO"
		print >> fp, "			break;"
	print >> fp, "	}"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	fp.close()

def hprintAddDelObj( faddobj, fdelobj ):
	fp = open('./.objadddel.h', 'a')
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char **instance);" % faddobj
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action);" % fdelobj
	fp.close()

def cprintBrowseObj( fbrowse, name, mappingobj, dmobject ):
	fp = open('./.objbrowse.c', 'a')
	if mappingobj != "false":
		type, file, sectiontype, dmmapfile = get_mapping_obj(mappingobj)
		print >> fp, "/*#%s!%s:%s/%s/%s*/" % (dmobject, type.upper(), file, sectiontype, dmmapfile)
	print >> fp, "int %s(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)" % fbrowse
	print >> fp, "{"
	if mappingobj != "false":
		if type == "uci" :
			print >> fp, "	char *inst = NULL, *inst_last = NULL;"
			print >> fp, "	struct dmmap_dup *p;"
			print >> fp, "	LIST_HEAD(dup_list);"
			print >> fp, ""
			print >> fp, "	synchronize_specific_config_sections_with_dmmap(\"%s\", \"%s\", \"%s\", &dup_list);" % (file, sectiontype, dmmapfile)
			print >> fp, "	list_for_each_entry(p, &dup_list, list) {"
			print >> fp, "		inst =  handle_update_instance(1, dmctx, &inst_last, update_instance_alias, 3, p->dmmap_section, \"%s\", \"%s\");" % (name+"instance", name+"alias")
			print >> fp, "		if (DM_LINK_INST_OBJ(dmctx, parent_node, (void *)p->config_section, inst) == DM_STOP)"
			print >> fp, "			break;"
			print >> fp, "	}"
			print >> fp, "	free_dmmap_config_dup_list(&dup_list);"	
	else:
		print >> fp, "	//TODO"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	fp.close()

def hprintBrowseObj( fbrowse ):
	fp = open('./.objbrowse.h', 'a')
	print >> fp, "int %s(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);" % fbrowse
	fp.close()

def cprintGetSetValue(getvalue, setvalue, mappingparam, instance, typeparam, parentname, dmparam):
	fp = open('./.getstevalue.c', 'a')
	if mappingparam != "false":
		type, res1, res2, res3, res4, res5 =get_mapping_param(mappingparam)
		if type == "uci":
			if res3 != "false":
				print >> fp, "/*#%s!%s:%s/%s,%s/%s*/" % (parentname+dmparam, type.upper(), res1, res2, res3, res5)
			else:
				print >> fp, "/*#%s!%s:%s/%s,%s/%s*/" % (parentname+dmparam, type.upper(), res1, res2, res4, res5)
		elif type == "ubus":
			if res3!= "false" and res4 != "false":
				print >> fp, "/*#%s!%s:%s/%s/%s,%s/%s*/" % (parentname+dmparam, type.upper(), res1, res2, res3, res4, res5)
			else:
				print >> fp, "/*#%s!%s:%s/%s//%s*/" % (parentname+dmparam, type.upper(), res1, res2, res5)

	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)" % getvalue
	print >> fp, "{"
	if mappingparam != "false":
			if type == "uci":
				if "NumberOfEntries" in dmparam:
					print >> fp, "	struct uci_section *s = NULL;"
					print >> fp, "	int cnt = 0;"
					print >> fp, ""
					print >> fp, "	uci_foreach_sections(\"%s\", \"%s\", s) {" % (res1, res2)
					print >> fp, "		cnt++;"
					print >> fp, "	}"
					print >> fp, "	dmasprintf(value, \"%d\", cnt);"
				elif instance == "TRUE":
					print >> fp, "	dmuci_get_value_by_section_string((struct uci_section *)data, \"%s\", value);" % res5
				else:
					print >> fp, "	dmuci_get_option_value_string(\"%s\", \"%s\", \"%s\", value);" % (res1, res3, res5)
			elif type == "ubus":
				print >> fp, "	json_object *res;"
				if res3 == "false" and res4 == "false":
					print >> fp, "	dmubus_call(\"%s\", \"%s\", UBUS_ARGS{}, 0, &res);" % (res1, res2)
				else:
					if res4 == "@Name":
						print >> fp, "	dmubus_call(\"%s\", \"%s\", UBUS_ARGS{{\"%s\", section_name((struct uci_section *)data), String}}, 1, &res);" % (res1, res2, res3)
					else:
						print >> fp, "	dmubus_call(\"%s\", \"%s\", UBUS_ARGS{{\"%s\", \"%s\", String}}, 1, &res);" % (res1, res2, res3, res4)
				print >> fp, "	DM_ASSERT(res, *value = \"\");"
				option = res5.split(".")
				if "." in res5:
					if option[0] == "@Name":
						print >> fp, "	*value = dmjson_get_value(res, 2, section_name((struct uci_section *)data), \"%s\");" % (option[1])
					else:
						print >> fp, "	*value = dmjson_get_value(res, 2, \"%s\", \"%s\");" % (option[0], option[1])
				else:
					print >> fp, "	*value = dmjson_get_value(res, 1, \"%s\");" % option[0]
			elif type == "cli":
					print >> fp, "	dmcmd(\"%s\", %s, %s);" % (res1, res2, res3)
	else:
		print >> fp, "	//TODO"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	if setvalue != "NULL":
		print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)" % setvalue
		print >> fp, "{"
		if mappingparam != "false":
			if type == "uci":
				if typeparam == "boolean":
					print >> fp, "	bool b;"
				if dmparam == "Alias":
					print >> fp, "	struct uci_section *dmmap_section;"
					print >> fp, ""
				print >> fp, "	switch (action)	{"
				print >> fp, "		case VALUECHECK:"
				if typeparam == "boolean":
					print >> fp, "			if (string_to_bool(value, &b))"
					print >> fp, "				return FAULT_9007;"
				print >> fp, "			break;"
				print >> fp, "		case VALUESET:"
				if typeparam == "boolean":
					print >> fp, "			string_to_bool(value, &b);"
					if instance == "TRUE":
						print >> fp, "			dmuci_set_value_by_section((struct uci_section *)data, \"%s\", b ? \"1\" : \"0\");" % res5
					else:
						print >> fp, "			dmuci_set_value(\"%s\", \"%s\", \"%s\", b ? \"1\" : \"0\");" % (res1, res3, res5)
				elif instance == "TRUE":
					print >> fp, "			dmuci_set_value_by_section((struct uci_section *)data, \"%s\", value);" % res5
				else:
					print >> fp, "			dmuci_set_value(\"%s\", \"%s\", \"%s\", value);" % (res1, res3, res5)
		else:
			print >> fp, "	switch (action)	{"
			print >> fp, "		case VALUECHECK:"
			print >> fp, "			break;"
			print >> fp, "		case VALUESET:"
			print >> fp, "			//TODO"
		print >> fp, "			break;"
		print >> fp, "	}"
		print >> fp, "	return 0;"
		print >> fp, "}"
		print >> fp, ""
	fp.close()

def hprintGetSetValue(getvalue, setvalue):
	fp = open('./.getstevalue.h', 'a')
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);" % getvalue
	if setvalue != "NULL":
		print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);" % setvalue
	fp.close()


def cprintheaderPARAMS( objname ):
	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "DMLEAF %s[] = {" % ("t" + getname(objname) + "Params")
	print >> fp,  "/* PARAM, permission, type, getvalue, setvalue, forced_inform, notification*/"
	fp.close()

def hprintheaderPARAMS( objname ):
	fp = open('./.objparamarray.h', 'a')
	print >> fp,  "extern DMLEAF %s[];" % ("t" + getname(objname) + "Params")
	fp.close()

def printPARAMline( parentname, dmparam, value ):
	commonname = getname(parentname) + "_" + dmparam
	ptype = getparamtype(value)
	getvalue = "get_" + commonname
	mappingparam = getoptionparam(value, "mapping")
	typeparam = getoptionparam(value, "type")
	accessparam = getoptionparam(value, "write")
	if (accessparam == "false"):
		access = "&DMREAD"
		setvalue = "NULL"
	else:
		access = "&DMWRITE"
		setvalue = "set_" + commonname
	if parentname.endswith(".{i}."):
		instance = "TRUE"
	else:
		instance = "FALSE"

	cprintGetSetValue(getvalue, setvalue, mappingparam, instance, typeparam, parentname, dmparam)
	hprintGetSetValue(getvalue, setvalue)

	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "{\"%s\", %s, %s, %s, %s, NULL, NULL}," % (dmparam, access, ptype, getvalue, setvalue)
	fp.close()

def printtailArray( ):
	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "{0}"
	print >> fp,  "};"
	print >> fp,  ""
	fp.close()

def printOBJline( dmobject, value ):
	commonname = getname(dmobject)
	hasobj = objhaschild(value)
	hasparam = objhasparam(value)
	accessobj = getoptionparam(value, "access")
	mappingobj = getoptionparam(value, "mapping")

	if accessobj == "false":
		access = "&DMREAD"
		faddobj = "NULL"
		fdelobj = "NULL"
	else:
		access = "&DMWRITE"
		faddobj = "addObj" + commonname
		fdelobj = "delObj" + commonname
		cprintAddDelObj(faddobj, fdelobj, (getlastname(dmobject)).lower(), mappingobj, dmobject)
		hprintAddDelObj(faddobj, fdelobj)
	if dmobject.endswith(".{i}."):
		fbrowse = "browse" + commonname + "Inst"
		cprintBrowseObj(fbrowse, (getlastname(dmobject)).lower(), mappingobj, dmobject)
		hprintBrowseObj(fbrowse)
	else:
		fbrowse = "NULL"
	if hasobj:
		objchildarray = "t" + commonname + "Obj"
	else:
		objchildarray = "NULL"
	if hasparam:
		paramarray = "t" + commonname + "Params"
	else:
		paramarray = "NULL"

	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "{\"%s\", %s, %s, %s, NULL, %s, NULL, NULL, %s, %s, NULL}," % (getlastname(dmobject), access, faddobj, fdelobj, fbrowse, objchildarray, paramarray)
	fp.close()

def printusage():
	print "Usage: " + sys.argv[0] + " <json data model>"
	print "Examples:"
	print "  - " + sys.argv[0] + " device.json"
	print "    ==> Generate the C code of all data model in rootdevice.c/.h"
	print "  - " + sys.argv[0] + " wifi.json"
	print "    ==> Generate the C code of all data model in wifi.c/.h"

def object_parse_childs( dmobject , value ):
	hasobj = objhaschild(value)
	hasparam = objhasparam(value)

	if hasobj or hasparam:
		printheaderObjCommon(dmobject)

	if hasobj:
		cprintheaderOBJS(dmobject)
		hprintheaderOBJS(dmobject)

		if isinstance(value,dict):
			for k,v in value.items():
				if isinstance(v,dict):
					for k1,v1 in v.items():
						if k1 == "type" and v1 == "object":
							printOBJline(k, v)
							break
		printtailArray()

	if hasparam:
		cprintheaderPARAMS(dmobject)
		hprintheaderPARAMS(dmobject)
		if isinstance(value,dict):
			for k,v in value.items():
				if k == "mapping":
					continue
				if isinstance(v,dict):
					for k1,v1 in v.items():
						if k1 == "type" and v1 != "object":
							printPARAMline(dmobject, k, v)
							break
		printtailArray()

	if hasobj:
		if isinstance(value,dict):
			for k,v in value.items():
				if isinstance(v,dict):
					for k1,v1 in v.items():
						if k1 == "type" and v1 == "object":
							object_parse_childs(k , v)

def generatecfromobj(pobj, pvalue, pdir):
	securemkdir(pdir)
	removetmpfiles()
	object_parse_childs(pobj, pvalue)
	
	dmfpc = open(pdir + "/" +  getname(pobj).lower() + ".c", "w")
	dmfph = open(pdir + "/" +  getname(pobj).lower() + ".h", "w")
	cprinttopfile(dmfpc, getname(pobj).lower())
	hprinttopfile(dmfph, getname(pobj).lower())
	try:
		tmpf = open("./.rootinclude.c", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfpc.write(tmpd)
		print >> dmfpc,  ""
	except:
		pass
	try:
		tmpf = open("./.objparamarray.c", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfpc.write(tmpd)
	except:
		pass
	try:
		tmpf = open("./.objparamarray.h", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfph.write(tmpd)
		print >> dmfph,  ""
	except:
		pass
	try:
		exists = os.path.isfile("./.objbrowse.c")
		if exists:
			print >> dmfpc,  "/*************************************************************"
			print >> dmfpc,  " * ENTRY METHOD"
			print >> dmfpc,  "/*************************************************************/"
		tmpf = open("./.objbrowse.c", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfpc.write(tmpd)
	except:
		pass
	try:
		tmpf = open("./.objbrowse.h", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfph.write(tmpd)
		print >> dmfph,  ""
	except:
		pass
	try:
		exists = os.path.isfile("./.objadddel.c")
		if exists:
			print >> dmfpc,  "/*************************************************************"
			print >> dmfpc,  " * ADD & DEL OBJ"
			print >> dmfpc,  "/*************************************************************/"
		tmpf = open("./.objadddel.c", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfpc.write(tmpd)
	except:
		pass
	try:
		tmpf = open("./.objadddel.h", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfph.write(tmpd)
		print >> dmfph,  ""
	except:
		pass
	try:
		exists = os.path.isfile("./.getstevalue.c")
		if exists:
			print >> dmfpc,  "/*************************************************************"
			print >> dmfpc,  " * GET & SET PARAM"
			print >> dmfpc,  "/*************************************************************/"
		tmpf = open("./.getstevalue.c", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfpc.write(tmpd)
	except:
		pass
	try:
		tmpf = open("./.getstevalue.h", "r")
		tmpd = tmpf.read()
		tmpf.close()
		dmfph.write(tmpd)
	except:
		pass

	hprintfootfile (dmfph, getname(pobj).lower())
	removetmpfiles()
	

def removetmpfiles():
	removefile("./.objparamarray.c")
	removefile("./.objparamarray.h")
	removefile("./.objadddel.c")
	removefile("./.objadddel.h")
	removefile("./.objbrowse.c")
	removefile("./.objbrowse.h")
	removefile("./.getstevalue.c")
	removefile("./.getstevalue.h")
	removefile("./.rootinclude.c")

### main ###
if len(sys.argv) < 2:
	printusage()
	exit(1)
	
if (sys.argv[1]).lower() == "-h" or (sys.argv[1]).lower() == "--help":
	printusage()
	exit(1)

model_root_name = "Root"
json_file = sys.argv[1]

with open(json_file) as file:
	data = json.loads(file.read(), object_pairs_hook=OrderedDict)

gendir = "source_" + time.strftime("%Y-%m-%d_%H-%M-%S")

for i,(key,value) in enumerate(data.items()):
	objstart = key
	device = key.split(".")
	dmroot = device[0]

	if dmroot == None:
		print "Wrong JSON Data model format!"
		exit(1)

	generatecfromobj(objstart, value, gendir)


if (os.path.isdir(gendir)):
	print "Source code generated under \"./%s\"" % gendir
else:
	print "No source code generated!"

