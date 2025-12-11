#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "ssd1322_test.h"

#define OLED_CS_LOW()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define OLED_CS_HIGH()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)
#define OLED_DC_CMD()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define OLED_DC_DATA()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)
#define OLED_RST_LOW()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET)
#define OLED_RST_HIGH()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi2;

static void SSD1322_WriteCommand(uint8_t cmd)
{
    OLED_CS_LOW();
    OLED_DC_CMD();
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
    OLED_CS_HIGH();
}

static void SSD1322_WriteData(uint8_t *data, uint16_t size)
{
    OLED_CS_LOW();
    OLED_DC_DATA();
    HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
    OLED_CS_HIGH();
}

void SSD1322_Reset(void)
{
    OLED_RST_LOW();
    HAL_Delay(50);
    OLED_RST_HIGH();
    HAL_Delay(50);
}

void SSD1322_Init(void)
{
    SSD1322_Reset();

    // Unlock
    SSD1322_WriteCommand(0xFD);
    uint8_t unlock = 0x12;
    SSD1322_WriteData(&unlock, 1);

    // Display off
    SSD1322_WriteCommand(0xAE);

    // Clock divider
    SSD1322_WriteCommand(0xB3);
    uint8_t clkdiv = 0x91;
    SSD1322_WriteData(&clkdiv, 1);

    // Multiplex ratio 1/64
    SSD1322_WriteCommand(0xCA);
    uint8_t multiplex = 0x3F;
    SSD1322_WriteData(&multiplex, 1);

    // Display offset
    SSD1322_WriteCommand(0xA2);
    uint8_t offset = 0x00;
    SSD1322_WriteData(&offset, 1);

    // Start line
    SSD1322_WriteCommand(0xA1);
    uint8_t start_line = 0x00;
    SSD1322_WriteData(&start_line, 1);

    // === ВАЖНО: корректный REMAP для нормального порядка байтов ===
    // Bit7=1: enable nibble remap
    // Bit6=0: disable column address remap
    // Bit5=1: address increment mode
    // Bit4=1: dual COM mode (256 px)
    // Byte2=0x00: left-to-right, top-to-bottom
    SSD1322_WriteCommand(0xA0);
    uint8_t remap[2] = {0x17, 0x00};
    SSD1322_WriteData(remap, 2);

    // Internal VDD regulator
    SSD1322_WriteCommand(0xAB);
    uint8_t vdd = 0x01;
    SSD1322_WriteData(&vdd, 1);

    // Contrast, master contrast
    SSD1322_WriteCommand(0xC1); uint8_t contrast = 0x9F; SSD1322_WriteData(&contrast, 1);
    SSD1322_WriteCommand(0xC7); uint8_t master_contrast = 0x0F; SSD1322_WriteData(&master_contrast, 1);

    // Phase length
    SSD1322_WriteCommand(0xB1); uint8_t phase = 0xE2; SSD1322_WriteData(&phase, 1);

    // Enhancement
    SSD1322_WriteCommand(0xD1);
    uint8_t enhance[2] = {0xA2, 0x20};
    SSD1322_WriteData(enhance, 2);

    // Pre-charge & VCOMH
    SSD1322_WriteCommand(0xBB); uint8_t precharge = 0x1F; SSD1322_WriteData(&precharge, 1);
    SSD1322_WriteCommand(0xB6); uint8_t precharge2 = 0x08; SSD1322_WriteData(&precharge2, 1);
    SSD1322_WriteCommand(0xBE); uint8_t vcomh = 0x07; SSD1322_WriteData(&vcomh, 1);

    SSD1322_WriteCommand(0xA6); // normal display
    SSD1322_WriteCommand(0xAF); // display ON
    HAL_Delay(100);
}


void SSD1322_SetWindow(uint8_t col_start, uint8_t col_end, uint8_t row_start, uint8_t row_end)
{
    SSD1322_WriteCommand(0x15);
    uint8_t cols[2] = {0x1C, 0x5B};  // ← именно этот диапазон 0x1C–0x5B даёт 256 px
    SSD1322_WriteData(cols, 2);

    SSD1322_WriteCommand(0x75);
    uint8_t rows[2] = {row_start, row_end};
    SSD1322_WriteData(rows, 2);

    SSD1322_WriteCommand(0x5C);
}


void SSD1322_FillWhite(void)
{
    SSD1322_SetWindow(0x00, 0x77, 0x00, 0x3F);
    uint8_t buf[128];
    for (int i = 0; i < 128; i++) buf[i] = 0xFF;

    for (int y = 0; y < 64; y++)
        SSD1322_WriteData(buf, 128);
}

void SSD1322_FillGray(uint8_t level)
{
    SSD1322_SetWindow(0x00, 0x77, 0x00, 0x3F);
    uint8_t pixel = (level << 4) | level; // одинаковый оттенок в обоих полубайтах
    uint8_t buf[128];
    for (int i = 0; i < 128; i++) buf[i] = pixel;

    for (int y = 0; y < 64; y++)
        SSD1322_WriteData(buf, 128);
}

// === Градиентный тест ===
void SSD1322_TestGradient(void)
{
    SSD1322_SetWindow(0x00, 0x77, 0x00, 0x3F);

    uint8_t buf[128];
    uint8_t steps = 16; // 16 градаций (4-bit)
    uint8_t col_per_step = 128 / steps;

    for (uint8_t s = 0; s < steps; s++)
    {
        uint8_t val = (s << 4) | s; // 0x00 .. 0xFF
        for (int i = 0; i < 128; i++)
        {
            if (i / col_per_step == s)
                buf[i] = val;
        }
        for (int y = 0; y < 64; y++)
            SSD1322_WriteData(buf, 128);
    }
}
