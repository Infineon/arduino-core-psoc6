// std includes

// test includes
#include "Test_includes.hpp"

// project includes


static void serial_uart_suiteSetup(void);
static void serial_uart_suiteTearDown(void);


// variables used in the tests below that have to be accessed in the setup and tear down methods


// test includes that may require dut


// define test group name
TEST_GROUP(serial_uart);
TEST_GROUP(serial_uartInternal);


// Setup method called by Unity before every individual test defined for this test group.
static TEST_SETUP(serial_uartInternal)
{ 
}


// Tear down method called by Unity after every individual test defined for this test group.
static TEST_TEAR_DOWN(serial_uartInternal)
{
}



TEST_IFX(serial_uartInternal, checkPingPong)
{
    
}


static TEST_GROUP_RUNNER(serial_uartInternal)
{
    RUN_TEST_CASE(serial_uartInternal, checkPingPong);
}


// Bundle all tests to be executed for this test group
TEST_GROUP_RUNNER(serial_uart)
{
    serial_uart_suiteSetup();

    RUN_TEST_GROUP(serial_uartInternal);
 
    serial_uart_suiteTearDown();
}
