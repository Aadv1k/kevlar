# Kevlar

An all-in-one batteries included SSR made using pure C and zero dependencies.

_NOTE_: This project is being continuously worked on, and is being built in fragments, so currently the full functionality might be missing.

## Usage

The project uses `make` and `GCC` to build and compile 

```shell
$ git clone git@github.com/aadv1k
$ make all
$ ./bin/kevlar
```

Everything is located within the `bin/` directory

## Recipes

These are all supported file loaders, that are used by kevlar, but all them can be compiled and used as standalone programs as well.

### `recepies/rst2html.c`

Convert a documentation language based on reStructuredText (Not to specification) to html

```shell
$ ./rst2html
rst2html -[OPT] INPUT.rst OUTPUT.html
        -h -- On invalid rst exit with error message
```

for all the valid rst, refer to [examples/example.rst](./examples/example.rst)
