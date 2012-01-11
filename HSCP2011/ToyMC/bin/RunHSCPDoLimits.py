#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPDoLimitsLaunchOnLxBatch
import glob
from subprocess import call


Date = "jan6"
# BG
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_dec23/outputs/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_dec28/outputs/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_dec30/outputs/"
OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_jan6/outputs/"
# signal
#OutputMakeHSCParticlePlotsDir = os.getcwd()+"/MakeHSCParticlePlots_dec22/outputs/"
OutputMakeHSCParticlePlotsDir = os.getcwd()+"/MakeHSCParticlePlots_signal_nov11_jan5/outputs/"
#sigInput = OutputMakeHSCParticlePlotsDir+'makeHSCParticlePlots_dec22_'
sigInput = OutputMakeHSCParticlePlotsDir+'makeHSCParticlePlots_jan5_'

IntLumi = 1091.0 # 1/pb
QueueName = "8nh"
BaseCfg = "makeScaledPredictionHistograms_template_cfg.py"
BaseChXML = "hscp_dataDriven_ch_template.xml"
BaseCombXML = "hscp_dataDriven_template.xml"
BaseMacro = os.getcwd() + "/StandardHypoTestInvDemo.C"

JobName = "doLimits_allNoMforCRegion_"
JobName+=Date
JobName+="_"
FarmDirectory = "FARM_doLimits_allNoMforCRegion_"
FarmDirectory+=Date

           # Gluino f10/f50         # GluinoN                # Stop                   # StopN                  # GMSTAU
# EPS
#MassCuts = [230,330,400,410,        220,270,300,300,         120,240,340,440,         110,210,280,350,         30,50,80,120,150,180,220,250,290]
#IasCuts =  [0.375,0.300,0.275,0.250, 0.375,0.350,0.325,0.325, 0.275,0.350,0.300,0.250, 0.300,0.375,0.325,0.300, 0.375,0.375,0.350,0.275,0.250,0.400,0.375,0.350,0.325]
#PtCuts =   [60,40,40,40,             60,40,40,40,            40,40,40,40,             40,60,40,40,             50, 50, 40, 40, 50, 120,50, 40, 40]
# All 2011
#PtCuts =   [60,40,40,40,             60,40,40,40,            40,40,40,40,             40,60,40,40,             50, 50, 40, 40, 50, 120,50, 40, 40]
# model/mass  Gluino600f50,Gluino1000f50,GMStau100,GMStau200,GMStau308,GMStau432,Stop200,Stop600
MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]
PtCuts   =   [50,          120,          65,       70,       70,       55,       60]#,     50]
IasCuts  =   [0.325,       0.225,        0.4,      0.4,      0.4,      0.375,    0.4]#, 0.325]
#
Mass =     [400,600,800,1000,       400,600,800,1000,        200,400,600,800,         200,400,600,800,         100,126,156,200,247,308,370,432,494]
#            0   1   2   3           4   5   6   7            8   9   10  11           12  13  14  15          16  17  18  19  20  21  22  23  24

HSCPDoLimitsLaunchOnLxBatch.SendCluster_Create(FarmDirectory,JobName,
                                            QueueName, IntLumi, BaseCfg, BaseChXML,
                                                         BaseCombXML, BaseMacro)

#TODO Do this more intelligently in the future
#listing = os.listdir(OutputMakeHSCParticlePlotsDir)
#for infile in listing:
#        print "makeHSCParticlePlots file is: " + infile
#        if infile.find("data") == -1 or infile.find("Data") == -1:
#            HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push()
# BG, SIG, MASS, IAS
bgInput = OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'
i=0
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Gluino600.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=1
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Gluino1000.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=2
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau100.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=3
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau200.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=4
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau308.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=5
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau432.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=6
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Stop200.root',MassCuts[i],IasCuts[i],PtCuts[i])
i=0
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Stop600.root',MassCuts[i],IasCuts[i],PtCuts[i])


HSCPDoLimitsLaunchOnLxBatch.SendCluster_Submit()

