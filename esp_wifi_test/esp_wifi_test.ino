// #include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFi.h>  // Use <ESP8266WiFi.h> for ESP8266
// #include <SPI.h>



const char* ssid = "net emam limited edition";
const char* password = "12345678an";
const IPAddress staticIP(192, 168, 43, 222);
const IPAddress gateway(192, 168, 43, 1);
const IPAddress subnet(255, 255, 255, 0);

unsigned int port = 8266;
WiFiUDP udp;



uint8_t buffer[8192];  // Large enough buffer

unsigned long start_time_main = micros();

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
        unsigned long start_time = micros();
        udp.read(buffer, packetSize);
        unsigned long end_time = micros();
        Serial.printf("Received %d bytes in %lu µs   time stamp %d \n", packetSize, end_time - start_time, end_time - start_time_main);
    }
}
