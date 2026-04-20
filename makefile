CFLAGS = -Wall -Werror -Wextra -g -ggdb
#
# CFLAGS += -fsanitize=address,undefined

CC = gcc

SRC_FILES = $(wildcard ./src/*.c)
TEST_FILES = $(wildcard ./tests/*.c)
VENDOR_SRC_UNITY = $(wildcard ./lib/unity/*.c)

OBJ_FILES := $(patsubst ./src/%.c,./bin/obj/%.o,$(SRC_FILES))

./bin/kelvar: $(OBJ_FILES)
ifeq ($(OS), Windows_NT)
	IF NOT EXIST $(@D) (
		mkdir $(@D)
	)
else
	mkdir -p $(@D)
endif
	$(CC) -o $@ $^

./bin/obj/%.o: ./src/%.c
ifeq ($(OS), Windows_NT)
	IF NOT EXIST $(@D) (
		mkdir $(@D)
	)
else
	mkdir -p $(@D)
endif
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: test
test: $(TEST_FILES) $(VENDOR_SRC_UNITY) $(filter-out ./bin/obj/main.o, $(OBJ_FILES))
	rm -rf ./bin/__fixtures__
	cp -rf ./tests/__fixtures__ ./bin/__fixtures__
	$(CC) $(CFLAGS) -g -ggdb -o ./bin/test $^

.PHONY: clean
clean:
ifeq ($(OS), Windows_NT)
	del /s /q bin
else
	rm -rf bin
endif

all: clean ./bin/kelvar test

.PHONY: format
format:
	clang-format -i ./src/*
