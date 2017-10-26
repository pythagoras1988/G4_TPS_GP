import numpy as np 
import os

class ErrorHandler: 
	def __init__(self): 
		pass

class CreateConstantWeightSpotList(SaveData): 
	def __init__(self,energy): 
		self.energy = energy
		self.dirName = "Spots List"
		self.listData = np.asarray([])
		self.weightData = np.asarray([])
		fname_spotWeight = "T_" + str(energy*10**4) + ".txt"

		self._createListText()
		self.saveListText(self.listData)
		self._createSpotWeightText()
		self.saveSpotWeightText(fname_spotWeight,self.weightData)

	def _createListText(self): 
		listData[0,0] = self.energy 
		listData[0,1] = 13.0 # arbitrary for the time

	def _createSpotWeightText(self): 
		fieldSize = 100 # Full size in mm
		spacing   = 5 # in mm 
		weightValue = 0.01 

		positionVector = np.arange(-fieldSize/2, fieldSize/2, spacing)
		lenVector = len(positionVector) 
		counter   = 0 
		for k in range(lenVector): 
			for kk in range(lenVector): 
				self.weightData[counter,0] = positionVector[k]
				self.weightData[counter,1] = positionVector[kk]
				self.weightData[counter,2] = weightValue


class SaveData: 
	def __init__(self,energy):
		pass

	def saveListText(self,data):
		with open("List.txt","w") as writeFile: 
			# header
			writeFile.write("nominal_energy,spot_size \n")
			# main data
			for k in range(len(self.listData[:,0])): 
				writeFile.write("%.1f,%.5f \n" %(data[k,0],data[k,1]))

	def saveSpotWeightText(self,fname,data): 
		with open(fname,"w") as writeFile:
			#header
			writeFile.write("SpotPositionMap_X,SpotPositionMap_Y,Spot_weights \n")
			#main data
			for k in range(len(data[:,0])):
				writeFile.write("%.5f,%.5f,%.4f" %(data[k,0],data[k,1],data[k,2]))

if (__name__=='__ main__'): 
	constantWeight_FLAG = true; 
	energy = 180 #in MeV
	if constantWeight_FLAG: 
		instance = CreateConstantWeightSpotList(energy)
	