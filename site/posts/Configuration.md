# Configuration

Kevlar uses `.ini` format to handle configuration. 

## Required fields 

- `author`: Author name
- `title`: Site title; Note that this supports html
- `theme`: This will simply try and look for a folder with the name of the theme in `./templates` directory and use it given it is a valid theme

## Optional fields 

- `rst_loader`: This will be run as a system command where kevlar will attempt to pass the input and output file to your specifed CLI
- `md_loader`:  This will be run as a system command where kevlar will attempt to pass the input and output file to your specifed CLI

## Example config

Here is a example config with [rst2html5](https://pypi.org/project/rst2html5/) configured as the `.rst` loader and markdown left as default

```
title=My cool ASF site
author=Joe mama
theme=kyudo
rst_loader=rst2html5
```
