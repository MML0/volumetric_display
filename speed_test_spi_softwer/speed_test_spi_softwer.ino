#include <Arduino.h>
#include <SPI.h>

// Define GPIO pins for SPI and Latch
#define LATCH_PIN 4  // ST_CP (Latch) pin of 74HC595

// Direct register access for GPIO
#define REG_GPIO_OUT_W1TS (uint32_t *)0x60004008  // GPIO output set register
#define REG_GPIO_OUT_W1TC (uint32_t *)0x6000400C  // GPIO output clear register
uint32_t latchPinMask = (1 << LATCH_PIN);

void setup() {
    Serial.begin(115200);
    pinMode(LATCH_PIN, OUTPUT);
    SPI.begin();
    SPI.beginTransaction(SPISettings(160000000, MSBFIRST, SPI_MODE0));
    Serial.println("Starting random data test with 74HC595 using SPI...");
}

void loop() {
    noInterrupts(); // Disable interrupts for precise timing
    uint32_t startTime = micros();
    uint8_t data = 0xAA;
    for (int i = 0; i < 1000; i++) {
        *REG_GPIO_OUT_W1TC = latchPinMask; // Latch low
        SPI.transfer(data); // Send data using SPI
        *REG_GPIO_OUT_W1TS = latchPinMask; // Latch high
    }
    
    uint32_t elapsedTime = micros() - startTime;
    interrupts(); // Enable interrupts

    Serial.print("Time taken for 1000 iterations: ");
    Serial.print(elapsedTime);
    Serial.println(" microseconds");

    delay(1000);  // Wait 1 second before repeating the test
}

// Function to send data to the 74HC595 shift register using SPI
void sendTo74HC595(uint8_t data, uint8_t bits) {
    *REG_GPIO_OUT_W1TC = latchPinMask; // Latch low
    SPI.transfer(data << (8 - bits)); // Send data using SPI
    *REG_GPIO_OUT_W1TS = latchPinMask; // Latch high
}
