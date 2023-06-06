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

V_REF_AD7195 = 3.3
GAIN = 1
RES_AD7195 = 2**24

#--------- functions ---------
def ADC_CONVERSION(ADC_CODE_):
    return (ADC_CODE_/RES)*V_REF

def AD7195_ADC_CONVERSION(ADC_CODE_):
    return ( ( ADC_CODE_ * V_REF_AD7195 ) / ( GAIN * RES_AD7195) )

#--------- plotting functions ---------
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

def data_plot(dataframe_, number_):
    # import data
    data = dataframe_
    # print (data)
    
    # convert ADC code to voltage
    # print (data[6:])
    data_V = ADC_CONVERSION(data[6:])
    # # data.loc[-1] = data_V
    # # print (data)
    # print (data_V)

    
    # calculate the fraction
    ADC_SAMPLES = data.loc[5]
    Atom_Number = data.loc[0]
    Fraction = data.loc[4]
    # print (ADC_SAMPLES)
    # N4 = np.sum(data['ADC_CODE'][0:ADC_SAMPLES])
    # N34 = np.sum(data['ADC_CODE'][ADC_SAMPLES:2*ADC_SAMPLES])
    # BG4 = np.sum(data['ADC_CODE'][2*ADC_SAMPLES:3*ADC_SAMPLES])
    # BG34 = np.sum(data['ADC_CODE'][3*ADC_SAMPLES:4*ADC_SAMPLES])
    # fraction = float((N4 - BG4) / (N34 - BG34))
    # print (N4, N34, BG4, BG34, fraction)
    
    # plot
    fig1, ax1 = plt.subplots()
    ax1.plot(data_V,'-', label='ADC')
    ax1.plot([], [], ' ', label='Samples=%u' %(ADC_SAMPLES))
    ax1.plot([], [], ' ', label='Atom Number=%u' %(Atom_Number))
    ax1.plot([], [], ' ', label='Fraction=%.5f' %(Fraction))
    ax1.plot([], [], ' ', label='Shot %u' %(number_))
    
    ax1.set_ylim([0,10])
    ax1.set_xlabel(r'index')
    ax1.set_ylabel(r'Voltage / V')
    ax1.legend(loc='best')
    ax1.grid()
    plt.show()
    
    # fig2, ax2 = plt.subplots()
    # ax2.plot(data[6:],'-', label='ADC')
    # ax2.grid()
    # plt.show()
    
    return


def data_plot_RABI(X_, Y_, freq_volt):
    
    # import data and convert to frequency detuning or voltage
    if (freq_volt == 0):
        X = (X_[10:] - (9192.631770))*1000
    else:
        X = X_[10:]
        
    Y = Y_[10:]
    # print (X)
    # print (Y)
    
    # plot
    fig1, ax1 = plt.subplots()
    ax1.plot(X, Y,'-')
    # ax1.plot([], [], ' ', label='Samples=%u' %(ADC_SAMPLES))

    # ax1.set_ylim([0,10])
    ax1.set_xlabel(r'Detuning / kHz')
    ax1.set_ylabel(r'Fraction')
    ax1.legend(loc='best')
    ax1.grid(which='major', color='#DDDDDD', linewidth=0.8)
    ax1.grid(which='minor', color='#EEEEEE', linestyle=':', linewidth=0.5)
    ax1.minorticks_on()
    
    # import data and convert to frequency detuning or voltage
    if (freq_volt == 0):
        ax1.set_xlabel(r'Detuning / kHz')
    else:
        ax1.set_xlabel(r'OXCO Control Voltage / V')
        
    plt.show()
    
    return

# data_plot("detection")