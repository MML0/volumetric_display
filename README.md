# volumetric_display



## colected data esp 82666 d1 mini
esp 82666 can acsept fagmented data in udp packets so it recive up to 8 kb of data in one packet insted of MTU size 1479kb
we only have one free spi hardware and can't go higher than 25Mhz or so (we should test this)  


### wifi tests:
CHUNK_SIZE = 7000 Bytes 
TOTAL_SIZE = 14000
Transfer completed in 34367 us
Transfer completed in 28247 us
Transfer completed in 31622 us
Transfer completed in 28496 us
Transfer completed in 26461 us
Transfer completed in 18665 us


CHUNK_SIZE = 1400  
TOTAL_SIZE = 14000
Transfer completed in 15538 us
Transfer completed in 16990 us
Transfer completed in 18052 us
Transfer completed in 19104 us


CHUNK_SIZE = 1400  
TOTAL_SIZE = 36000
Transfer completed in 41900 us
Transfer completed in 43722 us
Transfer completed in 43661 us


CHUNK_SIZE = 8000  
TOTAL_SIZE = 36000
Transfer completed in 42344 us
Transfer completed in 44057 us
Transfer completed in 46687 us
Transfer completed in 42498 us


CHUNK_SIZE = 4000  
TOTAL_SIZE = 14000
Transfer completed in 28323 us
Transfer completed in 23490 us
Transfer completed in 25020 us
Transfer completed in 24100 us
Transfer completed in 24210 us
Transfer completed in 24434 us


## colected data esp 32 wroom32
esp 32 can recive fagmented packet too but need to be set manualy 

### tests:

CHUNK_SIZE = 1400  
TOTAL_SIZE = 36000
Transfer completed in 30352 us
Transfer completed in 31156 us
Transfer completed in 26861 us
Transfer completed in 21954 us
Transfer completed in 25907 us
Transfer completed in 13915 us
Transfer completed in 17596 us
Transfer completed in 27077 us


CHUNK_SIZE = 1400  
TOTAL_SIZE = 14000
Transfer completed in 11668 us
Transfer completed in 6754 us
Transfer completed in 9636 us
Transfer completed in 11619 us
Transfer completed in 6498 us
Transfer completed in 6365 us
