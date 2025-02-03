# pip3.exe install pyyaml schema


# python.exe src/python/check_schemata/checkBoardsYAMLSchema.py


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
from check_schemata.boards_yaml_schema import boardsYAMLSchema

# from hil_yaml.readHILYAML import readHILYAML, mergeHILYAML


@staticmethod
def checkBoardsYAMLSchema(yml):
    schema = Schema(boardsYAMLSchema)

    try:
        schema.validate(yml)
    except SchemaError as e:
        print(e)
        exit(1)


if __name__ == "__main__":

    with open("src/python/hil_yaml/board_features.yml", "r") as file:
        boardFeatures = yaml.safe_load(file)

    print(f"boardFeatures : {boardFeatures}\n")

    with open("src/python/hil_yaml/boards.yml", "r") as file:
        boards = yaml.safe_load(file)

    print(f"boards : {boards}\n")

    boards = mergeHILYAML(boards, boardFeatures)

    print(f"boards : {boards}\n")

    checkBoardsYAMLSchema(boards)
