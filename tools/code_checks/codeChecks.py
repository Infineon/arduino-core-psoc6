#!/usr/bin/python3

# python3 codeChecks.py


import argparse
import subprocess
import sys


sys.path.insert(1, "tools")

from project_yaml.readProjectYAML import readProjectYAML


def parseArgs():
    parser = argparse.ArgumentParser()

    parser.add_argument("--getAllChecks", action="store_true", help="getAllChecks help")
    parser.add_argument("--runAllChecks", action="store_true", help="runAllChecks help")
    parser.add_argument("--runCheck", type=str, help="runCheck help")

    args = parser.parse_args()

    return args


def runCheck(checkType=None, check=None):
    returnCode = 0

    if checkType == None:
        print(f"ERROR : Check type 'None' is not valid !")
        returnCode = 1

    elif check == None:
        print(f"ERROR : Check 'None' is not valid !")
        returnCode = 1

    elif checkType == "build":
        returnCode |= subprocess.run(
            [
                "make",
                "run-build-target",
                f"FQBN={projectYAML[checkType][check]['fqbn']}",
                f"TARGET={projectYAML[checkType][check]['target']}",
            ]
        ).returncode

    elif checkType == "check":
        if "command" not in projectYAML[checkType][check]:
            print(
                f"ERROR : Option 'command' not found in project YAML for {checkType} / {check} !"
            )
            returnCode = 1

        else:
            paramList = projectYAML[checkType][check]["command"].split()
            paramList.insert(1, f"--file-prefix={check}")
            # print(f'cmd : {paramList}')
            returnCode |= subprocess.run(paramList).returncode

    if returnCode == 2:
        print(f"ERROR : Running check '{check}' failed due to failed code checks !")
    elif returnCode != 0:
        print(f"ERROR : Problem running check '{check}' !")

    return returnCode


if __name__ == "__main__":

    returnCode = 0
    args = parseArgs()

    (projectYAML, userYAML) = readProjectYAML("config/project.yml", "config/user.yml")

    if args.runAllChecks:
        for checkType, checkTypeList in userYAML.items():
            if checkType not in projectYAML:
                print(f"ERROR : Check type '{checkType}' not found in project YAML !")
                returnCode = 1

            else:
                for check in checkTypeList:
                    if check not in projectYAML[checkType]:
                        print(
                            f"ERROR : Check '{check}' not found in project YAML for check type '{checkType}' !"
                        )
                        returnCode = 1
                    else:
                        returnCode |= runCheck(checkType, check)

    elif args.runCheck:
        check = args.runCheck
        type = None

        for checkType, checkTypeList in userYAML.items():
            if check in checkTypeList:
                type = checkType
                break

        returnCode |= runCheck(type, check)

    elif args.getAllChecks:
        allChecks = 'echo "checks=['

        for checkType, checkTypeList in userYAML.items():
            if checkType not in projectYAML:
                print(f"ERROR : Check type '{checkType}' not found in project YAML !")
                returnCode = 1

            for check in checkTypeList:
                if check not in projectYAML[checkType]:
                    print(
                        f"ERROR : Check '{check}' not found in project YAML for check type '{checkType}' !"
                    )
                    returnCode = 1
                else:
                    allChecks += f'\\"{check}\\",'

        allChecks += ']" >> "$GITHUB_OUTPUT"'
        allChecks = allChecks.replace(",]", "]")

        print(f"{allChecks}")

    else:
        print(f"\nERROR : Wrong parameters passed !\n")
        returnCode = 1

    exit(returnCode)
