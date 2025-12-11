#ifndef SSD1322_TEST_H
#define SSD1322_TEST_H

#include "main.h"

void SSD1322_Reset(void);
void SSD1322_Init(void);
void SSD1322_FillWhite(void);
void SSD1322_FillGray(uint8_t level);
void SSD1322_TestGradient(void);

#endif
