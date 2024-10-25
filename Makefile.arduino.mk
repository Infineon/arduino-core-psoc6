FQBN  ?=
PORT  ?=
TESTS ?=

$(info FQBN : $(FQBN))
$(info PORT : $(PORT))

UNITY_PATH = ./extras/unity

TESTS_SINGLE_HARDWARE_CONNECTED=-DTEST_SINGLE_HARDWARE_CONNECTED


TESTS_MULTI_HARDWARE_CONNECTED=-DTEST_MULTI_HARDWARE_CONNECTED
		

single_hardware_connected: TESTS=-DTEST_SINGLE_HARDWARE_CONNECTED
multi_hardware_connected: TESTS=-DTEST_MULTI_HARDWARE_CONNECTED


single_hardware_connected \
\
    : unity_corelibs flash


test_all: TESTS=$(TESTS_SINGLE_HARDWARE_CONNECTED) $(TESTS_MULTI_HARDWARE_CONNECTED)
test: TESTS=$(TESTS_SINGLE_HARDWARE_CONNECTED)

test_all \
test: unity_corelibs flash
 

clean:
	-mkdir build
	-rm -rf build/*


arduino: clean
	#find src -name '*.[hc]*' -a \( \! -name '*mtb*' \) -print -exec cp {} build \;


# install Unity from https://www.throwtheswitch.org/unity or git
unity_corelibs: arduino
ifeq ($(UNITY_PATH),)
	$(error "Must set variable UNITY_PATH in order to be able to compile Arduino unit tests !")
else
	find $(UNITY_PATH) -name '*.[hc]' \( -path '*extras*' -a -path '*src*' -or -path '*src*' -a \! -path '*example*' \) -exec \cp {} build \;
	find test/unit -name '*.[hc]*' -a \! -path '*mtb*' -a \! -path '*applibs*' -exec \cp {} build \;
	cp test/unit/src/Test_main.ino build/build.ino
endif



# For WSL and Windows :
# download arduino-cli from : https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Windows_64bit.zip
compile:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
# CAUTION : only use '=' when assigning values to vars, not '+='
	arduino-cli compile --clean --log --warnings all --fqbn $(FQBN) --build-property "compiler.c.extra_flags=\"-DUNITY_INCLUDE_CONFIG_H=1\"" \
                                    		             --build-property compiler.cpp.extra_flags="$(TESTS)" build
endif


compileLTO:
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to compile Arduino sketches !")
else
# compiler.c.extra_flags : switch to -std=c23 whenever XMCLib is conforming; currently neither c99 nor c11 work !
# CAUTION : only use '=' when assigning values to vars, not '+='
	arduino-cli compile --clean --log --warnings all --fqbn $(FQBN) \
							--build-property compiler.c.extra_flags="\"-DUNITY_INCLUDE_CONFIG_H=1\" -DNDEBUG -flto -fno-fat-lto-objects -Wextra -Wall -Wfloat-equal -Wconversion -Wredundant-decls -Wswitch-default -Wdouble-promotion -Wpedantic -Wunreachable-code -fanalyzer -std=c20 " \
							--build-property compiler.cpp.extra_flags="$(TESTS) -DNDEBUG -flto -fno-fat-lto-objects -Wextra -Wall -Wfloat-equal -Wconversion -Wredundant-decls -Wswitch-default -Wdouble-promotion -Wpedantic -Wunreachable-code -fanalyzer -std=c++20 " \
							--build-property compiler.ar.cmd=arm-none-eabi-gcc-ar \
							--build-property compiler.libraries.ldflags=-lstdc++ \
							--build-property compiler.arm.cmsis.path="-isystem{compiler.xmclib_include.path}/XMCLib/inc -isystem{compiler.dsp_include.path} -isystem{compiler.nn_include.path} -isystem{compiler.cmsis_include.path} -isystem{compiler.xmclib_include.path}/LIBS -isystem{build.variant.path} -isystem{build.variant.config_path}" \
							--build-property compiler.usb.path="-isystem{runtime.platform.path}/cores/usblib -isystem{runtime.platform.path}/cores/usblib/Common -isystem{runtime.platform.path}/cores/usblib/Class -isystem{runtime.platform.path}/cores/usblib/Class/Common -isystem{runtime.platform.path}/cores/usblib/Class/Device -isystem{runtime.platform.path}/cores/usblib/Core -isystem{runtime.platform.path}/cores/usblib/Core/XMC4000" \
					build
endif


upload:	
ifeq ($(PORT),)
	$(error "Must set variable PORT (Windows port naming convention, ie COM16) in order to be able to flash Arduino sketches !")
endif
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to flash Arduino sketches !")
else
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) build
endif


flash: compile upload
flashLTO: compileLTO upload


monitor:
ifeq ($(PORT),)
	$(error "Must set variable PORT (Windows port naming convention, ie COM16) in order to be able to flash Arduino sketches !")
endif
ifeq ($(FQBN),)
	$(error "Must set variable FQBN in order to be able to flash Arduino sketches !")
else
	arduino-cli monitor -c baudrate=115200 -p $(PORT) --fqbn $(FQBN)
endif

