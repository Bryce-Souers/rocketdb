

all: driver.c includes/driver.h
	gcc driver.c -o build/driver.o -c
	gcc -shared -o build/driver.so build/driver.o -lsodium

build: driver.c
	make all