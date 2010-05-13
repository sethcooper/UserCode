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
#include "RooMCStudy.h"

using namespace RooFit;

// Globals
TFile* myInputRootFile_;
TTree* energyTimeTNtuple_;

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
  string dirName="hscpTimingAnalyzer/";
  string ntupleName = "energyAndTimeNTuple";
  string fullPath = dirName+ntupleName;
  energyTimeTNtuple_ = (TTree*)myInputRootFile_->Get(fullPath.c_str());
  if(!energyTimeTNtuple_)
  {
    cout << "****************No TNtuple with name " << ntupleName << " found in file: " << infile << endl;
    exit(-2);
  }
  else
    cout << "****************TNtuple found with " << energyTimeTNtuple_->GetEntries() << " entries." << endl;

}

//XXX: Main
int main(int argc, char* argv[])
{
  using namespace std;

  stringstream roofitstream;

  // parse arguments and load data
  //parseArguments(argc, argv);

  SetEStyle();
  gStyle->SetMarkerSize(0.5);
  gStyle->SetStatX(0.4);
  gStyle->SetStatW(0.3);

  // ************************ Define observables *******************************
  //RooRealVar crystalEnergy("crystalEnergy","crystalEnergy",0.4,2.5,"GeV"); // 400 MeV energy cut in 1 cry
  
  //RooRealVar crystalEnergy("crystalEnergy","crystalEnergy",0.2,2.5,"GeV"); // higher cut will be done with dE/dx below
  // Using this instead of above for Ledovskoy, DEC 10 2009
  RooRealVar crystalEnergy("crystalEnergy","crystalEnergy",0,5,"GeV");
  RooRealVar crystalTime("crystalTime","crystalTime",-25,25, "ns");
  RooRealVar crystalChi2("crystalChi2","crystalChi2",0,1000);
  RooRealVar crystalTrackLength("crystalTrackLength","crystalTrackLength",0,25,"cm");
  //RooRealVar crystalDeDx("crystalDeDx","crystalDeDx",17.4,50,"MeV/cm"); // 400 MeV energy cut in 1 cry
  // Using this instead of above for Ledovskoy, DEC 10 2009
  RooRealVar crystalDeDx("crystalDeDx","crystalDeDx",0,100,"MeV/cm");

  // *********** Set bins
  //crystalEnergy.setBins(125);
  //crystalTime.setBins(100);
  //crystalDeDx.setBins(250);


  // *********************** Load dataset **************************************
  //RooDataSet* dedxTimeChi2TrackLengthData = new RooDataSet("dedxTimeChi2Data","dedxChi2TimeData",energyTimeTNtuple_,
  //    RooArgSet(crystalDeDx,crystalTime,crystalChi2,crystalEnergy,crystalTrackLength));

  //cout << "Number of entries in initial dataset is: " << dedxTimeChi2TrackLengthData->numEntries() << endl;
  ////TCut chi2Cut1 = "chi2 < 0.96";
  //TCut chi2Cut2 = "crystalChi2 > 0";
  //RooDataSet* dedxTimeChi2CutData = (RooDataSet*) dedxTimeChi2TrackLengthData->reduce(chi2Cut2);
  //cout << "Number of entries after chi2 > 0 cut is: " << dedxTimeChi2CutData->numEntries() << endl;
  ////TCut cut1 = "energy > 0.4 || time < 15";
  ////TCut energyCut1 = "crystalEnergy > 0.4";
  //TCut trackLengthCut = "crystalTrackLength > 23.05 && crystalTrackLength < 23.15";
  ////RooDataSet* dedxTimeChi2CutTLCutData = (RooDataSet*) dedxTimeChi2CutData->reduce(trackLengthCut);
  ////cout << "Number of entries after trackLength cut is: " << dedxTimeChi2CutTLCutData->numEntries() << endl;
  ////RooDataSet* dedxTimeEnergyCutData = (RooDataSet*) dedxTimeDataChi2cut->reduce(energyCut1);
  //// Dataset with only dE/dx, time
  ////RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeEnergyCutData,RooArgSet(crystalDeDx,crystalTime));
  //RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeChi2CutData,RooArgSet(crystalDeDx,crystalTime));//chi2 cut only
  ////RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeChi2CutTLCutData,RooArgSet(crystalDeDx,crystalTime));//chi2+tkLength cuts

  //// 1-D datasets
  //RooDataSet* dedxData = new RooDataSet("dedxData","dedxData",dedxTimeData,RooArgSet(crystalDeDx));
  //RooDataSet* timeData = new RooDataSet("timeData","timeData",dedxTimeData,RooArgSet(crystalTime));

  //int numEntries = dedxTimeData->numEntries();
  //cout << "****************Created RooDataSet with " << numEntries << " entries!" << endl;
  //if(numEntries < 10)
  //{
  //  cout << "Number of entries in the dataset is too few to continue. " << endl;
  //  return -3;
  //}

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
  RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",13.0);
  RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",0.59);
  RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.46);

  //***** Floating
  //RooRealVar gaussianDeDxHSCPMean("geMean","geMean",23.,1,100);
  //RooRealVar gaussianDeDxHSCPSigma("geSigma","geSigma",2.,0.1,10);
  //RooRealVar gaussianTimeHSCPMean("ghMean","ghMean",6.,-2.,10);
  //***** FIXED params, now based on new samples (singlecry kk tau) SIC NOV 6 2009
  RooRealVar gaussianDeDxHSCPMean("geMean","geMean",23.5);
  RooRealVar gaussianDeDxHSCPSigma("geSigma","geSigma",2.23);
  RooRealVar gaussianTimeHSCPMean("ghMean","ghMean",6.89);
  
  // Background-only model -- muon
  //RooRealVar gaussDeDxMuonMeanBack("gaussDeDxMuonMeanBack","gaussDeDxMuMeanBack",10,1,1000);
  //RooRealVar gaussDeDxMuonSigmaBack("gaussDeDxMuonSigmaBack","gaussDeDxMuonSigmaBack",2,0.1,5);
  //RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",14.1,1,1000);
  //RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",2.183,0.1,100);
  //RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.33,-6,6);
  //RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",14.1); // old param values
  //RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",2.183); //
  //RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.33); //
  RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",13.0);
  RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",0.59);
  RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.46);

  // Signal-only model -- kktau
  RooRealVar gaussianDeDxHSCPMeanSignal("geMeanSignal","geMeanSignal",23.5);
  RooRealVar gaussianDeDxHSCPSigmaSignal("geSigmaSignal","geSigmaSignal",2.23);
  RooRealVar gaussianTimeHSCPMeanSignal("ghMeanSignal","ghMeanSignal",6.89);
  
  // **************** Create PDFs **********************************************
  //RooRealVar gausTimeSig("gausTimeSig","gausTimeSig",3);//testing
  //RooRealVar a("a","a",1.5766); // (0.042 GeV)/E = A/sigma
  //RooRealVar b("b","b",0.3527); // these are old
  RooRealVar a("a","a",0.8895); // from ecal craft timing note
  RooRealVar b("b","b",0.0008);
  RooRealVar ap("ap","ap",12110.); //XXX SIC April 5 1020 --  ~ 25% wider sigma for MC studies
  RooRealVar apReal("apReal","apReal",9685.); // @ 23 cm
  //RooRealVar ap("ap","ap",9601.4); // @ 23.1 cm length
  RooFormulaVar timingRes("timingRes","TMath::Sqrt(ap/crystalDeDx^2+0.0008)",RooArgList(ap,crystalDeDx)); // altered to use wider sigma
  RooFormulaVar timingResReal("timingResReal","TMath::Sqrt(apReal/crystalDeDx^2+0.0008)",RooArgList(apReal,crystalDeDx)); // altered to use dE/dx*23 cm (ap)
  //name,title,variable,mean,sigma
  RooLandau landauDeDxMuon("landauDeDxMuon","landauDeDxMuon",crystalDeDx,landauDeDxMuonMean,landauDeDxMuonSigma);
  //RooGaussian gaussianDeDxMuon("gaussianDeDxMuon","gaussianDeDxMuon",crystalDeDx,gaussDeDxMuonMean,gaussDeDxMuonSigma);
  //crystalDeDx.setBins(10000,"cache");
  //RooFFTConvPdf langausDeDxMuon("langausDeDxMuon","landau (X) gauss",crystalDeDx,landauDeDxMuon,gaussianDeDxMuon);
  
  RooGaussian gaussianTimeMuon("gaussianTimeMuon","gaussianTimeMuon",crystalTime,gaussianTimeMuonMean,timingResReal);
  //RooBreitWigner gaussianTimeMuon("gaussianTimeMuon","gaussianTimeMuon",crystalTime,gaussianTimeMuonMean,timingResReal);
  RooGaussian gaussianTimeMuonReal("gaussianTimeMuonReal","gaussianTimeMuonReal",crystalTime,gaussianTimeMuonMean,timingResReal);

  // HSCP
  RooGaussian gaussianDeDxHSCP("gaussianDeDxHSCP","gaussianDeDxHSCP",crystalDeDx,gaussianDeDxHSCPMean,gaussianDeDxHSCPSigma);
  RooGaussian gaussianTimeHSCP("gaussianTimeHSCP","gaussianTimeHSCP",crystalTime,gaussianTimeHSCPMean,timingResReal);
  //RooBreitWigner gaussianTimeHSCP("gaussianTimeHSCP","gaussianTimeHSCP",crystalTime,gaussianTimeHSCPMean,timingResReal);
  RooGaussian gaussianTimeHSCPReal("gaussianTimeHSCPReal","gaussianTimeHSCPReal",crystalTime,gaussianTimeHSCPMean,timingResReal);

  // ***************** Models **************************************************
  //XXX Here, sigFrac will be amount of signal generated
  RooRealVar sigFrac("sigFrac","signal fraction",0.01,0,1);
  RooAddPdf dedxModel("dedxModel","dedxModel",RooArgList(gaussianDeDxHSCP,landauDeDxMuon),sigFrac);
  RooAddPdf timeModel("timeModel","timeModel",RooArgList(gaussianTimeHSCP,gaussianTimeMuon),sigFrac);
  RooAddPdf timeModelReal("timeModelReal","timeModelReal",RooArgList(gaussianTimeHSCPReal,gaussianTimeMuonReal),sigFrac);
  // Signal + Background
  RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",dedxModel,Conditional(timeModel,crystalTime));
  RooProdPdf dedxAndTimeModelReal("dedxAndTimeModelReal","dedxAndTimeModelReal",dedxModel,Conditional(timeModelReal,crystalTime));
  //Signal only
  //RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",gaussianDeDxHSCP,Conditional(gaussianTimeHSCP,crystalTime));
  //Background only
  //RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",landauDeDxMuon,Conditional(gaussianTimeMuon,crystalTime));
  
  // fixed signal fraction PDF
  RooRealVar sigFracFixed("sigFracFixed","signal fraction fixed",0.5);
  RooAddPdf dedxModelFixed("dedxModelFixed","dedxModelFixed",RooArgList(gaussianDeDxHSCP,landauDeDxMuon),sigFracFixed);
  RooAddPdf timeModelFixed("timeModelFixed","timeModelFixed",RooArgList(gaussianTimeHSCP,gaussianTimeMuon),sigFracFixed);
  RooProdPdf dedxAndTimeModelFixed("dedxAndTimeModelFixed","dedxAndTimeModelFixed",dedxModelFixed,Conditional(timeModelFixed,crystalTime));

  // Make background-only model
  RooLandau landauDeDxMuonBack("landauDeDxMuonBack","landauDeDxMuonBack",crystalDeDx,landauDeDxMuonMeanBack,landauDeDxMuonSigmaBack);
  //RooGaussian gaussianDeDxMuonBack("gaussianDeDxMuonBack","gaussianDeDxMuonBack",crystalDeDx,gaussDeDxMuonMeanBack,gaussDeDxMuonSigmaBack);
  //RooFFTConvPdf langausDeDxBack("langausDeDxBack","langausDeDxBack",crystalDeDx,landauDeDxMuonBack,gaussianDeDxMuonBack);
  RooGaussian gaussianTimeMuonBack("gaussianTimeMuonBack","gaussianTimeMuonBack",crystalTime,gaussianTimeMuonMeanBack,timingResReal);
  //RooProdPdf dedxAndTimeBackModel("dedxAndTimeBackModel","dedxAndTimeBackModel",langausDeDxBack,Conditional(gaussianTimeMuonBack,crystalTime));
  RooProdPdf dedxAndTimeBackModel("dedxAndTimeBackModel","dedxAndTimeBackModel",landauDeDxMuonBack,Conditional(gaussianTimeMuonBack,crystalTime));

  // Make signal-only model
  RooGaussian gaussianDeDxHSCPSignal("gaussianDeDxHSCPSignal","gaussianDeDxHSCPSignal",crystalDeDx,gaussianDeDxHSCPMeanSignal,gaussianDeDxHSCPSigmaSignal);
  RooGaussian gaussianTimeHSCPSignal("gaussianTimeHSCPSignal","gaussianTimeHSCPSignal",crystalTime,gaussianTimeHSCPMeanSignal,timingResReal);
  RooProdPdf dedxAndTimeSignalModel("dedxAndTimeSignalModel","dedxAndTimeSignalModel",gaussianDeDxHSCPSignal,Conditional(gaussianTimeHSCPSignal,crystalTime));


  // 1-D model vars -- background
  //RooRealVar backDeDxMP("dEdx_backMP","dEdx_backMP",15.18);
  //RooRealVar backDeDxSigma("dEdx_backSigma","dEdx_backSigma",1.194);
  //RooRealVar backTimeMean("T_backMean","T_backMean",2.329);
  //RooLandau backEnergyModel1D = new RooLandau("backEnergyModel1D","backEnergyModel1D",crystalDeDx,backDeDxMP,backDeDxSigma);
  //RooGaussian* backTimeModel1D = new RooGaussian("backTimeModel1D","backTimeModel1D",time,backTimeMean,timingRes);
  
  // 1-D model vars -- background+signal (floating)
  RooRealVar gaussianDeDxMean1("dEdx_hMean1","dEdx_hMean1",23.5,20,25);
  RooRealVar gaussianDeDxSigma1("dEdx_hSigma1","dEdx_hSigma1",2.23,1,3);
  RooRealVar landauDeDxMP1("dEdx_landauMP1","dEdx_landauMP1",13.0,10,15);
  RooRealVar landauDeDxSigma1("dEdx_landauSigma1","dEdx_landauSigma1",0.6,0.1,1);
  RooRealVar gaussianTimeHSCPMean1("T_hMean1","T_hMean1",6.89,6,10);
  RooRealVar gaussianTimeHSCPSigma1("T_hSigma1","T_hSigma1",4.69,1,6);
  RooRealVar gaussianTimeMuonMean1("T_muMean1","T_muMean1",2.46,-5,4);
  RooRealVar gaussianTimeMuonSigma1("T_muSigma1","T_muSigma1",3.99,1,6);
  //RooLandau landauDeDx1D("landauDeDxMuon1","landauDeDxMuon1",crystalDeDx,landauDeDxMP1,landauDeDxSigma1);
  // 1-D model vars -- background+signal (fixed)
  //RooRealVar gaussianDeDxMean1("dEdx_hMean1","dEdx_hMean1",23.5);
  //RooRealVar gaussianDeDxSigma1("dEdx_hSigma1","dEdx_hSigma1",2.23);
  //RooRealVar landauDeDxMP1("dEdx_landauMP1","dEdx_landauMP1",13.0);
  //RooRealVar landauDeDxSigma1("dEdx_landauSigma1","dEdx_landauSigma1",0.59);
  //RooRealVar gaussianTimeHSCPMean1("T_hMean1","T_hMean1",6.89);
  //RooRealVar gaussianTimeHSCPSigma1("T_hSigma1","T_hSigma1",4.);
  //RooRealVar gaussianTimeMuonMean1("T_muMean1","T_muMean1",2.46);
  //RooRealVar gaussianTimeMuonSigma1("T_muSigma1","T_muSigma1",3.99);
  RooLandau landauDeDx1D("landauDeDxMuon1","landauDeDxMuon1",crystalDeDx,landauDeDxMP1,landauDeDxSigma1);

  //crystalDeDx.setBins(10000,"cache");
  //RooFFTConvPdf langausDeDxMuon1("langausDeDxMuon","landau (X) gauss",crystalDeDx,landauDeDxMuon,gaussianDeDxMuon);
  RooGaussian gaussianDeDx1D("gaussianDeDx1D","gaussianDeDx1D",crystalDeDx,gaussianDeDxMean1,gaussianDeDxSigma1);
  RooGaussian gaussianTimeMuon1D("gaussianTimeMuon1D","gaussianTimeMuon1D",crystalTime,gaussianTimeMuonMean1,gaussianTimeMuonSigma1);
  RooGaussian gaussianTimeHSCP1D("gaussianTimeHSCP1D","gaussianTimeHSCP1D",crystalTime,gaussianTimeHSCPMean1,gaussianTimeHSCPSigma1);
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
  //float frac = 0;
  //cout << "sic: Setting signal fractions to: " << frac << endl;
  //sigFrac = frac;
  //sigFracT = frac;
  //sigFracE = frac;

  //RooMCStudy mcStudy(dedxAndTimeBackModel,RooArgSet(crystalDeDx,crystalTime),FitModel(dedxAndTimeModel));
  //RooMCStudy mcStudy(dedxAndTimeModelFixed,RooArgSet(crystalDeDx,crystalTime),FitModel(dedxAndTimeModel),FitOptions("mhr"));
  //mcStudy.generateAndFit(100,10); // # times to gen, # events per gen

  cout << "************** Fitting done, now time to plot." << endl;
  TFile* outputFile = new TFile("likelihoodOutput_toyMC.root","recreate");
  outputFile->cd();

  // turn off for plotting
  ////TCanvas* canvas = new TCanvas("canvasNLL","canvasNLL",1,1,1200,1000);
  ////canvas->cd();
  //RooPlot* nllFrame = mcStudy.plotNLL();
  //nllFrame->SetTitle("-log(likelihood)");
  //nllFrame->Draw();
  //nllFrame->Write();
  ////canvas->Write();

  ////TCanvas* canvas2 = new TCanvas("canvasSigFracFit","canvasSigFracFit",1,1,1200,1000);
  ////canvas2->cd();
  //RooPlot* sigFracFitFrame = mcStudy.plotParam(sigFrac);
  //sigFracFitFrame->SetTitle("Fitted values of sigFrac");
  //sigFracFitFrame->Draw();
  //sigFracFitFrame->Write();
  ////canvas2->Write();

  sigFrac = 1;  // make S+B all HSCP
  //crystalDeDx = 23.5; //  ~ 2 MIPs in 1 single crystal
  
  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,1200,1000);
  combinedCanvas->Divide(2,1);
  combinedCanvas->cd(1);
  //Plot energy curves
  RooPlot* dedxFrame = crystalDeDx.frame();
  dedxAndTimeModel.plotOn(dedxFrame);
  dedxAndTimeModel.plotOn(dedxFrame,Components("gaussianDeDxHSCP"),LineStyle(kDashed),LineColor(2));
  //dedxAndTimeModel.paramOn(dedxFrame,Layout(0.35,0.8,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //sigFrac = 0.516;
  //dedxAndTimeModel.plotOn(dedxFrame,LineStyle(kDashed),LineColor(2));
  dedxFrame->Draw("e0");
  combinedCanvas->cd(2);
  //Plot time curves
  //sigFrac = sigFracFit;
  RooPlot* timeFrame = crystalTime.frame();
  dedxAndTimeModel.plotOn(timeFrame);
  dedxAndTimeModel.plotOn(timeFrame,Components("gaussianTimeHSCP"),LineStyle(kDashed),LineColor(2));
  //dedxAndTimeModel.paramOn(timeFrame,Layout(0.25,0.75,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //sigFrac = 0.516;
  //dedxAndTimeModel.plotOn(timeFrame,LineStyle(kDashed),LineColor(2));
  timeFrame->Draw("e0");
  combinedCanvas->Write();
  
  // Back only model
  TCanvas* combinedCanvasBack = new TCanvas("combinedCanvasBack","combinedCanvasBack",1,1,1200,1000);
  combinedCanvasBack->Divide(2,1);
  combinedCanvasBack->cd(1);
  //Plot energy curves
  RooPlot* dedxFrameBack = crystalDeDx.frame();
  dedxAndTimeBackModel.plotOn(dedxFrameBack);
  //dedxAndTimeBackModel.plotOn(dedxFrameBack,Components("landauDeDxMuonBack"),LineStyle(kDashed));
  //dedxAndTimeBackModel.paramOn(dedxFrameBack,Layout(0.175,0.95,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  dedxFrameBack->Draw("e0");
  combinedCanvasBack->cd(2);
  //Plot time curves
  RooPlot* timeFrameBack = crystalTime.frame();
  dedxAndTimeBackModel.plotOn(timeFrameBack);
  //dedxAndTimeBackModel.plotOn(timeFrameBack,Components("gaussianTimeMuonPBack"),LineStyle(kDashed));
  //dedxAndTimeBackModel.paramOn(timeFrameBack,Layout(0.175,0.95,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  timeFrameBack->Draw("e0");
  //combinedCanvasBack->Print("plotLikelihoodsBack.png");
  combinedCanvasBack->Write();

  ////Loop over hists: energyHist, timeHist and calculate the likelihood
  //float logLikelihood = 0;
  //float logLikelihoodBack = 0;
  //float logLikelihoodSignal = 0;
  //for(int entry = 0; entry < dedxTimeData->numEntries(); ++entry)
  //{
  //  RooArgSet temp = *dedxTimeData->get(entry);
  //  crystalDeDx = temp.getRealValue("crystalDeDx");
  //  crystalTime = temp.getRealValue("crystalTime");
  //  //std::cout << "crystalDeDx: " << crystalDeDx.getVal() << " crystalTime: " << crystalTime.getVal() << std::endl;
  //  //std::cout << "-LogLikelihood = " << -1*log(dedxAndTimeModel.getVal(new RooArgSet(crystalDeDx,crystalTime))) << std::endl;
  //  //std::cout << "-LogLikelihoodBack = " << -1*log(dedxAndTimeBackModel.getVal(new RooArgSet(crystalDeDx,crystalTime))) << std::endl;
  //  logLikelihood-=log(dedxAndTimeModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
  //  logLikelihoodBack-=log(dedxAndTimeBackModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
  //  logLikelihoodSignal-=log(dedxAndTimeSignalModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
  //  //std::cout << "-LogLikelihood+LogLikelihoodBack = " << logLikelihood-logLikelihoodBack << std::endl;
  //}
  
  outputFile->Close();
  //myInputRootFile_->Close();

  //cout << "-LogLikelihood for SB model: " << logLikelihood << " -LogLikelihood for B only model: " << logLikelihoodBack
  //  << " -LogLikelihood for S only model: " << logLikelihoodSignal << endl;
  //cout << "-LogLikelihood difference (B-SB; + indicates SB more likely): " << logLikelihoodBack-logLikelihood << endl;
  //cout << "-LogLikelihood difference (S-SB; + indicates SB more likely): " << logLikelihoodSignal-logLikelihood << endl;
  
  //ofstream outputStream("likelihoodDifferences_muons.txt",ios::app);
  //if(!outputStream.is_open())
  //{
  //  cout << "ERROR: Unable to write to file (file is not open)." << endl;
  //  return -1;
  //}
  //if(!outputStream.good())
  //{
  //  cout << "ERROR: Unable to write to file (file is not open)." << endl;
  //  return -1;
  //}

  //// Get to the end of the file
  //outputStream.seekp(0,ios::end);
  //// Energy, tab, time
  //outputStream << logLikelihood-logLikelihoodBack << std::endl;
  //outputStream.close();

  return 0;
}
