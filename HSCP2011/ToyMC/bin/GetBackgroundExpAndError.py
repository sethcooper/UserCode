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

def GetMassCut(fileName):
  ptCutPos = fileName.find("_ptCut")
  substr = fileName[fileName.find("_massCut")+len("_massCut"):ptCutPos]
  return substr

def GetPtCut(fileName):
  dotOutPos = fileName.rfind(".out")
  substr = fileName[fileName.find("_ptCut")+len("_ptCut"):dotOutPos]
  return substr

fileList = os.listdir(BaseDir)

print
print 'Model         PtCut   IasCut   MassCut     BackExpOverIas'
print

for file in fileList:
  if ".out" in file:
    for line in open(BaseDir+file):
      if "found" in line:
        break
    lineSplit = line.split(' ')
    backExp = float(lineSplit[1])
    backExpError = float(lineSplit[3])

    for line in open(BaseDir+file):
      if "Ias" in line:
        break
    lineSplit = line[line.find("(")+1:len(line)-2]
    iasCut  = lineSplit
    massCut = GetMassCut(file)
    ptCut = GetPtCut(file)
    signalName = GetModelName(file)
    printString = string.ljust(signalName,12)+string.center(ptCut,8)+string.center(iasCut,10)
    printString+=string.center(massCut,10)
    printString+=string.center(str(backExp)+' +/- '+str(backExpError),10)
    print printString


