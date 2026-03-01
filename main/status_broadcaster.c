#include <status_broadcaster.h>
#include <mqttcomm.h>
#include <esp_log.h>
#include <stdio.h>

#define MQTT_TOPIC "fiesta/buttons"

static const char* BC_TAG = "broadcaster";

static const char* button_name_str(ButtonName name) {
    switch (name) {
        case BUTTON_NAME_PIT:   return "PIT";
        case BUTTON_NAME_YES:   return "YES";
        case BUTTON_NAME_FCK:   return "FCK";
        case BUTTON_NAME_STINT: return "STINT";
        case BUTTON_NAME_NO:    return "NO";
        default:                return "UNKNOWN";
    }
}

static const char* button_state_str(ButtonState state) {
    switch (state) {
        case BUTTON_STATE_PRESSED:   return "PRESSED";
        case BUTTON_STATE_DEPRESSED: return "DEPRESSED";
        default:                     return "UNKNOWN";
    }
}

void broadcaster_send_button(ButtonName buttonName, ButtonState state) {
    char json[64];
    int len = snprintf(json, sizeof(json),
        "{\"button\":\"%s\",\"state\":\"%s\"}",
        button_name_str(buttonName),
        button_state_str(state));

    mqttcomm_publish(MQTT_TOPIC, json, len);
    ESP_LOGI(BC_TAG, "Sent: %s", json);
}