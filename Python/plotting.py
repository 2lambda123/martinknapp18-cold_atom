#imports
import numpy as np
import pandas as pd
import os

import matplotlib.pyplot as plt 
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches
from matplotlib import gridspec


#--------- path definitons ---------
# os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Python_Test/Data')
# cwd = os.getcwd()
# print (cwd)


#--------- constants ---------
V_REF = 10
RES = 4095


#--------- functions ---------
def ADC_CONVERSION(ADC_CODE_):
    return (ADC_CODE_/RES)*V_REF

def data_plot_file(path_, filename_):
    # import data
    data = pd.read_csv(''+path_+'/'+filename_+'.txt', header=None, skiprows=None, names=['ADC_CODE'])
    
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

def data_plot(dataframe_):
    # import data
    data = dataframe_
    
    # convert ADC code to voltage
    data.columns = ['ADC_CODE']
    data['ADC_CODE'] = pd.to_numeric(data['ADC_CODE']) # convert from strings to integer or float
    # print (data)
    data['V'] = ADC_CONVERSION(data['ADC_CODE'])
    # print (data['V'])
    
    # calculate the fraction
    ADC_SAMPLES = 127
    # N4 = np.sum(data['ADC_CODE'][0:ADC_SAMPLES])
    # N34 = np.sum(data['ADC_CODE'][ADC_SAMPLES:2*ADC_SAMPLES])
    # BG4 = np.sum(data['ADC_CODE'][2*ADC_SAMPLES:3*ADC_SAMPLES])
    # BG34 = np.sum(data['ADC_CODE'][3*ADC_SAMPLES:4*ADC_SAMPLES])
    # fraction = float((N4 - BG4) / (N34 - BG34))
    # print (N4, N34, BG4, BG34, fraction)
    
    # plot
    fig1, ax1 = plt.subplots()
    ax1.plot(data['V'],'.-', label='raw data')
    
    # ax1.plot([], [],'',label='Fraction=%.5f' %(fraction))
    
    ax1.set_xlabel(r'index')
    ax1.set_ylabel(r'Voltage / V')
    ax1.legend(loc='best')
    ax1.grid()
    plt.show()
    
    return

# data_plot("detection")