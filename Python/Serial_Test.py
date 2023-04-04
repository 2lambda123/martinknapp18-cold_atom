#imports
import numpy as np
import serial
import time as t
import pandas as pd
import re
import os
import datetime

import serial.tools.list_ports

import plotting

# get the date, create folder for saving
date = str(datetime.date.today())
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Detection/2023')
cwd = os.getcwd()
os.makedirs(''+cwd+'/'+date+'', exist_ok=True) 
# print(date)

# now change directory to current date
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Detection/2023/'+date+'')
cwd = os.getcwd()
# print (cwd)

# # print current comport connections
# ports = serial.tools.list_ports.comports()
# portList = []
# for onePort in ports:
#     portList.append(str(onePort))
#     print(str(onePort))
# print("")

# open a serial communication
ser = serial.Serial('COM7', baudrate=921600, timeout=(0))
print("Enter your commands below.\r\nInsert 'exit' to leave the application.")
# settings = ser.get_settings()
# print(ser.baudrate)

# case handler for main code
def switch(action):
    
    value = '\0'
    
    if action == "write":
        command_ = input(">> ")
        write_buffer_ = command_ + '\0'
        ser.write(write_buffer_.encode('utf-8'))
        # time.sleep(0.5)
        value = command_
    
    elif action == "read":
        # get the current timestamp HOURS-MINS for data saving
        now = datetime.datetime.now()
        time = now.strftime("%H%M")
        
        # read_buffer_ = ""
        # data = []
        # while (ser.in_waiting == 0):
        #     pass
        # while (ser.in_waiting != 0):
        #     out = ser.read(1).decode('utf-8')
        #     read_buffer_ += out
        
        # print (">> " + read_buffer_)
                
        
        # printing to terminal on the fly
        read_done_ = ""
        read_buffer_ = ""
        df = pd.DataFrame()
        while (read_done_.find('DONE') == -1 ):

            out = ser.read(1).decode('utf-8')
            read_buffer_ += out
            read_done_ += out
            
            # if SHOT is found, print/plot/save data
            if 'SHOT\r\n' in read_buffer_:
                print (">> " + read_buffer_)
                
                # extract all data enclosed between ( )
                substrings = re.findall(r'\(.*?\)', read_buffer_)
                # print (substrings)
                
                # delete the () and empty space characters
                for i in range(len(substrings)):
                    substrings[i] = substrings[i][1:-2]
                    # print (type(substrings[i]))
                # print (substrings)
                
                # convert from string to numerics
                substrings[0] = int(substrings[0])
                substrings[2] = float(substrings[2])
                substrings[3] = int(substrings[3])
                
                # seperate data using delimiters, then convert to numeric
                substrings[1] = substrings[1].split(',')
                substrings[-1] = substrings[-1].split(',')
                substrings[1] = np.asarray([eval(i) for i in substrings[1]])
                substrings[-1] = np.asarray([eval(i) for i in substrings[-1]])
                # print (substrings)
                # data = np.asarray(substrings[1])
                # print (type(substrings[1]))
                # print (data)
                # print (data.dtype)
                # print (type(data))
                
                # insert data into row 0 of dataframe
                df_shot = pd.DataFrame(data=substrings, index=['Atom Number', 'Detection', 'Fraction', 'Samples', 'ADC'])

                # concat shot dataframe to larger dataframe
                df = pd.concat([df,df_shot],axis=1)
                # print (df)
                # print (df_shot)
                
                # Save and plot ADC data
                os.makedirs(''+cwd+'/'+time+'', exist_ok=True)  
                df.to_csv(''+cwd+'/'+time+'/shot.txt', header=None, index=None, sep='\t')
                plotting.data_plot(df_shot)
                
                read_buffer_ = ""
                
        print (">> " + read_buffer_)
        
        # rename the columns with incrementing index, denotes shot number
        new_columns = np.arange(0,len(df.columns),1)
        df.columns = new_columns
        # print (df)

        # print certain values         
        print ("")
        print ("DETECTION")
        for i in range(len(df.columns)):
            print (""+str(df.loc['Detection',i][0])+", \
"+str(df.loc['Detection',i][1])+", \
"+str(df.loc['Detection',i][2])+", \
"+str(df.loc['Detection',i][3])+"\
")
            
        print ("")
        print ("FRACTION")
        data_fraction = []
        shots = len(df.columns)
        for i in range(len(df.columns)):
            print (""+str(df.loc['Fraction',i])+"")
            data_fraction.append(df.loc['Fraction',i])
        
        
        S = np.average(data_fraction[int(shots*0.1):])
        N = np.std(data_fraction[int(shots*0.1):])
        SNR = S/N
        print ("S = %.5f" %(S))
        print ("N = %.5f" %(N))
        print ("SNR = %.2f" %(SNR))
        
            
    elif action == "exit":
        ser.close()
    
    return value
    

# the try statement allows serial connection to be closed in case of error
try:
    while (1):
        ser.flushInput()
        ser.flushOutput()
                
        command = switch("write")
        if (command == "exit"):
            switch("exit")
            break
                    
        switch("read")

finally:
    ser.close()
    