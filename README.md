# Kevlar V3

## Plan for v3

- Add unit testing, particularly for the markdown component of the application
- GET RID OF ALL THE MEMORY LEAKS
- More robust and modern markdown parsing that implements best practices
- Introduce JSON as the standard config format for both the config files as well
  as the meta-data
- Implement first-class support for dates, and sorting the listing  

- better templating, with ``--IF {cond}--`` and ``--ELSE--`` where cond can be a
  flag that the user set in his config. The idea is that a template author may
  use this to write robust templates which work for a wide variety of formats. I
  really want this to be a highly configuration heavy experience. 

- `--IF {some_flag}--`, `--ELIF {some_flag}--`, `--ELSE--`, 
- `--CONF {some_opt}--` pick and replace a config option at place
- `--LISTING :asc :tag--` 

An _utterly_ simple and fast Static Site Generator built using C

- [✅] Simple and fast
- [✅] HTML-first approach
- [✅] Crossplatform (Tested on `Windows 11 v21H2` and `Debian GNU/Linux 11 x86_64`)
- [❌] JS-first SPA experience
- [❌] Frameworks and external build tools
- [❌] Plugins and lifecycle hooks

## Install

To get kevlar, you can either get a [binary from releases](https://github.com/Aadv1k/kevlar/releases/latest) or build from scratch; instructions for which are given here.

To get start with kevlar, you need `GCC` and `make` which are the primary build tools used by kevlar

### unix-like

**_build_**

```shell
git clone https://github.com/aadv1k/kevlar
cd kevlar && make all
./bin/kevlar help
```

### windows

You may need something like [mingw32-make](https://sourceforge.net/projects/mingw/files/MinGW/Extension/make/mingw32-make-3.80-3/), and gcc for windows for this to work

```cmd
git clone https://github.com/aadv1k/kevlar
cd kevlar && make kevlar_win32
"bin/kevlar.exe" help
```

## Recipes

Kevlar has it's own markdown loader if you would like to build it as it's own executable you can run `make md2html`
You can build the loader kevlar uses to parse `.md` files as their own executables by running.

You will get the respective CLI

## CLI

```shell
$ kevlar
kevlar <cmd> <opt>
  help       -- print this help message
  new        -- create a new site skeleton
  new-post   -- create a new post with defaults
  build      -- build if in a kevlar project
```

### `kevlar new <NAME>`

Sets up a basic skeleton for a site and a basic `config.ini`

```ini
theme=listing-kevlar-theme
```

- `theme=`: looks for the given theme within the `./templates/` dir.

### `kevlar new-post <TITLE>`

Generates a `.md` file in the current folder with the following archetype

```
Title=This is a title
Date=2023-01-24 17:44:10
Order=1
```

- Title: the name of the link that will show up with `--TITLE--`
- Date: current date, put AS-IS with `--DATE--`
- Order: the order in which the post will show up in `--LISTING--`

## Config

the only required config is the `theme` option in the `config.ini` files.

## Templating

### `*.html`

| Tags                     | Description                                                                                     |
| ------------------------ | ----------------------------------------------------------------------------------------------- |
| `--HEADER--`             | Reads and parses `header.html` in your theme                                                    |
| `--FOOTER--`             | Reads and parses `footer.html` in your theme                                                    |
| `--SCRIPT ./script.js--` | Finds the provided file within yout theme and inserts an inline script                          |
| `--STYLE ./style.css--`  | Finds the provided file within yout theme and inserts an inline style                           |
| `--CUSTOM_TAG--`         | will look up the tag (lowercase) in `config.ini`, where you can provide the option for this tag |

### `post.html`

Defines the layout of a post

| Tags          | Description                 |
| ------------- | --------------------------- |
| `--CONTENT--` | The parsed markdown content |

### `entry.html`

Defines to how a singular list item within `--LISTING--` will look like

| Tags          | Description                         |
| ------------- | ----------------------------------- |
| `--DATE--`    | The `Date=` option in the post file |
| `--PATH--`    | Html link of the parsed post        |
| `--CONTENT--` | parsed html content                 |

## `index.html`

| Tags          | Description                                        |
| ------------- | -------------------------------------------------- |
| `--LISTING--` | provides a list of all posts based on `entry.html` |
