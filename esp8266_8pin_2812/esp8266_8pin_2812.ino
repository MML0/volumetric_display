// Define the address of the registers
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

// Bits 1-8: Green (MSB first)
// Bits 9-16: Red (MSB first)
// Bits 17-24: Blue (MSB first)
uint16_t data3[124] ;
uint8_t data[48] = {
  // First 24 bytes: Constructing colors using first, second, and third bits
  0b01111111, // First bit = 1 (pink), Second bit = 1 (red), Third bit = 1 (yellow)
  0b01100000, // First bit = 1, Second bit = 1, Third bit = 1
  0b01100000, // First bit = 1, Second bit = 1, Third bit = 1
  0b11100000, // First bit = 1, Second bit = 1, Third bit = 1
  0b11100000, // First bit = 1, Second bit = 1, Third bit = 1
  0b11100000, // First bit = 1, Second bit = 1, Third bit = 1
  0b11100000, // First bit = 1, Second bit = 1, Third bit = 1
  0b11111110, // First bit = 1, Second bit = 1, Third bit = 1

  0b01010100, // First bit = 1 (pink), Second bit = 0 (red), Third bit = 1 (yellow)
  0b01000000, // First bit = 1, Second bit = 0, Third bit = 1
  0b10000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b10000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b00000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b10000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b00000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b00100000, // First bit = 1, Second bit = 0, Third bit = 0

  0b11111111, // First bit = 1 (pink), Second bit = 0 (red), Third bit = 0 (yellow)
  0b10000100, // First bit = 1, Second bit = 0, Third bit = 0
  0b00000000, // First bit = 0, Second bit = 0, Third bit = 0
  0b00000000, // First bit = 0, Second bit = 0, Third bit = 0
  0b10000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b00000000, // First bit = 0, Second bit = 0, Third bit = 0
  0b10000000, // First bit = 1, Second bit = 0, Third bit = 0
  0b11111111, // First bit = 0, Second bit = 0, Third bit = 0

  // Remaining 24 bytes: All zeros
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
};
uint16_t data2[2] = {
  0x0000, 0xFFFF
};
uint16_t register_value = 0; // Initialize value to 0

void waitNOP(int cycles) {
  for (int i = 0; i < cycles; i++) {
    asm volatile ("nop");
  }
}






void setup() {
  Serial.begin(115200);
  pinMode(0, OUTPUT);   // GPIO0 (D3)
  pinMode(2, OUTPUT);   // GPIO2 (D4)
  pinMode(4, OUTPUT);   // GPIO4 (D2)
  pinMode(5, OUTPUT);   // GPIO5 (D1)
  pinMode(12, OUTPUT);  // GPIO12 (D6)
  pinMode(13, OUTPUT);  // GPIO13 (D7)
  pinMode(14, OUTPUT);  // GPIO14 (D5)
  pinMode(15, OUTPUT);  // GPIO15 (D8)
  Serial.println("Starting data transmission...");
}


void loop() {
  // i=(i+1)%2;


  int wait = 8;
  int i = 0;
  

  uint32_t startTime = micros();
  // pixel_show(data, sizeof(data));

  for(int j=0;j<1000;j++){
    // // i=j;


    // Set all pins HIGH
    *(volatile uint32_t*)GPOS = ALL_PINS_MASK;

   


    // // asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");


    register_value = 0x0000;
    register_value |= ((data[i] & 0x01 )<< 0);  // Bit 0 -> PIN0_MASK
    register_value |= ((data[i] & 0x02) << 1);  // Bit 1 -> PIN2_MASK
    register_value |= ((data[i] & 0x04) << 2);  // Bit 2 -> PIN4_MASK
    register_value |= ((data[i] & 0x08) << 2);  // Bit 3 -> PIN5_MASK
    register_value |= ((data[i] & 0x10) << 8); // Bit 4 -> PIN12_MASK
    register_value |= ((data[i] & 0x20) << 8);  // Bit 5 -> PIN13_MASK
    register_value |= ((data[i] & 0x40) << 8);  // Bit 6 -> PIN14_MASK
    register_value |= ((data[i] & 0x80) << 8);  // Bit 7 -> PIN15_MASK
    uint32_t value = data[i]; // Shift the data to align with our pin masks
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
    i=(i+1)%48;
  }


  uint32_t elapsedTime = micros() - startTime;
  Serial.print("Time taken for 1000 iterations: ");
  Serial.print(elapsedTime);
  Serial.println(" microseconds");

  delay(50);
}


