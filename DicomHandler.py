import numpy as np 
import dicom 
import os
import pylab

class ErrorHandler: 
	def __init__(self,fname_dir):
		self.fname_dir = fname_dir 
		self._checkBaseDirectory()
		self._checkSpecsFile()
		self._createNewDirectory()
		
	def _checkBaseDirectory(self): 
		if not os.path.isdir(self.fname_dir):
			raise IOError("Directory does not exists")

	def _checkSpecsFile(self): 
		if os.path.isfile(self.fname_dir + '_specsFile.txt'): 
			os.remove(self.fname_dir + '_specsFile.txt')

	def _createNewDirectory(self):
		fname = self.fname_dir + '_ascii'
		if not os.path.isdir(fname):
			os.mkdir(fname) # only work for unix env
		else:
			#os.rmdir(fname) #raise IOError when the directory is not empty
			pass

class ConvertDicomData: 
	def __init__(self,fname_baseDir,fname_newDir):
		self.fname_baseDir = fname_baseDir
		self.fname_newDir  = fname_newDir

		# Set header for specs file
		with open(self.fname_baseDir+'_specsFile.txt','a') as writeFile: 
			writeFile.write("FileName \t SliceThickness \t Col \t rows \t pixelSpacings \t Position \t Orientation \n") 

		self._convertData()
	
	def _convertData(self): 
		fname = os.listdir(self.fname_baseDir) 
		for k in range(len(fname)):
			new_fname = self.fname_newDir + '/' + fname[k][0:-3] + 'g4dcm'
			ds = dicom.read_file(self.fname_baseDir + '/' + fname[k]) 
			self._saveDicomSpecs(ds,fname[k])
			np.savetxt(new_fname,ds.pixel_array)

	def _saveDicomSpecs(self,ds,fname): 
		with open(self.fname_baseDir+'_specsFile.txt','a') as writeFile: 
			writeFile.write("%s \t" %fname)
			writeFile.write("%f \t" %ds[0x18,0x50].value)
			writeFile.write("%d \t" %ds[0x28,0x10].value)
			writeFile.write("%d \t" %ds[0x28,0x11].value)
			writeFile.write("%f \t" %ds[0x28,0x30].value[0])
			writeFile.write("%f \t" %ds[0x28,0x30].value[1])
			try: 
				writeFile.write("%f \t" %ds[0x20,0x32].value[0]) 
				writeFile.write("%f \t" %ds[0x20,0x32].value[1])
				writeFile.write("%f \t" %ds[0x20,0x32].value[2])
				writeFile.write("%f \t %f \t %f \t" %(ds[0x20,0x37].value[0],ds[0x20,0x37].value[1],ds[0x20,0x37].value[2])) # orientation of row vector
				writeFile.write("%f \t %f \t %f \n" %(ds[0x20,0x37].value[3],ds[0x20,0x37].value[4],ds[0x20,0x37].value[5])) # orientation of col vector 
			except: 
				writeFile.write("-1 \n")


if __name__ == '__main__': 
	fname_baseDir = 'LungRT01'
	fname_newDir  = fname_baseDir + '_ascii'
	ErrorHandler(fname_baseDir)
	ConvertDicomData(fname_baseDir,fname_newDir)
	
	if False: 
		ds = dicom.read_file(fname)
		print ds[0x10,0x20].value # patient's ID 
		print ds[0x18,0x50].value # slice thickness 
		print ds[0x28,0x10].value # columns 
		print ds[0x28,0x11].value # rows 
		print ds[0x28,0x30].value # pixel spacing in mm
		#print ds[0x28,0x32].value # position in mm in upper left corner
		#print ds[0x28,0x37].value # Image orientation in direction cosine

		# show image
		x = np.arange(ds[0x28,0x10].value)*ds[0x28,0x30].value[0] / 10 # in cm
		im = ds.pixel_array # in HU
		pylab.imshow(im, cmap=pylab.cm.bone)
		pylab.colorbar()
		pylab.show()




	