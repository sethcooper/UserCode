#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunch
import HSCPMakeScaledPredictionsLaunch
import HSCPDoLimitsLaunch
import glob
from SignalDefinitions import *
from subprocess import call
import datetime

now = datetime.datetime.now()
Date = now.strftime("%b%d")

# Set things here
AllSlices = False
RunCondor = False
#QueueName = '1nd'
QueueName = '8nh'
## FOR NOW, CutPt=Std means CutIas Std also!
#CutPt = 'Std'
CutPt = 50
#CutIas = 'Std'
CutIas = 0.1

FarmDirectory = 'FARM_noTightRPCData_expFit_cutPt'
#
FarmDirectory+=str(CutPt)
FarmDirectory+='GeVcutIas'
FarmDirectory+=str(CutIas)
if(AllSlices):
  FarmDirectory+='_allSlices_'
else:
  FarmDirectory+='_oneSlice_'
FarmDirectory+=Date
InputDataRootFile = os.getcwd()
#InputDataRootFile+='/MakeHSCParticlePlots_dataWithTightRPCTrig_absEta_ptErrorPresel_feb6/outputs/makeHSCParticlePlots_Data2011_all.root'
InputDataRootFile+='/MakeHSCParticlePlots_data_absEta_ptErrorPresel_Mar08/outputs/makeHSCParticlePlots_Data2011_all.root'
sigInput = os.getcwd()
sigInput+='/MakeHSCParticlePlots_Signals_absEta_ptErrorPresel_Feb21/outputs/makeHSCParticlePlots_Feb21_'
#IntLumi = 4679 # 1/pb (2011)
# subtract the lumi before the RPC trigger change (runs before 165970) = 355.227 /pb
IntLumi = 4323.773 # 1/pb
#


OutputIasPredictionDir = os.getcwd()
OutputIasPredictionDir+="/"
OutputIasPredictionDir+=FarmDirectory
OutputIasPredictionDir+="/outputs/makeIasPredictions/"
bgInput = OutputIasPredictionDir+'makeIasPredictionsCombined_'
BaseCfgMakeScaled = "makeScaledPredictionHistograms_template_cfg.py"
BaseCfgMakePred = "makeIasPredictions_template_cfg.py"
BaseChXML = "hscp_dataDriven_ch_template.xml"
BaseCombXML = "hscp_dataDriven_template.xml"
BaseMacro = os.getcwd() + "/StandardHypoTestInvDemo.C"
BaseMacroBayesian = os.getcwd() + "/StandardBayesianNumericalDemo.C"
#BaseMacroBayesian = os.getcwd() + "/StandardBayesianMCMCDemo.C"


def printConfiguration():
  if(CutPt=='Std'):
    print 'Using standard analysis optimized Ias and Pt cuts'
  elif(CutIas=='Std'):
    print 'Using standard analysis optimized Ias cut only (pt=50)'
  else:
    print 'Using Pt=50 Ias=0.1'
  if(AllSlices):
    print 'Doing all eta/NoM slices'
  else:
    print 'Doing single eta/NoM slice only'

def checkForScaledPredictions(modelName):
  fileName = os.getcwd()+'/'+FarmDirectory+'/outputs/makeScaledPredictions/'+modelName+'/hscp_combined_hscp_model.root'
  #print 'Checking for ',fileName
  try:
    open(fileName)
  except IOError as e:
    print 'File: ',fileName,' does not exist.'
    return False
  return True


def doIasPredictions():
  #TODO in the real analysis we will use the same ih/pt cuts for sidebands
  #     so make a set of the mass cuts (no duplicates)
  printConfiguration()
  JobName = "makeIasPredictions_"
  for model in modelList:
    if(CutPt=='Std'):
      HSCPMakeIasPredictionsLaunch.SendCluster_Create(FarmDirectory,JobName+model.name+"_",InputDataRootFile,
                                                BaseCfgMakePred, model.massCut, model.ptCut, model.iasCut, AllSlices, RunCondor, QueueName)
    elif(CutIas=='Std'):
      HSCPMakeIasPredictionsLaunch.SendCluster_Create(FarmDirectory,JobName+model.name+"_",InputDataRootFile,
                                                BaseCfgMakePred, model.massCut, 50, model.iasCut, AllSlices, RunCondor, QueueName)
    else:
      HSCPMakeIasPredictionsLaunch.SendCluster_Create(FarmDirectory,JobName+model.name+"_",InputDataRootFile,
                                                BaseCfgMakePred, model.massCut, 50, 0.1, AllSlices, RunCondor, QueueName)

  HSCPMakeIasPredictionsLaunch.SendCluster_Submit()


def doMergeIasPredictions():
  printConfiguration()
  if(CutPt=='Std'):
    for model in modelList:
      thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*'+model.name+'_massCut'+str(model.massCut)+'_pt'+str(model.ptCut)+'_ias'+model.iasCut+'_eta*')
      thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_'+model.name+'_massCut'+str(model.massCut)+'_ptCut'+str(model.ptCut)+'_ias'+model.iasCut+'.root')
      thisMassCutFiles.insert(0,"hadd")
      print 'Merging files for mass cut = ' + str(model.massCut) + ' pt cut = ' + str(model.ptCut) + ' ias cut = ' + model.iasCut
      call(thisMassCutFiles)
  elif(CutIas=='Std'):
    for model in modelList:
      thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*'+model.name+'_massCut'+str(model.massCut)+'_pt50_ias'+model.iasCut+'_eta*')
      thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_'+model.name+'_massCut'+str(model.massCut)+'_ptCut50_ias'+model.iasCut+'.root')
      thisMassCutFiles.insert(0,"hadd")
      print 'Merging files for ' + model.name + ': mass cut = ' + str(model.massCut) + ' pt cut = 50 ias cut = ' + model.iasCut
      call(thisMassCutFiles)
  else:
    for model in modelList:
      thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*'+model.name+'_massCut'+str(model.massCut)+'_pt50_ias'+str(0.1)+'_eta*')
      thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_'+model.name+'_massCut'+str(model.massCut)+'_ptCut50_ias'+str(0.1)+'.root')
      thisMassCutFiles.insert(0,"hadd")
      print 'Merging files for mass cut = ' + str(model.massCut) + ' pt cut = 50 ias cut = ' + str(0.1)
      call(thisMassCutFiles)


def doScaledPredictions():
  printConfiguration()
  # BG
  JobName = "makeScaledPredictions_"
  HSCPMakeScaledPredictionsLaunch.SendCluster_Create(FarmDirectory,JobName,
                                              IntLumi, BaseCfgMakeScaled, BaseChXML,
                                                           BaseCombXML, BaseMacro, AllSlices, RunCondor, QueueName)
  for model in modelList:
    if(CutPt=='Std'):
      HSCPMakeScaledPredictionsLaunch.SendCluster_Push(
          bgInput+model.name+'_massCut',sigInput+model.name+'.root',model.name,model.massCut,model.iasCut,model.ptCut)
    elif(CutIas=='Std'):
      HSCPMakeScaledPredictionsLaunch.SendCluster_Push(
          bgInput+model.name+'_massCut',sigInput+model.name+'.root',model.name,model.massCut,model.iasCut,50)
    else:
      HSCPMakeScaledPredictionsLaunch.SendCluster_Push(
          bgInput+model.name+'_massCut',sigInput+model.name+'.root',model.name,model.massCut,0.1,50)

  HSCPMakeScaledPredictionsLaunch.SendCluster_Submit()


def doLimits():
  printConfiguration()
  JobName = "doLimits_"
  #HSCPDoLimitsLaunch.SendCluster_Create(FarmDirectory, JobName, IntLumi, BaseMacro, False, RunCondor, QueueName)
  HSCPDoLimitsLaunch.SendCluster_Create(FarmDirectory, JobName, IntLumi, BaseMacroBayesian, True, RunCondor, QueueName)
  for model in modelList:
    fileExists = checkForScaledPredictions(model.name)
    if not fileExists:
      return
    else:
      HSCPDoLimitsLaunch.SendCluster_Push(bgInput,sigInput+model.name+'.root',model.massCut,model.iasCut,model.ptCut)

  HSCPDoLimitsLaunch.SendCluster_Submit()



def Usage():
  print 'Usage: Launch.py [arg] where arg can be:'
  print '    0 --> makeHSCParticlePlots (process into RooDataSets)'
  print '    1 --> mergeHSCParticlePlots for data'
  print '    2 --> makeIasPredictions'
  print '    3 --> mergeIasPredictions'
  print '    4 --> makeScaledPredictionPlots (process input for limit-setting macro)'
  print '    5 --> doLimits'



# Running the functions

if len(sys.argv)==1:
  Usage()
  sys.exit()

elif sys.argv[1]=='0':
  print 'Step 0: MakeHSCParticlePlots'
  print 'TODO: Implement this'

elif sys.argv[1]=='1':
  print 'Step 1: MergeHSCParticlePlots for data'
  print 'TODO: Implement this'

elif sys.argv[1]=='2':
  print 'Step 2: Make Ias predictions for background'
  doIasPredictions()

elif sys.argv[1]=='3':
  print 'Step 3: Merge Ias predictions'
  doMergeIasPredictions()

elif sys.argv[1]=='4':
  print 'Step 4: Make scaled prediction plots'
  doScaledPredictions()

elif sys.argv[1]=='5':
  print 'Step 5: Compute limits'
  doLimits()
  

else:
  print 'Did not understand input.'
  Usage()
  sys.exit()


