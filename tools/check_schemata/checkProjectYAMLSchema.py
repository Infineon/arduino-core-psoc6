# pip3.exe install pyyaml schema

import os
import sys
import yaml

from schema import Schema, SchemaError


importPath = os.path.normpath(
    os.path.dirname(os.path.realpath(os.path.abspath(__file__))) + "/.."
)

if not importPath in sys.path:
    sys.path.insert(1, importPath)


# project file
from check_schemata.project_yaml_schema import projectYAMLSchema


def checkProjectYAMLSchema(yml):
    schema = Schema(projectYAMLSchema)

    try:
        schema.validate(yml)
    except SchemaError as e:
        print(e)
        exit(1)


if __name__ == "__main__":

    with open("./project.yml", "r") as file:
        projectYAML = yaml.safe_load(file)

    print(f"projectYAML : {projectYAML}\n")
    checkProjectYAMLSchema(projectYAML)
