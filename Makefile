# include test make file
#include  tests/arduino-core-tests/Makefile



FQBN   ?= infineon:psoc:cy8ckit_062s2_ai
TARGET ?= test_interrupts_single


##############################################################################################################################################################

clean-results:
	-rm -rf results/cppcheck/*  results/clang-tidy/* results/build/*
	-mkdir -p results/cppcheck results/clang-tidy results/build

##############################################################################################################################################################

run-build-target:
	(cd tests/arduino-core-tests ; make FQBN=$(FQBN) $(TARGET))

##############################################################################################################################################################


TAG=push
TAG=latest

IFX_DOCKER_REGISTRY=dockerregistry-v2.vih.infineon.com/ifxmakers/makers-docker:$(TAG)

DOCKER_REGISTRY=ifxmakers/makers-docker:$(TAG)
GHCR_REGISTRY=ghcr.io/infineon/makers-docker:$(TAG)

REGISTRY=$(DOCKER_REGISTRY)

DOCKER=docker run --rm -it -v $(PWD):/myLocalWorkingDir:rw $(REGISTRY)


pull-container: 
	docker pull $(REGISTRY)


run-container-project-setup-script: clean-results pull-container
	$(DOCKER) python3 tools/codeChecks.py --getAllChecks
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-clang-tidy
	$(DOCKER) python3 tools/codeChecks.py --runAllChecks
#	firefox results/cppcheck/cppcheck-reports/index.html


run-container-project-setup-script-with-show-logs: clean-results pull-container
	$(DOCKER) python3 tools/codeChecks.py --getAllChecks
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-clang-tidy --showLog
	$(DOCKER) python3 tools/codeChecks.py --runAllChecks --showLog
#	firefox results/cppcheck/cppcheck-reports/index.html


run-container-cppcheck: pull-container
	-rm -rf results/cppcheck/* 
	-mkdir -p results/cppcheck
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-cppcheck
#	firefox results/cppcheck/cppcheck-reports/index.html

run-container-clang-tidy-check: pull-container
	-rm -rf results/clang-tidy/* 
	-mkdir -p results/clang-tidy 
	$(DOCKER) python3 tools/codeChecks.py --runCheck check-clang-tidy 

run-container-clang-tidy-format: pull-container
	-rm -rf results/clang-tidy-format/* 
	-mkdir -p results/clang-tidy-format
	$(DOCKER) python3 tools/codeChecks.py --runCheck clang-format

##############################################################################################################################################################

# run stuff with container from docker hub
run-container-build: clean-results pull-container
	(cd tests/arduino-core-tests ; make FQBN=$(FQBN) $(TARGET))




