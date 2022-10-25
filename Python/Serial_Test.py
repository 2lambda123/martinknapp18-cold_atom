#imports
import numpy as np
import serial
import time
import pandas as pd
import re
import os

import serial.tools.list_ports

import plotting


os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Python_Test/Data')
cwd = os.getcwd()
date = '13.10.22'

ports = serial.tools.list_ports.comports()
portList = []
for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))
print("")


BAUD = 9600
ser = serial.Serial('COM4', BAUD, timeout=(0))
print("Enter your commands below.\r\nInsert 'exit' to leave the application.\r\nInsert 'init' to initialize the board.")

functions = [['MOT_TEMP',0],
          ['MOT_CYCLE',0],
          ['DROP_TEST',0],
          ['DROP_CYCLE',0],
          ['DIAGNOSTIC',1],
          ['EXPERIMENTAL',1]]

MCU_FUNC = pd.DataFrame(data=functions, columns=['FUNCTION','DATA'])

def switch(action):
    
    value = '\0'
    
    if action == "write":
        command_ = input(">> ")
        write_buffer_ = command_ + '\0'
        ser.write( write_buffer_.encode('utf-8') )
        # time.sleep(0.5)
        value = command_
    
    elif action == "read":
        
        read_buffer_ = ""
        data = []
        while ( read_buffer_[-4:] != "DONE" ):
            out = ser.read(1).decode('utf-8')
            read_buffer_ += out
            
            # if ( read_buffer_[-2:] == "\n\r" ):
            #     print (">> " + read_buffer_)
            #     read_buffer_ = ""
                
            
        print (">> " + read_buffer_)
        
        # if there is data to read, read it
        if 'DATA' in read_buffer_:
            # isolate just the part that contains the data
            read_buffer_ = read_buffer_[read_buffer_.find('DATA')+7 : -4]
            # print (read_buffer_)
            
            # find the number of rows
            # rows = len(re.findall('\n', read_buffer_))
            # n = int(len(read_buffer_)/rows)
            
            # remove the \n and \r chracters
            read_buffer_ = read_buffer_.replace('\n', '')
            read_buffer_ = read_buffer_.replace('\r', '')
            
            # convert the data to array and then dataframe
            data = read_buffer_.split(',')
            data = data[:-1]
            df = pd.DataFrame(data)

            # save the data to txt file
            filename = input(">> Please add filename: ")
            df.to_csv(''+cwd+'/'+filename+'_serial.txt', header=None, index=None, sep='\t')

            # Do you want to plot?
            plot = input(">> Plot? (y/n): ")
            if (plot == 'y'):
                plotting.data_plot(filename)
                

            
    elif action == "exit":
        ser.close()
    
    return value
    


while (1):
    ser.flushInput()
    ser.flushOutput()
        
    command = switch("write")
    if (command == "exit"):
        switch("exit")
        break
            
    switch("read")
    