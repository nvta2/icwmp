# README #

icwmp_bulkdatad is an implementation of The HTTP bulk data collection mechanism which is an extended feature of CPE and other agents implementing TR-069(CWMP) or TR-369(USP), defined by the Broadband Forum. It provides a means by which an Auto-Configuration Server (ACS), or USP Controller, can configure an agent to periodically send a JSON or CSV formatted set of Device information to an HTTP server running a data collection application.

## Configuration File ##

The icwmp_bulkdatad UCI configuration is located in **'/etc/config/cwmp\_bulkdata'**, and contains 4 sections: **bulkdata**, **profile**, **profile\_parameter** and **profile\_http\_request\_uri\_parameter**.

```
config bulkdata 'bulkdata'
	option enable '0'
	option log_level '3'

config profile
	option profile_id '1'
	option enable '0'
	option csv_encoding_row_time_stamp 'unix'
	option json_encoding_report_time_stamp 'unix'
	option http_retry_minimum_wait_interval '5'
	option http_retry_interval_multiplier '2000'

config profile_parameter
	option profile_id '1'
	option name ''
	option reference ''

config profile_http_request_uri_parameter
	option profile_id '1'
	option name ''
	option reference ''
```

### bulkdata section ###

It defines **bulkdata configuration**: enable and log\_level.

| Name        |  Type   | Description                                                                                     |
| ----------- | ------- | ----------------------------------------------------------------------------------------------- |
| `enable`    | boolean | Enables the BulkData feature if set to **1**.                                                     |
| `log_level` | integer | Specifies the log type to use, by default **'INFO'**. The possible types are **'EMERG', 'ALERT', 'CRITIC' ,'ERROR', 'WARNING', 'NOTICE', 'INFO' and 'DEBUG'**. |

### profile section ###

It defines **the profile section configuration**: enable, name,... The possible options for **profile** section are listed below:

| Name                               |  Type   | Description                                    |
| ---------------------------------- | ------- | ---------------------------------------------- |
| `profile_id`                       | integer | The profile id to use. |
| `enable`                           | boolean | If set to **1**, enables the bulkdata profile. |
| `alias`                            | string  | The alias of the profile. |
| `name`                             | string  | The name of the profile. |
| `nbre_of_retained_failed_reports`  | integer | The number of failed reports to be retained and transmitted at the end of the current reporting interval. |
| `protocol`                         | string  | The protocol used for the collection profile. |
| `encoding_type`                    | string  | The encoding type used for the collection profile. |
| `reporting_interval`               | integer | The reporting interval in seconds. |
| `time_reference`                   | integer | The time reference to determine when the profile will be transmitted to the ACS collector. |
| `csv_encoding_field_separator`     | string  | The field separator to use when encoding CSV data. |
| `csv_encoding_row_separator`       | string  | The row separator to use when encoding CSV data. |
| `csv_encoding_escape_character`    | string  | The escape character to use when encoding CSV data. |
| `csv_encoding_report_format`       | string  | Describes how reports will be formatted. Two possible formats are supported: **'ParameterPerRow' and 'ParameterPerColumn'**. |
| `csv_encoding_row_time_stamp`      | string  | The format of the timestamp to use for data inserted into the row. The row time stamp supported are **'Unix-Epoch', 'ISO-8601' and 'None'**. |
| `json_encoding_report_format`      | string  | Describes the report format. The supported report formats are **'ObjectHierarchy' and 'NameValuePair'**. |
| `json_encoding_report_time_stamp`  | string  | The format of the timestamp to use for the JSON Object named "CollectionTime". The supported timestamp are **'Unix-Epoch', 'ISO-8601' and 'None'**. |
| `http_url`                         | string  | The URL of the collection server. |
| `http_username`                    | string  | The username of the collection server. |
| `http_password`                    | string  | The password of the collection server. |
| `http_compression`                 | string  | The HTTP Compression mechanism used by the collection server. The supported compression mechanism are **'GZIP', 'Compress' and 'Deflate'**. |
| `http_method`                      | string  | The HTTP method used by the collection server. Two methods are supported: **'POST' and 'PUT'**. |
| `http_use_date_header`             | boolean | If set to **1**, the CPE encodes the HTTP Date Header. |
| `http_retry_enable`                | boolean | If set to **1**, the CPE retries unsuccessful attempts to transfer data. |
| `http_retry_minimum_wait_interval` | integer | The data transfer retry wait interval. |
| `http_retry_interval_multiplier`   | integer | The retry interval multiplier. |
| `http_persist_across_reboot`       | boolean | If set to **1**, failed data transfers must be persisted across reboots. |

### profile_parameter section ###

It defines **the profile\_parameter section configuration**: profile\_id, name, reference.

| Name         |  Type   | Description                             |
| ------------ | ------- | --------------------------------------- |
| `profile_id` | integer | The id of the used profile.             |
| `name`       | string  | The name of the profile parameter.      |
| `reference`  | string  | The reference of the profile parameter. |

### profile_http_request_uri_parameter section ###

It defines **the profile\_http\_request\_uri\_parameter section configuration**: profile\_id, name, reference.

| Name         |  Type   | Description                             |
| ------------ | ------- | --------------------------------------- |
| `profile_id` | integer | The id of the used profile.             |
| `name`       | string  | The name of the Request-URI parameter.  |
| `reference`  | string  | The reference of the profile parameter. |

## Dependencies ##

To successfully build icwmp_bulkdatad, the following libraries are needed:

| Dependency  | Link                                        | License        |
| ----------- | ------------------------------------------- | -------------- |
| libuci      | https://git.openwrt.org/project/uci.git     | LGPL 2.1       |
| libubox     | https://git.openwrt.org/project/libubox.git | BSD            |
| libjson-c   | https://s3.amazonaws.com/json-c_releases    | MIT            |
| libcurl     | https://dl.uxnr.de/mirror/curl              | MIT            |
| libbbfdm    | https://dev.iopsys.eu/iopsys/bbf.git        | LGPL 2.1       |

