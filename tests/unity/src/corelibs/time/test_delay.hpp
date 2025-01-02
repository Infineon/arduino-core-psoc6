// std includes

// test includes
#include "test_includes.hpp"

// project includes


#define TOLERANCE_MS 10 // Tolerance for delay in milliseconds
#define TOLERANCE_US 100 // Tolerance for delay in microseconds

/**
 * @brief Suite setup function called before any tests in this suite run.
 */
static void delay_suiteSetup(void);

/**
 * @brief Suite teardown function called after all tests in this suite run.
 */
static void delay_suiteTearDown(void);

// Define test group name
TEST_GROUP(delay);
TEST_GROUP(delayInternal);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(delayInternal)
{ 
    // Setup code if needed
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(delayInternal)
{
    // Tear down code if needed
}

/**
 * @brief Test the delay function.
 */
TEST_IFX(delayInternal, testDelay)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay = 1000; // 1000 milliseconds

    start_time = millis();
    delay(expected_delay);
    end_time = millis();
    
    unsigned long actual_delay = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_MS, expected_delay, actual_delay);
}

/**
 * @brief Test the delayMicroseconds function.
 */
TEST_IFX(delayInternal, testDelayMicroseconds)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_us = 500000; // 500000 microseconds

    start_time = micros();
    delayMicroseconds(expected_delay_us);
    end_time = micros();
    
    unsigned long actual_delay_us = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, expected_delay_us, actual_delay_us);
}

/**
 * @brief Test the delay function with smallest value of 1ms.
 */
TEST_IFX(delayInternal, testSmallDelay)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay = 1; // 1 millisecond

    start_time = millis();
    delay(expected_delay);
    end_time = millis();
    
    unsigned long actual_delay = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_MS, expected_delay, actual_delay);
}

/**
 * @brief Test the delayMicroseconds function with smallest value of 1 microsecond.
 */
TEST_IFX(delayInternal, testSmallDelayMicroseconds) 
{
    unsigned long start_time, end_time;
    unsigned long expected_delay_us = 1; // 1 microsecond

    start_time = micros();
    delayMicroseconds(expected_delay_us);
    end_time = micros();
    
    unsigned long actual_delay_us = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, expected_delay_us, actual_delay_us);
}

/**
 * @brief Test the delay function for 10 seconds.
 */
TEST_IFX(delayInternal, testLongDelay)
{
    unsigned long start_time, end_time;
    unsigned long expected_delay = 10000; // 10000 milliseconds (10 seconds)

    start_time = millis();
    delay(expected_delay);
    end_time = millis();
    
    unsigned long actual_delay = end_time - start_time;
    
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_MS * 10, expected_delay, actual_delay);
}

/**
 * @brief print the return value of function millis.
 */
TEST_IFX(delayInternal, testPrintMillis)
{
    unsigned long current_millis = millis();
    printf("Current millis: %lu\n", current_millis);
}

/**
 * @brief print the return value of function micros.
 */
TEST_IFX(delayInternal, testPrintMicros)
{
    unsigned long current_micros = micros();
    printf("Current micros: %lu\n", current_micros);
}

/**
 * @brief Test group runner to run all test cases in this group.
 */
static TEST_GROUP_RUNNER(delayInternal)
{
    RUN_TEST_CASE(delayInternal, testDelay);
    RUN_TEST_CASE(delayInternal, testDelayMicroseconds);
    RUN_TEST_CASE(delayInternal, testSmallDelay);
    RUN_TEST_CASE(delayInternal, testSmallDelayMicroseconds);
    RUN_TEST_CASE(delayInternal, testLongDelay);
    RUN_TEST_CASE(delayInternal, testPrintMillis);
    RUN_TEST_CASE(delayInternal, testPrintMicros);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(delay)
{
    delay_suiteSetup();

    RUN_TEST_GROUP(delayInternal);

    delay_suiteTearDown();
}
