#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCParticlePlotsLaunch
import glob
from subprocess import call


BaseCfg = "makeHSCParticlePlots_template_cfg.py"
Date = "jan9"
JobsName = "makeHSCParticlePlots_"
JobsName+=Date
JobsName+="_"
FarmDirBeg = "MakeHSCParticlePlots_signal_"
FarmDirBeg+=Date

# initialize dirs and shell file
HSCParticlePlotsLaunch.CreateDirectoryStructure(FarmDirBeg,JobsName)

# EPS DATA
#EPSinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_160404_163869.root',\n"
#EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_165001_166033.root',\n"
#EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166034_166500.root',\n"
#EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166894_167151.root',\n"
#EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166501_166893.root',\n"
#EPSinputFiles+="   'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_167153_167913.root',\n"
#EPScrossSection = 0.0
#EPSmassCut = 0.0
#EPSisMC = "False"
#EPSisHSCP = "False"
#HSCParticlePlotsLaunch.createJob("EPSdata", BaseCfg, EPSinputFiles, EPScrossSection,
#                    EPSmassCut, EPSisMC, EPSisHSCP)

## 2011 data
#DataInputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_160404_163869.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_160404_163869.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_165001_166033.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_166034_166500.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_166501_166893.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_166894_167151.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_167153_167913.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_170826_171500.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_171501_172619.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172620_172790.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172791_172802.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172803_172900.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172901_173243.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_173244_173692.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_175860_176099.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_176100_176309.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_176467_176800.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_176801_177053.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_177074_177783.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_177788_178380.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_178420_179411.root',\n"
#DataInputFiles+="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_179434_180252.root',\n"
#DataCrossSection = 0.0
#DataMassCut = 0.0
#DataIsMC = "False"
#DataIsHSCP = "False"
#HSCParticlePlotsLaunch.createJob("Data2011", BaseCfg, DataInputFiles, DataCrossSection,
#                    DataMassCut, DataIsMC, DataIsHSCP)

## SIGNAL MC
HSCPisMC = "True"
HSCPisHSCP = "True"
# GMSB STAU 100
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/GMStau100BX1.root'\n"
HSCPCrossSection=1.3398 # pb
HSCPmassCut=30.0
HSCPName="GMStau100"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GMSB STAU 200
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/GMStau200BX1.root'\n"
HSCPCrossSection=0.0118093
HSCPmassCut=120.0
HSCPName="GMStau200"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
## GMSB STAU 308
#HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/GMStau308BX1.root'\n"
#HSCPCrossSection=0.00098447
#HSCPmassCut=180.0
#HSCPName="GMStau308"
#HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
#                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
## GMSB STAU 432
#HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/GMStau432BX1.root'\n"
#HSCPCrossSection=0.000141817
#HSCPmassCut=250.0
#HSCPName="GMStau432"
#HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
#                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# STOP 200
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Stop200BX1.root'\n"
HSCPCrossSection=13.000000
HSCPmassCut=120.0
HSCPName="Stop200"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# STOP 600
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Stop600BX1.root'\n"
HSCPCrossSection=0.012500
HSCPmassCut=340.0
HSCPName="Stop600"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GLUINO 600
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Gluino600BX1.root'\n"
HSCPCrossSection=0.693000
HSCPmassCut=330.0
HSCPName="Gluino600"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)
# GLUINO 1000
HSCPinputFiles="    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Gluino1000BX1.root'\n"
HSCPCrossSection=0.0098700
HSCPmassCut=410.0
HSCPName="Gluino1000"
HSCParticlePlotsLaunch.createJob(HSCPName, BaseCfg, HSCPinputFiles, HSCPCrossSection,
                    HSCPmassCut, HSCPisMC, HSCPisHSCP)




# run them (serially)
HSCParticlePlotsLaunch.runJobs()
