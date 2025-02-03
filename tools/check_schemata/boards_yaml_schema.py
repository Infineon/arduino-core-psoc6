from schema import And, Or, Use, Optional

boardsYAMLSchema = {
    "boards": [
        {
            "sn": str,
            "description": str,
            "type": str,
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
            Optional("wiring"): Or(
                None,
                And(
                    {
                        Optional("iic_ping_pong_1_board"): bool,
                        Optional("iic_ping_pong_2_boards"): bool,
                        Optional("iic_ping_pong_2_boards_connection"): str,
                    }
                ),
            ),
        }
    ]
}


# [And(Or(int, float), error='\nERROR: primes must be int or float')]

# And(str, Use(str.lower), lambda s: s in ('squid', 'kid'), ),

# configSchema = {
#                 'rest': {
#                             'url' : And(str, lambda url: 'http' in url, error='\nERROR: 'rest -> url' must contain 'http''),
#                             'port': int
#                 },

#                 'primes': [And(Or(int, float), error='\nERROR: 'primes must be int or float')],
#             }

# print(boardsYAMLSchema)
