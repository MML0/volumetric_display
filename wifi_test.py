import socket
import time
import struct

ESP_IP = "192.168.43.222"  # Replace with ESP's IP
ESP_PORT = 8266
CHUNK_SIZE = 4096 - 2  # Reserve 2 bytes for the index
TOTAL_SIZE = 36 * 1024 +5  # 36 KB

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
data = bytearray(TOTAL_SIZE)  # Dummy data
start_time = time.time()

for i in range(0, TOTAL_SIZE, CHUNK_SIZE):
    chunk_index = i // CHUNK_SIZE
    header = struct.pack(">H", chunk_index)  # 2-byte big-endian index
    chunk = header + data[i:i+CHUNK_SIZE]
    sock.sendto(chunk, (ESP_IP, ESP_PORT))

end_time = time.time()
print(f"Transfer completed in {end_time - start_time:.2f} seconds")
