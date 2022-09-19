CFLAGS=
CC=gcc
CMD := $(CC) $(CFLAGS)

$(shell mkdir -p ./bin/)

kevlar_files := ./src/main.o ./src/kevlar_new.o ./src/kevlar_build.o ./src/kevlar_handle_config.o ./src/kevlar_templating.o ./utils/utils.o

kevlar: $(kevlar_files)
	mkdir -p bin
	gcc $(kevlar_files) -o ./bin/kevlar

kevlar/main: ./src/main.c
	$(CMD) ./src/main.c

utils: ./utils/utils.c
	$(CMD) ./utils/utils.c

kevlar/new: ./src/kevlar_new.c ./src/kevlar_new.h utils
	$(CMD) ./src/kevlar_new.c

kevlar/handle_config: ./src/kevlar_handle_config.c ./src/kevlar_handle_config.h utils
	$(CMD) ./src/kevlar_handle_config.c

kevlar/templating: ./src/kevlar_templating.c.c ./src/kevlar_templating.c.h kevlar/handle_config utils
	$(CMD)./src/kevlar_handle_templates.c

kevlar/build: ./src/kevlar_build.c ./src/kevlar_build.h 
	$(CMD) ./src/kevlar_build.c

rst2html: ./recipes/rst2html.c utils
	mkdir -p bin
	$(CMD) ./recipes/rst2html.c ./utils/utils.c -o ./bin/rst2html

all: kevlar rst2html

clean: 
	rm -rf ./src/*.o
	rm -rf ./bin
