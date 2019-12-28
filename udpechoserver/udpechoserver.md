# README #

icwmp_udpechoserverd is an implementation of udp echo server to perform the UDP Echo Service and UDP Echo Plus Service.

## Configuration File ##

The icwmp_udpechoserverd UCI configuration is located in **'/etc/config/cwmp\_udpechoserver'** and contains only one section: **udpechoserver**.

```
config udpechoserver 'udpechoserver'
	option enable '0'
	option interface ''
	option address ''
	option server_port ''
	option plus '0'
	option log_level '3'
```

### udpechoserver section ###

It defines **the udpechoserver configuration** such as enable, interface, address, etc... Possible options to be used in **the udpechoserver** section are listed in the table below.

| Name          |  Type   | Description                                       |
| ------------- | ------- | ------------------------------------------------- |
| `enable`      | boolean | If set to **1**, UDP Echo Server will be enabled. |
| `interface`   | string  | Specifies the interface on which the CPE MUST listen and receive UDP echo requests. |
| `address`     | string  | Specifies the source IP address which can make an UDP echo requests. |
| `server_port` | integer | The UDP port on which the UDP Echo server MUST listen and respond to UDP echo requests. |
| `plus`        | boolean | If set to **1**, the CPE will perform necessary packet processing for UDP Echo Plus packets. |
| `log_level`   | integer | Specifies the log type to use, by default it is set to **'INFO'**. The possible types are **'EMERG', 'ALERT', 'CRITIC' ,'ERROR', 'WARNING', 'NOTICE', 'INFO' and 'DEBUG'**. |


## Dependencies ##

To successfully build icwmp_udpechoserverd, the following libraries are needed:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |

