#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPDoLimitsLaunchOnLxBatch
import glob
from subprocess import call


Date = "dec28"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_dec23/outputs/"
OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_dec28/outputs/"
OutputMakeHSCParticlePlotsDir = os.getcwd()+"/MakeHSCParticlePlots_dec22/outputs/"

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
MassCuts = [230,330,400,410,        220,270,300,300,         120,240,340,440,         110,210,280,350,         30,50,80,120,150,180,220,250,290]
IasCuts = [0.375,0.300,0.275,0.250, 0.375,0.350,0.325,0.325, 0.275,0.350,0.300,0.250, 0.300,0.375,0.325,0.300, 0.375,0.375,0.350,0.275,0.250,0.400,0.375,0.350,0.325]
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
bgInput = OutputIasPredictionDir+'makeIasPredictions_massCut'
sigInput = OutputMakeHSCParticlePlotsDir+'makeHSCParticlePlots_dec22_'
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau100.root',MassCuts[16],IasCuts[16])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau200.root',MassCuts[19],IasCuts[19])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau308.root',MassCuts[21],IasCuts[21])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'GMStau432.root',MassCuts[23],IasCuts[23])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Stop200.root',MassCuts[8],IasCuts[8])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Stop600.root',MassCuts[10],IasCuts[10])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Gluino600.root',MassCuts[1],IasCuts[1])
HSCPDoLimitsLaunchOnLxBatch.SendCluster_Push(bgInput,sigInput+'Gluino1000.root',MassCuts[3],IasCuts[3])


HSCPDoLimitsLaunchOnLxBatch.SendCluster_Submit()

