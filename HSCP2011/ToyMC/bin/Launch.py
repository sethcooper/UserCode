#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunchOnCondor
import HSCPMakeScaledPredictionsLaunchOnCondor
import HSCPDoLimitsLaunchOnCondor
import glob
from SignalDefinitions import *
from subprocess import call
import datetime

now = datetime.datetime.now()
Date = now.strftime("%b%d")

# Set things here
AllSlices = True
FarmDirectory = 'FARM_moreModels_allData_cutPt50GeVIas0p1_'
if(AllSlices):
  FarmDirectory+='allSlices_'
else:
  FarmDirectory+='oneSlice_'
FarmDirectory+=Date
InputDataRootFile = '/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/'
InputDataRootFile+='/FARM_MakeHSCParticlePlots_dataWithTightRPCTrig_absEta_ptErrorPresel_feb6/outputs/makeHSCParticlePlots_feb1_Data2011_all.root'
#InputDataRootFile+='/FARM_MakeHSCParticlePlots_data_absEta_ptErrorPresel_Mar08/outputs/makeHSCParticlePlots_Data2011_all.root'
sigInput='/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/'
sigInput+='FARM_MakeHSCParticlePlots_Signals_absEta_ptErrorPresel_Feb21/outputs/makeHSCParticlePlots_Feb21_'
IntLumi = 4679 # 1/pb (2011)
# subtract the lumi before the RPC trigger change (runs before 165970) = 355.227 /pb
#IntLumi = 4323.773 # 1/pb
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


def doIasPredictions():
  #TODO in the real analysis we will use the same ih/pt cuts for sidebands
  #     so make a set of the mass cuts (no duplicates)
  JobName = "makeIasPredictions_"
  for model in modelList:
    HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName+model.name+"_",InputDataRootFile,
                                                #BaseCfgMakePred, model.massCut, model.ptCut, model.iasCut, AllSlices)
    # Use Pt cut of 50 GeV, ias 0.1
                                                BaseCfgMakePred, model.massCut, 50, 0.1, AllSlices)
    # Use Pt cut of 50 GeV, std ana ias
                                                #BaseCfgMakePred, model.massCut, 50, model.iasCut, AllSlices)

  HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Submit()


def doMergeIasPredictions():
  #for model in modelList:
  #    thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(model.massCut)+'_pt'+str(model.ptCut)+'_ias'+model.iasCut+'_eta*')
  #    thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'+str(model.massCut)+'_ptCut'+str(model.ptCut)+'_ias'+model.iasCut+'.root')
  #    thisMassCutFiles.insert(0,"hadd")
  #    print 'Merging files for mass cut = ' + str(model.massCut) + ' pt cut = ' + str(model.ptCut) + ' ias cut = ' + model.iasCut
  #    call(thisMassCutFiles)
  # all Pt = 50, ias 0.1
  for model in modelList:
      thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(model.massCut)+'_pt50_ias'+str(0.1)+'_eta*')
      thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'+str(model.massCut)+'_ptCut50_ias'+str(0.1)+'.root')
      thisMassCutFiles.insert(0,"hadd")
      print 'Merging files for mass cut = ' + str(model.massCut) + ' pt cut = 50 ias cut = ' + str(0.1)
      call(thisMassCutFiles)
  # all Pt = 50, ias stdAnalysis
  #for model in modelList:
  #    thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*'+model.name+'_massCut'+str(model.massCut)+'_pt50_ias'+model.iasCut+'_eta*')
  #    thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_'+model.name+'_massCut'+str(model.massCut)+'_ptCut50_ias'+model.iasCut+'.root')
  #    thisMassCutFiles.insert(0,"hadd")
  #    print 'Merging files for ' + model.name + ': mass cut = ' + str(model.massCut) + ' pt cut = 50 ias cut = ' + model.iasCut
  #    call(thisMassCutFiles)


def doScaledPredictions():
  # BG
  JobName = "makeScaledPredictions_"
  HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,
                                              IntLumi, BaseCfgMakeScaled, BaseChXML,
                                                           BaseCombXML, BaseMacro)
  for model in modelList:
    HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(
      #bgInput+'massCut',sigInput+model.name+'.root',model.name,model.massCut,model.iasCut,model.ptCut)
      # Pt = 50, ias 0.1
      bgInput+model.name+'_massCut',sigInput+model.name+'.root',model.name,model.massCut,0.1,50)
      #bgInput+'massCut',sigInput+model.name+'.root',model.name,model.massCut,0.1,50)
      # Pt = 50, ias stdAna
      #bgInput+model.name+'_massCut',sigInput+model.name+'.root',model.name,model.massCut,model.iasCut,50)

  HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Submit()


def doLimits():
  JobName = "doLimits_"
  #HSCPDoLimitsLaunchOnCondor.SendCluster_Create(FarmDirectory, JobName, IntLumi, BaseMacro, False)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Create(FarmDirectory, JobName, IntLumi, BaseMacroBayesian, True)
  #TODO work for all signal models
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Stop200.name+'.root',Stop200.massCut,Stop200.iasCut,Stop200.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Stop400.name+'.root',Stop400.massCut,Stop400.iasCut,Stop400.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Stop600.name+'.root',Stop600.massCut,Stop600.iasCut,Stop600.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Gluino600.name+'.root',Gluino600.massCut,Gluino600.iasCut,Gluino600.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Gluino800.name+'.root',Gluino800.massCut,Gluino800.iasCut,Gluino800.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Gluino1000.name+'.root',Gluino1000.massCut,Gluino1000.iasCut,Gluino1000.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Gluino1100.name+'.root',Gluino1100.massCut,Gluino1100.iasCut,Gluino1100.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+Gluino1200.name+'.root',Gluino1200.massCut,Gluino1200.iasCut,Gluino1200.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+GMStau308.name+'.root',GMStau308.massCut,GMStau308.iasCut,GMStau308.ptCut)
  HSCPDoLimitsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+GMStau432.name+'.root',GMStau432.massCut,GMStau432.iasCut,GMStau432.ptCut)

  HSCPDoLimitsLaunchOnCondor.SendCluster_Submit()



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


