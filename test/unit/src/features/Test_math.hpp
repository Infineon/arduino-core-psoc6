// std includes
#include <Arduino.h>

// test includes
#include "Test_includes.hpp"

// project includes

/**
 * @brief Suite setup function called before any tests in this suite run.
 */
static void math_suiteSetup(void);

/**
 * @brief Suite teardown function called after all tests in this suite run.
 */
static void math_suiteTearDown(void);

// Define test group name
TEST_GROUP(math);
TEST_GROUP(mathInternal);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(mathInternal)
{ 
    // Setup code if needed
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(mathInternal)
{
    // Tear down code if needed
}

/**
 * @brief Test the abs(int value) function.
 */
TEST_IFX(mathInternal, testAbs)
{
    TEST_ASSERT_EQUAL_MESSAGE(10, abs(-10), "absolute value of -10 should be 10");
    TEST_ASSERT_EQUAL_MESSAGE(0, abs(0), "absolute value of 0 should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(20, abs(20), "absolute value of 20 should be 20");
}

/**
 * @brief Test the constrain(int value, int min, int max) function.
 */
TEST_IFX(mathInternal, testConstrain)
{
    TEST_ASSERT_EQUAL_MESSAGE(10, constrain(5, 10, 20), "5 is less than minimum value 10 and should be constrained to 10");   
    TEST_ASSERT_EQUAL_MESSAGE(15, constrain(15, 10, 20), "15 is within the range, so it should remain 15");  
    TEST_ASSERT_EQUAL_MESSAGE(20, constrain(25, 10, 20), "25 is more than maximum value 20 and should be constrained to 20");  
    TEST_ASSERT_EQUAL_MESSAGE(10, constrain(10, 10, 20), "10 is exactly the minimum, so it should remain 10");  
    TEST_ASSERT_EQUAL_MESSAGE(20, constrain(20, 10, 20), "20 is exactly the maximum, so it should remain 20");
}

/**
 * @brief Test the map(long val, long min, long max, long newMin, long newMax) function.
 */
TEST_IFX(mathInternal, testMap)
{
    TEST_ASSERT_EQUAL_MESSAGE(50, map(5, 0, 10, 0, 100), "5 in range (0, 10) should map to 50 in range (0, 100)");      
    TEST_ASSERT_EQUAL_MESSAGE(0, map(0, 0, 10, 0, 100), "0 in range (0, 10) should map to 0 in range (0, 100)");       
    TEST_ASSERT_EQUAL_MESSAGE(100, map(10, 0, 10, 0, 100), "10 in range (0, 10) should map to 100 in range (0, 100)");    
    TEST_ASSERT_EQUAL_MESSAGE(-100, map(10, 0, 10, 100, -100), "0 in range (0, 10) should map to 100 in the new inverted range (100, -100)"); 
    TEST_ASSERT_EQUAL_MESSAGE(0, map(5, 0, 10, -100, 100), "5 in range (0, 10) should map to 0 in range (-100, 100)");    
    TEST_ASSERT_EQUAL_MESSAGE(100, map(20, 10, 30, 0, 200), "20 in range (10, 30) should map to 100 in range (0, 200)");   
}

/**
 * @brief Test the max(int val1, int val2) function.
 */
TEST_IFX(mathInternal, testMax)
{
    TEST_ASSERT_EQUAL_MESSAGE(10, max(5, 10), "max(5, 10) should be 10");
    TEST_ASSERT_EQUAL_MESSAGE(-5, max(-10, -5), "max(-10, -5) should be -5");
    TEST_ASSERT_EQUAL_MESSAGE(0, max(-10, 0), "max(-10, 0) should be 0");
}

/**
 * @brief Test the min(int val1, int val2) function.
 */
TEST_IFX(mathInternal, testMin)
{
    TEST_ASSERT_EQUAL_MESSAGE(5, min(5, 10), "min(5, 10) should be 5");
    TEST_ASSERT_EQUAL_MESSAGE(-10, min(-10, -5), "min(-10, -5) should be -10");
    TEST_ASSERT_EQUAL_MESSAGE(-10, min(-10, 0), "min(-10, 0) should be -10");
}

/**
 * @brief Test the pow(double base, double exponent) function.
 */
TEST_IFX(mathInternal, testPow)
{
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(100.0, pow(10, 2), "pow(10, 2) should be 100.0");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0, pow(10, 0), "pow(10, 0) should be 1.0");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.01, pow(10, -2), "pow(10, -2) should be 0.01");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(27.0, pow(3, 3), "pow(3, 3) should be 27.0");
}

/**
 * @brief Test the sq(int value) function.
 */
TEST_IFX(mathInternal, testSq)
{
    TEST_ASSERT_EQUAL_MESSAGE(25, sq(5), "square of 5 should be 25");
    TEST_ASSERT_EQUAL_MESSAGE(0, sq(0), "square of 0 should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(100, sq(-10), "square of -10 should be 100");
}

/**
 * @brief Test the sqrt(double value) function.
 */
TEST_IFX(mathInternal, testSqrt)
{
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(5.0, sqrt(25.0), "square root of 25 should be 5");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0, sqrt(0.0), "square root of 0.0 should be 0.0");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(10.0, sqrt(100.0), "square root of 100 should be 10");
}

/**
 * @brief Test group runner to run all test cases in this group.
 */
static TEST_GROUP_RUNNER(mathInternal)
{
    RUN_TEST_CASE(mathInternal, testAbs);
    RUN_TEST_CASE(mathInternal, testConstrain);
    RUN_TEST_CASE(mathInternal, testMap);
    RUN_TEST_CASE(mathInternal, testMax);
    RUN_TEST_CASE(mathInternal, testMin);
    RUN_TEST_CASE(mathInternal, testPow);
    RUN_TEST_CASE(mathInternal, testSq);
    RUN_TEST_CASE(mathInternal, testSqrt);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(math)
{
    math_suiteSetup();

    RUN_TEST_GROUP(mathInternal);

    math_suiteTearDown();
}
