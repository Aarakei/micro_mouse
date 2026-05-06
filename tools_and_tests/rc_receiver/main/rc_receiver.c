#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "esp_log.h"

static const char *TAG = "MOUSE_RECEIVER";

// 1. The exact same struct from the controller (Crucial for memory alignment)
typedef struct {
    int16_t x;
    int16_t y;
    bool pressed;
} __attribute__((packed)) joystick_t;

// 2. The Receive Callback Function
// Note: This specific signature (using esp_now_recv_info_t) is required for ESP-IDF v6.0.1
void on_data_recv(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
    
    // Verify the received packet size matches our expected struct size
    if (data_len == sizeof(joystick_t)) {
        joystick_t received_data;
        
        // Unpack the raw bytes back into our readable format
        memcpy(&received_data, data, sizeof(joystick_t));

        // Print the parsed commands
        printf("Joystick -> X: %d | Y: %d | Pressed: %s\n", 
               received_data.x, 
               received_data.y, 
               received_data.pressed ? "True" : "False");
               
    } else {
        ESP_LOGW(TAG, "Received unexpected data length: %d bytes", data_len);
    }
}

void init_wifi_esp_now(void) {
    // 1. Initialize NVS (Required for Wi-Fi to store its config)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Initialize the underlying network stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Initialize Wi-Fi in Station Mode
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    // 4. Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing ESP-NOW");
        return;
    }
    ESP_LOGI(TAG, "ESP-NOW Initialized Successfully");
}

void app_main(void) {
    // 1. Boot up the radio
    init_wifi_esp_now();

    // 2. Register the callback function so the ESP knows what to do when data arrives
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));

    ESP_LOGI(TAG, "Mouse is listening for controller commands...");

    // 3. Keep the main task alive. If app_main ends, the program terminates.
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}