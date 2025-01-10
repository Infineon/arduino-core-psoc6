// std includes

// test includes
#include "test_includes.hpp"

// test defines
const uint8_t INVALID_PIN = 255; 

/**
 * @brief Suite setup function called before any tests in this suite run.
 */
static void digital_io_suiteSetup(void);

/**
 * @brief Suite teardown function called after all tests in this suite run.
 */
static void digital_io_suiteTearDown(void);

// Define test group name
TEST_GROUP(digital_io);
TEST_GROUP(digital_io_Internal);

/**
 * @brief Setup method called by Unity before every test in this test group.
 */
static TEST_SETUP(digital_io_Internal)
{ 
    pinMode(TEST_DIGITAL_IO_OUTPUT, OUTPUT);
    pinMode(TEST_DIGITAL_IO_INPUT_1, INPUT);
}

/**
 * @brief Tear down method called by Unity after every test in this test group.
 */
static TEST_TEAR_DOWN(digital_io_Internal)
{
    // Tear down code if needed
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT mode.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_OUTPUT is connected to TEST_DIGITAL_IO_INPUT_1
 *       for the test cases to work as expected.
 */
TEST_IFX(digital_io_Internal, test_digital_io_read_write_input_normal)
{
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT_PULLUP mode.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_OUTPUT is connected to TEST_DIGITAL_IO_INPUT_1
 *       for the test cases to work as expected.
 */
TEST_IFX(digital_io_Internal, test_digital_io_read_write_input_pullup)
{
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when input pin is set to INPUT_PULLDOWN mode.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_OUTPUT is connected to TEST_DIGITAL_IO_INPUT_1
 *       for the test cases to work as expected.
 */
TEST_IFX(digital_io_Internal, test_digital_io_read_write_input_pulldown) 
{
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify digitalWrite and digitalRead functions when output pin is set to OPENDRAIN mode.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_OUTPUT is connected to TEST_DIGITAL_IO_INPUT_1
 *       for the test cases to work as expected.
 */
TEST_IFX(digital_io_Internal, test_digital_io_read_write_output_opendrain)
{
    pinMode(TEST_DIGITAL_IO_OUTPUT, OUTPUT_OPENDRAIN);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to LOW initially");

    digitalWrite(TEST_DIGITAL_IO_OUTPUT, HIGH);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to HIGH");
    digitalWrite(TEST_DIGITAL_IO_OUTPUT, LOW);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should be set to LOW");
}

/**
 * @brief This test is to verify input pin is set to INPUT mode correctly.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_INPUT_2 is not connected to any other pin
 */
TEST_IFX(digital_io_Internal, test_digital_io_input_mode_default)
{
    pinMode(TEST_DIGITAL_IO_INPUT_2, INPUT);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should read LOW initially");  
}

/**
 * @brief This test is to verify input pin is set to INPUT_PULLUP mode correctly.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_INPUT_2 is not connected to any other pin
 */
TEST_IFX(digital_io_Internal, test_digital_io_input_mode_pullup)
{
    pinMode(TEST_DIGITAL_IO_INPUT_2, INPUT_PULLUP);
    TEST_ASSERT_EQUAL_MESSAGE(HIGH, digitalRead(TEST_DIGITAL_IO_INPUT_2), "Input Pin should read HIGH initially due to pull-up"); 
}

/**
 * @brief This test is to verify input pin is set to INPUT_PULLDOWN mode correctly.
 * 
 * @note: Assumption is made that TEST_DIGITAL_IO_INPUT_2 is not connected to any other pin
 */
TEST_IFX(digital_io_Internal, test_digital_io_input_mode_pulldown)
{
    pinMode(TEST_DIGITAL_IO_INPUT_2, INPUT_PULLDOWN);
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(TEST_DIGITAL_IO_INPUT_1), "Input Pin should read LOW initially due to pull-down"); 
}

/**
 * @brief Test invalid pin number handling in pinMode
*/
TEST_IFX(digital_io_Internal, test_pinMode_invalid_pin) {
    pinMode(INVALID_PIN, OUTPUT);
    // No assertion as pinMode doesn't return a value, but ensure no crash
}

/**
 * @brief Test invalid pin number handling in digitalWrite
*/
TEST_IFX(digital_io_Internal, test_digitalWrite_invalid_pin) {
    digitalWrite(INVALID_PIN, HIGH);
    // No assertion as digitalWrite doesn't return a value, but ensure no crash
}

/**
 * @brief Test invalid pin number handling in digitalRead
*/
TEST_IFX(digital_io_Internal, test_digitalRead_invalid_pin) {
    TEST_ASSERT_EQUAL_MESSAGE(LOW, digitalRead(INVALID_PIN), "Should return LOW for invalid pin");
}

/**
 * @brief Test invalid pin mode
*/
TEST_IFX(digital_io_Internal, test_invalid_pinmode) {
    pinMode(TEST_DIGITAL_IO_INPUT_2, 255);
    // No assertion as pinMode doesn't return a value, but ensure no crash
}

/**
 * @brief Test group runner to run all test cases in this group.
 */
static TEST_GROUP_RUNNER(digital_io_Internal)
{
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_read_write_input_normal);
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_read_write_input_pullup);
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_read_write_input_pulldown);
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_read_write_output_opendrain);
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_input_mode_default);
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_input_mode_pullup);
    RUN_TEST_CASE(digital_io_Internal, test_digital_io_input_mode_pulldown);
    RUN_TEST_CASE(digital_io_Internal, test_pinMode_invalid_pin);
    RUN_TEST_CASE(digital_io_Internal, test_digitalWrite_invalid_pin);
    RUN_TEST_CASE(digital_io_Internal, test_digitalRead_invalid_pin);
    RUN_TEST_CASE(digital_io_Internal, test_invalid_pinmode);
}

/**
 * @brief Bundle all tests to be executed for this test group.
 */
TEST_GROUP_RUNNER(digital_io)
{
    digital_io_suiteSetup();

    RUN_TEST_GROUP(digital_io_Internal);

    digital_io_suiteTearDown();
}
