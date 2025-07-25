/*
 * Copyright 2025 Circuit Board Medics
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_io_expander.h"
#include "esp_io_expander_tca95x5.h"
#include "esp_log.h"

#define TIMEOUT_MS           (50)
#define TCA95X5_IO_COUNT     (16)
#define NUM_PORTS            (2)
#define TCA95X5_ADDRESS_BASE (0x20)
#define TCA95X5_SCL_HZ       (400 * 1000)
#define TCA95X5_DEFAULT_DIR  (UINT16_MAX)
#define TCA95X5_DEFAULT_OUT  (UINT16_MAX)

typedef struct esp_io_expander_tca95x5_t {
    esp_io_expander_t base;
    i2c_master_dev_handle_t dev;
    uint16_t output;
    uint16_t direction;
} esp_io_expander_tca95x5_t;

typedef enum tca95x5_register_t {
    INPUT_PORT_0    = 0x00,
    INPUT_PORT_1    = 0x01,
    OUTPUT_PORT_0   = 0x02,
    OUTPUT_PORT_1   = 0x03,
    POLARITY_PORT_0 = 0x04,
    POLARITY_PORT_1 = 0x05,
    CONFIG_PORT_0   = 0x06,
    CONFIG_PORT_1   = 0x07,
} __packed tca95x5_register_t;

static const char* TAG = "esp_io_expander_tca95x5";

static esp_err_t tca95x5_read_input_reg(esp_io_expander_handle_t handle, uint32_t* value)
{
    i2c_master_dev_handle_t dev = ((esp_io_expander_tca95x5_t*) handle)->dev;
    *value = 0;
    return i2c_master_transmit_receive(dev, INPUT_PORT_0, sizeof(INPUT_PORT_0), (uint8_t*) value, NUM_PORTS, TIMEOUT_MS);
}

static esp_err_t tca95x5_write_output_reg(esp_io_expander_handle_t handle, uint32_t value)
{
    esp_io_expander_tca95x5_t* tca95x5 = (esp_io_expander_tca95x5_t*) handle;
    if ((uint16_t) value == tca95x5->output) {
        return ESP_OK;
    }
    tca95x5->output = (uint16_t) value;
    uint8_t buffer[] = { OUTPUT_PORT_0, (uint8_t) value, (uint8_t) (value >> 8) };
    return i2c_master_transmit(tca95x5->dev, buffer, sizeof(buffer), TIMEOUT_MS);
}

static esp_err_t tca95x5_read_output_reg(esp_io_expander_handle_t handle, uint32_t *value)
{
    esp_io_expander_tca95x5_t* tca95x5 = (esp_io_expander_tca95x5_t*) handle;
    *value = tca95x5->output;
    return ESP_OK;
}

static esp_err_t tca95x5_write_direction_reg(esp_io_expander_handle_t handle, uint32_t value)
{
    esp_io_expander_tca95x5_t* tca95x5 = (esp_io_expander_tca95x5_t*) handle;
    if ((uint16_t) value == tca95x5->direction) {
        return ESP_OK;
    }
    tca95x5->direction = (uint16_t) value;
    uint8_t buffer[] = { CONFIG_PORT_0, (uint8_t) value, (uint8_t) (value >> 8) };
    return i2c_master_transmit(tca95x5->dev, buffer, sizeof(buffer), TIMEOUT_MS);
}

static esp_err_t tca95x5_read_direction_reg(esp_io_expander_handle_t handle, uint32_t *value)
{
    esp_io_expander_tca95x5_t* tca95x5 = (esp_io_expander_tca95x5_t*) handle;
    *value = tca95x5->direction;
    return ESP_OK;
}

static esp_err_t tca95x5_reset(esp_io_expander_handle_t handle)
{
    esp_io_expander_tca95x5_t* tca95x5 = (esp_io_expander_tca95x5_t*) handle;
    tca95x5->direction = UINT16_MAX;
    tca95x5->output = UINT16_MAX;

    // Set direction to input first for safety
    uint8_t buffer[] = { CONFIG_PORT_0, (uint8_t) tca95x5->direction, (uint8_t) (tca95x5->direction >> 8) };
    esp_err_t err = i2c_master_transmit(tca95x5->dev, buffer, sizeof(buffer), TIMEOUT_MS);
    if (err != ESP_OK) {
        return err;
    }
    buffer[1] = (uint8_t) tca95x5->output;
    buffer[2] = (uint8_t) (tca95x5->output >> 8);
    return i2c_master_transmit(tca95x5->dev, buffer, sizeof(buffer), TIMEOUT_MS);
}

static esp_err_t tca95x5_del(esp_io_expander_handle_t handle)
{
    i2c_master_dev_handle_t dev = ((esp_io_expander_tca95x5_t*) handle)->dev;
    esp_err_t err = i2c_master_bus_rm_device(dev);
    free(handle);
    return err;
}

esp_err_t esp_io_expander_create_tca95x5(i2c_master_bus_handle_t bus_handle,
                                         tca95x5_address_t address,
                                         esp_io_expander_handle_t* out_handle)
{
    if (!bus_handle || !out_handle) {
        ESP_LOGE(TAG, "Invalid arguments, cannot be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    esp_io_expander_tca95x5_t* tca95x5 = calloc(1, sizeof(*tca95x5));
    if (!tca95x5) {
        ESP_LOGE(TAG, "Out of memory");
        return ESP_ERR_NO_MEM;
    }

    tca95x5->base.read_input_reg      = tca95x5_read_input_reg;
    tca95x5->base.write_output_reg    = tca95x5_write_output_reg;
    tca95x5->base.read_output_reg     = tca95x5_read_output_reg;
    tca95x5->base.write_direction_reg = tca95x5_write_direction_reg;
    tca95x5->base.read_direction_reg  = tca95x5_read_direction_reg;
    tca95x5->base.reset               = tca95x5_reset;
    tca95x5->base.del                 = tca95x5_del;

    tca95x5->base.config.io_count                   = TCA95X5_IO_COUNT;
    tca95x5->base.config.flags.dir_out_bit_zero     = 1;
    tca95x5->base.config.flags.input_high_bit_zero  = 0;
    tca95x5->base.config.flags.output_high_bit_zero = 0;

    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = TCA95X5_ADDRESS_BASE + address,
        .scl_speed_hz = TCA95X5_SCL_HZ,
        .scl_wait_us = 0,
        .flags = {
            .disable_ack_check = 0,
        },
    };
    esp_err_t err = i2c_master_bus_add_device(bus_handle, &cfg, &tca95x5->dev);
    if (err != ESP_OK) {
        free(tca95x5);
        ESP_LOGE(TAG, "Error adding I2C device to bus");
        return err;
    }

    return tca95x5_reset((esp_io_expander_handle_t) &tca95x5->base);
}