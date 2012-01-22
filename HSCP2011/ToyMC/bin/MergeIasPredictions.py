#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from subprocess import call

OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_jan6/"
OutputIasPredictionDir+="/outputs/"

#MassCuts = [30,50,80,110,120,150,180,210,220,230,240,250,270,280,290,300,330,340,350,400,410,440]
MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]

# testing
#call(["ls","-l",os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_dec30"])
#call(["ls","/afs/cern.ch/user/s/scooper/scratch0/cmssw/MakeIasPredictionsBatch428/src/HSCP2011/ToyMC/bin/FARM_MakeIasPredictions_allNoMforCRegion_dec30/outputs/makeIasPredictions_massCut30_etaMin0.0_nomMin17.root"])

for i in MassCuts:
    #thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(i)+'_eta*')
    thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(i)+'_pt*')
    thisMassCutFiles.insert(0,OutputIasPredictionDir+"makeIasPredictionsCombined_massCut"+str(i)+".root")
    thisMassCutFiles.insert(0,"hadd")
    #thisMassCutFiles.insert(0,"ls")
    print thisMassCutFiles
    print 'Merging files for mass cut = ' + str(i)
    call(thisMassCutFiles)


