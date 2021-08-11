CC = gcc
CP = cp -f
RM = rm -f
MKDIR = mkdir
CFLAGS = -g -Wall -I./inc
LDFLAGS = -lmicroxml -luci -lblobmsg_json -lubox -ljson-c -lubus -lpthread -lcurl -lssl -lcrypto
LIBCFLAGS = -g -Wall -fPIC -c -I../../inc
LIBOBJ = libobj
UNIT_TESTS = icwmp_datamodel_interface_unit_testd icwmp_soap_msg_unit_testd icwmp_backup_session_unit_testd icwmp_download_unit_testd icwmp_notifications_unit_testd icwmp_cli_unit_testd
VALGRIND = valgrind --xml=yes --xml-file=/builds/iopsys/icwmp/memory-report.xml --leak-check=full --show-reachable=yes --show-leak-kinds=all --errors-for-leak-kinds=all

libobj:
	$(CC) $(LIBCFLAGS) ../../download.c ../../upload.c ../../log.c ../../md5.c ../../digestauth.c ../../netlink.c ../../cwmp_cli.c ../../cwmp_du_state.c ../../sched_inform.c \
	 ../../diagnostic.c ../../reboot.c ../../notifications.c ../../cwmp_zlib.c  ../../datamodel_interface.c ../../http.c ../../backupSession.c \
	 ../../cwmp_time.c ../../config.c ../../event.c ../../session.c ../../ubus.c ../../common.c ../../xml.c ../../rpc_soap.c ../../cwmp_uci.c ../../cwmp.c $(LDFLAGS)  

libicwmp: $(LIBOBJ)
	$(CC) -shared -Wl,-soname,libicwmp.so.1 -o libicwmp.so.1.0 *.o
	
install:
	$(CP) libicwmp.so.1.0 /usr/local/lib/
	ln -sf /usr/local/lib/libicwmp.so.1.0 /usr/local/lib/libicwmp.so.1
	ln -sf /usr/local/lib/libicwmp.so.1.0 /usr/local/lib/libicwmp.so
	$(MKDIR) -p /usr/local/include/libicwmp/
	$(CP) ../../inc/*.h /usr/local/include/libicwmp/

uninstall:
	$(RM) /usr/local/lib/libicwmp.so.1.0
	$(RM) /usr/local/lib/libicwmp.so.1
	$(RM) /usr/local/lib/libicwmp.so
	$(RM) -r /usr/local/include/libicwmp/
	
icwmp_datamodel_interface_unit_testd: icwmp_datamodel_interface_unit_test.o
	$(CC) -o $@ $^ -lcmocka -licwmp $(LDFLAGS) $(CFLAGS)

icwmp_soap_msg_unit_testd: icwmp_soap_msg_unit_test.o
	$(CC) -o $@ $^ -lcmocka -licwmp $(LDFLAGS) $(CFLAGS)

icwmp_backup_session_unit_testd: icwmp_backup_session_unit_test.o
	$(CC) -o $@ $^ -lcmocka -licwmp $(LDFLAGS) $(CFLAGS)
	
icwmp_download_unit_testd: icwmp_download_unit_test.o
	$(CC) -o $@ $^ -lcmocka -licwmp $(LDFLAGS) $(CFLAGS)

icwmp_notifications_unit_testd: icwmp_notifications_unit_test.o
	$(CC) -o $@ $^ -lcmocka -licwmp $(LDFLAGS) $(CFLAGS)

icwmp_cli_unit_testd: icwmp_cli_unit_test.o
	$(CC) -o $@ $^ -lcmocka -licwmp $(LDFLAGS) $(CFLAGS)
		
unit-test: $(UNIT_TESTS)
	for testprog in $(UNIT_TESTS); do \
		sudo $(VALGRIND) ./$$testprog; \
		cp ../files/etc/config/* /etc/config; \
	done
	
.PHONY: clean
clean:
	rm -fv *.o libicwmp.so.1.0 $(UNIT_TESTS)
