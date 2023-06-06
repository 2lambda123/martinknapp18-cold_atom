#imports
import numpy as np
import pandas as pd
import os

from scipy.optimize import curve_fit

import matplotlib.pyplot as plt 
import matplotlib.ticker as ticker
import matplotlib.patches as mpatches
from matplotlib import gridspec

# from matplotlib.ticker import (MultipleLocator,
#                                FormatStrFormatter,
#                                AutoMinorLocator)


#--------- path definitons ---------
# date_ = input(">> Provide Date (YYYY-MM-DD): ")
# folder_ = input(">> Provide Folder: ")
date_ = "2023-04-28"
folder_ = "LOAD_TIME"
os.chdir('C:/Users/MAC1/OneDrive - National Physical Laboratory/DPhil/Experimental Results/Detection/2023/'+date_+'/SNR/'+folder_+'')
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

def SNR_FIT(S_, const, prop, QPN):
    A = const**2
    B = S_ * QPN**2
    C = (prop * S_)**2
    return S_ / np.sqrt(A + B + C)

#--------- imports ---------

# import data
X = []
S = []
N = []
SNR = []
N_atom = []
disregard = 10 # disregard the first few values
for i in range(len(filename_)):
    data = pd.read_csv(''+cwd+'/'+filename_[i]+'', header=[0], sep='\t')
    
    X_ = float(filename_[i].partition(".")[0])
    S_ = np.average(data['fraction'][disregard:])
    N_ = np.std(data['fraction'][disregard:])
    N_atom_ = np.average(data['atom_number'][disregard:]) * ( (10/4095) * (218.75E-6) * 1000 ) # times by voltage, x axis conversion, 1000 for mV
    
    X.append(X_)
    S.append(S_)
    N.append(N_)
    SNR.append(S_/N_)
    N_atom.append(N_atom_)
    
df = pd.DataFrame()
df['X'] = X
df['S'] = S
df['N'] = N
df['SNR'] = SNR
df['N_atom'] = N_atom
df = df.sort_values(by=['X'], ascending=True)

print (df)


X_fit = np.linspace(np.min(df['N_atom']), np.max(df['N_atom']), 500)
# popt, pcov = curve_fit(SNR_FIT, df['N_atom'], df['SNR'], p0=[300,300], bounds=(0, [1000, 1000]))
popt, pcov = curve_fit(SNR_FIT, df['N_atom'], df['SNR'], p0=[300,300, 300], bounds=(0, [5000, 5000, 5000]))
print (popt)


#--------- plotting ---------
fig1, ax1 = plt.subplots()
# ax1.plot(df['X'], df['SNR'],'.-')
ax1.plot(df['N_atom'], df['SNR'], '.', label='data')
ax1.plot(X_fit, SNR_FIT(X_fit, *popt), '-', label='fit')
# ax1.plot([], ' ', label=r'Fitted $\sigma_{const}$ = %.2g' %(popt[0]))
ax1.plot([], ' ', label=r'Fitted $1/\sigma_{prop}$ = %u' %(1/popt[1]))
# ax1.plot([], ' ', label=r'Fitted $\sigma_{QPN}$ = %u' %(popt[2]))


# ax1.set_xlim([0,np.max(df['X'])+20])
# ax1.set_ylim([0,500])
# ax1.set_xlabel(r''+folder_+'')
ax1.set_xlabel(r'F4+F3 / mV$\cdot$s')
ax1.set_ylabel(r'SNR')
# ax1.xaxis.set_major_formatter(ticker.FormatStrFormatter('%g'))
# ax1.ticklabel_format(axis="x", style="sci", scilimits=(0,0))
# plt.gca().set_ylim(bottom=0)
# plt.gca().set_xlim(left=0)
ax1.legend(loc='best')
ax1.grid()
plt.show()

