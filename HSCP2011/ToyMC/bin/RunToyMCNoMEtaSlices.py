#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCPToyMCLaunchOnLxBatch
import glob
from subprocess import call


#if len(sys.argv)==1 or len(sys.argv[1])==0:
if len(sys.argv[1])==0:
        print "Usage:"
        print "  RunIasNoMEtaSlicedSBDists.py nTrials"
        #print "  RunIasNoMEtaSlicedSBDists.py backgroundRootFile signalRootFile nTrials nSigTracks nBackgroundTracksD backgroundOnly"
        #print "    where backgroundRootFile and signalRootFile contain the ias predictions"
        print "          nTrials is the number of trials to use"
        #print "          nSigTracks is the number of signal tracks after skim/preselections"
        #print "          nBackgroundTracksD is the number of background tracks after skim/preselection in the D (search) region"
        #print "          backgroundOnly is either True or False, whether or not to generate background-only samples"
        sys.exit()

else:
        QueueName = "1nh"
        #QueueName = "8nm"
        JobName = "toyMC_nov9_"
        FarmDirectory = "FARM_ToyMC_nov9_11signalTracks_10799backgroundTracks"
        backOnly = "False"
        #FarmDirectory = "FARM_ToyMC_nov9_0signalTracks_10799backgroundTracks"
        #backOnly = "True"

        BaseCfg = "doToyMC_cfg.py"
        #bgRootFile = sys.argv[1]
        #sigRootFile = sys.argv[2]
        #nTrials = sys.argv[3]
        #nSigTracks = sys.argv[4]
        #nBackgroundTracksD = sys.argv[5]
        #backOnly = sys.argv[6]
        bgRootFile = "1.nov9.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"
        sigRootFile = "nov1.withRooDataSet.makeHSCParticlePlots.GMstau200.root"
        nTrials = sys.argv[1]
        nSigTracks = 11
        nBackgroundTracksD = 10799

        HSCPToyMCLaunchOnLxBatch.SendCluster_Create(FarmDirectory,JobName,bgRootFile,sigRootFile,nTrials,
                                                    nSigTracks,nBackgroundTracksD,backOnly,QueueName,BaseCfg)
        HSCPToyMCLaunchOnLxBatch.SendCluster_Submit()

