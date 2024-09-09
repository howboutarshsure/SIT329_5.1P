#include "Task5.h"
#include <stdbool.h>
#include <Arduino.h>

// Function to set the mode of a GPIO pin
void gpio_pin_mode(uint8_t pin, uint8_t mode) {
    if (mode == OUTPUT) {
        PORT->Group[g_APinDescription[pin].ulPort].DIRSET.reg = (1 << g_APinDescription[pin].ulPin);
    } else {
        PORT->Group[g_APinDescription[pin].ulPort].DIRCLR.reg = (1 << g_APinDescription[pin].ulPin);
    }
}

// Function to write a digital value to a GPIO pin
void gpio_digital_write(uint8_t pin, uint8_t val) {
    if (val == HIGH) {
        PORT->Group[g_APinDescription[pin].ulPort].OUTSET.reg = (1 << g_APinDescription[pin].ulPin);
    } else {
        PORT->Group[g_APinDescription[pin].ulPort].OUTCLR.reg = (1 << g_APinDescription[pin].ulPin);
    }
}

// Function to configure timer parameters
bool config_timer(uint8_t timerId, uint16_t startVal, uint16_t stopVal, float resolution) {
    if (timerId != 3) {
        return false;
    }

    // Enable clock for Timer TC3
    PM->APBCMASK.reg |= PM_APBCMASK_TC3;
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID_TCC2_TC3 | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);

    // Set up the timer
    TC3->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;  
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

    TC3->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16;  
    TC3->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1024;

    // Set compare value based on desired interval
    TC3->COUNT16.CC[0].reg = stopVal;  
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

    // Enable overflow interrupt
    TC3->COUNT16.INTENSET.reg = TC_INTENSET_OVF;
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

    // Activate the timer
    TC3->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
    while (TC3->COUNT16.STATUS.bit.SYNCBUSY);

    return true;
}

// Function to retrieve the current TC3 timer count
uint16_t getTC3_count() {
    return TC3->COUNT16.COUNT.reg;
}
