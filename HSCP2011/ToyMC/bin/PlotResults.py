#!/usr/bin/env python

import string
import os
import sys
import glob
import math
import array

from SignalDefinitions import *

from ROOT import TGraph,TMultiGraph,TCanvas,TPaveText,gROOT,TLegend,TCutG,kGreen

BaseDir = 'FARM_allData_cutPt50GeVcutIas0.1_oneSlice_Mar16'
runCERN = True
PlotMinScale = 0.0005
PlotMaxScale = 3
GluinoXSecFile = 'gluino_XSec.txt'
StopXSecFile = 'stop_XSec.txt'
StauXSecFile = 'stau_XSec.txt'

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

def GetErrorBand(name, Mass, Low, High, MinLow=PlotMinScale,MaxHigh=PlotMaxScale):
   N = len(Mass)
   cutg = TCutG(name,2*N)
   cutg.SetFillColor(kGreen-7)
   for i in xrange(0,N):
     Min = max(Low[i],MinLow)
     cutg.SetPoint(i,Mass[i], Min)
   for i in xrange(0,N):
     Max = min(High[N-1-i],MaxHigh)
     cutg.SetPoint(N+i,Mass[N-1-i],Max)
   return cutg

def ReadXSection(InputFile):
  Mass = []
  XSec = []
  Low = []
  High = []
  for line in open(InputFile):
     line = line.strip()
     #print line
     split = line.split(' ')
     #print split
     splitString = []
     for i in split:
       if len(i) > 0:
         splitString.append(i)
     #print splitString
     Mass.append(float(splitString[0]))
     XSec.append(float(splitString[1]))
     High.append(float(splitString[2]))
     Low.append(float(splitString[3]))
  return {'mass':Mass,'xsec':XSec,'high':High,'low':Low}


# Run
makeScaledPredictionsDir = BaseDir+'/logs/makeScaledPredictions/'
makeScaledPredictionsFileList = os.listdir(makeScaledPredictionsDir)
doLimitsDir = BaseDir+'/logs/doLimits/'
doLimitsFileList = os.listdir(doLimitsDir)

massesGluinos = []
expCrossSectionsGluinos = []
massesStops = []
expCrossSectionsStops = []
massesStaus = []
expCrossSectionsStaus = []
for model in modelList:
    predFile = GetFile(makeScaledPredictionsFileList,model.name)
    limitFile = GetFile(doLimitsFileList,model.name)
    if predFile=='' or limitFile=='':
      print 'Warning: Found predFile ',predFile,' and limitFile ',limitFile,' one of which is empty'
      continue

    backExp = GetBackExp(makeScaledPredictionsDir+predFile)
    backExpError = GetBackExpError(makeScaledPredictionsDir+predFile)
    iasCut  = GetIasCut(makeScaledPredictionsDir+predFile)
    sigEff = GetSigEff(makeScaledPredictionsDir+predFile)
    bk = GetBkOverIas(makeScaledPredictionsDir+predFile)
    massCut = GetMassCut(predFile)
    ptCut = GetPtCut(predFile)
    signalName = GetModelName(predFile)
    expLimit = GetExpLimit(doLimitsDir+limitFile)

    if 'Gluino' in model.name and not 'GluinoN' in model.name:
      massesGluinos.append(float(model.mass))
      expCrossSectionsGluinos.append(float(expLimit))
      #print 'Gluino: Mass: ',model.mass,' thCrossSection: ',model.crossSection,' expLimit: ',expLimit
    if 'Stop' in model.name and not 'StopN' in model.name:
      massesStops.append(float(model.mass))
      expCrossSectionsStops.append(float(expLimit))
      #print 'Stop: Mass: ',model.mass,' thCrossSection: ',model.crossSection,' expLimit: ',expLimit
    if 'Stau' in model.name and not 'PPStau' in model.name:
      massesStaus.append(float(model.mass))
      expCrossSectionsStaus.append(float(expLimit))
      #print 'Stau: Mass: ',model.mass,' thCrossSection: ',model.crossSection,' expLimit: ',expLimit

gluinoThInfo = ReadXSection(GluinoXSecFile)
gluinoXSecErr = GetErrorBand("gluinoErr",gluinoThInfo['mass'],gluinoThInfo['low'],gluinoThInfo['high'])
theoryGraphGluinos = TGraph(len(gluinoThInfo['mass']),array.array("f",gluinoThInfo['mass']),array.array("f",gluinoThInfo['xsec']))
theoryGraphGluinos.SetTitle('')
theoryGraphGluinos.GetYaxis().SetTitle("CrossSection ( pb )")
theoryGraphGluinos.GetYaxis().SetTitleOffset(1.70)
theoryGraphGluinos.SetLineColor(4)
theoryGraphGluinos.SetMarkerColor(4)
theoryGraphGluinos.SetLineWidth(1)
theoryGraphGluinos.SetLineStyle(3)
theoryGraphGluinos.SetMarkerStyle(1)
expLimGraphGluinos = TGraph(len(massesGluinos),array.array("f",massesGluinos),array.array("f",expCrossSectionsGluinos))
expLimGraphGluinos.SetTitle('')
expLimGraphGluinos.GetYaxis().SetTitle("CrossSection ( pb )")
expLimGraphGluinos.GetYaxis().SetTitleOffset(1.70)
expLimGraphGluinos.SetLineColor(4)
expLimGraphGluinos.SetMarkerColor(4)
expLimGraphGluinos.SetLineWidth(2)
expLimGraphGluinos.SetLineStyle(1)
expLimGraphGluinos.SetMarkerStyle(22)
stopThInfo = ReadXSection(StopXSecFile)
stopXSecErr = GetErrorBand("stopErr",stopThInfo['mass'],stopThInfo['low'],stopThInfo['high'])
theoryGraphStops = TGraph(len(stopThInfo['mass']),array.array("f",stopThInfo['mass']),array.array("f",stopThInfo['xsec']))
theoryGraphStops.SetTitle('')
theoryGraphStops.GetYaxis().SetTitle("CrossSection ( pb )")
theoryGraphStops.GetYaxis().SetTitleOffset(1.70)
theoryGraphStops.SetLineColor(2)
theoryGraphStops.SetMarkerColor(2)
theoryGraphStops.SetLineWidth(1)
theoryGraphStops.SetLineStyle(2)
theoryGraphStops.SetMarkerStyle(1);
expLimGraphStops = TGraph(len(massesStops),array.array("f",massesStops),array.array("f",expCrossSectionsStops))
expLimGraphStops.SetTitle('')
expLimGraphStops.GetYaxis().SetTitle("CrossSection ( pb )")
expLimGraphStops.GetYaxis().SetTitleOffset(1.70)
expLimGraphStops.SetLineColor(2)
expLimGraphStops.SetMarkerColor(2)
expLimGraphStops.SetLineWidth(2)
expLimGraphStops.SetLineStyle(1)
expLimGraphStops.SetMarkerStyle(21)
stauThInfo = ReadXSection(StauXSecFile)
stauXSecErr = GetErrorBand("stauErr",stauThInfo['mass'],stauThInfo['low'],stauThInfo['high'])
theoryGraphStaus = TGraph(len(stauThInfo['mass']),array.array("f",stauThInfo['mass']),array.array("f",stauThInfo['xsec']))
theoryGraphStaus.SetTitle('')
theoryGraphStaus.GetYaxis().SetTitle("CrossSection ( pb )")
theoryGraphStaus.GetYaxis().SetTitleOffset(1.70)
theoryGraphStaus.SetLineColor(1)
theoryGraphStaus.SetMarkerColor(1)
theoryGraphStaus.SetLineWidth(1)
theoryGraphStaus.SetLineStyle(1)
theoryGraphStaus.SetMarkerStyle(1);
expLimGraphStaus = TGraph(len(massesStaus),array.array("f",massesStaus),array.array("f",expCrossSectionsStaus))
expLimGraphStaus.SetTitle('')
expLimGraphStaus.GetYaxis().SetTitle("CrossSection ( pb )")
expLimGraphStaus.GetYaxis().SetTitleOffset(1.70)
expLimGraphStaus.SetLineColor(1)
expLimGraphStaus.SetMarkerColor(1)
expLimGraphStaus.SetLineWidth(2)
expLimGraphStaus.SetLineStyle(1)
expLimGraphStaus.SetMarkerStyle(20)

c1 = TCanvas("c1", "c1",600,600)
c1.SetLogy()
gluinoXSecErr.Draw("f")
stopXSecErr.Draw("f")
stauXSecErr.Draw("f")
#PPStauXSecErr  ->Draw("f")
#MGTk.Draw("same")
MGTk = TMultiGraph()
MGTk.Add(theoryGraphGluinos      ,"L")
MGTk.Add(theoryGraphStops      ,"L")
MGTk.Add(theoryGraphStaus      ,"L")
MGTk.Add(expLimGraphGluinos      ,"LP")
MGTk.Add(expLimGraphStops      ,"LP")
MGTk.Add(expLimGraphStaus      ,"LP")
MGTk.Draw("A")
MGTk.SetTitle("")
MGTk.GetXaxis().SetTitle("Mass (GeV/c^{2})")
MGTk.GetYaxis().SetTitle("#sigma (pb)")
MGTk.GetYaxis().SetTitleOffset(1.0)
MGTk.GetYaxis().SetRangeUser(PlotMinScale,PlotMaxScale)

Lumi=4679
X=0.40
Y=0.995
W=0.82
H=0.945
text = TPaveText(X,Y,W,H, "NDC")
text.SetFillColor(0)
text.SetTextAlign(22)
text.AddText("CMS Preliminary   #sqrt{s} = 7 TeV   "+str(Lumi)+" fb^{-1}")
text.SetBorderSize(0)
text.Draw("same")

LEGTh = TLegend(0.15,0.7,0.42,0.9)
LEGTh.SetHeader("Theoretical Prediction")
LEGTh.SetFillColor(0)
LEGTh.SetBorderSize(0)
GlThLeg = theoryGraphGluinos.Clone("GluinoThLeg")
GlThLeg.SetFillColor(gluinoXSecErr.GetFillColor())
LEGTh.AddEntry(GlThLeg, "gluino (NLO+NLL)" ,"LF")
StThLeg = theoryGraphStops.Clone("StopThLeg")
StThLeg.SetFillColor(stopXSecErr.GetFillColor())
LEGTh.AddEntry(StThLeg   ,"stop   (NLO+NLL)" ,"LF")
StauThLeg = theoryGraphStaus.Clone("StauThLeg")
StauThLeg.SetFillColor(stauXSecErr.GetFillColor())
LEGTh.AddEntry(StauThLeg   ,"GMSB stau   (NLO)" ,"LF")
LEGTh.Draw()

LEGTk = TLegend(0.6,0.7,0.8,0.9)
#LEGTk->SetHeader("95% C.L. Limits")
LEGTk.SetHeader("Tk - Only")
LEGTk.SetFillColor(0)
LEGTk.SetBorderSize(0)
#LEGTk.AddEntry(Tk_Obs_GluinoF5 , "gluino; 50% #tilde{g}g"    ,"LP")
LEGTk.AddEntry(expLimGraphGluinos , "gluino; 10% #tilde{g}g"    ,"LP")
#LEGTk.AddEntry(Tk_Obs_GluinoNF1, "gluino; 10% #tilde{g}g; ch. suppr.","LP")
LEGTk.AddEntry(expLimGraphStops     , "stop"            ,"LP")
#LEGTk.AddEntry(Tk_Obs_StopN    , "stop; ch. suppr.","LP")
#LEGTk.AddEntry(Tk_Obs_PPStau   , "Pair Prod. stau"       ,"LP")
LEGTk.AddEntry(expLimGraphStaus   , "GMSB stau"       ,"LP")
LEGTk.Draw()

limit = FindIntersection(expLimGraphGluinos,theoryGraphGluinos,300,1200,1,0.00,False)
print 'Found Gluino (f=0.1) mass limit = ',limit
limit = FindIntersection(expLimGraphStops,theoryGraphStops,300,1200,1,0.00,False)
print 'Found Stop mass limit = ',limit
limit = FindIntersection(expLimGraphStaus,theoryGraphStaus,100,1000,1,0.00,False)
print 'Found GMStau mass limit = ',limit
c1.Update()
c1.Print('gluinosStopsStaus.png')




