import math
import numpy as np

#Original foot print (0.8, 1.6)
footPrint = (0.4, 0.8)

centerTop = (-4.035, -1.6)
centerBottom = (4.235, -5.3)


# 1 | 2
# -----
# 3 | 4

quadrants = [ (-1,1), (1,1), (1,-1), (-1,-1) ]


for i in range(0, len(quadrants)):
    print ("Quadrant %i: %4.3f, %4.3f") %(i,
                                           centerBottom[0]+(quadrants[i][0] * footPrint[0]),
                                           centerBottom[1]+(quadrants[i][1] * footPrint[1]),
                                           )
