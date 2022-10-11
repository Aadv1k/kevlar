# Kevlar 🪢

An _utterly_ simple and fast Static Site Generator built using C

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

## Get

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

You can build the loader kevlar uses to parse `.rst` and `.md` files as their own executables by running

- `make rst2html`
- `make md2html`

You will get the respective CLI
