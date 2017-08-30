#******************************************************************************
# This is the main file to run for running dose calculation engine with MPI.
# Run in the shell with: mpiexec -n 4 python main.py
#
#******************************************************************************
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
from weight2dose_class import EstimateTotalFluence
from weight2dose_mpi import InitMPI
from mpi4py import MPI

# This is the main code
if __name__ == '__main__':
    #--------------------------------------------------------------------------
    # Initialization and Constructor
    #--------------------------------------------------------------------------
    startTime = time.time()
    regData   = RegisterDataToMemory('masterDose.txt')
    totData   = RegisterDataToMemory('')
    mpiPtr    = InitMPI()
    size      = mpiPtr.get_num_processes()
    rank      = mpiPtr.get_rank_processes() # this is unique to each process
    threadsPerProcess = 8
    fname_doseFile = 'Dose' + str(rank) + '.out'

    assert size>1

    # rank 0 --> Check for masterDose files in current directory
    # rank 0 --> Create logFile
    if rank == 0:
        fname_masterDose = 'masterDose.txt'
        if os.path.isfile(fname_masterDose):
            os.remove(fname_masterDose)

        totalFluence = EstimateTotalFluence('List.txt').getTotalFluenceForRun()
        totalFluence = int(totalFluence)
        print('Total particles to be run in this MC simulation is %d ...' %totalFluence)
        print('Total Processes = %d, Total G4 Threads = %d ...' %(size,threadsPerProcess))

        answer = raw_input('Agree to run? (y/n)')
        if answer == 'n':
            raise Exception('Stopped program!')
        else:
            pass

    mpiPtr.comm.Barrier()
    #--------------------------------------------------------------------------
    # Getting information on the energy layers used
    #--------------------------------------------------------------------------
    energyLayersInfo = np.loadtxt('List.txt',skiprows=1,delimiter=',')
    totalEnergyLayers = len(energyLayersInfo[:,0])
    assert totalEnergyLayers > 0 # Ensure there is something to process

    #--------------------------------------------------------------------------
    # Main code for running Geant4
    #--------------------------------------------------------------------------
    for kk in xrange(totalEnergyLayers):
        # run scheduler to allocate energy layer processing to different processes
        scheduler = InitMPI.scheduler_simple(energyLayersInfo,totalEnergyLayers,size)
        print scheduler

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
                setup.change_numThreads(threadsPerProcess)
                setup.change_energy()
                setup.change_field()
                setup.change_fluence()
                setup.write_file()

                # Run geant4 for hadron therapy --> Each process produces its own
                # dose.out files
                #mpiPtr.runG4PerProcess()

                # Process data from geant 4
                regData.registerData()
                #regData.saveData()

                # Verbosity
                print('Processing Energy Layers %d' %kk)
                elapsedTime = (time.time()-startTime)/3600
                print('Elapsed time = %f hours' %elapsedTime)

    #********************************************************************
    # After completing all Geant4 processing and saving data files
    #********************************************************************
    #mpiPtr.destructor()
    mpiPtr.comm.Barrier()
    mpiPtr.comm.Reduce(regData.data,totData.data,op=MPI.SUM,root=0)
    if rank==0:
        totData.saveData()
