#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from decimal import *

CopyRights  = '####################################################\n'
CopyRights += '#        HSCPDoLimitsLaunchOnCondor script         #\n'
CopyRights += '#               seth.cooper@cern.ch                #\n'
CopyRights += '#                    Jan 2012                      #\n'
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
Base_Cfg = ''
Int_Lumi = 0
Base_xml_channel = ''
Base_xml_combined = ''


def CreateTheShellFile(bgInputFile,sigInputFile,massCut,iasCut,ptCut,xSecMin,xSecMax,index):
    global Path_Shell
    global CopyRights
    global Jobs_Name
    global Base_macro
    # variables for limit setting
    nPoints = 4 # per job
    nToys = 1000 # per xSec trial point
    # end variables for limit setting
    posLastUndsc = sigInputFile.rfind("_")
    posDotRoot = sigInputFile.rfind(".root")
    signalName = sigInputFile[posLastUndsc+1:posDotRoot]
    outputFile = 'doLimits_'+signalName+'_massCut'+`massCut`+'_ptCut'+`ptCut`+'_index'+str(index)+'.root'
    Path_Shell = Farm_Directories[1]+Jobs_Name+signalName+'_massCut'+`massCut`+'_ptCut'+`ptCut`+'_index'+str(index)+'.sh'
    shell_file=open(Path_Shell,'w')
    shell_file.write('#!/bin/sh\n')
    shell_file.write(CopyRights + '\n')
    #shell_file.write('export SCRAM_ARCH=slc5_amd64_gcc434\n')
    shell_file.write('# use later root\n')
    #shell_file.write('source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh\n')
    shell_file.write('source /local/cms/user/cooper/root/bin/thisroot.sh\n')
    shell_file.write('cd ' + os.getcwd() + '/' + Farm_Directories[4] + signalName + '\n')
    shell_file.write('root -l -b -q "'+Base_macro
        +'(\\"hscp_combined_hscp_model.root\\",\\"combined\\",\\"ModelConfig\\",\\"\\",\\"asimovData\\",0,3,true,'
        +str(nPoints)+','
        +str(xSecMin)+','
        +str(xSecMax)+','
        +str(nToys)+','
        +'false,\\"\\",\\"'
        +outputFile
        +'\\")"'
        + '\n')
    #shell_file.write('mv ' + outputFile + ' ' + os.getcwd() + '/' + Farm_Directories[4] + '\n\n')
    shell_file.close()
    os.system("chmod 777 "+Path_Shell)


def CreateTheCmdFile():
    global Path_Cmd
    global CopyRights
    global Jobs_Name
    global Farm_Directories
    Path_Cmd   = Farm_Directories[0]+Jobs_Name+'submit.sh'
    cmd_file=open(Path_Cmd,'w')
    #cmd_file.write('#!/bin/bash\n')
    cmd_file.write(CopyRights + '\n')
    #cmd_file.write('cd ' + os.getcwd() + Farm_Directories[1] + '\n')
    #cmd_file.write('# init root and prepare histfactory\n')
    #cmd_file.write('source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh\n')
    #cmd_file.write('source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.00/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh\n')
    #cmd_file.write('prepareHistFactory\n')
    #cmd_file.write('cd -\n\n')
    #cmd_file.write('# list all bsub commands' + '\n')
    cmd_file.write('Universe                = vanilla\n')
    cmd_file.write('Environment             = CONDORJOBID=$(Process)\n')
    cmd_file.write('notification            = Error\n')
    cmd_file.write('requirements            = (Memory > 512)&&(Arch=?="X86_64")\n')
    cmd_file.write('+CondorGroup            = "cmsfarm"\n')
    cmd_file.write('should_transfer_files   = NO\n')
    cmd_file.write('Notify_user = cooper@physics.umn.edu\n')
    #cmd_file.write('should_transfer_files   = YES\n')
    #cmd_file.write('when_to_transfer_output = ON_EXIT\n')
    cmd_file.close()

def AddJobToCmdFile(massCut,ptCut,sigInputFile,index):
    global Path_Shell
    global Path_Cmd
    global Jobs_Name
    posLastUndsc = sigInputFile.rfind("_")
    posDotRoot = sigInputFile.rfind(".root")
    signalName = sigInputFile[posLastUndsc+1:posDotRoot]
    Path_Log   = os.getcwd()+'/'+Farm_Directories[2]+Jobs_Name+signalName+'_massCut'+`massCut`+'_ptCut'+`ptCut`+'_index'+str(index)
    Path_Error   = os.getcwd()+'/'+Farm_Directories[3]+Jobs_Name+signalName+'_massCut'+`massCut`+'_ptCut'+`ptCut`+'_index'+str(index)
    cmd_file=open(Path_Cmd,'a')
    cmd_file.write('\n')
    cmd_file.write('Executable              = %s\n'     % Path_Shell)
    cmd_file.write('output                  = %s.out\n' % Path_Log)
    cmd_file.write('error                   = %s.err\n' % Path_Error)
    cmd_file.write('log                     = %s.log\n' % Path_Log)
    cmd_file.write('Queue 1\n')
    #cmd_file.write(os.getcwd() + '/' + Path_Shell + '\n')
    cmd_file.close()

def SendCluster_Create(FarmDirectory, jobName, intLumi, baseMacro):
    global Jobs_Name
    global Jobs_Count
    global Base_macro
    global Int_Lumi
    global Farm_Directories
    Jobs_Name  = jobName
    Jobs_Count = 0
    Base_macro = baseMacro
    Int_Lumi=intLumi
    Farm_Directories = [FarmDirectory+'/', FarmDirectory+'/inputs/', FarmDirectory+'/logs/',
                        FarmDirectory+'/errors/', FarmDirectory+'/outputs/']
    CreateTheCmdFile()


def SendCluster_Push(bgInputFilesBase,sigInputFile,massCut,iasCut,ptCut):
    global Jobs_Count
    global Jobs_Index
    Jobs_Index = "%04i" % Jobs_Count
    bgInputFile = bgInputFilesBase+`massCut`+'_ptCut'+`ptCut`+'.root'
    xSecMin = 0.0001
    xSecMax = 0.05
    nSteps = 100
    stepSize = (xSecMax-xSecMin)/nSteps
    for index in range(0,nSteps):
      xSecMinPt = xSecMin+index*stepSize
      xSecMaxPt = xSecMin+(index+1)*stepSize
      CreateTheShellFile(bgInputFile,sigInputFile,massCut,iasCut,ptCut,xSecMinPt,xSecMaxPt,index)
      AddJobToCmdFile(massCut,ptCut,sigInputFile,index)
      Jobs_Count = Jobs_Count+1

def SendCluster_Submit():
    global CopyRights
    global Jobs_Count
    global Path_Cmd
    os.system("condor_submit " + Path_Cmd)
    print '\n'+CopyRights
    print '%i job(s) has/have been submitted to condor' % Jobs_Count

