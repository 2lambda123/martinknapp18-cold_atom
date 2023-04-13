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
# date_ = input(">> Provide Date (YYYY-MM-DD): ")
# folder_ = input(">> Provide Folder: ")
date_ = "2023-04-12"
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
    N_atom_ = np.average(data['atom_number'][disregard:])
    
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

#--------- plotting ---------
fig1, ax1 = plt.subplots()
ax1.plot(df['N_atom'], df['SNR'],'.')

# ax1.set_xlim([0,np.max(df['X'])+20])
# ax1.set_ylim([0,500])
# ax1.set_xlabel(r''+folder_+'')
ax1.set_xlabel(r'atom_number')
ax1.set_ylabel(r'SNR')
# ax1.legend(loc='best')
ax1.grid()
plt.show()
