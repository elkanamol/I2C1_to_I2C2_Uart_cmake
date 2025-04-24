# STM32F7 I2C1 to I2C2 with UART Project

## Overview

This project implements bidirectional communication between I2C1 and I2C2 interfaces with UART functionality on an STM32F7 microcontroller. It serves as a bridge between I2C and UART interfaces, with data integrity verification using CRC-16-CCITT checksums. The project uses CMake as its build system.

## Features

- **I2C Communication**:
  - I2C1 operates as master, I2C2 operates as slave
  - Bidirectional data transfer between I2C interfaces
  - Automatic CRC-16-CCITT checksum calculation and verification

- **UART Communication**:
  - UART2 for data input/output
  - UART3 for debugging and console output
  - Support for standard input/output (printf, scanf)

- **Data Integrity**:
  - CRC-16-CCITT checksum generation and validation
  - Error reporting for failed transmissions
  - Debug output for transmission status

- **Build System**:
  - Built with CMake for cross-platform development

## Hardware Requirements

- STM32F7 series microcontroller
- Hardware with I2C and UART interfaces
- Connections for UART debugging (UART3)

## Data Flow

1. Data received on UART2 is processed and CRC is calculated
2. Data with CRC is transmitted from I2C1 (master) to I2C2 (slave)
3. I2C2 validates the CRC of received data
4. Valid data is forwarded to UART2 output
5. Status and debug information is output on UART3

## Getting Started

1. Clone the repository
2. Create a build directory: `mkdir build && cd build`
3. Configure the project: `cmake ..`
4. Build the project: `cmake --build .`
5. Flash the binary to your STM32F7 device

## Code Structure

- **CRC Module**: Handles CRC-16-CCITT calculation, validation, and buffer operations
- **Serial Module**: Manages I2C and UART communications, including callbacks and data processing
- **Main Application**: Coordinates data flow between interfaces

## Debugging

The project provides extensive debugging output on UART3, including:

- Data transmission status
- CRC calculation results
- Buffer contents in hexadecimal format
- Error conditions and their causes

![Project Architecture](<Screenshot 2025-04-06 205224.png>)
