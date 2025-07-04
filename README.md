# STM32 LMP91000 Library

A simple STM32 HAL library for the Texas Instruments LMP91000 precision analog front-end IC, designed for electrochemical sensor applications.

## Hardware Requirements

- STM32 microcontroller with HAL library
- LMP91000 IC or evaluation board
- I2C connection (SCL, SDA with pull-up resistors)
- GPIO pin for MENB (Module Enable)
- ADC input for reading VOUT

## Quick Start

### 1. Hardware Connections

| LMP91000 Pin | STM32 Connection | Notes |
|--------------|------------------|-------|
| VDD | 3.3V | Power supply |
| AGND/DGND | GND | Ground |
| SCL | I2C SCL | With 10kΩ pull-up |
| SDA | I2C SDA | With 10kΩ pull-up |
| MENB | GPIO Pin | Module enable (active low) |
| VOUT | ADC Input | Sensor output |
| VREF | 2.5V | Internal or external reference |

### 2. Basic Usage

```c
#include "lmp91000.h"

// Initialize LMP91000
LMP91000_Handle_t hlmp;
LMP91000_Init(&hlmp, &hi2c1, GPIOB, GPIO_PIN_0);

// Configure for electrochemical sensor
LMP91000_SetTIA(&hlmp, LMP91000_TIA_GAIN_120K, LMP91000_RLOAD_100);
LMP91000_SetReference(&hlmp, LMP91000_REF_INTERNAL, LMP91000_ZERO_50PCT);
LMP91000_SetBias(&hlmp, LMP91000_BIAS_POSITIVE, LMP91000_BIAS_0PCT);
LMP91000_SetMode(&hlmp, LMP91000_MODE_3LEAD_AMPEROMETRIC);

// Read sensor via ADC
HAL_ADC_Start(&hadc1);
HAL_ADC_PollForConversion(&hadc1, 5);
uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
float voltage = adc_value * 3.3f / 4095.0f;
float current_nA = (voltage - 1.25f) / 120000.0f * 1e9f;
```

## API Reference

### Initialization
```c
LMP91000_Status_t LMP91000_Init(LMP91000_Handle_t *hlmp, I2C_HandleTypeDef *hi2c, 
                                GPIO_TypeDef *menb_port, uint16_t menb_pin);
```

### TIA Configuration
```c
LMP91000_Status_t LMP91000_SetTIA(LMP91000_Handle_t *hlmp, 
                                  LMP91000_TIA_Gain_t gain, 
                                  LMP91000_RLoad_t rload);
```

**Available TIA Gains:**
- `LMP91000_TIA_GAIN_EXT` - External resistor
- `LMP91000_TIA_GAIN_2K75` - 2.75kΩ
- `LMP91000_TIA_GAIN_3K5` - 3.5kΩ
- `LMP91000_TIA_GAIN_7K` - 7kΩ
- `LMP91000_TIA_GAIN_14K` - 14kΩ
- `LMP91000_TIA_GAIN_35K` - 35kΩ
- `LMP91000_TIA_GAIN_120K` - 120kΩ
- `LMP91000_TIA_GAIN_350K` - 350kΩ

### Reference Configuration
```c
LMP91000_Status_t LMP91000_SetReference(LMP91000_Handle_t *hlmp, 
                                        LMP91000_RefSource_t ref_source, 
                                        LMP91000_IntZero_t int_zero);
```

### Operation Modes
```c
LMP91000_Status_t LMP91000_SetMode(LMP91000_Handle_t *hlmp, LMP91000_OpMode_t mode);
```

**Available Modes:**
- `LMP91000_MODE_3LEAD_AMPEROMETRIC` - Standard 3-electrode sensor
- `LMP91000_MODE_2LEAD_GROUND` - 2-electrode ground-referenced
- `LMP91000_MODE_STANDBY` - Low power standby
- `LMP91000_MODE_DEEP_SLEEP` - Minimum power consumption

## Current Calculation

For a configured TIA gain, convert ADC reading to sensor current:

```c
float voltage = adc_raw * 3.3f / 4095.0f;  // Convert ADC to voltage
float current_nA = (voltage - zero_voltage) / tia_resistance * 1e9f;  // Convert to nanoamps
```

Where:
- `zero_voltage` = Reference voltage × Internal zero percentage
- `tia_resistance` = Selected TIA gain resistor value

## Examples

See the `examples/` directory for complete implementation examples including:
- Basic sensor reading
- BLE data transmission
- Real-time plotting integration

## Tested Platforms

- STM32WB series


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## References

- [LMP91000 Datasheet](https://www.ti.com/lit/ds/symlink/lmp91000.pdf)
- [LMP91000 EVM User Guide](https://www.ti.com/lit/ug/snau121a/snau121a.pdf)

