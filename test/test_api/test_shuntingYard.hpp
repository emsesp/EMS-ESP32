
#include <Arduino.h>
#include <unity.h>
#include <HTTPClient.h>
#include "core/shuntingYard.hpp"

void shuntingYard_test1() {
    std::string expected_result = "locale is en";
    std::string test_value      = "\"locale is \"system/settings/locale";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test2() {
    // test with negative value
    std::string expected_result = "rssi is -23";
    std::string test_value      = "\"rssi is \"0+system/network/rssi";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test3() {
    std::string expected_result = "rssi is -23 dBm";
    std::string test_value      = "\"rssi is \"(system/network/rssi)\" dBm\"";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test4() {
    std::string expected_result = "14";
    std::string test_value      = "(custom/seltemp/value)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test5() {
    std::string expected_result = "seltemp=14";
    std::string test_value      = "\"seltemp=\"(custom/seltemp/value)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test6() {
    std::string expected_result = "14";
    std::string test_value      = "(custom/seltemp)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test7() {
    std::string expected_result = "40";
    std::string test_value      = "boiler/flowtempoffset";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test8() {
    std::string expected_result = "40";
    std::string test_value      = "(boiler/flowtempoffset/value)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test9() {
    std::string expected_result = "53.8";
    std::string test_value      = "(boiler/storagetemp1/value)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test10() {
    // (14 - 40) * 2.8 + 5 = -67.8
    std::string expected_result = "-67.8";
    std::string test_value      = "(custom/seltemp - boiler/flowtempoffset) * 2.8 + 5";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test11() {
    std::string expected_result = "4";
    std::string test_value      = "1 > 2 ? 3 : 4";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test12() {
    std::string expected_result = "3";
    std::string test_value      = "1 < 2 ? 3 : 4";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test13() {
    std::string expected_result = "5";
    std::string test_value      = "1<2?(3<4?5:6):7";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test14() {
    std::string expected_result = "7";
    std::string test_value      = "1>2?(3<4?5:6):7";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test15() {
    std::string expected_result = "6";
    std::string test_value      = "1<2?(3>4?5:6):7";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test16() {
    std::string expected_result = "3";
    std::string test_value      = "1<2?3:(4<5?6:7)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test17() {
    std::string expected_result = "6";
    std::string test_value      = "1>2?3:(4<5?6:7)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test18() {
    std::string expected_result = "7";
    std::string test_value      = "1>2?3:(4>5?6:7)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test19() {
    std::string expected_result = "44";
    std::string test_value      = "(1>2?3:4)+(10>20?30:40)";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test20() {
    std::string expected_result = "8";
    std::string test_value      = "1<2 ? 3>4 ? 5 : 6<7 ? 8 : 9 : 10";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
}

void shuntingYard_test21() {
    std::string expected_result = "1";
    std::string test_value      = "boiler/storagetemp2 == \"\"";
    TEST_ASSERT_EQUAL_STRING(expected_result.c_str(), compute(test_value).c_str());
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
}
