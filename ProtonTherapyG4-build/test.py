from ProcessDoseWithCT import Construct3dLET
from ProcessDoseWithCT import Construct3dDose
import pylab
import numpy as np

a = Construct3dLET().let3d
b =  a[:,:,0].reshape(512,512)
pylab.imshow(b,cmap=pylab.cm.jet);
print b
print np.amax(b)
pylab.clim(0,20)
pylab.colorbar()
pylab.show()
