import math
import numpy as np

pi = np.pi
s = ""
lut = []
for m in range(32):
    y = round(32767*math.sin(pi/32 * m))
    #print(m, "->", y)
    s += str(y)+ ","
    lut.append(y)
    #print(str(y)+ ",")
#print(s)

print(lut)

#====================================
#determine maximum difference
#====================================
array1 = np.array(lut[:(len(lut)-1)])
array2 = np.array(lut[1:])

diff = np.subtract(array1,array2)
#print(diff)
print(np.max(diff), np.min(diff))
