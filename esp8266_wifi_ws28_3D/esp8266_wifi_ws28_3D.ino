#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>

#define LATCH_PIN D2  // Pin connected to the latch pin of 74HC595
#define DATA_PIN D7   // SPI MOSI pin
#define CLOCK_PIN D5  // SPI SCK pin
#define NUM_LEDS 1024

uint8_t Data[NUM_LEDS * 3 * 8];


const char* ssid = "net emam limited edition";
const char* password = "12345678an";
const IPAddress staticIP(192, 168, 43, 222);
const IPAddress gateway(192, 168, 43, 1);
const IPAddress subnet(255, 255, 255, 0);

unsigned int localPort = 8266;
WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);

  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 50) {
    delay(500);
    Serial.print(".");
    // wifiAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("ESP IP Address: ");
    Serial.println(WiFi.localIP());
    udp.begin(localPort);
    Serial.printf("Listening for UDP packets on port %d\n", localPort);
  } else {
    Serial.println("\nWiFi connection failed!");
  }

  SPI.begin();
  // frequency 20 MHz
  uint32_t frequency = 20000000;
  uint32_t clockDivider = ESP8266_CLOCK / frequency;
  SPI1CLK = (clockDivider & 0xFFFFF); // Set the SPI clock divider

  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, LOW);
}


void preciseDelayMicroseconds(uint16_t us) {
  for (volatile uint16_t i = 0; i < us * 10; i++) { 
    __asm__ __volatile__("nop");  // 1 NOP ~ 100ns (assuming 80MHz CPU)
  }
}


void sendToShiftRegister(uint8_t *data, size_t length) {
  noInterrupts(); // Disable interrupts for precise timing

  // Use direct SPI register access
  for (size_t i = 0; i < length; i++) {
    while (SPI1CMD & SPIBUSY) {} // Wait for the SPI bus to be ready
    // Directly manipulate the LATCH_PIN (D2) to set it LOW
    GPOC = (1 << D2); // Clear the pin (set LOW)
    
    SPI1W0 = data[i];            // Load data into the SPI data register
    SPI1CMD |= SPIBUSY;          // Start the SPI transfer
    
    preciseDelayMicroseconds(10);
    GPOS = (1 << D2); // Set the pin (set HIGH)

    // while (SPI1CMD & SPIBUSY) {} // Wait for the transfer to complete
  }

  // Directly manipulate the LATCH_PIN (D2) to set it HIGH

  interrupts(); // Re-enable interrupts
}

void loop() {
  // int packetSize = udp.parsePacket();
  // if (packetSize) {
  //   uint8_t data[8]; // Adjust size as needed
  //   udp.read(data, sizeof(data));
  //   sendToShiftRegister(data, sizeof(data));
  // }


  int packetSize = udp.parsePacket();
  if (packetSize) {
    //lastDataTime = millis(); // Reset timeout timer
    
    byte startByte = udp.read();
    
    // Check for sync byte (0xAA)
    if (startByte == 0xAA) {   
      udp.read(Data, NUM_LEDS * 3*8);  // Read RGB data
      sendToShiftRegister(Data, sizeof(Data));
    }
  }
}