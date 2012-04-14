#!/usr/bin/env python

import urllib
import string
import os
import sys
import HSCParticlePlotsLaunch
import glob
from subprocess import call
import datetime
from SignalDefinitions import *


FarmDirBeg = "FARM_MakeHSCParticlePlots_Signals_NewLumi_"
now = datetime.datetime.now()
Date = now.strftime("%b%d")
BaseCfg = "makeHSCParticlePlots_template_cfg.py"
JobsName = "makeHSCParticlePlots_"
JobsName+=Date
JobsName+="_"
FarmDirBeg+=Date

# initialize dirs and shell file
HSCParticlePlotsLaunch.SendCluster_Create(FarmDirBeg,JobsName)

## 2011 data
#DataCrossSection = 0.0
#DataMassCut = 0.0
#DataIsMC = "False"
#DataIsHSCP = "False"
#DataInputFiles=[]
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_160404_163869.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_165001_166033.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_166034_166500.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_166501_166893.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_166894_167151.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_167153_167913.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_170826_171500.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_171501_172619.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172620_172790.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172791_172802.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172803_172900.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_172901_173243.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_173244_173692.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_175860_176099.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_176100_176309.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_176467_176800.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_176801_177053.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_177074_177783.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_177788_178380.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_178420_179411.root'")
#DataInputFiles.append("    'file:/hdfs/cms/user/cooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Nov11/Data_RunA_179434_180252.root'")
#for index,item in enumerate(DataInputFiles):
#  HSCParticlePlotsLaunch.SendCluster_Push("Data2011_"+str(index), BaseCfg, item, DataCrossSection,
#                      DataMassCut, DataIsMC, DataIsHSCP)


# SIGNAL MC
HSCPisMC = "True"
HSCPisHSCP = "True"
IntLumiBefTrigChange = 352.067 #Total luminosity taken before RPC L1 trigger change (went into effect on run 165970)
IntLumi = 4976 # 1/pb (2011, new pixel measurement)
#
for model in modelList:
  fileName = SignalBasePath+model.name+"BX1.root'"
  HSCParticlePlotsLaunch.SendCluster_Push(model.name+'BX1',BaseCfg,fileName,
                                          model.crossSection,model.massCut,
                                          IntLumi,IntLumiBefTrigChange,HSCPisMC,HSCPisHSCP)
  fileName = SignalBasePath+model.name+".root'"
  HSCParticlePlotsLaunch.SendCluster_Push(model.name,BaseCfg,fileName,
                                          model.crossSection,model.massCut,
                                          IntLumi,IntLumiBefTrigChange,HSCPisMC,HSCPisHSCP)



HSCParticlePlotsLaunch.SendCluster_Submit()
