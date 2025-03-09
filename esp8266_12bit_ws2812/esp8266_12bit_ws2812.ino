#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "gpio_driver.h"

// #include <WiFi.h>  // Use <ESP8266WiFi.h> for ESP8266
const char* ssid = ".";
const char* password = "12345678an";
const IPAddress staticIP(192, 168, 43, 222);
const IPAddress gateway(192, 168, 43, 1);
const IPAddress subnet(255, 255, 255, 0);

unsigned int port = 8266;
WiFiUDP udp;


const int CHUNK_SIZE = 5000;  
const int TOTAL_SIZE = 24 * 1202;
uint8_t data[TOTAL_SIZE];
uint8_t data_buffer[TOTAL_SIZE/2];
uint8_t buffer[CHUNK_SIZE];  // Declare globally
unsigned long start_time, end_time;
unsigned long lastPacketTime = 0;  // Track the last time a packet was received
int receivedBytes = 0;

size_t data_size = sizeof(data) / sizeof(data[0]); // Calculate the size


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
  
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("Connecting ... ");

    while (WiFi.status() != WL_CONNECTED ) {
      // set_color(data,"blue",2,0xFF) ;
      memset(data, 0xFF, 24);        // Fill the data array with 0s
      memset(data, 0x00, 9);        // Fill the data array with 0s
      pixel_update(data, TOTAL_SIZE);

      delay(250);
      Serial.print(".");

      memset(data, 0, TOTAL_SIZE);  // Fill the data array with 0s
      pixel_update(data, TOTAL_SIZE);
      delay(250);

    }

    Serial.println("Connected to WiFi");
    udp.begin(port);
    Serial.println("Starting data transmission...");

}
// get a data in 12 or 9 bit format and convert it to 24 
void convevrt_to_24bit(uint8_t* data, size_t size, uint8_t* data24 , uint8_t from = 12){
    memset(data24, 0, TOTAL_SIZE);
    if(from == 12){
        for(int i=0;i<size / from;i++){
          data24[i * 24 + 0] =  data[i * 12 + 0];
          data24[i * 24 + 2] =  data[i * 12 + 1];
          data24[i * 24 + 4] =  data[i * 12 + 2];
          data24[i * 24 + 6] =  data[i * 12 + 3];
          data24[i * 24 + 8] =  data[i * 12 + 4];
          data24[i * 24 + 10] = data[i * 12 + 5];
          data24[i * 24 + 12] = data[i * 12 + 6];
          data24[i * 24 + 14] = data[i * 12 + 7];
          data24[i * 24 + 16] = data[i * 12 + 8];
          data24[i * 24 + 18] = data[i * 12 + 9];
          data24[i * 24 + 20] = data[i * 12 + 10];
          data24[i * 24 + 22] = data[i * 12 + 11];
        }
    }
    if (from == 9) {
        for (int i = 0; i < size / 9; i++) {
            data24[i*24]= 0x00;

            data24[i * 24+  1 ] = data[i * 9 + 0];
            data24[i * 24 + 4 ] = data[i * 9 + 1];
            data24[i * 24 + 7 ] = data[i * 9 + 2];
            data24[i * 24 + 9 ] = data[i * 9 + 3];
            data24[i * 24 + 12] = data[i * 9 + 4];
            data24[i * 24 + 15] = data[i * 9 + 5];
            data24[i * 24 + 17] = data[i * 9 + 6];
            data24[i * 24 + 21] = data[i * 9 + 7];
            data24[i * 24 + 23] = data[i * 9 + 8];

        }
    }
    
    return;
}
void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
      // Update the last packet time
      lastPacketTime = millis();

      udp.read(buffer, packetSize);

      if (buffer[0] == 0xAA && packetSize == 1 ){
          receivedBytes = 0;
          start_time = micros();
          return;
      }
      if (buffer[0] == 0xBB && packetSize == 1 ){
          end_time = micros();
          Serial.printf("WIFI > data size: %lu time: %lu us FPS: %lu\n", receivedBytes, end_time - start_time,1000000/(end_time - start_time));

          // // you need this delay to reset data if data come faster than 50us
          // delay(0.05); 
          // add a moving avrege i if receve data shrter than avrege dope data and notity me with a blink in strip 
          convevrt_to_24bit(data_buffer, receivedBytes,data,12);

          pixel_update(data, receivedBytes*2);

          end_time = micros();
          Serial.printf("%c%c%c  > frame time: %lu time: %lu us FPS: %lu \n\n",(receivedBytes/100)%93+33,(receivedBytes/10)%93+33,(receivedBytes/5+128)%93+33
                        ,receivedBytes/12, end_time - start_time,1000000/(end_time - start_time));
          return;
      }

      if ((receivedBytes + packetSize)> TOTAL_SIZE) receivedBytes = 0;

      // Store data in correct position
      memcpy(&data_buffer[receivedBytes], buffer , packetSize );
      receivedBytes += packetSize;
      // if (receivedBytes == 30000){
      //     end_time = micros();
      //     Serial.printf("Transfer completed in %lu us\n", end_time - start_time);
      //     // pixel_update(data, receivedBytes);
      //     return;
      // }

      // remote command to reset
      if (buffer[0] == 0x10 && buffer[1] == 0xFF && packetSize == 2) {
          // Reset ESP
          ESP.restart(); // Replace this with your actual ESP reset function
          return;
      }
      // remote command to turn off all pixels
      if (buffer[0] == 0x0B && buffer[1] == 0x1C && packetSize == 2) {
          memset(data, 0, TOTAL_SIZE);  // Fill the data array with 0s
          pixel_update(data, TOTAL_SIZE);  // Update pixels with all black
          Serial.println("command: Sent all black.");
          return;
      }
  }

  
  // Check if no packet has been received for 10 seconds
  if (millis() - lastPacketTime > 5000) {  
    // Send all black (0) as the last packet size
    memset(data, 0, TOTAL_SIZE);  // Fill the data array with 0s
    pixel_update(data, TOTAL_SIZE);  // Update pixels with all black
    Serial.println("No packet received for  sseconds. Sent all black.");

    // Reset the last packet time to avoid repeated triggers
    lastPacketTime = millis();
  }
}
