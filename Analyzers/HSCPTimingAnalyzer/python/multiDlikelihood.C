#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include <iostream>
#include <memory>
#include <fstream>
#include "TFile.h"
#include "TH1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TF1.h"
#include "TNtuple.h"
#include "TTree.h"
#include "RooExtendPdf.h"
#include "RooRealVar.h"
#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooDataSet.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooAbsPdf.h"
#include "RooProdPdf.h"
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
  EStyle->SetTitleBorderSize(Width_t(0.5));
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

  TFile* f = new TFile(infile);
  //string dirName="hscpTimingAnalyzer/";
  string ntupleName = "energyAndTimeNTuple";
  //string fullPath = dirName+ntupleName;
  TTree* energyTimeTNtuple = (TTree*)f->Get(ntupleName.c_str());
  if(!energyTimeTNtuple)
  {
    cout << "****************No TNtuplewith name " << ntupleName << " found in file: " << infile << endl;
    return -1;
  }
  else
    cout << "****************TNtuple found with " << energyTimeTNtuple->GetEntries() << " entries." << endl;

  cout << "****************Beginning fit for file: " << infile << endl;

  //SetEStyle();
  gStyle->SetMarkerSize(0.5);
  gStyle->SetStatX(0.4);
  gStyle->SetStatW(0.3);

  RooRealVar energy("energy","energy",0,2,"GeV");
  RooRealVar time("time","time",-25,25, "ns");

  //TFile* myfile = new TFile("tmp.root","RECREATE");
  //myfile->cd();
  RooDataSet* energyTimeData = new RooDataSet("energyTimeData","energyTimeData",energyTimeTNtuple,RooArgSet(energy,time));//,"energy>0.400");

  cout << "****************Created RooDataSet" << endl;

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
  LandauGauss* langausEnergy = new LandauGauss("lanGausEnergy","lanGausEnergy",energy,langausEnergyWidth,langausEnergyMP,langausEnergyArea,langausEnergyGSigma);
  RooGaussian* gaussianEnergy = new RooGaussian("gaussianEnergy","gaussianEnergy",energy,gaussianEnergyMean,gaussianEnergySigma);
  RooGaussian* gaussianTimeMuons = new RooGaussian("gaussianTimeMuons","gaussianTimeMuons",time,gaussianTimeMuonsMean,gaussianTimeMuonsSigma);
  RooGaussian* gaussianTimeHSCPs = new RooGaussian("gaussianTimeHSCPs","gaussianTimeHSCPs",time,gaussianTimeHSCPsMean,gaussianTimeHSCPsSigma);

  // Make product PDF of energy and time, S+B
  RooRealVar sigFrac("sigFrac","signal fraction",0.5,0,1);
  RooAddPdf energyModel("energyModel","energyModel",RooArgList(*gaussianEnergy,*langausEnergy),sigFrac);
  RooAddPdf timeModel("timeModel","timeModel",RooArgList(*gaussianTimeHSCPs,*gaussianTimeMuons),sigFrac);
  RooProdPdf energyAndTimeModel("energyAndTimeModel","energyAndTimeModel",RooArgSet(energyModel,timeModel));

  // Make background-only model
  RooProdPdf energyAndTimeBackModel("energyAndTimeBackModel","energyAndTimeBackModel",RooArgSet(*langausEnergy,*gaussianTimeMuons));

  //Plot once before fitting
  //TCanvas* combinedCanvasBef = new TCanvas("combinedCanvasBef","combinedCanvasBef",1,1,1800,600);
  //combinedCanvasBef->Divide(2,1);
  //combinedCanvasBef->cd(1);
  ////Plot energy curves
  //RooPlot* energyFrameBef = energy.frame();
  //energyData.plotOn(energyFrameBef);
  //energyModel.plotOn(energyFrameBef);
  //energyModel.plotOn(energyFrameBef,Components("gaussianEnergy"),LineStyle(kDashed));
  //energyModel.paramOn(energyFrameBef);
  //energyFrameBef->Draw("e0");
  //combinedCanvasBef->cd(2);
  ////Plot time curves
  //RooPlot* timeFrameBef = time.frame();
  //timeData.plotOn(timeFrameBef);
  //timeModel.plotOn(timeFrameBef);
  //timeModel.plotOn(timeFrameBef,Components("gaussianTimeHSCPs"),LineStyle(kDashed));
  //timeModel.paramOn(timeFrameBef);
  //timeFrameBef->Draw("e0");
  //combinedCanvasBef->Print("plotLikelihoodsBeforeFit.png");

  // Fit, including maximizing signal fraction
  energyAndTimeModel.fitTo(*energyTimeData);
  energyAndTimeBackModel.fitTo(*energyTimeData);

  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,1800,600);
  combinedCanvas->Divide(2,1);
  combinedCanvas->cd(1);
  //Plot energy curves
  RooPlot* energyFrame = energy.frame();
  energyTimeData->plotOn(energyFrame);
  energyAndTimeModel.plotOn(energyFrame);
  energyAndTimeModel.plotOn(energyFrame,Components("gaussianEnergy"),LineStyle(kDashed));
  energyAndTimeModel.paramOn(energyFrame,Layout(0.55));
  energyFrame->Draw("e0");
  combinedCanvas->cd(2);
  //Plot time curves
  RooPlot* timeFrame = time.frame();
  energyTimeData->plotOn(timeFrame);
  energyAndTimeModel.plotOn(timeFrame);
  energyAndTimeModel.plotOn(timeFrame,Components("gaussianTimeHSCPs"),LineStyle(kDashed));
  energyAndTimeModel.paramOn(timeFrame);
  timeFrame->Draw("e0");
  //Print
  combinedCanvas->Print("plotLikelihoods.png");

  //Make 2-D plot of PDF
  TH1* hh_model = energyAndTimeModel.createHistogram("hh_model",energy,Binning(50),YVar(time,Binning(50))) ;
  hh_model->SetLineColor(kBlue);
  TCanvas* modelCanvas = new TCanvas("2DCanvas","2DCanvas",400,400);
  modelCanvas->cd();
  hh_model->Draw("surf");
  //energyTimeData->Draw();
  modelCanvas->Print("plotLikelihoods2D_data.png");

  //Loop over hists: energyHist, timeHist and calculate the likelihood
  float logLikelihood = 0;
  float logLikelihoodBack = 0;
  float ntupEnergy, ntupTime;
  energyTimeTNtuple->SetBranchAddress("energy",&ntupEnergy);
  energyTimeTNtuple->SetBranchAddress("time",&ntupTime);
  RooArgSet* energyTimeArgSet = new RooArgSet(energy,time);
  for(int entry = 1; entry < energyTimeTNtuple->GetEntries(); entry++)
  {
    energyTimeTNtuple->GetEntry(entry);
    energy = ntupEnergy;
    time = ntupTime;
    logLikelihood+=log(energyAndTimeModel.getVal(energyTimeArgSet));
    logLikelihoodBack+=log(energyAndTimeBackModel.getVal(energyTimeArgSet));
  }

  delete energyTimeArgSet;
  f->Close();

  cout << "Likelihood for S+B model: " << logLikelihood << " Likelihood for B only model: " << logLikelihoodBack << endl;
  cout << "Log likelihood difference: " << logLikelihood-logLikelihoodBack << endl;
  
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
  outputStream << logLikelihood-logLikelihoodBack << std::endl;
  outputStream.close();

  return 0;
}
