
Foo does exist, but it is empty, yet this command doesn't work.

```
kevlar % ./bin/kevlar new foo
ERROR: folder "foo" already exists and is not empty!
```

Also "kevlar new ." doesn't work either

--------------------

Doesn't inform me if this is an error, or a warning

```
../bin/kevlar build
Parsing ./posts/hello-world.md
[kevlar] Option "GH_LINK" not found in ./config.ini
``` 

(it infact was an error because index.html was never generated)

```
foo % ls dist
hello-world.html
```

--------------------

Let's observe what happens when we have a large blog post

```
wicked % wc -w ./posts/house-across.md # number of words
   37025 ./posts/house-across.md
wicked % ../bin/kevlar build
Parsing ./posts/house-across.md
zsh: trace trap  ../bin/kevlar build
```

Trace trap refers to `SIGTRAP`, an error that is typically raised during debugging, but we aren't debugging, which means the program is doing something weird that the hardware simply refuses to parse. Let's break it further


```
wicked % wc -w ./posts/house-across.md
   57413 ./posts/house-across.md
wicked % ../bin/kevlar build          
zsh: segmentation fault  ../bin/kevlar build
```

Finally, the good old seg fault. Here is our culprit

```
void md_parse(char *in_file_path, char *out_file_path, int offset) {
  // ...

  md_outfile = fopen(out_file_path, "w");

  # get the number of lines in the file
  long fileLength = rst_get_file_length(in_file_path);

  char file[fileLength][RST_LINE_LENGTH];

  // ...
}
```

Here we first get number of lines in the file, which is `1069`, and then for each allocate `#define RST_LINE_LENGTH 10000` which is 10 MB of straight stack allocation, no wonder we get a seg fault. Again, my excuse is that I did not write this code, the 2-year-back-me did :) 


### A 

Let's observe the function `kevlar_handle_build_command` in `kevlar_build.c`


```
kevlar_check_if_kevlar_proj(file_path, &skel);
// ...
kevlar_get_opt_from_config("./config.ini", "theme", cTheme);
// ...
kevlar_check_if_theme_valid(cTheme);

utl_mkdir_crossplatform("dist");

size_t md_count = kevlar_count_files_in_folder("./posts", "md");

ListingItem * itemsList = malloc(md_count*sizeof(ListingItem));
kevlar_parse_md_from_folder("./posts", "./dist", themePath, itemsList);

bsortDesc(itemsList, md_count);

kevlar_generate_index_from_template( "./dist/index.html", themePath, "./config.ini", itemsList, md_count);

// ...

```

Essentially, the core component here is the `kevlar_parse_md_from_folder` function, because it literally just does that. It strips the frontmatter from the markdown file, parses the markdown and spits out the HTML file in `dist` immediately. The issue with such an approach is that the written `.html` files BECOME our registery, so to provide any context for each post, we have to always read from the directory.

Breaking kevlar's parsing wasn't very hard. I took my [bitcoin post](link to btc post) and instead of splitting lines, I had everyone on one line, and boom

```
wicked % ../bin/kevlar build
Parsing ./posts/building-an-email-platform.md
Parsing ./posts/btc.md
ERROR: was unable to read
```

this error causes the entire pipeline to break, since an error causes a non-zero exit

```
    md_parse(in_fp, out_fp, 3);
    FILE * out_file = fopen(out_fp, "r");

    fseek(out_file, 0, SEEK_END);
    int out_file_length = ftell(out_file);
    rewind(out_file);

    char * buf = malloc(out_file_length + 1);
    if (!fread(buf, out_file_length, 1, out_file)) kevlar_err("was unable to read %s", out_file);
```

The magic numbers aside this code fails because `md_parse` function siletnly failed (because the line exceeded it's fixed 10K line len!), hence it never generated the `out_file` and since we never checked if the `fopen` was successful, the code fails at the `fread` where we exit with a `1` code. This bug took me a while to figure out, simply because there are so many points of failure here, it's hard to be sure without the good old `printf` debugging. (I should really use GDB) 

### B

```
Title=Discovering Programming at the Darkest Point in my Life
Date=2025-06-21 11:01:42
Order=0
```

This is an insane way to handle ordering for blog posts by literally maintaing a "order" field and making it display in an descending order. I guess the 2-year-back me simply could not figure out how to parse date time and use that to order, this.


```c
  bsortDesc(itemsList, md_count);
```

And to add salt to the wound, it's bubble sort no less! We've got a lot to fix here. 

Also a rather unimpressive surprise, apparently 2-year-back-me decided to resue `config.ini` parsing logic for the front-matter too. I must've thought it's quite clever back then.

```c
    // ...
    kevlar_get_opt_from_config(in_fp, "date", itemsList[post_count].lDate);
    kevlar_get_opt_from_config(in_fp, "title", itemsList[post_count].lTitle);


    char order[TEMPLATE_MAX_TAG_SIZE];
    kevlar_get_opt_from_config(in_fp, "order", order);
```

(Notice the stack allocations everywhere)  

```c
  char cTheme[CONFIG_MAX_PATH_SIZE];
  kevlar_get_opt_from_config("./config.ini", "theme", cTheme);
```

Behold, the advanced packaging code that makes `npm` shy!

```
  char clone_git_command[NEW_SYS_CMD_LEN] = "git clone https://github.com/aadv1k/listed-kevlar-theme ";
  strcat(clone_git_command, skeleton->skel_template_folder_path);
  strcat(clone_git_command, "/listed-kevlar-theme");
  strcat(clone_git_command, " >/dev/null 2>&1");
```

Let's not even question why this string is stack allocated when very easily it can be a `const* char clone_git_command = "..."` 

----

## How people actually use kevlar? 

I noticed a starer of kevlar,  [h5law](https://github.com/h5law) actualyl usign the kevlar project in production. This is what gave me that push to go ahead and fix several glaring issues wit the project because I saw other were actually getting value from it.

- Users had been using a hacky approach to creating a `404` page by essentialyl bakign the actual `404.html` file into the `dist` folder and commiting it into main repo. This is bad because obiviously now one is required ot always commit `dist` folder. 

- Project lacks a `.gitignore` + the kevlar theme is simply cloned into the subfolder. This leads to conflicts with git, the correct approach here is to use submodules and to write a `.gitignore` file during `kevlar build` 

--- 

## To refactor vs to rewrite? 

My plan for the kevlar v3 requires significantly different architecture and approach compared to kevlar v2, hence the best approach in my case would be to do a complete ground up re-write of the codebase.  
