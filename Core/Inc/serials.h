#ifndef SERIALS_H
#define SERIALS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stm32f7xx_hal.h"

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
int __io_putchar(int ch);
int _write(int file, char *ptr, int len);
int _read(int file, char *ptr, int len);

#ifdef __cplusplus
}
#endif

#endif