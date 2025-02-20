# include test make file
#include  tests/arduino-core-tests/Makefile



FQBN   ?= infineon:psoc:cy8ckit_062s2_ai
TARGET ?= test_interrupts_single


##############################################################################################################################################################

clean-results:
	-rm -rf _results/cppcheck/*  _results/clang-tidy/* _results/build/*
	-mkdir -p _results/cppcheck _results/clang-tidy _results/build

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

run-container-check-all: clean-results pull-container
	$(DOCKER) python3 tools/code_checks/codeChecks.py --getAllChecks
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runAllChecks

run-container-project-setup-script-with-show-logs: clean-results pull-container
	$(DOCKER) python3 tools/code_checks/codeChecks.py --getAllChecks
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runCheck check-clang-tidy
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runAllChecks --showLog

run-container-cppcheck: pull-container
	-rm -rf _results/cppcheck/* 
	-mkdir -p _results/cppcheck
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runCheck check-cppcheck --showLog
#	firefox _results/cppcheck/cppcheck-reports/index.html

run-container-clang-tidy-check: pull-container
	-rm -rf _results/clang-tidy/* 
	-mkdir -p _results/clang-tidy 
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runCheck check-clang-tidy --showLog

run-container-clang-tidy-check-fix: pull-container
	-rm -rf _results/clang-tidy/* 
	-mkdir -p _results/clang-tidy 
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runCheck check-clang-tidy-fix

run-container-clang-tidy-format: pull-container
	$(DOCKER) python3 tools/code_checks/codeChecks.py --runCheck clang-format --showLog

run-container-black-format:
	python3 tools/code_checks/codeChecks.py --runCheck black-format

run-container-analyze_cppcheck:
	python3 tools/code_checks/analyze_cppcheck.py

##############################################################################################################################################################

# run stuff with container from docker hub
run-build-target: 
	(cd tests/arduino-core-tests ; make compile FQBN=$(FQBN) $(TARGET))

