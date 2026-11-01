#pragma once
#include <Arduino.h>

// RTC PCF85063: time persists as long as the board is powered
bool rtcBegin();
uint32_t rtcEpoch();             // unix seconds; 0 when the RTC is not valid
void rtcSetEpoch(uint32_t e);

// PMU AXP2101: battery state
bool batBegin();
void pmuEnablePanel();           // enables BLDO1 (OLED VDD 3.3V); call before gfx->begin()
int batPercent();                // 0-100, -1 when no battery is connected
bool batCharging();
bool usbPresent();
void setPowerCacheInterval(uint32_t ms);

// AXP2101 PWR button: 4s long press = physical power-off (RTC stays alive);
// the firmware captures the short press (display on/off)
void pwrSetup();
bool pwrShortPressed();  // poll from the loop
