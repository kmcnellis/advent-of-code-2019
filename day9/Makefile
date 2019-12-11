# build an executable named myprog from myprog.c

.PHONY: run test1 test2 test3

all: build run

build: main.cpp
	clang++ -std=c++17  -o main.exe main.cpp

run: build
	./main.exe input.txt

test1: build
	echo "Should print 109,1,204,-1,1001,100,1,100,1008,100,16,101,1006,101,0,99"
	./main.exe test.txt

test2: build
	echo "Should print a 16-digit number"
	./main.exe test2.txt

test3: build
	echo "Should print a 1125899906842624"
	./main.exe test3.txt

clean:
	$(RM) main.exe
