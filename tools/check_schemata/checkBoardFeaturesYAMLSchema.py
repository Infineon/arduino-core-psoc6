# pip3.exe install pyyaml schema


# python.exe src/python/check_schemata/checkBoardFeaturesYAMLSchema.py


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
from check_schemata.board_features_yaml_schema import boardsFeaturesYAMLSchema


@staticmethod
def checkBoardFeaturesYAMLSchema(yml):
    schema = Schema(boardsFeaturesYAMLSchema)

    try:
        schema.validate(yml)
    except SchemaError as e:
        print(e)
        exit(1)


if __name__ == "__main__":

    with open("src/python/hil_yaml/board_features.yml", "r") as file:
        boardFeatures = yaml.safe_load(file)

    print(f"boardFeatures : {boardFeatures}\n")

    checkBoardFeaturesYAMLSchema(boardFeatures)
