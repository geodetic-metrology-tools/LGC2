## test LGC python module

import pyLGC
import numpy as np
import scipy as sp
from scipy import sparse
from scipy.io import mmread

a= pyLGC.LGCController('Title-Example.lgc2')
par=a.get_parameter()
print(par)
print('hello')