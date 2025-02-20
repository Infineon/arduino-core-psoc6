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
from check_schemata.user_yaml_schema import userYAMLSchema


def checkUserYAMLSchema(yml):
    schema = Schema(userYAMLSchema)

    try:
        schema.validate(yml)
    except SchemaError as e:
        print(e)
        exit(1)


if __name__ == "__main__":

    with open(".//user.yml", "r") as file:
        userYAML = yaml.safe_load(file)

    print(f"userYAML : {userYAML}\n")

    checkUserYAMLSchema(userYAML)
