import socket
import time
import random

ESP_IP = "192.168.43.222"  # Replace with ESP's IP
ESP_PORT = 8266
CHUNK_SIZE = 5400  
TOTAL_SIZE = 24000

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

while 1:
    data = bytearray(TOTAL_SIZE)
    for i in range(len(data)):
        data[i] = random.randint(0, 255)

    start_time = time.time()
    send_data(data, ESP_IP, ESP_PORT, CHUNK_SIZE)
    end_time = time.time()

    print(f"Sent {len(data)} bytes in {end_time - start_time:.6f} seconds")
    time.sleep(0.03)  # Small delay for ESP to process

sock.close()  # Close socket
