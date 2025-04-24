#include <stm32f756xx.h>
#include "main.h"
#include "serials.h"
#include <stdio.h>
#include <stdint.h>

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    // Update to print the full received data length
    printf("Data received from I2C master\r\n");

    // Print the last few bytes including CRC for debugging
    printf("Last bytes including CRC: ");
    uint16_t total_len = sizeof(txBufStatic) + 2;
    for (int i = total_len - 5; i < total_len; i++)
    {
        printf("%02X ", data_rsv1[i]);
    }
    printf("\r\n");

    i2c2_flag_h = 1;

    // Re-enable with the correct buffer size
    HAL_I2C_Slave_Receive_IT(&hi2c2, data_rsv1, total_len);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart2_flag_calback = 1;
        HAL_UART_Receive_IT(&huart2, uart_buffer, 1);
    }
}

// printf
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}

int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)ptr, len, 0xFFFF);
    return len;
}

// scanf
int _read(int file, char *ptr, int len)
{
    int ch = 0;
    HAL_UART_Receive(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    if (ch == 13)
    {
        ch = 10;
        HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    }
    else if (ch == 8)
    {
        ch = 0x30;
        HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    }

    *ptr = ch;

    return 1;
}