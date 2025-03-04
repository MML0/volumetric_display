#include "Arduino.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string>

#include <WiFi.h>
#include <WiFiUdp.h>

#define PIN0_MASK (1 << 0)  // GPIO0
#define PIN2_MASK (1 << 2)  // GPIO2
#define PIN4_MASK (1 << 4)  // GPIO4
#define PIN5_MASK (1 << 5)  // GPIO5
#define PIN12_MASK (1 << 12) // GPIO12
#define PIN13_MASK (1 << 13) // GPIO13
#define PIN14_MASK (1 << 14) // GPIO14
#define PIN15_MASK (1 << 15) // GPIO15
#define LED_PIN 2
#define ALL_PINS_MASK (PIN0_MASK | PIN2_MASK | PIN4_MASK | PIN5_MASK | PIN12_MASK | PIN13_MASK | PIN14_MASK | PIN15_MASK)


const int CHUNK_SIZE = 8000;  
const int TOTAL_SIZE = 30 * 1024;
uint8_t data[TOTAL_SIZE];
uint8_t buffer[CHUNK_SIZE];  // Declare globally
unsigned long start_time, end_time;
unsigned long lastPacketTime = 0;  // Track the last time a packet was received
int receivedBytes = 0;

size_t data_size = sizeof(data) / sizeof(data[0]); // Calculate the size

void setup() {

    // Print elapsed time in microseconds
    Serial.begin(115200);
    Serial.println("starting ...");
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = ALL_PINS_MASK;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}
void preditelbe_delay(int iterations) {
    for (int i = 0; i < iterations; i++) {
        asm volatile("nop;");
    }
}
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
void pixel_update(uint8_t* data, size_t size) {
    noInterrupts();  
    uint64_t start_time = esp_timer_get_time();
    uint16_t register_value = 0;
    
    for (int i = 0; i < size; i++) {
        GPIO.out_w1ts = ALL_PINS_MASK; // Set all pins HIGH
        // asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; "); // 20 nop ==4.27 / 1000us
        // asm volatile("nop;");
        preditelbe_delay(4);
        register_value = 0x0000;
        register_value |= ((data[i] & 0x01) << 0);  // Bit 0 -> PIN0_MASK
        register_value |= ((data[i] & 0x02) << 1);  // Bit 1 -> PIN2_MASK
        register_value |= ((data[i] & 0x04) << 2);  // Bit 2 -> PIN4_MASK
        register_value |= ((data[i] & 0x08) << 2);  // Bit 3 -> PIN5_MASK
        register_value |= ((data[i] & 0x10) << 8);  // Bit 4 -> PIN12_MASK
        register_value |= ((data[i] & 0x20) << 8);  // Bit 5 -> PIN13_MASK
        register_value |= ((data[i] & 0x40) << 8); // Bit 6 -> PIN14_MASK
        register_value |= ((data[i] & 0x80) << 8); // Bit 7 -> PIN15_MASK
        
        GPIO.out_w1ts = register_value; // Set required pins HIGH
        GPIO.out_w1tc = ALL_PINS_MASK & ~register_value; // Set other pins LOW

        preditelbe_delay(7);
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; "); // 20 nop ==4.27 / 1000us

        GPIO.out_w1tc = ALL_PINS_MASK; // Set all pins LOW
        
        preditelbe_delay(7);
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; "); // 20 nop ==4.27 / 1000us


    }
    
    uint64_t elapsed_time = esp_timer_get_time() - start_time;
    Serial.printf("GPIO > iteration: %lu time: %llu us FPS: %llu\n", size, elapsed_time, 1000000 / elapsed_time);
    interrupts();  
}
void loop() {
    // Disable interrupts
    
    memset(data, 0x00, 1000);        // Fill the data array with 0s
    set_color(data,"blue",4,0xFF) ;
    // memset(data, 0xFF, 48);        // Fill the data array with 0s
    pixel_update(data, 1000);
    delay(100);
    set_color(data,"red",2,0xFF) ;
    pixel_update(data, TOTAL_SIZE);
    delay(100);
    set_color(data,"blue",3,0xFF) ;
    int i = 3;
    data[i * 24 + 0] =  0xF2 ;
    data[i * 24 + 1] =  0xF0;

    // Red (Bits 9-16): 0xFF
    data[i * 24 + 8] =  0xa2 ;
    data[i * 24 + 9] =  0xF2;

    // Blue (Bits 17-24): 0xFF
    data[i * 24 + 16] =  0x14 ;
    data[i * 24 + 17] =  0x04;

    pixel_update(data, TOTAL_SIZE);
    delay(100);
    set_color(data,"blue",4,0xFF) ;
    pixel_update(data, TOTAL_SIZE);
    delay(100);
    set_color(data,"blue",5,0xFF) ;
    pixel_update(data, TOTAL_SIZE);

    // GPIO.out_w1ts = (1 << LED_PIN); // Set LED on
    // delay(100);
    // GPIO.out_w1tc = (1 << LED_PIN); // Clear LED off
    // delay(100);

}
