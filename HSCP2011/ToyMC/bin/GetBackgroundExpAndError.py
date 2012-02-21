#!/usr/bin/env python

import string
import os
import sys
import glob
import math
from subprocess import call

BaseDir1 = "FARM_moreModels_cutPt50GeV_Feb21/logs/makeScaledPredictions/"
BaseDir2 = "FARM_moreModels_usePtOver50GeVOnly_Feb21/logs/makeScaledPredictions/"

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

def GetIasCut(filePath):
  for line in open(filePath):
    if "Ias" in line:
      break
  return line[line.find("(")+1:len(line)-2]

def GetBackExp(filePath):
  for line in open(BaseDir+file):
    if "found" in line:
      break
  lineSplit = line.split(' ')
  return float(lineSplit[1])

def GetBackExpError(filePath):
  for line in open(BaseDir+file):
    if "found" in line:
      break
  lineSplit = line.split(' ')
  return float(lineSplit[3])

def GetSigEff(filePath):
  for line in open(BaseDir+file):
    if "event level" in line:
      break
  lineSplit = line.split(' ')
  return float(lineSplit[0])



fileList = os.listdir(BaseDir1)
print
print 'Model         PtCut   IasCut     MassCut    BackExpOverIas    SignalEff'
print
for file in fileList:
  if ".out" in file:
    backExp = GetBackExp(BaseDir+file)
    backExpError = GetBackExpError(BaseDir+file)
    iasCut  = GetIasCut(BaseDir+file)
    sigEff = GetSigEff(BaseDir+file)
    massCut = GetMassCut(file)
    ptCut = GetPtCut(file)
    signalName = GetModelName(file)
    printString = string.ljust(signalName,15)+string.ljust(ptCut,8)+string.ljust(iasCut,10)
    printString+=string.center(massCut,8)
    printString+=string.center(str(round(backExp,4))+' +/- '+str(round(backExpError,4)),20)
    printString+=string.center(str(round(sigEff,4)),10)
    print printString


fileList = os.listdir(BaseDir2)
print
print 'Model         PtCut   IasCut     MassCut    BackExpOverIas    SignalEff'
print
for file in fileList:
  if ".out" in file:
    backExp = GetBackExp(BaseDir+file)
    backExpError = GetBackExpError(BaseDir+file)
    iasCut  = GetIasCut(BaseDir+file)
    sigEff = GetSigEff(BaseDir+file)
    massCut = GetMassCut(file)
    ptCut = GetPtCut(file)
    signalName = GetModelName(file)
    printString = string.ljust(signalName,15)+string.ljust(ptCut,8)+string.ljust(iasCut,10)
    printString+=string.center(massCut,8)
    printString+=string.center(str(round(backExp,4))+' +/- '+str(round(backExpError,4)),20)
    printString+=string.center(str(round(sigEff,4)),10)
    print printString

# Now do ratios/error
fileList = os.listdir(BaseDir1)
print
print 'Model         BackExp1/BackExp2        SigEff1/SigEff2'
print
for file in fileList:
  if ".out" in file:
    backExp1 = GetBackExp(BaseDir1+file)
    backExpError1 = GetBackExpError(BaseDir1+file)
    sigEff1 = GetSigEff(BaseDir1+file)
    backExp2 = GetBackExp(BaseDir2+file)
    backExpError2 = GetBackExpError(BaseDir2+file)
    sigEff2 = GetSigEff(BaseDir2+file)
    backExpErrorComb = math.sqrt(math.pow(backExpError1/backExp1,2)+math.pow(backExpError2/backExp2,2))
    backExpErrorComb*=(backExp1/backExp2)
    signalName = GetModelName(file)
    printString = string.ljust(signalName,15)
    printString+=string.center(str(round(backExp1/backExp2,4))+' +/- '+str(round(backExpErrorComb,4)),20)
    printString+=string.center(str(round(sigEff1/sigEff2,4)),10)
    print printString

