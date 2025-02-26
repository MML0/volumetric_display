import socket
import time
import random

ESP_IP = "192.168.43.222"  # Replace with ESP's IP
ESP_PORT = 8266
CHUNK_SIZE = 5400  
TOTAL_SIZE = 3000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_data(data, ESP_IP, ESP_PORT, chunk_size=1400):
    """ Sends data in chunks via UDP to ESP8266. """
    sock.sendto(b"\xAA", (ESP_IP, ESP_PORT))  # Start byte
    time.sleep(0.0001)  # Small delay for ESP to process
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]  # Extract chunk
        # time.sleep(0.1 /1000)  # Small delay for ESP to process
        sock.sendto(chunk, (ESP_IP, ESP_PORT))  # Send chunk
    # time.sleep(0.0005)  # Small delay for ESP to process
    sock.sendto(b"\xBB", (ESP_IP, ESP_PORT))  # End byte
    print("Data transfer completed.")
#  Bits 1-8: Green (MSB first)
#  Bits 9-16: Red (MSB first)
#  Bits 17-24: Blue (MSB first)
j=0

while 1:
    data = bytearray(TOTAL_SIZE)
    for i in range(0,len(data)):
        data[i] = 0b00000010
    j= j%16
    j+=1
    for i in range(240,len(data),24):
        # data[i] = random.randint(0, 255)
        if i//24 == j//1:
            data[i]  =   0b10000010
            data[i+1]  = 0b10001001
            data[i+8]  = 0b01000010
            data[i+9]  = 0b00011000
            data[i+16] = 0b00100100
            data[i+17] = 0b00010011


    start_time = time.time()
    send_data(data, ESP_IP, ESP_PORT, CHUNK_SIZE)
    end_time = time.time()

    print(f"Sent {len(data)} bytes in {end_time - start_time:.6f} seconds")
    time.sleep(0.09)  # Small delay for ESP to process

sock.close()  # Close socket
