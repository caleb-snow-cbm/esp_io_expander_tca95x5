#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_log.h"

#include "esp_io_expander_tca95x5.h"

static const char* TAG = "example";

#define EXAMPLE_I2C_SDA     (GPIO_NUM_23)
#define EXAMPLE_I2C_SCL     (GPIO_NUM_4)
#define EXAMPLE_I2C_PORT    (I2C_NUM_0)
#define EXAMPLE_ADDRESS     (TCA95X5_ADDRESS_000)
#define EXAMPLE_OUTPUT_PIN  (IO_EXPANDER_PIN_NUM_0)

void app_main(void)
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    i2c_master_bus_config_t buscfg = {
        .i2c_port = EXAMPLE_I2C_PORT,
        .sda_io_num = EXAMPLE_I2C_SDA,
        .scl_io_num = EXAMPLE_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .trans_queue_depth = 8
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&buscfg, &bus_handle));

    ESP_LOGI(TAG, "Adding IO expander");
    esp_io_expander_handle_t io_expander;
    ESP_ERROR_CHECK(esp_io_expander_create_tca9535(bus_handle, EXAMPLE_ADDRESS, &io_expander));

    // Toggle pin on and off
    ESP_ERROR_CHECK(esp_io_expander_set_dir(io_expander, EXAMPLE_OUTPUT_PIN, IO_EXPANDER_OUTPUT));
    bool level = 0;
    for (;;) {
        ESP_ERROR_CHECK(esp_io_expander_set_level(io_expander, EXAMPLE_OUTPUT_PIN, level));
        level = !level;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
