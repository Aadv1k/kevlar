# Kevlar 🪢

An _utterly_ simple and fast Static Site Generator built using C

_NOTE_: This project is being continuously worked on, and is being built in fragments, so currently the full functionality might be missing.

**elevator pitch**

> maybe you take notes, or write articles in X format, you would like to quickly convert your project into a website without any extra baggage and minimal shift to your existing workflow, introducing kevlar, an utterly simple SSG.

## Features

- **Cross platform:** Runs both on windows and unix-like operating systems!, tested with -
  - windows 11 v21H2
  - Debian GNU/Linux 11 (bullseye) on Windows 10 x86_64 ([WSL](https://learn.microsoft.com/en-us/windows/wsl/))
- **Recipes included:** with support for a custom spec of reStructuredText out of the box, (more to come)
  - [Markdown](#convert-markdown)
  - [reStructuredText](#convert-restructured-text)
- **Highly extensible:** Support for basic yet scalable templating
- **Simple:** Kevlar does not try to do too little or too much, just enough to get the job done.

## Documentation

- [Get started](#get-started)
  - [windows](#windows)
  - [Linux and MacOS](#unix-linke)
- [Configuration](#configuration)
- [Recipes](#recipes)
  - [Markdown](#convert-markdown)
  - [reStructuredText](#convert-restructuredtext)
- [Templating](#templating)
  - [Loader tags](#loader-tags)
  - [File specific tags](#file-specific-tags)
  - [Attribute tags](#attribute-tags)
- [Creating themes](#theming)

## Get started

To get start with kevlar, you need `GCC` and `make` which are the primary build tools used by kevlar

### unix-like

**_build_**

```shell
git clone https://github.com/aadv1k/kevlar
cd kevlar && make all
./bin/kevlar help
```

**_Create a new project_**

```shell
./bin/kevlar new myproject
cd myproject
../kevlar build
```

### windows

You may need something like [mingw32-make](https://sourceforge.net/projects/mingw/files/MinGW/Extension/make/mingw32-make-3.80-3/), and gcc for windows for this to work

```cmd
git clone https://github.com/aadv1k/kevlar
cd kevlar && make kevlar_win32
"bin/kevlar.exe" help
```

**_Create a new project_**

```cmd
"bin/kevlar" new myproject
cd myproject
../kevlar build
```

## Configuration

Each kelvar project must have a `config.ini` file; kevlar uses a custom `.ini` parser which was hacked together under 30 minutes, so some advance `.ini` functionality might be missing. Here is a configuration with all the options.

if `rst_loader` or `md_loader` fields are not specified, default loaders for each will be used, unless a CLI is provided

```ini
author=John Doe
title=The title of my site

rst_loader=rst2html5
md_loader=cmark

theme=kyudo
# ^- looks for themes inside ./templates dir
```

## Recipes

These are the loader kevlar comes with out-of-the-box.

### Convert reStructuredText

- A higly customized spec of [reStructuredText](https://docutils.sourceforge.io/rst.html) is supported.
- can be built as a separate program using `make rst2html`

```rst
=================
The main h1 title
=================

A h2 title
==========

A h3 title
----------

Every other text is a paragraph; which supports *italic*, **bold** and ***bold italic*** text.

1. This is an ordered list
#. This syntax is also supported
4. Your lists don't need to be in order for them to be parsed properly

- **Unordered** list item 1
- Unordered *list item* 2

Inline-code blocks are supported: `printf("%s\n", "hello world")`

Links are also supported; here is a `Link to this repository https://github.com/aadv1k/kevlar`_
```

### Convert markdown

- An customized implementation of [GFM](https://github.github.com/gfm/)
- Can be built as a separate program using `make md2html`

```md
# Heading 1

## Heading 2

### Heading 3

#### Heading 4

##### Heading 5

This is a paragraph
Which also includes this sentence, so multiline
paragraphs are supported

Unless you leave a line, in which case this is a separate para

This is _italic_, this is **bold** and this is **_bold italic_**

1. This is an ordered list
2. Your lists don't need to be in order for them to be parsed properly

hr separates

---

- Unordered list item 1
- Unordered list item 2

* Another way
* Anohter item
```

### Templating

kevlar has it's own custom templating language which is supported out of the box

#### Loader tags

This tag looks for a `header.html` in your specified theme, it then parses the header with this templating and replaces the `--HEADER--` with it.

```html
--HEADER--
```

This tag looks for a `footer.html` in your specified theme, it then parses the header with this templating and replaces the `--FOOTER--` with it.

```html
--FOOTER--
```

The `--STYLE--` will look for the specified css file and insert it's content as style tags within the html

```html
--STYLE ./style.css--
```

#### File specific tags

You can add `--LISTING--` anywhere to get a list of all the files that were passed from `posts` to html.

```html
<ul>
  --LISTING--
</ul>
```

As each file is parsed into html, on success it is read internally stored inside a struct, putting `--CONTENT--` will simply load the contents of the string.
This only works when called from `post.html` within your theme, since `post.html` is parsed everytime a new file is parsed from the posts folder.

```html
--CONTENT--
```

#### Attribute tags

- `--AUTHOR--`: The `author` field in your `config.ini` file
- `--TITLE--`: The `title` field in your `config.ini` file

### Theming

Kevlar supports theming with very specific archetypes, each theme must contain a

- `index.html`: is called once all files are parsed, `--LISTING--` will give you `<li><a></a></li>`
- `post.html`: is called for each individual post; `--CONTENT--` tag within this to get the html post.
- `header.html`: The header file called by `--HEADER--`
- `footer.html`: The footer file called by `--FOOTER--`

psst - you can check out the theme [kyudo](https://github.com/aadv1k/kyudo) to see the flexibility you get with this templating system
