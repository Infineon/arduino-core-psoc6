#!/usr/bin/python3

# python3 runProjectSetup.py


import argparse
import os
import subprocess
import sys
import yaml
import xml.etree.ElementTree as ET

sys.path.insert(1, "tools")

from project_yaml.readProjectYAML import readProjectYAML


def parseArgs():
    # parser = argparse.ArgumentParser(prog = 'checkCode')
    parser = argparse.ArgumentParser()  # prog = 'runProjectSetup')

    parser.add_argument("--getAllChecks", action="store_true", help="getAllChecks help")
    parser.add_argument("--runAllChecks", action="store_true", help="runAllChecks help")
    parser.add_argument("--runCheck", type=str, help="runCheck help")
    parser.add_argument(
        "--parseCppCheckReport", action="store_true", help="parseCppCheckReport help"
    )
    parser.add_argument("--showLog", action="store_true", help="showLog help")

    args = parser.parse_args()  # sys.argv) # [1:])

    return args


if __name__ == "__main__":

    returnCode = 0
    args = parseArgs()

    (projectYAML, userYAML) = readProjectYAML("config/project.yml", "config/user.yml")

    resultPath = "_results"

    if not os.path.isdir(resultPath):
        os.makedirs(resultPath)

    if args.runAllChecks:
        for checkType, checkTypeList in userYAML.items():
            if checkType not in projectYAML:
                print(f"ERROR : {checkType} not found in project YAML !")
                exit(1)

            for check in checkTypeList:
                returnCodeLocal = 0

                if check not in projectYAML[checkType]:
                    print(
                        f"ERROR : {check} not found in project YAML for {checkType} !"
                    )
                    exit(1)

                if checkType == "build":
                    with open(f"{resultPath}/{check}.log", "w") as logfile:
                        returnCodeLocal |= subprocess.run(
                            [
                                "make",
                                "run-build-target",
                                f"FQBN={projectYAML[checkType][check]['fqbn']}",
                                f"TARGET={projectYAML[checkType][check]['target']}",
                            ],
                            stdout=logfile,
                            stderr=logfile,
                        ).returncode

                elif checkType == "check":
                    if "command" not in projectYAML[checkType][check]:
                        print(
                            f"ERROR : 'command' not found in project YAML for {checkType} / {check} !"
                        )
                        exit(1)

                    with open(f"{resultPath}/{check}.log", "w") as logfile:
                        returnCodeLocal |= subprocess.run(
                            projectYAML[checkType][check]["command"].split(),
                            stdout=logfile,
                            stderr=logfile,
                        ).returncode

                if args.showLog:
                    with open(f"{resultPath}/{check}.log", "r") as f:
                        print(f.read())

                if returnCodeLocal != 0:
                    print(f"ERROR : Running check '{check}' failed !")

        # returnCode |= returnCodeLocal

    elif args.runCheck:
        check = args.runCheck
        type = None
        returnCodeLocal = 0

        for checkType, checkTypeList in userYAML.items():
            if check in checkTypeList:
                type = checkType
                break

        if type == None:
            print(f"ERROR : {check} not found in user YAML !")
            exit(1)

        if type == "build":
            with open(f"{resultPath}/{check}.log", "w") as logfile:
                returnCodeLocal |= subprocess.run(
                    [
                        "make",
                        "run-build-target",
                        f"FQBN={projectYAML[type][check]['fqbn']}",
                        f"TARGET={projectYAML[type][check]['target']}",
                    ],
                    stdout=logfile,
                    stderr=logfile,
                ).returncode

        elif type == "check":
            if "command" not in projectYAML[type][check]:
                print(
                    f"ERROR : 'command' not found in project YAML for {type} / {check} !"
                )
                exit(1)

            with open(f"{resultPath}/{check}.log", "w") as logfile:
                returnCodeLocal |= subprocess.run(
                    projectYAML[checkType][check]["command"].split(),
                    stdout=logfile,
                    stderr=logfile,
                ).returncode

        if args.showLog:
            with open(f"{resultPath}/{check}.log", "r") as f:
                print(f.read())

        if returnCodeLocal != 0:
            print(f"ERROR : Running check '{check}' failed !")

        # returnCode |= returnCodeLocal

    elif args.getAllChecks:
        allChecks = 'echo "checks=['

        for checkType, checkTypeList in userYAML.items():
            if checkType not in projectYAML:
                print(f"ERROR : {checkType} not found in project YAML !")
                exit(1)

            for check in checkTypeList:
                if check not in projectYAML[checkType]:
                    print(
                        f"ERROR : {check} not found in project YAML for {checkType} !"
                    )
                    exit(1)

                allChecks += f'\\"{check}\\",'

        allChecks += ']" >> "$GITHUB_OUTPUT"'
        allChecks = allChecks.replace(",]", "]")

        print(f"{allChecks}")
        returnCode = 0

    else:
        print(f"\nERROR : Wrong parameters passed !\n")
        returnCode = 1

    exit(returnCode)
