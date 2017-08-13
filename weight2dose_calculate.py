import numpy as np;
import os.path;
import subprocess;
import warnings;
import time;

global testing
testing = True

# Class for loading weight data from commercial TPS
class Load_dose_weight:
    def __init__(self,fname):
        self.data = np.loadtxt(fname,skiprows=1,delimiter=',')
        self.dataLength = len(self.data[:,0])

        if np.abs(sum(self.data[:,2])-1)>0.5:
            warnings.warn('Sum of weight not equal to 1...')

#
class Merge_data:
    def __init__(self,fname_masterdata):
        self.data = np.loadtxt('Dose.out',skiprows=1)
        if os.path.isfile(fname_masterdata):
            self.masterdata = np.loadtxt(fname_masterdata)
            self._merge()
        else:
            self.masterdata = self.data
        np.savetxt(fname_masterdata,self.masterdata)

    def _merge(self): # private method
        for k in xrange(len(self.data[:,0])):
            temp = merge_data.compareVector(self.masterdata[:,0:2],self.data[k,0:2])
            if  temp != -1:
                self.masterdata[temp,3] = self.masterdata[temp,3] + self.data[k,3]
            else:
                self.masterdata = np.append(self.masterdata,self.data[k,:],axis=0)

    @staticmethod
    def compareVector(big,small):
        tmp = (big[:,0]==small[0]) * (big[:,1]==small[1]) * (big[:,2]==small[2])
        if sum(tmp) == 0:
            return -1
        else:
            return np.argmax(tmp)

#
class Rename_data:
    def __init__(self,fname_Dose_out):
        if not os.path.isfile(fname_Dose_out):
            os.rename('Dose.out',fname_Dose_out)
        else:
            raise Exception('File name already exists!')

# This class for configuring the setup condition of geant4
class G4_setup:
    def __init__(self,energy,xPos,yPos,weight):
        self.fname_G4 = 'hadron_therapy.mac'
        self.energy   = energy
        self.xPos = xPos
        self.yPos = yPos
        self.weight = weight
        self._totalFluence = 5000000
        self.prop_constant = 180; # for magnetic field spot position mapping

    def read_file(self):
        with open(self.fname_G4,'r') as f:
            self.lines = f.readlines()
        f.close()

    def write_file(self):
        writeFile = open(self.fname_G4,'w')
        for item in self.lines:
            writeFile.write("%s" % item)
        writeFile.close();
        print('Geant4 input file created.....')

    def change_energy(self):
        particleEnergyLine = '/gps/ene/mono ' + str(self.energy) + ' MeV' + '\n'
        self.lines[59] = particleEnergyLine

    def change_field(self):
        field_x = self.xPos / 10 * np.sqrt(self.energy) / self.prop_constant
        field_y = self.yPos / 10 * np.sqrt(self.energy) / self.prop_constant
        field_x = round(field_x,4)
        field_y = round(field_y,4)

        fieldLine = '/beamLine/setField ' + '0.0 ' + str(field_x) + ' ' + str(field_y) +'\n'
        self.lines[60] = fieldLine

    def change_fluence(self):
        fluenceLine = '/run/beamOn ' + str(int(np.rint(self.weight*self._totalFluence)))
        self.lines[115] = fluenceLine

# create log file for debug use
class CreateLogFile: 
	def __init__(self): 
		self.data  = np.array([])
		self.index = 0
		if os.path.isfile('log.txt'): 
			os.remove('log.txt') 

	def saveLog(self,elapsedTime,energy,run_number):
		self.data[self.index,0] = energy 
		self.data[self.index,1] = run_number
		self.data[self.index,0] = elapsedTime

# This is the main code
if __name__ == '__main__':
    startTime = time.time()
    logFile   = CreateLogFile()

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
            processData = Merge_data(fname_masterDose)

            # Verbosity
            print('Processing Energy Layers %d' %kk)
            elapsedTime = (time.time()-startTime)/3600
            print('Elapsed time = %f hours' %elapsedTime)
            logFile.saveLog(elapsedTime,energy,k)
