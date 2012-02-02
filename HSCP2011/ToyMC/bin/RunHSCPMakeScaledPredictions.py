#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeScaledPredictionsLaunchOnCondor
import glob
from subprocess import call


Date = "jan26"
# BG
OutputIasPredictionDir = "/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/"
#OutputIasPredictionDir+="FARM_MakeIasPredictions_oneTrackPerEvent_jan21/outputs/"
OutputIasPredictionDir+="FARM_MakeIasPredictions_oneTrackPerEvent_ptIas_jan26/outputs/"
# signal
#OutputMakeHSCParticlePlotsDir = "/local/cms/user/cooper/data/hscp/428/makeHSCParticlePlotsOutput/MakeHSCParticlePlots_signals_jan14/outputs/"
OutputMakeHSCParticlePlotsDir = "/local/cms/user/cooper/cmssw/428/HSCPAnalysis/src/HSCP2011/ToyMC/bin/MakeHSCParticlePlots_signals_jan26/outputs/"
sigInput = OutputMakeHSCParticlePlotsDir+'makeHSCParticlePlots_jan26_'

#IntLumi = 4679 # 1/pb (2011)
# subtract the lumi before the RPC trigger change (runs before 165970) = 355.227 /pb
IntLumi = 4323.773 # 1/pb
BaseCfg = "makeScaledPredictionHistograms_template_cfg.py"
BaseChXML = "hscp_dataDriven_ch_template.xml"
BaseCombXML = "hscp_dataDriven_template.xml"
BaseMacro = os.getcwd() + "/StandardHypoTestInvDemo.C"

JobName = "makeScaledPredictions_"
JobName+=Date
JobName+="_"
FarmDirectory = "FARM_makeScaledAndDoLimits_oneTrackPerEvent_"
FarmDirectory+=Date

           # Gluino f10/f50         # GluinoN                # Stop                   # StopN                  # GMSTAU
# All 2011
#PtCuts =   [60,40,40,40,             60,40,40,40,            40,40,40,40,             40,60,40,40,             50, 50, 40, 40, 50, 120,50, 40, 40]
# model/mass  Gluino600f10,Gluino1000f10,GMStau100,GMStau200,GMStau308,GMStau432,Stop200,Stop600
#MassCuts =   [360,         540,          20,       120,      190,      260,      130,    360]
#PtCuts   =   [50,          120,          65,       70,       70,       55,       60,     50]
#IasCuts  =   [0.325,       0.225,        0.4,      0.4,      0.4,      0.375,    0.4, 0.325]
#
#Mass =     [400,600,800,1000,       400,600,800,1000,        200,400,600,800,         200,400,600,800,         100,126,156,200,247,308,370,432,494]
#            0   1   2   3           4   5   6   7            8   9   10  11           12  13  14  15          16  17  18  19  20  21  22  23  24

# model/mass  Gluino1000f10,Gluino1200f10,Stop600,Stop800,GMStau494
MassCuts =   [540,          600,          360,    450,    300]
IasCuts  =   [0.225,        0.200,        0.325,  0.275,  0.35]
PtCuts   =   [120,          155,          50,     50,     65]


HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Create(FarmDirectory,JobName,
                                            IntLumi, BaseCfg, BaseChXML,
                                                         BaseCombXML, BaseMacro)

#TODO Do this more intelligently in the future
#listing = os.listdir(OutputMakeHSCParticlePlotsDir)
#for infile in listing:
#        print "makeHSCParticlePlots file is: " + infile
#        if infile.find("data") == -1 or infile.find("Data") == -1:
#            HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push()
# BG, SIG, MASS, IAS
bgInput = OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'
#i=0
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Gluino600.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=1
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Gluino1000.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=2
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'GMStau100.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=3
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'GMStau200.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=4
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'GMStau308.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=5
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'GMStau432.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=6
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Stop200.root',MassCuts[i],IasCuts[i],PtCuts[i])
#i=0
#HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Stop600.root',MassCuts[i],IasCuts[i],PtCuts[i])

i=0
HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Gluino1000.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=1
HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Gluino1200.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=2
HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Stop600.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=3
HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'Stop800.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=4
HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Push(bgInput,sigInput+'GMStau494.root',MassCuts[i],IasCuts[i],PtCuts[i])

HSCPMakeScaledPredictionsLaunchOnCondor.SendCluster_Submit()
