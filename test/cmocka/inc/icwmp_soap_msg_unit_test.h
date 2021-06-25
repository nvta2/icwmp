#ifndef __SOAP_MSG_TEST__
#define __SOAP_MSG_TEST__

#define CWMP_GETPARAMETERVALUES_REQ                                                                                                                                                                                                                                                                        \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "                                                            \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"                                                                                                                                                                                                                                         \
		"<SOAP-ENV:Header>" \
		       "<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.GetParameterValues1623750334191.21093494</cwmp:ID>" \
		       "<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:GetParameterValues>" \
				"<ParameterNames SOAP-ENC:arrayType=\"xsd:string[1]\">" \
				"</ParameterNames>" \
			"</cwmp:GetParameterValues>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#define CWMP_SETPARAMETERVALUES_REQ                                                                                                                                                                                                                                                                        \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "  \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"                                                                                                                                                                                                                                          \
		"<SOAP-ENV:Header>" \
		        "<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.SetParameterValues1624295944533.1489013116</cwmp:ID>" \
		        "<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:SetParameterValues xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\">" \
				"<ParameterList SOAP-ENC:arrayType=\"cwmp:ParameterValueStruct[1]\">" \
					"<ParameterValueStruct>" \
					"</ParameterValueStruct>" \
				"</ParameterList>" \
				"<ParameterKey>set_value_test</ParameterKey>" \
			"</cwmp:SetParameterValues>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#define CWMP_ADDOBJECT_REQ  \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "  \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"  \
		"<SOAP-ENV:Header>" \
			"<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.AddObject1624461905714.1384387619</cwmp:ID>" \
			"<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:AddObject xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\">" \
				"<ObjectName></ObjectName>" \
				"<ParameterKey>add_object_test</ParameterKey>" \
			"</cwmp:AddObject>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#define CWMP_DELOBJECT_REQ  \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "  \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"  \
		"<SOAP-ENV:Header>" \
			"<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.DeleteObject1624464905078.1243670982</cwmp:ID>" \
			"<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:DeleteObject xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\">" \
				"<ObjectName></ObjectName>" \
				"<ParameterKey>del_object_test</ParameterKey>" \
			"</cwmp:DeleteObject>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#define CWMP_GETATTRIBUTES_REQ \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "  \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"  \
		"<SOAP-ENV:Header>" \
			"<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.GetParameterAttributes1624541216937.139484077</cwmp:ID>" \
			"<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:GetParameterAttributes xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\">" \
				"<ParameterNames SOAP-ENC:arrayType=\"xsd:string[1]\"></ParameterNames>" \
			"</cwmp:GetParameterAttributes>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#define CWMP_SETATTRIBUTES_REQ \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "  \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"  \
		"<SOAP-ENV:Header>" \
			"<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.SetParameterAttributes1624546080334.1155903494</cwmp:ID>" \
			"<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:SetParameterAttributes xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\">" \
				"<ParameterList SOAP-ENC:arrayType=\"cwmp:SetParameterAttributesStruct[1]\">" \
					"<SetParameterAttributesStruct>" \
						"<Name></Name>" \
						"<NotificationChange>1</NotificationChange>" \
						"<Notification></Notification>" \
						"<AccessList SOAP-ENC:arrayType=\"xsd:string[1]\"><string xsi:type=\"xsd:string\">subscriber</string></AccessList>" \
						"<AccessListChange>1</AccessListChange>" \
					"</SetParameterAttributesStruct>" \
				"</ParameterList>" \
			"</cwmp:SetParameterAttributes>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#define CWMP_DOWNLOAD_REQ \
	"<SOAP-ENV:Envelope xmlns:SOAP-ENC=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "  \
	"xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"  \
		"<SOAP-ENV:Header>" \
			"<cwmp:ID SOAP-ENV:mustUnderstand=\"1\">ID:intrnl.unset.id.Download1624556569149.1849926569</cwmp:ID>" \
			"<cwmp:NoMoreRequests>0</cwmp:NoMoreRequests>" \
		"</SOAP-ENV:Header>" \
		"<SOAP-ENV:Body>" \
			"<cwmp:Download xmlns:cwmp=\"urn:dslforum-org:cwmp-1-0\">" \
				"<CommandKey>download_test</CommandKey>" \
				"<FileType></FileType>" \
				"<URL></URL>" \
				"<Username></Username>" \
				"<Password></Password>" \
				"<FileSize>0</FileSize>" \
				"<TargetFileName></TargetFileName>" \
				"<DelaySeconds>0</DelaySeconds>" \
				"<SuccessURL></SuccessURL>" \
				"<FailureURL></FailureURL>" \
			"</cwmp:Download>" \
		"</SOAP-ENV:Body>" \
	"</SOAP-ENV:Envelope>"

#endif
