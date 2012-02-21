#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunchOnCondor
import glob
from SignalDefinitions import *
from subprocess import call
import datetime

FarmDirectory = "FARM_moreModels_cutPt50GeV_"
InputDataRootFile = os.getcwd()+'/FARM_MakeHSCParticlePlots_dataWithTightRPCTrig_absEta_ptErrorPresel_feb6/outputs/makeHSCParticlePlots_feb1_Data2011_all.root'

now = datetime.datetime.now()
Date = now.strftime("%b%d")
BaseCfg = "makeIasPredictions_template_cfg.py"

JobName = "makeIasPredictions_allTracks_allEvents_ptIas_ptPresel_"
JobName+=Date
JobName+="_"
FarmDirectory+=Date

#TODO in the real analysis we will use the same ih/pt cuts for sidebands
#     so make a set of the mass cuts (no duplicates)
for model in modelList:
  #HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputDataRootFile,
  #                                            BaseCfg, model.massCut, model.ptCut, model.iasCut)
  # Use Pt cut of 50 GeV
  HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputDataRootFile,
                                              BaseCfg, model.massCut, 50, model.iasCut)

HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Submit()

