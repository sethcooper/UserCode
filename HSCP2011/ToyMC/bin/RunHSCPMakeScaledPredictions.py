#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeScaledPredictionsLaunchOnCondor
import glob
from subprocess import call
import datetime
from SignalDefinitions import *

FarmDirectory = "FARM_moreModels_cutPt50GeV_Feb21"
IntLumi = 4679 # 1/pb (2011)
# subtract the lumi before the RPC trigger change (runs before 165970) = 355.227 /pb
#IntLumi = 4323.773 # 1/pb

# BG
OutputIasPredictionDir = "/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/"
OutputIasPredictionDir+=FarmDirectory
OutputIasPredictionDir+="/outputs/makeIasPredictions/"
bgInput = OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'
# signal
sigInput='/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/'
sigInput+='FARM_MakeHSCParticlePlots_Signals_absEta_ptErrorPresel_Feb21/outputs/makeHSCParticlePlots_Feb21_'

BaseCfg = "makeScaledPredictionHistograms_template_cfg.py"
BaseChXML = "hscp_dataDriven_ch_template.xml"
BaseCombXML = "hscp_dataDriven_template.xml"
BaseMacro = os.getcwd() + "/StandardHypoTestInvDemo.C"


now = datetime.datetime.now()
Date = now.strftime("%b%d")
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

