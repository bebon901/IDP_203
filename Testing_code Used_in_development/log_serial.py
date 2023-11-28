port = "/dev/tty6"
import serial
import time
import datetime

f = open("log.txt", "a")
Arduino = serial.Serial(port, 9600)
time.sleep(2)

while True:
    data = Arduino.readline()
    print(str(datetime.datetime.now()) + ", " + str(data))
    f.write(str(datetime.datetime.now()) + ", " + str(data) + "\n")

f.close()
