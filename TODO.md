Markdown Compiler 
-----------------

- [ ] Move away from the "next occurance" based matching for `em` & `strong` elements, instead implement something that is close to the commonmark spec, also maintain a stack data structure

- [ ] Refactor the code blocks implementation to also be closer to the spec 

- [ ] Implements links parsing (including image links) 
- [ ] Implement block quotes functionality 
- [ ] Implement unordered list functionality 
- [ ] Implement ordered list functionality 
- [ ] Implement basic table parsing (have a look at the AST syntax for the same)
- [ ] Basic HTML recognition and parsing 


INI Parser
----------

Features to implement
- Namespacing  
- Support for omission or inclusion of string literals
    - Everything is parsed as string literals by default
- Multi-line strings via triple quotes (similar to python)
- Support for either ":" or "=", spacing is allowed around the separator
- Commenting via either ';' or '#'

```ini
config: foo bar
config = "baz"
```

Templating
----------
