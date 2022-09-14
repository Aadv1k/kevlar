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

### `kevlar new`

This will create the following skeleton for a kevlar project

```shell
.
├── config.ini
├── posts
└── templates

2 directories, 1 file`
```

### `kevlar build`

**WIP** Checks if the given directory is a kevlar project and builds the file using the template and files within `posts/` directory,


## Configuration

When you create a new site using the `kevlar new` command, you may find a `config.ini` located within the site directory, you can use this to configure several things. Here is an exaustive list of all possible values

```ini
author=
title=

rst_loader=
# ^ if you want to add a custom reStructuredText loader with arguments; By default `recipes/rst2html` will be used.

markdown_loader=
# ^ if you want to add a custom markdown loader with arguments

```

## Recipes

These are all supported file loaders, that used by default by kevlar, but all them can be compiled and used as standalone programs as well.

### `recipes/rst2html.c`

Converts rSt to text; However it is not to specficiation.

```shell
$ make rst2html
$ ./bin/rst2html
rst2html -[OPT] INPUT.rst OUTPUT.html
        -h -- On invalid rst exit with error message
```

for all the valid rst, refer to [examples/example.rst](./examples/example.rst)

