idf_component_register(SRCS "rc_controller.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi esp_driver_gpio nvs_flash esp_adc)
