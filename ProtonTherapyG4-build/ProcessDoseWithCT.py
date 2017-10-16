import numpy as np;
import dicom
import os
import pylab
import matplotlib.pyplot as plt

class ErrorHandler:
    def __init__(self,nameList):
        self._checkFile_dose(nameList[0])
        self._checkFile_specsFile(nameList[2])
        self._checkDir_dicom(nameList[1])
        self._checkDir_output(nameList[3])

    def _checkFile_dose(self,fname):
        if not os.path.isfile(fname):
            raise IOError("Dose File does not exists!!")

    def _checkFile_specsFile(self,fname):
        if not os.path.isfile(fname):
            raise IOError("Specsfile does not exists")

    def _checkDir_dicom(self,fname):
        if not os.path.isdir(fname):
            raise IOError("Ascii Dicom Directory does not exists")

    def _checkDir_output(self,fname):
        if not os.path.isdir(fname):
            os.mkdir(fname)

class ReadSpecsFile:
    def __init__(self,fname):
        self.fname = fname
        self.fnameList = []
        self.numericArray = np.array([])
        self._ReadSpecsToMemory()

    def _ReadSpecsToMemory(self):
        with open(self.fname,"r") as readFile:
            readFile.readline()
            count = 0
            for line in readFile:
                line = line.strip()
                line = line.split('\t')
                self.fnameList.append(line[0])
                lineArray = np.array([float(line[1]),float(line[2]),float(line[3]),
                float(line[4]),float(line[5]),float(line[6]),float(line[7]),float(line[8]),
                float(line[9]),float(line[10]),float(line[11]),float(line[12]),float(line[13]),
                float(line[14])])
                lineArray = lineArray.reshape(1,14)
                if count!=0:
                    self.numericArray = np.append(self.numericArray,lineArray,axis=0)
                else:
                    self.numericArray = lineArray
                count += 1
        #print self.fnameList
        #print self.numericArray

class Construct3dDose:
    def __init__(self,NVoxel,fname):
        self.NVoxel = NVoxel
        self.dose   = np.loadtxt(fname)
        # initialize 3d array of dose
        print NVoxel
        self.dose3d = np.zeros((NVoxel[0],NVoxel[1],NVoxel[2]))
        self.dose3d[:] = np.NAN
        self._construct3darray()

    def _construct3darray(self):
        for k in range(len(self.dose[:,0])):
            indexTuple = Construct3dDose.decompressIndices(self.dose[k,0],
                        NVoxel[0],NVoxel[1],NVoxel[2])
            self.dose3d[indexTuple[0],indexTuple[1],indexTuple[2]] = self.dose[k,1]

    @staticmethod
    def decompressIndices(index,Ni,Nj,Nk):
        k = index%Nk
        tmp = (index - k)/Nk
        j = tmp%Nj
        i = (tmp- j)/Nj
        return (int(i),int(j),int(k))

if __name__=="__main__":
    fileName_dose = "dose.out"
    dirName_dicom = "LungRT01_ascii"
    fileName_spcsFile = "LungRT01_specsFile.txt"
    dirName_outputFile = "LungRT01_outFile"
    nameList = [fileName_dose, dirName_dicom, fileName_specsFile,dirName_outputFile]

    ErrorHandler(nameList)
    readSpecsfile_instance =  ReadSpecsFile(fileName_specsFile)
    fnameList = readSpecsfile_instance.fnameList;
    numericArray = readSpecsfile_instance.numericArray;

    NVoxelX = int(numericArray[0,1])
    NVoxelY = int(numericArray[0,2])
    minZ    = min(numericArray[:,7])
    binsizeZ = numericArray[0,0] #in mm
    maxZ    = max(numericArray[:,7])
    NVoxelZ = len(np.unique(numericArray[:,7]))
    NVoxel  = [NVoxelX, NVoxelY, NVoxelZ]

    dose3d = Construct3dDose(NVoxel,fileName_dose).dose3d

    ## Main code for producing combined image of Dose and Dicom Data
    numDicomFiles = len(fnameList)
    for k in range(numDicomFiles):
        fname_ascii = dirName_dicom + "/" + fnameList[k][0:-1]
        dicomData   = np.loadtxt(fname_ascii)
        pylab.imshow(dicomData,cmap=pylab.cm.bone)

        zSlice = (numericArray[k,7]-minZ)/binsizeZ
        zSlice = int(zSlice)
        pylab.title("z="+str(zSlice))
        pylab.colorbar()
        doseData = dose3d[:,:,zSlice].reshape(NVoxelX,NVoxelY)
        pylab.imshow(doseData.T,cmap=pylab.cm.jet,alpha=0.6)

        pylab.savefig(fnameList[k][0:-6]+".pdf")
        pylab.close()
