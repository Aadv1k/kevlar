# Kevlar 🪢

An all-in-one batteries-included Static Site Generator built using pure C and 0 other dependencies.

- [Installation](#installation)
- [Usage](#usage)

_NOTE_: This project is being continuously worked on, and is being built in fragments, so currently the full functionality might be missing.

## Installation

To get started with kevlar, you need `gcc` and `make`, after which you can:

```shell
$ git clone git@github.com/aadv1k
$ make all
$ ./bin/kevlar
```

## Usage

Kevlar comes with a CLI which will provide you with almost everything you need to get building. To generate a new project use `kevlar new [NAME]` this will generate This will create the following skeleton for an empty kevlar project.

```shell
.
├── config.ini
├── posts
└── templates
```
After this you can type in [WIP] `kevlar serve` to get started.


## Configuration

Kevlar uses an in-house `.ini` parser for configuration. Here is what all you can configure

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

## Templating

Kevlar supports templating albeit basic, all fields will be parsed when you run the `kevlar build` command and have `config.ini` setup properly


```html
  --HEADER--
  --STYLE--
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
- `--HEADER--`: The text content for `header.html` inside your theme
- `--FOOTER--`: The footer content for `header.html` inside your theme
- `--STYLE--`: Stylesheet; looks for `main.css` in your theme

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

