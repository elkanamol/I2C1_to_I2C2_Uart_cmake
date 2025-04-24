#include <stm32f756xx.h>
#include "main.h"
#include "serials.h"
#include "crc.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>


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
    uint16_t total_len = get_uart_buffer_len(data_rsv1, I2C_BUFFER_SIZE);
    for (int i = total_len - 5; i < total_len; i++)
    {
        printf("%02X ", data_rsv1[i]);
    }
    printf("\r\n");

    i2c2_flag_h = 1;

    // Re-enable with the correct buffer size
    // HAL_I2C_Slave_Receive_IT(&hi2c2, data_rsv1, total_len);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart2_flag_calback = 1;
        HAL_UART_Receive_IT(&huart2, uart_buffer, 10);
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

// functon to get the length of data recieved from UART, the data terminated by \r\n
uint16_t get_uart_buffer_len(uint8_t *buffer, uint16_t len)
{
    uint16_t len_buffer = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        if ((buffer[i] == '\r' && buffer[i + 1] == '\n') || (buffer[i] == 0))
        {
            len_buffer = i;
            break;
        }
    }
    return len_buffer;
}

/**
 * @brief This function checks the UART buffer and sends the data to I2C if the CRC is correct
 *
 * @param buffer received from UART
 * @param crc_buffer crc buffer
 * @param len
 * @return uint8_t (HAL_Status) 0 if CRC is correct, 1 if
 */
uint8_t check_uart_buffer(uint8_t *uart_buffer, uint8_t *uart2_flag_calback, uint16_t len)
{
    HAL_StatusTypeDef status;
    uint8_t retval = 0;
    uint8_t txBufDynamic[UART_BUFFER_SIZE];
    if (uart2_flag_calback)
    {
        uint16_t len_buffer = get_uart_buffer_len(uart_buffer, len);
        if (len_buffer > 2)
        {
            memcpy(txBufDynamic, uart_buffer, len_buffer);
            uint16_t tx_crc = crc16(txBufDynamic, len_buffer);
            printf("CRC: %d\r\n", tx_crc);
            crc16_update_buffer(tx_crc, txBufDynamic, len_buffer);
                HAL_I2C_Slave_Receive_IT(&hi2c2, data_rsv1, len_buffer);

                status = HAL_I2C_Master_Transmit(&hi2c1, 68, txBufDynamic, len_buffer, 1000);
                if (status == HAL_OK)
                {
                    printf("HAL_OK Success sending data over I2C. Status: %d\r\n", status);
                    // Re-enable with the correct buffer size
                }
                else if (status == HAL_ERROR)
                    printf("HAL_ERROR: Check for bus errors\r\n");
                else if (status == HAL_BUSY)
                    printf("HAL_BUSY: I2C bus or peripheral is busy\r\n");
                else if (status == HAL_TIMEOUT)
                    printf("HAL_TIMEOUT: Timeout waiting for I2C operation\r\n");
                retval = status;
                uart2_flag_calback = 0;
                memset(uart_buffer, 0, UART_BUFFER_SIZE + 1);
        }
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        printf("I2C data: %s\r\n", (char *)txBufDynamic);
        
    }
    return retval;
}

uint8_t check_i2c2_buffer(uint8_t *i2c_buffer, uint8_t *i2c2_flag_h, uint16_t len)
{
    uint8_t retval = 0;
    if (i2c2_flag_h)
    {
        uint16_t len_buffer = get_uart_buffer_len(i2c_buffer, len);
        uint16_t rx_crc = crc16(i2c_buffer, len_buffer - 2);
        uint16_t crc_recived = crc16_extract_crc(data_rsv1, sizeof(data_rsv1));
        if (crc_recived == rx_crc)
        {
            printf("CRC recived: %d\r\n", crc_recived);
            printf("The crc is %s\r\n", crc_recived == rx_crc ? "OK" : "NOK");
            printf("[check_i2c2_buffer] I2C data: #%d\r\n", i);
            for (int i = 0; i < len_buffer; i++)
            {
                printf("%02x ", i2c2_buffer[i]);
            }
            printf("\r\n");
            memset(i2c_buffer, 0, I2C_BUFFER_SIZE + 1);
            retval = 0;
        }
        else
        {
            printf("CRC NOK: %d\r\n", rx_crc);
            printf("The crc is %s\r\n", crc_recived == rx_crc ? "OK" : "NOK");
            printf("[check_i2c2_buffer] I2C data: #%d\r\n", i);
            for (int i = 0; i < len_buffer; i++)
            {
                printf("%02x ", i2c2_buffer[i]);
            }
            printf("\r\n");
            memset(i2c_buffer, 0, I2C_BUFFER_SIZE + 1);
            retval = 1;
        }
        i2c2_flag_h = 0;
    }
    return retval;
}
