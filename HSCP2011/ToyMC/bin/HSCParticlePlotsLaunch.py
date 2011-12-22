#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from decimal import *

CopyRights  = '########################################\n'
CopyRights += '#     HSCParticlePlotsLaunch script    #\n'
CopyRights += '#        seth.cooper@cern.ch           #\n'
CopyRights += '#             Dec 2011                 #\n'
CopyRights += '#     modified from script by Loic     #\n'
CopyRights += '########################################\n'

Path_Cfg          = ''
Jobs_Index        = ''
Jobs_Inputs       = []
Jobs_Name         = ''
Farm_Directories  = []
Path_Log          = ''
Path_Shell        = ''


def CreateTheConfigFile(jobName, baseCfg, inputFiles, crossSection, massCut,
                        isMC, isHSCP):
    global CopyRights
    global Farm_Directories
    global Jobs_Name
    path_Cfg   = Farm_Directories[1]+jobName+'_cfg.py'
    outputFile = Jobs_Name+jobName+'.root'
    config_file=open(baseCfg,'r')
    config_txt   = '\n\n' + CopyRights + '\n\n'
    config_txt  += config_file.read()
    config_file.close()

	#Default Replacements
    config_txt = config_txt.replace("XXX_INPUTFILES_XXX", inputFiles)
    config_txt = config_txt.replace("XXX_OUTPUTFILE_XXX", outputFile)
    config_txt = config_txt.replace("XXX_CROSSSECTION_XXX" , str(crossSection))
    config_txt = config_txt.replace("XXX_MASSCUT_XXX", str(massCut))
    config_txt = config_txt.replace("XXX_ISMC_XXX", isMC)
    config_txt = config_txt.replace("XXX_ISHSCP_XXX", isHSCP)

    config_file=open(path_Cfg,'w')
    config_file.write(config_txt)
    config_file.close()

def AddJobToShellFile(jobName):
    global Path_Shell
    global Jobs_Name
    path_Log   = os.getcwd()+'/'+Farm_Directories[2]+Jobs_Name+jobName+'.log'
    path_Cfg   = os.getcwd()+'/'+Farm_Directories[1]+jobName+'_cfg.py'
    outputFile = Jobs_Name+jobName+'.root'
    print 'Adding job %s to shell file' % jobName
    shell_file=open(Path_Shell,'a')
    shell_file.write('echo "Running job: %s"\n' % jobName)
    shell_file.write('makeHSCParticlePlots %s' % path_Cfg)
    shell_file.write(' >& %s ' % path_Log)
    shell_file.write('\n')
    shell_file.write('mv %s ' % outputFile)
    shell_file.write(os.getcwd()+'/'+Farm_Directories[4])
    shell_file.write('\n')
    shell_file.close()

def CreateTheShellFile():
    global Path_Shell
    global CopyRights
    global Jobs_Name
    Path_Shell = Farm_Directories[0]+Jobs_Name+'drive.sh'
    shell_file=open(Path_Shell,'w')
    shell_file.write('#!/bin/sh\n')
    shell_file.write(CopyRights + '\n')
    shell_file.write('export SCRAM_ARCH=slc5_amd64_gcc434\n')
    shell_file.write('source /afs/cern.ch/cms/sw/cmsset_default.sh\n')
    shell_file.write('cd ' + os.getcwd() + '\n')
    shell_file.write('eval `scramv1 runtime -sh`\n')
    shell_file.write('cd -\n')
    shell_file.close()
    os.system("chmod 777 " + Path_Shell)

def CreateDirectoryStructure(FarmDirectory, jobsName):
    global Farm_Directories
    global Jobs_Name
    Jobs_Name = jobsName
    Farm_Directories = [FarmDirectory+'/', FarmDirectory+'/inputs/', FarmDirectory+'/logs/',
                        FarmDirectory+'/errors/', FarmDirectory+'/outputs/']
    for i in range(0,len(Farm_Directories)):
        if os.path.isdir(Farm_Directories[i]) == False:
            os.system('mkdir ' + Farm_Directories[i])
    CreateTheShellFile()

def createJob(jobName, baseCfg, inputFiles, crossSection,
                    massCut, isMC, isHSCP):
    CreateTheConfigFile(jobName, baseCfg, inputFiles, crossSection, massCut,
                        isMC, isHSCP)
    AddJobToShellFile(jobName)

def runJobs():
    os.system("source " + Path_Shell)

