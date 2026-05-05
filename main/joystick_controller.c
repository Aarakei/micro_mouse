#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "esp_log.h"

// make sure this is the mac address of the actual device you want to be sending joystick data to
const uint8_t MOUSE_MAC[6] = {0xA4,0xF0,0x0F,0x75,0xAF,0x30}; 

void add_peer(uint8_t mac_address[6]) {
    // 2. Create the "Contact Card" (The Struct)
    esp_now_peer_info_t peer_info;

    // 3. Zero out the memory of the struct to ensure no "garbage" data is inside
    // Syntax: memset(pointer, value, size)
    memset(&peer_info, 0, sizeof(peer_info));

    // 4. Fill in the "Contact Card" details
    // Syntax: target_array, source_array, number_of_bytes
    memcpy(peer_info.peer_addr, mac_address, 6); // Set the MAC address
    peer_info.channel = 0;                          // 0 means "use current Wi-Fi channel"
    peer_info.encrypt = false;                      // No encryption (faster for MicroMouse)
    
    // 5. Register the peer with the system
    if (esp_now_add_peer(&peer_info) != ESP_OK) {
        printf("Failed to add peer\n");
        return;
    }
    printf("Mouse added as peer successfully!\n");
}

typedef struct {
    int16_t x;
    int16_t y;
    bool pressed;
} __attribute__((packed)) joystick_t;

joystick_t read_joystick() {
    joystick_t data;
    data.x = 256;
    data.y = 1024;
    data.pressed = false;
    return data;
}

void send_joystick_data(void) {
    // 1. Pack the data
    joystick_t data = read_joystick();

    // 2. Send the data
    // template : esp_now_send(peer_mac, data_buffer, data_length);
    esp_err_t ret = esp_now_send(MOUSE_MAC, (uint8_t *)&data, sizeof(data));

    // 3. Check how the send went
    if (ret == ESP_OK) {
        printf("data successfully sent\n");
    } else {
        printf("Error in sending data\n");
        printf("Error code: %d\n", ret);
    }
}

static const char *TAG = "MOUSE_CONTROLLER";

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
    init_wifi_esp_now();
    add_peer(MOUSE_MAC);
    send_joystick_data();
}