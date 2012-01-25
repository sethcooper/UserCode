#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from subprocess import call

#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_jan6/"
OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_oneTrackPerEvent_jan21"
OutputIasPredictionDir+="/outputs/"

#MassCuts = [30,50,80,110,120,150,180,210,220,230,240,250,270,280,290,300,330,340,350,400,410,440]
#MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]
# 2011 data
MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]
PtCuts   =   [50,          120,          65,       70,       70,       55,       60]#,     50]

# testing
#call(["ls","-l",os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_dec30"])
#call(["ls","/afs/cern.ch/user/s/scooper/scratch0/cmssw/MakeIasPredictionsBatch428/src/HSCP2011/ToyMC/bin/FARM_MakeIasPredictions_allNoMforCRegion_dec30/outputs/makeIasPredictions_massCut30_etaMin0.0_nomMin17.root"])

for i,massCut in enumerate(MassCuts):
    #thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(i)+'_eta*')
    thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(massCut)+'_pt'+str(PtCuts[i])+'_eta*')
    thisMassCutFiles.insert(0,OutputIasPredictionDir+"makeIasPredictionsCombined_massCut"+str(massCut)+"_ptCut"+str(PtCuts[i])+".root")
    thisMassCutFiles.insert(0,"hadd")
    #thisMassCutFiles.insert(0,"ls")
    print thisMassCutFiles
    print 'Merging files for mass cut = ' + str(massCut) + ' pt cut = ' + str(PtCuts[i])
    call(thisMassCutFiles)


