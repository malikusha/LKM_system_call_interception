all: system_call_test

system_call_test: system_call_test.o
	gcc -pthread system_call_test.o -o system_call_test

system_call_test.o: system_call_test.c
	gcc -g -c system_call_test.c

clean:
	rm -f *.o system_call_test
