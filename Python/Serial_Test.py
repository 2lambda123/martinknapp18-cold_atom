#imports
import numpy as np
import serial
import csv
import time

import serial.tools.list_ports


ports = serial.tools.list_ports.comports()
portList = []
for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))
print("")


ser = serial.Serial('COM4', 9600)
buffer_size = 32
print("Enter your commands below.\r\nInsert 'exit' to leave the application.\r\nInsert 'init' to initialize the board.")

while 1:
    # get keyboard input
    ser.flushInput()
    ser.flushOutput()
    write_buffer = input(">> ")
    

    if write_buffer == 'exit':
        ser.close()
        break

    else:
        write_buffer += '\0'
        ser.write( write_buffer.encode('utf-8') )
        read_buffer = "" 
        time.sleep(0.25)
        
        while (ser.inWaiting() != 0):
            out = ser.read(1).decode('utf-8')
            read_buffer += out
     			
        print (">> " + read_buffer)