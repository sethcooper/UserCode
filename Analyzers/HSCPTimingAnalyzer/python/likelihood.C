#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooExtendPdf.h"
#include <iostream>
#include <fstream>
#include "TFile.h"
#include "TH1.h"
#include "TStyle.h"
#include "RooRealVar.h"
#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooAddPdf.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include <memory>
#include "TMath.h"
#include "RooAbsPdf.h"
#include "TF1.h"
#include "LandauGauss.h"

using namespace RooFit;

void SetEStyle()
{
  TStyle* EStyle = new TStyle("EStyle", "E's not Style");

  //set the background color to white
  EStyle->SetFillColor(10);
  EStyle->SetFrameFillColor(10);
  EStyle->SetFrameFillStyle(0);
  EStyle->SetFillStyle(0);
  EStyle->SetCanvasColor(10);
  EStyle->SetPadColor(10);
  EStyle->SetTitleFillColor(0);
  EStyle->SetStatColor(10);

  //dont put a colored frame around the plots
  EStyle->SetFrameBorderMode(0);
  EStyle->SetCanvasBorderMode(0);
  EStyle->SetPadBorderMode(0);

  //use the primary color palette
  EStyle->SetPalette(1,0);
  EStyle->SetNumberContours(255);

  //set the default line color for a histogram to be black
  EStyle->SetHistLineColor(kBlack);

  //set the default line color for a fit function to be red
  EStyle->SetFuncColor(kRed);

  //make the axis labels black
  EStyle->SetLabelColor(kBlack,"xyz");

  //set the default title color to be black
  EStyle->SetTitleColor(kBlack);
   
  // Sizes

  //For Small Plot needs
  //set the margins
 //  EStyle->SetPadBottomMargin(.2);
//   EStyle->SetPadTopMargin(0.08);
//   EStyle->SetPadLeftMargin(0.12);
//   EStyle->SetPadRightMargin(0.12);

//   //set axis label and title text sizes
//   EStyle->SetLabelSize(0.06,"xyz");
//   EStyle->SetTitleSize(0.06,"xyz");
//   EStyle->SetTitleOffset(1.,"x");
//   EStyle->SetTitleOffset(.9,"yz");
//   EStyle->SetStatFontSize(0.04);
//   EStyle->SetTextSize(0.06);
//   EStyle->SetTitleBorderSize(0.5);
  

  //set the margins
  EStyle->SetPadBottomMargin(.15);
  EStyle->SetPadTopMargin(0.08);
  EStyle->SetPadLeftMargin(0.14);
  EStyle->SetPadRightMargin(0.12);
  
  //set axis label and title text sizes
  EStyle->SetLabelSize(0.04,"xyz");
  EStyle->SetTitleSize(0.06,"xyz");
  EStyle->SetTitleOffset(1.,"x");
  EStyle->SetTitleOffset(1.1,"yz");
  EStyle->SetStatFontSize(0.04);
  EStyle->SetTextSize(0.04);
  EStyle->SetTitleBorderSize(0.5);
  //EStyle->SetTitleY(0.5);
  
  //set line widths
  EStyle->SetHistLineWidth(1);
  EStyle->SetFrameLineWidth(2);
  EStyle->SetFuncWidth(2);

  //Paper Size
  EStyle->SetPaperSize(TStyle::kUSLetter);

  // Misc

  //align the titles to be centered
  //Style->SetTextAlign(22);

  //set the number of divisions to show
  EStyle->SetNdivisions(506, "xy");

  //turn off xy grids
  EStyle->SetPadGridX(0);
  EStyle->SetPadGridY(0);

  //set the tick mark style
  EStyle->SetPadTickX(1);
  EStyle->SetPadTickY(1);

  //show the fit parameters in a box
  EStyle->SetOptFit(111111);

  //turn on all other stats
   //EStyle->SetOptStat(0000000);
  EStyle->SetOptStat(1111111);

  //Move stats box
  EStyle->SetStatX(0.75);

  //marker settings
  EStyle->SetMarkerStyle(8);
  EStyle->SetMarkerSize(0.8);
   
  // Fonts
   EStyle->SetStatFont(42);
   EStyle->SetLabelFont(42,"xyz");
   EStyle->SetTitleFont(42,"xyz");
   EStyle->SetTextFont(42);
//  EStyle->SetStatFont(82);
//   EStyle->SetLabelFont(82,"xyz");
//   EStyle->SetTitleFont(82,"xyz");
//   EStyle->SetTextFont(82);


  //done
  EStyle->cd();
}

//XXX: Main
int main(int argc, char* argv[])
{
  using namespace std;

  char* infile = argv[1];
  if (!infile)
  {
    cout << " No input file specified !" << endl;
    return -1;
  }

  std::auto_ptr<TFile> f(new TFile(infile));
  string dirName="hscpTimingAnalyzer/";
  string histName = "summedEnergyOfTrackMatchedHits";
  string fullPath = dirName+histName;
  TH1D* energyHist = 0;
  f->GetObject(fullPath.c_str(),energyHist);
  if(!energyHist)
  {
    cout << "No hist with name " << histName << " found in file: " << infile << endl;
    return -1;
  }
  string histName2 = "timeOfTrackMatchedHits";
  fullPath = dirName+histName2;
  TH1D* timeHist = 0;
  f->GetObject(fullPath.c_str(),timeHist);
  if(!timeHist)
  {
    cout << "No hist with name " << histName2 << " found in file: " << infile << endl;
    return -1;
  }

  cout << "****************BEGINNING FIT FOR FILE: " << infile << endl;

  //SetEStyle();
  gStyle->SetMarkerSize(0.5);
  gStyle->SetStatX(0.4);
  gStyle->SetStatW(0.3);

  energyHist->Rebin();
  timeHist->Rebin();  // to 250 bins
  energyHist->Rebin();
  timeHist->Rebin();  // to 125 bins
  timeHist->Rebin();  // to 50 bins, each 1 ns wide; sigma is about 3 ns

  RooRealVar energy("energy","energy",0,10,"GeV");
  RooDataHist energyData("energyData","dataset with energy",energy,energyHist);
  RooRealVar time("time","time",-25,25, "ns");
  RooDataHist timeData("TimeData","dataset with time",time,timeHist);

  //RooRealVar xLandauEnergy("x","x",0,5);
  //RooRealVar xGaussianEnergy("x","x",0,5);
  //RooRealVar xGaussianTimeMuons("x","x",-25,25);
  //RooRealVar xGaussianTimeHSCPs("x","x",-25,25);
  // Hard-coding the langaus parameters for Pt=50 GeV muons
  //RooRealVar langausEnergyWidth("langausWidth","landgausWidth",0.01309);
  //RooRealVar langausEnergyMP("langausMP","langausMP",0.281);
  //RooRealVar langausEnergyArea("langausArea","langausArea",15.21);
  //RooRealVar langausEnergyGSigma("langausGSigma","langausGSigma",0.06304);
  //// Hard-coding the gaussian parameters for KKTau300 and the muons
  //RooRealVar gaussianEnergyMean("geMean","geMean",0.5188);
  //RooRealVar gaussianEnergySigma("geSigma","geSigma",0.05863);
  //RooRealVar gaussianTimeMuonsMean("gmMean","gmMean",2.473);
  //RooRealVar gaussianTimeMuonsSigma("gmSigma","gmSigma",3.084);
  //RooRealVar gaussianTimeHSCPsMean("ghMean","ghMean",6.39);
  //RooRealVar gaussianTimeHSCPsSigma("ghSigma","ghSigma",2.472);

  RooRealVar langausEnergyWidth("langausWidth","landgausWidth",0.005,0.025);
  RooRealVar langausEnergyMP("langausMP","langausMP",0.200,0.400);
  RooRealVar langausEnergyArea("langausArea","langausArea",1,25);
  RooRealVar langausEnergyGSigma("langausGSigma","langausGSigma",0.030,0.070);
  RooRealVar gaussianEnergyMean("geMean","geMean",0.45,0.65);
  RooRealVar gaussianEnergySigma("geSigma","geSigma",0.04,0.07);
  RooRealVar gaussianTimeMuonsMean("gmMean","gmMean",1.5,3.5);
  RooRealVar gaussianTimeMuonsSigma("gmSigma","gmSigma",3,8);
  RooRealVar gaussianTimeHSCPsMean("ghMean","ghMean",5,8);
  RooRealVar gaussianTimeHSCPsSigma("ghSigma","ghSigma",2,7);
  //name,title,variable,mean,sigma
  //RooLandau* landauEnergy = new RooLandau("landauEnergy","landauEnergy",energy,landauEnergyMean,landauEnergySigma);
  LandauGauss* langausEnergy = new LandauGauss("lanGausEnergy","lanGausEnergy",energy,langausEnergyWidth,langausEnergyMP,langausEnergyArea,langausEnergyGSigma);
  RooGaussian* gaussianEnergy = new RooGaussian("gaussianEnergy","gaussianEnergy",energy,gaussianEnergyMean,gaussianEnergySigma);
  RooGaussian* gaussianTimeMuons = new RooGaussian("gaussianTimeMuons","gaussianTimeMuons",time,gaussianTimeMuonsMean,gaussianTimeMuonsSigma);
  RooGaussian* gaussianTimeHSCPs = new RooGaussian("gaussianTimeHSCPs","gaussianTimeHSCPs",time,gaussianTimeHSCPsMean,gaussianTimeHSCPsSigma);

  RooRealVar sigFracEne("sigFracE","signal fraction (energy)",0.5,0,1);
  RooAddPdf energyModel("energyModel","energyModel",RooArgList(*gaussianEnergy,*langausEnergy),sigFracEne);

  RooRealVar sigFracTime("sigFracT","signal fraction (time)",0.5,0,1);
  RooAddPdf timeModel("timeModel","timeModel",RooArgList(*gaussianTimeHSCPs,*gaussianTimeMuons),sigFracTime);

  //Plot once before fitting
  TCanvas* combinedCanvasBef = new TCanvas("combinedCanvasBef","combinedCanvasBef",1,1,1800,600);
  combinedCanvasBef->Divide(2,1);
  combinedCanvasBef->cd(1);
  //Plot energy curves
  RooPlot* energyFrameBef = energy.frame();
  energyData.plotOn(energyFrameBef);
  energyModel.plotOn(energyFrameBef);
  energyModel.plotOn(energyFrameBef,Components("gaussianEnergy"),LineStyle(kDashed));
  energyModel.paramOn(energyFrameBef);
  energyFrameBef->Draw("e0");
  combinedCanvasBef->cd(2);
  //Plot time curves
  RooPlot* timeFrameBef = time.frame();
  timeData.plotOn(timeFrameBef);
  timeModel.plotOn(timeFrameBef);
  timeModel.plotOn(timeFrameBef,Components("gaussianTimeHSCPs"),LineStyle(kDashed));
  timeModel.paramOn(timeFrameBef);
  timeFrameBef->Draw("e0");
  combinedCanvasBef->Print("plotLikelihoodsBeforeFit.png");

  // Fit, including maximizing signal fraction
  energyModel.fitTo(energyData);
  timeModel.fitTo(timeData);

  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,1800,600);
  combinedCanvas->Divide(2,1);
  combinedCanvas->cd(1);
  //Plot energy curves
  RooPlot* energyFrame = energy.frame();
  energyData.plotOn(energyFrame);
  energyModel.plotOn(energyFrame);
  energyModel.plotOn(energyFrame,Components("gaussianEnergy"),LineStyle(kDashed));
  energyModel.paramOn(energyFrame,Layout(0.55));
  energyFrame->Draw("e0");
  combinedCanvas->cd(2);
  //Plot time curves
  gStyle->SetStatX(0.8);
  RooPlot* timeFrame = time.frame();
  timeData.plotOn(timeFrame);
  timeModel.plotOn(timeFrame);
  timeModel.plotOn(timeFrame,Components("gaussianTimeHSCPs"),LineStyle(kDashed));
  timeModel.paramOn(timeFrame);
  timeFrame->Draw("e0");
  //Print
  combinedCanvas->Print("plotLikelihoods.png");


  //Loop over hists: energyHist, timeHist and calculate the likelihood
  float logEnergyLikelihood = 0;
  float logEnergyLikelihoodBack = 0;
  RooArgSet* energyArgSet = new RooArgSet(energy);
  for(int bin = 1; bin < energyHist->GetNbinsX(); bin++)
  {
    int numBinEntries = energyHist->GetBinContent(bin);
    energy = energyHist->GetBinCenter(bin);
    if(numBinEntries > 0)
    {
      logEnergyLikelihood+=log(energyModel.getVal(energyArgSet))*numBinEntries;
      logEnergyLikelihoodBack+=log(langausEnergy->getVal(energyArgSet))*numBinEntries;
    }
    //debug
    //if(numBinEntries > 0)
    //{
    //  cout << "Energy bin entries: " << numBinEntries << ".  Probs at " << energyHist->GetBinCenter(bin) << ": " << endl;
    //  cout << "\t S+B prob: " << energyModel.getVal(energyArgSet) << endl;
    //  cout << "\t B only prob: " << langausEnergy->getVal(energyArgSet) << endl;
    //  cout << "\t S+B - B prob: " << energyModel.getVal(energyArgSet) - langausEnergy->getVal(energyArgSet) << endl;
    //  cout << "\t S+B LL so far: " << logEnergyLikelihood << " vs. B only LL so far: " << logEnergyLikelihoodBack << endl;
    //  //cout << "Total likelihood at bin: " << log(energyModel.getVal())*numBinEntries << " likelihood so far: " << logEnergyLikelihood << endl;
    //}
  }
  float logTimeLikelihood = 0;
  float logTimeLikelihoodBack = 0;
  RooArgSet* timeArgSet = new RooArgSet(time);
  for(int bin = 1; bin < timeHist->GetNbinsX(); bin++)
  {
    int numBinEntries = timeHist->GetBinContent(bin);
    time = timeHist->GetBinCenter(bin);
    if(numBinEntries > 0)
    {
      logTimeLikelihood+=log(timeModel.getVal(timeArgSet))*numBinEntries;
      logTimeLikelihoodBack+=log(gaussianTimeMuons->getVal(timeArgSet))*numBinEntries;
    }
  }

  delete energyArgSet;
  delete timeArgSet;
  f->Close();

  cout << "Energy likelihood for S+B model: " << logEnergyLikelihood << " Likelihood for B only model: " << logEnergyLikelihoodBack << endl;
  cout << "Energy (log) likelihood difference: " << logEnergyLikelihood-logEnergyLikelihoodBack << endl;
  cout << "Time likelihood for S+B model: " << logTimeLikelihood << " Likelihood for B only model: " << logTimeLikelihoodBack << endl;
  cout << "Time (log) likelihood difference: " << logTimeLikelihood-logTimeLikelihoodBack << endl;
  
  ofstream outputStream("likelihoodDifferences_muons.txt",ios::app);
  if(!outputStream.is_open())
  {
    cout << "ERROR: Unable to write to file (file is not open)." << endl;
    return -1;
  }
  if(!outputStream.good())
  {
    cout << "ERROR: Unable to write to file (file is not open)." << endl;
    return -1;
  }

  // Get to the end of the file
  outputStream.seekp(0,ios::end);
  // Energy, tab, time
  outputStream << logEnergyLikelihood-logEnergyLikelihoodBack << "\t" << logTimeLikelihood-logTimeLikelihoodBack << endl;
  outputStream.close();

  return 0;
}
