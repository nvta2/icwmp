CWMP_VERSION = 3.0.0
bin_PROGRAMS = icwmpd

icwmpd_SOURCES =			\
	../common.c				\
	../cwmp_time.c			\
	../log.c				\
	../cwmp_uci.c			\
	../config.c				\
	../session.c			\
	../backupSession.c		\
	../md5.c				\
	../digestauth.c			\
	../event.c				\
	../http.c				\
	../netlink.c			\
	../ubus.c				\
	../datamodel_interface.c \
	../cwmp_cli.c			\
	../notifications.c		\
	../cwmp_zlib.c			\
	../cwmp_du_state.c		\
	../download.c			\
	../upload.c				\
	../sched_inform.c		\
	../xml.c				\
	../diagnostic.c			\
	../cwmp.c	

icwmpd_CFLAGS =			\
	$(AM_CFLAGS)		\
	$(LIBUCI_CFLAGS)	\
	$(LIBUBOX_CFLAGS)	\
	$(LIBUBUS_CFLAGS)	\
	$(MICROXML_CFLAGS)	\
	$(LIBCURL_CFLAGS) 	\
	-Wall -Wextra 		\
	-Werror -Wformat

icwmpd_LDFLAGS =		\
	$(AM_LDFLAGS)		\
	$(LIBUCI_LDFLAGS)	\
	$(LIBUBOX_LDFLAGS)	\
	$(LIBUBUS_LDFLAGS)	\
	$(MICROXML_LDFLAGS)	\
	$(LIBCURL_LDFLAGS)

icwmpd_LDADD =			\
	$(AM_LIBS)			\
	$(LIBUCI_LIBS)		\
	$(LIBUBOX_LIBS)		\
	$(LIBUBUS_LIBS)		\
	$(MICROXML_LIBS)	\
	$(LIBCURL_LIBS)		\
	$(LIBPTHREAD_LIBS)	\
	$(LCRYPTO_LIBS)		\
	$(LSSL_LIBS)		\
	$(LIBJSON_LIBS)		\
	$(LBLOBMSG_LIBS)	\
	$(LIBZ_LIBS)		\
	$(LIBM_LIBS)
	
icwmpd_CFLAGS+=-DCWMP_VERSION=\"$(CWMP_VERSION)\"
icwmpd_LDFLAGS+=-DCWMP_VERSION=\"$(CWMP_VERSION)\"

icwmpd_CFLAGS+=-I../inc