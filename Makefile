all:
	gcc -g main.c edit_record/*.c models/*.c io/*.c parser/*.c search/*.c utils/*.c -o main

run: all
	./main

build:
	gcc main.c edit_record/*.c models/*.c io/*.c parser/*.c search/*.c utils/*.c -o main -w
