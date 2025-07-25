# ESP IO Expander Chip TCA95x5

Implementation of the TCA9535/TCA9555 io expander chips with `esp_io_expander` component.

| Chip             | Communication interface | Component name | Link to datasheet |
| :--------------: | :---------------------: | :------------: | :---------------: |
| TCA9535       | I2C                     | esp_io_expander_tca95x5 | [datasheet](https://www.ti.com/lit/gpn/tca9535) |
| TCA9555       | I2C                     | esp_io_expander_tca95x5 | [datasheet](https://www.ti.com/lit/gpn/tca9555) |

## Add to project

Packages from this repository are uploaded to [Espressif's component service](https://components.espressif.com/).
You can add them to your project via `idf.py add-dependency`, e.g.
```
    idf.py add-dependency caleb-snow-cbm/esp_io_expander_tca95x5==1.0.0
```

Alternatively, you can clone the repository and copy the contents into your project's `components` directory.

## Example use

Creation of the i2c bus.

```c
    i2c_master_bus_handle_t i2c_handle = NULL;
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = 47,
        .scl_io_num = 48,
        .clk_source = I2C_CLK_SRC_DEFAULT,
    };
    i2c_new_master_bus(&bus_config, &i2c_handle);
```

Creation of the component.

```c
    esp_io_expander_handle_t io_expander = NULL;
    esp_io_expander_new_i2c_tca9535(i2c_handle, TCA95X5_ADDRESS_001, &io_expander);
```

Print all pins's status to the log:

```c
    esp_io_expander_print_state(io_expander);
```

Set pin 0 and pin 1 with output dircetion and low level:

```c
    esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_1, IO_EXPANDER_OUTPUT);
    esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_1, 0);
```

Set pin 2 and pin 3 with input dircetion:

```c
    uint32_t pin_levels = 0;
    esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_2 | IO_EXPANDER_PIN_NUM_3, IO_EXPANDER_INPUT);
    esp_io_expander_get_level(io_expander, IO_EXPANDER_PIN_NUM_2 | IO_EXPANDER_PIN_NUM_3, &pin_levels);
```
