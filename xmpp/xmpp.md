# README #

In order to reach the devices that are connected behind NAT, the cwmp protocol introduces alternative method of executing Connection Request via NAT based on XMPP. The icwmp_xmppd is an implementation of XMPP functionality that performs this feature.

## Configuration File ##

The icwmp_xmppd UCI configuration is located in **'/etc/config/cwmp\_xmpp'**, and contains 3 sections: **xmpp**, **xmpp\_connection** and **xmpp\_connection\_server**.

```
config cwmp 'xmpp'
	option enable '0'
	option id '0'
	option allowed_jid ''
	option loglevel '3'

config xmpp_connection
	option enable '0'
	option username ''
	option password ''
	option domain 	''
	option resource ''
	option serveralgorithm 'DNS-SRV'
	
config xmpp_connection_server
	option id_connection '1'
	option connection_server_instance '1'
	option enable '0'
	option port '5222'
```

### cwmp xmpp section ###

It defines **the cwmp xmpp section configuration**: enable, id, etc... The possible options for **cwmp xmpp** section are listed in the table below.

| Name          |  Type   | Description                                   |
| --------------| ------- | --------------------------------------------- |
| `enable`      | boolean | If set to **1**, it enables the XMPP feature. |
| `id`          | integer | The id of XMPP connection. |
| `allowed_jid` | string  | The list of Jabber IDs or addresses that are allowed to initiate an XMPP Connection Request. |
| `loglevel`    | integer | The log type to use, by default it is set to **'INFO'**. The possible types are **'EMERG', 'ALERT', 'CRITIC' ,'ERROR', 'WARNING', 'NOTICE', 'INFO' and 'DEBUG'**. |

### xmpp_connection section ###

It defines the xmpp\_connection section configuration**: enable, connection\_instance, username, etc... The possible options for **xmpp\_connection** section are listed in the table below.

| Name                        |  Type   | Description                                           |
| --------------------------- | ------- | ----------------------------------------------------- |
| `connection_instance`       | integer | The instance number of the XMPP connection. |
| `enable`                    | boolean | If set to **1**, it enables the XMPP connection. |
| `connection_alias`          | string  | The alias of the XMPP connection. |
| `username`                  | string  | The username of the XMPP connection. |
| `password`                  | string  | The password of the XMPP connection. |
| `domain`                    | string  | The proposed domain-part of the Jabber ID of the XMPP connection. |
| `resource`                  | string  | The proposed resource-part of the Jabber ID of the XMPP connection. |
| `usetls`                    | boolean | If set to **1**, the CPE will initiate TLS negotiation. |
| `interval`                  | integer | The number of seconds, that keep alive events are sent by the XMPP connection. |
| `attempt`                   | string  | The number of times that the Connection attempts to connect to a given IP address. |
| `initial_retry_interval`    | integer | The maximum first reconnection wait interval in seconds. |
| `retry_interval_multiplier` | integer | The reconnection interval multiplier. |
| `retry_max_interval`        | integer | The maximum reconnection wait interval in seconds. |
| `serveralgorithm`           | string  | The algorithm used when connecting with the XMPP server. Two algorithms are supported: **'DNS-SRV' and 'ServerTable'**. |


### xmpp_connection_server section ###

It defines **the xmpp\_connection\_server section configuration**: id\_connection, enable, port, etc... The possible options for **xmpp\_connection\_server** section are listed in the table below.

| Name                         |  Type   | Description                    |
| ---------------------------- | ------- | ------------------------------ |
| `id_connection`              | string  | The id XMPP connection to use. |
| `connection_server_instance` | integer | The instance number of the XMPP server. |
| `enable`                     | boolean | If set to **1**, it enables this XMPP server. |
| `connection_server_alias`    | string  | The alias of the XMPP connection. |
| `port`                       | integer | The port of the XMPP connection. |
| `server_address`             | string  | The server address of the XMPP connection. |

## Dependencies ##

To successfully build icwmp, the following libraries are needed:

| Dependency  | Link                                    | License        |
| ----------- | --------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git | LGPL 2.1       |
| libstrophe  | https://github.com/strophe/libstrophe   | GPL 3.0        |

