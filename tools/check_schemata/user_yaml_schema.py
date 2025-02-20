from schema import And, Or, Use, Optional

userYAMLSchema = {
    Optional("build"): [And(str)],
    Optional("check"): [And(str)],
}
