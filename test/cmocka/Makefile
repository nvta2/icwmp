CC = gcc
CFLAGS = -g -Wall -I./inc
LDFLAGS = -lcmocka -licwmp -lmicroxml
UNIT_TESTS = icwmp_datamodel_interface_unit_testd icwmp_soap_msg_unit_testd

VALGRIND = valgrind --xml=yes --xml-file=/builds/iopsys/icwmp/memory-report.xml --leak-check=full --show-reachable=yes --show-leak-kinds=all --errors-for-leak-kinds=all

icwmp_datamodel_interface_unit_testd: icwmp_datamodel_interface_unit_test.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

icwmp_soap_msg_unit_testd: icwmp_soap_msg_unit_test.o
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

unit-test: $(UNIT_TESTS)
	$(foreach testprog, $(UNIT_TESTS), sudo $(VALGRIND) ./$(testprog);)
	
.PHONY: clean
clean:
	rm -fv *.o $(UNIT_TESTS)