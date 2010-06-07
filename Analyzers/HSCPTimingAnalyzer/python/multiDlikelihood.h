#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif

#include <iostream>
#include <sstream>
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
#include "TGraphErrors.h"
#include "TPaveLabel.h"

#include "RooExtendPdf.h"
#include "RooRealVar.h"
#include "RooLandau.h"
#include "RooGaussian.h"
#include "RooBreitWigner.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooAddPdf.h"
#include "RooPlot.h"
#include "RooFitResult.h"
#include "RooAbsPdf.h"
#include "RooProdPdf.h"
#include "RooGenericPdf.h"
#include "RooPolyVar.h"
#include "RooMinuit.h"
#include "RooNLLVar.h"
#include "RooChi2Var.h"
#include "RooFFTConvPdf.h"
#include "RooBinning.h"

using namespace RooFit;

// Globals
TFile* myInputRootFile_;
TTree* energyTimeTNtuple_;

// ************************ Define observables *******************************
//RooRealVar sumEnergy("sumEnergy","sumEnergy",0.4,2.5,"GeV"); // 400 MeV sumEnergy cut in 1 cry

//RooRealVar sumEnergy("sumEnergy","sumEnergy",0.2,2.5,"GeV"); // higher cut will be done with dE/dx below
// Using this instead of above for Ledovskoy, DEC 10 2009
RooRealVar sumEnergy("sumEnergy","sumEnergy",0,5,"GeV");
RooRealVar maxCryTime("maxCryTime","maxCryTime",-25,25, "ns");
RooRealVar maxCryTimeError("maxCryTimeError","maxCryTimeError",0,1000);
RooRealVar sumTrackLength("sumTrackLength","sumTrackLength",0,25,"cm");
//RooRealVar sumDeDx("sumDeDx","sumDeDx",17.4,50,"MeV/cm"); // 400 MeV sumEnergy cut in 1 cry
// Using this instead of above for Ledovskoy, DEC 10 2009
RooRealVar sumDeDx("sumDeDx","sumDeDx",0,100,"MeV/cm");

// **************** Parameters ***********************************************
//***** Floating
//RooRealVar gaussDeDxMuonMean("gaussDeDxMuonMean","gaussDeDxMuonMean",5,1,10);
//RooRealVar gaussDeDxMuonSigma("gaussDeDxMuonSigma","gaussDeDxMuonSigma",1,0.6,5);
//RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",15.1,5,25);
//RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",1.1,0.1,5);
//RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.3,-2,4);
//***** FIXED params, now based on new samples (singlecry 50 GeV pt muons) SIC NOV 6 2009
//RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",12.9);
//RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",0.5);
//RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.48);
//***** FIXED PARAMS; V2 -- based on new sample (single cry filtered muons 50 GeV pt) all run 5 files
//***** SIC DEC 10 2009
//RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",13.0);
//RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",0.59);
//RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.46);
//***** FIXED PARAMS; V# -- based on new sample (low eta 50 GeV Pt muons, correlated noise, 361)
//***** SIC May 20 2010
RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",12.58);
RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",0.8513);
RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.46);
//added back May 20 2010
//XXX: need first to make a binary that just fits the input tree for these numbers
RooRealVar gaussDeDxMuonMean("gaussDeDxMuonMean","gaussDeDxMuonMean",12.58);
RooRealVar gaussDeDxMuonSigma("gaussDeDxMuonSigma","gaussDeDxMuonSigma",2.194);

//***** Floating
//RooRealVar gaussianDeDxHSCPMean("geMean","geMean",23.,1,100);
//RooRealVar gaussianDeDxHSCPSigma("geSigma","geSigma",2.,0.1,10);
//RooRealVar gaussianTimeHSCPMean("ghMean","ghMean",6.,-2.,10);
//***** FIXED params, now based on new samples (singlecry kk tau) SIC NOV 6 2009
RooRealVar gaussianDeDxHSCPMean("geMean","geMean",23.5);
RooRealVar gaussianDeDxHSCPSigma("geSigma","geSigma",2.23);
RooRealVar gaussianTimeHSCPMean("ghMean","ghMean",6.89);

// Background-only model -- muon
RooRealVar gaussDeDxMuonMeanBack("gaussDeDxMuonMeanBack","gaussDeDxMuMeanBack",10,1,1000);
RooRealVar gaussDeDxMuonSigmaBack("gaussDeDxMuonSigmaBack","gaussDeDxMuonSigmaBack",2,0.1,5);
RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",14.1,1,1000);
RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",2.183,0.1,100);
RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.33,-6,6);
//RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",14.1); // old param values
//RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",2.183); //
//RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.33); //
//RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",13.0);
//RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",0.59);
//RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.46);

// Signal-only model -- kktau
RooRealVar gaussianDeDxHSCPMeanSignal("gHSCPdEdxMeanSignal","gHSCPdEdxMeanSignal",30,10,100);
RooRealVar gaussianDeDxHSCPSigmaSignal("gHSCPdEdxSigmaSignal","gHSCPdEdxSigmaSignal",2.8,0.5,5);
RooRealVar gaussianTimeHSCPMeanSignal("gHSCPtimeMeanSignal","gHSCPtimeMeanSignal",6.1,-2,10);
RooRealVar gaussianTimeHSCPSigmaSignal("gHSCPtimeSigmaSignal","gHSCPtimeSigmaSignal",4,0.5,10);

// **************** Create PDFs **********************************************
//RooRealVar gausTimeSig("gausTimeSig","gausTimeSig",3);//testing
//RooRealVar a("a","a",1.5766); // (0.042 GeV)/E = A/sigma
//RooRealVar b("b","b",0.3527); // these are old
RooRealVar a("a","a",0.8895); // from ecal craft timing note
RooRealVar b("b","b",0.0008);
RooRealVar ap("ap","ap",12110.); //XXX SIC April 5 1020 --  ~ 25% wider sigma for MC studies
//TODO: Update this to low-amplitude resolution fit results (SIC May 20 2010)
RooRealVar apReal("apReal","apReal",9685.); // @ 23 cm
//RooRealVar ap("ap","ap",9601.4); // @ 23.1 cm length
RooFormulaVar timingRes("timingRes","TMath::Sqrt(ap/sumDeDx^2+0.0008)",RooArgList(ap,sumDeDx)); // altered to use wider sigma
RooFormulaVar timingResReal("timingResReal","TMath::Sqrt(apReal/sumDeDx^2+0.0008)",RooArgList(apReal,sumDeDx)); // altered to use dE/dx*23 cm (ap)
//name,title,variable,mean,sigma
RooLandau landauDeDxMuon("landauDeDxMuon","landauDeDxMuon",sumDeDx,landauDeDxMuonMean,landauDeDxMuonSigma);
RooGaussian gaussianDeDxMuon("gaussianDeDxMuon","gaussianDeDxMuon",sumDeDx,gaussDeDxMuonMean,gaussDeDxMuonSigma);
//sumDeDx.setBins(10000,"cache");
//sumDeDx.setBins("fft",10000);
RooFFTConvPdf langausDeDxMuon("langausDeDxMuon","landau (X) gauss",sumDeDx,landauDeDxMuon,gaussianDeDxMuon);

RooGaussian gaussianTimeMuon("gaussianTimeMuon","gaussianTimeMuon",maxCryTime,gaussianTimeMuonMean,timingRes);
//RooBreitWigner gaussianTimeMuon("gaussianTimeMuon","gaussianTimeMuon",time,gaussianTimeMuonMean,timingResReal);
RooGaussian gaussianTimeMuonReal("gaussianTimeMuonReal","gaussianTimeMuonReal",maxCryTime,gaussianTimeMuonMean,timingResReal);

// HSCP
RooGaussian gaussianDeDxHSCP("gaussianDeDxHSCP","gaussianDeDxHSCP",sumDeDx,gaussianDeDxHSCPMean,gaussianDeDxHSCPSigma);
RooGaussian gaussianTimeHSCP("gaussianTimeHSCP","gaussianTimeHSCP",maxCryTime,gaussianTimeHSCPMean,timingResReal);
//RooBreitWigner gaussianTimeHSCP("gaussianTimeHSCP","gaussianTimeHSCP",time,gaussianTimeHSCPMean,timingResReal);
RooGaussian gaussianTimeHSCPReal("gaussianTimeHSCPReal","gaussianTimeHSCPReal",maxCryTime,gaussianTimeHSCPMean,timingResReal);

// ***************** Models **************************************************
//XXX Here, sigFrac will be amount of signal generated
RooRealVar sigFrac("sigFrac","signal fraction",0.01,0,1);
//RooAddPdf dedxModel("dedxModel","dedxModel",RooArgList(gaussianDeDxHSCP,landauDeDxMuon),sigFrac);
RooAddPdf dedxModel("dedxModel","dedxModel",RooArgList(gaussianDeDxHSCP,langausDeDxMuon),sigFrac);
RooAddPdf timeModel("timeModel","timeModel",RooArgList(gaussianTimeHSCP,gaussianTimeMuon),sigFrac);
RooAddPdf timeModelReal("timeModelReal","timeModelReal",RooArgList(gaussianTimeHSCPReal,gaussianTimeMuonReal),sigFrac);
// Signal + Background
RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",dedxModel,Conditional(timeModel,maxCryTime));
RooProdPdf dedxAndTimeModelReal("dedxAndTimeModelReal","dedxAndTimeModelReal",dedxModel,Conditional(timeModelReal,maxCryTime));
//Signal only
//RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",gaussianDeDxHSCP,Conditional(gaussianTimeHSCP,maxCryTime));
//Background only
//RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",landauDeDxMuon,Conditional(gaussianTimeMuon,maxCryTime));

// Make background-only model
RooLandau landauDeDxMuonBack("landauDeDxMuonBack","landauDeDxMuonBack",sumDeDx,landauDeDxMuonMeanBack,landauDeDxMuonSigmaBack);
RooGaussian gaussianDeDxMuonBack("gaussianDeDxMuonBack","gaussianDeDxMuonBack",sumDeDx,gaussDeDxMuonMeanBack,gaussDeDxMuonSigmaBack);
RooFFTConvPdf langausDeDxBack("langausDeDxBack","langausDeDxBack",sumDeDx,landauDeDxMuonBack,gaussianDeDxMuonBack);
RooGaussian gaussianTimeMuonBack("gaussianTimeMuonBack","gaussianTimeMuonBack",maxCryTime,gaussianTimeMuonMeanBack,timingResReal);
//RooProdPdf dedxAndTimeBackModel("dedxAndTimeBackModel","dedxAndTimeBackModel",langausDeDxBack,Conditional(gaussianTimeMuonBack,maxCryTime));
RooProdPdf dedxAndTimeBackModel("dedxAndTimeBackModel","dedxAndTimeBackModel",landauDeDxMuonBack,Conditional(gaussianTimeMuonBack,maxCryTime));

// Make signal-only model
RooGaussian gaussianDeDxHSCPSignal("gaussianDeDxHSCPSignal","gaussianDeDxHSCPSignal",sumDeDx,gaussianDeDxHSCPMeanSignal,gaussianDeDxHSCPSigmaSignal);
RooGaussian gaussianTimeHSCPSignal("gaussianTimeHSCPSignal","gaussianTimeHSCPSignal",maxCryTime,gaussianTimeHSCPMeanSignal,timingResReal);
RooProdPdf dedxAndTimeSignalModel("dedxAndTimeSignalModel","dedxAndTimeSignalModel",gaussianDeDxHSCPSignal,Conditional(gaussianTimeHSCPSignal,maxCryTime));

RooGaussian gaussianTimeHSCPSignal1D("gaussianTimeHSCPSignal1D","gaussianTimeHSCPSignal1D",maxCryTime,gaussianTimeHSCPMeanSignal,gaussianTimeHSCPSigmaSignal);

// 1-D model vars -- background
//RooRealVar backDeDxMP("dEdx_backMP","dEdx_backMP",15.18);
//RooRealVar backDeDxSigma("dEdx_backSigma","dEdx_backSigma",1.194);
//RooRealVar backTimeMean("T_backMean","T_backMean",2.329);
//RooLandau backEnergyModel1D = new RooLandau("backEnergyModel1D","backEnergyModel1D",sumDeDx,backDeDxMP,backDeDxSigma);
//RooGaussian* backTimeModel1D = new RooGaussian("backTimeModel1D","backTimeModel1D",maxCryTime,backTimeMean,timingRes);

// 1-D model vars -- background+signal (floating)
RooRealVar gaussianDeDxMean1("dEdx_hMean1","dEdx_hMean1",23.5,20,25);
RooRealVar gaussianDeDxSigma1("dEdx_hSigma1","dEdx_hSigma1",2.23,1,3);
RooRealVar landauDeDxMP1("dEdx_landauMP1","dEdx_landauMP1",13.0,10,15);
RooRealVar landauDeDxSigma1("dEdx_landauSigma1","dEdx_landauSigma1",0.6,0.1,1);
RooRealVar gaussianTimeHSCPMean1("T_hMean1","T_hMean1",6.89,6,10);
RooRealVar gaussianTimeHSCPSigma1("T_hSigma1","T_hSigma1",4.69,1,6);
RooRealVar gaussianTimeMuonMean1("T_muMean1","T_muMean1",2.46,-5,4);
RooRealVar gaussianTimeMuonSigma1("T_muSigma1","T_muSigma1",3.99,1,6);
//RooLandau landauDeDx1D("landauDeDxMuon1","landauDeDxMuon1",sumDeDx,landauDeDxMP1,landauDeDxSigma1);
// 1-D model vars -- background+signal (fixed)
//RooRealVar gaussianDeDxMean1("dEdx_hMean1","dEdx_hMean1",23.5);
//RooRealVar gaussianDeDxSigma1("dEdx_hSigma1","dEdx_hSigma1",2.23);
//RooRealVar landauDeDxMP1("dEdx_landauMP1","dEdx_landauMP1",13.0);
//RooRealVar landauDeDxSigma1("dEdx_landauSigma1","dEdx_landauSigma1",0.59);
//RooRealVar gaussianTimeHSCPMean1("T_hMean1","T_hMean1",6.89);
//RooRealVar gaussianTimeHSCPSigma1("T_hSigma1","T_hSigma1",4.);
//RooRealVar gaussianTimeMuonMean1("T_muMean1","T_muMean1",2.46);
//RooRealVar gaussianTimeMuonSigma1("T_muSigma1","T_muSigma1",3.99);
RooLandau landauDeDx1D("landauDeDxMuon1","landauDeDxMuon1",sumDeDx,landauDeDxMP1,landauDeDxSigma1);

//sumDeDx.setBins(10000,"cache");
//RooFFTConvPdf langausDeDxMuon1("langausDeDxMuon","landau (X) gauss",sumDeDx,landauDeDxMuon,gaussianDeDxMuon);
RooGaussian gaussianDeDx1D("gaussianDeDx1D","gaussianDeDx1D",sumDeDx,gaussianDeDxMean1,gaussianDeDxSigma1);
RooGaussian gaussianTimeMuon1D("gaussianTimeMuon1D","gaussianTimeMuon1D",maxCryTime,gaussianTimeMuonMean1,gaussianTimeMuonSigma1);
RooGaussian gaussianTimeHSCP1D("gaussianTimeHSCP1D","gaussianTimeHSCP1D",maxCryTime,gaussianTimeHSCPMean1,gaussianTimeHSCPSigma1);
RooRealVar sigFracE("sigFracE","signal fractionE",0.5,0,1);
RooRealVar sigFracT("sigFracT","signal fractionT",0.5,0,1);
// Signal + background
RooAddPdf dedxModel1D("dedxModel1D","dedxModel1D",RooArgList(gaussianDeDx1D,landauDeDx1D),sigFracE);
RooAddPdf timeModel1D("timeModel1D","timeModel1D",RooArgList(gaussianTimeHSCP1D,gaussianTimeMuon1D),sigFracT);
// Signal only
//RooGaussian dedxModel1D = gaussianDeDx1D;
//RooGaussian timeModel1D = gaussianTimeHSCP1D;
// Background only
//RooLandau dedxModel1D = landauDeDx1D;
//RooGaussian timeModel1D = gaussianTimeMuon1D;



// **************************************************************************************
// *******************  Function to set up my nice TStyle  ******************************
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
  //EStyle->SetOptStat(1111111);
  EStyle->SetOptStat(2222);

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

// **************************************************************************************
// *******************  Function to parse arguments and load hists **********************
void parseArguments(int arc, char* argv[])
{
  char* infile = argv[1];
  if (!infile)
  {
    cout << " No input file specified !" << endl;
    exit(-1);
  }

  myInputRootFile_ = new TFile(infile);
  //string dirName="hscpTimingAnalyzer/";
  //string ntupleName = "hscpEcalTree";
  string ntupleName = "hscpEcalRooFitTree";
  //string fullPath = dirName+ntupleName;
  string fullPath = ntupleName;
  energyTimeTNtuple_ = (TTree*)myInputRootFile_->Get(fullPath.c_str());
  if(!energyTimeTNtuple_)
  {
    cout << "****************No TNtuple with name " << ntupleName << " found in file: " << infile << endl;
    exit(-2);
  }
  else
    cout << "****************TNtuple found with " << energyTimeTNtuple_->GetEntries() << " entries." << endl;

}

