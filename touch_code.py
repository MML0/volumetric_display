# me - this DAT
# 
# channel - the Channel object which has changed
# sampleIndex - the index of the changed sample
# val - the numeric value of the changed sample
# prev - the previous sample value
# 
# Make sure the corresponding toggle is enabled in the CHOP Execute DAT.


import socket
import time
import random
import select
ESP_IP = "192.168.43.222"  # Replace with ESP's IP
ESP_PORT = 8266
CHUNK_SIZE = 1400  
TOTAL_SIZE =   12* int (op('data_cons1')['led_count'])


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
    time.sleep(0.0001)  # Small delay for ESP to process
    
    for i in range(0, len(data), chunk_size):
        chunk = data[i:i+chunk_size]  # Extract chunk

        # ready = select.select([], [sock], [], 0.0001)  
        sock.sendto(chunk, (ESP_IP, ESP_PORT))  # Send chunk
        time.sleep(0.001)  # Small delay for ESP to process
    time.sleep(0.001)  # Small delay for ESP to process
    sock.sendto(b"\xBB", (ESP_IP, ESP_PORT))  # End byte
    # print("Data transfer completed.")
#  Bits 1-8: Green (MSB first)
#  Bits 9-16: Red (MSB first)
#  Bits 17-24: Blue (MSB first)
j=0

def onOffToOn(channel, sampleIndex, val, prev):
    global j
    data = bytearray(TOTAL_SIZE)
    for i in range(0,len(data)):
        data[i] = 0x00 #0b00000000
    j+=5
    j= j%400
    for i in range(0,len(data),12):
        # data[i] = random.randint(0, 255)
        if i//12 == j//1:
            data[i]  = 0b10001001
            data[i+1]  = 0b00000010
            data[i+4]  = 0b00011001
            data[i+5] = 0b11000010
            data[i+8] = 0b00010011
            data[i+9] = 0b10100101
            

    start_time = time.time()
    send_data(data, ESP_IP, ESP_PORT, CHUNK_SIZE)
    end_time = time.time()
    # sleep_fps(10)
    #time.sleep(0.01)

    end_time2 = time.time()
    print(f"Sent {len(data)} bytes in {end_time - start_time:.6f} seconds FPS: {1/(end_time - start_time):6.1f} - {1/(end_time2 - start_time):3.1f}")

    return

def whileOn(channel, sampleIndex, val, prev):
    return

def onOnToOff(channel, sampleIndex, val, prev):
    reds  =  [[] for _ in range(8)]
    greens = [[] for _ in range(8)]
    blues  = [[] for _ in range(8)]
    data = bytearray(TOTAL_SIZE)
    for i in range(1, 9):  # Pins are numbered from 1 to 8
        pin_name = f'pin{i}'  # Construct the pin name dynamically (e.g., 'pin1', 'pin2', etc.)
        colors = (str(op(pin_name)[0, 0])).split(',')  # Get the color data as a list
        # Separate into reds, greens, and blues
        reds[i-1] = [int(c) for c in colors[0::3]]   # Convert every 3rd element starting from index 0 to int
        greens[i-1] = [int(c) for c in colors[1::3]] # Convert every 3rd element starting from index 1 to int
        blues[i-1] = [int(c) for c in colors[2::3]]
        print(reds[0][0:10])
        # print(greens[0][0:10])
        # print(blues[0][0:10])
    # print(max(reds[0])//16)
    print(bin(reds[0][0] // 16))
    for i in range(0,len(greens[0]),12):
        for j in range(4):
            for k in range(8):
                data[i+j+0]  |= (1 if  ((1 << (3 - j)) & (greens[k][i] // 16)) else 0) << k
                data[i+j+4]  |= (1 if  ((1 << (3 - j)) & (reds[k][i] //   16)) else 0) << k
                data[i+j+8]  |= (1 if  ((1 << (3 - j)) & (blues[k][i] // 16 )) else 0) << k

    print(data[0:24])
        # data[i+0]  = 0b10001001
        # data[i+1]  = 0b00000010
        # data[i+2]  = 0b00000010
        # data[i+3]  = 0b00000010
        # data[i+4]  = 0b00011001
        # data[i+5]  = 0b11000010
        # data[i+6]  = 0b11000010
        # data[i+7]  = 0b00010011
        # data[i+8]  = 0b00010011
        # data[i+9]  = 0b10100101
        # data[i+10] = 0b10100101
        # data[i+11] = 0b10100101

    start_time = time.time()
    send_data(data, ESP_IP, ESP_PORT, CHUNK_SIZE)
    end_time = time.time()
    # sleep_fps(10)
    #time.sleep(0.01)

    end_time2 = time.time()
    print(f"Sent {len(data)} bytes in {end_time - start_time:.6f} seconds FPS: {1/(end_time - start_time):6.1f} - {1/(end_time2 - start_time):3.1f}")

    return

def whileOff(channel, sampleIndex, val, prev):
    return

def onValueChange(channel, sampleIndex, val, prev):
    return
    