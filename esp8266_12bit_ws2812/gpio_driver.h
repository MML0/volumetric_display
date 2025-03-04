#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#define GPOC 0x60000308 // Set bits to LOW
#define GPOS 0x60000304 // Set bits to HIGH

// Mask for the pin we want to control (e.g., GPIO2)
#define PIN0_MASK (1 << 0)  // GPIO0 (D3)
#define PIN2_MASK (1 << 2)  // GPIO2 (D4)
#define PIN4_MASK (1 << 4)  // GPIO4 (D2)
#define PIN5_MASK (1 << 5)  // GPIO5 (D1)
#define PIN12_MASK (1 << 12) // GPIO12 (D6)
#define PIN13_MASK (1 << 13) // GPIO13 (D7)
#define PIN14_MASK (1 << 14) // GPIO14 (D5)
#define PIN15_MASK (1 << 15) // GPIO15 (D8)

#define ALL_PINS_MASK (PIN0_MASK | PIN2_MASK | PIN4_MASK | PIN5_MASK | PIN12_MASK | PIN13_MASK | PIN14_MASK | PIN15_MASK)

#include <string>

uint16_t register_value = 0; // Initialize value to 0

// Bits 1-8: Green (MSB first)
// Bits 9-16: Red (MSB first)
// Bits 17-24: Blue (MSB first)
void pixel_update(uint8_t* data, size_t size) {
  noInterrupts();  
  uint32_t startTime = micros();
  for(int i=0;i<size;i++){
    // // i=j;
    // Set all pins HIGH
    *(volatile uint32_t*)GPOS = ALL_PINS_MASK;

    asm volatile ("nop; nop; nop; nop; nop;");


    register_value = 0x0000;
    register_value |= ((data[i] & 0x01 )<< 0);  // Bit 0 -> PIN0_MASK
    register_value |= ((data[i] & 0x02) << 1);  // Bit 1 -> PIN2_MASK
    register_value |= ((data[i] & 0x04) << 2);  // Bit 2 -> PIN4_MASK
    register_value |= ((data[i] & 0x08) << 2);  // Bit 3 -> PIN5_MASK
    register_value |= ((data[i] & 0x10) << 8); // Bit 4 -> PIN12_MASK
    register_value |= ((data[i] & 0x20) << 8);  // Bit 5 -> PIN13_MASK
    register_value |= ((data[i] & 0x40) << 8);  // Bit 6 -> PIN14_MASK
    register_value |= ((data[i] & 0x80) << 8);  // Bit 7 -> PIN15_MASK
    // uint32_t value = data[i]; // Shift the data to align with our pin masks
    *(volatile uint32_t*)GPOS = register_value ;  // Set the specified pins HIGH
    *(volatile uint32_t*)GPOC = ALL_PINS_MASK & ~register_value; // Set other pins LOW


    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");


    // Set all pins LOW
    *(volatile uint32_t*)GPOC = ALL_PINS_MASK;

    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
    asm volatile ("nop; nop; nop; nop; nop; nop; nop; ");
    // i=(i+1)%48;
  }


  uint32_t elapsedTime = micros() - startTime;
  Serial.printf("GPIO > iteration: %lu time: %lu us FPS: %lu\n", size, elapsedTime,1000000/elapsedTime);
  interrupts();  
}

// Function to set the color of pixels
void set_color(uint8_t* data, std::string color, int pixels, int from = 0,uint8_t pin_masck=0xFF) {
    // Ensure the 'from' index does not exceed the total number of pixels
    if (from >= pixels) return;

    // Convert the color string to lowercase for case-insensitive comparison
    for (char& c : color) c = tolower(c);

    // Bits 1-8: Green (MSB first)
    // Bits 9-16: Red (MSB first)
    // Bits 17-24: Blue (MSB first)

    // Iterate through the pixels and set the color based on the input
    for (int i = from; i < pixels; i++) {

        if (color == "red") {
            data[i * 24 + 0] =  0x00 & pin_masck;
            data[i * 24 + 1] =  0x00 & pin_masck;
            data[i * 24 + 2] =  0x00 & pin_masck;
            data[i * 24 + 3] =  0x00 & pin_masck;
            data[i * 24 + 4] =  0x00 & pin_masck;
            data[i * 24 + 5] =  0x00 & pin_masck;
            data[i * 24 + 6] =  0x00 & pin_masck;
            data[i * 24 + 7] =  0x00 & pin_masck;

            // Red (Bits 9-16): 0xFF
            data[i * 24 + 8] =  0xFF & pin_masck;
            data[i * 24 + 9] =  0xFF & pin_masck;
            data[i * 24 + 10] =  0xFF & pin_masck;
            data[i * 24 + 11] =  0xFF & pin_masck;
            data[i * 24 + 12] =  0xFF & pin_masck;
            data[i * 24 + 13] =  0xFF & pin_masck;
            data[i * 24 + 14] =  0xFF & pin_masck;
            data[i * 24 + 15] =  0xFF & pin_masck;

            // Blue (Bits 17-24): 0x00
            data[i * 24 + 16] =  0x00 & pin_masck;
            data[i * 24 + 17] =  0x00 & pin_masck;
            data[i * 24 + 18] =  0x00 & pin_masck;
            data[i * 24 + 19] =  0x00 & pin_masck;
            data[i * 24 + 20] =  0x00 & pin_masck;
            data[i * 24 + 21] =  0x00 & pin_masck;
            data[i * 24 + 22] =  0x00 & pin_masck;
            data[i * 24 + 23] =  0x00 & pin_masck;

        } else if (color == "green") {
            // Set GREEN color: G=255, R=0, B=0
            // Green (Bits 1-8): 0xFF
            data[i * 24 + 0] =  0xFF & pin_masck;
            data[i * 24 + 1] =  0xFF & pin_masck;
            data[i * 24 + 2] =  0xFF & pin_masck;
            data[i * 24 + 3] =  0xFF & pin_masck;
            data[i * 24 + 4] =  0xFF & pin_masck;
            data[i * 24 + 5] =  0xFF & pin_masck;
            data[i * 24 + 6] =  0xFF & pin_masck;
            data[i * 24 + 7] =  0xFF & pin_masck;

            // Red (Bits 9-16): 0x00
            data[i * 24 + 8] =  0x00 & pin_masck;
            data[i * 24 + 9] =  0x00 & pin_masck;
            data[i * 24 + 10] =  0x00 & pin_masck;
            data[i * 24 + 11] =  0x00 & pin_masck;
            data[i * 24 + 12] =  0x00 & pin_masck;
            data[i * 24 + 13] =  0x00 & pin_masck;
            data[i * 24 + 14] =  0x00 & pin_masck;
            data[i * 24 + 15] =  0x00 & pin_masck;

            // Blue (Bits 17-24): 0x00
            data[i * 24 + 16] =  0x00 & pin_masck;
            data[i * 24 + 17] =  0x00 & pin_masck;
            data[i * 24 + 18] =  0x00 & pin_masck;
            data[i * 24 + 19] =  0x00 & pin_masck;
            data[i * 24 + 20] =  0x00 & pin_masck;
            data[i * 24 + 21] =  0x00 & pin_masck;
            data[i * 24 + 22] =  0x00 & pin_masck;
            data[i * 24 + 23] =  0x00 & pin_masck;

        } else if (color == "blue") {
            // Set BLUE color: G=0, R=0, B=255
            // Green (Bits 1-8): 0x00
            data[i * 24 + 0] =  0x00 & pin_masck;
            data[i * 24 + 1] =  0x00 & pin_masck;
            data[i * 24 + 2] =  0x00 & pin_masck;
            data[i * 24 + 3] =  0x00 & pin_masck;
            data[i * 24 + 4] =  0x00 & pin_masck;
            data[i * 24 + 5] =  0x00 & pin_masck;
            data[i * 24 + 6] =  0x00 & pin_masck;
            data[i * 24 + 7] =  0x00 & pin_masck;

            // Red (Bits 9-16): 0x00
            data[i * 24 + 8] =  0x00 & pin_masck;
            data[i * 24 + 9] =  0x00 & pin_masck;
            data[i * 24 + 10] =  0x00 & pin_masck;
            data[i * 24 + 11] =  0x00 & pin_masck;
            data[i * 24 + 12] =  0x00 & pin_masck;
            data[i * 24 + 13] =  0x00 & pin_masck;
            data[i * 24 + 14] =  0x00 & pin_masck;
            data[i * 24 + 15] =  0x00 & pin_masck;

            // Blue (Bits 17-24): 0xFF
            data[i * 24 + 16] =  0xFF & pin_masck;
            data[i * 24 + 17] =  0xFF & pin_masck;
            data[i * 24 + 18] =  0xFF & pin_masck;
            data[i * 24 + 19] =  0xFF & pin_masck;
            data[i * 24 + 20] =  0xFF & pin_masck;
            data[i * 24 + 21] =  0xFF & pin_masck;
            data[i * 24 + 22] =  0xFF & pin_masck;
            data[i * 24 + 23] =  0xFF & pin_masck;

        } else if (color == "white") {
            // Set WHITE color: G=255, R=255, B=255
            // Green (Bits 1-8): 0xFF
            data[i * 24 + 0] =  0xFF & pin_masck;
            data[i * 24 + 1] =  0xFF & pin_masck;

            // Red (Bits 9-16): 0xFF
            data[i * 24 + 8] =  0xFF & pin_masck;
            data[i * 24 + 9] =  0xFF & pin_masck;

            // Blue (Bits 17-24): 0xFF
            data[i * 24 + 16] =  0xFF & pin_masck;
            data[i * 24 + 17] =  0xFF & pin_masck;

        } else {
            // If an invalid color is provided, do nothing for this pixel
            continue;
        }
    }
}

#endif  // GPIO_DRIVER_H
