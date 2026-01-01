#include "../src/kevlar_markdown.h"
#include "../src/utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_check_count_and_type(Md_Ast *ast, size_t c_count, Md_Node_Type typ) {
    if (ast->node_type != typ) {
        puts("NodeType mismatch");
        printf("\tWANTED: %s\n", utl_node_type_to_str(typ));
        printf("\tGOT: %s\n", utl_node_type_to_str(ast->node_type));
        assert(0 && "NodeType mismatch");
    }

    if (ast->c_count != c_count) {
        puts("c_count mismatch");
        printf("\tWANTED: %zu\n", c_count);
        printf("\tGOT: %zu\n", ast->c_count);
        assert(0 && "c_count mismatch");
    }
}

void test_match_text_node_text(Md_Ast *txt_node, const char *dest) {
    assert(txt_node->node_type == MD_NODE_TEXT);

    if (strcmp(txt_node->opt.text_opt.data, dest) != 0) {
        printf("\tWANTED: \"%s\"\n", dest);
        printf("\tGOT: \"%s\"\n", txt_node->opt.text_opt.data);
        assert(0 && "txt_node content did not match data");
    }
}

void test_md_heading() {
    Md_Ast *ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("# Foo\n");
    test_check_count_and_type(ast->children[0], 1, MD_NODE_HEADING);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "Foo");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("###");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C");
    ast = kevlar_md_generate_ast("##### A");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_HEADING);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "A");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("########## Should NOT be a heading");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "########## Should NOT be a heading");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("## ");

    test_check_count_and_type(ast->children[0], 0, MD_NODE_HEADING);

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test F");
    ast = kevlar_md_generate_ast("### ## ### # ## # # # # ## ##### ");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_HEADING);
    test_match_text_node_text(ast->children[0]->children[0], "## ### # ## # # # # ## ##### ");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("#\t\t\t\tHello");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_HEADING);
    test_match_text_node_text(ast->children[0]->children[0], "Hello");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test F");
    ast = kevlar_md_generate_ast("# ये हाथ मुझे दे दे ठाकुर!");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_HEADING);
    assert(ast->children[0]->opt.h_opt.level == 1);
    test_match_text_node_text(ast->children[0]->children[0], "ये हाथ मुझे दे दे ठाकुर!");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test G");
    ast = kevlar_md_generate_ast("# H1\n## H2\n### H3\n#### H4\n##### H5\n###### H6");
    test_check_count_and_type(ast, 6, MD_NODE_ROOT);
    for (int i = 0; i < 6; i++) {
        test_check_count_and_type(ast->children[i], 1, MD_NODE_HEADING);
        assert(ast->children[i]->opt.h_opt.level == i + 1);
    }
    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_md_emphasis_and_strong() {
    Md_Ast *ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("*Be *dazz* led*");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Be ");

    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "dazz");

    test_check_count_and_type(ast->children[0]->children[0]->children[2], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[2], " led");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("Hello, *World*!");

    /*
     * - Para
     * -- text
     * -- em
     * --- text
     * -- text
     */

    test_check_count_and_type(ast->children[0], 3, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "Hello, ");

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "World");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[2], "!");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C");
    ast = kevlar_md_generate_ast("*नमस्ते* दुनिया! 👋🌏");

    /*
     * - Para
     * -- em
     * --- text
     * -- text
     */

    test_check_count_and_type(ast->children[0], 2, MD_NODE_PARAGRAPH);

    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "नमस्ते");

    test_check_count_and_type(ast->children[0]->children[1], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[1], " दुनिया! 👋🌏");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C/1");
    ast = kevlar_md_generate_ast("The **quick *brown* fox** jumps __over the lazy dog__");
    test_check_count_and_type(ast->children[0], 4, MD_NODE_PARAGRAPH);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "The ");

    test_check_count_and_type(ast->children[0]->children[1], 3, MD_NODE_STRONG);
    /**/ test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_NODE_TEXT);
    /**/ test_match_text_node_text(ast->children[0]->children[1]->children[0], "quick ");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C/2");
    ast = kevlar_md_generate_ast("**_Hello World_**");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0],
                              "Hello World");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C/3");
    ast = kevlar_md_generate_ast("***A***");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "A");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("The _quick_ *brown* **fox** ***jumps*** over the lazy dog");

    // "The "
    // "quick"
    // " "
    // "brown"
    // " "
    // "fox"
    // " "
    // "jumps"
    // " over the lazy dog"

    test_check_count_and_type(ast->children[0], 9, MD_NODE_PARAGRAPH);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "The ");

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "quick");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[2], " ");

    test_check_count_and_type(ast->children[0]->children[3], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[3]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[3]->children[0], "brown");

    test_check_count_and_type(ast->children[0]->children[4], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[4], " ");

    test_check_count_and_type(ast->children[0]->children[5], 1, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[5]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[5]->children[0], "fox");

    test_check_count_and_type(ast->children[0]->children[7], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[7]->children[0], 1, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[7]->children[0]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[7]->children[0]->children[0], "jumps");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("**Bold with *nested emphasis* inside**");
    /*
     * - Para
     * -- Strong
     * --- Text "Bold with "
     * --- Em
     * ---- Text "nested emphasis"
     * --- Text " inside"
     */
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "Bold with ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[1]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
                              "nested emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test F");
    ast = kevlar_md_generate_ast("This has *unclosed emphasis");
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "This has *unclosed emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test G");
    ast = kevlar_md_generate_ast("*no space*immediately*adjacent*");
    test_check_count_and_type(ast->children[0], 3, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_EMPH);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "no space");
    test_check_count_and_type(ast->children[0]->children[1], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[1], "immediately");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[2]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "adjacent");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test H");
    ast = kevlar_md_generate_ast("***I got an appointment with _Dr. Monster_***");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 2, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0],
                              "I got an appointment with ");

    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[1], 1,
                              MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[0]->children[0]->children[1]->children[0],
                              0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[1]->children[0],
                              "Dr. Monster");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    /*************************************/
    puts("Test I");
    ast = kevlar_md_generate_ast("*日本語*, **中文**, ***العربية***, עִברִית");

    test_check_count_and_type(ast->children[0], 6, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_EMPH);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "日本語");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_NODE_STRONG);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "中文");

    test_check_count_and_type(ast->children[0]->children[4], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[4]->children[0], 1, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[4]->children[0]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[4]->children[0]->children[0], "العربية");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test J");
    ast = kevlar_md_generate_ast("*🔥fire🔥* **💪strong💪** ***🚀rocket🚀***");
    test_check_count_and_type(ast->children[0], 5, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_EMPH);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "🔥fire🔥");
    test_check_count_and_type(ast->children[0]->children[2], 1, MD_NODE_STRONG);
    test_match_text_node_text(ast->children[0]->children[2]->children[0], "💪strong💪");

    test_check_count_and_type(ast->children[0]->children[4], 1, MD_NODE_EMPH);
    test_check_count_and_type(ast->children[0]->children[4]->children[0], 1, MD_NODE_STRONG);
    test_check_count_and_type(ast->children[0]->children[4]->children[0]->children[0], 0,
                              MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[4]->children[0]->children[0],
                              "🚀rocket🚀");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test K");
    ast = kevlar_md_generate_ast("\\*not emphasis\\* but *this is*");
    test_check_count_and_type(ast->children[0], 2, MD_NODE_PARAGRAPH);
    test_match_text_node_text(ast->children[0]->children[0], "*not emphasis* but ");
    test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_EMPH);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "this is");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test L");
    ast = kevlar_md_generate_ast("     \t\t\t     ");
    test_check_count_and_type(ast->children[0], 0, MD_NODE_PARAGRAPH);
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test M");
    char long_text[10000];
    memset(long_text, 'a', 9999);
    long_text[9999] = '\0';
    ast = kevlar_md_generate_ast(long_text);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    kevlar_md_free_ast(ast);
    /*************************************/
}

void test_md_code_blocks() {
    Md_Ast *ast;

    /*************************************/
    puts("Test A");
    ast = kevlar_md_generate_ast("`Namaste, Arigato, Shalom, Hello`");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_INLINE_CODE);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0],
                              "Namaste, Arigato, Shalom, Hello");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test B");
    ast = kevlar_md_generate_ast("\\`Pls escape lol\\`");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "`Pls escape lol`");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test C");
    ast = kevlar_md_generate_ast("Foo ``Bar`` Baz");

    test_check_count_and_type(ast->children[0], 3, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "Foo ");

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_INLINE_CODE);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[1]->children[0], "Bar");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[2], " Baz");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test D");
    ast = kevlar_md_generate_ast("Foo ``**The formatting** _is def_, ***disabled....***`` Baz");

    test_check_count_and_type(ast->children[0], 3, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "Foo ");

    test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_INLINE_CODE);
    test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[1]->children[0],
                              "**The formatting** _is def_, ***disabled....***");

    test_check_count_and_type(ast->children[0]->children[2], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[2], " Baz");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test E");
    ast = kevlar_md_generate_ast("```Unmatched pairs should NOT close``");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0],
                              "```Unmatched pairs should NOT close``");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test F");
    ast = kevlar_md_generate_ast("```python\n"
                                 "print(\"Hello World\")```");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_CODE_BLOCK);
    assert(strcmp(ast->children[0]->opt.code_block_opt.lang_str, "python") == 0);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "print(\"Hello World\")");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test G");
    ast = kevlar_md_generate_ast("```python\n"
                                 "print(\"Hello World\")\n\n"
                                 "print(\"When the imposter is sus 😈\")\n"
                                 "```");

    test_check_count_and_type(ast->children[0], 1, MD_NODE_CODE_BLOCK);
    assert(strcmp(ast->children[0]->opt.code_block_opt.lang_str, "python") == 0);

    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0],
                              "print(\"Hello World\")\n\nprint(\"When the imposter is sus 😈\")\n");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test H");
    ast = kevlar_md_generate_ast("Example:\n"
                                 "```python\n"
                                 "print(\"Hello World\")\n"
                                 "```\n"
                                 "That is how we write hello world in python");

    test_check_count_and_type(ast, 3, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "Example:\n");

    test_check_count_and_type(ast->children[1], 1, MD_NODE_CODE_BLOCK);
    assert(strcmp(ast->children[1]->opt.code_block_opt.lang_str, "python") == 0);
    test_check_count_and_type(ast->children[1]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[1]->children[0], "print(\"Hello World\")\n");

    test_check_count_and_type(ast->children[2], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[2]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[2]->children[0],
                              "That is how we write hello world in python");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test H/1");
    ast = kevlar_md_generate_ast("Here is some code for you:\n"
                                 "```python\n"
                                 "print(\"Hello World\")\n\n"
                                 "print(\"When the imposter is sus 😈\")\n"
                                 "```\n"
                                 "Got it? Good. Now better get to work");

    test_check_count_and_type(ast, 3, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0], "Here is some code for you:\n");

    test_check_count_and_type(ast->children[1], 1, MD_NODE_CODE_BLOCK);
    assert(strcmp(ast->children[1]->opt.code_block_opt.lang_str, "python") == 0);
    test_check_count_and_type(ast->children[1]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[1]->children[0],
                              "print(\"Hello World\")\n\nprint(\"When the imposter is sus 😈\")\n");

    test_check_count_and_type(ast->children[2], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[2]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[2]->children[0],
                              "Got it? Good. Now better get to work");

    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test I");
    ast = kevlar_md_generate_ast("Escaped!!!\n"
                                 "```python\n"
                                 "print(\"Hello World\")\n"
                                 "\\`\\`\\`\n");

    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);

    kevlar_md_free_ast(ast);
    /*************************************/
}


void test_md_strikethru() {
    Md_Ast* ast;

    /*************************************/
    puts("Test: Strikethrough Alone");
    ast = kevlar_md_generate_ast("~~deleted text~~");
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_STRIKETHRU);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Strikethrough with Emphasis");
    ast = kevlar_md_generate_ast("~~deleted *with emphasis*~~");
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 2, MD_NODE_STRIKETHRU);
    test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
    test_match_text_node_text(ast->children[0]->children[0]->children[0], "deleted ");
    test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_NODE_EMPH);
    test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0],
                              "with emphasis");
    kevlar_md_free_ast(ast);
    /*************************************/


}

void test_md_links() {
    Md_Ast* ast;

#if 1
    /*************************************/
    puts("Test: Simple Link");
    ast = kevlar_md_generate_ast("[link text](https://example.com)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("https://example.com"));
        assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "https://example.com",
               ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "link text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link in Sentence");
    ast = kevlar_md_generate_ast("Check out [this site](https://example.com) for more info.");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 3, MD_NODE_PARAGRAPH);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0], "Check out ");
        test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_LINK);
            assert(ast->children[0]->children[1]->opt.link_opt.href_len == strlen("https://example.com"));
            assert(strncmp(ast->children[0]->children[1]->opt.link_opt.href_str, "https://example.com",
                   ast->children[0]->children[1]->opt.link_opt.href_len) == 0);
            test_check_count_and_type(ast->children[0]->children[1]->children[0], 0, MD_NODE_TEXT);
            test_match_text_node_text(ast->children[0]->children[1]->children[0], "this site");
        test_check_count_and_type(ast->children[0]->children[2], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[2], " for more info.");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Multiple Formatting");
    ast = kevlar_md_generate_ast("[*em* **strong** `code`](url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 5, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 1, MD_NODE_EMPH);
            test_match_text_node_text(ast->children[0]->children[0]->children[0]->children[0], "em");
        test_check_count_and_type(ast->children[0]->children[0]->children[1], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0]->children[1], " ");
        test_check_count_and_type(ast->children[0]->children[0]->children[2], 1, MD_NODE_STRONG);
            test_match_text_node_text(ast->children[0]->children[0]->children[2]->children[0], "strong");
        test_check_count_and_type(ast->children[0]->children[0]->children[3], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0]->children[3], " ");
        test_check_count_and_type(ast->children[0]->children[0]->children[4], 1, MD_NODE_INLINE_CODE);
            test_match_text_node_text(ast->children[0]->children[0]->children[4]->children[0], "code");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Emphasis Around Link");
    ast = kevlar_md_generate_ast("*emphasis [link](url) text*");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 3, MD_NODE_EMPH);
        test_check_count_and_type(ast->children[0]->children[0]->children[0], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "emphasis ");
        test_check_count_and_type(ast->children[0]->children[0]->children[1], 1, MD_NODE_LINK);
            assert(ast->children[0]->children[0]->children[1]->opt.link_opt.href_len == strlen("url"));
            test_match_text_node_text(ast->children[0]->children[0]->children[1]->children[0], "link");
        test_check_count_and_type(ast->children[0]->children[0]->children[2], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0]->children[2], " text");
    kevlar_md_free_ast(ast);
    /*************************************/

    // /*************************************/
    // puts("Test: Link with Empty URL");
    // ast = kevlar_md_generate_ast("[link text]()");
    // test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    // test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    // test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
    //     assert(ast->children[0]->children[0]->opt.link_opt.href_len == 0);
    //     test_match_text_node_text(ast->children[0]->children[0]->children[0], "link text");
    // kevlar_md_free_ast(ast);
    // /*************************************/

    /*************************************/
    puts("Test: Link with Query Parameters");
    ast = kevlar_md_generate_ast("[text](https://example.com?foo=bar&baz=qux)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("https://example.com?foo=bar&baz=qux"));
        assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "https://example.com?foo=bar&baz=qux",
               ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Adjacent Links");
    ast = kevlar_md_generate_ast("[first](url1)[second](url2)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 2, MD_NODE_PARAGRAPH);
        test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
            assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url1"));
            test_match_text_node_text(ast->children[0]->children[0]->children[0], "first");
        test_check_count_and_type(ast->children[0]->children[1], 1, MD_NODE_LINK);
            assert(ast->children[0]->children[1]->opt.link_opt.href_len == strlen("url2"));
            test_match_text_node_text(ast->children[0]->children[1]->children[0], "second");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Link Bracket");
    ast = kevlar_md_generate_ast("[unclosed link(url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0], "[unclosed link(url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unclosed Link Parenthesis");
    ast = kevlar_md_generate_ast("[text](unclosed");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0], "[text](unclosed");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Space Between Bracket and Parenthesis");
    ast = kevlar_md_generate_ast("[text] (url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
        test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_TEXT);
        test_match_text_node_text(ast->children[0]->children[0], "[text] (url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Bracket in Link Text");
    ast = kevlar_md_generate_ast("[text \\[with bracket\\]](url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "text [with bracket]");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Escaped Parenthesis in URL");
    ast = kevlar_md_generate_ast("[text](url\\(with\\)parens)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url(with)parens"));
        assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "url(with)parens",
               ast->children[0]->children[0]->opt.link_opt.href_len) == 0);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/
#endif

    /*************************************/
    puts("Test: Link with Newline in Text - Soft Break");
    ast = kevlar_md_generate_ast("[link\ntext](url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "link\ntext");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link Cannot Span Paragraphs");
    ast = kevlar_md_generate_ast("[text\n\nmore](url)");
    test_check_count_and_type(ast, 2, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
        test_match_text_node_text(ast->children[0]->children[0], "[text");
    test_check_count_and_type(ast->children[1], 1, MD_NODE_PARAGRAPH);
        test_match_text_node_text(ast->children[1]->children[0], "more](url)");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode in Link Text");
    ast = kevlar_md_generate_ast("[日本語 🌏](url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "日本語 🌏");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Unicode in URL");
    ast = kevlar_md_generate_ast("[text](https://例え.jp/パス)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("https://例え.jp/パス"));
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    kevlar_md_free_ast(ast);
    /*************************************/

    // TODO: this requires a stack based parser which we do not have as of yet
    // /*************************************/
    // puts("Test: Link with Parentheses in URL - Unescaped");
    // ast = kevlar_md_generate_ast("[text](https://en.wikipedia.org/wiki/C_(programming_language))");
    // utl_visualize_ast(ast, 0);
    // test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    // test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    // test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
    //     // Parser should handle balanced parens in URL
    //     assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("https://en.wikipedia.org/wiki/C_(programming_language)"));
    //     test_match_text_node_text(ast->children[0]->children[0]->children[0], "text");
    // kevlar_md_free_ast(ast);
    // /*************************************/

    /*************************************/
    puts("Test: Long URL");
    ast = kevlar_md_generate_ast("[link](https://example.com/very/long/path/with/many/segments/and/query?param1=value1&param2=value2&param3=value3#fragment)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len > 50);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "link");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Just Fragment URL");
    ast = kevlar_md_generate_ast("[top](#)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 1, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == 1);
        assert(strncmp(ast->children[0]->children[0]->opt.link_opt.href_str, "#", 1) == 0);
        test_match_text_node_text(ast->children[0]->children[0]->children[0], "top");
    kevlar_md_free_ast(ast);
    /*************************************/

    /*************************************/
    puts("Test: Link with Only Whitespace in Text");
    ast = kevlar_md_generate_ast("[   ](url)");
    test_check_count_and_type(ast, 1, MD_NODE_ROOT);
    test_check_count_and_type(ast->children[0], 1, MD_NODE_PARAGRAPH);
    test_check_count_and_type(ast->children[0]->children[0], 0, MD_NODE_LINK);
        assert(ast->children[0]->children[0]->opt.link_opt.href_len == strlen("url"));
    kevlar_md_free_ast(ast);
    /*************************************/
}


void test_md_basic() {
    Md_Ast *ast = kevlar_md_generate_ast("Hello, World!");

    assert(ast->node_type == MD_NODE_ROOT && "AST must have a root node");

    kevlar_md_free_ast(ast);
}

void test_markdown() {
#if 1
    puts("INFO: test_md_basic()");
    test_md_basic();
    puts("SUCCESS: test_md_basic()");

    puts("INFO: test_md_emphasis_and_strong()");
    test_md_emphasis_and_strong();
    puts("SUCCESS: test_md_emphasis_and_strong()");

    puts("INFO: test_md_heading()");
    test_md_heading();
    puts("SUCCESS: test_md_heading()");

    puts("INFO: test_md_strikethru()");
    test_md_strikethru();
    puts("SUCCESS: test_md_strikethru()");

    puts("INFO: test_md_code_blocks()");
    test_md_code_blocks();
    puts("SUCCESS: test_md_code_blocks()");
#endif

    puts("INFO: test_md_links()");
    test_md_links();
    puts("SUCCESS: test_md_links()");

};
