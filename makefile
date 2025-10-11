CFLAGS = -Wall -Werror -Wextra
CC = gcc

SRC_FILES = $(wildcard ./src/*.c)
TEST_FILES = $(wildcard ./tests/*.c)


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
test: $(TEST_FILES) $(filter-out ./bin/obj/main.o, $(OBJ_FILES))
	$(CC) $(CFLAGS) -g -ggdb -o ./bin/test $^

.PHONY: clean
clean:
ifeq ($(OS), Windows_NT)
	del /s /q bin
else
	rm -rf bin
endif
