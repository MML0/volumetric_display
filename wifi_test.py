import socket
import time
import random
import select
ESP_IP = "192.168.43.222"  # Replace with ESP's IP
ESP_PORT = 8266
CHUNK_SIZE = 5000  
TOTAL_SIZE =   24*500

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# sock.setblocking(False)
def sleep_fps(FPS:int=10):
    if not hasattr(sleep_fps, "last_call"):
        sleep_fps.last_call = time.time()  # Initialize on first call

    current_time = time.time()
    elapsed_time = current_time - sleep_fps.last_call
    target_time = 1 / FPS

    if elapsed_time < target_time:
        time.sleep(target_time - elapsed_time)
    else:
        print(f'!!! FPS drop to {1 / elapsed_time:.2f}')

    sleep_fps.last_call = time.time()  # Update last_call after sleeping

def send_data(data, ESP_IP, ESP_PORT, chunk_size=1400):
    """ Sends data in chunks via UDP to ESP8266. """

    # ready = select.select([], [sock], [], 2)  
    sock.sendto(b"\xAA", (ESP_IP, ESP_PORT))  # Start byte
    time.sleep(0.0005)  # Small delay for ESP to process

    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]  # Extract chunk

        ready = select.select([], [sock], [], 0.0001)  
        sock.sendto(chunk, (ESP_IP, ESP_PORT))  # Send chunk
        # time.sleep(0.001)  # Small delay for ESP to process

    sock.sendto(b"\xBB", (ESP_IP, ESP_PORT))  # End byte
    # print("Data transfer completed.")
#  Bits 1-8: Green (MSB first)
#  Bits 9-16: Red (MSB first)
#  Bits 17-24: Blue (MSB first)
j=0

while 1:
    data = bytearray(TOTAL_SIZE)
    for i in range(0,len(data)):
        data[i] = 0x00 #0b00000000
    j+=1 
    j= j%17
    for i in range(0,len(data),24):
        # data[i] = random.randint(0, 255)
        if i//24 == j//1:
            data[i+1]  = 0b00001001
            data[i+2]  = 0b10000010
            data[i+9]  = 0b00011000
            data[i+10] = 0b01000010
            data[i+17] = 0b00010011
            data[i+18] = 0b00100100


    start_time = time.time()
    send_data(data, ESP_IP, ESP_PORT, CHUNK_SIZE)
    end_time = time.time()
    sleep_fps(19)

    end_time2 = time.time()
    print(f"Sent {len(data)} bytes in {end_time - start_time:.6f} seconds FPS: {1/(end_time - start_time):6.1f} - {1/(end_time2 - start_time):3.1f}")
sock.close()  # Close socket
