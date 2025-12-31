
#include <Arduino.h>
#include <unity.h>
#include "core/shuntingYard.h"

void run_shuntingYard_test(const std::string & expected, const std::string & actual) {
    TEST_ASSERT_EQUAL_STRING(expected.c_str(), emsesp::compute(actual).c_str());
}

void shuntingYard_test1() {
    run_shuntingYard_test("locale is en", "\"locale is \"system/settings/locale");
}

void shuntingYard_test2() {
    run_shuntingYard_test("rssi is -23", "\"rssi is \"0+system/network/rssi");
}

void shuntingYard_test3() {
    run_shuntingYard_test("rssi is -23 dBm", "\"rssi is \"(system/network/rssi)\" dBm\"");
}

void shuntingYard_test4() {
    run_shuntingYard_test("14", "(custom/test_seltemp/value)");
}

void shuntingYard_test5() {
    run_shuntingYard_test("seltemp=14", "\"seltemp=\"(custom/test_seltemp/value)");
}

void shuntingYard_test6() {
    run_shuntingYard_test("14", "custom/test_seltemp");
}

void shuntingYard_test7() {
    run_shuntingYard_test("40", "boiler/flowtempoffset");
}

void shuntingYard_test8() {
    run_shuntingYard_test("40", "(boiler/flowtempoffset/value)");
}

void shuntingYard_test9() {
    run_shuntingYard_test("53.8", "(boiler/storagetemp1/value)");
}

void shuntingYard_test10() {
    run_shuntingYard_test("-67.8", "(custom/test_seltemp - boiler/flowtempoffset) * 2.8 + 5");
}

void shuntingYard_test11() {
    run_shuntingYard_test("4", "1 > 2 ? 3 : 4");
}

void shuntingYard_test12() {
    run_shuntingYard_test("3", "1 < 2 ? 3 : 4");
}

void shuntingYard_test13() {
    run_shuntingYard_test("5", "1<2?(3<4?5:6):7");
}

void shuntingYard_test14() {
    run_shuntingYard_test("7", "1>2?(3<4?5:6):7");
}

void shuntingYard_test15() {
    run_shuntingYard_test("6", "1<2?(3>4?5:6):7");
}

void shuntingYard_test16() {
    run_shuntingYard_test("3", "1<2?3:(4<5?6:7)");
}

void shuntingYard_test17() {
    run_shuntingYard_test("6", "1>2?3:(4<5?6:7)");
}

void shuntingYard_test18() {
    run_shuntingYard_test("7", "1>2?3:(4>5?6:7)");
}

void shuntingYard_test19() {
    run_shuntingYard_test("44", "(1>2?3:4)+(10>20?30:40)");
}

void shuntingYard_test20() {
    run_shuntingYard_test("8", "1<2 ? 3>4 ? 5 : 6<7 ? 8 : 9 : 10");
}

void shuntingYard_test21() {
    run_shuntingYard_test("", "boiler/storagetemp2 == \"\"");
}

void shuntingYard_test22() {
    run_shuntingYard_test("", "boiler/storagetemp2 == ''");
}

void shuntingYard_test23() {
    run_shuntingYard_test("9", "custom/test_ram");
}

void shuntingYard_test24() {
    run_shuntingYard_test("hello world!", "\"hello world!\"");
}

void shuntingYard_test25() {
    run_shuntingYard_test("hello world!", "'hello world!'");
}

void run_shuntingYard_tests() {
    RUN_TEST(shuntingYard_test1);
    RUN_TEST(shuntingYard_test2);
    RUN_TEST(shuntingYard_test3);
    RUN_TEST(shuntingYard_test4);
    RUN_TEST(shuntingYard_test5);
    RUN_TEST(shuntingYard_test6);
    RUN_TEST(shuntingYard_test7);
    RUN_TEST(shuntingYard_test8);
    RUN_TEST(shuntingYard_test9);
    RUN_TEST(shuntingYard_test10);
    RUN_TEST(shuntingYard_test11);
    RUN_TEST(shuntingYard_test12);
    RUN_TEST(shuntingYard_test13);
    RUN_TEST(shuntingYard_test14);
    RUN_TEST(shuntingYard_test15);
    RUN_TEST(shuntingYard_test16);
    RUN_TEST(shuntingYard_test17);
    RUN_TEST(shuntingYard_test18);
    RUN_TEST(shuntingYard_test19);
    RUN_TEST(shuntingYard_test20);
    RUN_TEST(shuntingYard_test21);
    RUN_TEST(shuntingYard_test22);
    RUN_TEST(shuntingYard_test23);
    RUN_TEST(shuntingYard_test24);
    RUN_TEST(shuntingYard_test25);
}
