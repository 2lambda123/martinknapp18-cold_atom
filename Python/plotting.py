#imports
import numpy as np
import pandas as pd
import os

import matplotlib.pyplot as plt 
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches
from matplotlib import gridspec


#--------- path definitons ---------
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Python_Test/Data')
cwd = os.getcwd()
# print (cwd)


#--------- constants ---------
V_REF = 10
RES = 4095


#--------- functions ---------
def ADC_CONVERSION(ADC_CODE_):
    return (ADC_CODE_/RES)*V_REF

def data_plot(filename_):
    # import data
    data = pd.read_csv(''+cwd+'/'+filename_+'_serial.txt', header=None, skiprows=None, names=['ADC_CODE'])
    
    # convert ADC code to voltage
    data['V'] = ADC_CONVERSION(data['ADC_CODE'])
    
    
    # plot
    fig1, ax1 = plt.subplots()
    ax1.plot(data['V'],'.-', label='raw data')
    ax1.set_xlabel(r'index')
    ax1.set_ylabel(r'Voltage / V')
    ax1.legend(loc='best')
    ax1.grid()
    plt.show()
    
    return

# data_plot("detection")