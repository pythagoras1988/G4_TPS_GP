import numpy as np
import os

class ErrorHandler:
	def __init__(self):
		if not os.path.isdir("Spots List"):
			os.mkdir("Spots List")

class SaveData:
	def __init__(self,energy):
		self.dirName = ""

	def saveListText(self,data):
		fname = self.dirName + "/" + "List.txt"
		with open(fname,"w") as writeFile:
			# header
			writeFile.write("nominal_energy,spot_size \n")
			# main data
			for k in range(len(self.listData[:,0])):
				writeFile.write("%.1f,%.5f \n" %(data[k,0],data[k,1]))

	def saveSpotWeightText(self,fname,data):
		with open(self.dirName + "/" + fname,"w") as writeFile:
			#header
			writeFile.write("SpotPositionMap_X,SpotPositionMap_Y,Spot_weights \n")
			#main data
			for k in range(len(data)):
				writeFile.write("%.5f,%.5f,%.4f\n" %(data[k][0],data[k][1],data[k][2]))

	def createLogFile(self):
		fname = self.dirName + "/" + "Log_file.txt"

class CreateConstantWeightSpotList(SaveData):
	def __init__(self,energy):
		self.energy = energy
		self.dirName = "Spots List"
		self.listData = np.zeros((1,2))
		self.weightData = []
		fname_spotWeight = "T_" + str(energy*10**4) + ".txt"

		self._createListText()
		self.saveListText(self.listData)
		self._createSpotWeightText()
		self.saveSpotWeightText(fname_spotWeight,self.weightData)
		self.createLogFile()

	def _createListText(self):
		self.listData[0,0] = self.energy
		self.listData[0,1] = 13.0 # arbitrary for the time

	def _createSpotWeightText(self):
		fieldSize = 100 # Full size in mm
		spacing   = 5 # in mm
		weightValue = 0.01

		positionVector = np.arange(-fieldSize/2, fieldSize/2, spacing)
		print positionVector
		lenVector = len(positionVector)
		for k in range(lenVector):
			for kk in range(lenVector):
				tmpData = [0,0,0]
				tmpData[0] = positionVector[k]
				tmpData[1] = positionVector[kk]
				tmpData[2] = weightValue
				self.weightData.append(tmpData)
				del tmpData

if __name__=="__main__":
	constantWeight_FLAG = True;
	energy = 180 #in MeV
	ErrorHandler()
	print "shit"
	if constantWeight_FLAG:
		CreateConstantWeightSpotList(energy)
