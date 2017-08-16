from mpi4py import MPI
import numpy as np

class InitMPI:
    def __init__(self):
        self.comm = MPI.COMM_WORLD
        self.size = self.comm.Get_size()
        print('Initializing MPI mode for %d processes' %self.size)
    
