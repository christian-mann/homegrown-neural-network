from numpy import linspace
from itertools import product
x = list(linspace(0.0,1.0,20))
for item in product(x,x): print item[0], item[1]