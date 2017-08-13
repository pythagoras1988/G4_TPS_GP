import numpy as np;
import os.path;
import subprocess;
import warnings;
import time;

# import classes
from weight2Dose_class import Load_dose_weight
from weight2Dose_class import MergeDataToFile
from weight2Dose_class import RegisterDataToMemory
from weight2Dose_class import G4_setup
from weight2Dose_class import CreateLogFile

global testing
testing = True

# This is the main code
if __name__ == '__main__':
    startTime = time.time()
    logFile   = CreateLogFile()
    regData   = RegisterDataToMemory('masterDose.txt')

    # Check for masterDose files
    fname_masterDose = 'masterDose.txt'
    if os.path.isfile(fname_masterDose):
        answer = raw_input('masterDose.txt file exists. Do you want to overwrite? (y/n)')
        if answer == 'n':
            raise Exception('Stopped program!')
        else:
            os.remove(fname_masterDose)

    if testing:
        weightData = Load_dose_weight('T_1311000.txt')
        energy = 131.1
        totalEnergyLayers = 1
    else:
        # Read the entire possible energy layers and spot size
        energyLayersInfo = np.loadtxt('List.txt',skiprows=1)
        totalEnergyLayers = len(energyLayersInfo[:,0])

    # Main code for running Geant4
    for kk in xrange(totalEnergyLayers):
        if not testing:
            fname_doseWeight = 'T_' + str(energyLayersInfo[kk,0]*10000) + '.txt'
            weightData = load_dose_weight(fname_doseWeight)
            energy = energyLayersInfo[kk,0] / 10000 # in MeV

        for k in xrange(weightData.dataLength):
            xPos   = weightData.data[k,0] # in mm
            yPos   = weightData.data[k,1] # in mm
            weight = weightData.data[k,2]

            #Setup Geant4 parameters
            setup = G4_setup(energy,xPos,yPos,weight)
            setup.read_file()
            setup.change_energy()
            setup.change_field()
            setup.change_fluence()
            setup.write_file()

            # Run geant4 for hadron therapy
            subprocess.call(["./hadrontherapy", "hadron_therapy.mac"]);

            # Process data from geant 4
            #processData = MergeDataToFile(fname_masterDose)
            regData.RegisterData()
            regData.saveData()

            # Verbosity
            print('Processing Energy Layers %d' %kk)
            elapsedTime = (time.time()-startTime)/3600
            print('Elapsed time = %f hours' %elapsedTime)
            logFile.saveLog(elapsedTime,energy,k)
