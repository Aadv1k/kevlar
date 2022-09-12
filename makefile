CFLAGS= 
CC=gcc
rec := recipes

$(shell mkdir -p ./bin/)

all: kevlar rst2html 

kevlar: ./src/main.c ./src/kevlar_new.c
	$(CC) $(CFLAGS) ./src/main.c ./src/kevlar_new.c -o ./bin/kevlar

rst2html: ./$(rec)/rst2html.c 
	$(CC) $(CFLAGS) ./$(rec)/rst2html.c -o ./bin/rst2html

clean: 
	rm -rf ./bin/
