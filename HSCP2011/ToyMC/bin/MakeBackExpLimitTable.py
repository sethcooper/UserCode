#!/usr/bin/env python

import string
import os
import sys
import glob
import math
from subprocess import call

BaseDirPt50Ias0p1 = 'FARM_allData_cutPt50GeVcutIas0.1_oneSlice_Mar16'
BaseDirPt50IasStd = 'FARM_allData_validation_cutPt50GeVcutIasStd_oneSlice_Mar16'
BaseDirPtStdIasStd = 'FARM_allData_cutPtStdGeVcutIasStd_oneSlice_Mar16'
runCERN = True

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
  if(runCERN):
    dotOutPos = fileName.rfind(".log")
  else:
    dotOutPos = fileName.rfind(".out")
  substr = fileName[fileName.find("_ptCut")+len("_ptCut"):dotOutPos]
  return substr

def GetIasCut(filePath):
  for line in open(filePath):
    if "Ias" in line and "Running" in line:
      break
  #print "GetIasCut ",line
  return line[line.find("(")+1:len(line)-2]

def GetBackExp(filePath):
  for line in open(filePath):
    if "found" in line and not "WARNING" in line:
      break
  lineSplit = line.split(' ')
  if "found" in line and not "WARNING" in line:
    return float(lineSplit[1])
  else:
    return 0

def GetBackExpError(filePath):
  for line in open(filePath):
    if "found" in line and not "WARNING" in line:
      break
  lineSplit = line.split(' ')
  if "found" in line and not "WARNING" in line:
    return float(lineSplit[3])
  else:
    return 0

def GetSigEff(filePath):
  for line in open(filePath):
    if "event level" in line:
      break
  lineSplit = line.split(' ')
  if "event level" in line:
    return float(lineSplit[0])
  else:
    return 0

def GetBkOverIas(filePath):
  for line in open(filePath):
    if "Bk" in line:
      break
  lineSplit = line.split(' ')
  if "Bk" in line:
    #print line
    return float(lineSplit[0])
  else:
    return 0.0


makeScaledPredictionsDirPtStdIasStd = BaseDirPtStdIasStd+'/logs/makeScaledPredictions/'
fileListPtStdIasStd = os.listdir(makeScaledPredictionsDirPtStdIasStd)
fileListPtStdIasStd.sort()
makeScaledPredictionsDirPt50IasStd = BaseDirPt50IasStd+'/logs/makeScaledPredictions/'
fileListPt50IasStd = os.listdir(makeScaledPredictionsDirPt50IasStd)
fileListPt50IasStd.sort()
makeScaledPredictionsDirPt50Ias0p1 = BaseDirPt50Ias0p1+'/logs/makeScaledPredictions/'
fileListPt50Ias0p1 = os.listdir(makeScaledPredictionsDirPt50Ias0p1)
fileListPt50Ias0p1.sort()
print
#print 'FarmDirectory =',BaseDir
titleString = string.ljust('Model',15)+string.ljust('Pt',8)+string.ljust('Ias',10)+string.center('Mass',8)+string.center('BackExpOverIas',30)
titleString+=string.center('BkOverIas',10)+string.center('SigEff',10)
print
print titleString
if(runCERN):
  fileEnd = '.log'
else:
  fileEnd = '.out'
for index,file in enumerate(fileListPtStdIasStd):
  if fileEnd in file:
    backExpPtStdIasStd = GetBackExp(makeScaledPredictionsDirPtStdIasStd+file)
    backExpErrorPtStdIasStd = GetBackExpError(makeScaledPredictionsDirPtStdIasStd+file)
    iasCutPtStdIasStd  = GetIasCut(makeScaledPredictionsDirPtStdIasStd+file)
    sigEffPtStdIasStd = GetSigEff(makeScaledPredictionsDirPtStdIasStd+file)
    bkPtStdIasStd = GetBkOverIas(makeScaledPredictionsDirPtStdIasStd+file)
    massCutPtStdIasStd = GetMassCut(file)
    ptCutPtStdIasStd = GetPtCut(file)
    signalNamePtStdIasStd = GetModelName(file)
    printString = string.ljust(signalNamePtStdIasStd,15)+string.ljust(ptCutPtStdIasStd,8)+string.ljust(iasCutPtStdIasStd,10)
    printString+=string.center(massCutPtStdIasStd,8)
    printString+=string.center(str(round(backExpPtStdIasStd,30))+' +/- '+str(round(backExpErrorPtStdIasStd,4)),30)
    printString+=string.center(str(bkPtStdIasStd),10)
    printString+=string.center(str(round(sigEffPtStdIasStd,4)),10)
    print printString
    file = fileListPt50IasStd[index]
    backExpPt50IasStd = GetBackExp(makeScaledPredictionsDirPt50IasStd+file)
    backExpErrorPt50IasStd = GetBackExpError(makeScaledPredictionsDirPt50IasStd+file)
    iasCutPt50IasStd  = GetIasCut(makeScaledPredictionsDirPt50IasStd+file)
    sigEffPt50IasStd = GetSigEff(makeScaledPredictionsDirPt50IasStd+file)
    bkPt50IasStd = GetBkOverIas(makeScaledPredictionsDirPt50IasStd+file)
    massCutPt50IasStd = GetMassCut(file)
    ptCutPt50IasStd = GetPtCut(file)
    signalNamePt50IasStd = GetModelName(file)
    printString = string.ljust(signalNamePt50IasStd,15)+string.ljust(ptCutPt50IasStd,8)+string.ljust(iasCutPt50IasStd,10)
    printString+=string.center(massCutPt50IasStd,8)
    printString+=string.center(str(round(backExpPt50IasStd,30))+' +/- '+str(round(backExpErrorPt50IasStd,4)),30)
    printString+=string.center(str(bkPt50IasStd),10)
    printString+=string.center(str(round(sigEffPt50IasStd,4)),10)
    print printString
    file = fileListPt50Ias0p1[index]
    backExpPt50Ias0p1 = GetBackExp(makeScaledPredictionsDirPt50Ias0p1+file)
    backExpErrorPt50Ias0p1 = GetBackExpError(makeScaledPredictionsDirPt50Ias0p1+file)
    iasCutPt50Ias0p1  = GetIasCut(makeScaledPredictionsDirPt50Ias0p1+file)
    sigEffPt50Ias0p1 = GetSigEff(makeScaledPredictionsDirPt50Ias0p1+file)
    bkPt50Ias0p1 = GetBkOverIas(makeScaledPredictionsDirPt50Ias0p1+file)
    massCutPt50Ias0p1 = GetMassCut(file)
    ptCutPt50Ias0p1 = GetPtCut(file)
    signalNamePt50Ias0p1 = GetModelName(file)
    printString = string.ljust(signalNamePt50Ias0p1,15)+string.ljust(ptCutPt50Ias0p1,8)+string.ljust(iasCutPt50Ias0p1,10)
    printString+=string.center(massCutPt50Ias0p1,8)
    printString+=string.center(str(round(backExpPt50Ias0p1,30))+' +/- '+str(round(backExpErrorPt50Ias0p1,4)),30)
    printString+=string.center(str(bkPt50Ias0p1),10)
    printString+=string.center(str(round(sigEffPt50Ias0p1,4)),10)
    print printString


