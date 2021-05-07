#ifndef CWMP_DOWNLOAD_H
#define CWMP_DOWNLOAD_H

#define DOWNLOAD_PROTOCOL_HTTP "http://"
#define DOWNLOAD_PROTOCOL_HTTPS "https://"
#define DOWNLOAD_PROTOCOL_FTP "ftp://"
#define MAX_DOWNLOAD_QUEUE 10

extern struct list_head list_download;
extern struct list_head list_schedule_download;
extern struct list_head list_apply_schedule_download;

extern pthread_mutex_t mutex_download;
extern pthread_cond_t threshold_download;
extern pthread_mutex_t mutex_schedule_download;
extern pthread_cond_t threshold_schedule_download;
extern pthread_mutex_t mutex_apply_schedule_download;
extern pthread_cond_t threshold_apply_schedule_download;

extern int count_download_queue;

int cwmp_free_download_request(struct download *download);
int cwmp_free_schedule_download_request(struct download *schedule_download);
int cwmp_free_apply_schedule_download_request(struct apply_schedule_download *apply_schedule_download);
int cwmp_scheduledDownload_remove_all();
int cwmp_scheduled_Download_remove_all();
int cwmp_apply_scheduled_Download_remove_all();
int cwmp_rpc_acs_destroy_data_transfer_complete(struct session *session, struct rpc *rpc);
void *thread_cwmp_rpc_cpe_download(void *v);
void *thread_cwmp_rpc_cpe_schedule_download(void *v);
void *thread_cwmp_rpc_cpe_apply_schedule_download(void *v);
#endif
