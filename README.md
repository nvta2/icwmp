# README #

icwmp is a client implementation of [TR-069/CWMP](https://cwmp-data-models.broadband-forum.org/) protocol.

## Build Instructions ##

icwmp is written in C programming language and depends on a number of librairies of OpenWrt for building and running.

## Project Components ##

The package is composed of the following components:

- [ICWMP](https://dev.iopsys.eu/iopsys/icwmp/blob/master/README.md) daemon written in C programming language and it's in charge of communication with ACS using the protocol TR-069/CWMP.
- [STUN](https://dev.iopsys.eu/iopsys/icwmp/blob/master/stun/stun.md) daemon written in C for TR-069 stun connection request feature. the TR-069 stun is described in TR-069(Annex G) and in TR111(Part 2).
- [XMPP](https://dev.iopsys.eu/iopsys/icwmp/blob/master/xmpp/xmpp.md) daemon written in C for TR-069 xmpp connection request feature. the TR-069 xmpp is described in TR-069(Annex K).
- [BulkData](https://dev.iopsys.eu/iopsys/icwmp/blob/master/bulkdata/bulkdata.md) daemon written in C for collecting data from device to the data server collector. the TR-069 bulkdata is described in TR-069(Annex N) and TR157 Amendment 10(Annex A).
- [TWAMP](https://dev.iopsys.eu/iopsys/icwmp/blob/master/twamp/twamp.md) daemon written in C. It is an implementation of the Two-Way Active Measurement Protocol (TWAMP) reflector measuring network as described in TR-390.
- [UDP Echo Server](https://dev.iopsys.eu/iopsys/icwmp/blob/master/udpechoserver/udpechoserver.md) daemon written in C to support the UDP Echo Service. It's described in TR143.

**NOTE: The [STUN](https://dev.iopsys.eu/iopsys/icwmp/blob/master/stun/stun.md), [XMPP](https://dev.iopsys.eu/iopsys/icwmp/blob/master/xmpp/xmpp.md), [BulkData](https://dev.iopsys.eu/iopsys/icwmp/blob/master/bulkdata/bulkdata.md), [TWAMP](https://dev.iopsys.eu/iopsys/icwmp/blob/master/twamp/twamp.md) and [UDP Echo Server](https://dev.iopsys.eu/iopsys/icwmp/blob/master/udpechoserver/udpechoserver.md) are described in details in their relative sub packages.**

**NOTE: Below the detailed description about the icwmp.**

## Good To know ##

The icwmp client is :
* tested with several ACS such as **Axiros**, **AVSytem**, **GenieACS**, **OpenACS**, etc...
* supports all required **TR069 RPCs**.
* supports all DataModel of TR family such as **TR-181**, **TR-104**, **TR-143**, **TR-157**, **TR-098**, etc...
* supports all types of connection requests such as **HTTP**, **XMPP**, **STUN**.
* supports integrated file transfer such as **HTTP**, **HTTPS**, **FTP**.

## Configuration File ##

The icwmp UCI configuration is located in **'/etc/config/cwmp'**, and contains 3 sections: **'acs'**, **'cpe'** and **'lwn'**.

```
config cwmp 'acs'
	option userid 'iopsys'
	option dhcp_discovery 'enable'
	option compression 'Disabled'
	option retry_min_wait_interval '5'
	option retry_interval_multiplier '2000'

config cwmp 'cpe'
	option interface 'eth0.1'
	option default_wan_interface 'wan'
	option userid 'iopsys'
	option vlan_method '1'
	option datamodel 'tr181'
	option exec_download '0'
	
config cwmp 'lwn'
	option enable '1'
	option hostname ''
	option port ''
```

### cwmp asc section ###

It defines **acs configuration** (like acs url, acs username, etc...). The possible options for **acs** section are listed in the table below.

| Name                        |  Type   | Description                     |
| --------------------------- | ------- | ------------------------------- |
| `url`                       | string  | the **url** of ACS server. |
| `userid`                    | string  | the **username** of ACS server. |
| `passwd`                    | string  | the **password** of ACS server. |
| `periodic_inform_enable`    | boolean | If set to **1**, the CPE must periodically open session with ACS by sending Inform message to the ACS. |
| `periodic_inform_interval`  | integer | The duration in seconds of the interval for which the CPE must attempt to connect with the ACS and call the Inform method. |
| `periodic_inform_time`      | integer | An absolute time reference to determine when the CPE will initiate the periodic Inform method calls. |
| `ParameterKey`              | string  | Provides the ACS a reliable and extensible means to track changes made by the ACS. |
| `dhcp_discovery`            | string  | if set to **enable**, the CPE will get the url of ACS from DHCP server Option 43. |
| `compression`               | boolean | if set to **1**, the CPE must use the HTTP Compression when communicating with the ACS. |
| `retry_min_wait_interval`   | integer | The min wait interval of rerty session (in seconds) as described in the standard. |
| `retry_interval_multiplier` | integer | The retry interval mulpilier of rerty session as described in the standard. |
| `https_ssl_capath`          | string  | The path of ssl certicafications files. the ssl certification is for Upload and Downlod methods. |
| `ipv6_enable`               | boolean | if set to **1**, enable IPv6. |
| `ssl_capath`                | string  | The path of ssl certicafications files. the ssl certification is  for TR-069 sessions. |
| `insecure_enable`           | boolean | if set to **1**, the CPE checks the validity of the ACS certificates. |
| `http_disable_100continue`  | boolean | if set to **1**, disables the http 100 continue behaviour. |
| `dhcp_url`                  | string  | the **url** of ACS server received from the DHCP server Option 43 when **'dhcp_discovery'** option is enabled. This option is updated automatically by the daemon. |

### cwmp cpe section ###

It defines **device configuration** (such as interface, manufacturer, etc...). The possible options for **cpe** section are listed in the table below.

| Name                    |  Type   | Description                                                  |
| ----------------------- | ------- | ------------------------------------------------------------ |
| `interface`             | string  | Specifies the device interface to use for connecting to ACS. |
| `default_wan_interface` | string  | Specifies the default wan interface of the device. |
| `manufacturer`          | string  | Specifies the manafacturer of the device. |
| `log_to_console`        | string  | If set to **1**, the log messages will be shown in the console. |
| `log_to_file`           | string  | If set to **1**, the log messages will be saved in the log file. |
| `log_severity`          | string  | Specifies the log type to use, by default **'INFO'**. The possible types are **'EMERG', 'ALERT', 'CRITIC' ,'ERROR', 'WARNING', 'NOTICE', 'INFO' and 'DEBUG'**. |
| `log_file_name`         | string  | Specifies the path of the log file, by default **'/var/log/icwmpd.log'**. |
| `log_max_size`          | integer | Size of the log file. The default value is **102400**. |
| `userid`                | string  | The username of the device used in a connection request from ACS to CPE. |
| `passwd`                | string  | The password of the device when sending a connection request from ACS to CPE. |
| `port`                  | integer | The port used for connection request. |
| `ubus_socket`           | string  | Specifies the path of the ubus socket file, by default **'/var/run/ubus.sock'**. |
| `provisioning_code`     | string  | Specifies the primary service provider and other provisioning information, which may be used by the ACS to determine service provider-specific customization and provisioning parameters. |
| `amd_version`           | integer | Specifies the amendment version to use. The default amendment version is **'2'**. |
| `instance_mode`         | string  | Specifies the instance mode to use, by default **'InstanceNumber'**. Two instance modes are supported: **'InstanceNumber' and 'InstanceNumber'**. |
| `session_timeout`       | integer | |
| `notification`          | boolean | If set to **1**, it enables the notification feature. |
| `vlan_method`           | integer | Specifies the vlan method to use, by default **'1'**. Two vlan methods are supported: **'1' and '2'** (1: only tagged vlan termination, 2: all vlan terminations tagged and untagged). |
| `datamodel`             | string  | Specifies the data model to use, by default **'tr181'**. Two data models are supported: **'tr181' and 'tr098'**. |
| `exec_download`         | boolean | If set to **1**, Specifies if Download method is executed. |

### cwmp lwn section ###

It defines **lightweight notification configuration** (like enable, hostname, etc...). The possible options for **lwn** section are listed in the table below.

| Name       | Type    | Description                                                                        |
| ---------- | ------- | ---------------------------------------------------------------------------------- |
| `enable`   | boolean | if set to **1**, the Lightweight Notifications will be enabled.                    |
| `hostname` | string  | The hostname or address to be used when sending the UDP Lightweight Notifications. |
| `port`     | integer | The port number to be used when sending UDP Lightweight Notifications.             |

## RPCs Method supported ##

the folowing tables provides a summary of all methods, and indicates the conditions under which implementation of each RPC method defined in Annex A is `REQUIRED` or `OPTIONAL`.

### Methods for CPE responding ###

| Method name              | CPE requirement | Supported |
| ------------------------ | --------------- | --------- |
| `GetRPCMethods`          | `REQUIRED`      | `Yes`     |
| `SetParameterValues`     | `REQUIRED`      | `Yes`     |
| `GetParameterValues`     | `REQUIRED`      | `Yes`     |
| `GetParameterNames`      | `REQUIRED`      | `Yes`     |
| `SetParameterAttributes` | `REQUIRED`      | `Yes`     |
| `GetParameterAttributes` | `REQUIRED`      | `Yes`     |
| `AddObject`              | `REQUIRED`      | `Yes`     |
| `DeleteObject`           | `REQUIRED`      | `Yes`     |
| `Reboot`                 | `REQUIRED`      | `Yes`     |
| `Download`               | `REQUIRED`      | `Yes`     |
| `ScheduleDownload`       | OPTIONAL        | `Yes`     |
| `Upload`                 | OPTIONAL        | `Yes`     |
| `FactoryReset`           | OPTIONAL        | `Yes`     |
| `GetQueuedTransfers`     | OPTIONAL        | No        |
| `GetAllQueuedTransfers`  | OPTIONAL        | No        |
| `CancelTransfer`         | OPTIONAL        | `Yes`     |
| `ScheduleInform`         | OPTIONAL        | `Yes`     |
| `ChangeDUState`          | OPTIONAL        | `Yes`     |
| `SetVouchers`            | OPTIONAL        | No        |
| `GetOptions`             | OPTIONAL        | No        |


### Methods for CPE calling ###

| Method name                       | CPE requirement | Supported |
| --------------------------------- | --------------- | --------- |
| `GetRPCMethods`                   | OPTIONAL        | `Yes`     |
| `Inform`                          | `REQUIRED`      | `Yes`     |
| `TransferComplete`                | `REQUIRED`      | `Yes`     |
| `AutonomousTransferComplete`      | OPTIONAL        | No        |
| `DUStateChangeComplete`           | OPTIONAL        | `Yes`     |
| `AutonomousDUStateChangeComplete` | OPTIONAL        | No        |
| `RequestDownload`                 | OPTIONAL        | No        |
| `Kicked`                          | OPTIONAL        | No        |


## Concepts and Workflow ##

As indicated in the TR069 standard, the icwmpd starts automatically when the system is started. Then it connects to the ACS, that can be set manually by the admin or found by dhcp discovery. And later  it could start other sessions due to event causes.

Session workflow could be checked with sniffer packets tool such as wireshark or tcpdump.
In addition icwmpd has a log file '/var/log/icwmpd.log', that describes the workflow. E.g. below you can find an abstract of a log file content:

```
24-12-2019, 10:21:18 [INFO]    STARTING ICWMP with PID :7762
24-12-2019, 10:21:18 [INFO]    Periodic event is enabled. Interval period = 180000s
24-12-2019, 10:21:18 [INFO]    Periodic time is Unknown
24-12-2019, 10:21:18 [INFO]    Connection Request server initiated with the port: 7547
24-12-2019, 10:21:18 [INFO]    Start session
24-12-2019, 10:21:18 [INFO]    ACS url: http://genieacs:7547
24-12-2019, 10:21:18 [INFO]    Preparing the Inform RPC message to send to the ACS
24-12-2019, 10:21:18 [INFO]    Send the Inform RPC message to the ACS
24-12-2019, 10:21:19 [INFO]    Get the InformResponse message from the ACS
24-12-2019, 10:21:19 [INFO]    Send empty message to the ACS
24-12-2019, 10:21:19 [INFO]    Receive HTTP 204 No Content
24-12-2019, 10:21:19 [INFO]    End session
24-12-2019, 10:21:19 [INFO]    Waiting the next session
```

You could set the uci config `cwmp.cpe.log_severity` option to `'DEBUG'` in order to show in details the cwmp log.

## ICWMP UBUS ##

icwmpd must be launched on startup after ubusd. It exposes the CWMP functionality over ubus. The icwmpd registers `tr069` namespaces with ubus, that has the shown below functionalities:

```
root@iopsys:~# ubus -v list tr069
'tr069' @04d3de4e
	"notify":{}
	"command":{"command":"String"}
	"status":{}
	"inform":{"GetRPCMethods":"Boolean","event":"String"}
root@iopsys:~# 
```

Each object registered with the `'tr069'` namespace has a specific functionality.

- To get the status of cwmp client, use the `status` ubus method:

```
root@iopsys:~# ubus call tr069 status
{
	"cwmp": {
		"status": "up",
		"start_time": "2019-12-24T10:21:18+01:00",
		"acs_url": "http:\/\/genieacs:7547"
	},
	"last_session": {
		"status": "success",
		"start_time": "2019-12-24T10:21:18+01:00",
		"end_time": "2019-12-24T10:21:19+01:00"
	},
	"next_session": {
		"status": "waiting",
		"start_time": "2019-12-26T12:21:18+01:00",
		"end_time": "N\/A"
	},
	"statistics": {
		"success_sessions": 1,
		"failure_sessions": 0,
		"total_sessions": 1
	}
}
root@iopsys:~#
```

- To trigger a new session of notify when a parameter is changed, then use the `notify` ubus method:

```
root@iopsys:~# ubus call tr069 notify
{
	"status": 1
}
root@iopsys:~#
```

- To trigger a new session to ACS with the event `'6 CONNECTION REQUEST'` or `'8 DIAGNOSTICS COMPLETE'`, etc.., use the `inform` ubus method with the appropriate `event` argument:

```
root@iopsys:~# ubus call tr069 inform '{"event":"6 CONNECTION REQUEST"}'
{
	"status": 1,
	"info": "Session started"
}
root@iopsys:~#
root@iopsys:~# ubus call tr069 inform '{"event":"8 DIAGNOSTICS COMPLETE"}'
{
	"status": 1,
	"info": "Session started"
}
root@iopsys:~#
root@iopsys:~# ubus call tr069 inform '{"GetRPCMethods":"1"}'
{
	"status": 1,
	"info": "Session started"
}
root@iopsys:~#
```

- To reload the icwmp config, use the `command` ubus method with `reload` argument:

```
root@iopsys:~# ubus call tr069 command '{"command":"reload"}'
{
	"status": 1,
	"info": "cwmp config reloaded"
}
root@iopsys:~# 
```
- To exit the icwmpd daemod, use the `command` ubus method with `exit` argument:

```
root@iopsys:~# ubus call tr069 command '{"command":"exit"}'
{
	"status": 1,
	"info": "icwmpd daemon stopped"
}
root@iopsys:~# 
```

## ICWMP CLI ##

icwmpd offers a cli tool which its options are described with `--help` option as below:

```
root@iopsys:~# icwmpd --help
Usage: icwmpd [OPTIONS]
 -b, --boot-event                                    (CWMP daemon) Start CWMP with BOOT event
 -g, --get-rpc-methods                               (CWMP daemon) Start CWMP with GetRPCMethods request to ACS
 -c, --command-input                                 (DataModel CLI) Execute data model rpc(s) with commands input
 -m, --shell-cli <data model rpc>                    (DataModel CLI) Execute data model RPC command directly from shell.
 -a, --alias-based-addressing                        (DataModel CLI) Alias based addressing supported
 -N, --instance-mode-number                          (DataModel CLI) Instance mode is Number (Enabled by default)
 -A, --instance-mode-alias                           (DataModel CLI) Instance mode is Alias
 -M, --amendment <amendment version>                 (DataModel CLI) Amendment version (Default amendment version is 2)
 -U, --upnp                                          (DataModel CLI) Use UPNP data model paths
 -u, --user-acl <public|basic|xxxadmin|superadmin>   (DataModel CLI) user access level. Default: superadmin
 -t, --time-tracking                                 (DataModel CLI) Tracking time of RPC commands
 -E, --evaluating-test                               (DataModel CLI) Evaluating test format
 -f, --file <file path>                              (DataModel CLI) Execute data model rpc(s) from file
 -w, --wep <strength> <passphrase>                   (WEP KEY GEN) Generate wep keys
 -h, --help                                          Display this help text
 -v, --version                                       Display the version
root@iopsys:~# 
```

For example `'-m'` option is used to connect to the datamodel **(libbbfdm)** as following:

```
root@iopsys:~# icwmpd -m 1 get_value Device.Time.
{ "parameter": "Device.Time.CurrentLocalTime", "value": "2019-12-19T16:51:53+01:00", "type": "xsd:dateTime" }
{ "parameter": "Device.Time.Enable", "value": "1", "type": "xsd:boolean" }
{ "parameter": "Device.Time.LocalTimeZone", "value": "CET-1CEST,M3.5.0,M10.5.0/3", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer1", "value": "ntp1.sth.netnod.se", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer2", "value": "ntp1.gbg.netnod.se", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer3", "value": "", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer4", "value": "", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer5", "value": "", "type": "xsd:string" }
{ "parameter": "Device.Time.Status", "value": "Synchronized", "type": "xsd:string" }
{ "parameter": "Device.Time.X_IOPSYS_EU_LocalTimeZoneOlson", "value": "Europe/Stockholm", "type": "xsd:string" }
{ "parameter": "Device.Time.X_IOPSYS_EU_SourceInterface", "value": "", "type": "xsd:string" }
root@iopsys:~#
```
Another way to use icwmp cli is via the script `'icwmp'` that can be runned as follow:

```
root@iopsys:~# icwmp get Device.Time.
{ "parameter": "Device.Time.CurrentLocalTime", "value": "2019-12-19T16:52:05+01:00", "type": "xsd:dateTime" }
{ "parameter": "Device.Time.Enable", "value": "1", "type": "xsd:boolean" }
{ "parameter": "Device.Time.LocalTimeZone", "value": "CET-1CEST,M3.5.0,M10.5.0/3", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer1", "value": "ntp1.sth.netnod.se", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer2", "value": "ntp1.gbg.netnod.se", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer3", "value": "", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer4", "value": "", "type": "xsd:string" }
{ "parameter": "Device.Time.NTPServer5", "value": "", "type": "xsd:string" }
{ "parameter": "Device.Time.Status", "value": "Synchronized", "type": "xsd:string" }
{ "parameter": "Device.Time.X_IOPSYS_EU_LocalTimeZoneOlson", "value": "Europe/Stockholm", "type": "xsd:string" }
{ "parameter": "Device.Time.X_IOPSYS_EU_SourceInterface", "value": "", "type": "xsd:string" }
root@iopsys:~#
```

**NOTE: It's the same, whether to use icwmp script or icwmpd command for connecting to datamodel**

```
root@iopsys:~# icwmpd -m 1 inform
{ "parameter": "Device.DeviceInfo.HardwareVersion", "value": "DG400PRIMEA", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.Manufacturer", "value": "iopsys", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ManufacturerOUI", "value": "201F31", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ModelName", "value": "DG400PRIMEA", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ProductClass", "value": "DG400PRIME", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ProvisioningCode", "value": "", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.SerialNumber", "value": "J814001008", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.SoftwareVersion", "value": "DG400PRIME-A-IOPSYS-4.4.0RC1-191205_1004", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.SpecVersion", "value": "1.0", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.AliasBasedAddressing", "value": "false", "type": "xsd:boolean" }
{ "parameter": "Device.ManagementServer.ConnReqJabberID", "value": "", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.ConnReqXMPPConnection", "value": "Device.XMPP.Connection.", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.ConnectionRequestURL", "value": "http://192.168.117.45:7547/", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.ParameterKey", "value": "", "type": "xsd:string" }
root@iopsys:~# 
root@iopsys:~# icwmp inform
{ "parameter": "Device.DeviceInfo.HardwareVersion", "value": "DG400PRIMEA", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.Manufacturer", "value": "iopsys", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ManufacturerOUI", "value": "201F31", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ModelName", "value": "DG400PRIMEA", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ProductClass", "value": "DG400PRIME", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.ProvisioningCode", "value": "", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.SerialNumber", "value": "J814001008", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.SoftwareVersion", "value": "DG400PRIME-A-IOPSYS-4.4.0RC1-191205_1004", "type": "xsd:string" }
{ "parameter": "Device.DeviceInfo.SpecVersion", "value": "1.0", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.AliasBasedAddressing", "value": "false", "type": "xsd:boolean" }
{ "parameter": "Device.ManagementServer.ConnReqJabberID", "value": "", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.ConnReqXMPPConnection", "value": "Device.XMPP.Connection.", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.ConnectionRequestURL", "value": "http://192.168.117.45:7547/", "type": "xsd:string" }
{ "parameter": "Device.ManagementServer.ParameterKey", "value": "", "type": "xsd:string" }
root@iopsys:~# 
```

## Dependencies ##

To successfully build icwmp, the following libraries are needed:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |
| libubox     | https://git.openwrt.org/project/libubox.git | BSD            |
| libubus     | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| libjson-c   | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libopenssl  | http://ftp.fi.muni.cz/pub/openssl/source/   | OpenSSL        |
| libcurl     | https://dl.uxnr.de/mirror/curl              | MIT            |
| libbbfdm    | https://dev.iopsys.eu/iopsys/bbf.git        | LGPL 2.1       |
| libmicroxml | https://dev.freecwmp.org/microxml           | LGPL 2.0       |
| libpthread  |                                             |                |

