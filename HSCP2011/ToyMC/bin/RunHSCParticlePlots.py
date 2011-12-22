#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCParticlePlotsLaunch
import glob
from subprocess import call


BaseCfg = "makeHSCParticlePlots_template_cfg.py"
Date = "dec22"
JobsName = "makeHSCParticlePlots_"
JobsName+=Date
JobsName+="_"
FarmDirBeg = "MakeHSCParticlePlots_"
FarmDirBeg+=Date

# initialize dirs and shell file
HSCParticlePlotsLaunch.CreateDirectoryStructure(FarmDirBeg,JobsName)

# DATA
EPSinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_160404_163869.root',\n"
EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_165001_166033.root',\n"
EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166034_166500.root',\n"
EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166894_167151.root',\n"
EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166501_166893.root',\n"
EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_167153_167913.root',\n"
EPScrossSection = 0.0
EPSmassCut = 0.0
EPSisMC = "False"
EPSisHSCP = "False"
HSCParticlePlotsLaunch.createJob("EPSdata", BaseCfg, EPSinputFiles, EPScrossSection,
                    EPSmassCut, EPSisMC, EPSisHSCP)

# SIGNAL MC
HSCPisMC = "True"
HSCPisHSCP = "True"
# GMSB STAU 100
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/GMStau100.root'\n"
HSCPCrossSection=1.3398 # pb
HSCPmassCut=30.0
HSCPName="GMStau100"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GMSB STAU 200
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/GMStau200.root'\n"
HSCPCrossSection=0.0118093
HSCPmassCut=120.0
HSCPName="GMStau200"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GMSB STAU 308
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/GMStau308.root'\n"
HSCPCrossSection=0.00098447
HSCPmassCut=180.0
HSCPName="GMStau308"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GMSB STAU 432
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/GMStau432.root'\n"
HSCPCrossSection=0.000141817
HSCPmassCut=250.0
HSCPName="GMStau432"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# STOP 200
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Stop200.root'\n"
HSCPCrossSection=13.000000
HSCPmassCut=120.0
HSCPName="Stop200"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# STOP 600
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Stop600.root'\n"
HSCPCrossSection=0.012500
HSCPmassCut=340.0
HSCPName="Stop600"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GLUINO 600
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Gluino600.root'\n"
HSCPCrossSection=0.693000
HSCPmassCut=330.0
HSCPName="Gluino600"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GLUINO 1000
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Gluino1000.root'\n"
HSCPCrossSection=0.0098700
HSCPmassCut=410.0
HSCPName="Gluino1000"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)




# run them (serially)
HSCParticlePlotsLaunch.runJobs()
