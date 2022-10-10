# Theming

Theming in kevlar must be done using certain archetypes; In a theme

- Kelvar looks for `post.html` when parsing each individual post
- Kevlar looks for `index.html` once it has successfully parsed app files within `./posts/`

<br>

Here are the theme variables; they must be called in `--VARIABLE--` format

<br>

- `--HEADER--`: Looks for `header.html` within your theme and replaces the line with contents
- `--FOOTER--`: Looks for `footer.html` within your theme and replaces the line with contents
- `--AUTHOR--`: replaces the line with `author` field from your config.
- `--TITLE--`: replaces the line with `title` field from your config.
- `--STYLE ./styles/index.css--`: Replaces with the line with contents of `.css` file sorrounded with `<style></style>` tags
- `--CONTENT--`: Exclusive to `post.html`, replaces lines with parsed html contents of the post in question

## Examples and references

- [Kyudo](https://github.com/aadv1k/kyudo) an utterly minimalistic theme for kevlar
