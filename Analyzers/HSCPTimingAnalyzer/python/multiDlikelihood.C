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
#include "TTree.h"
#include "TCut.h"
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
#include "RooGenericPdf.h"
#include "LandauGauss.h"
#include "RooPolyVar.h"

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

  RooRealVar energy("energy","energy",0,2.5,"GeV");
  RooRealVar time("time","time",-25,25, "ns");
  RooRealVar chi2("chi2","chi2",0,10);

  //TFile* myfile = new TFile("tmp.root","RECREATE");
  //myfile->cd();
  //RooFormulaVar myForm("myForm","chi2 > 0",RooArgSet(chi2));
  //RooFormulaVar myForm("myForm","chi2 < 1",RooArgSet(chi2));
  RooDataSet* energyTimeChi2Data = new RooDataSet("energyTimeChi2Data","energyChi2TimeData",energyTimeTNtuple,RooArgSet(energy,time,chi2));
  TCut chi2Cut1 = "chi2 < 0.96227527";
  TCut chi2Cut2 = "chi2 > 0";
  RooDataSet* energyTimeDataChi2cut = (RooDataSet*) energyTimeChi2Data->reduce(chi2Cut1);
  RooDataSet* energyTimeData = new RooDataSet("energyTimeData","energyTimeData",energyTimeDataChi2cut,RooArgSet(energy,time));
  //TCut energyCut = "energy > 0.320";
  //RooDataSet* energyTimeData = (RooDataSet*) energyTimeDataRaw->reduce(energyCut);

  // 1-D datasets
  RooDataSet* energyData = new RooDataSet("energyData","energyData",energyTimeData,RooArgSet(energy));
  RooDataSet* timeData = new RooDataSet("timeDataRaw","timeDataRaw",energyTimeData,RooArgSet(time));

  int numEntries = energyTimeData->numEntries();
  cout << "****************Created RooDataSet with " << numEntries << " entries!" << endl;

  //RooRealVar xLandauEnergy("x","x",0,5);
  //RooRealVar xGaussianEnergy("x","x",0,5);
  //RooRealVar xGaussianTimeMuons("x","x",-25,25);
  //RooRealVar xGaussianTimeHSCPs("x","x",-25,25);
  // Hard-coding the langaus parameters for Pt=50 GeV muons
  RooRealVar langausEnergyWidth("langausWidth","landgausWidth",0.01309);
  RooRealVar langausEnergyMP("langausMP","langausMP",0.281);
  RooRealVar langausEnergyArea("langausArea","langausArea",15.21);
  RooRealVar langausEnergyGSigma("langausGSigma","langausGSigma",0.06304);
  //// Hard-coding the gaussian parameters for KKTau300 and the muons
  RooRealVar gaussianEnergyMean("geMean","geMean",0.5188);
  RooRealVar gaussianEnergySigma("geSigma","geSigma",0.05863);
  RooRealVar gaussianTimeMuonsMean("gmMean","gmMean",2.473);
  RooRealVar gaussianTimeMuonsSigma("gmSigma","gmSigma",3.084);
  RooRealVar gaussianTimeHSCPsMean("ghMean","ghMean",6.39);
  RooRealVar gaussianTimeHSCPsSigma("ghSigma","ghSigma",2.472);

  //RooRealVar gaussianEnergyMean("E_hgMean","E_hgMean",0.5167);
  //RooRealVar gaussianEnergySigma("E_hgSigma","E_hgSigma",0.06243);
  //RooRealVar langausEnergyArea("E_langausArea","E_langausArea",75.53);
  //RooRealVar langausEnergyGSigma("E_langausGSigma","E_langausGSigma",0.03954);
  //RooRealVar langausEnergyMP("E_langausMP","E_langausMP",0.317);
  //RooRealVar langausEnergyWidth("E_langausWidth","E_landgausWidth",0.01606);
  //RooRealVar gaussianTimeHSCPsMean("T_hMean","T_hMean",6.538);
  //RooRealVar gaussianTimeHSCPsSigma("T_hSigma","T_hSigma",2.291);
  //RooRealVar gaussianTimeMuonsMean("T_muMean","T_muMean",2.338);
  //RooRealVar gaussianTimeMuonsSigma("T_muSigma","T_muSigma",2.991);

  //TODO: for testing
  RooRealVar a0("a0","a0",5) ;
  RooRealVar a1("a1","a1",1) ;
  RooPolyVar fy("fy","fy",energy,RooArgSet(a0,a1)) ;

  RooRealVar a("a","a",1.5766); // (0.042 GeV)/E = A/sigma
  RooRealVar b("b","b",0.3527);
  //RooGenericPdf timingRes("timingRes","timingRes","sqrt((a/energy)*(a/energy)+b)",RooArgSet(energy,a,b)); // in nanoseconds
  //RooGenericPdf timingRes("timingRes","a0+a1*energy",RooArgSet(a0,a1,energy)); // in nanoseconds
  RooFormulaVar timingRes("timingRes","sqrt(b+a/energy^2)",RooArgSet(energy,b,a));
  //name,title,variable,mean,sigma
  LandauGauss* langausEnergy = new LandauGauss("lanGausEnergy","lanGausEnergy",energy,langausEnergyWidth,langausEnergyMP,langausEnergyArea,langausEnergyGSigma);
  RooGaussian* gaussianEnergy = new RooGaussian("gaussianEnergy","gaussianEnergy",energy,gaussianEnergyMean,gaussianEnergySigma);
  RooGaussian* gaussianTimeMuons = new RooGaussian("gaussianTimeMuons","gaussianTimeMuons",time,gaussianTimeMuonsMean,timingRes);
  RooGaussian* gaussianTimeHSCPs = new RooGaussian("gaussianTimeHSCPs","gaussianTimeHSCPs",time,gaussianTimeHSCPsMean,timingRes);

  // Make product PDF of energy and time, S+B
  RooRealVar sigFrac("sigFrac","signal fraction",0.5,0,1);
  RooAddPdf energyModel("energyModel","energyModel",RooArgList(*gaussianEnergy,*langausEnergy),sigFrac);
  RooAddPdf timeModel("timeModel","timeModel",RooArgList(*gaussianTimeHSCPs,*gaussianTimeMuons),sigFrac);
  RooProdPdf energyAndTimeModel("energyAndTimeModel","energyAndTimeModel",energyModel,Conditional(timeModel,time));
  //For testing
  //RooProdPdf energyAndTimeModel("energyAndTimeModel","energyAndTimeModel",*gaussianEnergy,Conditional(*gaussianTimeHSCPs,time));

  // Make background-only model
  LandauGauss* langausEnergyBack = new LandauGauss("lanGausEnergyBack","lanGausEnergyBack",energy,langausEnergyWidth,langausEnergyMP,langausEnergyArea,langausEnergyGSigma);
  RooGaussian* gaussianTimeMuonsBack = new RooGaussian("gaussianTimeMuonsBack","gaussianTimeMuonsBack",time,gaussianTimeMuonsMean,timingRes);
  RooProdPdf energyAndTimeBackModel("energyAndTimeBackModel","energyAndTimeBackModel",*langausEnergyBack,Conditional(*gaussianTimeMuonsBack,time));

  // 1-D model vars -- background
  RooRealVar backEnergyArea("E_backArea","E_backArea",75.53);
  RooRealVar backEnergyGSigma("E_backGSigma","E_backGSigma",0.03954);
  RooRealVar backEnergyMP("E_backMP","E_backMP",0.317);
  RooRealVar backEnergyWidth("E_backWidth","E_backWidth",0.01606);
  RooRealVar backTimeMean("T_backMean","T_backMean",2.338);
  RooRealVar backTimeSigma("T_backSigma","T_backSigma",2.991); 
  //LandauGauss* backEnergyModel1D = new LandauGauss("backEnergyModel1D","backEnergyModel1D",energy,backEnergyWidth,backEnergyMP,backEnergyArea,backEnergyGSigma);
  //RooGaussian* backTimeModel1D = new RooGaussian("backTimeModel1D","backTimeModel1D",time,backTimeMean,backTimeSigma);

  // 1-D model vars -- background+signal
  RooRealVar gaussianEnergyMean1("E_hgMean1","E_hgMean1",0.5167);
  RooRealVar gaussianEnergySigma1("E_hgSigma1","E_hgSigma1",0.06243);
  RooRealVar langausEnergyArea1("E_langausArea1","E_langausArea1",75.53);
  RooRealVar langausEnergyGSigma1("E_langausGSigma1","E_langausGSigma1",0.03954);
  RooRealVar langausEnergyMP1("E_langausMP1","E_langausMP1",0.317);
  RooRealVar langausEnergyWidth1("E_langausWidth1","E_landgausWidth1",0.01606);
  RooRealVar gaussianTimeHSCPsMean1("T_hMean1","T_hMean1",6.538);
  RooRealVar gaussianTimeHSCPsSigma1("T_hSigma1","T_hSigma1",2.291);
  RooRealVar gaussianTimeMuonsMean1("T_muMean1","T_muMean1",2.338);
  RooRealVar gaussianTimeMuonsSigma1("T_muSigma1","T_muSigma1",2.991);
  LandauGauss* langausEnergy1D = new LandauGauss("lanGausEnergy1D","lanGausEnergy1D",energy,langausEnergyWidth1,langausEnergyMP1,langausEnergyArea1,langausEnergyGSigma1);
  RooGaussian* gaussianEnergy1D = new RooGaussian("gaussianEnergy1D","gaussianEnergy1D",energy,gaussianEnergyMean1,gaussianEnergySigma1);
  RooGaussian* gaussianTimeMuons1D = new RooGaussian("gaussianTimeMuons1D","gaussianTimeMuons1D",time,gaussianTimeMuonsMean1,gaussianTimeMuonsSigma1);
  RooGaussian* gaussianTimeHSCPs1D = new RooGaussian("gaussianTimeHSCPs1D","gaussianTimeHSCPs1D",time,gaussianTimeHSCPsMean1,gaussianTimeHSCPsSigma1);
  RooRealVar sigFracE("sigFracE","signal fractionE",0.5,0,1);
  RooRealVar sigFracT("sigFracT","signal fractionT",0.5,0,1);
  RooAddPdf energyModel1D("energyModel1D","energyModel1D",RooArgList(*gaussianEnergy1D,*langausEnergy1D),sigFracE);
  RooAddPdf timeModel1D("timeModel1D","timeModel1D",RooArgList(*gaussianTimeHSCPs1D,*gaussianTimeMuons1D),sigFracT);

  // Fit, maximizing signal fraction
  energyAndTimeModel.fitTo(*energyTimeData);
  energyModel1D.fitTo(*energyData);
  timeModel1D.fitTo(*timeData);
  //TODO: fix this background model fitting issue...
  // Well, you can't fit a function with fixed parameters...
  //energyAndTimeBackModel.fitTo(*energyTimeData);
  //backEnergyModel1D.fitTo(*energyData);
  // backTimeModel1D.fitTo(*timeData);

  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,2500,1000);
  combinedCanvas->Divide(3,1);
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
  energyAndTimeModel.paramOn(timeFrame,Layout(0.55));
  timeFrame->Draw("e0");

  combinedCanvas->cd(3);
  //Make 2-D plot of PDF
  TH1* hh_model = energyAndTimeModel.createHistogram("hh_model",energy,Binning(50),YVar(time,Binning(50))) ;
  hh_model->SetLineColor(kBlue);
  //TCanvas* modelCanvas = new TCanvas("2DCanvas","2DCanvas",600,600);
  //modelCanvas->cd();
  hh_model->Draw("surf");
  //energyTimeData->Draw("energy");
  //energyTimeData->Draw("time");
  //Print
  combinedCanvas->Print("plotLikelihoods.png");
  //modelCanvas->Print("plotLikelihoods2D_data.png");

  // 1-D plots
  TCanvas* combinedCanvas1D = new TCanvas("combinedCanvas1D","combinedCanvas1D",1,1,1800,2000);
  combinedCanvas1D->Divide(1,2);
  combinedCanvas1D->cd(1);
  RooPlot* energyFrame2 = energy.frame();
  energyData->plotOn(energyFrame2);
  energyModel1D.plotOn(energyFrame2);
  energyModel1D.plotOn(energyFrame2,Components("gaussianEnergy1D"),LineStyle(kDashed));
  energyModel1D.paramOn(energyFrame2,Layout(0.55));
  energyFrame2->Draw("e0");
  combinedCanvas1D->cd(2);
  RooPlot* timeFrame2 = time.frame();
  timeData->plotOn(timeFrame2);
  timeModel1D.plotOn(timeFrame2);
  timeModel1D.plotOn(timeFrame2,Components("gaussianTimeHSCPs1D"),LineStyle(kDashed));
  timeModel1D.paramOn(timeFrame2,Layout(0.7));
  timeFrame2->Draw("e0");
  //Print
  combinedCanvas1D->Print("plotLikelihoods1D.png");

  //Loop over hists: energyHist, timeHist and calculate the likelihood
  float logLikelihood = 0;
  float logLikelihoodBack = 0;
  float ntupEnergy, ntupTime;
  energyTimeTNtuple->SetBranchAddress("energy",&ntupEnergy);
  energyTimeTNtuple->SetBranchAddress("time",&ntupTime);
  for(int entry = 1; entry < energyTimeTNtuple->GetEntries(); entry++)
  {
    energyTimeTNtuple->GetEntry(entry);
    energy = ntupEnergy;
    time = ntupTime;
    logLikelihood+=log(energyAndTimeModel.getVal(new RooArgSet(energy,time)));
    logLikelihoodBack+=log(energyAndTimeBackModel.getVal(new RooArgSet(energy,time)));
    //std::cout << "LogLikelihood = " << log(energyAndTimeModel.getVal(energyTimeArgSet)) << std::endl;
    //std::cout << "LogLikelihoodBack = " << log(energyAndTimeBackModel.getVal(energyTimeArgSet)) << std::endl;
  }

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
