#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunchOnCondor
import glob
from subprocess import call


Date = "jan26"
DataDirBase = "/local/cms/user/cooper/data/hscp/428/makeHSCParticlePlotsOutput"
BaseCfg = "makeIasPredictions_template_cfg.py"
InputRootFile = DataDirBase+"/MakeHSCParticlePlots_2011data_jan13/outputs/makeHSCParticlePlots_jan13_Data2011.root"
#InputRootFile = os.getcwd()+'/MakeHSCParticlePlots_dec22/outputs/makeHSCParticlePlots_dec22_EPSdata.root'
# MassCuts from EPS AN note
#MassCuts = [30,50,80,110,120,150,180,210,220,220,230,240,250,270,280,290,300,330,340,350,400,410,440]
# PtCuts from EPS AN note
#PtCuts =   [50,50,40,40, 40, 50, 120,60, 50, 60, 60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40]

# From final 2011 analysis
# model/mass  Gluino600f10,Gluino1000f10,GMStau100,GMStau200,GMStau308,GMStau432,Stop200,Stop600
#MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]
#PtCuts   =   [50,          120,          65,       70,       70,       55,       60]#,     50]
#IasCuts  =   [0.325,       0.225,        0.400,    0.400,    0.400,    0.375,    0.400]#,  0.325]
# model/mass  Gluino1000f10,Gluino1200f10,Stop600,Stop800,GMStau494
MassCuts =   [540,          600,          360,    450,    300]
IasCuts  =   [0.225,        0.200,        0.325,  0.275,  0.35]
PtCuts   =   [120,          155,          50,     50,     65]

JobName = "makeIasPredictions_oneTrackPerEvent_ptIas_"
JobName+=Date
JobName+="_"
FarmDirectory = "FARM_MakeIasPredictions_oneTrackPerEvent_ptIas_"
FarmDirectory+=Date

HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,InputRootFile,
                                            BaseCfg, MassCuts, PtCuts, IasCuts)
HSCPMakeIasPredictionsLaunchOnCondor.SendCluster_Submit()

