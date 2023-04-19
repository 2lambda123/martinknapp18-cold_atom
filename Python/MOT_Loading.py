#imports
import numpy as np
import pandas as pd
import os

from scipy.optimize import curve_fit

import matplotlib.pyplot as plt 
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches
from matplotlib import gridspec


#--------- path definitons ---------
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/MOT_Load')
cwd = os.getcwd()
# print (cwd)


#--------- constants ---------
V_REF = 10
RES = 4095

hbar = 1.05E-34             #Js
h = 6.63E-34                #Js
c = 3E8                     #m/s

I_SAT = 1.1049              #mW/cm^2
Gamma = (2*np.pi)*5.234E6   #rad/s

R_f = 2.38E6                  #Ohm  feedback resistor converting detector current to voltage
alpha = 0.65                  #A/W responsivity of the detector
omega = 2*np.pi*(351.722E12)  #rad/s
f = 0.005                     #dimensionless


#--------- variables ---------
P = 3*6                       #mW  Power in the laser beams
w = 7.2E-3                  #cm  beam waist
delta = 2.5*Gamma      #rad/s  laser detuning
d = 12.7E-3                    #mm  radius of the lens collection system
f_lens = 100E-3                #mm  focal length of collection lens
#theta = 0.11                #rad


#--------- functions ---------
def ADC_CONVERSION(ADC_CODE_):
    return (ADC_CODE_/RES)*V_REF

def loading(t_, A_, B_, C_):
    return (A_/B_) * ( 1 - np.exp(-B_*t_) ) + C_

def linear(x_,M_,C_):
    return x_*M_ + C_

def I(P_, w_):
    A = 2*P_
    B = np.pi*w_**2
    return A/B

def R_SCATT(I_, delta_):
    A = I_/I_SAT
    B = Gamma / 2
    C = 1 + A + (delta_/B)**2
    return (A*B) / C

def Angle(d_, f_lens_):
    return np.arcsin(d_/(2*f_lens_))

def Solid_Angle(theta_):
    A = 2*np.pi * (1-np.cos(theta_))
    B = 4*np.pi
    return A/B

def N_atom(V_, I_, delta_, theta_):
    A = V_ / (R_f*alpha)
    B = 1 / R_SCATT(I_,delta_)
    C = 1 / (hbar*omega)
    D = 1 / (1-f)
    E = 1 / (Solid_Angle(theta_))
    return A*B*C*D*E


#--------- imports ---------

# background data
data_BG = pd.read_csv(''+cwd+'/NON_RES_BACKGROUND.csv', header=None, skiprows=[0], names=['t','V'])
# print(data_BG)
BG = np.average(data_BG['V'])
print (BG)

# loading data
data = pd.read_csv(''+cwd+'/MOT_LOAD.csv', header=None, skiprows=[0], names=['t','V'])


# # resample the data by averaging
# N = 100
# for i in range(len(data['V'])):
    
    
data = data.iloc[::100, :] #resample the data
data.reset_index(drop=True)

data['V'] = data['V'] - BG
data = data[(data > 0).all(1)]
data.reset_index(drop=True)

# data = data.iloc[200:1000]
data = data.iloc[300:-150000]
data.reset_index(drop=True)
# print (data['V'])
# data['N'] = N_atom(data['V'], I(P,w), delta, Angle(d, f_lens))
# print (N_atom(1, I(P,w), delta, Angle(d, f_lens))/1E6)

t_spacing = np.diff(data['t'])[0]
time = np.arange(0, len(data['t'])*t_spacing, t_spacing)
data['t_rescaled'] = time
# print (time)
# print(data)

# # data.to_csv(''+cwd+'/data.csv', header=None, index=None)


#--------- analysis ---------
A = 14
B = 3
C = 1E-6

# fitting the data
# popt, pcov = curve_fit(linear, data['t_rescaled'], data['V'])
# print (popt)

popt, pcov = curve_fit(loading, data['t_rescaled'], data['V'], p0=[A,B,C], bounds=([[0,0,0],[20,10,0.01]]))
print (popt)
# perr_Y = np.sqrt(np.diag(pcov_Y))

    
# plot
fig1 = plt.figure()
ax1 = fig1.add_subplot(111)
# fig1, ax1 = plt.subplots()
ax1.plot(data['t_rescaled'], data['V'],'.', label='V')
# ax1.plot(data['t_rescaled'], linear(data['t_rescaled'], *popt),'-', label='fitted')
ax1.plot(data['t_rescaled'], loading(data['t_rescaled'], *popt),'-', label='fitted')
# ax1.plot(data['t_rescaled'], loading(data['t_rescaled'], A,B,0.06),'-', label='guess')
# ax1.plot([], [], ' ', label='Samples=%u' %(ADC_SAMPLES))ADC

sub_range = 500
ax2 = plt.axes([.5, 0.25, 0.25, 0.25])
ax2.plot(data['t_rescaled'][0:sub_range], data['V'][0:sub_range],'.', label='V')
ax2.plot(data['t_rescaled'][0:sub_range], loading(data['t_rescaled'][0:sub_range], *popt),'-', label='fitted')
ax2.grid()
plt.setp(ax2, xticks=[], yticks=[])

    
# ax1.set_ylim([0,10])
ax1.set_xlabel(r'time / s')
ax1.set_ylabel(r'Voltage / V')
ax1.legend(loc='best')
ax1.grid()
plt.show()

