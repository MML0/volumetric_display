#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <WiFi.h>  // Use <ESP8266WiFi.h> for ESP8266
// #include <SPI.h>



const char* ssid = "net emam limited edition";
const char* password = "12345678an";
const IPAddress staticIP(192, 168, 43, 222);
const IPAddress gateway(192, 168, 43, 1);
const IPAddress subnet(255, 255, 255, 0);

unsigned int port = 8266;
WiFiUDP udp;


const int CHUNK_SIZE = 8500;  
const int TOTAL_SIZE = 36 * 1024;
uint8_t data[TOTAL_SIZE];
uint8_t buffer[CHUNK_SIZE];  // Declare globally
unsigned long start_time, end_time;
int receivedBytes = 0;

void predictableDelay(int iterations) {
    for (int i = 0; i < iterations; i++) {
        asm volatile("nop"); // 'volatile' ensures the asm is not optimized away
    }
}
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
            return;
        }

        if ((receivedBytes + packetSize)> TOTAL_SIZE) receivedBytes = 0;

        // Store data in correct position
        memcpy(&data[receivedBytes], buffer , packetSize );
        receivedBytes += packetSize;
        if (receivedBytes == 36000){
            end_time = micros();
            Serial.printf("Transfer completed in %lu us\n", end_time - start_time);
            return;
        }

    }
}
