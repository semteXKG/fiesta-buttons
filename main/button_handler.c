#include <esp_err.h>
#include <iot_button.h>
#include <esp_log.h>
#include <status_broadcaster.h>
#include <button_types.h>


static const char* TAG = "PIT_RADIO";

void event_cb(void* args, void* data) {
    ButtonName name = (ButtonName) data;
    ESP_LOGI(TAG, "Button Pressed: %d", name);
    broadcaster_send_button(name, BUTTON_STATE_PRESSED);
}

void register_button(gpio_num_t num, ButtonName name) {
    button_config_t btn_config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = num,
            .active_level = 1,
        },
    };

    button_handle_t btn_handler = iot_button_create(&btn_config);
    iot_button_register_cb(btn_handler, BUTTON_PRESS_DOWN, event_cb, (void *)name);
} 

void button_handler_start() {     
    register_button(GPIO_NUM_18, BUTTON_NAME_ALARM);
    register_button(GPIO_NUM_19, BUTTON_NAME_STINT);
    register_button(GPIO_NUM_20, BUTTON_NAME_FCK);
    register_button(GPIO_NUM_21, BUTTON_NAME_FUEL);
    register_button(GPIO_NUM_14, BUTTON_NAME_PIT);
}