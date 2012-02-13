#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from subprocess import call

#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_jan6/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_oneTrackPerEvent_jan21"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_oneTrackPerEvent_ptIas_jan26/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_oneTrackPerEvent_ptIas_jan31/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allTracks_newPRange_ptIas_ih3_feb2/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allTracks_allEvents_ptIas_ih3_pt45_sameBinsfeb3/"
#OutputIasPredictionDir = os.getcwd()+"/FARM_MakeIasPredictions_allTracks_allEvents_ptIas_ih3_pt45_sameBins_ptPresel_absEta_feb6/"
OutputIasPredictionDir = os.getcwd()
#OutputIasPredictionDir+="/FARM_MakeIasPredictions_allTracks_allEvents_pt50Ias0p1_ih3_pt45_sameBins_ptPresel_absEta_feb7/"
#OutputIasPredictionDir+="/FARM_MakeIasPredictions_allTracks_allEvents_sameCutsUpperPcut__ptPresel_allSlices_feb9/"
#OutputIasPredictionDir+="/FARM_MakeIasPredictions_allTracks_allEvents_upperPtcut_ptPresel_allSlices_feb9/"
#OutputIasPredictionDir+="/FARM_MakeIasPredictions_allTracks_allEvents_noUpperPcutC_ptPresel_allSlices_feb10"
#OutputIasPredictionDir+="/FARM_MakeIasPredictions_allTracks_allEvents_noUpperPcutC_ptPresel_allSlices_loosePtSB_feb11"
OutputIasPredictionDir+="/FARM_MakeIasPredictions_allTracks_allEvents_upperPcutC_ptPresel_allSlices_loosePtSB_feb11"
OutputIasPredictionDir+="/outputs/"

#MassCuts = [30,50,80,110,120,150,180,210,220,230,240,250,270,280,290,300,330,340,350,400,410,440]
#MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]
# 2011 data
#MassCuts =   [360,         540,          20,       120,      190,      260,      130]#,    360]
#PtCuts   =   [50,          120,          65,       70,       70,       55,       60]#,     50]
# model/mass  Gluino1000f10,Gluino1200f10,Stop600,Stop800,GMStau494,Gluino1100f10
MassCuts =   [540,          600,          360,    450,    300,      570]
IasCuts  =   [0.225,        0.200,        0.325,  0.275,  0.35,     0.225]
#PtCuts   =   [120,          155,          50,     50,     65,       120]
PtCuts   =   [50,          50,          50,     50,     50,       50]

# Gluino1100f10
#PtCuts = [120]
#IasCuts = [0.225]
#MassCuts = [570]
# model/mass  Gluino600f10,Gluino1000f10,GMStau100,GMStau200,GMStau308,GMStau432,Stop200,Gluino1100f10,Gluino1200f10
#PtCuts =      [50,          50,           50,       50,       50,       50,       50,    50,           50]
#IasCuts =     [0.1,         0.1,          0.1,      0.1,      0.1,      0.1,      0.1,   0.1,          0.1]
#MassCuts =    [360,         540,          20,       120,      190,      260,      130,   570,          600]

# testing
#call(["ls","-l",os.getcwd()+"/FARM_MakeIasPredictions_allNoMforCRegion_dec30"])
#call(["ls","/afs/cern.ch/user/s/scooper/scratch0/cmssw/MakeIasPredictionsBatch428/src/HSCP2011/ToyMC/bin/FARM_MakeIasPredictions_allNoMforCRegion_dec30/outputs/makeIasPredictions_massCut30_etaMin0.0_nomMin17.root"])

for i,massCut in enumerate(MassCuts):
    #thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(i)+'_eta*')
    thisMassCutFiles = glob.glob(OutputIasPredictionDir+'*massCut'+str(massCut)+'_pt'+str(PtCuts[i])+'_ias'+str(IasCuts[i])+'_eta*')
    thisMassCutFiles.insert(0,OutputIasPredictionDir+'makeIasPredictionsCombined_massCut'+str(massCut)+'_ptCut'+str(PtCuts[i])+'_ias'+str(IasCuts[i])+'.root')
    thisMassCutFiles.insert(0,"hadd")
    #thisMassCutFiles.insert(0,"ls")
    #print thisMassCutFiles
    print 'Merging files for mass cut = ' + str(massCut) + ' pt cut = ' + str(PtCuts[i]) + ' ias cut = ' + str(IasCuts[i])
    call(thisMassCutFiles)

