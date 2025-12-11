#ifndef U8X8_STM32_HAL_H
#define U8X8_STM32_HAL_H

#include "main.h"
#include "u8g2.h"

extern SPI_HandleTypeDef hspi2;

#define OLED_CS_GPIO_Port GPIOB
#define OLED_CS_Pin       GPIO_PIN_12
#define OLED_DC_GPIO_Port GPIOB
#define OLED_DC_Pin       GPIO_PIN_1
#define OLED_RST_GPIO_Port GPIOB
#define OLED_RST_Pin       GPIO_PIN_14

uint8_t u8x8_byte_stm32_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_stm32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif
