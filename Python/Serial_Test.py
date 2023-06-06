#imports
import numpy as np
import serial
import time as t
import pandas as pd
import re
import os
import datetime
import allantools

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

# print current comport connections
ports = serial.tools.list_ports.comports()
portList = []
for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))
print("")

# open a serial communication
ser = serial.Serial('COM9', baudrate=921600, timeout=(0))
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
        data_read = 0
        shot_number = 0
        while (read_done_.find('DONE') == -1 ):

            out = ser.read(1).decode('utf-8')
            read_buffer_ += out
            read_done_ += out
            
            # save the data from each individual shot
            if 'SHOT\r\n' in read_buffer_:
                data_read = 1
                # print (">> " + read_buffer_)
                
                # extract all data enclosed between ()
                substrings = re.findall(r'\(.*?\)', read_buffer_)
                # print (substrings)
                
                # delete the () and empty space characters
                for i in range(len(substrings)):
                    substrings[i] = substrings[i][1:-2]
                    # print (substrings[i])
                
                
                # seperate array data using delimiters, then convert to numeric
                for i in range(len(substrings)):
                    if (substrings[i].find(',') != -1):
                        substrings[i] = substrings[i].split(',')
                        substrings[i] = [eval(i) for i in substrings[i]]
                    # print (substrings[i])
                    
                
                # now convert everything to numeric values apart form the arrays of data
                for i in range(len(substrings)):
                    if isinstance(substrings[i], str):
                        if ((substrings[i].find('.') or substrings[i].find('-')) != -1):
                            substrings[i] = float(substrings[i])
                    # print (substrings[i])
                    
                    
                # prepare data into large array for saving data
                test = []
                for i in range(len(substrings)):
                    if isinstance(substrings[i], list):
                        for j in range(len(substrings[i])):
                            test.append(substrings[i][j])
                    else:  
                        test.append(substrings[i])
                    
                    # print (test)
                    # print ("")

                    
                # insert data into row 0 of dataframe
                df_shot = pd.DataFrame(data=test)
                # print (df_shot)

                # concat shot dataframe to larger dataframe
                df = pd.concat([df,df_shot],axis=1)
                df.columns = [i for i in range(df.shape[1])]
                # print (df)


                # Save and plot ADC data
                os.makedirs(''+cwd+'/'+time+'', exist_ok=True)
                df.to_csv(''+cwd+'/'+time+'/shot.txt', header=True, index=True, sep='\t')
                plotting.data_plot(df_shot, shot_number)
                
                read_buffer_ = ""
                shot_number += 1
                
                
        print (">> " + read_buffer_)
        
        
        # collate the data from all shots
        if (data_read == 1):

            # print key values and save into arrays for file save        
            print ("")
            data_detection_0 = []
            data_detection_1 = []
            data_detection_2 = []
            data_atom_number = []
            data_fraction = []
            for i in range(len(df.columns)):
                # save to arrays
                data_detection_0.append(df.loc[1,i])
                data_detection_1.append(df.loc[2,i])
                data_detection_2.append(df.loc[3,i])
                data_atom_number.append(df.loc[0,i])
                data_fraction.append(df.loc[4,i])
            
            # save the key data from all shots in one file
            df = pd.DataFrame()
            df['N4'] = np.array(data_detection_0, int)
            df['N34'] = np.array(data_detection_1, int)
            df['BG4'] = np.array(data_detection_2, int)
            df['atom_number'] = np.array(data_atom_number,float)
            df['fraction'] = data_fraction
            # print (df)
            
            
            # just check after everything if there was extra data
            RABI = 0
            if 'RABI\r\n' in read_buffer_:
                RABI = 1
                # extract all data enclosed between ()
                substrings = re.findall(r'\(.*?\)', read_buffer_)
                # print (substrings)
                
                # extract all data enclosed between ()
                substrings = re.findall(r'\(.*?\)', read_buffer_)
                # print (substrings)
                    
                # delete the () and empty space characters
                for i in range(len(substrings)):
                    substrings[i] = substrings[i][1:-2]
                    # print (substrings[i])


                # seperate array data using delimiters, then convert to numeric
                for i in range(len(substrings)):
                    if (substrings[i].find(',') != -1):
                        substrings[i] = substrings[i].split(',')
                        substrings[i] = [eval(i) for i in substrings[i]]
                    # print (substrings[i])


                # now convert everything to numeric values apart form the arrays of data
                for i in range(len(substrings)):
                    if isinstance(substrings[i], str):
                        if ((substrings[i].find('.') or substrings[i].find('-')) != -1):
                            substrings[i] = float(substrings[i])
                    
                    
                # prepare data into large array for saving data
                test = []
                for i in range(len(substrings)):
                    if isinstance(substrings[i], list):
                        for j in range(len(substrings[i])):
                            test.append(substrings[i][j])
                    else:  
                        test.append(substrings[i])
                        
                print (test)
                df['u_WAVE_FREQ'] = test
                
            print (df)
                
            # calculate the SNR
            S = np.average(data_fraction[10:])
            N = np.std(data_fraction[10:])
            N_ADEV = allantools.adev(data_fraction[10:], rate=1, data_type="freq", taus=1)[1]
            SNR = S/N
            print ("")
            print ("S = %.5f" %(S))
            print ("N = %.5f" %(N))
            print ("ADEV(tau=1s) = %.5f" %(N_ADEV))
            print ("SNR = %u" %(SNR))
            print ("SNR_ADEV = %u" %(S/N_ADEV))
            print ("ATOM_NUMBER = %d Vs" %(np.average(df['atom_number'])))
            print ("")
            
            # plot the RABI data if it's there
            if (RABI == 1):
                plotting.data_plot_RABI(df['u_WAVE_FREQ'], df['fraction'],1)
                
            
            # save to file
            plotting_ = input(">> Save?: ")
            if (plotting_ == 'Y'):
                
                folder_ = input(">> Provide Folder: ")
                filename_ = input(">> Provide Filename: ")
                df.to_csv(''+cwd+'/SNR/'+folder_+'/'+filename_+'.txt', index=True, sep='\t')
                
    elif action == "CLOCK_OPERATION":
        print ("")
        # print the the error and the control voltage
        # then plot them both against time
        # this should tell me whether it's locking or not
        
            
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
        
        elif (command == "read"):
                switch("read")
        
        elif (command == "CLOCK_OPERATION"):
            switch("CLOCK_OPERATION")

                    
        # switch("read")

finally:
    ser.close()
    