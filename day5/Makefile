# build an executable named myprog from myprog.c

.PHONY: run

all: build run

build: main.cpp
	clang++ -o main.exe main.cpp

run: build
	./main.exe input.txt

test: build
	./main.exe test.txt

clean:
	$(RM) main.exe
