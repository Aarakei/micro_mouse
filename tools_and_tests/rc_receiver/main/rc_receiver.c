#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "esp_mac.h"
#include "esp_log.h"

static const char *TAG = "MOUSE_RECEIVER";

// ======================
// CONFIG
// ======================

#define SAMPLE_COUNT 100          // Number of entries to average
#define PAUSE_TIME_MS 5000       // Pause after averaging

// ======================
// DATA STRUCT
// ======================

typedef struct {
    int16_t x;
    int16_t y;
    bool pressed;
} __attribute__((packed)) joystick_t;

// ======================
// AVERAGING VARIABLES
// ======================

static int32_t x_total = 0;
static int32_t y_total = 0;
static int sample_counter = 0;

static bool averaging_enabled = true;

// ======================
// RECEIVE CALLBACK
// ======================

void on_data_recv(const esp_now_recv_info_t *esp_now_info,
                  const uint8_t *data,
                  int data_len)
{
    if (data_len == sizeof(joystick_t)) {

        joystick_t received_data;

        memcpy(&received_data, data, sizeof(joystick_t));

        // Print raw values
        printf("Joystick -> X: %d | Y: %d | Pressed: %s\n",
               received_data.x,
               received_data.y,
               received_data.pressed ? "True" : "False");

        // Only average if enabled
        if (averaging_enabled) {

            x_total += received_data.x;
            y_total += received_data.y;

            sample_counter++;

            // Enough samples collected?
            if (sample_counter >= SAMPLE_COUNT) {

                float x_avg = (float)x_total / sample_counter;
                float y_avg = (float)y_total / sample_counter;

                printf("\n========== AVERAGE ==========\n");
                printf("Average X: %.2f\n", x_avg);
                printf("Average Y: %.2f\n", y_avg);
                printf("=============================\n\n");

                // Disable averaging temporarily
                averaging_enabled = false;

                // Reset totals for next averaging cycle
                x_total = 0;
                y_total = 0;
                sample_counter = 0;

                // Pause averaging
                vTaskDelay(PAUSE_TIME_MS / portTICK_PERIOD_MS);

                // Re-enable averaging
                averaging_enabled = true;
            }
        }

    } else {
        ESP_LOGW(TAG, "Received unexpected data length: %d bytes", data_len);
    }
}

// ======================
// WIFI + ESP-NOW INIT
// ======================

void init_wifi_esp_now(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (esp_now_init() != ESP_OK) {
        ESP_LOGE(TAG, "Error initializing ESP-NOW");
        return;
    }

    ESP_LOGI(TAG, "ESP-NOW Initialized Successfully");
}

// ======================
// MAIN
// ======================

void app_main(void)
{
    init_wifi_esp_now();

    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_data_recv));

    ESP_LOGI(TAG, "Mouse is listening for controller commands...");

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }











    #include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

// TODO: Figure out which pins are going to be used
#define MOTOR_INPUT_1 15
#define MOTOR_INPUT_2 16
#define MOTOR_PWM 17

void motor_init() {
    // configure the io pins for the motors
    gpio_config_t io_configuration = {
        .pin_bit_mask = (1ULL << MOTOR_INPUT_1) | (1ULL << MOTOR_INPUT_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE
    };
    gpio_config(&io_configuration);

    // configure the pwm settings
    ledc_timer_config_t ledc_timer_configuration = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 20000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer_configuration);

    ledc_channel_config_t ledc_channel_configuration = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = MOTOR_PWM,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel_configuration);
}

void set_motor_speed(uint32_t duty, bool forward) {
    // Set direction
    gpio_set_level(MOTOR_INPUT_1, forward ? 1 : 0);
    gpio_set_level(MOTOR_INPUT_2, forward ? 0 : 1);

    // Set speed (duty cycle)
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void motors_task(void *pv_parameters) {
    while(1) {
        for(int i = 0; i < 80; i++) {
            set_motor_speed(i, true);
            printf("i: %d\n", i);
            vTaskDelay(100);
        }

        for(int i = 80; i >= 0; i++) {
            set_motor_speed(i, false);
            printf("i: %d\n", i);
            vTaskDelay(100);
        }
    }
}
}