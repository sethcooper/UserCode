#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunch
import HSCPMakeScaledPredictionsLaunch
import HSCPDoLimitsLaunch
import HSCPDoSignificanceLaunch
import glob
from SignalDefinitions import *
from subprocess import call
import datetime

now = datetime.datetime.now()
Date = now.strftime("%b%d")

# Set things here
AllSlices = True
RunCondor = True
BayesianLimit = True
DoMass = False
QueueName = '8nh'
ThrowToysSignificance = False
## FOR NOW, CutPt=Std means CutIas Std also!
#CutPt = 'Std'
CutPt = 50
#CutIas = 'Std'
CutIas = 0.1

FarmDirectory = 'FARM_allData_NewLumi_50IasBins_'
FarmDirectory+='cutPt'
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
InputDataRootFile+='/FARM_MakeHSCParticlePlots_dataWithTightRPCTrig_absEta_ptErrorPresel_feb6/outputs/makeHSCParticlePlots_feb1_Data2011_all.root'
#InputDataRootFile+='/FARM_MakeHSCParticlePlots_data_absEta_ptErrorPresel_Mar08/outputs/makeHSCParticlePlots_Data2011_all.root'
#InputDataRootFile+='/FARM_MakeHSCParticlePlots_absEta_ptErrorPresel_Mar21/outputs/makeHSCParticlePlots_Mar21_Data2011_all.root'
sigInput = os.getcwd()
#sigInput+='/FARM_MakeHSCParticlePlots_Signals_PUWeights_Mar23/outputs/makeHSCParticlePlots_Mar23_'
sigInput+='/FARM_MakeHSCParticlePlots_Signals_NewLumi_Apr13/outputs/makeHSCParticlePlots_Apr13_'
IntLumi = 4976 # 1/pb (2011, new pixel measurement)
# subtract the lumi before the RPC trigger change (runs before 165970) = 352.067 /pb
#IntLumi = 4623.993 # 1/pb
#


OutputIasPredictionDir = os.getcwd()
OutputIasPredictionDir+="/"
OutputIasPredictionDir+=FarmDirectory
OutputIasPredictionDir+="/outputs/makeIasPredictions/"
bgInput = OutputIasPredictionDir+'makeIasPredictionsCombined_'
BaseCfgMakeScaled = "makeScaledPredictionHistograms_template_cfg.py"
BaseCfgMakePred = "makeIasPredictions_template_cfg.py"
if(DoMass):
  BaseChXML = "hscp_dataDriven_ch_mass_template.xml"
else:
  BaseChXML = "hscp_dataDriven_ch_template.xml"
BaseCombXML = "hscp_dataDriven_template.xml"
BaseChXMLSig = "hscp_dataDriven_ch_sig_template.xml"
BaseMacro = os.getcwd() + "/StandardHypoTestInvDemo.C"
BaseMacroBayesian = os.getcwd() + "/StandardBayesianNumericalDemo.C"
#BaseMacroBayesian = os.getcwd() + "/StandardBayesianMCMCDemo.C"
BaseMacroSignificance = os.getcwd() + "/StandardHypoTestDemo.C"


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
  fileName = os.getcwd()+'/'+FarmDirectory+'/outputs/makeScaledPredictions/'+modelName+'_Limits/hscp_combined_hscp_model.root'
  #print 'Checking for ',fileName
  try:
    open(fileName)
  except IOError:
    print 'File: ',fileName,' does not exist.'
    return False
  return True


def doIasPredictions():
  # in the real analysis we will use the same ih/pt cuts for sidebands
  # so make a set of the mass cuts (no duplicates)
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
      print 'Merging files for ' + model.name + ' mass cut = ' + str(model.massCut) + ' pt cut = ' + str(model.ptCut) + ' ias cut = ' + model.iasCut
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
      print 'Merging files for ' + model.name + ' mass cut = ' + str(model.massCut) + ' pt cut = 50 ias cut = ' + str(0.1)
      call(thisMassCutFiles)


def doScaledPredictions():
  printConfiguration()
  # BG
  JobName = "makeScaledPredictions_"
  HSCPMakeScaledPredictionsLaunch.SendCluster_Create(FarmDirectory,JobName,
                                              IntLumi, BaseCfgMakeScaled, BaseChXML, BaseChXMLSig,
                                                           BaseCombXML, BaseMacro, AllSlices, RunCondor, QueueName)
  for model in modelList:
    looseRPCFileName = sigInput+model.name+'BX1.root'
    tightRPCFileName = sigInput+model.name+'.root'
    if(CutPt=='Std'):
      HSCPMakeScaledPredictionsLaunch.SendCluster_Push(
          bgInput+model.name+'_massCut',looseRPCFileName,tightRPCFileName,model.name,model.massCut,model.iasCut,model.ptCut,model.crossSection)
    elif(CutIas=='Std'):
      HSCPMakeScaledPredictionsLaunch.SendCluster_Push(
          bgInput+model.name+'_massCut',looseRPCFileName,tightRPCFileName,model.name,model.massCut,model.iasCut,50,model.crossSection)
    else:
      HSCPMakeScaledPredictionsLaunch.SendCluster_Push(
          bgInput+model.name+'_massCut',looseRPCFileName,tightRPCFileName,model.name,model.massCut,0.1,50,model.crossSection)

  HSCPMakeScaledPredictionsLaunch.SendCluster_Submit()


def doLimits():
  printConfiguration()
  JobName = "doLimits_"
  if(BayesianLimit):
    HSCPDoLimitsLaunch.SendCluster_Create(FarmDirectory, JobName, IntLumi, BaseMacroBayesian, True, RunCondor, QueueName)
  else:
    HSCPDoLimitsLaunch.SendCluster_Create(FarmDirectory, JobName, IntLumi, BaseMacro, False, RunCondor, QueueName)
  for model in modelList:
    fileExists = checkForScaledPredictions(model.name)
    if not fileExists:
      return
    else:
      HSCPDoLimitsLaunch.SendCluster_Push(bgInput,sigInput+model.name+'.root',model.massCut,model.iasCut,model.ptCut)

  HSCPDoLimitsLaunch.SendCluster_Submit()


def doSignificance():
  printConfiguration()
  JobName = "doSignificance_"
  poiList = [6e-3,5e-3,4e-3,3e-3,2e-3,1e-3]
  inputWorkspaceFile = 'hscp_combined_hscp_model.root'
  inputWorkspacePathBeg = os.getcwd()+'/'+FarmDirectory+'/outputs/makeScaledPredictions/'
  HSCPDoSignificanceLaunch.SendCluster_Create(FarmDirectory, JobName, BaseMacroSignificance, ThrowToysSignificance, RunCondor, QueueName)
  for model in modelList:
   fileExists = checkForScaledPredictions(model.name)
   if not fileExists:
     return
   else:
     for poi in poiList:
       #HSCPDoSignificanceLaunch.SendCluster_Push(bgInput,sigInput+model.name+'.root',model.massCut,model.iasCut,
       #  model.ptCut,model.crossSection)
       #HSCPDoSignificanceLaunch.SendCluster_Push('GMStau100',os.getcwd()+'/'+FarmDirectory+'/outputs/makeScaledPredictions/GMStau100_Significance/hscp_combined_hscp_model.root',poi)
       HSCPDoSignificanceLaunch.SendCluster_Push(model.name,inputWorkspacePathBeg+model.name+'_Significance/'+inputWorkspaceFile,poi)

  HSCPDoSignificanceLaunch.SendCluster_Submit()

def combineHypoTestResults():
  doSignificanceOutputDir = FarmDirectory+'/outputs/doSignificance'
  poiList = [6e-3,5e-3,4e-3,3e-3,2e-3,1e-3]
  for poi in poiList:
    doSignificanceMerge = 'combineHypoTestResults'
    doSignificanceMerge.append(doSignificanceOutputDir)
    doSignificanceMerge.append('GMStau100')
    doSignificanceMerge.append(poi)
    #print 'Merging files for ' + model.name + ' poi = ' + poi
    print 'Merging files for GMStau100 poi = ' + poi
    call(doSignificanceMerge)
  

def Usage():
  print 'Usage: Launch.py [arg] where arg can be:'
  print '    0 --> makeHSCParticlePlots (process into RooDataSets)'
  print '    1 --> mergeHSCParticlePlots for data'
  print '    2 --> makeIasPredictions'
  print '    3 --> mergeIasPredictions'
  print '    4 --> makeScaledPredictionPlots (process input for limit-setting macro)'
  print '    5 --> doLimits'
  print '    6 --> significance test using CLs'



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

elif sys.argv[1]=='6':
  print 'Step 6: Calculate significance using CLs'
  doSignificance()
  

else:
  print 'Did not understand input.'
  Usage()
  sys.exit()


