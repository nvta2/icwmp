CWMP_VERSION = 3.0.0
bin_PROGRAMS = icwmpd

icwmpd_SOURCES =			\
	../backupSession.c		\
	../config.c				\
	../cwmp.c				\
	../digestauth.c			\
	../event.c				\
	../external.c			\
	../http.c				\
	../jshn.c				\
	../log.c				\
	../netlink.c			\
	../time.c				\
	../ubus.c				\
	../xml.c				\
	../diagnostic.c			\
	../zlib.c

icwmpd_CFLAGS =			\
	$(AM_CFLAGS)		\
	$(LIBUCI_CFLAGS)	\
	$(LIBUBOX_CFLAGS)	\
	$(LIBUBUS_CFLAGS)	\
	$(MICROXML_CFLAGS)	\
	$(LIBCURL_CFLAGS)	\
	$(LIBZSTREAM_CFLAGS)

icwmpd_LDFLAGS =		\
	$(AM_LDFLAGS)		\
	$(LIBUCI_LDFLAGS)	\
	$(LIBUBOX_LDFLAGS)	\
	$(LIBUBUS_LDFLAGS)	\
	$(MICROXML_LDFLAGS)	\
	$(LIBCURL_LDFLAGS)	\
	$(LIBZSTREAM_LDFLAGS)

icwmpd_LDADD =			\
	$(AM_LIBS)			\
	$(LIBUCI_LIBS)		\
	$(LIBUBOX_LIBS)		\
	$(LIBUBUS_LIBS)		\
	$(MICROXML_LIBS)	\
	$(LIBCURL_LIBS)		\
	$(LIBZSTREAM_LIBS)	\
	$(LIBPTHREAD_LIBS)	\
	$(LCRYPTO_LIBS)		\
	$(LSSL_LIBS)		\
	$(LIBJSON_LIBS)		\
	$(LBLOBMSG_LIBS)	\
	$(LIBZ_LIBS)		\
	$(LIBM_LIBS) 		\
	-lbbfdm				\
	-lbbf_api

icwmpd_CFLAGS+=-DCWMP_VERSION=\"$(CWMP_VERSION)\"
icwmpd_LDFLAGS+=-DCWMP_VERSION=\"$(CWMP_VERSION)\"

CWMP_BKP_FILE=/etc/icwmpd/.icwmpd_backup_session.xml
icwmpd_CFLAGS+=-DCWMP_BKP_FILE=\"$(CWMP_BKP_FILE)\"
icwmpd_CFLAGS+=-I../inc

if ICWMP_TR098
bin_PROGRAMS += icwmp_tr098d

icwmp_tr098d_SOURCES =			\
	../backupSession.c		\
	../config.c				\
	../cwmp.c				\
	../digestauth.c			\
	../event.c				\
	../external.c			\
	../http.c				\
	../jshn.c				\
	../log.c				\
	../netlink.c			\
	../time.c				\
	../ubus.c				\
	../xml.c				\
	../diagnostic.c			\
	../zlib.c

icwmp_tr098d_CFLAGS =			\
	$(AM_CFLAGS)		\
	$(LIBUCI_CFLAGS)	\
	$(LIBUBOX_CFLAGS)	\
	$(LIBUBUS_CFLAGS)	\
	$(MICROXML_CFLAGS)	\
	$(LIBCURL_CFLAGS)	\
	$(LIBZSTREAM_CFLAGS) 

icwmp_tr098d_LDFLAGS =		\
	$(AM_LDFLAGS)		\
	$(LIBUCI_LDFLAGS)	\
	$(LIBUBOX_LDFLAGS)	\
	$(LIBUBUS_LDFLAGS)	\
	$(MICROXML_LDFLAGS)	\
	$(LIBCURL_LDFLAGS)	\
	$(LIBZSTREAM_LDFLAGS)

icwmp_tr098d_LDADD =			\
	$(AM_LIBS)			\
	$(LIBUCI_LIBS)		\
	$(LIBUBOX_LIBS)		\
	$(LIBUBUS_LIBS)		\
	$(MICROXML_LIBS)	\
	$(LIBCURL_LIBS)		\
	$(LIBZSTREAM_LIBS)	\
	$(LIBPTHREAD_LIBS)	\
	$(LCRYPTO_LIBS)		\
	$(LSSL_LIBS)		\
	$(LIBJSON_LIBS)		\
	$(LBLOBMSG_LIBS)	\
	$(LIBZ_LIBS)		\
	$(LIBM_LIBS)		\
	-ltr098

icwmp_tr098d_CFLAGS+=-DCWMP_VERSION=\"$(CWMP_VERSION)\"
icwmp_tr098d_CFLAGS+=-DTR098
icwmp_tr098d_LDFLAGS+=-DCWMP_VERSION=\"$(CWMP_VERSION)\"

CWMP_BKP_FILE=/etc/icwmpd/.icwmpd_backup_session.xml
icwmp_tr098d_CFLAGS+=-DCWMP_BKP_FILE=\"$(CWMP_BKP_FILE)\"
icwmp_tr098d_CFLAGS+=-I../inc
endif

if ICWMP_STUN
bin_PROGRAMS += icwmp_stund

icwmp_stund_SOURCES =		\
	../stun/stun.c			\
	../stun/stun.h			\
	../stun/log.c			\
	../stun/log.h			\
	../stun/ubus.c			\
	../stun/ubus.h			\
	../stun/config.c 		\
	../stun/config.h

icwmp_stund_CFLAGS =			\
	$(AM_CFLAGS)				\
	$(LIBUCI_CFLAGS)			\
	$(LIBUBUS_CFLAGS)			\
	$(LIBUBOX_CFLAGS)

icwmp_stund_LDFLAGS =			\
	$(AM_LDFLAGS)				\
	$(LIBUCI_LDFLAGS)			\
	$(LIBUBUS_LDFLAGS)			\
	$(LIBUBOX_LDFLAGS)			\
	$(LIBJSONC_LDFLAGS)			

icwmp_stund_LDADD =				\
	$(AM_LIBS)					\
	$(LIBUCI_LIBS)				\
	$(LIBUBOX_LIBS)				\
	$(LIBJSON_LIBS)			    \
	$(LBLOBMSG_LIBS)			\
	$(LIBUBUS_LIBS)				\
	$(LCRYPTO_LIBS)
endif

if ICWMP_XMPP
bin_PROGRAMS += icwmp_xmppd

icwmp_xmppd_SOURCES =		    \
	../xmpp/xmpp.c				\
	../xmpp/xmpp.h				\
	../xmpp/xmppuci.c			\
	../xmpp/xmppuci.h			\
	../xmpp/xmppcmd.c			\
	../xmpp/xmppcmd.h			\
	../xmpp/xmpplog.c			\
	../xmpp/xmpplog.h			\
	../xmpp/xmppconfig.c		\
	../xmpp/xmppconfig.h

icwmp_xmppd_CFLAGS =			\
	$(AM_CFLAGS)				\
	$(LIBUCI_CFLAGS) 			\
	-Wall -Werror

icwmp_xmppd_LDFLAGS =			\
	$(AM_LDFLAGS)				\
	$(LIBUCI_LDFLAGS)
	
icwmp_xmppd_LDADD =				\
	$(AM_LIBS)					\
	$(LIBUCI_LIBS)				\
	$(LIBSTROPHE_LIBS)
endif

if ICWMP_UDPECHOSERVER
bin_PROGRAMS += icwmp_udpechoserverd

icwmp_udpechoserverd_SOURCES =		    		\
	../udpechoserver/udpechoserver.c			\
	../udpechoserver/udpechoserver.h			\
	../udpechoserver/udpechoserveruci.c			\
	../udpechoserver/udpechoserveruci.h			\
	../udpechoserver/udpechoserverlog.c			\
	../udpechoserver/udpechoserverlog.h

icwmp_udpechoserverd_CFLAGS =			\
	$(AM_CFLAGS)				\
	$(LIBUCI_CFLAGS)

icwmp_udpechoserverd_LDFLAGS =			\
	$(AM_LDFLAGS)				\
	$(LIBUCI_LDFLAGS)
	
icwmp_udpechoserverd_LDADD =				\
	$(AM_LIBS)					\
	$(LIBUCI_LIBS)
endif

if ICWMP_TWAMP
bin_PROGRAMS += icwmp_twampd

icwmp_twampd_SOURCES =		    \
	../twamp/twamp.c			\
	../twamp/twamp.h			\
	../twamp/twamptimestamp.c	\
	../twamp/twampuci.c			\
	../twamp/twampuci.h			\
	../twamp/twamplog.c			\
	../twamp/twamplog.h

icwmp_twampd_CFLAGS =			\
	$(AM_CFLAGS)				\
	$(LIBUCI_CFLAGS)

icwmp_twampd_LDFLAGS =			\
	$(AM_LDFLAGS)				\
	$(LIBUCI_LDFLAGS)
	
icwmp_twampd_LDADD =			\
	$(AM_LIBS)					\
	$(LIBUCI_LIBS)
endif

if ICWMP_BULKDATA
bin_PROGRAMS += icwmp_bulkdatad

icwmp_bulkdatad_SOURCES =		\
	../bulkdata/bulkdata.c			\
	../bulkdata/bulkdata.h			\
	../bulkdata/config.c 			\
	../bulkdata/config.h 			\
	../bulkdata/common.c 			\
	../bulkdata/common.h 			\
	../bulkdata/http.c 				\
	../bulkdata/http.h 				\
	../bulkdata/log.c				\
	../bulkdata/log.h				\
	../bulkdata/times.c 				\
	../bulkdata/times.h 				\
	../bulkdata/buci.c 				\
	../bulkdata/buci.h				\
	../bulkdata/report.c			\
	../bulkdata/report.h	

icwmp_bulkdatad_CFLAGS =			\
	$(AM_CFLAGS)				\
	$(LIBUCI_CFLAGS)			\
	$(LIBUBUS_CFLAGS)			\
	$(LIBUBOX_CFLAGS)			\
	$(LIBCURL_CFLAGS) 			\
	-Wall -Werror

icwmp_bulkdatad_LDFLAGS =			\
	$(AM_LDFLAGS)				\
	$(LIBUCI_LDFLAGS)			\
	$(LIBUBUS_LDFLAGS)			\
	$(LIBUBOX_LDFLAGS)			\
	$(LIBJSONC_LDFLAGS)			\
	$(LIBCURL_LDFLAGS)		

icwmp_bulkdatad_LDADD =				\
	$(AM_LIBS)					\
	$(LIBUCI_LIBS)				\
	$(LIBUBOX_LIBS)				\
	$(LIBJSON_LIBS)			    \
	$(LBLOBMSG_LIBS)			\
	$(LIBUBUS_LIBS)				\
	$(LCRYPTO_LIBS)				\
	$(LIBCURL_LIBS) 			\
	-lbbfdm
endif
