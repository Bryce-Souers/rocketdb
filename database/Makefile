

all:
	make build
	./build/rocket_service 127.0.0.1 3306

run:
	make all
	./build/rocket_service 127.0.0.1 3306

build: main.c includes/main.h worker.c includes/worker.h
	gcc -Wall main.c worker.c -o build/rocket_service -lpthread -lsodium