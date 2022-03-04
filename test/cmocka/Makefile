include ../../Makefile.am

LIB_LDFLAGS:= -lmicroxml -luci -lblobmsg_json -lubox\
	      -ljson-c -lubus -lpthread -lcurl\
	      -lcrypto
LIB_CFLAGS:= -fPIC -I../../inc -DWC_NO_HARDEN -g -O0
UNIT_TESTS:= icwmp_unit_testd

VALGRIND = /usr/bin/valgrind --xml=yes --xml-file=memory-report.xml --leak-check=full --show-reachable=yes --show-leak-kinds=all --errors-for-leak-kinds=all

ICWMP_SOURCES=$(patsubst ./%.c, ../../%.c, $(icwmpd_SOURCES))
ICWMP_OBJS=$(patsubst ../../%.c, %.o, $(ICWMP_SOURCES))
TEST_SRCS =  $(wildcard *.c)
TEST_OBJS = $(TEST_SRCS:=.o)

%.c.o: %.c
	$(CC) -g -O0 -I../../inc -c -o $@ $<

%.o: ../../%.c
	$(CC) ${LIB_CFLAGS} -c -o $@ $<

libunit: ${ICWMP_OBJS}
	$(CC) ${LIB_CFLAGS} -shared -o libicwmp.so ${ICWMP_OBJS} ${LIB_LDFLAGS}
	
icwmp_unit_testd:
	make ${TEST_OBJS}
	$(CC) -o $@ ${TEST_OBJS} ${LIB_LDFLAGS} -lcmocka -L. -licwmp -Wl,-rpath=.
	
all: libunit ${UNIT_TESTS}
	$(VALGRIND) ./${UNIT_TESTS}
	mv memory-report.xml ../../
	echo "All test done"

clean:
	rm -fv *.o libicwmp.so ${UNIT_TESTS}

.PHONY: clean unit-test
