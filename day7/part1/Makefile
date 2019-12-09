# build an executable named myprog from myprog.c

.PHONY: run test1 test2 test3

all: build run

build: main.cpp
	clang++ -o main.exe main.cpp

run: build
	./main.sh input.txt

test1: build
	./main.sh test.txt

test2: build
	./main.sh test2.txt

test3: build
	./main.sh test3.txt

clean:
	$(RM) main.exe
