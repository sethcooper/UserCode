#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunchOnCondor
import glob
from SignalDefinitions import *
from subprocess import call


Date = "feb20"
BaseCfg = "makeIasPredictions_template_cfg.py"
InputDataRootFile = os.getcwd()+'/FARM_MakeHSCParticlePlots_dataWithTightRPCTrig_absEta_ptErrorPresel_feb6/outputs/makeHSCParticlePlots_feb1_Data2011_all.root'

# From final 2011 analysis
# model/mass  Gluino1000f10,Gluino1200f10,Stop600,Stop800,GMStau494, Gluino1100f10
#PtCuts   =   [120,          155,          50,     50,     65,        120]
#IasCuts  =   [0.225,        0.200,        0.325,  0.275,  0.35,      0.225]
#MassCuts =   [540,          600,          360,    450,    300,       570]
# model/mass  Gluino1000f10,Gluino1200f10,Stop600,Stop800,GMStau494, Gluino1100f10
#PtCuts   =   [50,          50,          50,     50,     50,        50]


JobName = "makeIasPredictions_allTracks_allEvents_ptIas_ptPresel_"
JobName+=Date
JobName+="_"
FarmDirectory = "FARM_MakeIasPredictions_moreModels_cutPt50GeV_"
FarmDirectory+=Date

modelList = []
modelList.extend([Gluino300,Gluino400,Gluino500,Gluino600,Gluino700,Gluino800,Gluino900,Gluino1000,Gluino1100,Gluino1200])
modelList.extend([GluinoN300,GluinoN400,GluinoN500,GluinoN600,GluinoN700,GluinoN800,GluinoN900,GluinoN1000,GluinoN1100,GluinoN1200])
modelList.extend([Stop130,Stop200,Stop300,Stop400,Stop500,Stop600,Stop700,Stop800])
modelList.extend([StopN130,StopN200,StopN300,StopN400,StopN500,StopN600,StopN700,StopN800])
modelList.extend([GMStau100,GMStau126,GMStau156,GMStau200,GMStau247,GMStau308,GMStau370,GMStau432,GMStau494])
modelList.extend([PPStau100,PPStau126,PPStau156,PPStau200,PPStau247,PPStau308])

for model in modelList:
  #HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputDataRootFile,
  #                                            BaseCfg, model.massCut, model.ptCut, model.iasCut)
  # Use Pt cut of 50 GeV
  HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputDataRootFile,
                                              BaseCfg, model.massCut, 50, model.iasCut)

HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Submit()

