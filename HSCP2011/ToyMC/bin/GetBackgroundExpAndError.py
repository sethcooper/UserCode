#!/usr/bin/env python

import string
import os
import sys
import glob
from subprocess import call

BaseDir = "FARM_moreModels_cutPt50GeV_Feb21/logs/makeScaledPredictions/"

def GetModelName(fileName):
  massCutPos = fileName.find("_massCut")
  fileNameReduced = fileName[0:massCutPos]
  lastUndscPos = fileNameReduced.rfind("_")
  signalName = fileNameReduced[lastUndscPos+1:len(fileNameReduced)]
  return signalName



fileList = os.listdir(BaseDir)

for file in fileList:
  if ".out" in file:
    for line in open(BaseDir+file):
      if "found" in line:
        break
    lineSplit = line.split(' ')
    backExp = float(lineSplit[1])
    backExpError = float(lineSplit[3])
    signalName = GetModelName(file)
    print 'Model: '+signalName+' '+' backExpOverIasCut: '+str(backExp)+' +/- '+str(backExpError)


