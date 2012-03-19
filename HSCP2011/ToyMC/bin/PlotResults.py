#!/usr/bin/env python

import string
import os
import sys
import glob
import math
import array

from SignalDefinitions import *

from ROOT import TGraph,TMultiGraph,TCanvas,TPaveText,gROOT

BaseDir = 'FARM_allData_cutPt50GeVcutIas0.1_oneSlice_Mar16'
runCERN = True
PlotMinScale = 0.0005
PlotMaxScale = 3

gROOT.Reset()

if(runCERN):
  fileEnd = '.log'
else:
  fileEnd = '.out'


def GetModelName(fileName):
  massCutPos = fileName.find("_massCut")
  fileNameReduced = fileName[0:massCutPos]
  lastUndscPos = fileNameReduced.rfind("_")
  signalName = fileNameReduced[lastUndscPos+1:len(fileNameReduced)]
  return signalName

def GetMassCut(fileName):
  ptCutPos = fileName.find("_ptCut")
  substr = fileName[fileName.find("_massCut")+len("_massCut"):ptCutPos]
  return substr

def GetPtCut(fileName):
  if(runCERN):
    dotOutPos = fileName.rfind(".log")
  else:
    dotOutPos = fileName.rfind(".out")
  substr = fileName[fileName.find("_ptCut")+len("_ptCut"):dotOutPos]
  return substr

def GetIasCut(filePath):
  for line in open(filePath):
    if "Ias" in line and "Running" in line:
      break
  #print "GetIasCut ",line
  return line[line.find("(")+1:len(line)-2]

def GetBackExp(filePath):
  for line in open(filePath):
    if "found" in line and not "WARNING" in line:
      break
  lineSplit = line.split(' ')
  if "found" in line and not "WARNING" in line:
    return float(lineSplit[1])
  else:
    return 0

def GetBackExpError(filePath):
  for line in open(filePath):
    if "found" in line and not "WARNING" in line:
      break
  lineSplit = line.split(' ')
  if "found" in line and not "WARNING" in line:
    return float(lineSplit[3])
  else:
    return 0

def GetSigEff(filePath):
  for line in open(filePath):
    if "event level" in line:
      break
  lineSplit = line.split(' ')
  if "event level" in line:
    return float(lineSplit[0])
  else:
    return 0

def GetBkOverIas(filePath):
  for line in open(filePath):
    if "Bk" in line:
      break
  lineSplit = line.split(' ')
  if "Bk" in line:
    #print line
    return float(lineSplit[0])
  else:
    return 0.0

def GetExpLimit(filePath):
  for line in open(filePath):
    if "95%" in line:
      break
  return float(line[line.find("0,")+2:line.find("]")-1])

def GetFile(fileList,modelName):
  for fileName in fileList:
    if fileEnd in fileName and modelName in fileName:
      return fileName
  return ''

def FindIntersection(obsGraph, thGraph, min, max, step, thUncertainty, debug):
  intersection = -1
  thShift = 1.0-thUncertainty
  previousX = min
  previousV = obsGraph.Eval(previousX, 0, "") - (thShift * thGraph.Eval(previousX, 0, ""))
  if previousV > 0 :
     return -1
  for x in xrange(min,max,step):
      v = obsGraph.Eval(x, 0, "") - (thShift * thGraph.Eval(x, 0, "") )
      #if debug:
      #  print "%7.2f --> Obs=%6.2E  Th=%6.2E",x,obs->Eval(x, 0, ""),ThShift * th->Eval(x, 0, "")
      #  if(V>=0)printf("   X\n");
      #  else printf("\n");
      if v < 0:
        previousX = x
        previousV = v
      else:
        intersection = previousX
  return intersection

def DrawPreliminary(Lumi,  X=0.40,  Y=0.995,  W=0.82, H=0.945):
  text = TPaveText(X,Y,W,H, "NDC");
  text.SetFillColor(0);
  text.SetTextAlign(22);
  if(Lumi<0):
    text.AddText("CMS Preliminary   #sqrt{s} 7 TeV");
  if(Lumi>0):
    text.AddText("CMS Preliminary   #sqrt{s} = 7 TeV   "+str(Lumi)+" fb^{-1}");
  text.Draw("same");


makeScaledPredictionsDir = BaseDir+'/logs/makeScaledPredictions/'
makeScaledPredictionsFileList = os.listdir(makeScaledPredictionsDir)
doLimitsDir = BaseDir+'/logs/doLimits/'
doLimitsFileList = os.listdir(doLimitsDir)

modelListGluinos = [Gluino300,Gluino400,Gluino500,Gluino600,Gluino700,Gluino800,Gluino900,Gluino1000,Gluino1100,Gluino1200]
massesGluinos = []
thCrossSectionsGluinos = []
expCrossSectionsGluinos = []
for model in modelListGluinos:
    predFile = GetFile(makeScaledPredictionsFileList,model.name)
    limitFile = GetFile(doLimitsFileList,model.name)
    if predFile=='' or limitFile=='':
      print 'Error: Found predFile ',predFile,' and limitFile ',limitFile,' one of which is empty'
      exit(1)

    backExp = GetBackExp(makeScaledPredictionsDir+predFile)
    backExpError = GetBackExpError(makeScaledPredictionsDir+predFile)
    iasCut  = GetIasCut(makeScaledPredictionsDir+predFile)
    sigEff = GetSigEff(makeScaledPredictionsDir+predFile)
    bk = GetBkOverIas(makeScaledPredictionsDir+predFile)
    massCut = GetMassCut(predFile)
    ptCut = GetPtCut(predFile)
    signalName = GetModelName(predFile)
    expLimit = GetExpLimit(doLimitsDir+limitFile)

    massesGluinos.append(float(model.mass))
    thCrossSectionsGluinos.append(float(model.crossSection))
    expCrossSectionsGluinos.append(float(expLimit))
    #print 'Mass: ',model.mass,' thCrossSection: ',model.crossSection,' expLimit: ',expLimit

theoryGraph = TGraph(len(modelListGluinos),array.array("f",massesGluinos),array.array("f",thCrossSectionsGluinos))
theoryGraph.SetTitle('')
theoryGraph.GetYaxis().SetTitle("CrossSection ( pb )")
theoryGraph.GetYaxis().SetTitleOffset(1.70)
theoryGraph.SetLineColor(4)
theoryGraph.SetMarkerColor(4)
theoryGraph.SetLineWidth(1)
theoryGraph.SetLineStyle(3)
theoryGraph.SetMarkerStyle(1)

expLimGraph = TGraph(len(modelListGluinos),array.array("f",massesGluinos),array.array("f",expCrossSectionsGluinos))
expLimGraph.SetTitle('')
expLimGraph.GetYaxis().SetTitle("CrossSection ( pb )")
expLimGraph.GetYaxis().SetTitleOffset(1.70)
expLimGraph.SetLineColor(4)
expLimGraph.SetMarkerColor(4)
expLimGraph.SetLineWidth(2)
expLimGraph.SetLineStyle(1)
expLimGraph.SetMarkerStyle(22)

c1 = TCanvas("c1", "c1",600,600)
c1.SetLogy()
MGTk = TMultiGraph()
MGTk.Add(theoryGraph      ,"L");
MGTk.Add(expLimGraph      ,"LP");
MGTk.Draw("A");
#GluinoXSecErr->Draw("f");
#StopXSecErr  ->Draw("f");
#StauXSecErr  ->Draw("f");
#PPStauXSecErr  ->Draw("f");
#MGTk.Draw("same");
MGTk.SetTitle("");
MGTk.GetXaxis().SetTitle("Mass (GeV/c^{2})");
MGTk.GetYaxis().SetTitle("#sigma (pb)");
MGTk.GetYaxis().SetTitleOffset(1.0);
MGTk.GetYaxis().SetRangeUser(PlotMinScale,PlotMaxScale);

Lumi=4679
X=0.40
Y=0.995
W=0.82
H=0.945
text = TPaveText(X,Y,W,H, "NDC");
text.SetFillColor(0);
text.SetTextAlign(22);
text.AddText("CMS Preliminary   #sqrt{s} = 7 TeV   "+str(Lumi)+" fb^{-1}");
text.SetBorderSize(0)
text.Draw("same");

limit = FindIntersection(expLimGraph,theoryGraph,300,1200,1,0.00,False)
print 'Found mass limit = ',limit
c1.Update()
c1.Print('gluino.png')
