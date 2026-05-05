idf_component_register(SRCS "get_mac_address.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi esp_driver_gpio nvs_flash)