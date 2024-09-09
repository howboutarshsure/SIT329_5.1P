#ifndef TASK5_H
#define TASK5_H

#include <Arduino.h>

void gpio_pin_mode(uint8_t pin, uint8_t mode);
void gpio_digital_write(uint8_t pin, uint8_t val);
bool config_timer(uint8_t timerNum, uint16_t startCount, uint16_t endCount, float resolution);
uint16_t getTC3_count();

#endif
