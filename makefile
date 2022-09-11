CFLAGS= 
CC=gcc
rec := recipes
out := bin

all: clean main rst2html 

main: bin ./main.c 
	$(CC) $(CFLAGS) main.c -o ./$(out)/kevlar

rst2html: bin ./$(rec)/rst2html.c 
	$(CC) $(CFLAGS) ./$(rec)/rst2html.c -o ./$(out)/rst2html

bin: 
	mkdir $(out)

clean: 
	rm -rf ./$(out)/
