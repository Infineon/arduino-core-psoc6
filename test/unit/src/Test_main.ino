// test includes
#include "Test_includes.hpp"


/***
     * Uses flags / defines to determine which groups of tests should be added to this test such that multiple tests CAN be executed in a single run.
     * Use the '--build-property option of the 'arduino_compile' target to add the flags by defining the respective targets in the makefile.
     * makefile : --build-property "compiler.cpp.extra_flags=\"-D<TEST_SPECIFIER>=1\"" build
*/
void RunAllTests(void)
{

    RUN_TEST_GROUP(serial_uart);
    RUN_TEST_GROUP(math);

}


//
void setup() {
    // Serial.begin(115200);
    // Serial.println("setup done.");
    printf("setup done.");
}


//
void loop() {
    // Serial.println("\n");
    printf("\n");

    const int   argc       = 2;
    const char *argv[argc] = { "", "-v" };

    (void) UnityMain(argc, argv, RunAllTests);
}