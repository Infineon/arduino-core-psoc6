import xml.etree.ElementTree as ET
import sys
import os


def create_annotation(error):
    file = error.get("file0")
    id = error.get("id")
    message = error.get("msg")
    severity = error.get("severity")
    annotation_level = (
        "warning"
        if severity in ["style", "performance", "portability", "information"]
        else "error"
    )
    print(f"::{annotation_level} type:{severity} {id} : in file {file} - {message}")


def analyze_cppcheck_report(report_file):
    try:
        tree = ET.parse(report_file)
        root = tree.getroot()
    except ET.ParseError as e:
        print(f"Failed to parse XML report: {str(e)}")
        return False

    errors = root.findall("errors/error")
    error_count = len(errors)

    if error_count > 0:
        for error in errors:
            create_annotation(error)
        print(f"Cppcheck found {error_count} issues.")
        return False
    return True


if __name__ == "__main__":
    report_file = f"_results/cppcheck/cppcheck-errors.xml"

    if not os.path.isfile(report_file):
        print("No cppcheck report found.")
        returnCode = 1

    if not analyze_cppcheck_report(report_file):
        returnCode = 0  # Keep as 0 to not fail the job
    else:
        print("No issues found by Cppcheck.")
        returnCode = 0

    exit(returnCode)
