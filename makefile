CFLAGS= 
CC=gcc
CMD := $(CC) $(CFLAGS)

$(shell mkdir -p ./bin/)

kevlar_files := ./src/main.o ./src/kevlar_new.o ./src/kevlar_build.o ./src/kevlar_handle_config.o 

kevlar: $(kevlar_files)
	mkdir -p bin
	gcc $(kevlar_files) -o ./bin/kevlar

kevlar/main: ./src/main.c
	$(CMD) ./src/main.c

kevlar/new: ./src/kevlar_new.c ./src/kevlar_new.h
	$(CMD) ./src/kevlar_new.c

kevlar/handle_config: ./src/kevlar_handle_config.c ./src/kevlar_handle_config.h
	$(CMD) ./src/kevlar_handle_config.c

kevlar/handle_templates: ./src/kevlar_handle_templates.c ./src/kevlar_handle_templates.h kevlar/handle_config
	$(CMD)./src/kevlar_handle_templates.c

kevlar/build: ./src/kevlar_build.c ./src/kevlar_build.h 
	$(CMD) ./src/kevlar_build.c

rst2html: ./recipes/rst2html.c 
	mkdir -p bin
	$(CMD) ./recipes/rst2html.c -o ./bin/rst2html

all: kevlar rst2html


clean: 
	rm -rf ./src/*.o
	rm -rf ./bin
