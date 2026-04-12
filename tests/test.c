#include <stdio.h>
#include "../lib/unity/unity_internals.h"

void setUp() { }
void tearDown() { }

void test_ini();

int main(void) {
    UNITY_BEGIN();
    test_ini();
    return UNITY_END();
}
