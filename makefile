CFLAGS=-Wall -Wextra -Wno-stringop-overread -Wno-stringop-overflow -O3
CC=gcc
CMD := $(CC) $(CFLAGS)

kevlar_files := ./src/main.o ./src/kevlar_new.o ./src/kevlar_build.o ./src/kevlar_handle_config.o ./src/kevlar_templating.o ./utils/utils.o ./src/kevlar_rst_to_html.o ./src/kevlar_md_to_html.o ./src/kevlar_errors.o

kevlar: $(kevlar_files)
	mkdir -p bin
	$(CC) $(kevlar_files) -o ./bin/kevlar

kevlar_win32: $(kevlar_files)
	if not exist bin mkdir bin
	$(CC) $(kevlar_files) -o ./bin/kevlar.exe

kevlar/main: ./src/main.c
	$(CMD) ./src/main.c

utils: ./utils/utils.c ./utils/utils.h
	$(CMD) ./utils/utils.c

kevlar/new: ./src/kevlar_new.c ./src/kevlar_new.h utils
	$(CMD) ./src/kevlar_new.c

kevlar/handle_config: ./src/kevlar_handle_config.c ./src/kevlar_handle_config.h utils
	$(CMD) ./src/kevlar_handle_config.c

kevlar/errors: ./src/kevlar_errors.c ./src/kevlar_errors.h
	$(CMD) ./src/kevlar_handle_stdout.c

kevlar/templating: ./src/kevlar_templating.c ./src/kevlar_templating.h kevlar/handle_config utils
	$(CMD)./src/kevlar_handle_templates.c

kevlar/build: ./src/kevlar_build.c ./src/kevlar_build.h kevlar/rst_to_html
	$(CMD) ./src/kevlar_build.c

kevlar/rst_to_html: ./src/kevlar_rst_to_html.c ./src/kevlar_rst_to_html.h utils
	$(CMD) ./src/kevlar_rst_to_html.c

kevlar/md_to_html: ./src/kevlar_md_to_html.c ./src/kevlar_md_to_html.h kevlar/rst_to_html utils
	$(CMD) ./src/kevlar_md_to_html.c

rst2html: ./recipes/rst2html.o ./src/kevlar_rst_to_html.o ./utils/utils.o
	mkdir -p bin 
	$(CC) ./recipes/rst2html.o ./src/kevlar_rst_to_html.o ./utils/utils.o -o ./bin/rst2html

md2html: ./recipes/md2html.o ./src/kevlar_md_to_html.o ./src/kevlar_rst_to_html.o ./utils/utils.o
	mkdir -p bin 
	$(CC) ./recipes/md2html.o ./src/kevlar_md_to_html.o ./src/kevlar_rst_to_html.o ./utils/utils.o -o ./bin/md2html

all: kevlar rst2html md2html

clean: 
	rm -rf ./src/*.o
	rm -rf ./recipes/*.o
	rm -rf ./bin
	rm -rf ./utils/*.o
