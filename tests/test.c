#include <stdio.h>
#include "../lib/unity/unity_internals.h"

void setUp() { }
void tearDown() { }

void test_ini();
void test_markdown();
void test_arena();

int main(void) {
    UNITY_BEGIN();
    test_ini();
#if 0
    test_markdown();
#endif
    test_arena();
    return UNITY_END();
}
