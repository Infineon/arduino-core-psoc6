from schema import And, Or, Use, Optional

boardsFeaturesYAMLSchema = {
    str: {
        Optional("description"): str,
        "architecture": str,
        "subarchitecture": str,
        "arduino_related": Or(
            None,
            And(
                {
                    Optional("fqbn"): str,
                    Optional("iic"): int,
                    Optional("spi"): int,
                    Optional("uart"): int,
                    Optional("analog_pins"): int,
                    Optional("gpio_pins"): int,
                    Optional("pwm_pins"): int,
                }
            ),
        ),
        "mtb_related": Or(
            None,
            And(
                {
                    Optional("hw_ext"): str,
                    Optional("iic"): int,
                    Optional("spi"): int,
                    Optional("uart"): int,
                    Optional("analog_pins"): int,
                    Optional("gpio_pins"): int,
                    Optional("pwm_pins"): int,
                }
            ),
        ),
        "features": Or(
            None,
            And(
                {
                    Optional("has_wifi"): bool,
                    Optional("wifi_channels"): int,
                    Optional("wifi_standards"): str,
                    Optional("has_bt"): bool,
                    Optional("bt_version"): Or(str, float),
                    Optional("has_can"): bool,
                    Optional("has_iic"): bool,
                    Optional("has_spi"): bool,
                    Optional("has_uart"): bool,
                    Optional("has_analog_pins"): bool,
                    Optional("has_gpio_pins"): bool,
                    Optional("has_pwm_pins"): bool,
                }
            ),
        ),
        Optional("sensors"): Or(
            None,
            And(
                {
                    str: int,
                }
            ),
        ),
    }
}
