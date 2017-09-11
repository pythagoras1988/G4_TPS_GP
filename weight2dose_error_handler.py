import os
import numpy as np

class ErrorHandler:
    def __init__(self):
        print('Checking Required Files in present Directory.....')
        self._checkFileSystem()

    def _checkFileSystem(self):
        if os.path.isfile('hadrontherapy'):
            print('Geant4 exe found...')
        else:
            raise IOError('Geant4 exe not found!')

        if os.path.isfile('hadron_therapy.mac'):
            print('Geant4 .mac file found...')
        else:
            raise IOError('Geant4 .mac file not found')

        if os.path.isfile('List.txt'):
            print('Energy layers file found...')
        else:
            raise IOError('Energy layers file not found')

        if os.path.isfile('energy_calibration_list.txt'):
            print('Energy calibration list file found...')
        else:
            raise IOError('Energy calibration list file not found')

        self._checkSpotWeightFile()

    def _checkSpotWeightFile(self):
        energyLayersInfo = np.loadtxt('List.txt',skiprows=1,delimiter=',')
        for k in xrange(len(energyLayersInfo[:,0])):
            fname_doseWeight = 'T_' + str(int(energyLayersInfo[kk,0]*10000)) + '.txt'
            if not os.path.isfile(fname_doseWeight):
                raise Exception('Spot weight file not found - ' + fname_doseWeight)
        print('Spot weight file all found...')
