#!/usr/bin/env python

import string
import os
import sys
import glob
import math
import array
from SignalDefinitions import *
# run in batch
sys.argv.append('-b')
import ROOT
#
from ROOT import TGraph,TMultiGraph,TCanvas,TPaveText,gROOT,TLegend,TCutG,kGreen,TFile,RooStats,Math,vector,gROOT,TLine,kRed,gPad

# at UMN, must use /usr/bin/python (2.4) and my compiled ROOT version (5.32.02)
#BaseDir = 'FARM_dReg_NL_50IasBins_emptyBins1e-25_iasPredScalingFix_cutPt50GeVcutIas0.1_allSlices_Apr30'
#BaseDir = 'FARM_dReg_NL_20IasBins_emptyBins1e-25_1gausSystWithNoStatErr_cutPt50GeVcutIas0.1_allSlices_May02'
#BaseDir = 'FARM_20IasB_4etaSlices_5NoMSlices_emptyB1e-25_1gausSystWithStatErr_cutPt50GeVcutIas0.1_allSlices_May03'
#BaseDir = 'FARM_20IasB_normEtaNoMSlicing_emptyB1e-25_1gausSystWithStatErrOneSliceOnly_cutPt50GeVcutIas0.1_allSlices_May06'
#BaseDir = 'FARM_50IasB_emptyB1e-25_1gausSystWithStatErrLargeErrorBinsToZero_cutPt50GeVcutIas0.1_oneSlice_May09'
#BaseDir = 'FARM_50IasB_emptyB1e-25_1gausSystWithStatErr_cutPt50GeVcutIas0.1_oneSlice_May09'
#BaseDir = 'FARM_50IasB_emptyB1e-25_1gausSystWithNoStatErr_cutPt50GeVcutIas0.1_oneSlice_May09'
#BaseDir = 'FARM_50IasB_emptyB1e-25_1gausSystWithStatErrLargeErrorsZero_cutPt50GeVcutIas0.1_allSlices_May11'
#BaseDir = 'FARM_50IasB_emptyB1e-25_1gausSystWithStatErrLargeErrorsZero_2pctBackDecr_cutPt50GeVcutIas0.1_allSlices_May16'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatErr_cutPt50GeVcutIas0.1_allSlices_May26'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatErr_nominalAsNominal_cutPt50GeVcutIas0.1_allSlices_May28'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystWithStatErr400SortedBins_nominalAsNominal_cutPt50GeVcutIas0.1_allSlices_May28'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystWithStatErrIgnoreFirst400SortedBins_nominalAsNominal_cutPt50GeVcutIas0.1_allSlices_May28'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystWithStatErr400SortedBins_nominalAsNominal_highToyStatTestFewModels_cutPt50GeVcutIas0.1_allSlices_Jun01'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_fullIasShiftAsNominal_cutPt50GeVcutIas0.1_allSlices_Jun05'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystWithStatErrIgnoreFirst400SortedBins_nominalAsNominal_highToyStatTestFewModels_cutPt50GeVcutIas0.1_allSlices_Jun04'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_nominalAsNominal_highToyStatTestFewModels_cutPt50GeVcutIas0.1_allSlices_Jun05'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_halfIasShiftAsNominalPlusShapeVariation_cutPt50GeVcutIas0.1_allSlices_Jun06'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_nominalAsNominal_highToyStatAllModels_cutPt50GeVcutIas0.1_allSlices_Jun06'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_nomShiftAsNominal_highToyStatsAllModels_cutPt50GeVcutIas0.1_allSlices_Jun06'
#BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_halfIasShiftAsNominalNoShapeVariation_cutPt50GeVcutIas0.1_allSlices_Jun07'
BaseDir = 'FARM_chk50IasB_emptyB1e-25_1gausSystNoStatError_halfIasHalfNoMAsNominal_highToyStatAllModels_cutPt50GeVcutIas0.1_allSlices_Jun07'

runCERN = False
PlotMinScale = 0.0005
PlotMaxScale = 3
GluinoXSecFile = 'gluino_XSec.txt'
StopXSecFile = 'stop_XSec.txt'
StauXSecFile = 'stau_XSec.txt'

gROOT.Reset()

# Load/apply TDR style
gROOT.ProcessLine('.L tdrStyle.C')
gROOT.ProcessLine('setTDRStyle()')

if(runCERN):
  fileEnd = '.log'
else:
  fileEnd = '.out'



class LimitResult:
  def __init__(self,name,mass,thCrossSection,ptCut,iasCut,massCut,sigEff,expBg,expBgStatErr,obsEvts,expLimit,obsLimit,fiveSigmaXSec,obsSignalSignif):
    self.name = name
    self.mass = mass
    self.thCrossSection = thCrossSection
    self.ptCut = ptCut
    self.iasCut = iasCut
    self.massCut = massCut
    self.sigEff = sigEff
    self.expBg = expBg
    self.expBgStatErr = expBgStatErr
    self.obsEvts = obsEvts
    self.expLimit = expLimit
    self.obsLimit = obsLimit
    self.fiveSigmaXSec = fiveSigmaXSec
    self.obsSignalSignif = obsSignalSignif

  def Print(self):
    print 'LimitResults object information:'
    print '\tname=',self.name
    print '\tmass=',self.mass
    print '\tthCrossSection=',self.thCrossSection
    print '\tptCut=',self.ptCut
    print '\tiasCut=',self.iasCut
    print '\tmassCut=',self.massCut
    print '\tsigEff=',self.sigEff
    print '\texpBg=',self.expBg
    print '\texpBgStatErr=',self.expBgStatErr
    print '\tobsEvts=',self.obsEvts
    print '\texpLimit=',self.expLimit
    print '\tobsLimit=',self.obsLimit
    print '\tfiveSigmaXSec=',self.fiveSigmaXSec
    print '\tobsSignalSignif=',self.obsSignalSignif

  def LatexTableLine(self):
    latexLine = self.name+'&'+self.massCut+'&'+str(round(self.sigEff,2))+'&'+str(round(self.expBg,2))+'$\pm$'+str(round(self.expBgStatErr,2))+'&'
    latexLine+=str(self.obsEvts)+'&'+str(round(float(self.thCrossSection),4))+'&'+str(round(self.expLimit,4))+'&'
    latexLine+=str(round(self.obsLimit,4))+'&'+str(round(self.fiveSigmaXSec,4))+'&'+str(round(self.obsSignalSignif,4))
    latexLine+='\\\\'
    return latexLine

  def StringTableLine(self):
    tableString = string.ljust(self.name,15)+string.ljust(self.ptCut,6)+string.ljust(self.iasCut,6)
    tableString+=string.center(str(self.massCut),8)+string.center(str(round(self.sigEff,4)),10)
    backExpString = str(round(self.expBg,4))+' +/- '+str(round(self.expBgStatErr,4))
    tableString+=string.center(backExpString,22)+string.center(str(self.obsEvts),10)
    tableString+=string.center(str(round(float(self.thCrossSection),6)),10)+string.center(str(round(self.expLimit,6)),10)
    tableString+=string.center(str(round(self.obsLimit,6)),10)+string.center(str(round(self.fiveSigmaXSec,6)),12)
    tableString+=string.center(str(round(self.obsSignalSignif,6)),10)
    return tableString



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
    return float(lineSplit[0])
  else:
    return 0.0

def GetFile(fileList,modelName):
  for fileName in fileList:
    if fileEnd in fileName and modelName in fileName:
      return fileName
  return ''

def GetRootFile(fileList,modelName,limitFile):
  for fileName in fileList:
    if modelName in fileName:
      if limitFile and 'combined' in fileName:
        return fileName
      elif not limitFile:
        return fileName
  return ''

def GetExpDiscSignif(filePath):
  for line in open(filePath):
    if "significance at 0" in line:
      value = line[line.rfind("significance")+13:line.rfind("sigma")-2]
      if(value[0]=='-'):
        return 0.0
      else:
        return float(value)
  return 0.0

def GetSignalSignificanceFiveSigma(doSignificanceDir,fileList,modelName):
  if(len(fileList) == 0):
    return -666
  poiSigDict = {}
  for fileName in fileList:
    if modelName in fileName and fileEnd in fileName:
      if 'asymptotic' in fileName:
        medianSignificance = GetExpDiscSignif(doSignificanceDir+fileName)
        poi = float(fileName[fileName.find(modelName)+len(modelName)+1:fileName.rfind("asymptotic")-1])
        if(medianSignificance > 0):
          poiSigDict[poi] = medianSignificance
  medianSignifs = vector("double")()
  poiValues = vector("double")()
  for poi,sig in sorted(poiSigDict.items()):
    poiValues.push_back(poi)
    medianSignifs.push_back(sig)
    if(modelName=='GMStau494'):
      print 'Poi: ',poi,' medianSignificance: ',sig
  if(len(medianSignifs) < 3):
    print 'ERROR: Not enough (nonzero) points to do interpolation!'
    return -666
  interp = Math.Interpolator(medianSignifs,poiValues)
  return interp.Eval(5)

def GetObsDiscSignif(filePath):
  for line in open(filePath):
    if "Significance =" in line:
      signif = line[line.rfind("Significance =")+15:line.rfind("sigma")-1]
      if "-" in signif:
        signif = signif.lstrip("-")
      return float(signif)
  return 0.0

def GetSignalSignificance(doSignificanceDir,fileList,modelName):
  significance = -999
  for fileName in fileList:
    if modelName in fileName and fileEnd in fileName:
      if 'asymptotic' in fileName:
        significance = GetObsDiscSignif(doSignificanceDir+fileName)
        break
  return significance

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
    text.AddText("CMS Preliminary   #sqrt{s} = 7 TeV   "+str(Lumi)+" pb^{-1}");
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
     split = line.split(' ')
     splitString = []
     for i in split:
       if len(i) > 0:
         splitString.append(i)
     Mass.append(float(splitString[0]))
     XSec.append(float(splitString[1]))
     High.append(float(splitString[2]))
     Low.append(float(splitString[3]))
  return {'mass':Mass,'xsec':XSec,'high':High,'low':Low}

def MakePValuePlot(htip):
  gobs = htip.MakePlot()
  gobs.Draw("APL")   
  gplot = gobs
  gplot.GetHistogram().SetTitle( htip.GetTitle() )
  # exp
  gexp = htip.MakeExpectedPlot()
  gexp.Draw()
  # line for CL
  alpha = 0.05
  x1 = gplot.GetXaxis().GetXmin()
  x2 = gplot.GetXaxis().GetXmax()
  line = TLine(x1, alpha, x2,alpha)
  line.SetLineColor(kRed)
  line.Draw()
  # put axis labels 
  gplot.GetXaxis().SetTitle("#sigma (pb)")
  gplot.GetXaxis().SetNdivisions(508)
  gplot.GetYaxis().SetTitle("p value")
  # draw again observed values otherwise will be covered by the bands
  gobs.Draw("PL");
  # redraw the axis 
  gPad.RedrawAxis()
  #
  y0 = 0.6
  verticalSize = 0.3
  y1 = y0 + verticalSize
  l = TLegend(0.6,y0,0.8,y1)
  l.AddEntry(gobs,"","PEL")
  # loop in reverse order (opposite to drawing one)
  #ngraphs = gexp.GetListOfGraphs().GetSize()
  #for i in range(ngraphs-1,-1,-1):
  #  obj =  gexp.GetListOfGraphs().At(i)
  #  lopt = "F"
  #  if(i == ngraphs-1):
  #    lopt = "L"
  #  if(obj != 0):
  #    l.AddEntry(obj,"",lopt);
  l.Draw()
  gPad.Update()


# Run
makeScaledPredictionsDir = BaseDir+'/logs/makeScaledPredictions/'
makeScaledPredictionsLogFileList = os.listdir(makeScaledPredictionsDir)
makeScaledPredictionsRootDir = BaseDir+'/outputs/makeScaledPredictions/'
makeScaledPredictionsRootFileList = os.listdir(makeScaledPredictionsRootDir)
doLimitsDir = BaseDir+'/logs/doLimits/'
try:
  doLimitsLogFileList = os.listdir(doLimitsDir)
except OSError:
  doLimitsLogFileList = []
doLimitsRootDir = BaseDir+'/outputs/doLimits/'
try:
  doLimitsRootFileList = os.listdir(doLimitsRootDir)
except OSError:
  doLimitsRootFileList = []
doSignificanceDir = BaseDir+'/logs/doSignificance/'
try:
  doSignificanceLogFileList = os.listdir(doSignificanceDir)
except OSError:
  doSignificanceLogFileList = []

LimitResultsGluinos = []
LimitResultsStops = []
LimitResultsStaus = []

# MKDIR
plotsDir = BaseDir+'/results/'
if os.path.isdir(plotsDir) == False:
  os.mkdir(plotsDir)
myCanvas = TCanvas("myCanvas", "myCanvas",1000,800)
##########
#modelList = [Stop200,GMStau156,GMStau200]
##########
for model in modelList:
    predLogFile = GetFile(makeScaledPredictionsLogFileList,model.name)
    limitLogFile = GetFile(doLimitsLogFileList,model.name)
    predRootFile = GetRootFile(makeScaledPredictionsRootFileList,model.name,False)
    limitRootFile = GetRootFile(doLimitsRootFileList,model.name,True)
    if predLogFile=='' or limitLogFile=='':
      print 'Warning: Found predLogFile ',predLogFile,' and limitLogFile ',limitLogFile,' one of which is empty'
    if predRootFile=='' or limitRootFile=='':
      print 'Warning: Found predRootFile ',predRootFile,' and limitRootFile ',limitRootFile,' one of which is empty'

    backExp = GetBackExp(makeScaledPredictionsDir+predLogFile)
    backExpError = GetBackExpError(makeScaledPredictionsDir+predLogFile)
    iasCut  = GetIasCut(makeScaledPredictionsDir+predLogFile)
    sigEff = GetSigEff(makeScaledPredictionsDir+predLogFile)
    bk = GetBkOverIas(makeScaledPredictionsDir+predLogFile)
    massCut = GetMassCut(predLogFile)
    ptCut = GetPtCut(predLogFile)
    signalName = GetModelName(predLogFile)
    if(len(limitRootFile) > 0):
      limitTFile = TFile(doLimitsRootDir+limitRootFile)
      htr = limitTFile.Get("result_SigXsec")
      expLimit = htr.GetExpectedUpperLimit(0)
      obsLimit = htr.UpperLimit()
      #pValuePlotTitle = "Cross section limit (pb) for "+model.name
      pValuePlotTitle = ""
      #pValuePlotName = "crossSectionLimit_pValue_"+model.name
      pValueFileName = "crossSectionLimit_pValue_"+model.name
      pValuePlotName = ""
      myCanvas.cd()
      pValuePlot = RooStats.HypoTestInverterPlot("htrResultPlot",pValuePlotName,htr)
      pValuePlot.Draw()
      #MakePValuePlot(pValuePlot)
      Lumi=4976
      X=0.40
      Y=0.995
      W=0.82
      H=0.945
      text = TPaveText(X,Y,W,H, "NDC")
      text.SetFillColor(0)
      text.SetTextAlign(22)
      text.AddText("CMS Preliminary   #sqrt{s} = 7 TeV   "+str(Lumi)+" pb^{-1}")
      text.SetBorderSize(0)
      text.Draw("same")
      myCanvas.Update()
      myCanvas.Print(plotsDir+pValueFileName+".C")
      myCanvas.Print(plotsDir+pValueFileName+".pdf")
      myCanvas.Print(plotsDir+pValueFileName+".png")
      ## wait for input to keep the GUI (which lives on a ROOT event dispatcher) alive
      #if __name__ == '__main__':
      #   rep = ''
      #   while not rep in [ 'q', 'Q' ]:
      #      rep = raw_input( 'enter "q" to quit: ' )
      #      if 1 < len(rep):
      #         rep = rep[0]

      limitTFile.Close()
    else:
      expLimit = -99
      obsLimit = -99
    predTFile = TFile(makeScaledPredictionsRootDir+predRootFile)
    dataHist = predTFile.Get("dataAllNoMAllEtaUnrolledHist")
    obsEvts = dataHist.Integral()
    predTFile.Close()
    expSignalSignifRaw = GetSignalSignificanceFiveSigma(doSignificanceDir,doSignificanceLogFileList,model.name)
    # Multiply by 1.5, since we are underestimating 5-sigma cross section by using asymptotic method vs. toys
    # see spreadsheet compareToysAsymptoticDiscovery_GMStau100_Gluino1200.xls
    # 1.5 is the spread between the interpolations for the gluino1200, where the asymptotic approx should be the worst
    expSignalSignif = 1.5*expSignalSignifRaw
    obsSignalSignif = GetSignalSignificance(doSignificanceDir,doSignificanceLogFileList,model.name)

    if 'Gluino' in model.name and not 'GluinoN' in model.name:
      thisLR = LimitResult(model.name,model.mass,model.crossSection,ptCut,iasCut,massCut,sigEff,backExp,backExpError,obsEvts,expLimit,obsLimit,expSignalSignif,obsSignalSignif)
      LimitResultsGluinos.append(thisLR)
      #print thisLR.StringTableLine()
    if 'Stop' in model.name and not 'StopN' in model.name:
      thisLR = LimitResult(model.name,model.mass,model.crossSection,ptCut,iasCut,massCut,sigEff,backExp,backExpError,obsEvts,expLimit,obsLimit,expSignalSignif,obsSignalSignif)
      LimitResultsStops.append(thisLR)
      #print thisLR.StringTableLine()
    if 'Stau' in model.name and not 'PPStau' in model.name:
      thisLR = LimitResult(model.name,model.mass,model.crossSection,ptCut,iasCut,massCut,sigEff,backExp,backExpError,obsEvts,expLimit,obsLimit,expSignalSignif,obsSignalSignif)
      LimitResultsStaus.append(thisLR)
      #print thisLR.StringTableLine()

# TABLE OUTPUT
print
print
titleString = string.ljust('Model',15)+string.ljust('Pt',6)+string.ljust('Ias',6)+string.center('Mass',8)
titleString+=string.center('SigEff',10)+string.center('BackExpOverIas',22)+string.center('Obs.',10)
titleString+=string.center('ThCrossSec',10)+string.center('ExpLim',10)+string.center('ObsLim',10)
titleString+=string.center('Exp5SigDisc',12)
titleString+=string.center('ObsSignf.',10)
print titleString
# write text file
txt_file=open(plotsDir+'results.txt','w')
txt_file.write(titleString+'\n')
# print table lines
for limitRes in LimitResultsGluinos:
  print limitRes.StringTableLine()
  txt_file.write(limitRes.StringTableLine()+'\n')
for limitRes in LimitResultsStops:
  print limitRes.StringTableLine()
  txt_file.write(limitRes.StringTableLine()+'\n')
for limitRes in LimitResultsStaus:
  print limitRes.StringTableLine()
  txt_file.write(limitRes.StringTableLine()+'\n')
txt_file.close()

# write tbl fragment file
titleTableString = '\\begin{tabular}{|l|c|c|c|}\n'
titleTableString+= '\\hline\n'
    #latexLine = self.name+'&'+self.massCut+'&'+str(round(self.sigEff,4))+'&'+str(round(self.expBg,4))+'&'+str(round(self.expBgStatErr,4))+'&'
    #latexLine+=self.obsEvts+'&'+str(round(self.ThCrossSec,6))+'&'+str(round(self.expLimit,6))+'&'
    #latexLine+=str(round(self.obsLimit,6))+'&'+str(round(self.fiveSigmaXSec,6))+'&'+str(round(self.obsSignalSignif,6))
titleTableString+= '\\textbf{Model and mass}&\\textbf{Mass Cut}&\\textbf{Signal Eff.}&\\textbf{Background Exp.}&'
titleTableString+= '\\textbf{Obs.}&\\textbf{Th. Cross Sec.}&\\textbf{Exp. Lim.}&\\textbf{Obs. Lim.}&\\textbf{5$\sigma$ Disc. Cross Section}&\\textbf{Obs. Sig. Eff.}\n'
titleTableString+= '\\hline\n'
txt_file=open(plotsDir+'results.tbl','w')
txt_file.write(titleTableString+'\n')
print titleTableString
for limitRes in LimitResultsGluinos:
  txt_file.write(limitRes.LatexTableLine()+'\n')
  print limitRes.LatexTableLine()
for limitRes in LimitResultsStops:
  print limitRes.LatexTableLine()
  txt_file.write(limitRes.LatexTableLine()+'\n')
for limitRes in LimitResultsStaus:
  print limitRes.LatexTableLine()
  txt_file.write(limitRes.LatexTableLine()+'\n')
txt_file.write('\\hline\n')
print '\\hline'
txt_file.write('\\end{tabular}\n')
print '\\end{tabular}'
txt_file.close()


# TESTING
sys.exit()

# gluinos
massesGluinos = []
expCrossSectionsGluinos = []
obsCrossSectionsGluinos = []
for lr in LimitResultsGluinos:
  massesGluinos.append(lr.mass)
  expCrossSectionsGluinos.append(lr.expLimit)
  obsCrossSectionsGluinos.append(lr.obsLimit)
# stops
massesStops = []
expCrossSectionsStops = []
obsCrossSectionsStops = []
for lr in LimitResultsStops:
  massesStops.append(lr.mass)
  expCrossSectionsStops.append(lr.expLimit)
  obsCrossSectionsStops.append(lr.obsLimit)
# staus
massesStaus = []
expCrossSectionsStaus = []
obsCrossSectionsStaus = []
for lr in LimitResultsStaus:
  massesStaus.append(lr.mass)
  expCrossSectionsStaus.append(lr.expLimit)
  obsCrossSectionsStaus.append(lr.obsLimit)

    

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

Lumi=4976
X=0.40
Y=0.995
W=0.82
H=0.945
text = TPaveText(X,Y,W,H, "NDC")
text.SetFillColor(0)
text.SetTextAlign(22)
text.AddText("CMS Preliminary   #sqrt{s} = 7 TeV   "+str(Lumi)+" pb^{-1}")
text.SetBorderSize(0)
text.Draw("same")

LEGTh = TLegend(0.2,0.7,0.42,0.9)
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

LEGTk = TLegend(0.7,0.7,0.9,0.9)
#LEGTk->SetHeader("95% C.L. Limits")
LEGTk.SetHeader("")
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
c1.Print('gluinosStopsStaus.pdf')




