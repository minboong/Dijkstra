all: compile

compile: main.c
	gcc main.c -o assignment3

run: assignment3
	./assignment3 input.txt output.txt

clean: assignment3
	rm assignment3