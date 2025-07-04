/* LMP91000 Precision Analog Front-End Library
 * Simplified version for easy TIA and VREF configuration
 */

#ifndef LMP91000_H
#define LMP91000_H

#include "stm32wbxx_hal.h"

/* LMP91000 I2C Address (7-bit) */
#define LMP91000_I2C_ADDR        (0x48 << 1)  // 0x90 for write, 0x91 for read

/* Register Addresses */
#define LMP91000_REG_STATUS      0x00
#define LMP91000_REG_LOCK        0x01
#define LMP91000_REG_TIACN       0x10
#define LMP91000_REG_REFCN       0x11
#define LMP91000_REG_MODECN      0x12

/* TIA Gain Options (Transimpedance Amplifier) */
typedef enum {
    LMP91000_TIA_GAIN_EXT = 0,    // External resistor
    LMP91000_TIA_GAIN_2K75,       // 2.75kΩ
    LMP91000_TIA_GAIN_3K5,        // 3.5kΩ
    LMP91000_TIA_GAIN_7K,         // 7kΩ
    LMP91000_TIA_GAIN_14K,        // 14kΩ
    LMP91000_TIA_GAIN_35K,        // 35kΩ
    LMP91000_TIA_GAIN_120K,       // 120kΩ
    LMP91000_TIA_GAIN_350K        // 350kΩ
} LMP91000_TIA_Gain_t;

/* Load Resistor Options */
typedef enum {
    LMP91000_RLOAD_10 = 0,        // 10Ω
    LMP91000_RLOAD_33,            // 33Ω
    LMP91000_RLOAD_50,            // 50Ω
    LMP91000_RLOAD_100            // 100Ω
} LMP91000_RLoad_t;

/* Reference Source */
typedef enum {
    LMP91000_REF_INTERNAL = 0,    // Use internal 2.5V reference
    LMP91000_REF_EXTERNAL         // Use external VREF pin
} LMP91000_RefSource_t;

/* Internal Zero (bias voltage) */
typedef enum {
    LMP91000_ZERO_20PCT = 0,      // 20% of reference (0.5V @ 2.5V ref)
    LMP91000_ZERO_50PCT,          // 50% of reference (1.25V @ 2.5V ref)
    LMP91000_ZERO_67PCT,          // 67% of reference (1.675V @ 2.5V ref)
    LMP91000_ZERO_BYPASS          // Bypass (for ground-referred sensors)
} LMP91000_IntZero_t;

/* Bias Settings */
typedef enum {
    LMP91000_BIAS_NEGATIVE = 0,   // Negative bias polarity
    LMP91000_BIAS_POSITIVE        // Positive bias polarity
} LMP91000_BiasSign_t;

typedef enum {
    LMP91000_BIAS_0PCT = 0,       // 0% bias
    LMP91000_BIAS_1PCT,           // 1% bias
    LMP91000_BIAS_2PCT,           // 2% bias
    LMP91000_BIAS_4PCT,           // 4% bias
    LMP91000_BIAS_6PCT,           // 6% bias
    LMP91000_BIAS_8PCT,           // 8% bias
    LMP91000_BIAS_10PCT,          // 10% bias
    LMP91000_BIAS_12PCT,          // 12% bias
    LMP91000_BIAS_14PCT,          // 14% bias
    LMP91000_BIAS_16PCT,          // 16% bias
    LMP91000_BIAS_18PCT,          // 18% bias
    LMP91000_BIAS_20PCT,          // 20% bias
    LMP91000_BIAS_22PCT,          // 22% bias
    LMP91000_BIAS_24PCT           // 24% bias
} LMP91000_BiasLevel_t;

/* Operation Modes */
typedef enum {
    LMP91000_MODE_DEEP_SLEEP = 0,      // Deep sleep mode
    LMP91000_MODE_2LEAD_GROUND,        // 2-lead ground-referred galvanic
    LMP91000_MODE_STANDBY,             // Standby mode
    LMP91000_MODE_3LEAD_AMPEROMETRIC,  // 3-lead amperometric cell
    LMP91000_MODE_TEMP_TIA_OFF = 6,    // Temperature measurement (TIA OFF)
    LMP91000_MODE_TEMP_TIA_ON          // Temperature measurement (TIA ON)
} LMP91000_OpMode_t;

/* Error Codes */
typedef enum {
    LMP91000_OK = 0,
    LMP91000_ERROR,
    LMP91000_ERROR_I2C,
    LMP91000_ERROR_NOT_READY
} LMP91000_Status_t;

/* Handle Structure */
typedef struct {
    I2C_HandleTypeDef *hi2c;
    GPIO_TypeDef *menb_port;
    uint16_t menb_pin;
} LMP91000_Handle_t;

/* Function Prototypes */
LMP91000_Status_t LMP91000_Init(LMP91000_Handle_t *hlmp, I2C_HandleTypeDef *hi2c,
                                GPIO_TypeDef *menb_port, uint16_t menb_pin);
LMP91000_Status_t LMP91000_SetTIA(LMP91000_Handle_t *hlmp, LMP91000_TIA_Gain_t gain, LMP91000_RLoad_t rload);
LMP91000_Status_t LMP91000_SetReference(LMP91000_Handle_t *hlmp, LMP91000_RefSource_t ref_source,
                                        LMP91000_IntZero_t int_zero);
LMP91000_Status_t LMP91000_SetBias(LMP91000_Handle_t *hlmp, LMP91000_BiasSign_t sign,
                                   LMP91000_BiasLevel_t level);
LMP91000_Status_t LMP91000_SetMode(LMP91000_Handle_t *hlmp, LMP91000_OpMode_t mode);
LMP91000_Status_t LMP91000_Enable(LMP91000_Handle_t *hlmp);
LMP91000_Status_t LMP91000_Disable(LMP91000_Handle_t *hlmp);
LMP91000_Status_t LMP91000_ReadRegister(LMP91000_Handle_t *hlmp, uint8_t reg, uint8_t *data);
LMP91000_Status_t LMP91000_WriteRegister(LMP91000_Handle_t *hlmp, uint8_t reg, uint8_t data);

#endif /* LMP91000_H */
