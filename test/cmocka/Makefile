CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS = -lcmocka -licwmp -lmicroxml
UNIT_TESTS = unit_test_icwmpd

VALGRIND = valgrind --show-reachable=yes \
           --show-leak-kinds=all --errors-for-leak-kinds=all \
           --error-exitcode=1 --track-origins=yes
           
unit_test_icwmpd: unit_test_icwmp.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
unit-test: $(UNIT_TESTS)
	$(foreach testprog, $(UNIT_TESTS), sudo $(VALGRIND) ./$(testprog);)
	
.PHONY: clean
clean:
	rm -fv *.o $(UNIT_TESTS)