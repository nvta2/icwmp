include ../../Makefile.am

LIB_LDFLAGS:=-lmicroxml -luci -lblobmsg_json -lubox\
	      -ljson-c -lubus -lpthread -lcurl\
	      -lssl -lcrypto
LIB_CFLAGS:=-g -Wall -fPIC -I../../inc
UNIT_TESTS:= icwmp_unit_testd

VALGRIND = /usr/bin/valgrind --xml=yes --xml-file=memory-report.xml --leak-check=full --show-reachable=yes --show-leak-kinds=all --errors-for-leak-kinds=all

ICWMP_SOURCES=$(patsubst ./%.c, ../../%.o, $(icwmpd_SOURCES))
TEST_OBJ =  icwmp_datamodel_interface_unit_test.o \
	    icwmp_soap_msg_unit_test.o  \
	    icwmp_backup_session_unit_test.o  \
	    icwmp_download_unit_test.o \
	    icwmp_notifications_unit_test.o \
	    icwmp_cli_unit_test.o \
	    icwmp_uci_unit_test.o \
	    icwmp_custom_inform_parameters_unit_test.o \
	    icwmp_unit_test.o

%.o: %.c
	$(CC) ${LIB_CFLAGS} -c -o $@ $<

libunit: ${ICWMP_SOURCES}
	$(CC) ${LIB_CFLAGS} -shared -o libicwmp.so ${ICWMP_SOURCES} ${LIB_LDFLAGS}
	
icwmp_unit_testd: ${TEST_OBJ}
	$(CC) -o $@ $^ ${LIB_LDFLAGS} -lcmocka -L. -licwmp -Wl,-rpath=.
	
all: libunit ${UNIT_TESTS}
	$(VALGRIND) ./${UNIT_TESTS}
	mv memory-report.xml ../../
	echo "All test done"

clean:
	rm -fv *.o libicwmp.so ${UNIT_TESTS}

.PHONY: clean all unit-test
