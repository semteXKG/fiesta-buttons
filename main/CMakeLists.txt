idf_component_register(SRCS "main.c" "wlan.c" "mqttcomm.c" "led_status.c" "status_broadcaster.c" "button_handler.c"
                    INCLUDE_DIRS ".")
spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
nvs_create_partition_image(nvs ../nvs.csv FLASH_IN_PROJECT)