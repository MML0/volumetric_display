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
int receivedBytes = 0;

size_t data_size = sizeof(data) / sizeof(data[0]); // Calculate the size


void setup() {
    Serial.begin(921600);
  
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("Connecting ... ");

    while (WiFi.status() != WL_CONNECTED ) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("Connected to WiFi");
    udp.begin(port);


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
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
      udp.read(buffer, packetSize);

      if (buffer[0] == 0xAA && packetSize == 1 ){
          receivedBytes = 0;
          start_time = micros();
          return;
      }
      if (buffer[0] == 0xBB && packetSize == 1 ){
          end_time = micros();
          Serial.printf("Transfer completed in %lu us\n", end_time - start_time);
          // data_size = sizeof(data) / sizeof(data[0]);
          delay(0.05);
          pixel_update(data, receivedBytes);

          return;
      }

      if ((receivedBytes + packetSize)> TOTAL_SIZE) receivedBytes = 0;

      // Store data in correct position
      memcpy(&data[receivedBytes], buffer , packetSize );
      receivedBytes += packetSize;
      if (receivedBytes == 30000){
          end_time = micros();
          Serial.printf("Transfer completed in %lu us\n", end_time - start_time);
          return;
      }

  }
}
