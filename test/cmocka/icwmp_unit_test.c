#include <stdio.h>
#include "icwmp_unit_test.h"

int main()
{
	int ret = 0;

	ret += icwmp_notifications_test();
	ret += icwmp_cli_unit_test();
	ret += icwmp_custom_inform_test();
	ret += icwmp_soap_msg_test();
	ret += icwmp_uci_test();
	ret += icwmp_datamodel_interface_test();
	ret += icwmp_backup_session_test();
	ret += icwmp_download_unit_test();

	return ret;
}
