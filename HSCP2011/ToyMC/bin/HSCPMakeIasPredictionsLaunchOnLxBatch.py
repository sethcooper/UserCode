#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from decimal import *

CopyRights  = '####################################################\n'
CopyRights += '#  HSCPMakeIasPredictionsLaunchOnLXBatch script    #\n'
CopyRights += '#            seth.cooper@cern.ch                   #\n'
CopyRights += '#                 Dec 2011                         #\n'
CopyRights += '#         modified from script by Loic             #\n'
CopyRights += '####################################################\n'

Path_Cfg          = ''
Jobs_Count        = 0
Jobs_Name         = ''
Jobs_Index        = ''
#
Farm_Directories  = []
Path_Cmd          = ''
Path_Shell        = ''
Path_Log          = ''
Queue_Name        = ''
Base_Cfg = ''
Input_File = ''


def CreateTheConfigFile(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh):
    global Jobs_Name
    global Jobs_Index
    global Jobs_Count
    global Path_Cfg
    global CopyRights
    global Base_cfg
    global Input_File
    Path_Cfg = Farm_Directories[1]+Jobs_Name+'massCut'+`massCut`+'_pt'+str(ptThresh)+'_etaMin'+str(etaMin)+'_nomMin'+`nomMin`+'_cfg.py'
    outputFile = 'makeIasPredictions_'+'massCut'+`massCut`+'_pt'+str(ptThresh)+'_etaMin'+str(etaMin)+'_nomMin'+`nomMin`+'.root'
    config_file=open(Base_Cfg,'r')
    config_txt   = '\n\n' + CopyRights + '\n\n'
    config_txt  += config_file.read()
    config_file.close()

	#Default Replacements
    config_txt = config_txt.replace("XXX_INPUTFILE_XXX", Input_File)
    config_txt = config_txt.replace("XXX_OUTPUTFILE_XXX"        , outputFile)
    config_txt = config_txt.replace("XXX_MASSCUT_XXX"        , `massCut`)
    config_txt = config_txt.replace("XXX_ETAMIN_XXX"        , str(etaMin))
    config_txt = config_txt.replace("XXX_ETAMAX_XXX"        , str(etaMax))
    config_txt = config_txt.replace("XXX_NOMMIN_XXX"        , `nomMin`)
    config_txt = config_txt.replace("XXX_NOMMAX_XXX"        , `nomMax`)
    config_txt = config_txt.replace("XXX_PTTHRESH_XXX"        , str(ptThresh))

    config_file=open(Path_Cfg,'w')
    config_file.write(config_txt)
    config_file.close()


def CreateTheShellFile(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh):
    global Path_Shell
    global CopyRights
    global Jobs_Name
    global Castor_Path
    CreateTheConfigFile(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh)
    outputFile = 'makeIasPredictions_' + 'massCut'+`massCut`+'_pt'+str(ptThresh)+'_etaMin'+str(etaMin)+'_nomMin'+`nomMin`+'.root'
    Path_Shell = Farm_Directories[1]+Jobs_Name+'massCut'+`massCut`+'_pt'+str(ptThresh)+'_etaMin'+str(etaMin)+'_nomMin'+`nomMin`+'.sh'
    shell_file=open(Path_Shell,'w')
    shell_file.write('#!/bin/sh\n')
    shell_file.write(CopyRights + '\n')
    shell_file.write('export SCRAM_ARCH=slc5_amd64_gcc434\n')
    shell_file.write('source /afs/cern.ch/cms/sw/cmsset_default.sh\n')
    shell_file.write('cd ' + os.getcwd() + '\n')
    shell_file.write('eval `scramv1 runtime -sh`\n')
    shell_file.write('cd -\n')
    shell_file.write('makeIasPredictions ' + os.getcwd() + '/'+Path_Cfg + '\n')
    shell_file.write('mv ' + outputFile + ' ' + os.getcwd() + '/' + Farm_Directories[4] )
    shell_file.close()
    os.system("chmod 777 "+Path_Shell)

def CreateTheCmdFile():
        global Path_Cmd
        global CopyRights
        global Jobs_Name
        Path_Cmd   = Farm_Directories[0]+Jobs_Name+'submit.sh'
        cmd_file=open(Path_Cmd,'w')
        cmd_file.write('#!/bin/bash\n')
        cmd_file.write(CopyRights + '\n')
        cmd_file.write('# File listing all bsub commands' + '\n')
        cmd_file.close()

def AddJobToCmdFile(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh):
        global Path_Shell
        global Path_Cmd
        global Queue_Name
        global Jobs_Name
        Path_Log   = os.getcwd()+'/'+Farm_Directories[2]+Jobs_Name+'massCut'+`massCut`+'_pt'+str(ptThresh)+'_etaMin'+str(etaMin)+'_nomMin'+`nomMin`+'.log'
        Path_Error   = os.getcwd()+'/'+Farm_Directories[3]+Jobs_Name+'massCut'+`massCut`+'_pt'+str(ptThresh)+'_etaMin'+str(etaMin)+'_nomMin'+`nomMin`+'.err'
        cmd_file=open(Path_Cmd,'a')
        #cmd_file.write('\n')
        cmd_file.write('bsub -q "%s" '     % Queue_Name)
        cmd_file.write(' -o %s' % Path_Log)
        cmd_file.write(' -e %s ' % Path_Error)
        cmd_file.write(os.getcwd() + '/' + Path_Shell + '\n')
        cmd_file.close()

def CreateDirectoryStructure(FarmDirectory):
    global Jobs_Name
    global Farm_Directories
    Farm_Directories = [FarmDirectory+'/', FarmDirectory+'/inputs/', FarmDirectory+'/logs/',
                        FarmDirectory+'/errors/', FarmDirectory+'/outputs/']
    for i in range(0,len(Farm_Directories)):
        if os.path.isdir(Farm_Directories[i]) == False:
            os.system('mkdir ' + Farm_Directories[i])

def SendCluster_Create(farmDirectory, jobName, inputRootFile, queue, baseCfg,
                       massCuts, ptCuts):
    global Jobs_Name
    global Jobs_Count
    global Queue_Name
    global Input_File
    global Base_Cfg
    Jobs_Name  = jobName
    Jobs_Count = 0
    Queue_Name = queue
    Input_File = inputRootFile
    Base_Cfg = baseCfg
    CreateDirectoryStructure(farmDirectory)
    CreateTheCmdFile()
    #for massCut in massCuts:
    for index, massCut in enumerate(massCuts):
        for etaIndex in range(0,24,4):
            for nomIndex in range(5,22,4):
                SendCluster_Push(massCut,etaIndex/10.0,(etaIndex+4)/10.0,nomIndex,nomIndex+3,ptCuts[index])
                #SendCluster_Push(massCut,0.0,1.2,5,10,ptCuts[index])
                #SendCluster_Push(massCut,1.2,2.4,5,10,ptCuts[index])
                #SendCluster_Push(massCut,0.0,0.2,11,16,ptCuts[index])
                #SendCluster_Push(massCut,0.2,0.4,11,16,ptCuts[index])
                #SendCluster_Push(massCut,0.4,0.6,11,16,ptCuts[index])
                #SendCluster_Push(massCut,0.6,0.8,11,16,ptCuts[index])
                #SendCluster_Push(massCut,0.8,1.2,11,16,ptCuts[index])
                #SendCluster_Push(massCut,1.2,1.6,11,16,ptCuts[index])
                #SendCluster_Push(massCut,1.6,2.4,11,16,ptCuts[index])
                #SendCluster_Push(massCut,0.0,0.6,17,22,ptCuts[index])
                #SendCluster_Push(massCut,0.6,1.2,17,22,ptCuts[index])
                #SendCluster_Push(massCut,1.2,1.8,17,22,ptCuts[index])
                #SendCluster_Push(massCut,1.8,2.4,17,22,ptCuts[index])


def SendCluster_Push(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh):
    global Jobs_Count
    global Jobs_Index
    Jobs_Index = "%04i" % Jobs_Count
    CreateTheShellFile(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh)
    AddJobToCmdFile(massCut,etaMin,etaMax,nomMin,nomMax,ptThresh)
    Jobs_Count = Jobs_Count+1

def SendCluster_Submit():
    global CopyRights
    global Jobs_Count
    global Path_Cmd
    os.system("source " + Path_Cmd)
    print '\n'+CopyRights
    print '%i job(s) has/have been submitted to lxbatch' % Jobs_Count

