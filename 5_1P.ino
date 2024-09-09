#include "Task5.h"
#include <DHT.h>

#define DHT_PIN 2               // Define the pin for the DHT22 sensor
#define TEMP_THRESHOLD_HIGH 25  // Define the high-temperature threshold
#define TEMP_THRESHOLD_LOW 10   // Define the low-temperature threshold
#define LED_PIN 13              // Define the pin for the LED

// Initialize the DHT22 sensor
DHT dht(DHT_PIN, DHT22);

// Global variables for controlling LED blink patterns
unsigned long previousMillis = 0;
bool blink20Hz = false;
bool blink0_5Hz = false;
bool ledState = false;
float currentTemperature = 0;
unsigned long interval = 1000;  // Default interval for LED blinking

void handleTemperature(float temperature) {
    if (temperature > TEMP_THRESHOLD_HIGH) {
        blink20Hz = true;
        blink0_5Hz = false;
        interval = 50;  // Set interval for 20Hz (50ms)
        Serial.println("LED blinking at 20Hz (Temperature > 25°C)");
    } else if (temperature < TEMP_THRESHOLD_LOW) {
        blink0_5Hz = true;
        blink20Hz = false;
        interval = 2000;  // Set interval for 0.5Hz (2000ms)
        Serial.println("LED blinking at 0.5Hz (Temperature < 10°C)");
    } else {
        blink20Hz = false;
        blink0_5Hz = false;
        gpio_digital_write(LED_PIN, LOW);  // Turn off LED when temp is between 10°C and 25°C
        Serial.println("LED off (Temperature between 10°C and 25°C)");
    }
}

// Timer interrupt handler for reading the sensor and controlling the LED
void TC3_Handler() {
    currentTemperature = dht.readTemperature();
    if (!isnan(currentTemperature)) {
        Serial.print("Temperature: ");
        Serial.println(currentTemperature);
        handleTemperature(currentTemperature);
    }

    // Clear the interrupt flag
    TC3->COUNT16.INTFLAG.bit.OVF = 1;
}

void setup() {
    Serial.begin(9600);
    while (!Serial);  
    Serial.println("System startup...");

    dht.begin();
    gpio_pin_mode(LED_PIN, OUTPUT);

    // Set up the TC3 timer for 2-second intervals
    if (config_timer(3, 0, 93750, 2.0)) {
        Serial.println("Timer TC3 successfully configured.");
    } else {
        Serial.println("Error configuring Timer TC3.");
    }
    NVIC_EnableIRQ(TC3_IRQn);

    Serial.println("System initialized.");
}

void loop() {
    unsigned long currentMillis = millis();

    // Blink LED at the appropriate frequency
    if ((blink20Hz || blink0_5Hz) && (currentMillis - previousMillis >= interval)) {
        previousMillis = currentMillis;
        ledState = !ledState;
        gpio_digital_write(LED_PIN, ledState);  
    }
}
