#include "Arduino.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"

// #include "esp_task_wdt.h"

#include <string>

#include <WiFi.h>
#include <WiFiUdp.h>

#define PIN4_MASK (1 << 4)   // GPIO4
#define PIN5_MASK (1 << 5)   // GPIO5
#define PIN18_MASK (1 << 18) // GPIO18
#define PIN19_MASK (1 << 19) // GPIO19
#define PIN21_MASK (1 << 21) // GPIO21
#define PIN22_MASK (1 << 22) // GPIO22
#define PIN23_MASK (1 << 23) // GPIO23
#define PIN13_MASK (1 << 13) // GPIO13

#define LED_PIN 2
#define ALL_PINS_MASK (PIN4_MASK | PIN5_MASK | PIN18_MASK | PIN19_MASK | PIN21_MASK | PIN22_MASK | PIN23_MASK | PIN13_MASK)


const char* ssid = ".";
const char* password = "12345678an";
const IPAddress staticIP(192, 168, 43, 222);
const IPAddress gateway(192, 168, 43, 1);
const IPAddress subnet(255, 255, 255, 0);

unsigned int port = 8266;
WiFiUDP udp;



const int CHUNK_SIZE = 2000;  
const int TOTAL_SIZE = 24 * 1100;
// uint8_t data[100];
uint8_t data1[TOTAL_SIZE];
// uint8_t data2[TOTAL_SIZE];
uint8_t buffer[CHUNK_SIZE];  // Declare globally
unsigned long start_time, end_time;
unsigned long lastPacketTime = 0;  // Track the last time a packet was received
int receivedBytes = 0;
int packetSize = 0;
// size_t data_size = sizeof(data1) / sizeof(data1[0]); // Calculate the size

// Flags for buffer usage
volatile bool buffer1InUse = false;
volatile bool buffer2InUse = true;

// Flags for buffer readiness (ensures each buffer is sent only once)
volatile bool buffer1Ready = false;
volatile bool buffer2Ready = false;

volatile bool mid_transfer = false;


TaskHandle_t task1Handle = NULL; // UDP Receiver Task
TaskHandle_t task2Handle = NULL; 


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
    GPIO.out_w1tc = ALL_PINS_MASK; // Set all pins LOW
    noInterrupts();  
    uint64_t start_time = esp_timer_get_time();
    uint32_t register_value = 0;
    
    for (int i = 0; i < size; i++) {
        GPIO.out_w1ts = ALL_PINS_MASK; // Set all pins HIGH
        // asm volatile("nop;");
        // asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; "); // 20 nop ==4.27 / 1000us   
        preditelbe_delay(1);
        // preditelbe_delay(4);   // 16 bit register wait time
        register_value = 0x00000000;
        register_value |= ((data[i] & 0x01) << 4);  // Bit 0 -> GPIO4
        register_value |= ((data[i] & 0x02) << 4);  // Bit 1 -> GPIO5
        register_value |= ((data[i] & 0x04) << 16); // Bit 2 -> GPIO18
        register_value |= ((data[i] & 0x08) << 16); // Bit 3 -> GPIO19
        register_value |= ((data[i] & 0x10) << 17); // Bit 4 -> GPIO21
        register_value |= ((data[i] & 0x20) << 17); // Bit 5 -> GPIO22
        register_value |= ((data[i] & 0x40) << 17); // Bit 6 -> GPIO23
        register_value |= ((data[i] & 0x80) << 6); // Bit 7 -> GPIO13

        GPIO.out_w1ts = register_value; // Set required pins HIGH
        GPIO.out_w1tc = ALL_PINS_MASK & ~register_value; // Set other pins LOW

        preditelbe_delay(7);
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; "); // 20 nop ==4.27 / 1000us

        GPIO.out_w1tc = ALL_PINS_MASK; // Set all pins LOW
        
        preditelbe_delay(7);
        asm volatile("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; "); // 20 nop ==4.27 / 1000us


    }
    
    uint64_t elapsed_time = esp_timer_get_time() - start_time +1;
    Serial.printf("GPIO > iteration: %lu time: %llu us FPS: %llu\n", size, elapsed_time, 1000000 / elapsed_time);
    interrupts();  
}


void setup() {
    // Print elapsed time in microseconds
    // Serial.begin(115200);
    Serial.println("starting ...");
    Serial.print("Tick rate (Hz): ");
    Serial.println(configTICK_RATE_HZ); // Prints the tick rate
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = ALL_PINS_MASK;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("Connecting ... ");

    while (WiFi.status() != WL_CONNECTED ) {
      // set_color(data1,"blue",2,0xFF) ;
      memset(data1, 0, TOTAL_SIZE);        // Fill the data array with 0s
      // memset(data2, 0, TOTAL_SIZE); 
      memset(data1, 0xFF, 24);        // Fill the data array with 0s
      // memset(data2, 0xFF, 24);        // Fill the data array with 0s
      pixel_update(data1, TOTAL_SIZE);

      delay(500);
      Serial.print(".");

      memset(data1, 0, TOTAL_SIZE);  // Fill the data array with 0s
      pixel_update(data1, TOTAL_SIZE);
      delay(500);

    }

    // esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(0));
    // disableCore0WDT();
    // disableCore1WDT();
    // esp_task_wdt_config_t config = {
    //     .timeout_ms = 30000, // Timeout in milliseconds (30 seconds)
    //     .idle_core_mask = (1 << 2) - 1, // Monitor all cores
    //     .trigger_panic = true, // Trigger a panic when the watchdog expires
    // };

    // // Initialize the Task Watchdog Timer with the configuration
    // esp_err_t err = esp_task_wdt_init(&config);
    // if (err != ESP_OK) {
    //     // Handle initialization error
    //     printf("Failed to initialize Task Watchdog Timer: %s\n", esp_err_to_name(err));
    // }

    Serial.println("Connected to WiFi");
    udp.begin(port);
    Serial.println("Starting data transmission...");
    // xTaskCreatePinnedToCore(receive_data_task, "Receive Task", 4096, NULL, 1, &task1Handle, 1); // Run on Core 0
    // xTaskCreatePinnedToCore(update_pixel_task, "Update Task", 4096, NULL, 1, &task2Handle, 0); // Run on Core 1


}
void loop() {
  packetSize = udp.parsePacket();
  if (packetSize > 0) {

      udp.read(buffer, packetSize);

      if ((receivedBytes + packetSize)> TOTAL_SIZE) receivedBytes = 0;

      // Store data in correct position
      if (packetSize > 2 ){
          memcpy(&data1[receivedBytes], buffer , packetSize );
          receivedBytes += packetSize;
          // return ;
      }

      if (buffer[0] == 0xAA && packetSize == 1 ){
          receivedBytes = 0;
          start_time = micros();
          // return;
      }
      if (buffer[0] == 0xBB && packetSize == 1 ){
          end_time = micros();
          // Serial.printf("WIFI > data size: %lu time: %lu us FPS: %lu\n", receivedBytes, end_time - start_time,1000000/(end_time - start_time));

          // // you need this delay to reset data if data come faster than 50us
          // delay(0.05); 
          // add a moving avrege i if receve data shrter than avrege dope data and notity me with a blink in strip 
          pixel_update(data1, receivedBytes);

          // end_time = micros();
          // Serial.printf("%c%c%c > data size: %lu time : %lu us FPS: %lu \n\n",(receivedBytes/100)%255,(receivedBytes/10)%255,(receivedBytes/10+128)%255
          //               ,receivedBytes, end_time - start_time,1000000/(end_time - start_time));
          return;
      }


      // if (receivedBytes == 30000){
      //     end_time = micros();
      //     Serial.printf("Transfer completed in %lu us\n", end_time - start_time);
      //     // pixel_update(data, receivedBytes);
      //     return;
      // }
      // Update the last packet time
      lastPacketTime = millis();

      // remote command to reset
      if (buffer[0] == 0x10 && buffer[1] == 0xFF && packetSize == 2) {
          // Reset ESP
          ESP.restart(); // Replace this with your actual ESP reset function
          return;
      }
      // remote command to turn off all pixels
      if (buffer[0] == 0x0B && buffer[1] == 0x1C && packetSize == 2) {
          memset(data1, 0, TOTAL_SIZE);  // Fill the data array with 0s
          pixel_update(data1, TOTAL_SIZE);  // Update pixels with all black
          Serial.println("command: Sent all black.");
          return;
      }
  } else if (millis() - lastPacketTime > 5000) {  // Check if no packet has been received for 10 seconds
    // Send all black (0) as the last packet size
    memset(data1, 0, TOTAL_SIZE);  // Fill the data array with 0s
    pixel_update(data1, TOTAL_SIZE);  // Update pixels with all black
    Serial.println("No packet received for  sseconds. Sent all black.");

    // Reset the last packet time to avoid repeated triggers
    lastPacketTime = millis();
  }
}

// void receive_data_task(void *pvParameters){
//   // esp_task_wdt_add(NULL);
  
//   while(true){
//     int packetSize = udp.parsePacket();
//     if (packetSize > 0) {
//         // Update the last packet time
//         lastPacketTime = millis();

//         udp.read(buffer, packetSize);

//         if (buffer[0] == 0xAA && packetSize == 1 ){
//             receivedBytes = 0;
//             mid_transfer = true;
//             start_time = micros();
//             // return;
//         }
//         if (buffer[0] == 0xBB && packetSize == 1 ){
//             mid_transfer = false;

//             end_time = micros();
//             Serial.printf("WIFI > data size: %lu time: %lu us FPS: %lu\n", receivedBytes, end_time - start_time,1000000/(end_time - start_time));

//             if (!buffer1InUse && !buffer1Ready) {
//               // memcpy(&data1[receivedBytes], buffer, packetSize);
//               buffer1InUse = true;
//               buffer2InUse = false;
//               buffer1Ready = true; // Mark buffer1 as ready
//             } else if (!buffer2InUse && !buffer2Ready) {
//               // memcpy(&data2[receivedBytes], buffer, packetSize);
//               buffer2InUse = true;
//               buffer1InUse = false;
//               buffer2Ready = true; // Mark buffer2 as ready
//             }

//             end_time = micros()+1;
//             Serial.printf("%c%c%c > data size: %lu time : %lu us FPS: %lu \n\n",(receivedBytes/100)%255,(receivedBytes/10)%255,(receivedBytes/10+128)%255
//                           ,receivedBytes, end_time - start_time,1000000/(end_time - start_time));
//             // return;
//         }

//         if ((receivedBytes + packetSize)> TOTAL_SIZE) receivedBytes = 0;

//         if (!buffer1InUse && !buffer1Ready && packetSize > 2 ) {
//           memcpy(&data1[receivedBytes], buffer, packetSize);
//           // buffer1InUse = true;
//           // buffer2InUse = false;
//           // buffer1Ready = true; // Mark buffer1 as ready
//         } else if (!buffer2InUse && !buffer2Ready && packetSize > 2 ) {
//           memcpy(&data2[receivedBytes], buffer, packetSize);
//           // buffer2InUse = true;
//           // buffer1InUse = false;
//           // buffer2Ready = true; // Mark buffer2 as ready
//         }
//         // Store data in correct position
//         // memcpy(&data[receivedBytes], buffer , packetSize );
//         if (packetSize > 2 ) {
//           receivedBytes += packetSize;
//         }
//         // if (receivedBytes == 30000){
//         //     end_time = micros();
//         //     Serial.printf("Transfer completed in %lu us\n", end_time - start_time);
//         //     // pixel_update(data, receivedBytes);
//         //     return;
//         // }

//         // remote command to reset
//         if (buffer[0] == 0x10 && buffer[1] == 0xFF && packetSize == 2) {
//             // Reset ESP
//             ESP.restart(); // Replace this with your actual ESP reset function
//             // return;
//         }
//         // remote command to turn off all pixels
//         if (buffer[0] == 0x0B && buffer[1] == 0x1C && packetSize == 2) {
//             memset(data1, 0, TOTAL_SIZE);  // Fill the data array with 0s
//             pixel_update(data1, TOTAL_SIZE);  // Update pixels with all black
//             Serial.println("command: Sent all black.");
//             // return;
//         }
//     }
  
//     // Check if no packet has been received for 10 seconds
//     if (millis() - lastPacketTime > 1000) {  
//       mid_transfer = false;
//       // Send all black (0) as the last packet size
//       memset(data1, 0, TOTAL_SIZE);  // Fill the data array with 0s
//       memset(data2, 0, TOTAL_SIZE);  // Fill the data array with 0s
//       pixel_update(data1, TOTAL_SIZE);  // Update pixels with all black
//       Serial.println("No packet received for  sseconds. Sent all black.");

//       // Reset the last packet time to avoid repeated triggers
//       lastPacketTime = millis();
//     }
//     // vTaskDelay(pdMS_TO_TICKS(1));

//     if (!mid_transfer){
//       // taskYIELD();
//       vTaskDelay(1); // Short delay to allow switching
//     }
//     // esp_task_wdt_reset();
//     // portYIELD();

//   }
// }

// void update_pixel_task(void *pvParameters) {
//   while (true) {
//     if (buffer1Ready) {
//       // Serial.println("Data from data1:");
//       // Serial.println(data1);
//       pixel_update(data1, receivedBytes);
//       buffer1Ready = false; // Reset flag after sending
//       buffer2InUse = false; // Allow buffer2 to be filled
//     } else if (buffer2Ready) {
//       // Serial.println("Data from data2:");
//       // Serial.println(data2);
//       pixel_update(data2, receivedBytes);

//       buffer2Ready = false; // Reset flag after sending
//       buffer1InUse = false; // Allow buffer1 to be filled
//     }
//     vTaskDelay(1); // Short delay to allow switching
//     // taskYIELD();
//   }
// }

