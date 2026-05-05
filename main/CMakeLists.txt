idf_component_register(SRCS "get_mac_address.c" "blink_example_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi esp_driver_gpio nvs_flash)
