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



fileList = os.listdir(BaseDir)

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


