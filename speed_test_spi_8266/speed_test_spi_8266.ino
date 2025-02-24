#include <Arduino.h>

// Define GPIO pins for Data, Clock, and Latch
#define DATA_PIN   2  // DS (Data) pin of 74HC595
#define CLOCK_PIN  0  // SH_CP (Clock) pin of 74HC595
#define LATCH_PIN  4  // ST_CP (Latch) pin of 74HC595

// Macros for direct register manipulation
#define GPIO_OUTPUT_SET    0x3FF20804  // Address to set GPIO pins
#define GPIO_OUTPUT_CLEAR  0x3FF20808  // Address to clear GPIO pins

void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  Serial.println("Starting test with 74HC595...");
}

void loop() {
  volatile uint32_t *gpio_set = (volatile uint32_t *)GPIO_OUTPUT_SET;
  volatile uint32_t *gpio_clear = (volatile uint32_t *)GPIO_OUTPUT_CLEAR;

  uint32_t dataPinMask = (1 << DATA_PIN);
  uint32_t clockPinMask = (1 << CLOCK_PIN);
  uint32_t latchPinMask = (1 << LATCH_PIN);

  uint32_t startTime = micros();
  uint8_t data = 0x1F;  // 5-bit data
    
  for (int i = 0; i < 1000; i++) {
    *gpio_clear = latchPinMask; // Latch low
    
    // asm volatile("NOP"); 
    // Unrolled loop for shifting 5 bits
    *gpio_set = (data & 0x80) ? dataPinMask : 0;
    *gpio_clear = (~data & 0x80) ? dataPinMask : 0;
    *gpio_set = clockPinMask;
    *gpio_clear = clockPinMask;
    
    *gpio_set = (data & 0x40) ? dataPinMask : 0;
    *gpio_clear = (~data & 0x40) ? dataPinMask : 0;
    *gpio_set = clockPinMask;
    *gpio_clear = clockPinMask;
    
    *gpio_set = (data & 0x20) ? dataPinMask : 0;
    *gpio_clear = (~data & 0x20) ? dataPinMask : 0;
    *gpio_set = clockPinMask;
    *gpio_clear = clockPinMask;
    
    // *gpio_set = (data & 0x10) ? dataPinMask : 0;
    // *gpio_clear = (~data & 0x10) ? dataPinMask : 0;
    // *gpio_set = clockPinMask;
    // *gpio_clear = clockPinMask;
    
    // *gpio_set = (data & 0x08) ? dataPinMask : 0;
    // *gpio_clear = (~data & 0x08) ? dataPinMask : 0;
    // *gpio_set = clockPinMask;
    // *gpio_clear = clockPinMask;
    
    *gpio_set = latchPinMask; // Latch high
    *gpio_clear = latchPinMask;
  }

  uint32_t elapsedTime = micros() - startTime;
  Serial.print("Time taken for 1000 iterations: ");
  Serial.print(elapsedTime);
  Serial.println(" microseconds");

  delay(1000);
}
