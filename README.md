# CWMP Agent

icwmp is a client implementation of [TR-069/CWMP](https://cwmp-data-models.broadband-forum.org/) protocol.

It is written in C programming language and depends on a number of libraries of OpenWrt for building and running.

## Good to Know

The icwmp client is :
* Tested with several ACS such as **Axiros**, **AVSytem**, **GenieACS**, **OpenACS**, etc...
* Supports all required **TR069 RPCs**.
* Supports all DataModel of TR family such as **TR-181**, **TR-104**, **TR-143**, **TR-157**, etc...
* Supports all types of connection requests such as **HTTP**, **XMPP**, **STUN**.
* Supports integrated file transfer such as **HTTP**, **HTTPS**, **FTP**.

## Configuration File

The `icwmp` UCI configuration is located in **'/etc/config/cwmp'**, and contains 3 sections: **'acs'**, **'cpe'** and **'lwn'**.

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

> Note: icwmp depends on usp.raw for all datamodel parameters, some `DeviceId` related parameters can be overwritten by writing them directly on `/etc/config/cwmp` file.

```bash
uci set cwmp.cpe.manufacturer="ABC"
uci set cwmp.cpe.manufacturer_oui="XXX"
uci set cwmp.cpe.product_class="TEST_CLASS"
uci set cwmp.cpe.serial_number="1234567890"
uci set cwmp.cpe.software_version="X.Y.Z"
uci set cwmp.cpe.model_name="MODELXXX"
uci set cwmp.cpe.description="This is a test device"
uci commit cwmp
```
> Complete UCI for `cwmp` configuration available in [link](./docs/api/uci.cwmp.md) or [raw schema](./schemas/uci/cwmp.json)

## RPCs Method supported

The following tables provides a summary of all methods, and indicates the conditions under which implementation of each RPC method defined in Annex A is `REQUIRED` or `OPTIONAL`.

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

As indicated in the TR069 standard, the `icwmpd` starts automatically when the system is started. Then it reads the initial configuration from UCI and if configured connects to the ACS. ACS configuration in `icwmp` can be done manually by the admin using UCI or by operator using DHCP Option 43, and later it could start other sessions due to event causes.

Session workflow could be checked with sniffer packets tool such as Wireshark or `tcpdump`.
In addition `icwmpd` has a log file '/var/log/icwmpd.log', that describes the workflow. E.g. below you can find an abstract of a log file content:

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

You could set the UCI config `cwmp.cpe.log_severity` option to `'DEBUG'` in order to show in details the cwmp log.

## icwmp uBus

`icwmpd` must be launched on startup after `ubusd`. It exposes some CWMP client RPC along with some debug utilities over UBUS. The `icwmpd` registers `tr069` namespaces with UBUS, that has below functionalities:

> Note: For more info on the `tr069` ubus schema see [link](./docs/api/tr069.md) or [raw schema](./schemas/ubus/tr069.json)

### tr069 ubus examples
The output shown in below examples are just for demonstration purpose, the actual output shall vary as per the cwmp configuration and state. The schema for UBUS is available at [link](./docs/api/tr069.md) or [raw schema](./schemas/ubus/tr069.json)

```bash
root@iopsys:~# ubus -v list tr069
'tr069' @aadff65c
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
                "start_time": "2021-07-29T09:29:02+02:00",
                "acs_url": "http://genieacs:7547"
        },
        "last_session": {
                "status": "success",
                "start_time": "2021-07-29T09:29:59+02:00",
                "end_time": "2021-07-29T09:30:00+02:00"
        },
        "next_session": {
                "status": "waiting",
                "start_time": "2021-07-29T09:59:59+02:00",
                "end_time": "N/A"
        },
        "statistics": {
                "success_sessions": 2,
                "failure_sessions": 0,
                "total_sessions": 2
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
- To exit the icwmpd daemon, use the `command` ubus method with `exit` argument:

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
 -b, --boot-event                                    (CWMP daemon) Start CWMP with BOOT event
 -g, --get-rpc-methods                               (CWMP daemon) Start CWMP with GetRPCMethods request to ACS
 -c, --cli                                           CWMP CLI
 -h, --help                                          Display this help text
 -v, --version                                       Display the version
```

## icwmpd CLI

The icwmpd CLI is a debug utility and can be invoked using -c (--cli) command line option.

Different options of this CLI are described with help command as below:

```bash
root@iopsys:~# icwmpd -c help
Valid commands:
        help                                    => show this help
        get [path-expr]                         => get parameter values
        get_names [path-expr] [next-level]      => get parameter names
        set [path-expr] [value]                 => set parameter value
        add [object]                            => add object
        del [object]                            => delete object
        get_notif [path-expr]                   => get parameter notifications
        set_notif [path-expr] [notification]    => set parameter notifications
```
> Note: icwmpd CLI is a debug utility and hence it is advised to use for debug and development purpose only.
> icwmpd CLI utility is independent of icwmpd daemon.

icwmp CLI command success result is displayed in the terminal as following:

```bash
root@iopsys:~# icwmpd -c get Device.DeviceInfo.UpTime
Device.DeviceInfo.UpTime => 91472
root@iopsys:~# icwmpd -c set Device.WiFi.SSID.1.SSID wifi1_ssid
Set value is successfully done
Device.WiFi.SSID.1.SSID => wifi1_ssid
```
In the case of fault the result is displayed as following:

```bash
root@iopsys:~# icwmpd -c get Device.DeviceInfo.UpTme
Fault 9005: Invalid parameter name
root@iopsys:~# icwmpd -c set 
Fault 9003: Invalid arguments
root@iopsys:~# icwmpd -c set Device.WiFi.SSID.1.SSID
Fault 9003: Invalid arguments
```
## SPV Response and restart services
In case icwmp receives from the ACS SetParameterValues Request, it will use the uspd setm_values ubus method for all requested parameters.

Basing on setm_values response the icwmp will do the following:

- in case of fault icwmp aborts the set of all parameters and then sends Response to the ACS with FAULT 9003 including all parameters faults as defined in TR069 standard.
- in case of success icwmp commits the set of all parameters, without applying them so without restarting services, and then sends a success Response to the ACS including the status code 1.

> - All restart services are done in the CWMP end session in order to prevent any session interruption.
> - icwmp always returns 1 as status value in case of success SPV because all restart services are done in the end session.
 
## icwmpd forced inform parameters
As per the cwmp inform requirements, cwmp client has list of parameters defined internally. The list contains below parameters:

| Parameter name                                 |
| ---------------------------------------------- |
| Device.RootDataModelVersion                    |
| Device.DeviceInfo.HardwareVersion              |
| Device.DeviceInfo.SoftwareVersion              |
| Device.DeviceInfo.ProvisioningCode             |
| Device.ManagementServer.ParameterKey           |
| Device.ManagementServer.ConnectionRequestURL   |
| Device.ManagementServer.AliasBasedAddressing   |

In addition to the above defined forced inform parameters as specified in datamodel standard, icwmp gives the possibility to add other datamodel parameters as forced inform parameters, by defining them in a JSON file.

Additional inform parameters can be configured in a JSON file as below:

```bash
root@iopsys:~# cat /etc/icwmpd/inform.json
{
  "forced_inform":[
    "Device.DeviceInfo.X_IOPSYS_EU_BaseMACAddress",
    "Device.DeviceInfo.UpTime"
    ]
}
root@iopsys:~#
```
And then the path of the JSON file can be set in the UCI option: `cwmp.cpe.forced_inform_json` like below:

```bash
root@iopsys:~# uci set cwmp.cpe.forced_inform_json=/etc/icwmpd/inform.json
root@iopsys:~# uci commit cwmp
root@iopsys:~# /etc/init.d/icwmpd restart
```

> - It is required to restart icwmp service after the changes to use the new forced inform parameters    
> - This JSON file shouldn't contain duplicate parameters or parameters of the standard inform parameters specified in the datamodel    
> - Forced inform parameters defined in JSON should be leaf elements

## Boot inform parameters
In addition to the above defined forced inform parameters as specified in datamodel standard and forced inform parameters specified by the customer in a json file (defined in previous section), icwmp gives also possibility to add Boot Inform parameter by defining them in a JSON file.

Boot inform parameters will appear in inform messages that includes '0 BOOTSTRAP' or '1 BOOT' events.

inform parameters can be configured in a JSON file as below:

```bash
root@iopsys:~# cat /etc/icwmpd/inform.json
{
  "boot_inform":[
    "Device.DeviceInfo.UpTime"
    ]
}
root@iopsys:~#
```
And then the path of the JSON file can be set in the UCI option: `cwmp.cpe.boot_inform_json` like below:

```bash
root@iopsys:~# uci set cwmp.cpe.boot_inform_json=/etc/icwmpd/inform.json
root@iopsys:~# uci commit cwmp
root@iopsys:~# /etc/init.d/icwmpd restart
```
> - It is required to restart icwmp service after the changes to use the new boot inform parameters    
> - This JSON file shouldn't contain duplicate parameters or parameters of the standard inform parameters specified in the datamodel    
> - Boot inform parameters defined in JSON should be leaf elements
> - Boot inform parameters appears only in BOOT or BOOTSTRAP inform message.

## icwmpd notifications
As per the cwmp notifications requirements, there is a list parameters specified in the standard that has forced notification type. Those parameters are defined internally in icwmp client. The list contains below parameters:

| Parameter name                        | Notification  |
| ------------------------------------- |---------------|
| Device.DeviceInfo.SoftwareVersion     |	2			|
| Device.DeviceInfo.ProvisioningCode    |	2			|

According to the standard, if the ACS set new notification of one of those parameters so icwmp will return the CWMP fault 9009.

In addition to thos parameters, icwmp gives the possibility to set default notifications of some parameters that the customer choose using a json file, in order to permit icwmp to start with those parameters with default notification type.
The json file must respect following form:

```bash
root@iopsys:~# cat /etc/icwmpd/inform.json
{
  "custom_notification": [
    {
      "parameter": "Device.Users.",
      "notify_type": "2"
    },
    {
      "parameter": "Device.WiFi.SSID.1.SSID",
      "notify_type": "1"
    }
  ]
}
```

The location of this file should be specified in the uci config option: cwmp.cpe.custom_notify_json
```bash
root@iopsys:~# uci set cwmp.cpe.custom_notify_json=/etc/icwmpd/inform.json
root@iopsys:~# uci commit cwmp
root@iopsys:~# /etc/init.d/icwmpd restart
```

Contrary to forced parameters notifications, the ACS has privileges to set new notification type of those custom parameters.

>- The actual update doesn't support wildcard parameters. So parameter like Device.WiFi.SSID.*. will be skipped
>- The actual update doesn't support multiple json notify files

## Dependencies

To successfully build icwmp, the following libraries are needed:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |
| libubox     | https://git.openwrt.org/project/libubox.git | BSD            |
| libubus     | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| libjson-c   | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libwolfssl  | https://github.com/wolfSSL/wolfssl          | GPL-2.0        |
| libcurl     | https://dl.uxnr.de/mirror/curl              | MIT            |
| libmicroxml | https://dev.freecwmp.org/microxml           | LGPL 2.0       |

Runtime dependencies:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| ubus        | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| bbf         | https://dev.iopsys.eu/iopsys/bbf.git        | LGPLv2.1       |
| uspd        | https://dev.iopsys.eu/iopsys/uspd.git       | GPL v2.0       |

> icwmpd gets the datamodel from the DUT via ubus using uspd, and also it registers `tr069` ubus namespace to expose some debug and cwmp client rpc funtionalities, so it is required to start it after starting `ubusd` and `uspd`.
