#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooExtendPdf.h"
#include <iostream>
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
  //EStyle->SetStatX(0.85);

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



int main(int argc, char* argv[])
{
  using namespace std;

  char* infile = argv[1];
  if (!infile)
  {
    cout << " No input file specified !" << endl;
    return -1;
  }

  TFile* f = new TFile(infile);
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

  RooRealVar energy("energy","energy",0,10);
  RooDataHist energyData("energyData","dataset with energy",energy,energyHist);
  RooRealVar time("time","time",-25,25);
  RooDataHist timeData("TimeData","dataset with time",time,timeHist);

  RooRealVar xLandauEnergy("x","x",0,5);
  RooRealVar xGaussianEnergy("x","x",0,5);
  RooRealVar xGaussianTimeMuons("x","x",-25,25);
  RooRealVar xGaussianTimeHSCPs("x","x",-25,25);
  RooRealVar landauEnergyMean("leMean","leMean",0.2654);
  RooRealVar landauEnergySigma("leSigma","leSigma",0.03383);
  RooRealVar gaussianEnergyMean("geMean","geMean",0.5188);
  RooRealVar gaussianEnergySigma("geSigma","geSigma",0.05863);
  RooRealVar gaussianTimeMuonsMean("gmMean","gmMean",2.658);
  RooRealVar gaussianTimeMuonsSigma("gmSigma","gmSigma",3.655);
  RooRealVar gaussianTimeHSCPsMean("ghMean","ghMean",6.39);
  RooRealVar gaussianTimeHSCPsSigma("ghSigma","ghSigma",2.472);
  //name,title,variable,mean,sigma
  RooLandau* landauEnergy = new RooLandau("landauEnergy","landauEnergy",energy,landauEnergyMean,landauEnergySigma);
  RooGaussian* gaussianEnergy = new RooGaussian("gaussianEnergy","gaussianEnergy",energy,gaussianEnergyMean,gaussianEnergySigma);
  RooGaussian* gaussianTimeMuons = new RooGaussian("gaussianTimeMuons","gaussianTimeMuons",xGaussianTimeMuons,gaussianTimeMuonsMean,gaussianTimeMuonsSigma);
  RooGaussian* gaussianTimeHSCPs = new RooGaussian("gaussianTimeHSCPs","gaussianTimeHSCPs",xGaussianTimeHSCPs,gaussianTimeHSCPsMean,gaussianTimeHSCPsSigma);

  RooRealVar fsig("fsig","signal fraction",0.5,0,1);
  RooAddPdf energyModel("model","model",RooArgList(*gaussianEnergy,*landauEnergy),fsig);

  //RooFitResult* fitRes= energyModel.fitTo(energyData);
  //fitRes->Print();
  energyModel.fitTo(energyData);

  TCanvas* canvas = new TCanvas();
  canvas->cd();

  RooPlot* frame = energy.frame();
  energyData.plotOn(frame);
  energyModel.plotOn(frame);
  energyModel.plotOn(frame,Components("gaussianEnergy"),LineStyle(kDashed));
  energyModel.paramOn(frame);
  frame->Draw("e0");
  canvas->Print("testGraph.png");

  f->Close();
  return 0;
}
