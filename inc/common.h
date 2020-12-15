#ifndef __COMMON_H
#define __COMMON_H
#include <libbbfdm/dmdiagnostics.h>
#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmbbfcommon.h>
enum cwmp_ret_err {
	CWMP_OK,			/* No Error */
	CWMP_GEN_ERR, 		/* General Error */
	CWMP_MEM_ERR,  		/* Memory Error */
	CWMP_MUTEX_ERR,
	CWMP_RETRY_SESSION
};

/*enum amd_version_enum {
	AMD_1 = 1,
	AMD_2,
	AMD_3,
	AMD_4,
	AMD_5,
};

enum instance_mode {
	INSTANCE_MODE_NUMBER,
	INSTANCE_MODE_ALIAS
};*/
#endif
#ifndef FREE
#define FREE(x) do { if (x) {free(x); x = NULL;} } while (0)
#endif
