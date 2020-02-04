#include "unity.h"
#include "include/utils.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_function_code_method(void) {
    TEST_ASSERT_TRUE(code_method("getValve") == 10);
    TEST_ASSERT_TRUE(code_method("setValve") == 11);
    TEST_ASSERT_TRUE(code_method("toggleValve") == 12);
    TEST_ASSERT_TRUE(code_method("getFlow") == 20);
    TEST_ASSERT_TRUE(code_method("setRed") == 30);
    TEST_ASSERT_TRUE(code_method("setGreen") == 31);
    TEST_ASSERT_TRUE(code_method("setBlue") == 32);
}

void test_function_encode_params(void) {
    //more test stuff
    char param[50];
    int val = 51;
    double db_val = -1.256;
    char str[10] = "hello";
    encode_params(param, "int", &val);
    TEST_ASSERT_EQUAL_STRING("i51", param);
    encode_params(param, "double", &db_val);
    TEST_ASSERT_EQUAL_STRING("d-1.256", param);
    encode_params(param, "string", str);
    TEST_ASSERT_EQUAL_STRING("shello", param);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_function_code_method);
    RUN_TEST(test_function_encode_params);
    return UNITY_END();
}