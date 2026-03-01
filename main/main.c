#include <stdio.h>
#include "esp_err.h"
#include "wlan.h"
#include "button_handler.h"
#include "mqttcomm.h"
#include "led_status.h"

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    led_status_init();
    esp_netif_t* netif = wlan_start();
    mqttcomm_start(netif);
    button_handler_start();
}
