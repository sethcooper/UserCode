#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPMakeIasPredictionsLaunchOnLxBatch
import glob
from subprocess import call


QueueName = "8nh"
BaseCfg = "makeIasPredictions_template_cfg.py"
InputRootFile = os.getcwd()+'/MakeHSCParticlePlots_dec22/outputs/makeHSCParticlePlots_dec22_EPSdata.root'
Date = "dec27"
# MassCuts from EPS AN note
MassCuts = [30,50,80,110,120,150,180,210,220,230,240,250,270,280,290,300,330,340,350,400,410,440]

JobName = "makeIasPredictions_allNoMforCRegion_"
JobName+=Date
JobName+="_"
FarmDirectory = "FARM_MakeIasPredictions_allNoMforCRegion_"
FarmDirectory+=Date

HSCPMakeIasPredictionsLaunchOnLxBatch.SendCluster_Create(FarmDirectory,JobName,InputRootFile,
                                            QueueName, BaseCfg, MassCuts)
HSCPMakeIasPredictionsLaunchOnLxBatch.SendCluster_Submit()

