#Calculate LED distances

import math
import numpy as np

yOffset = 1.03
xOffset = 1.02

l = 0.86

numOfLeds = 12

ledPositions = np.arange(numOfLeds)

for i in range(0, numOfLeds):
    xPos = ( l * math.cos( math.radians( ledPositions[i] * 30 ) ) ) + xOffset
    yPos = ( l * math.sin( math.radians( ledPositions[i] * 30 ) ) ) + yOffset

    print ("%i - %4.3f - %4.3f") %(i, yPos, xPos)
