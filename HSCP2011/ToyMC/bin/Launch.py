#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunchOnCondor
import HSCPMakeScaledPredictionsLaunchOnCondor
import glob
from SignalDefinitions import *
from subprocess import call
import datetime

# Set things here
FarmDirectory = 'FARM_moreModels_usePtOver50GeVOnly_'
InputDataRootFile = '/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/'
InputDataRootFile+='/FARM_MakeHSCParticlePlots_dataWithTightRPCTrig_absEta_ptErrorPresel_feb6/outputs/makeHSCParticlePlots_feb1_Data2011_all.root'
sigInput='/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/'
sigInput+='FARM_MakeHSCParticlePlots_Signals_absEta_ptErrorPresel_Feb21/outputs/makeHSCParticlePlots_Feb21_'
IntLumi = 4679 # 1/pb (2011)
# subtract the lumi before the RPC trigger change (runs before 165970) = 355.227 /pb
#IntLumi = 4323.773 # 1/pb
#


now = datetime.datetime.now()
Date = now.strftime("%b%d")
FarmDirectory+=Date
OutputIasPredictionDir = os.getcwd()
OutputIasPredictionDir+="/"
OutputIasPredictionDir+=FarmDirectory
OutputIasPredictionDir+="/outputs/makeIasPredictions/"


def doIasPredictions():
  #TODO in the real analysis we will use the same ih/pt cuts for sidebands
  #     so make a set of the mass cuts (no duplicates)
  BaseCfg = "makeIasPredictions_template_cfg.py"
  JobName = "makeIasPredictions_"
  JobName+=Date
  JobName+="_"
  for model in modelList:
    #HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputDataRootFile,
    #                                            BaseCfg, model.massCut, model.ptCut, model.iasCut)
    # Use Pt cut of 50 GeV
    HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputDataRootFile,
                                                BaseCfg, model.massCut, 50, model.iasCut)
    HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Submit()


def doMergeIasPredictions():
  #for model in modelList:
  #    thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(model.massCut)+'_pt'+str(model.ptCut)+'_ias'+str(model.iasCut)+'_eta*')
  #    thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'+str(model.massCut)+'_ptCut'+str(model.ptCut)+'_ias'+str(model.iasCut)+'.root')
  #    thisMassCutFiles.insert(0,"hadd")
  #    print 'Merging files for mass cut = ' + str(model.massCut) + ' pt cut = ' + str(model.ptCut) + ' ias cut = ' + str(model.iasCut)
  #    call(thisMassCutFiles)
  
  # all Pt = 50
  for model in modelList:
      thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(model.massCut)+'_pt50_ias'+str(model.iasCut)+'_eta*')
      thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'+str(model.massCut)+'_ptCut50_ias'+str(model.iasCut)+'.root')
      thisMassCutFiles.insert(0,"hadd")
      print 'Merging files for mass cut = ' + str(model.massCut) + ' pt cut = 50 ias cut = ' + str(model.iasCut)
      call(thisMassCutFiles)


def doScaledPredictions():
  # BG
  bgInput = OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'
  BaseCfg = "makeScaledPredictionHistograms_template_cfg.py"
  BaseChXML = "hscp_dataDriven_ch_template.xml"
  BaseCombXML = "hscp_dataDriven_template.xml"
  BaseMacro = os.getcwd() + "/StandardHypoTestInvDemo.C"
  JobName = "makeScaledPredictions_"
  JobName+=Date
  JobName+="_"
  HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,
                                              IntLumi, BaseCfg, BaseChXML,
                                                           BaseCombXML, BaseMacro)
  for model in modelList:
    HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(
      #bgInput,sigInput+'Gluino1000_28.root',model.massCut,model.iasCut,model.ptCut)
      # Pt = 50
      bgInput,sigInput+model.name+'.root',model.name,model.massCut,model.iasCut,50)

  HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Submit()



# Running the functions

if len(sys.argv)==1:
  print 'Usage: Launch.py [arg] where arg can be:'
  print '    0 --> makeHSCParticlePlots (process into RooDataSets)'
  print '    1 --> makeIasPredictions'
  print '    2 --> mergeIasPredictions'
  print '    3 --> makeScaledPredictionPlots (process input for limit-setting macro)'
  print '    4 --> doLimits'
  sys.exit()

elif sys.argv[1]=='0':
  print 'Step 0: MakeHSCParticlePlots'
  print 'TODO: Implement this'

elif sys.argv[1]=='1':
  print 'Step 1: Make Ias predictions for background'
  doIasPredictions()

elif sys.argv[1]=='2':
  print 'Step 2: Merge Ias predictions'
  doMergeIasPredictions()

elif sys.argv[1]=='3':
  print 'Step 3: Make scaled prediction plots'
  doScaledPredictions()

elif sys.argv[1]=='4':
  print 'Step 4: Compute limits'
  print 'TODO: Implement this'
  

else:
  print 'Did not understand input.'
  print 'Usage: Launch.py [arg] where arg can be:'
  print '    0 --> makeHSCParticlePlots (process into RooDataSets)'
  print '    1 --> makeIasPredictions'
  print '    2 --> mergeIasPredictions'
  print '    3 --> makeScaledPredictionPlots (process input for limit-setting macro)'
  print '    4 --> doLimits'
  sys.exit()


