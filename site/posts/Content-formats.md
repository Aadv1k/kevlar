# Writing formats

Files prepended with `.filename.md` will be parsed but will remain unlisted.

Kevlar primarily support two content formtats; [markdown](https://www.markdownguide.org/) and [reStructuredText](https://docutils.sourceforge.io/rst.html). Since they were hacked together in-house,

## markdown

```
# h1
## h2
### h3
#### h4

- List item #1
- List itme #2

1. Ordered list #1
2. Ordered list #2

--- 

Content with *italic*, _another italic_, **bold**, ***bold italic** and ~~striked~~ support.
Each new line will be added to the previous as a singular paragraph

[Links are supported as well](https://github.com/aadv1k/kevlar)
```

## reStructuredText

```
==========
h1 heading
==========

h2 heading
==========

h3 heading
----------

- List item #1
- List itme #2

1. Ordered list #1
#. Ordered list #2

Content with *italic*, _another italic_, **bold**, ***bold italic** and ~~striked~~ support.
Each new line will be added to the previous as a singular paragraph
```
