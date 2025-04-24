#include "crc.h"

#define CRC16_CCITT_INIT_VALUE 0xFFFF
#define CRC16_CCITT_POLYNOMIAL 0x1021
#define CRC16_CCITT_XOR_VALUE 0xFFFF

uint16_t crc16(uint8_t *data, uint16_t length)
{
    uint16_t crc = CRC16_CCITT_INIT_VALUE;
    
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for(uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ CRC16_CCITT_POLYNOMIAL;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc ^ CRC16_CCITT_XOR_VALUE;
}

void crc16_update_buffer(uint16_t crc, uint8_t *data, uint16_t length)
{
    data[length] = (uint8_t)(crc  >> 8);
    data[length + 1] = (uint8_t)(crc & 0xFF);
}

// uint16_t crc16_extract_crc(uint8_t *data, uint16_t length)
// {
//     return (uint16_t)(data[length-1] << 8) | data[length];
// }

uint16_t crc16_extract_crc(uint8_t *data, uint16_t length)
{
    if (length < 2)
    {
        return 0;
    }

    // For little-endian CRC storage
    return (uint16_t)(data[length - 2] << 8) | data[length - 1];
}

// uint16_t crc16_extract_crc(uint8_t *data, uint16_t length)
// {
//     // Assuming the CRC is the last 2 bytes of the data
//     // And length is the total length including CRC
//     if (length < 2)
//     {
//         return 0; // Not enough data to contain a CRC
//     }

//     // Correct indexing for the last two bytes
//     return (uint16_t)(data[length - 2] << 8) | data[length - 1];
// }
