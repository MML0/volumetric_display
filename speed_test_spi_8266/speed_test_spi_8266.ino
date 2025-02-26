#include <Arduino.h>

// Define GPIO pins for three output data pins
#define DATA_PIN1 2  // First data pin
#define DATA_PIN2 5  // Second data pin
#define DATA_PIN3 14 // Third data pin

// Macros for direct register manipulation
#define GPIO_OUTPUT_SET    0x3FF20804  // Address to set GPIO pins
#define GPIO_OUTPUT_CLEAR  0x3FF20808  // Address to clear GPIO pins
#define GPIO_OUT_REG 0x60000300  // GPIO output register address



// Define the address of the registers
#define GPOC 0x60000308 // Set bits to LOW
#define GPOS 0x60000304 // Set bits to HIGH

// Mask for the pin we want to control (e.g., GPIO2)
#define PIN_MASK (1 << 2)






uint8_t data[24] = {
  0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x00, 0xFF, 0xA5, 0xFF, 0x00
};
void waitNOP(int cycles) {
  for (int i = 0; i < cycles; i++) {
    asm volatile ("nop");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(DATA_PIN1, OUTPUT);
  pinMode(DATA_PIN2, OUTPUT);
  pinMode(DATA_PIN3, OUTPUT);
  Serial.println("Starting data transmission...");

}

void loop() {
  int i = 0;
  i=(i+1)%24;


  // waitNOP(wait);

  uint32_t startTime = micros();
  int wait = 8;
  for(int j=0;j<1000;j++){
    // i=(i+1)%24;

    *(volatile uint32_t*)GPOS = PIN_MASK;
    
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop;");
    
    // Set the pin LOW
    *(volatile uint32_t*)GPOS = PIN_MASK;


    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop;");

    *(volatile uint32_t*)GPOC = PIN_MASK;


    // *gpio_clear = dataPin1Mask | dataPin2Mask | dataPin3Mask;

    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop;");


    // for (int i = 0; i < sizeof(data); i++) {
    //   *gpio_set = (data[i] & 0x80) ? dataPin1Mask : 0;
    //   *gpio_clear = (~data[i] & 0x80) ? dataPin1Mask : 0;
    //   waitNOP(wait);

      // *gpio_set = (data[i+1] & 0x80) ? dataPin2Mask : 0;
      // *gpio_clear = (~data[i+1] & 0x80) ? dataPin2Mask : 0;
      // waitNOP(wait);

      // *gpio_set = (data[i+2] & 0x80) ? dataPin3Mask : 0;
      // *gpio_clear = (~data[i+2] & 0x80) ? dataPin3Mask : 0;
      // waitNOP(wait);
    // }
  }


  uint32_t elapsedTime = micros() - startTime;
  Serial.print("Time taken for 1000 iterations: ");
  Serial.print(elapsedTime);
  Serial.println(" microseconds");

  delay(1000);
}


