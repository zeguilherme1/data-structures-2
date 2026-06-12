all:
	gcc -g main.c models/*.c io/*.c parser/*.c search/*.c utils/*.c -o main

run: all
	./main

build:
	gcc main.c models/*.c io/*.c parser/*.c search/*.c utils/*.c -o main -w
