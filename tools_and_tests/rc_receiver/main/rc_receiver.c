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
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "math.h"



static const char *TAG = "MOUSE_RECEIVER";

// ======================
// CONFIG
// ======================

#define SAMPLE_COUNT 100          // Number of entries to average
#define PAUSE_TIME_MS 5000       // Pause after averaging
#define JOY_X_CENTER 1837
#define JOY_X_MIN 597
#define JOY_X_MAX 4072
#define JOY_Y_CENTER 1945
#define JOY_Y_MIN 147
#define JOY_Y_MAX 4072 // This measurement was botched, make sure to check it
#define JOY_MAX      4095
#define DEADZONE     150

// ======================
// DATA STRUCT
// ======================

typedef struct {
    int16_t x;
    int16_t y;
    bool pressed;
} __attribute__((packed)) joystick_t;

// Define Pins
#define LEFT_IO_1   4
#define LEFT_IO_2   5

#define RIGHT_IO_1  40
#define RIGHT_IO_2  41

#define MAX_DUTY 255

// Struct configuration explicitly for DRV8833 style IN/IN driving
typedef struct {
    int pin_in1;
    int pin_in2;
    ledc_channel_t channel_in1;
    ledc_channel_t channel_in2;
} drv8833_motor_t;

// Standard Initialization: Binds both physical pins to separate hardware timers
void drv8833_motor_init(drv8833_motor_t *m) {
    // 1. Configure the shared LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_8_BIT, // 0 to 255
        .freq_hz = 20000,                    // 20kHz is standard for silent operation
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // 2. Configure Channel 1 for IN1 pin
    ledc_channel_config_t chan_1_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = m->channel_in1,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = m->pin_in1,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&chan_1_config);

    // 3. Configure Channel 2 for IN2 pin
    ledc_channel_config_t chan_2_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = m->channel_in2,
        .timer_sel = LEDC_TIMER_0,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = m->pin_in2,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&chan_2_config);
}

// Control function matching the DRV8833 datasheet logic rules
void drv8833_set_speed(drv8833_motor_t *m, uint32_t duty, bool forward) {
    if (forward) {
        // IN1 gets the PWM waveform, IN2 is pulled entirely to ground (0)
        ledc_set_duty(LEDC_LOW_SPEED_MODE, m->channel_in1, duty);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, m->channel_in2, 0);
    } else {
        // IN1 is pulled entirely to ground (0), IN2 gets the PWM waveform
        ledc_set_duty(LEDC_LOW_SPEED_MODE, m->channel_in1, 0);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, m->channel_in2, duty);
    }
    
    // Apply changes immediately to hardware registers
    ledc_update_duty(LEDC_LOW_SPEED_MODE, m->channel_in1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, m->channel_in2);
}

// Left Motor config (Uses LEDC channels 0 and 1)
drv8833_motor_t left_motor = {
    .pin_in1 = LEFT_IO_1,
    .pin_in2 = LEFT_IO_2,
    .channel_in1 = LEDC_CHANNEL_0,
    .channel_in2 = LEDC_CHANNEL_1
};

// Right Motor config (Uses LEDC channels 2 and 3)
drv8833_motor_t right_motor = {
    .pin_in1 = RIGHT_IO_1,
    .pin_in2 = RIGHT_IO_2,
    .channel_in1 = LEDC_CHANNEL_2,
    .channel_in2 = LEDC_CHANNEL_3
};

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

        // Convert Joystick values to motor values
        float joy_x;
        float joy_y;

        if(received_data.x > JOY_X_MAX){
            joy_x = 1;
        } else if(received_data.x < JOY_X_MIN){
            joy_x = -1;
        } else if(abs(received_data.x - JOY_X_CENTER) < DEADZONE){
            joy_x = 0;
        } else if (received_data.x > JOY_X_CENTER){
            joy_x = (float)(received_data.x - JOY_X_CENTER) / (JOY_X_MAX - JOY_X_CENTER);
        } else {
            joy_x = (float)(received_data.x - JOY_X_CENTER) / (JOY_X_CENTER - JOY_X_MIN);
        }

        if(received_data.y > JOY_Y_MAX){
            joy_y = 1;
        } else if(received_data.y < JOY_Y_MIN){
            joy_y = -1;
        } else if(abs(received_data.y - JOY_Y_CENTER) < DEADZONE){
            joy_y = 0;
        } else if (received_data.y > JOY_Y_CENTER){
            joy_y = (float)(received_data.y - JOY_Y_CENTER) / (JOY_Y_MAX - JOY_Y_CENTER);
        } else {
            joy_y = (float)(received_data.y - JOY_Y_CENTER) / (JOY_Y_CENTER - JOY_Y_MIN);
        }

        // 1. Mix arcade steering: Combined linear and turning forces
        float left_power  = joy_y + joy_x;
        float right_power = joy_y - joy_x;

        // 2. Clamp powers strictly between -1.0 and 1.0 to prevent mathematical overflow
        if (left_power > 1.0f)  left_power = 1.0f;
        if (left_power < -1.0f) left_power = -1.0f;
        if (right_power > 1.0f) right_power = 1.0f;
        if (right_power < -1.0f) right_power = -1.0f;

        // 3. Convert float power to absolute hardware duty cycle integers (0 to 255)
        // fabsf() ensures duty cycle is always a positive value
        uint32_t left_duty  = (uint32_t)(fabsf(left_power) * MAX_DUTY);
        uint32_t right_duty = (uint32_t)(fabsf(right_power) * MAX_DUTY);

        // 4. Determine direction flags based on the positive or negative signs
        bool left_forward  = (left_power >= 0.0f);
        bool right_forward = (right_power >= 0.0f);

        // 5. Send these computed values straight to your DRV8833 motor structs
        drv8833_set_speed(&left_motor, left_duty, left_forward);
        drv8833_set_speed(&right_motor, right_duty, right_forward);


        // // Only average if enabled
        // if (averaging_enabled) {

        //     x_total += received_data.x;
        //     y_total += received_data.y;

        //     sample_counter++;

        //     // Enough samples collected?
        //     if (sample_counter >= SAMPLE_COUNT) {

        //         float x_avg = (float)x_total / sample_counter;
        //         float y_avg = (float)y_total / sample_counter;

        //         printf("\n========== AVERAGE ==========\n");
        //         printf("Average X: %.2f\n", x_avg);
        //         printf("Average Y: %.2f\n", y_avg);
        //         printf("=============================\n\n");

        //         // Disable averaging temporarily
        //         averaging_enabled = false;

        //         // Reset totals for next averaging cycle
        //         x_total = 0;
        //         y_total = 0;
        //         sample_counter = 0;

        //         // Pause averaging
        //         vTaskDelay(PAUSE_TIME_MS / portTICK_PERIOD_MS);

        //         // Re-enable averaging
        //         averaging_enabled = true;
        //     }
        // }

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

    

    // Initialize both independent instances
    drv8833_motor_init(&left_motor);
    drv8833_motor_init(&right_motor);

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}