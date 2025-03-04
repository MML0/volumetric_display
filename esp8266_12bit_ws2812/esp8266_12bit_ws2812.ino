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


const int CHUNK_SIZE = 8000;  
const int TOTAL_SIZE = 30 * 1024;
uint8_t data[TOTAL_SIZE];
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
      set_color(data,"blue",2,0xFF) ;
      memset(data, 0xFF, 24);        // Fill the data array with 0s
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
          pixel_update(data, receivedBytes);

          end_time = micros();
          Serial.printf("%c%c%c > data size: %lu time : %lu us FPS: %lu \n\n",(receivedBytes/100)%255,(receivedBytes/10)%255,(receivedBytes/10+128)%255
                        ,receivedBytes, end_time - start_time,1000000/(end_time - start_time));
          return;
      }

      if ((receivedBytes + packetSize)> TOTAL_SIZE) receivedBytes = 0;

      // Store data in correct position
      memcpy(&data[receivedBytes], buffer , packetSize );
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
