CFLAGS=-Wall -Wextra 
CC=gcc
CMD := $(CC) $(CFLAGS)

kevlar_files := ./src/main.o ./src/kevlar_new.o ./src/kevlar_build.o ./src/kevlar_handle_config.o ./src/kevlar_templating.o ./utils/utils.o ./src/kevlar_rst_to_html.o

kevlar: $(kevlar_files)
	mkdir -p bin
	$(CC) $(kevlar_files) -o ./bin/kevlar

kevlar_win32: $(kevlar_files)
	mkdir "bin"
	$(CC) $(kevlar_files) -o ./bin/kevlar.exe

kevlar/main: ./src/main.c
	$(CMD) ./src/main.c

utils: ./utils/utils.c ./utils/utils.h
	$(CMD) ./utils/utils.c

kevlar/new: ./src/kevlar_new.c ./src/kevlar_new.h utils
	$(CMD) ./src/kevlar_new.c

kevlar/handle_config: ./src/kevlar_handle_config.c ./src/kevlar_handle_config.h utils
	$(CMD) ./src/kevlar_handle_config.c

kevlar/templating: ./src/kevlar_templating.c ./src/kevlar_templating.h kevlar/handle_config utils
	$(CMD)./src/kevlar_handle_templates.c

kevlar/build: ./src/kevlar_build.c ./src/kevlar_build.h kevlar/rst_to_html
	$(CMD) ./src/kevlar_build.c

kevlar/rst_to_html: ./src/kevlar_rst_to_html.c ./src/kevlar_rst_to_html.h utils
	$(CMD) ./src/kevlar_rst_to_html.c

recipes/rst2html: ./recipes/rst2html.c utils
	$(CC) ./recipes/rst2html.c

rst2html: ./recipes/rst2html.o ./src/kevlar_rst_to_html.o ./utils/utils.o
	mkdir -p bin 
	$(CC) ./recipes/rst2html.o ./src/kevlar_rst_to_html.o ./utils/utils.o -o ./bin/rst2html


all: kevlar rst2html

clean: 
	rm -rf ./src/*.o
	rm -rf ./recipes/*.o
	rm -rf ./bin
	rm -rf ./utils/*.o

clean_win32:
	# Sorry to fellow window users, maybe just shift to WSL 
	rmdir "bin"
