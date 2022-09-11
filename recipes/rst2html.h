#ifndef RST2HTML_H
#define RST2HTML_H

#include <stdio.h>
#define LINELEN 999 ///< The length of each line we expect
#define PARAMSIZE 50 ///< rst field list size

/**
 * Handle "=", which includes titles (h1), and subtitles (h2)
 * @param[in] file[][LINELEN] an array of character arrays corresponding to file lines
 * @param[in] line the target line starting with '='
 */
void rst_handleEqual(char file[][LINELEN], int line);

/**
 * Read a file based on the name and get the lines for it
 * @param[in] filename the path to the target file
 */
int rst_getFileLen(char filename[]);

/**
 * This is the "default" case to check if the line is not a list, title or other special token
 * @param[in] file[][LINELEN] an array of character arrays corresponding to file lines
 * @param[in] line the target line starting with '='
 */
int rst_isTextOnly(char file[][LINELEN], int line);

/**
 * The function to handle text, this includes the text styling and wrapping it within the <p> tag
 * @param[in] file[][LINELEN] an array of character arrays corresponding to file lines
 * @param[in] line the target line starting with '='
 * @param[in] newLine specifies if there should be a new line after the para. This is because in lists or similar setups we don't need para to be in a new line.
 */
void rst_handleText(char file[][LINELEN], int line, bool newLine);

/* The function to handle the '-' character; It figures out if its a <ul> or a <h3>
* @param[in] file[][LINELEN] an array of character arrays corresponding to file lines
* @param[in] line the target line starting with '='
*/
void rst_handleDashAndUl(char file[][LINELEN], int line);

/* The function to handle rst "Field Lists"
* @param[in] file[][LINELEN] an array of character arrays corresponding to file lines
* @param[in] line the target line starting with '='
*/
void rst_handleConfig(char file[][LINELEN], int line);

FILE *infile; 
FILE *outfile;

#endif
