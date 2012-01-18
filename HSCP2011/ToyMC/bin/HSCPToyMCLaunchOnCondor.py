#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from decimal import *

CopyRights  = '########################################\n'
CopyRights += '#    HSCPToyMCLaunchOnCondor script    #\n'
CopyRights += '#        seth.cooper@cern.ch           #\n'
CopyRights += '#             Nov 2011                 #\n'
CopyRights += '#     modified from script by Loic     #\n'
CopyRights += '########################################\n'

Path_Cfg          = ''
Jobs_Count        = 0
Jobs_Name         = ''
Jobs_Index        = ''
Jobs_NEvent       =-1
Jobs_Inputs       = []
Jobs_FinalCmds    = []
Total_Events      = 0
Config_Template   = ''
Events_Job        = 0
Num_Jobs          = 0
Jobs_NumFiles     = 0
#
Farm_Directories  = []
Path_Cmd          = ''
Path_Shell        = ''
Path_Log          = ''
Background_RootFile  = ''
Signal_RootFile      = ''
NTrials          = 0
NSigTracks = 0
NBackTracksD = 0
Base_Cfg = ''


def CreateTheConfigFile(eta,nom):
    global Jobs_Name
    global Jobs_Index
    global Jobs_Count
    global Path_Cfg
    global CopyRights
    global Background_RootFile
    global Signal_RootFile
    global NTrials
    global Base_cfg
    global NBackTracksD
    global NSigTracks
    #global BackgroundOnly
    Path_Cfg   = Farm_Directories[1]+Jobs_Name+'nom'+`nom`+'eta'+`int(eta*10)`+'_cfg.py'
    outputFile = 'toyMCoutput_' + `NBackTracksD` + 'bgTracks_'
    lastPart = `NSigTracks` + 'sigTracks_NoM' + `nom` + 'eta' + `int(eta*10)` + '.root'
    #if BackgroundOnly=="True":
    #    lastPart = '0sigTracks_NoM' + `nom` + 'eta' + `int(eta*10)` + '.root'
    outputFile+=lastPart
    config_file=open(Base_Cfg,'r')
    config_txt   = '\n\n' + CopyRights + '\n\n'
    config_txt  += config_file.read()
    config_file.close()

	#Default Replacements
    config_txt = config_txt.replace("XXX_BACKGROUNDINPUTFILE_XXX", Background_RootFile)
    config_txt = config_txt.replace("XXX_SIGNALINPUTFILE_XXX", Signal_RootFile)
    #config_txt = config_txt.replace("XXX_PATH_XXX"		,os.getcwd())
    config_txt = config_txt.replace("XXX_OUTPUT_XXX"        , outputFile)
    config_txt = config_txt.replace("XXX_NBACKTRACKSD_XXX"        ,`NBackTracksD`)
    config_txt = config_txt.replace("XXX_NSIGTRACKS_XXX"      , `NSigTracks`) 
    config_txt = config_txt.replace("XXX_LOWERNOM_XXX"        , `nom`)
    config_txt = config_txt.replace("XXX_LOWERETA_XXX"        , str(eta))
    config_txt = config_txt.replace("XXX_NUMTRIALS_XXX"       , NTrials)
    #config_txt = config_txt.replace("XXX_BACKGROUNDONLY_XXX"       , BackgroundOnly)

    config_file=open(Path_Cfg,'w')
    config_file.write(config_txt)
    config_file.close()


def CreateTheShellFile(eta,nom):
    global Path_Shell
    global CopyRights
    global Jobs_Name
    #global BackgroundOnly
    global NSigTracks
    global NBackTracksD
    CreateTheConfigFile(eta,nom)
    outputFile = 'toyMCoutput_' + `NBackTracksD` + 'bgTracks_'
    lastPart = `NSigTracks` + 'sigTracks_NoM' + `nom` + 'eta' + `int(eta*10)` + '.root'
    #if BackgroundOnly=="True":
    #    lastPart = '0sigTracks_NoM' + `nom` + 'eta' + `int(eta*10)` + '.root'
    outputFile+=lastPart
    Path_Shell = Farm_Directories[1]+Jobs_Name+'nom'+`nom`+'eta'+`int(eta*10)`+'.sh'
    shell_file=open(Path_Shell,'w')
    shell_file.write('#!/bin/sh\n')
    shell_file.write(CopyRights + '\n')
    #shell_file.write('export SCRAM_ARCH=slc5_amd64_gcc434\n')
    shell_file.write('source /local/cms/sw/cmsset_default.sh\n')
    shell_file.write('cd ' + os.getcwd() + '\n')
    shell_file.write('eval `scramv1 runtime -sh`\n')
    shell_file.write('cd -\n')
    cfgPath = os.getcwd() + '/' + Path_Cfg
    shell_file.write('cd ' + Farm_Directories[4] + '\n')
    shell_file.write('doToyMC ' + cfgPath + '\n')
    #shell_file.write('doToyMC ' + os.getcwd() + '/'+Path_Cfg + '\n')
    #shell_file.write('mv '+outputFile+' '+os.getcwd()+'/'+Farm_Directories[4]+'\n')
    shell_file.close()
    os.system("chmod 777 "+Path_Shell)

def CreateTheCmdFile():
        global Path_Cmd
        global CopyRights
        global Jobs_Name
        Path_Cmd   = Farm_Directories[0]+Jobs_Name+'submit.sh'
        cmd_file=open(Path_Cmd,'w')
        cmd_file.write(CopyRights + '\n')
        cmd_file.write('Universe                = vanilla\n')
        cmd_file.write('Environment             = CONDORJOBID=$(Process)\n')
        cmd_file.write('notification            = Error\n')
        #cmd_file.write('requirements            = (Memory > 512)&&(CondorGroup=?="cmsfarm")&&(Arch=?="X86_64")\n')
        cmd_file.write('requirements            = (Memory > 512)&&(Arch=?="X86_64")\n')
        cmd_file.write('+CondorGroup            = "cmsfarm"\n')
        cmd_file.write('should_transfer_files   = NO\n')
        cmd_file.write('Notify_user = cooper@physics.umn.edu\n')
        #cmd_file.write('should_transfer_files   = YES\n')
        #cmd_file.write('when_to_transfer_output = ON_EXIT\n')
        cmd_file.close()


def AddJobToCmdFile(eta,nom):
        global Path_Shell
        global Path_Cmd
        global Jobs_Name
        Path_Log   = os.getcwd()+'/'+Farm_Directories[2]+Jobs_Name+'nom'+`nom`+'eta'+`int(eta*10)`
        Path_Error   = os.getcwd()+'/'+Farm_Directories[3]+Jobs_Name+'nom'+`nom`+'eta'+`int(eta*10)`
        cmd_file=open(Path_Cmd,'a')
        cmd_file.write('\n')
        cmd_file.write('Executable              = %s\n'     % Path_Shell)
        cmd_file.write('output                  = %s.out\n' % Path_Log)
        cmd_file.write('error                   = %s.err\n' % Path_Error)
        cmd_file.write('log                     = %s.log\n' % Path_Log)
        cmd_file.write('Queue 1\n')
        cmd_file.close()


def CreateDirectoryStructure(FarmDirectory):
    global Jobs_Name
    global Farm_Directories
    Farm_Directories = [FarmDirectory+'/', FarmDirectory+'/inputs/', FarmDirectory+'/logs/',
                        FarmDirectory+'/errors/', FarmDirectory+'/outputs/']
    for i in range(0,len(Farm_Directories)):
        if os.path.isdir(Farm_Directories[i]) == False:
            os.system('mkdir ' + Farm_Directories[i])

def SendCluster_Create(farmDirectory, jobName, backgroundRootFile, signalRootFile,
                       ntrials, nsigtracks, nbacktracksd, baseCfg):
    global Jobs_Name
    global Jobs_Count
    global Background_RootFile
    global Signal_RootFile
    global NTrials
    global NSigTracks
    global NBackTracksD
    global Base_Cfg
    #global BackgroundOnly
    Jobs_Name  = jobName
    Jobs_Count = 0
    Background_RootFile = os.getcwd()+'/'+backgroundRootFile
    Signal_RootFile = os.getcwd()+'/'+signalRootFile
    NTrials = ntrials
    NSigTracks = nsigtracks
    NBackTracksD = nbacktracksd
    #BackgroundOnly = backgroundOnly
    Base_Cfg = baseCfg
    CreateDirectoryStructure(farmDirectory)
    CreateTheCmdFile()
    for etaIndex in xrange(0,12):
        for nom in xrange(5,21,2):
            SendCluster_Push(Decimal(etaIndex)*Decimal('0.2'),nom)


def SendCluster_Push(eta,nom):
    global Jobs_Count
    global Jobs_Index
    Jobs_Index = "%04i" % Jobs_Count
    CreateTheShellFile(eta,nom)
    AddJobToCmdFile(eta,nom)
    Jobs_Count = Jobs_Count+1

def SendCluster_Submit():
    global CopyRights
    global Jobs_Count
    global Path_Cmd
    os.system("condor_submit " + Path_Cmd)
    print '\n'+CopyRights
    print '%i job(s) has/have been submitted to condor' % Jobs_Count

