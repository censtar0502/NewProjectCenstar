#include "u8g2.h"
#include "main.h"
#include "spi.h"
#include "gpio.h"

extern SPI_HandleTypeDef hspi2;

uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:
            HAL_SPI_Transmit(&hspi2, (uint8_t*)arg_ptr, arg_int, HAL_MAX_DELAY);
            break;

        case U8X8_MSG_BYTE_INIT:
            break;

        case U8X8_MSG_BYTE_SET_DC:
            HAL_GPIO_WritePin(SPI2_DC_GPIO_Port, SPI2_DC_Pin,
                              arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
            HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
            break;

        case U8X8_MSG_BYTE_END_TRANSFER:
            HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
            break;

        default:
            return 0;
    }
    return 1;
}

uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;

        case U8X8_MSG_GPIO_DC:
            HAL_GPIO_WritePin(SPI2_DC_GPIO_Port, SPI2_DC_Pin,
                              arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case U8X8_MSG_GPIO_CS:
            HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin,
                              arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case U8X8_MSG_GPIO_RESET:
            HAL_GPIO_WritePin(SPI2_RST_GPIO_Port, SPI2_RST_Pin,
                              arg_int ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        default:
            break;
    }
    return 1;
}
