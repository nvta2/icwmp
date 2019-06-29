#!/usr/bin/python

#      This program is free software: you can redistribute it and/or modify
#      it under the terms of the GNU General Public License as published by
#      the Free Software Foundation, either version 2 of the License, or
#      (at your option) any later version.
#
#      Copyright (C) 2019 iopsys Software Solutions AB
#		Author: Amin Ben Ramdhane <amin.benramdhane@pivasoftware.com>

#	Copyright (C) 2019 PIVA SOFTWARE <www.pivasoftware.com> - All Rights Reserved
#		Author: Mohamed Kallel <mohamed.kallel@pivasoftware.com>

import os
import xml.etree.ElementTree as xml
import sys
import time

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
		OBJSname = objname.replace(dmroot.get('name'), "", 1)
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
	if (is_service_model == 1):
		OBJSname = model_root_name + OBJSname
	return OBJSname;

def getparamdatatyperef( datatyperef ):
	ptype = None
	for d in xmlroot:
		if d.tag == "dataType" and d.get("name") == datatyperef:
			if d.get("base") != "" and d.get("base") != None:
				ptype = getparamdatatyperef(d.get("base"))
			else:
				for dd in d:
					ptype = arrtype.get(dd.tag, None)
					if ptype != None:
						break
				break
	return ptype

def getparamtype( dmparam ):
	ptype = None
	for s in dmparam:
		if s.tag == "syntax":
			for c in s:
				if c.tag == "list":
					ptype = "DMT_STRING"
					break
				ptype = arrtype.get(c.tag, None)
				if ptype != None:
					break
				if c.tag == "dataType":
					reftype = c.get("ref")
					ptype = getparamdatatyperef(reftype)
				if ptype != None:
					break
			break
	if ptype == None:
		ptype = "__NA__"
	return ptype

def objhaschild (parentname, level):
	hasobj = 0
	for c in model:
		objname = c.get('name')
		if c.tag == "object" and parentname in objname and (objname.count('.') - objname.count('{i}')) == level:
			hasobj = 1
			break;
	return hasobj

def objhasparam (dmobject):
	hasparam = 0
	for c in dmobject:
		if c.tag == "parameter":
			hasparam = 1
			break;
	return hasparam

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

def cprintAddDelObj( faddobj, fdelobj ):
	fp = open('./.objadddel.c', 'a')
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char **instance)" % faddobj
	print >> fp, "{"
	print >> fp, "	//TODO"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, unsigned char del_action)" % fdelobj
	print >> fp, "{"
	print >> fp, "	switch (del_action) {"
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

def cprintBrowseObj( fbrowse ):
	fp = open('./.objbrowse.c', 'a')
	print >> fp, "int %s(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance)" % fbrowse
	print >> fp, "{"
	print >> fp, "	//TODO"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	fp.close()

def hprintBrowseObj( fbrowse ):
	fp = open('./.objbrowse.h', 'a')
	print >> fp, "int %s(struct dmctx *dmctx, DMNODE *parent_node, void *prev_data, char *prev_instance);" % fbrowse
	fp.close()

def cprintGetSetValue(getvalue, setvalue, ptype, dmparam):
	fp = open('./.getstevalue.c', 'a')
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value)" % getvalue
	print >> fp, "{"
	print >> fp, "	//TODO"
	print >> fp, "	return 0;"
	print >> fp, "}"
	print >> fp, ""
	if setvalue != "NULL":
		print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action)" % setvalue
		print >> fp, "{"
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

def hprintGetSetValue(getvalue, setvalue, ptype):
	fp = open('./.getstevalue.h', 'a')
	print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char **value);" % getvalue
	if setvalue != "NULL":
		print >> fp, "int %s(char *refparam, struct dmctx *ctx, void *data, char *instance, char *value, int action);" % setvalue
	fp.close()

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

def printOBJline( dmobject, level ):
	commonname = getname(dmobject.get('name'))
	hasobj = objhaschild(dmobject.get('name'), level)
	hasparam = objhasparam(dmobject)

	if (dmobject.get('access') == "readOnly"):
		access = "&DMREAD"
		faddobj = "NULL"
		fdelobj = "NULL"
	else:
		access = "&DMWRITE"
		faddobj = "addObj" + commonname
		fdelobj = "delObj" + commonname
		cprintAddDelObj(faddobj, fdelobj)
		hprintAddDelObj(faddobj, fdelobj)
	if (dmobject.get('name')).endswith(".{i}."):
		fbrowse = "browse" + commonname + "Inst"
		cprintBrowseObj(fbrowse)
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
	print >> fp,  "{\"%s\", %s, %s, %s, NULL, %s, NULL, NULL, %s, %s, NULL}," % (getlastname(dmobject.get('name')), access, faddobj, fdelobj, fbrowse, objchildarray, paramarray)
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

def printPARAMline( parentname, dmparam ):
	commonname = getname(parentname) + "_" + dmparam.get('name')
	ptype = getparamtype(dmparam)
	getvalue = "get_" + commonname
	if (dmparam.get('access') == "readOnly"):
		access = "&DMREAD"
		setvalue = "NULL"
	else:
		access = "&DMWRITE"
		setvalue = "set_" + commonname

	cprintGetSetValue(getvalue, setvalue, ptype, dmparam)
	hprintGetSetValue(getvalue, setvalue, ptype)

	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "{\"%s\", %s, %s, %s, %s, NULL, NULL}," % (dmparam.get('name'), access, ptype, getvalue, setvalue)
	fp.close()

def printtailArray( ):
	fp = open('./.objparamarray.c', 'a')
	print >> fp,  "{0}"
	print >> fp,  "};"
	print >> fp,  ""
	fp.close()

def printincluderoot( objname ):
	fp = open('./.rootinclude.c', 'a')
	print >> fp,  "#include \"dm%s.h\"" % getname(objname).lower()
	fp.close()

def printusage():
	print "Usage: " + sys.argv[0] + " <xml data model> [Object path]...[Object path]"
	print "Examples:"
	print "  - " + sys.argv[0] + " tr-181-2-12-0-cwmp-full.xml Device.WiFi."
	print "    ==> Generate the C code of the sub tree Device.WiFi. in wifi.c/.h"
	print "  - " + sys.argv[0] + " tr-181-2-12-0-cwmp-full.xml Device.IP.Diagnostics."
	print "    ==> Generate the C code of the sub tree Device.IP.Diagnostics. in ipdiagnostics.c/.h"
	print "  - " + sys.argv[0] + " tr-181-2-12-0-cwmp-full.xml Device.WiFi. Device.Time."
	print "    ==> Generate the C code of the sub tree Device.IP. and Device.WiFi. in time.c/.h and wifi.c/.h"
	print "  - " + sys.argv[0] + " tr-181-2-12-0-cwmp-full.xml Device."
	print "    ==> Generate the C code of all data model in rootdevice.c/.h"
	print "Example of xml data model file: https://www.broadband-forum.org/cwmp/tr-181-2-12-0-cwmp-full.xml"

def getobjectpointer( objname ):
	obj = None
	for c in model:
		if c.tag == "object" and (c.get('name') == objname or c.get('name') == (objname + "{i}.") ):
			obj = c
			break
	return obj

def object_parse_childs( dmobject, level ):

	hasobj = objhaschild(dmobject.get('name'), level)
	hasparam = objhasparam(dmobject)

	if hasobj or hasparam:
		printheaderObjCommon(dmobject.get('name'))

	if hasobj:
		cprintheaderOBJS(dmobject.get('name'))
		hprintheaderOBJS(dmobject.get('name'))

		for c in model:
			objname = c.get('name')
			if c.tag == "object" and dmobject.get('name') in objname and (objname.count('.') - objname.count('{i}')) == level:
				printOBJline(c, level+1)

		printtailArray()

	if hasparam:
		cprintheaderPARAMS(dmobject.get('name'))
		hprintheaderPARAMS(dmobject.get('name'))
		for c in dmobject:
			paramname = c.get('name')
			if c.tag == "parameter":
				printPARAMline(dmobject.get('name'), c)
		printtailArray()

	if hasobj:	
		for c in model:
			objname = c.get('name')
			if c.tag == "object" and dmobject.get('name') in objname and (objname.count('.') - objname.count('{i}')) == level:
				object_parse_childs(c, level+1)
	return;

def generatecfromobj(pobj, pdir):
	securemkdir(pdir)
	removetmpfiles()
	dmlevel = (pobj.get('name')).count(".") - (pobj.get('name')).count("{i}.") + 1
	object_parse_childs(pobj, dmlevel)
	
	dmfpc = open(pdir + "/" +  (getname(pobj.get('name'))).lower() + ".c", "w")
	dmfph = open(pdir + "/" +  (getname(pobj.get('name'))).lower() + ".h", "w")
	cprinttopfile(dmfpc, (getname(pobj.get('name'))).lower())
	hprinttopfile(dmfph, (getname(pobj.get('name'))).lower())
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

	hprintfootfile (dmfph, (getname(pobj.get('name'))).lower())
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
if len(sys.argv) < 3:
	printusage()
	exit(1)
	
if (sys.argv[1]).lower() == "-h" or (sys.argv[1]).lower() == "--help":
	printusage()
	exit(1)

is_service_model = 0
model_root_name = "Root"
tree = xml.parse(sys.argv[1])

xmlroot = tree.getroot()
model = xmlroot

for child in model:
	if child.tag == "model":
		model = child

if model.tag != "model":
	print "Wrong XML Data model format!"
	exit(1)

dmroot = None
for c in model:
	if c.tag == "object" and c.get("name").count(".") == 1:
		dmroot = c
		break;

#If it is service data model
if dmroot == None:
	is_service_model = 1
	for c in model:
		if c.tag == "object" and c.get("name").count(".") == 2:
			dmroot = c
			break;

if dmroot == None:
	print "Wrong XML Data model format!"
	exit(1)

gendir = "source_" + time.strftime("%Y-%m-%d_%H-%M-%S")
isemptytreeargs = 1
	
if (len(sys.argv) > 2):
	for i in range(2, len(sys.argv)):
		if sys.argv[i] == "":
			continue
		isemptytreeargs = 0
		objstart = getobjectpointer(sys.argv[i])
		if objstart == None:
			print "Wrong Object Name! %s" % sys.argv[i]
			continue
		generatecfromobj(objstart, gendir)

if (os.path.isdir(gendir)):
	print "Source code generated under \"./%s\"" % gendir
else:
	print "No source code generated!"

