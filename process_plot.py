import numpy as np 
import matplotlib.pyplot as plt

data = np.loadtxt('energy_calibration_list.txt')
plt.plot(data[:,0],(data[:,4]-data[:,1])/data[:,1]*100,'mo-')
#plt.plot(data[:,0],data[:,4],'bo-',label='Geant4 Simulation')
plt.xlabel('Range/cm')
plt.ylabel('Percentage Error')
#plt.legend()
plt.show()

