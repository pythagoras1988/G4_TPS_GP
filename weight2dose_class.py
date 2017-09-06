import numpy as np;
import os.path;
import subprocess;
import warnings;

fluenceConstant = 100000

# Class for loading weight data from commercial TPS
class Load_dose_weight:
    def __init__(self,fname):
    	try:
        	self.data = np.loadtxt(fname,skiprows=1,delimiter=',')
        	self.dataLength = len(self.data[:,0])
        except:
            # if unable to load file, set length of data to be 0
        	self.dataLength = 0
        #if np.abs(sum(self.data[:,2])-1)>0.5:
        #    warnings.warn('Sum of weight not equal to 1...')

# create log file for debug use
class CreateLogFile:
    def __init__(self):
        self.data = np.empty((1,3))
        self.index = 0
        if os.path.isfile('log.txt'):
            os.remove('log.txt')

    def saveLog(self,elapsedTime,energy,run_number):
        temp = [energy,int(run_number),elapsedTime]
        temp = np.array([temp])
        self.data = np.append(self.data,temp,axis=0)
        np.savetxt('log.txt',self.data)
#
class MergeDataToFile:
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

class RegisterDataToMemory:
    def __init__(self,fname_masterdata,dimX=200,dimY=200,dimZ=200):
        self.data = np.zeros(dimX*dimY*dimZ)
        self.fname = fname_masterdata
        self.dim = [dimX,dimY,dimZ]
        print('Memory allocated for dose scoring...')

    def saveData(self):
        np.savetxt(self.fname,self.data)

    def registerData(self):
        doseData = np.loadtxt('Dose.out',skiprows=1)
        for k in xrange(len(doseData[:,0])):
            index = self.__convert1d(doseData[k,0],doseData[k,1],doseData[k,2])
            index = int(index)
            self.data[index] = self.data[index] + doseData[k,3]

    def __convert1d(self,x,y,z):
        return x + y*self.dim[0] + z*self.dim[0]*self.dim[1]

class Rename_data:
    def __init__(self,fname_Dose_out):
        if not os.path.isfile(fname_Dose_out):
            os.rename('Dose.out',fname_Dose_out)
        else:
            raise Exception('File name already exists!')

# This class estimates the total fluence that needs to be run
class EstimateTotalFluence:
    def __init__(self,fname_energyInfo):
        self.energyInfo = np.loadtxt(fname_energyInfo,skiprows=1,delimiter=',')
        self.totalEnergyLayers = len(self.energyInfo[:,0])
        assert self.totalEnergyLayers > 0
        self.totalFluence = fluenceConstant
        self.totalWeight  = 0;
        self._sumFluenceAllEnergy()

    def _sumFluenceAllEnergy(self):
        for kk in xrange(self.totalEnergyLayers):
            fname_doseWeight = 'T_' + str(int(self.energyInfo[kk,0]*10000)) + '.txt'
            weightData = Load_dose_weight(fname_doseWeight)
            self.totalWeight += sum(weightData.data[:,2])

    def getTotalFluenceForRun(self):
        return self.totalWeight*self.totalFluence

# This class for configuring the setup condition of geant4
class G4_setup:
    def __init__(self,energy,xPos,yPos,weight,fname):
        self.fname_G4 = fname
        self.energy   = energy
        self.xPos = xPos
        self.yPos = yPos
        self.weight = weight
        self._totalFluence = fluenceConstant
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

    def change_numThreads(self,numThreads):
        threadLine = '/run/numberOfThreads ' + str(numThreads) + '\n'
        self.lines[29] = threadLine

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

    def change_twissParam(self):
        pass

    def _calibrateEnergy(self):
        pass
