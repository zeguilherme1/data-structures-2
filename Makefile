all:
	g++ -g main.cpp edit_record/*.cpp graph/*.cpp models/*.cpp io/*.cpp parser/*.cpp search/*.cpp utils/*.cpp -o main

run: all
	./main

build:
	g++ main.cpp edit_record/*.cpp graph/*.cpp models/*.cpp io/*.cpp parser/*.cpp search/*.cpp utils/*.cpp -o main -w
