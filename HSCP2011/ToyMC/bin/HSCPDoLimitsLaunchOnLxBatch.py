#!/usr/bin/env python

import urllib
import string
import os
import sys
import glob
from decimal import *

CopyRights  = '####################################################\n'
CopyRights += '#       HSCPDoLimitsLaunchOnLXBatch script         #\n'
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
Int_Lumi = 0
Base_xml_channel = ''
Base_xml_combined = ''


def CreateTheXMLFiles(signalName,inputFile,massCut):
    global Base_xml_channel
    global Base_xml_combined

    baseXMLchannel_file = open(Base_xml_channel,'r')
    XMLchannel_txt = '\n'
    XMLchannel_txt += baseXMLchannel_file.read()
    baseXMLchannel_file.close()

    # replacements
    XMLchannel_txt = XMLchannel_txt.replace("XXX_INPUTFILE_XXX",os.getcwd()+'/'+Farm_Directories[4]+inputFile)

    chXMLFilename = 'hscp_dataDriven_ch_'+signalName+'_massCut'+`massCut`+'.xml'
    pathXMLFile = Farm_Directories[1]+'config/'+chXMLFilename
    XMLchannel_file = open(pathXMLFile,'w')
    XMLchannel_file.write(XMLchannel_txt)
    XMLchannel_file.close()

    baseXMLcombined_file = open(Base_xml_combined,'r')
    XMLcombined_txt = '\n'
    XMLcombined_txt += baseXMLcombined_file.read()
    baseXMLcombined_file.close()

    # replacements
    XMLcombined_txt = XMLcombined_txt.replace("XXX_INPUTFILE_XXX",'./config/'+chXMLFilename)

    combXMLFilename = 'hscp_dataDriven_'+signalName+'_massCut'+`massCut`+'.xml'
    pathXMLFile = Farm_Directories[1]+'config/'+combXMLFilename
    XMLcombined_file = open(pathXMLFile,'w')
    XMLcombined_file.write(XMLcombined_txt)
    XMLcombined_file.close()


def CreateTheConfigFile(bgInputFile,sigInputFile,massCut,iasCut,ptCut,signalName):
    global Jobs_Name
    global Jobs_Index
    global Jobs_Count
    global Path_Cfg
    global CopyRights
    global Base_cfg
    global Int_Lumi
    Path_Cfg   = Farm_Directories[1]+Jobs_Name+signalName+'_massCut'+`massCut`+'_cfg.py'
    outputFile = 'makeScaledPredictionHistograms_'+signalName+'_massCut'+`massCut`+'.root'
    config_file=open(Base_Cfg,'r')
    config_txt   = '\n\n' + CopyRights + '\n\n'
    config_txt  += config_file.read()
    config_file.close()

	#Default Replacements
    config_txt = config_txt.replace("XXX_BGINPUTFILE_XXX", bgInputFile)
    config_txt = config_txt.replace("XXX_SIGINPUTFILE_XXX", sigInputFile)
    config_txt = config_txt.replace("XXX_OUTPUTFILE_XXX"        , outputFile)
    config_txt = config_txt.replace("XXX_MASSCUT_XXX"        , `massCut`)
    config_txt = config_txt.replace("XXX_IASCUT_XXX"        , str(iasCut))
    config_txt = config_txt.replace("XXX_INTLUMI_XXX"        , str(Int_Lumi))
    config_txt = config_txt.replace("XXX_PTTHRESH_XXX"       , str(ptCut))

    config_file=open(Path_Cfg,'w')
    config_file.write(config_txt)
    config_file.close()


def CreateTheShellFile(bgInputFile,sigInputFile,massCut,iasCut,ptCut):
    global Path_Shell
    global CopyRights
    global Jobs_Name
    global Base_macro
    posLastUndsc = sigInputFile.rfind("_")
    posDotRoot = sigInputFile.rfind(".root")
    signalName = sigInputFile[posLastUndsc+1:posDotRoot]
    outputFile = 'makeScaledPredictionHistograms_'+signalName+'_massCut'+`massCut`+'.root'
    CreateTheConfigFile(bgInputFile,sigInputFile,massCut,iasCut,ptCut,signalName)
    CreateTheXMLFiles(signalName,outputFile,massCut)
    Path_Shell = Farm_Directories[1]+Jobs_Name+signalName+'_massCut'+`massCut`+'.sh'
    combXMLFilename = 'hscp_dataDriven_'+signalName+'_massCut'+`massCut`+'.xml'
    pathCombXMLFile = os.getcwd()+'/'+Farm_Directories[1]+'/config/'+combXMLFilename
    chXMLFilename = 'hscp_dataDriven_ch_'+signalName+'_massCut'+`massCut`+'.xml'
    pathChXMLFile = os.getcwd()+'/'+Farm_Directories[1]+'config/'+chXMLFilename
    shell_file=open(Path_Shell,'w')
    shell_file.write('#!/bin/sh\n')
    shell_file.write(CopyRights + '\n')
    shell_file.write('export SCRAM_ARCH=slc5_amd64_gcc434\n')
    shell_file.write('source /afs/cern.ch/cms/sw/cmsset_default.sh\n')
    shell_file.write('cd ' + os.getcwd() + '\n')
    shell_file.write('eval `scramv1 runtime -sh`\n')
    shell_file.write('cd -\n')
    shell_file.write('makeScaledPredictionHistograms ' + os.getcwd() + '/'+Path_Cfg + '\n')
    shell_file.write('mv ' + outputFile + ' ' + os.getcwd() + '/' + Farm_Directories[4] + '\n\n')
    shell_file.write('# switch to later root\n')
    shell_file.write('source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh\n')
    shell_file.write('source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.00/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh\n')
    shell_file.write('cd ' + os.getcwd() + '/' + Farm_Directories[4] + '\n')
    shell_file.write('mkdir ' + signalName + '\n')
    shell_file.write('cd ' + signalName + '\n')
    shell_file.write('prepareHistFactory\n')
    shell_file.write('cp ' + pathChXMLFile + ' config/\n')
    shell_file.write('cp ' + pathCombXMLFile + ' config/\n')
    shell_file.write('hist2workspace ' + 'config/' + combXMLFilename + '\n')
    shell_file.write('root -b -q "'+Base_macro+'(\\"hscp_combined_hscp_model.root\\",\\"combined\\",\\"ModelConfig\\",\\"\\",\\"asimovData\\",0,3,true,20,0.001,0.05,1000,false,\\"\\")"' + '\n')
    shell_file.close()
    os.system("chmod 777 "+Path_Shell)


def CreateTheCmdFile():
        global Path_Cmd
        global CopyRights
        global Jobs_Name
        global Farm_Directories
        Path_Cmd   = Farm_Directories[0]+Jobs_Name+'submit.sh'
        cmd_file=open(Path_Cmd,'w')
        cmd_file.write('#!/bin/bash\n')
        cmd_file.write(CopyRights + '\n')
        #cmd_file.write('cd ' + os.getcwd() + Farm_Directories[1] + '\n')
        #cmd_file.write('# init root and prepare histfactory\n')
        #cmd_file.write('source /afs/cern.ch/sw/lcg/external/gcc/4.3.2/x86_64-slc5/setup.sh\n')
        #cmd_file.write('source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.32.00/x86_64-slc5-gcc43-opt/root/bin/thisroot.sh\n')
        #cmd_file.write('prepareHistFactory\n')
        #cmd_file.write('cd -\n\n')
        cmd_file.write('# list all bsub commands' + '\n')
        cmd_file.close()

def AddJobToCmdFile(massCut,sigInputFile):
        global Path_Shell
        global Path_Cmd
        global Queue_Name
        global Jobs_Name
        posLastUndsc = sigInputFile.rfind("_")
        posDotRoot = sigInputFile.rfind(".root")
        signalName = sigInputFile[posLastUndsc+1:posDotRoot]
        Path_Log   = os.getcwd()+'/'+Farm_Directories[2]+Jobs_Name+signalName+'_massCut'+`massCut`+'.log'
        Path_Error   = os.getcwd()+'/'+Farm_Directories[3]+Jobs_Name+signalName+'_massCut'+`massCut`+'.err'
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
    if os.path.isdir(FarmDirectory+'/inputs/config/') == False:
        os.system('mkdir ' + FarmDirectory+'/inputs/config/')

def SendCluster_Create(farmDirectory, jobName, queue, intLumi, baseCfg,
                       baseChXML, baseCombXML, baseMacro):
    global Jobs_Name
    global Jobs_Count
    global Queue_Name
    global Base_Cfg
    global Base_xml_channel
    global Base_xml_combined
    global Base_macro
    global Int_Lumi
    Jobs_Name  = jobName
    Jobs_Count = 0
    Queue_Name = queue
    Base_Cfg = baseCfg
    Base_xml_channel = baseChXML
    Base_xml_combined = baseCombXML
    Base_macro = baseMacro
    Int_Lumi=intLumi
    CreateDirectoryStructure(farmDirectory)
    CreateTheCmdFile()
    #for massCut in massCuts:
    #    SendCluster_Push(massCut)


def SendCluster_Push(bgInputFilesBase,sigInputFile,massCut,iasCut,ptCut):
    global Jobs_Count
    global Jobs_Index
    Jobs_Index = "%04i" % Jobs_Count
    bgInputFile = bgInputFilesBase+`massCut`+'.root'
    CreateTheShellFile(bgInputFile,sigInputFile,massCut,iasCut,ptCut)
    AddJobToCmdFile(massCut,sigInputFile)
    Jobs_Count = Jobs_Count+1

def SendCluster_Submit():
    global CopyRights
    global Jobs_Count
    global Path_Cmd
    os.system("source " + Path_Cmd)
    print '\n'+CopyRights
    print '%i job(s) has/have been submitted to lxbatch' % Jobs_Count

