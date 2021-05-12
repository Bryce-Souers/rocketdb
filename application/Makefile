

all: main.c
	make -C ../driver -B
	make build -B
	clear
	./build/app

build: main.c
	gcc -Wall main.c ../driver/build/driver.so -o build/app -I/home/bryce/CLionProjects/rocketdb/driver/includes