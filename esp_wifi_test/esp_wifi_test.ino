#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
// #include <WiFi.h>  // Use <ESP8266WiFi.h> for ESP8266
// #include <SPI.h>

#define LATCH_PIN D2  // Pin connected to the latch pin of 74HC595
#define DATA_PIN D7   // SPI MOSI pin
#define CLOCK_PIN D5  // SPI SCK pin
#define NUM_LEDS 1024

// uint8_t Data[NUM_LEDS * 3 * 8];


const char* ssid = "net emam limited edition";
const char* password = "12345678an";
const IPAddress staticIP(192, 168, 43, 222);
const IPAddress gateway(192, 168, 43, 1);
const IPAddress subnet(255, 255, 255, 0);

unsigned int port = 8266;
WiFiUDP udp;


// const int port = 12345;
const int CHUNK_SIZE = 4096 - 2;  // Remove 2 bytes for index
const int TOTAL_SIZE = 36 * 1024;
uint8_t data[TOTAL_SIZE];
uint8_t buffer[CHUNK_SIZE];  // Declare globally
bool receivedChunks[64] = {false};  // Track received chunks
unsigned long start_time, end_time;
bool first_packet = true;

void setup() {
    Serial.begin(115200);
  
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);
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
        // uint8_t buffer[CHUNK_SIZE];
        udp.read(buffer, packetSize);

        // Extract chunk index
        uint16_t chunkIndex = (buffer[0] << 8) | buffer[1];
        // if (chunkIndex >= 256 || receivedChunks[chunkIndex]) return;  // Ignore invalid/duplicate chunks

        if (first_packet) {
            start_time = millis();
            first_packet = false;
        }

        // Store data in correct position
        memcpy(&data[chunkIndex * CHUNK_SIZE], buffer + 2, packetSize - 2);
        receivedChunks[chunkIndex] = true;

        // Check if all chunks arrived
        bool allReceived = true;
        for (int i = 0; i < (TOTAL_SIZE / CHUNK_SIZE); i++) {
            if (!receivedChunks[i]) {
                allReceived = false;
                break;
            }
        }

        if (allReceived) {
            end_time = millis();
            Serial.printf("Transfer completed in %lu ms\n", end_time - start_time);
        }
    }
}
