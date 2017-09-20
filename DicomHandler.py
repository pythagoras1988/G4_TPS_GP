import numpy as np 
import dicom 
import pylab

if __name__ == '__main__': 
	fname = 'LungRT01/CT.1.3.12.2.1107.5.1.4.66919.30000016082401040855300003557.dcm'
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




	