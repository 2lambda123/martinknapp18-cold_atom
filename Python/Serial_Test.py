#imports
import numpy as np
import serial
import time
import pandas as pd
import re
import os

import serial.tools.list_ports

import plotting

date = '15.02.23'
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Detection/2023/'+date+'')
cwd = os.getcwd()
print (cwd)

ports = serial.tools.list_ports.comports()
portList = []
for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))
print("")


ser = serial.Serial('COM7', baudrate=921600, timeout=(0))
print("Enter your commands below.\r\nInsert 'exit' to leave the application.\r\nInsert 'init' to initialize the board.")
# settings = ser.get_settings()
# print(ser.baudrate)

def switch(action):
    
    value = '\0'
    
    if action == "write":
        command_ = input(">> ")
        write_buffer_ = command_ + '\0'
        ser.write( write_buffer_.encode('utf-8') )
        # time.sleep(0.5)
        value = command_
    
    elif action == "read":
        
        # # printing to terminal on the fly
        # read_buffer_done_ = ""
        # read_buffer_ = ""
        # while ( read_buffer_done_[-4:] != "DONE" ):  
        #     out = ser.read(1).decode('utf-8')
        #     read_buffer_ += out
        #     read_buffer_done_ += out
            
        #     if ( read_buffer_[-2:] == "\n\r" ):
        #         print (">> " + read_buffer_)
        #         read_buffer_ = ""
                

        # printing to terminal after function has finished
        read_buffer_ = ""
        data = []
        while ( read_buffer_[-4:] != "DONE" ):
            out = ser.read(1).decode('utf-8')
            read_buffer_ += out
    
        print (">> " + read_buffer_)
        
        # if there is data to read, read it
        if 'DATA' in read_buffer_:
            # isolate just the part that contains the data
            read_buffer_ = read_buffer_[read_buffer_.find('DATA')+7 :]
            # print (read_buffer_)
            
            # find the number of rows
            # rows = len(re.findall('\n', read_buffer_))
            # n = int(len(read_buffer_)/rows)
            
            # remove the \n and \r chracters
            read_buffer_ = read_buffer_.replace('DATA', '')
            read_buffer_ = read_buffer_.replace('\n', '')
            read_buffer_ = read_buffer_.replace('\r', '')
            
            # convert the data to array and then dataframe
            data = read_buffer_.split(',')
            data = data[:-1]
            # print (data)
            df = pd.DataFrame(data)

            # # save the data to txt file
            # filename = input(">> Please add filename: ")
            # df.to_csv(''+cwd+'/'+filename+'.txt', header=None, index=None, sep='\t')
            # # plotting.data_plot(filename)
            
            # # Do you want to plot?
            # plot = input(">> Plot? (Y/N): ")
            # if (plot == 'Y'):
            #     plotting.data_plot_file(str(cwd), filename)
            
            # Just plot
            plotting.data_plot(df)
            
            # # save the data to txt file
            # N_shots = 5 # no. of shots per variable
            # N_variable_X = 27 # total number of variables
            # N_total = N_shots * N_variable_X
            # for i in range(N_total):
            #     start = (127 + 2) * (i)
            #     stop = (127 + 2) * (i+1)
            #     df = pd.DataFrame(data[start:stop - 2])
            #     variable_X_index = data[stop-2]
            #     shot_index = data[stop-1]
            #     # print (df)
            #     # print (variable_X_index)
            #     # print (shot_index)
                
            #     filename = ''+str(variable_X_index)+'_'+str(shot_index)+''
            #     variable = 'PGC_TRAP_FREQ'
            #     path = ''+cwd+'/'+variable+'/'+filename+'.txt'
            #     df.to_csv(''+path+'', header=None, index=None, mode='w', sep='\t')
            #     # plotting.data_plot( str(path), filename)
                

            
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
    