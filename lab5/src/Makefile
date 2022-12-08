CC = gcc
CFLAGS = -g -c -Wall

all: main1 main2

main1: libfunc1.so main1.c
	$(CC) main1.c -L. -lfunc1 -o main1

main2: libfunc1.so libfunc2.so main2.c
	$(CC) main2.c -ldl -o main2

libfunc1.so: functions1.o
	$(CC) -shared functions1.o -o libfunc1.so

libfunc2.so: functions2.o
	$(CC) -shared functions2.o -o libfunc2.so

functions1.o: functions1.c
	$(CC) -fPIC -c functions1.c -o functions1.o

functions2.o: functions2.c
	$(CC) -fPIC -c functions2.c -o functions2.o

clean:
	rm -r *.so *.o main1 main2