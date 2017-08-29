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
from weight2dose_mpi import InitMPI

# This is the main code
if __name__ == '__main__':
    startTime = time.time()
    regData   = RegisterDataToMemory('masterDose.txt')
    mpiPtr    = InitMPI()
    size      = mpiPtr.get_num_processes()
    rank      = mpiPtr.get_rank_processes() # this is unique to each process
    time.sleep(3)

    # rank 0 --> Check for masterDose files in current directory
    # rank 0 --> Create logFile
    if rank == 0:
        logFile   = CreateLogFile()
        fname_masterDose = 'masterDose.txt'
        if os.path.isfile(fname_masterDose):
            os.remove(fname_masterDose)

    # Getting information on the energy layers used
    energyLayersInfo = np.loadtxt('List.txt',skiprows=1,delimiter=',')
    totalEnergyLayers = len(energyLayersInfo[:,0])
    assert totalEnergyLayers > 0 # Ensure there is something to process

    # Main code for running Geant4
    for kk in xrange(totalEnergyLayers):

        # run scheduler to allocate energy layer processing to different processes
        scheduler = InitMPI.scheduler_simple(energyLayersInfo,totalEnergyLayers,size)

        if rank == scheduler[kk]:
            print('Process %d processing energy layers %d...' %(rank,kk))

            fname_doseWeight = 'T_' + str(int(energyLayersInfo[kk,0]*10000)) + '.txt'
            fname_inputFile  = 'hadron_therapy' + str(rank) + '.mac'
            weightData = Load_dose_weight(fname_doseWeight)
            energy = energyLayersInfo[kk,0] # in MeV

            for k in xrange(weightData.dataLength):
                xPos   = weightData.data[k,0] # in mm
                yPos   = weightData.data[k,1] # in mm
                weight = weightData.data[k,2]

                #Setup Geant4 parameters
                setup = G4_setup(energy,xPos,yPos,weight,fname_inputFile)
                setup.read_file()
                setup.change_energy()
                setup.change_field()
                setup.change_fluence()
                setup.write_file()

                # Run geant4 for hadron therapy

                # Process data from geant 4
                #regData.registerData()
                #regData.saveData()

                # Verbosity
                print('Processing Energy Layers %d' %kk)
                elapsedTime = (time.time()-startTime)/3600
                print('Elapsed time = %f hours' %elapsedTime)
                logFile.saveLog(elapsedTime,energy,k)
