# build an executable named myprog from myprog.c

.PHONY: run test1 test2 test3

all: build run

build: main.cpp
	clang++ -std=c++17  -o main.exe main.cpp

run: build
	./main.sh input.txt

test1: build
	echo "should be signal 139629729, sequence 98765"
	./main.sh test.txt

test2: build
	echo "should be signal 18216, sequence 97856"
	./main.sh test2.txt

clean:
	$(RM) main.exe
