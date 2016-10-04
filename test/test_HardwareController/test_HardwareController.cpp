#include "unity.h"
#include "HardwareController.cpp"

#ifdef UNIT_TEST

void test1() {
    HardwareController controller;

    TEST_ASSERT_EQUAL(1, 1);
}
int main() {
    UNITY_BEGIN();    // IMPORTANT LINE!
    RUN_TEST(test1);
    UNITY_END(); // stop unit testing
    return 0;
}

#endif
