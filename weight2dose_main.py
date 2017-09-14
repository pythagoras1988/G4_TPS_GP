import numpy as np;
import os.path;
import subprocess;
import warnings;
import time;
import argparse;

# import classes
from weight2dose_class import Load_dose_weight
from weight2dose_class import MergeDataToFile
from weight2dose_class import RegisterDataToMemory
from weight2dose_class import G4_setup
from weight2dose_class import CreateLogFile
from weight2dose_error_handler import ErrorHandler

global testing
testing = False

# This is the main code
if __name__ == '__main__':
    # Parsing arguments
    parser = argparse.ArgumentParser('Set the number of computers to be used')
    helpStatement = 'First input: Total number of computers, Second input: Computer number'
    parser.add_argument('--ncpus',required=True,type=int,nargs=2, help=helpStatement)
    args = parser.parse_args()
    if args.ncpus[0]<1 or arg.cpus[1]<1:
        raise ValueError('Invalid option in --ncpus')

    # Read total energy layers
    if testing:
        weightData = Load_dose_weight('T_1311000.txt')
        energy = 131.1
        totalEnergyLayers = 1
    else:
        # Read the entire possible energy layers and spot size
        energyLayersInfo = np.loadtxt('List.txt',skiprows=1,delimiter=',')
        totalEnergyLayers = len(energyLayersInfo[:,0])

    # Check for option to manually partition the run of the energy layers ->
    if args.ncpus[0]==1:
        startEnergyLayers = 0
        stepEnergyLayers  = 1
    else:
        startEnergyLayers = args.ncpus[1] - 1
        stepEnergyLayers  = args.ncpus[0]
        fname_masterDose = 'masterDose' + str(args.ncpus[1]) + '.txt'

    # Initialization
    ErrorHandler()
    startTime = time.time()
    logFile   = CreateLogFile()
    regData   = RegisterDataToMemory(fname_masterDose)

    # Check for masterDose files
    if os.path.isfile(fname_masterDose):
        answer = raw_input('masterDose.txt file exists. Do you want to overwrite? (y/n)')
        if answer == 'n':
            raise Exception('Stopped program!')
        else:
            os.remove(fname_masterDose)

    # Main code for running Geant4
    for kk in xrange(startEnergyLayers,totalEnergyLayers,stepEnergyLayers):
        if not testing:
            fname_doseWeight = 'T_' + str(int(energyLayersInfo[kk,0]*10000)) + '.txt'
            print fname_doseWeight
            weightData = Load_dose_weight(fname_doseWeight)
            energy = energyLayersInfo[kk,0] # in MeV

        for k in xrange(weightData.dataLength):
            xPos   = weightData.data[k,0] # in mm
            yPos   = weightData.data[k,1] # in mm
            weight = weightData.data[k,2]

            #Setup Geant4 parameters
            setup = G4_setup(energy,xPos,yPos,weight,'hadron_therapy.mac')
            setup.read_file()
            setup.change_energy()
            setup.change_field()
            setup.change_fluence()
            setup.write_file()

            # Run geant4 for hadron therapy
            subprocess.call(["./hadrontherapy", "hadron_therapy.mac"]);

            # Process data from geant 4
            #processData = MergeDataToFile(fname_masterDose)
            regData.registerData()
            regData.saveData()

            # Verbosity
            print('Processing Energy Layers %d' %kk)
            elapsedTime = (time.time()-startTime)/3600
            print('Elapsed time = %f hours' %elapsedTime)
            logFile.saveLog(elapsedTime,energy,k)
