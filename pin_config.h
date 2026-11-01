#pragma once

// Official pinout of the Waveshare ESP32-S3-Touch-AMOLED-1.75
// Source: github.com/waveshareteam/ESP32-S3-Touch-AMOLED-1.75 (libraries/Mylibrary/pin_config.h)

#define XPOWERS_CHIP_AXP2101

// 466x466 AMOLED display, CO5300 driver over QSPI
#define LCD_SDIO0 4
#define LCD_SDIO1 5
#define LCD_SDIO2 6
#define LCD_SDIO3 7
#define LCD_SCLK 38
#define LCD_CS 12
#define LCD_RESET 39
#define LCD_WIDTH 466
#define LCD_HEIGHT 466

// CST9217 capacitive touch over I2C
#define IIC_SDA 15
#define IIC_SCL 14
#define TP_INT 11
#define TP_RESET 40

// ES8311 audio. NOTE: the real MCLK is GPIO42 (verified on the board with the
// PlaneRadar2.0 project); the 16 listed before was wrong. The codec is still
// configured with a clock derived from BCLK, so MCLK barely matters.
#define I2S_MCK_IO 42
#define I2S_BCK_IO 9
#define I2S_DI_IO 10
#define I2S_WS_IO 45
#define I2S_DO_IO 8
#define PA 46

// QMI8658 (6-axis IMU) sits on the same I2C bus as touch/RTC/PMU.
// 7-bit address 0x6B (QMI8658_L_SLAVE_ADDRESS). No INT pin in this pinout:
// the firmware polls instead of waiting for a wake GPIO.
#define QMI8658_I2C_ADDR 0x6B

// TF slot (not used yet)
#define SDMMC_CLK 2
#define SDMMC_CMD 1
#define SDMMC_DATA 3
#define SDMMC_CS 41
