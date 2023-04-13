#imports
import numpy as np
import pandas as pd
import os

from scipy.optimize import curve_fit

import matplotlib.pyplot as plt 
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches
from matplotlib import gridspec

from ast import literal_eval


#--------- path definitons ---------
# date_ = input(">> Provide Date (YYYY-MM-DD): ")
# folder_ = input(">> Provide Folder: ")
date_ = "2023-04-13"
time_ = "1437"
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Detection/2023/'+date_+'/'+time_+'')
cwd = os.getcwd()
print (cwd)
filename_ = os.listdir(cwd)
# print (filename_)


#--------- constants ---------
V_REF = 10
RES = 4095


#--------- functions ---------
def ADC_CONVERSION(ADC_CODE_):
    return (ADC_CODE_/RES)*V_REF

def cold_atom(V_, t_):
    A = 4
    B = 0.005
    C = 0.2
    return (A * np.exp(-B*t_) + C) + V_

#--------- imports ---------

# import data
df = pd.read_csv(''+cwd+'/'+filename_[0]+'', header=0, index_col=(0), sep='\t')
print (df)

samples = int(df.iloc[5,0])
ADC_row_start = 6
# print (samples)

fraction = []
for i in range(len(df.columns)):
# for i in range(10):
    # print (df.iloc[6:,0])
    
    # convert to voltage
    df['V'] = ADC_CONVERSION(df.iloc[ADC_row_start:,i])
    # print (df['V'][ADC_row_start:])
    
    # insert perfect cold-atom signal
    N4 = slice(ADC_row_start, ADC_row_start+samples)
    N34 = slice(ADC_row_start+samples,ADC_row_start+samples*2)
    BG4 = slice(ADC_row_start+samples*2,ADC_row_start+samples*3)
    # print (df.iloc[ADC_row_start,i])
    # print (df['V'][ADC_row_start:ADC_row_start+samples])
    # print (df['V'][ADC_row_start+samples:ADC_row_start+samples*2])
    # print (df['V'][ADC_row_start+samples*2:])
    # print ('')
    
    # constant offset
    # df['V'][N4] = df['V'][N4] + 6
    # df['V'][N34] = df['V'][N34] + 6
    # df['V'][BG4] = df['V'][BG4] + 0.5
    
    # cold_atom offset
    t = np.arange(0,75,1)
    df['V'][N4] = cold_atom(df['V'][N4],t)
    df['V'][N34] = cold_atom(df['V'][N34],t)
    df['V'][BG4] = 0.2 + df['V'][BG4]
    
    #calculate the fraction and SNR
    N4 = np.sum(df['V'][N4])
    N34 = np.sum(df['V'][N34])
    BG4 = np.sum(df['V'][BG4])
    fraction_ = (N4 - BG4) / (N34- BG4)
    fraction.append(fraction_)


    #--------- plotting ---------
    fig1, ax1 = plt.subplots()
    ax1.plot(df['V'][ADC_row_start:],'-')
    
    ax1.set_ylim([0,10])
    ax1.set_xlabel(r'index')
    ax1.set_ylabel(r'V')
    # ax1.legend(loc='best')
    ax1.grid()
    plt.show()
    
# print (fraction)
S = np.average(fraction[10:])
N = np.std(fraction[10:])
SNR = S/N
print ("Signal = %.5f" %S)
print ("Noise = %.5f" %N)
print ("SNR = %.5f" %SNR)

    

