# include test make file
#include  tests/arduino-core-tests/Makefile



FQBN   ?=
TARGET ?=
UNITY_PATH ?= Unity


##############################################################################################################################################################

clean-results:
	-rm -rf results/cppcheck/*  results/clang-tidy/* results/build/*
	- mkdir -p results/cppcheck results/clang-tidy results/build



##############################################################################################################################################################

run-build-target:
	(cd tests/arduino-core-tests ; make FQBN=$(FQBN) UNITY_PATH=Unity $(TARGET))


##############################################################################################################################################################


run-build-target-all:
	make FQBN=Infineon:xmc:XMC4700_Relax_Kit UNITY_PATH=Unity TARGET=test_wire_connected1_pingpong       run-build-target
	make FQBN=Infineon:xmc:XMC4700_Relax_Kit UNITY_PATH=Unity TARGET=test_wire_connected2_slavepingpong  run-build-target
	make FQBN=Infineon:xmc:XMC4700_Relax_Kit UNITY_PATH=Unity TARGET=test_wire_connected2_masterpingpong run-build-target


run-build-all:
	cd tests/arduino-core-tests ; make FQBN=Infineon:xmc:XMC4700_Relax_Kit UNITY_PATH=Unity test_wire_connected1_pingpong
	cd tests/arduino-core-tests ; make FQBN=Infineon:xmc:XMC4700_Relax_Kit UNITY_PATH=Unity test_wire_connected2_slavepingpong
	cd tests/arduino-core-tests ; make FQBN=Infineon:xmc:XMC4700_Relax_Kit UNITY_PATH=Unity test_wire_connected2_masterpingpong


##############################################################################################################################################################


TAG=push
TAG=latest

IFX_DOCKER_REGISTRY=dockerregistry-v2.vih.infineon.com/ifxmakers/makers-docker:$(TAG)

DOCKER_REGISTRY=ifxmakers/makers-docker:$(TAG)
GHCR_REGISTRY=ghcr.io/infineon/makers-docker:$(TAG)

REGISTRY=$(DOCKER_REGISTRY)

DOCKER=docker run --rm -it -v $(PWD):/myLocalWorkingDir:rw $(REGISTRY)
#DOCKER=


pull-container: 
	docker pull $(REGISTRY)


run-container-build-all: clean-results pull-container
	$(DOCKER) make run-build-target-all


run-container-check-wire: clean-results pull-container
	chmod +x tools/run_cppcheck.sh
	$(DOCKER) tools/run_cppcheck.sh cores/psoc/* tests/arduino-core-tests/src/corelibs/wire
#	firefox results/cppcheck/cppcheck-reports/index.html


run-container-project-setup-script: clean-results pull-container
# $(DOCKER) python3 tools/codeChecks.py --getAllChecks
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-clang-tidy
# $(DOCKER) python3 tools/codeChecks.py --runAllChecks
#	firefox results/cppcheck/cppcheck-reports/index.html


run-container-project-setup-script-with-show-logs: clean-results pull-container
	$(DOCKER) python3 tools/codeChecks.py --getAllChecks
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-clang-tidy --showLog
	$(DOCKER) python3 tools/codeChecks.py --runAllChecks --showLog
#	firefox results/cppcheck/cppcheck-reports/index.html


run-container-cppcheck: clean-results pull-container
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-cppcheck
#	firefox results/cppcheck/cppcheck-reports/index.html

##############################################################################################################################################################

# check container content
run-container-bash: pull-container
	$(DOCKER) 


# run stuff with container from docker hub
run-container-build: clean-results pull-container
	$(DOCKER) make run-build-all




