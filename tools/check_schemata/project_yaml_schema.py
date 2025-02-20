from schema import And, Or, Use, Optional

projectYAMLSchema = {
    "build": {
        str: {
            "description": str,
            "target": str,
            "fqbn": str,
        }
    },
    "check": {
        str: {
            "description": str,
            "tool": str,
            "command": str,
        }
    },
}
