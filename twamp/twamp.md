# README #

icwmp_twampd is an implementation of the Two-Way Active Measurement Protocol (TWAMP) reflector.


## Configuration File ##

The icwmp_twampd UCI configuration is located in **'/etc/config/cwmp\_twamp'**, and contains 2 sections the **twamp** and the **twamp\_reflector**.

```
config cwmp 'twamp'
	option id '1'
	option log_level '3'

config twamp_reflector
	option id '1'
	option enable ''
	option interface ''
	option device ''
	option ip_version ''
	option port ''
	option max_ttl ''
	option ip_list ''
	option port_list ''
```

### cwmp twamp section ###

It defines **the twamp configuration**: id, log_level. The possible options for **twamp** section are:

| Name      |  Type   | Description                                 |
| --------- | ------- | ------------------------------------------- |
| `id`        | integer | Specifies the id of TWAMP reflector to use. |
| `log_level` | integer | Specifies the log type to use, by default **'INFO'**. The possible types are **'EMERG', 'ALERT', 'CRITIC' ,'ERROR', 'WARNING', 'NOTICE', 'INFO' and 'DEBUG'**. |

### twamp_reflector section ###

It describes **the twamp reflector configuration**: id, ip\_version, etc... The possible options for **twamp_reflector** section are:

| Name       |  Type   | Description                    |
| ---------- | ------- | ------------------------------ |
| `id`         | integer | Specifies the TWAMP id to use. |
| `enable`     | boolean | If set to **1**, it enables the TWAMP reflector. |
| `ip_version` | integer | Specifies the IP version to use, **4** by default. The possible versions are **4** and **6**. |
| `port`       | integer | Specifies the port to listen on. |
| `max_ttl`    | integer | Specifies the maximum TTL of a received packet, that the TWAMP reflector will reflect to the TWAMP controller. |
| `ip_list`    | string  | Specifies the allowed source IP addresses and subnets to handle test packets. |
| `port_list`  | string  | Specifies the range of source ports allowed to use for twamp\_reflector tests. |

## Dependencies ##

To successfully build icwmp_twampd, the following libraries are needed:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |

