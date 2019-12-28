# README #

In order to reach the devices that are connected behind NAT, the cwmp protocol introduces alternative method of executing Connection Request via NAT based on STUN. The icwmp_stund is an implementation of STUN functionality that performs this feature.

## Configuration File ##

The icwmp_stund UCI configuration is located in **'/etc/config/cwmp\_stun'** and contains only one section **stun**:

```
config stun 'stun'
	option username 'tr069_stun'
	option password 'tr069_stun'
	option server_address 'stun.l.google.com'
	option server_port '19302'
	option min_keepalive '30'
	option max_keepalive '3600'
	option client_port 7547
	option log_level '1'
```

### stun section ###

It defines **the stun section configuration** (like username, password, etc...). The possible options for **stun** section are listed in the table below.

| Name             |  Type   | Description                                       |
| ---------------- | ------- | ------------------------------------------------- |
| `username`       | string  | The STUN username to be used in Binding Requests. |
| `password`       | string  | The STUN Password to be used in computing the MESSAGE-INTEGRITY. |
| `server_address` | string  | The host name or IP address of the STUN server to send Binding Requests. |
| `server_port`    | integer | The port number of the STUN server to send Binding Requests. |
| `min_keepalive`  | integer | The minimum period that STUN Binding Requests must be sent by the CPE for the purpose of maintaining the binding in the Gateway. |
| `max_keepalive`  | integer | The maximum period that STUN Binding Requests must be sent by the CPE for the purpose of maintaining the binding in the Gateway. |
| `client_port`    | integer | The client source port of the STUN UDP binding. |
| `log_level`      | integer | The log type to use, by default it is set to **'INFO'**. The possible types are **'EMERG', 'ALERT', 'CRITIC' ,'ERROR', 'WARNING', 'NOTICE', 'INFO' and 'DEBUG'**. |

## Dependencies ##

To successfully build icwmp_stund, the following libraries are needed:

| Dependency      | Link                                        | License        |
| --------------- | ------------------------------------------- | -------------- |
| libuci          | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |
| libubox         | https://git.openwrt.org/project/libubox.git | BSD            |
| libubus         | https://git.openwrt.org/project/ubus.git    | LGPL 2.1       |
| libjson-c       | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libopenssl      | http://ftp.fi.muni.cz/pub/openssl/source/   | OpenSSL        |

