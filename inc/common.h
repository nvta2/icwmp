#ifndef __COMMON_H
#define __COMMON_H

enum cwmp_ret_err {
	CWMP_OK,			/* No Error */
	CWMP_GEN_ERR, 		/* General Error */
	CWMP_MEM_ERR,  		/* Memory Error */
	CWMP_MUTEX_ERR,
	CWMP_RETRY_SESSION
};
#endif
#ifndef FREE
#define FREE(x) do { if (x) {free(x); x = NULL;} } while (0)
#endif
