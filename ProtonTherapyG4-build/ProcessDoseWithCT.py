import numpy as np;
import dicom
import os
import pylab
import matplotlib.pyplot as plt
import csv
import math

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

class Construct3dLET:
    def __init__(self):
        self.fname1 = 'LET_D_h3_1.csv' #contain edep2
        self.fname2 = 'LET_D_h3_2.csv' #contain edep
        #######********* This needs to change when needed *********##########
        self.Nx = 512
        self.Ny = 512
        self.Nz = 3
        self.let3d  = np.zeros((self.Nx,self.Ny,self.Nz))
        #self.let3d[:] = np.NAN
        self.edepData = []
        self.edep2Data = []

        self._ReadLetData()
        self._Construct3dArray()
        self._Convert2NumpyArray()

    def _ReadLetData(self):
        # Read edep file
        with open(self.fname2,'r') as readFile:
            reader = csv.reader(readFile)
            counter = 0
            for row in reader:
                if counter > 12:
                    row = [float(i) for i in row]
                    self.edepData.append(row)
                else:
                    counter += 1
                    continue
        # Read edep2 file
        with open(self.fname1,'r') as readFile:
            reader = csv.reader(readFile)
            counter = 0
            for row in reader:
                if counter > 12:
                    row = [float(i) for i in row]
                    self.edep2Data.append(row)
                else:
                    counter += 1
                    continue

    def _Construct3dArray(self):
        for k in range(len(self.edep2Data)):
            indexTuple = Construct3dLET.decompressIndices(k,
                        self.Nx+2,self.Ny+2,self.Nz+2)

            if self._CheckBoundary(indexTuple,self.Nx+2,self.Ny+2,self.Nz+2):
                if self.edep2Data[k][0] == 0 or self.edepData[k][1]==0:
                    self.let3d[indexTuple[0]-1,indexTuple[1]-1,indexTuple[2]-1] = np.nan
                else:
                    self.let3d[indexTuple[0]-1,indexTuple[1]-1,indexTuple[2]-1] = self.edep2Data[k][1]/self.edepData[k][1]
                    #print self.let3d[indexTuple[0]-1,indexTuple[1]-1,indexTuple[2]-1]
    def _Convert2NumpyArray(self):
        self.let3d = np.asarray(self.let3d)

    def _CheckBoundary(self,ind,Nx,Ny,Nz):
        if ind[0]*ind[1]*ind[2] == 0:
            return False
        elif ind[0]==Nx-1 or ind[1]==Ny-1 or ind[2]==Nz-1:
            return False
        else:
            return True

    @staticmethod
    def decompressIndices(index,Ni,Nj,Nk):
        i = index%Ni
        j = ((index-i)/Ni)%Nj
        k = ((index-i)/Ni -j)/Nj
        return (int(i),int(j),int(k))

if __name__=="__main__":
    fileName_dose = "dose.out"
    dirName_dicom = "LungRT01_ascii"
    fileName_specsFile = "LungRT01_specsFile.txt"
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
    construct3dLET  = Construct3dLET()
    let3d = construct3dLET.let3d

    ## Main code for producing combined image of Dose and Dicom Data
    numDicomFiles = len(fnameList)
    letMapArr = np.arange(math.ceil(NVoxelZ/2.0)-int(construct3dLET.Nz/2.0), math.ceil(NVoxelZ/2)+int(construct3dLET.Nz/2)+2)
    print letMapArr

    for k in range(numDicomFiles):
        fname_ascii = dirName_dicom + "/" + fnameList[k][0:-1]
        dicomData   = np.loadtxt(fname_ascii)
        pylab.imshow(dicomData,cmap=pylab.cm.bone)

        # overlay with Dose
        zSlice = (numericArray[k,7]-minZ)/binsizeZ
        zSlice = int(zSlice)
        pylab.title("z="+str(zSlice))
        pylab.colorbar()
        doseData = dose3d[:,:,zSlice].reshape(NVoxelX,NVoxelY)
        pylab.imshow(doseData.T,cmap=pylab.cm.jet,alpha=0.6)

        pylab.savefig(dirName_outputFile + "/" + fnameList[k][0:-6]+"_dose.pdf")
        pylab.close()

        # Overlay with LET
        if zSlice in letMapArr:
            pylab.imshow(dicomData,cmap=pylab.cm.bone)

            pylab.title("z="+str(zSlice))
            ind = np.where(letMapArr==zSlice)
            letData = let3d[:,:,ind].reshape(NVoxelX,NVoxelY)
            print sum(sum(np.isnan(letData[:])))
            #np.savetxt('test'+str(zSlice)+'.txt',letData)
            pylab.imshow(letData.T,cmap=pylab.cm.jet,alpha=0.6)
            pylab.clim(0,20)
            pylab.colorbar()
            pylab.savefig(dirName_outputFile + "/" + fnameList[k][0:-6]+"_let.pdf")
            pylab.close()

            # Additional Processing with LET data
            a1 = 0.2
            a0 = 7.33
            dsbData = np.multiply((a1*letData.T+a0),doseData.T)

            #Save DSB break data
            pylab.imshow(dicomData,cmap=pylab.cm.bone)
            pylab.title("z="+str(zSlice))
            pylab.colorbar()
            ind = np.where(letMapArr==zSlice)
            pylab.imshow(dsbData,cmap=pylab.cm.jet,alpha=0.6)

            pylab.savefig(dirName_outputFile + "/" + fnameList[k][0:-6]+"_dsb.pdf")
            pylab.close()

            '''
            # For comparing maximal dose and DSBs

            doseMax=np.zeros(100)
            dsbMax=np.zeros(100)
            for l in range(200,300):
                doseVector = doseData[:,l].reshape(512)
                dsbVector  = dsbData[l,:].reshape(512)
                doseMax[l-200] = np.argmax(doseVector)
                dsbMax[l-200] = np.argmax(dsbVector[(doseMax[l-200]-5):(doseMax[l-200]+5)]) - 5 + doseMax[l-200]

            pylab.plot(doseMax,'r-', label='Dose')
            pylab.plot(dsbMax,'b-', label = 'DSB')
            pylab.ylabel('Position of maximal values')
            pylab.legend()
            pylab.show()

            break

            # Process the tau variable

            tauData = np.zeros((NVoxelX,NVoxelY))
            tauData[:] = np.NAN
            for m in range(NVoxelX):
                #print m
                for mm in range(NVoxelY):
                    if not (math.isnan(dsbData[m,mm]) or math.isnan(doseData[m,mm])) and dsbData[m,mm]>0.:
                        tmp1 = sum(np.multiply((dsbData[m,mm] - dsbData)>0,(doseData[m,mm] - doseData)>0) +
                            np.multiply((dsbData[m,mm] - dsbData)<0,(doseData[m,mm] - doseData)<0))

                        tmp2 = sum(np.multiply((dsbData[m,mm] - dsbData)>0,(doseData[m,mm] - doseData)<0) +
                            np.multiply((dsbData[m,mm] - dsbData)<0,(doseData[m,mm] - doseData)>0))

                        tmp1 = sum(tmp1)
                        tmp2 = sum(tmp2)
                        #print tmp1,tmp2
                        tauData[m,mm] = float(tmp1-tmp2)/(NVoxelX*NVoxelY)
                        #print tauData[m,mm]
                    else:
                        continue

            pylab.imshow(dicomData,cmap=pylab.cm.bone)
            pylab.title("z="+str(zSlice))

            ind = np.where(letMapArr==zSlice)
            pylab.imshow(tauData,cmap=pylab.cm.jet,alpha=0.6)
            pylab.clim(-0.10,0.4)
            pylab.colorbar()
            pylab.savefig(dirName_outputFile + "/" + fnameList[k][0:-6]+"_tau.pdf")
            pylab.close()
            '''
