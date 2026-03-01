#include <esp_err.h>
#include <iot_button.h>
#include <button_gpio.h>
#include <esp_log.h>
#include <status_broadcaster.h>
#include "funkbox_types.h"
#include <driver/gpio.h>

static const char* TAG = "PIT_RADIO";

void event_cb(void* button_handle, void* usr_data) {
    ButtonName name = (ButtonName)(intptr_t) usr_data;
    ESP_LOGI(TAG, "Button Pressed: %d", name);
    broadcaster_send_button(name, BUTTON_STATE_PRESSED);
}

void register_button(gpio_num_t num, ButtonName name) {
    button_config_t btn_config = {0};
    button_gpio_config_t gpio_cfg = {
        .gpio_num = num,
        .active_level = 1,
    };

    button_handle_t btn_handle;
    ESP_ERROR_CHECK(iot_button_new_gpio_device(&btn_config, &gpio_cfg, &btn_handle));
    ESP_ERROR_CHECK(iot_button_register_cb(btn_handle, BUTTON_PRESS_DOWN, NULL, event_cb, (void *)(intptr_t)name));
}

void button_handler_start() {
    register_button(GPIO_NUM_18, BUTTON_NAME_ALARM);
    register_button(GPIO_NUM_19, BUTTON_NAME_STINT);
    register_button(GPIO_NUM_20, BUTTON_NAME_FCK);
    register_button(GPIO_NUM_21, BUTTON_NAME_FUEL);
    register_button(GPIO_NUM_14, BUTTON_NAME_PIT);
}