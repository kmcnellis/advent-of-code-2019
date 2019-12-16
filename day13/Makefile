# build an executable named myprog from myprog.c

.PHONY: run test1 test2 test3

all: build run

build: main.cpp
	clang++ -std=c++17 -lncurses  -o main.exe main.cpp

run: build
	./main.exe input.txt

clean:
	$(RM) main.exe
