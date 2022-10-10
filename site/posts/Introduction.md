# Introduction

Kevlar is meant to be an out-of-the-box experience, hence it has minimal setup.

## Installation

You can get the kevlar [executable](https://github.com/Aadv1k/kevlar/releases/latest) from the github releases page; where you can download executable for your system;

## Create your first project

Alright; Let's create our first project;

```
./kevlar new my-project
```

This will setup an initial boilerplate with the following file structur

```
.
├── config.ini
├── posts
│   └── hello-world.rst
└── templates
    └── kyudo

3 directories, 2 files
```

you can then run, and serve `./dist/index.html` to see your site in action.

```
../kevlar build
```
