import serial
import time
import datetime

f = open("log.txt", "a")
Arduino = serial.Serial("COM10", 9600)
time.sleep(2)

while True:
    data = Arduino.readline()
    print(str(datetime.datetime.now()) + ", " + str(data))
    f.write(str(datetime.datetime.now()) + ", " + str(data))

f.close()
