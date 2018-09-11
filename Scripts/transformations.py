import json
import math as math
import sys
import re
import statsmodels.api as sm
import numpy as np
import pandas as pd
from hashlib import md5
import json
import base64
from base64 import b64decode
from base64 import b64encode
from Crypto.Cipher import AES
from sklearn import datasets
from sklearn import linear_model
from sklearn.neighbors import NearestNeighbors
from sklearn.ensemble import RandomForestClassifier

key = b'\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c'
BLOCK_SIZE = 8  # Bytes
pad = lambda s: s + (BLOCK_SIZE - len(s) % BLOCK_SIZE) * \
                chr(BLOCK_SIZE - len(s) % BLOCK_SIZE)
unpad = lambda s: s[:-ord(s[len(s) - 1:])]

#real dimensions of floor in meters
xF = 15.8
yF = 27.2

#angle of axis rotation, calculated by simple math equations of rotating axes.
angle  = 1.43330453

# beacon positions as used
# 55.944 at the beginning of Latitude
#-3.186 at the beginning of Longituted
b1Lat  = 4578
b2Lat  = 4424
b3Lat  = 5233
b4Lat  = 5226
b5Lat  = 4839
b6Lat  = 4905
b7Lat  = 4377
b8Lat  = 4321
b9Lat  = 4493
b10Lat = 4910
b1Lon  = -6151
b2Lon  = -7264
b3Lon  = -6540
b4Lon  = -7526
b5Lon  = -8280
b6Lon  = -6483
b7Lon  = -7992
b8Lon  = -9048 
b9Lon  = -9836
b10Lon = -9414


#decryption function in case of AES encryption
def decrypt(inp):
    a = bytes(inp)
    cipher = AES.new(key, AES.MODE_ECB)
    b = unpad(cipher.decrypt(a))
    res = [int(x) for x in b]
    return res    

#transformation an X,Y back to lat,lng
def transformPositionToOutout(posX, posY):
    posX = posX/10000000 + 55.944
    posY = posY/10000000 - 3.186
    return (posX, posY)

#transformation of lat, lng to ready for x,y coordinates
def transformPositionToEdit(posX, posY):
    posX = (posX - 55.944) * 10000000
    posY = (posY + 3.186) * 10000000
    return (int(math.trunc(posX)), int(math.trunc(posY)))

#final transformation of the above to x,y coordinates
def transformLatLontoXY(latitude, longtitude):
    y = math.trunc(latitude*math.cos(angle) + longtitude*math.sin(angle)) 
    x = math.trunc(-latitude*math.sin(angle) + longtitude*math.cos(angle))
    x = int (-x)
    x = x//10 -500
    y = int (-y)
    y = y//10 -500
    return(x,y)

def transformXYtoLatLon(x, y):
    x          = -(x + 500)*10
    y          = -(y + 500)*10
    longtitude = math.trunc(x*math.cos(angle) + y*math.sin(angle))
    latitude   = math.trunc(-x*math.sin(angle) + y*math.cos(angle)) 
    return(latitude,longtitude)

# transforming gyroscope values back to the correct range
def transformGyrVal(val):
    if val >= 128:
        val = 128 - val
    val = val * 10
    return val

# transforming accelerometer values back to the correct range
def transformAccVal(val):
    if val >= 128:
        val = 128 - val
    val = val / 50
    return val

# a JSON parser in case we have a JSON object and raw as now
def parseJSONtoArray(line):
    wjdata = json.loads(line)
    return (wjdata["byte1bid"], wjdata["byte2rss"], wjdata["byte3accx"], wjdata["byte4accy"], wjdata["byte5accz"], wjdata["byte6gyrx"], wjdata["byte7gyry"], wjdata["byte8gyrz"], wjdata["lat"], wjdata["long"],  wjdata["time"])

# helper function to make an int x to decimal 0.x
def toF(x):
    if x>1:
        return toF(x/10)
    else:
        return x

# a function that trains a system based on alogrihtm and returns the result ready for use
def trainSystem(x,y):
    #reg = RandomForestClassifier(max_depth=2, random_state=0)
    #reg = linear_model.LinearRegression()
    reg = linear_model.LogisticRegression()
    return reg.fit(x,y)

# the 'hacky' parser we use 
def readingline(line):
    if line.strip()!='':
        a = (line.split("ImmutableMultiDict"))
        a = a[1]
        a=re.split('\D',a)
        newL = []
        for x in a:
            if len(x)>0:
                newL.append(x)
        i = 0
        res1 = []
        res  = []
        time = newL[11]+ ':' + newL[12]+ ':'+ newL[13]
        for l in range(0,len(newL)):
            if ((l % 2) !=0 and (l<10)) or ((l>17) and (l%2)==0):
                res1.append(newL[l])
        for x in res1:
            res.append(int(x))
        return (time,res)

# the case we have training data
def trainingProcess():
    f = open("training", "r")
    b_ids = []
    b_rss = []
    res_x = []
    res_y = []
    accX  = []
    accY  = []
    accZ  = []
    gyrX  = []
    gyrZ  = []
    gyrY  = []
    i = 0
    for line in f:
        if line.strip()!='':
            (time,res) = readingline(line)
            lat  = 55.0+toF(res[5]*1.0)
            lon  = -3.0-toF(res[3]*1.0)
            (x,y) = transformPositionToEdit(lat,lon)
            (x,y) = transformLatLontoXY(x,y) 
            res_x.append(x)
            res_y.append(y)
            # in case of encrypted uncomment the following
            #enc1 = [res[2], res[7], res[8], res[9], res[4], res[1], res[6], res[0]]
            #(time,res) = readingline(line)
            #enc2 = [res[2], res[7], res[8], res[9], res[4], res[1], res[6], res[0]]
            #[res[2], res[7], res[8], res[9], res[4], res[1], res[6], res[0]] = decrypt(enc1 + enc2)
            b_ids.append(res[2])
            b_rss.append(res[7])
            accX.append(res[8])
            accY.append(res[9])
            accZ.append(res[4])
            gyrX.append(res[1])
            gyrY.append(res[6])
            gyrZ.append(res[0])
            i = i+1
    n = len(b_ids)
    x =  [[0 for x in range(16)] for y in range(n)]
    for i in range(0,len(b_ids)):
        index = b_ids[i] 
        x[i][index-1] = b_rss[i]
        x[i][10]      = accX[i]
        x[i][11]      = accY[i]
        x[i][12]      = accZ[i]
        x[i][13]      = gyrX[i]
        x[i][14]      = gyrY[i]
        x[i][15]      = gyrZ[i]
    x_T = trainSystem(x,res_x)
    y_T = trainSystem(x,res_y)
    return(x_T, y_T)

def deviation(x,x1,y,y1):
    return (x-x1)**2 + (y - y1)**2

# the case we have real world test data
def testingProcess(x_T,y_T):
    if (x_T == None) or (y_T == None):
        (x_T,y_T) = trainingProcess()
    f2    = open("results.csv","w")
    f3    = open("real_tests","r")
    i     = 0
    dev_s = 0
    f2.write("Lat" + ', ' + "Lng" + ',' + 'timestamp' + '\n')
    for line in f3:
        if line.strip()!='':
            (time,res) = readingline(line)
            idB = res[2]
            t   = [0]*16
            # in case of encrypted uncomment the following
            # enc1 = [res[2], res[7], res[8], res[9], res[4], res[1], res[6], res[0]]
            #(time,res) = readingline(line)
            # enc2 = [res[2], res[7], res[8], res[9], res[4], res[1], res[6], res[0]]
            # [res[2], res[7], res[8], res[9], res[4], res[1], res[6], res[0]] = decrypt(enc1 + enc2)            t[idB-1] = res[7]
            t[10] = res[8]
            t[11] = res[9]
            t[12] = res[4]
            t[13] = res[1]
            t[14] = res[6]
            t[15] = res[0]
            test = [t]
            pr1 = x_T.predict(test)
            pr2 = y_T.predict(test)
            (lat,lon) = transformXYtoLatLon(pr1[0],pr2[0])
            (lat,lon) = transformPositionToOutout(lat,lon)
            f2.write(str(lat)+', ' +str(lon)+ ', ' + time + '\n')
            # in case we want to measure the error, uncomment the following to check
            #lat_Real  = 55.0+toF(res[5]*1.0)
            #lon_Real  = -3.0-toF(res[3]*1.0)
            #dev_s = deviation(lat_Real,lat, lon_Real, lon)
    #print(dev_s)            

# a simple test, given as input no train, so it does it by its own
testingProcess(None,None)