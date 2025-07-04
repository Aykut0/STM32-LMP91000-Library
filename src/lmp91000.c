/* LMP91000 Library Implementation*/

#include "lmp91000.h"

#define LMP91000_I2C_TIMEOUT 1000

/**
 * @brief Initialize LMP91000
 */
LMP91000_Status_t LMP91000_Init(LMP91000_Handle_t *hlmp, I2C_HandleTypeDef *hi2c,
                                GPIO_TypeDef *menb_port, uint16_t menb_pin)
{
    if (!hlmp || !hi2c) return LMP91000_ERROR;

    hlmp->hi2c = hi2c;
    hlmp->menb_port = menb_port;
    hlmp->menb_pin = menb_pin;

    // Configure MENB pin as output
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = menb_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(menb_port, &GPIO_InitStruct);

    // Start disabled, then enable
    HAL_GPIO_WritePin(menb_port, menb_pin, GPIO_PIN_SET);  // Disable
    HAL_Delay(10);
    HAL_GPIO_WritePin(menb_port, menb_pin, GPIO_PIN_RESET); // Enable
    HAL_Delay(10);

    // Check if device is ready
    uint8_t status;
    if (LMP91000_ReadRegister(hlmp, LMP91000_REG_STATUS, &status) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    return (status & 0x01) ? LMP91000_OK : LMP91000_ERROR_NOT_READY;
}

/**
 * @brief Enable LMP91000 (MENB = LOW)
 */
LMP91000_Status_t LMP91000_Enable(LMP91000_Handle_t *hlmp)
{
    if (!hlmp) return LMP91000_ERROR;
    HAL_GPIO_WritePin(hlmp->menb_port, hlmp->menb_pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    return LMP91000_OK;
}

/**
 * @brief Disable LMP91000 (MENB = HIGH)
 */
LMP91000_Status_t LMP91000_Disable(LMP91000_Handle_t *hlmp)
{
    if (!hlmp) return LMP91000_ERROR;
    HAL_GPIO_WritePin(hlmp->menb_port, hlmp->menb_pin, GPIO_PIN_SET);
    return LMP91000_OK;
}

/**
 * @brief Read register from LMP91000
 */
LMP91000_Status_t LMP91000_ReadRegister(LMP91000_Handle_t *hlmp, uint8_t reg, uint8_t *data)
{
    if (!hlmp || !data) return LMP91000_ERROR;

    // Send register address
    if (HAL_I2C_Master_Transmit(hlmp->hi2c, LMP91000_I2C_ADDR, &reg, 1, LMP91000_I2C_TIMEOUT) != HAL_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Read data
    if (HAL_I2C_Master_Receive(hlmp->hi2c, LMP91000_I2C_ADDR, data, 1, LMP91000_I2C_TIMEOUT) != HAL_OK) {
        return LMP91000_ERROR_I2C;
    }

    return LMP91000_OK;
}

/**
 * @brief Write register to LMP91000
 */
LMP91000_Status_t LMP91000_WriteRegister(LMP91000_Handle_t *hlmp, uint8_t reg, uint8_t data)
{
    if (!hlmp) return LMP91000_ERROR;

    uint8_t buffer[2] = {reg, data};

    if (HAL_I2C_Master_Transmit(hlmp->hi2c, LMP91000_I2C_ADDR, buffer, 2, LMP91000_I2C_TIMEOUT) != HAL_OK) {
        return LMP91000_ERROR_I2C;
    }

    return LMP91000_OK;
}

/**
 * @brief Set TIA (Transimpedance Amplifier) configuration
 */
LMP91000_Status_t LMP91000_SetTIA(LMP91000_Handle_t *hlmp, LMP91000_TIA_Gain_t gain, LMP91000_RLoad_t rload)
{
    if (!hlmp) return LMP91000_ERROR;

    // Unlock registers
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_LOCK, 0x00) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Set TIA configuration
    uint8_t tiacn = (gain << 2) | rload;
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_TIACN, tiacn) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Lock registers
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_LOCK, 0x01) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    return LMP91000_OK;
}

/**
 * @brief Set reference configuration
 */
LMP91000_Status_t LMP91000_SetReference(LMP91000_Handle_t *hlmp, LMP91000_RefSource_t ref_source,
                                        LMP91000_IntZero_t int_zero)
{
    if (!hlmp) return LMP91000_ERROR;

    // Unlock registers
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_LOCK, 0x00) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Set reference configuration (keeping existing bias settings)
    uint8_t refcn;
    if (LMP91000_ReadRegister(hlmp, LMP91000_REG_REFCN, &refcn) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Clear and set ref source and int zero bits, keep bias bits
    refcn = (refcn & 0x1F) | (ref_source << 7) | (int_zero << 5);

    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_REFCN, refcn) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Lock registers
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_LOCK, 0x01) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    return LMP91000_OK;
}

/**
 * @brief Set bias configuration
 */
LMP91000_Status_t LMP91000_SetBias(LMP91000_Handle_t *hlmp, LMP91000_BiasSign_t sign,
                                   LMP91000_BiasLevel_t level)
{
    if (!hlmp) return LMP91000_ERROR;

    // Unlock registers
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_LOCK, 0x00) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Read current reference configuration
    uint8_t refcn;
    if (LMP91000_ReadRegister(hlmp, LMP91000_REG_REFCN, &refcn) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Clear bias bits and set new values, keep ref source and int zero
    refcn = (refcn & 0xE0) | (sign << 4) | level;

    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_REFCN, refcn) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    // Lock registers
    if (LMP91000_WriteRegister(hlmp, LMP91000_REG_LOCK, 0x01) != LMP91000_OK) {
        return LMP91000_ERROR_I2C;
    }

    return LMP91000_OK;
}

/**
 * @brief Set operation mode
 */
LMP91000_Status_t LMP91000_SetMode(LMP91000_Handle_t *hlmp, LMP91000_OpMode_t mode)
{
    if (!hlmp) return LMP91000_ERROR;

    // Mode register is not locked, can write directly
    uint8_t modecn = mode; // FET short disabled by default

    return LMP91000_WriteRegister(hlmp, LMP91000_REG_MODECN, modecn);
}
