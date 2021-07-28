# CWMP Agent

icwmp is a client implementation of [TR-069/CWMP](https://cwmp-data-models.broadband-forum.org/) protocol.

It is written in C programming language and depends on a number of libraries of OpenWrt for building and running.

## Good to Know

The icwmp client is :
* tested with several ACS such as **Axiros**, **AVSytem**, **GenieACS**, **OpenACS**, etc...
* supports all required **TR069 RPCs**.
* supports all DataModel of TR family such as **TR-181**, **TR-104**, **TR-143**, **TR-157**, etc...
* supports all types of connection requests such as **HTTP**, **XMPP**, **STUN**.
* supports integrated file transfer such as **HTTP**, **HTTPS**, **FTP**.

## Configuration File

The icwmp UCI configuration is located in **'/etc/config/cwmp'**, and contains 3 sections: **'acs'**, **'cpe'** and **'lwn'**.

```bash
config acs 'acs'
	option userid 'iopsys'
	option dhcp_discovery 'enable'
	option compression 'Disabled'
	option retry_min_wait_interval '5'
	option retry_interval_multiplier '2000'

config cpe 'cpe'
	option interface 'eth0.1'
	option default_wan_interface 'wan'
	option userid 'iopsys'
	option exec_download '0'
	
config lwn 'lwn'
	option enable '1'
	option hostname ''
	option port ''
```

For more info on the `cwmp` UCI configuration see [link](./docs/api/uci.cwmp.md) or [raw schema](./schemas/uci/cwmp.json)

## RPCs Method supported

the folowing tables provides a summary of all methods, and indicates the conditions under which implementation of each RPC method defined in Annex A is `REQUIRED` or `OPTIONAL`.

### Methods for CPE responding

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


### Methods for CPE calling

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


## Concepts and Workflow

As indicated in the TR069 standard, the icwmpd starts automatically when the system is started. Then it connects to the ACS, that can be set manually by the admin or found by dhcp discovery. And later  it could start other sessions due to event causes.

Session workflow could be checked with sniffer packets tool such as wireshark or tcpdump.
In addition icwmpd has a log file '/var/log/icwmpd.log', that describes the workflow. E.g. below you can find an abstract of a log file content:

```bash
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

## icwmp uBus

icwmpd must be launched on startup after ubusd. It exposes the CWMP functionality over ubus. The icwmpd registers `tr069` namespaces with ubus, that has the shown below functionalities:

For more info on the `tr069` ubus schema see [link](./docs/api/tr069.md) or [raw schema](./schemas/ubus/tr069.json)

### tr069 ubus examples
```bash
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

```bash
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

- To trigger a new session to ACS with the event `'6 CONNECTION REQUEST'` or `'8 DIAGNOSTICS COMPLETE'`, etc.., use the `inform` ubus method with the appropriate `event` argument:

```bash
root@iopsys:~# ubus call tr069 inform '{"event":"6 connection request"}'
{
	"status": 1,
	"info": "Session started"
}
root@iopsys:~#
root@iopsys:~# ubus call tr069 inform '{"event":"8 diagnostics complete"}'
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

- To reload the icwmpd config, use the `command` ubus method with `reload` argument:

```bash
root@iopsys:~# ubus call tr069 command '{"command":"reload"}'
{
	"status": 1,
	"info": "icwmpd config reloaded"
}
root@iopsys:~# 
```
- To exit the icwmpd daemod, use the `command` ubus method with `exit` argument:

```bash
root@iopsys:~# ubus call tr069 command '{"command":"exit"}'
{
	"status": 1,
	"info": "icwmpd daemon stopped"
}
root@iopsys:~# 
```
## icwmpd command line

icwmpd command line options are described with `--help` option as below:

```bash
root@iopsys:~# icwmpd --help
Usage: icwmpd [OPTIONS]
 -s, --ubus_socket                                    Ubus socket path for IPC
 -b, --boot-event                                    (CWMP daemon) Start CWMP with BOOT event
 -g, --get-rpc-methods                               (CWMP daemon) Start CWMP with GetRPCMethods request to ACS
 -c, --cli                              	     CWMP CLI
 -h, --help                                          Display this help text
 -v, --version                                       Display the version
```

## icwmpd CLI

The icwmpd CLI is the -c (--cli) option of the icwmpd command line. 

Different options of this CLI are described with help command as below:

```bash
root@iopsys:~# icwmpd -c help
Valid commands:
	help 									=> show this help
	get [path-expr] 						=> get parameter values
	get_names [path-expr] [next-level] 		=> get parameter names
	set [path-expr] [value] 				=> set parameter value
	add [object] 							=> add object
	del [object] 							=> delete object
	get_notif [path-expr]					=> get parameter notifications
	set_notif [path-expr] [notification]	=> set parameter notifications

```

## icwmpd forced inform parameters

In addition to the forced inform parameters specified in datamodel stanadard, icwmp gives the possibility to add other inform parameters.
Those new inform parameters can be set in a json file as following:

```bash
{
  "forced_inform":[
    "Device.DeviceInfo.X_IOPSYS_EU_BaseMACAddress",
    "Device.DeviceInfo.UpTime"
    ]
}
```

The path of the json file is set in the uci option: cwmp.cpe.forced_inform_json

Please be sure that this json_file shouldn't contain duplicate parameters or parameters of the standard inform parameters specified in the datamodel.

## Dependencies

To successfully build icwmp, the following libraries are needed:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |
| libubox     | https://git.openwrt.org/project/libubox.git | BSD            |
| libubus     | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| libjson-c   | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libopenssl  | http://ftp.fi.muni.cz/pub/openssl/source/   | OpenSSL        |
| libcurl     | https://dl.uxnr.de/mirror/curl              | MIT            |
| libmicroxml | https://dev.freecwmp.org/microxml           | LGPL 2.0       |
