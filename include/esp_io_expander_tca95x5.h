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

#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_io_expander.h"

#ifdef __cplusplus
extern "C" {
#endif

// State of `A2`, `A1`, `A0` pins
// e.g. `A2` = `A1` = GND, `A0` = VCC -> `TCA95X5_ADDRESS_001`
typedef enum tca95x5_address_t {
    TCA95X5_ADDRESS_000,
    TCA95X5_ADDRESS_001,
    TCA95X5_ADDRESS_010,
    TCA95X5_ADDRESS_011,
    TCA95X5_ADDRESS_100,
    TCA95X5_ADDRESS_101,
    TCA95X5_ADDRESS_110,
    TCA95X5_ADDRESS_111,
} tca95x5_address_t;


/**
 * @brief Alias for `esp_io_expander_create_tca95x5`
 * 
 * @param bus_handle I2C master bus handle, must already be initialized
 * @param address    State of address selection pins
 * @param out_handle [output] Handle to use in `esp_io_expander_*` function calls
 * @return esp_err_t `ESP_OK` if successful, error code if not
 */
#define esp_io_expander_create_tca9535(...) esp_io_expander_create_tca95x5(__VA_ARGS__)

/**
 * @brief Alias for `esp_io_expander_create_tca95x5`
 * 
 * @param bus_handle I2C master bus handle, must already be initialized
 * @param address    State of address selection pins
 * @param out_handle [output] Handle to use in `esp_io_expander_*` function calls
 * @return esp_err_t `ESP_OK` if successful, error code if not
 */
#define esp_io_expander_create_tca9555(...) esp_io_expander_create_tca95x5(__VA_ARGS__)

/**
 * @brief Initializes I2C device and `esp_io_expander_t` struct
 * 
 * @param bus_handle I2C master bus handle, must already be initialized
 * @param address    State of address selection pins
 * @param out_handle [output] Handle to use in `esp_io_expander_*` function calls
 * @return esp_err_t `ESP_OK` if successful, error code if not
 */
esp_err_t esp_io_expander_create_tca95x5(i2c_master_bus_handle_t bus_handle,
                                         tca95x5_address_t address,
                                         esp_io_expander_handle_t* out_handle);


#ifdef __cplusplus
}
#endif
