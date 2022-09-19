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

This command does the following in-order

- Check if current directory is a "kevlar project" eg: Makes sure it has all the files from `kevlar new`
- Loads the `config.ini` file and puts the information into a structure
- [WIP] Parse rst files from "./posts" and outputs them to "./dist"; uses a loader specified in the `config.ini` file to convert rst to html
- Iterate through all the `.html` file paths and put them inside `configListing` field in a config struct
- [WIP] opens the template from the theme specified in the `config.ini` file and fills it with params.


## Configuration

Kevlar uses a in-house `.ini` parser for configuration, so it might be a rough on edges. Here is what all you can configure

```ini
author=
title=
theme=
# ^ looks for theme inside the ./templates/ directory

rst_loader=
# ^ could be anything; rst2html5, kevlar/bin/rst2html;
```
Given the nature of how these files are parsed, html would be valid within these `config.ini` eg:  

```ini
author=<b>I am a bold author</b>
```
I call it a feature rather than a bug.

## Templating

Kevlar supports templating albeit basic, all fields will be parsed when you run the `kevlar build` command and have `config.ini` setup properly

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>--TITLE--</title>
</head>
<body>
  <h1>--AUTHOR--</h1>
  <ul>
    --LISTING--
  </ul>
</body>
</html>
```

- `--TITLE--`: fills in whatever title you filled in config file. 
- `--AUTHOR--`: fills in whatever author name you provided in config file
- `--LISTING--`: provides links surrounded by `<li>` of all the html files in `./dist`

## Recipes

These are all in-house parsers programmed from scratch. Since I am not very good with developing languages nor with C, these by no means are a "to-spec" parser, mostly just implemented on top of my head with whatever I preferred and could remember.

### `recipes/rst2html.c`

```shell
$ make rst2html
$ ./bin/rst2html
rst2html -[OPT] INPUT.rst OUTPUT.html
        -h -- On invalid rst exit with error message
```

for all the valid rst, refer to [examples/example.rst](./examples/example.rst)

