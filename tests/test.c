#include <stdio.h>
#include "../lib/unity/unity_internals.h"

void test_function_should_parseSimplestConfig();

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_function_should_parseSimplestConfig);
    return UNITY_END();
}
