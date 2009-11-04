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

#include "RooExtendPdf.h"
#include "RooRealVar.h"
#include "RooLandau.h"
#include "RooGaussian.h"
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

  std::stringstream roofitstream;
  
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

  //cout << "****************Beginning fit for file: " << infile << endl;

  SetEStyle();
  gStyle->SetMarkerSize(0.5);
  gStyle->SetStatX(0.4);
  gStyle->SetStatW(0.3);

  // ************************ Define observables *******************************
  RooRealVar crystalEnergy("crystalEnergy","crystalEnergy",0.4,2.5,"GeV"); // 400 MeV energy cut in 1 cry
  RooRealVar crystalTime("crystalTime","crystalTime",-25,25, "ns");
  RooRealVar crystalChi2("crystalChi2","crystalChi2",0,100000);
  RooRealVar crystalTrackLength("crystalTrackLength","crystalTrackLength",0,24,"cm");
  RooRealVar crystalDeDx("crystalDeDx","crystalDeDx",17.4,50,"MeV/cm"); // 400 MeV energy cut in 1 cry

  // *********************** Load dataset **************************************
  RooDataSet* dedxTimeChi2TrackLengthData = new RooDataSet("dedxTimeChi2Data","dedxChi2TimeData",energyTimeTNtuple,
      RooArgSet(crystalDeDx,crystalTime,crystalChi2,crystalEnergy,crystalTrackLength));

  cout << "Number of entries in initial dataset is: " << dedxTimeChi2TrackLengthData->numEntries() << endl;
  //TCut chi2Cut1 = "chi2 < 0.96";
  TCut chi2Cut2 = "crystalChi2 > 0";
  RooDataSet* dedxTimeChi2CutData = (RooDataSet*) dedxTimeChi2TrackLengthData->reduce(chi2Cut2);
  cout << "Number of entries after chi2 > 0 cut is: " << dedxTimeChi2CutData->numEntries() << endl;
  //TCut cut1 = "energy > 0.4 || time < 15";
  //TCut energyCut1 = "crystalEnergy > 0.4";
  TCut trackLengthCut = "crystalTrackLength > 23.05 && crystalTrackLength < 23.15";
  RooDataSet* dedxTimeChi2CutTLCutData = (RooDataSet*) dedxTimeChi2CutData->reduce(trackLengthCut);
  //cout << "Number of entries after trackLength cut is: " << dedxTimeChi2CutTLCutData->numEntries() << endl;
  //RooDataSet* dedxTimeEnergyCutData = (RooDataSet*) dedxTimeDataChi2cut->reduce(energyCut1);
  // Dataset with only dE/dx, time
  //RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeEnergyCutData,RooArgSet(crystalDeDx,crystalTime));
  RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeChi2CutData,RooArgSet(crystalDeDx,crystalTime));//chi2 cut only
  //RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeChi2CutTLCutData,RooArgSet(crystalDeDx,crystalTime));//chi2+tkLength cuts

  // 1-D datasets
  RooDataSet* dedxData = new RooDataSet("dedxData","dedxData",dedxTimeData,RooArgSet(crystalDeDx));
  RooDataSet* timeData = new RooDataSet("timeData","timeData",dedxTimeData,RooArgSet(crystalTime));

  int numEntries = dedxTimeData->numEntries();
  cout << "****************Created RooDataSet with " << numEntries << " entries!" << endl;
  if(numEntries < 10)
  {
    cout << "Number of entries in the dataset is too few to continue. " << endl;
    return -2;
  }
  
  
  // **************** Parameters ***********************************************
  // Hard-coding the dE/dx and time parameters for Pt=50 GeV muons
  RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",15.18);
  RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",1.194);
  RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.329);
  //RooRealVar gaussDeDxMuonMean("gaussDeDxMuonMean","gaussDeDxMuonMean",5,1,10); // floating to get best fit
  //RooRealVar gaussDeDxMuonSigma("gaussDeDxMuonSigma","gaussDeDxMuonSigma",1,0.6,5);
  //RooRealVar landauDeDxMuonMean("landauDeDxMuonMean","landauDeDxMuonMean",15.1,5,25);
  //RooRealVar landauDeDxMuonSigma("landauDeDxMuonSigma","landauDeDxMuonSigma",1.1,0.1,5);
  //RooRealVar gaussianTimeMuonMean("gTimeMuonMean","gTimeMuonMean",2.3,-2,4);
  // Hard-coding the gaussian dE/dx and time parameters for kktau
  RooRealVar gaussianDeDxHSCPMean("geMean","geMean",23.035);
  RooRealVar gaussianDeDxHSCPSigma("geSigma","geSigma",2.2299);
  RooRealVar gaussianTimeHSCPMean("ghMean","ghMean",6.708);
  //RooRealVar gaussianDeDxHSCPMean("geMean","geMean",23.,1,100); // floating to get best fit
  //RooRealVar gaussianDeDxHSCPSigma("geSigma","geSigma",2.,0.1,10);
  //RooRealVar gaussianTimeHSCPMean("ghMean","ghMean",6.,-2.,10);
  
  // Background-only model -- muon
  //RooRealVar gaussDeDxMuonMeanBack("gaussDeDxMuonMeanBack","gaussDeDxMuMeanBack",10,1,1000);
  //RooRealVar gaussDeDxMuonSigmaBack("gaussDeDxMuonSigmaBack","gaussDeDxMuonSigmaBack",2,0.1,5);
  //RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",14.1,1,1000);
  //RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",2.183,0.1,100);
  //RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.33,-6,6);
  //RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",14.1); // old param values
  //RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",2.183); //
  //RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.33); //
  RooRealVar landauDeDxMuonMeanBack("landauDeDxMuonMeanBack","landauDeDxMuMeanBack",15.18);
  RooRealVar landauDeDxMuonSigmaBack("landauDeDxMuonSigmaBack","landauDeDxMuSigmaBack",1.194);
  RooRealVar gaussianTimeMuonMeanBack("gTimeMuonMeanBack","gTimeMuonMeanBack",2.329);

  // Signal-only model -- kktau
  RooRealVar gaussianDeDxHSCPMeanSignal("geMeanSignal","geMeanSignal",23.035);
  RooRealVar gaussianDeDxHSCPSigmaSignal("geSigmaSignal","geSigmaSignal",2.2299);
  RooRealVar gaussianTimeHSCPMeanSignal("ghMeanSignal","ghMeanSignal",6.708);
  
  // **************** Create PDFs **********************************************
  RooRealVar gausTimeSig("gausTimeSig","gausTimeSig",3);//testing
  //RooRealVar a("a","a",1.5766); // (0.042 GeV)/E = A/sigma
  //RooRealVar b("b","b",0.3527); // these are old
  RooRealVar a("a","a",0.8895); // from ecal craft timing note
  RooRealVar b("b","b",0.0008);
  RooRealVar ap("ap","ap",9685.); // @ 23 cm
  //RooRealVar ap("ap","ap",9601.4); // @ 23.1 cm length
  RooFormulaVar timingRes("timingRes","TMath::Sqrt(ap/crystalDeDx^2+0.0008)",RooArgList(ap,crystalDeDx)); // altered to use dE/dx*23 cm (ap)
  //name,title,variable,mean,sigma
  RooLandau landauDeDxMuon("landauDeDxMuon","landauDeDxMuon",crystalDeDx,landauDeDxMuonMean,landauDeDxMuonSigma);
  //RooGaussian gaussianDeDxMuon("gaussianDeDxMuon","gaussianDeDxMuon",crystalDeDx,gaussDeDxMuonMean,gaussDeDxMuonSigma);
  //crystalDeDx.setBins(10000,"cache");
  //RooFFTConvPdf langausDeDxMuon("langausDeDxMuon","landau (X) gauss",crystalDeDx,landauDeDxMuon,gaussianDeDxMuon);
  RooGaussian gaussianTimeMuon("gaussianTimeMuon","gaussianTimeMuon",crystalTime,gaussianTimeMuonMean,timingRes);

  // HSCP
  RooGaussian gaussianDeDxHSCP("gaussianDeDxHSCP","gaussianDeDxHSCP",crystalDeDx,gaussianDeDxHSCPMean,gaussianDeDxHSCPSigma);
  RooGaussian gaussianTimeHSCP("gaussianTimeHSCP","gaussianTimeHSCP",crystalTime,gaussianTimeHSCPMean,timingRes);

  // ***************** Models **************************************************
  RooRealVar sigFrac("sigFrac","signal fraction",0.5,0,1);
  RooAddPdf dedxModel("dedxModel","dedxModel",RooArgList(gaussianDeDxHSCP,landauDeDxMuon),sigFrac);
  RooAddPdf timeModel("timeModel","timeModel",RooArgList(gaussianTimeHSCP,gaussianTimeMuon),sigFrac);
  // Signal + Background
  RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",dedxModel,Conditional(timeModel,crystalTime));
  //Signal only
  //RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",gaussianDeDxHSCP,Conditional(gaussianTimeHSCP,crystalTime));
  //Background only
  //RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",landauDeDxMuon,Conditional(gaussianTimeMuon,crystalTime));
  //RooProdPdf dedxAndTimeModel("dedxAndTimeModel","dedxAndTimeModel",langausDeDxMuon,Conditional(gaussianTimeMuon,crystalTime));

  // Make background-only model
  RooLandau landauDeDxMuonBack("landauDeDxMuonBack","landauDeDxMuonBack",crystalDeDx,landauDeDxMuonMeanBack,landauDeDxMuonSigmaBack);
  //RooGaussian gaussianDeDxMuonBack("gaussianDeDxMuonBack","gaussianDeDxMuonBack",crystalDeDx,gaussDeDxMuonMeanBack,gaussDeDxMuonSigmaBack);
  //RooFFTConvPdf langausDeDxBack("langausDeDxBack","langausDeDxBack",crystalDeDx,landauDeDxMuonBack,gaussianDeDxMuonBack);
  RooGaussian gaussianTimeMuonBack("gaussianTimeMuonBack","gaussianTimeMuonBack",crystalTime,gaussianTimeMuonMeanBack,timingRes);
  //RooProdPdf dedxAndTimeBackModel("dedxAndTimeBackModel","dedxAndTimeBackModel",langausDeDxBack,Conditional(gaussianTimeMuonBack,crystalTime));
  RooProdPdf dedxAndTimeBackModel("dedxAndTimeBackModel","dedxAndTimeBackModel",landauDeDxMuonBack,Conditional(gaussianTimeMuonBack,crystalTime));

  // Make signal-only model
  RooGaussian gaussianDeDxHSCPSignal("gaussianDeDxHSCPSignal","gaussianDeDxHSCPSignal",crystalDeDx,gaussianDeDxHSCPMeanSignal,gaussianDeDxHSCPSigmaSignal);
  RooGaussian gaussianTimeHSCPSignal("gaussianTimeHSCPSignal","gaussianTimeHSCPSignal",crystalTime,gaussianTimeHSCPMeanSignal,timingRes);
  RooProdPdf dedxAndTimeSignalModel("dedxAndTimeSignalModel","dedxAndTimeSignalModel",gaussianDeDxHSCPSignal,Conditional(gaussianTimeHSCPSignal,crystalTime));


  // 1-D model vars -- background
  //RooRealVar backDeDxMP("dEdx_backMP","dEdx_backMP",15.18);
  //RooRealVar backDeDxSigma("dEdx_backSigma","dEdx_backSigma",1.194);
  //RooRealVar backTimeMean("T_backMean","T_backMean",2.329);
  //RooLandau backEnergyModel1D = new RooLandau("backEnergyModel1D","backEnergyModel1D",crystalDeDx,backDeDxMP,backDeDxSigma);
  //RooGaussian* backTimeModel1D = new RooGaussian("backTimeModel1D","backTimeModel1D",time,backTimeMean,timingRes);
  
  // 1-D model vars -- background+signal
  RooRealVar gaussianDeDxMean1("dEdx_hMean1","dEdx_hMean1",23.035);
  RooRealVar gaussianDeDxSigma1("dEdx_hSigma1","dEdx_hSigma1",2.23);
  RooRealVar landauDeDxMP1("dEdx_landauMP1","dEdx_landauMP1",15.2);
  RooRealVar landauDeDxSigma1("dEdx_landauSigma1","dEdx_landauSigma1",1.19);
  RooRealVar gaussianTimeHSCPMean1("T_hMean1","T_hMean1",6.79);
  RooRealVar gaussianTimeHSCPSigma1("T_hSigma1","T_hSigma1",4.69);
  RooRealVar gaussianTimeMuonMean1("T_muMean1","T_muMean1",2.84);
  RooRealVar gaussianTimeMuonSigma1("T_muSigma1","T_muSigma1",3.99);
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

  // 1-D fits
  dedxModel1D.fitTo(*dedxData);
  timeModel1D.fitTo(*timeData);

  // Fit, maximizing signal fraction
  //RooFitResult* fitResult =  dedxAndTimeModel.fitTo(*dedxTimeData,Save(),SumW2Error(true));
  // Background fit
  //dedxAndTimeBackModel.fitTo(*dedxTimeData);
  
// Custom fitting stuff
  RooFitResult *fitResult = 0;
  //RooAbsData::ErrorType fitError = RooAbsData::SumW2;
  //RooAbsData::ErrorType fitError = RooAbsData::Poisson;
  //RooNLLVar nll("nll","nll",dedxAndTimeModel,*dedxTimeData,ConditionalObservables(crystalTime),DataError(RooAbsData::SumW2));
  RooNLLVar nll("nll","nll",dedxAndTimeModel,*dedxTimeData,DataError(RooAbsData::SumW2));
  RooMinuit m(nll);
  m.setErrorLevel(0.2);
  m.setStrategy(2);
  m.hesse();
  //m.setVerbose(true);
  m.optimizeConst(true);
  m.setProfile(true);
  m.migrad();
  //m.improve();
  //m.hesse();
  m.hesse();
  m.minos();
  fitResult = m.save();
#if ROOT_VERSION_CODE <= ROOT_VERSION(5,19,0)
  fitResult->defaultStream(&roofitstream);
#else
  fitResult->defaultPrintStream(&roofitstream);
#endif
  fitResult->Print("v");
  cout << roofitstream.str();
  roofitstream.str(std::string(""));

  
  //TODO: fix this background model fitting issue...
  // Well, you can't fit a function with fixed parameters...
  //energyAndTimeBackModel.fitTo(*energyTimeData);
  //backEnergyModel1D.fitTo(*energyData);
  // backTimeModel1D.fitTo(*timeData);

  int autoPrecNum = 2;
  TFile* outputFile = new TFile("likelihoodOutput.root","recreate");
  outputFile->cd();

  float sigFracFit = sigFrac.getVal();
  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,2500,1000);
  combinedCanvas->Divide(3,1);
  combinedCanvas->cd(1);
  //Plot energy curves
  RooPlot* dedxFrame = crystalDeDx.frame();
  dedxTimeData->plotOn(dedxFrame);
  dedxAndTimeModel.plotOn(dedxFrame);
  dedxAndTimeModel.plotOn(dedxFrame,Components("gaussianDeDxHSCP"),LineStyle(kDashed),LineColor(2));
  dedxAndTimeModel.paramOn(dedxFrame,Layout(0.35,0.8,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //sigFrac = 0.516;
  //dedxAndTimeModel.plotOn(dedxFrame,LineStyle(kDashed),LineColor(2));
  dedxFrame->Draw("e0");
  combinedCanvas->cd(2);
  //Plot time curves
  //sigFrac = sigFracFit;
  RooPlot* timeFrame = crystalTime.frame();
  dedxTimeData->plotOn(timeFrame);
  dedxAndTimeModel.plotOn(timeFrame);
  dedxAndTimeModel.plotOn(timeFrame,Components("gaussianTimeHSCP"),LineStyle(kDashed),LineColor(2));
  dedxAndTimeModel.paramOn(timeFrame,Layout(0.25,0.75,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //sigFrac = 0.516;
  //dedxAndTimeModel.plotOn(timeFrame,LineStyle(kDashed),LineColor(2));
  timeFrame->Draw("e0");

  combinedCanvas->cd(3);
  //testing
  //RooPlot* energyFrame = crystalEnergy.frame();
  //dedxTimeEnergyCutData->plotOn(energyFrame);
  //energyFrame->Draw("e0");
  //Make 2-D plot of PDF
  TH1* hh_model = dedxAndTimeModel.createHistogram("hh_model",crystalDeDx,Binning(50),YVar(crystalTime,Binning(50))) ;
  hh_model->SetLineColor(kBlue);
  //TCanvas* modelCanvas = new TCanvas("2DCanvas","2DCanvas",600,600);
  //modelCanvas->cd();
  hh_model->Draw("surf");
  //energyTimeData->Draw("energy");
  //energyTimeData->Draw("time");
  //Print
  //combinedCanvas->Print("plotLikelihoods.png");
  combinedCanvas->Write();
  //modelCanvas->Print("plotLikelihoods2D_data.png");
  
  // Back only model
  TCanvas* combinedCanvasBack = new TCanvas("combinedCanvasBack","combinedCanvasBack",1,1,2500,1000);
  combinedCanvasBack->Divide(3,1);
  combinedCanvasBack->cd(1);
  //Plot energy curves
  RooPlot* dedxFrameBack = crystalDeDx.frame();
  dedxTimeData->plotOn(dedxFrameBack);
  dedxAndTimeBackModel.plotOn(dedxFrameBack);
  //dedxAndTimeBackModel.plotOn(dedxFrameBack,Components("landauDeDxMuonBack"),LineStyle(kDashed));
  dedxAndTimeBackModel.paramOn(dedxFrameBack,Layout(0.175,0.95,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  dedxFrameBack->Draw("e0");
  combinedCanvasBack->cd(2);
  //Plot time curves
  RooPlot* timeFrameBack = crystalTime.frame();
  dedxTimeData->plotOn(timeFrameBack);
  dedxAndTimeBackModel.plotOn(timeFrameBack);
  //dedxAndTimeBackModel.plotOn(timeFrameBack,Components("gaussianTimeMuonPBack"),LineStyle(kDashed));
  dedxAndTimeBackModel.paramOn(timeFrameBack,Layout(0.175,0.95,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  timeFrameBack->Draw("e0");
  combinedCanvasBack->cd(3);
  TH1* hh_modelBack = dedxAndTimeBackModel.createHistogram("hh_modelBack",crystalDeDx,Binning(50),YVar(crystalTime,Binning(50))) ;
  hh_modelBack->SetLineColor(kBlue);
  hh_modelBack->Draw("surf");
  //combinedCanvasBack->Print("plotLikelihoodsBack.png");
  combinedCanvasBack->Write();

  // Signal only model
  TCanvas* combinedCanvasSignal = new TCanvas("combinedCanvasSignal","combinedCanvasSignal",1,1,2500,1000);
  combinedCanvasSignal->Divide(3,1);
  combinedCanvasSignal->cd(1);
  //Plot energy curves
  RooPlot* dedxFrameSignal = crystalDeDx.frame();
  dedxTimeData->plotOn(dedxFrameSignal);
  dedxAndTimeSignalModel.plotOn(dedxFrameSignal);
  //dedxAndTimeSignalModel.plotOn(dedxFrameSignal,Components("gaussianDeDxMuonSignal"),LineStyle(kDashed));
  dedxAndTimeSignalModel.paramOn(dedxFrameSignal,Layout(0.25,0.75,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  dedxFrameSignal->Draw("e0");
  combinedCanvasSignal->cd(2);
  //Plot time curves
  RooPlot* timeFrameSignal = crystalTime.frame();
  dedxTimeData->plotOn(timeFrameSignal);
  dedxAndTimeSignalModel.plotOn(timeFrameSignal);
  //dedxAndTimeSignalModel.plotOn(timeFrameSignal,Components("gaussianTimeHSCPSignal"),LineStyle(kDashed));
  dedxAndTimeSignalModel.paramOn(timeFrameSignal,Layout(0.1,0.7,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  timeFrameSignal->Draw("e0");
  combinedCanvasSignal->cd(3);
  TH1* hh_modelSignal = dedxAndTimeSignalModel.createHistogram("hh_modelSignal",crystalDeDx,Binning(50),YVar(crystalTime,Binning(50))) ;
  hh_modelSignal->SetLineColor(kBlue);
  hh_modelSignal->Draw("surf");
  //combinedCanvasSignal->Print("plotLikelihoodsSignal.png");
  combinedCanvasSignal->Write();

  // 1-D plots
  TCanvas* combinedCanvas1D = new TCanvas("combinedCanvas1D","combinedCanvas1D",1,1,1800,2000);
  combinedCanvas1D->Divide(1,2);
  combinedCanvas1D->cd(1);
  RooPlot* dedxFrame2 = crystalDeDx.frame();
  dedxData->plotOn(dedxFrame2);
  dedxModel1D.plotOn(dedxFrame2);
  dedxModel1D.plotOn(dedxFrame2,Components("gaussianDeDx1D"),LineStyle(kDashed),LineColor(2));
  dedxModel1D.paramOn(dedxFrame2,Layout(0.55));
  dedxFrame2->Draw("e0");
  combinedCanvas1D->cd(2);
  RooPlot* timeFrame2 = crystalTime.frame();
  timeData->plotOn(timeFrame2);
  timeModel1D.plotOn(timeFrame2);
  timeModel1D.plotOn(timeFrame2,Components("gaussianTimeHSCP1D"),LineStyle(kDashed),LineColor(2));
  timeModel1D.paramOn(timeFrame2,Layout(0.7));
  timeFrame2->Draw("e0");
  //Print
  //combinedCanvas1D->Print("plotLikelihoods1D.png");
  combinedCanvas1D->Write();

  //// Make dE/dx and timing hists -- binned
  //TCanvas* combinedCanvasBinned = new TCanvas("combinedCanvasBinned","combinedCanvasBinned",1,1,2000,1000);
  //combinedCanvasBinned->Divide(2,1);
  //combinedCanvasBinned->cd(1);
  //crystalDeDx.setBins(15); // bin dedx var
  //crystalTime.setBins(25); // bin time var
  //RooDataHist* modelPDFBinned = dedxAndTimeModel.generateBinned(RooArgSet(crystalDeDx,crystalTime),dedxTimeData->numEntries());
  ////Plot dE/dx curves
  //RooPlot* dedxFrameBinned = crystalDeDx.frame();
  //dedxTimeData->plotOn(dedxFrameBinned);
  //modelPDFBinned->plotOn(dedxFrameBinned,LineColor(4));
  //dedxFrameBinned->Draw("e0");
  ////Plot time curves
  //combinedCanvasBinned->cd(2);
  ////RooBinning bins(-25,25,12.5,"bins");
  //RooPlot* timeFrameBinned = crystalTime.frame();
  //dedxTimeData->plotOn(timeFrameBinned);
  //modelPDFBinned->plotOn(timeFrameBinned,LineColor(4));
  //timeFrameBinned->Draw("e0");
  //combinedCanvasBinned->Write();
  //// Plot binned PDF value / data value in bin
  //TCanvas* combinedCanvasRatio = new TCanvas("combinedCanvasRatio","combinedCanvasRatio",1,1,2000,1000);
  //combinedCanvasRatio->Divide(2,1);
  //combinedCanvasRatio->cd(1);
  //vector<float> ratios;
  //vector<float> ratioErrors;
  //vector<float> binCenters;
  //vector<float> binWidths;
  //TH1* dedxDataHist = dedxTimeData->createHistogram("dedxData",crystalDeDx);
  //TH1* timeDataHist = dedxTimeData->createHistogram("timeData",crystalTime);
  //TH1* dedxPDFHist = modelPDFBinned->createHistogram("dedxPDF",crystalDeDx);
  //TH1* timePDFHist = modelPDFBinned->createHistogram("timePDF",crystalTime);
  //for(int i=1; i < dedxDataHist->GetNbinsX()-1; ++i)
  //{
  //  binCenters.push_back(dedxDataHist->GetBinCenter(i));
  //  binWidths.push_back(dedxDataHist->GetBinWidth(i));
  //  if(dedxPDFHist->GetBinContent(i) != 0)
  //    ratios.push_back(dedxDataHist->GetBinContent(i)/dedxPDFHist->GetBinContent(i));
  //  else
  //    ratios.push_back(0);
  //  ratioErrors.push_back(0.5*dedxDataHist->GetBinError(i)/dedxDataHist->GetBinContent(i));
  //  //cout << "ratios: " << dedxPDFHist->GetBinContent(i)/dedxDataHist->GetBinContent(i)
  //  //  << " binCenter: " << dedxDataHist->GetBinCenter(i) << endl;

  //}
  //TGraphErrors dedxRatioGraph(binCenters.size(),&(*binCenters.begin()),&(*ratios.begin()),&(*binWidths.begin()),&(*ratioErrors.begin()));
  //dedxRatioGraph.SetTitle("Data/PDF: dE/dx");
  //dedxRatioGraph.Draw("ap");
  //combinedCanvasRatio->cd(2);
  //ratios.clear();
  //binCenters.clear();
  //ratioErrors.clear();
  //binWidths.clear();
  //for(int i=1; i < timeDataHist->GetNbinsX()-1; ++i)
  //{
  //  binCenters.push_back(timeDataHist->GetBinCenter(i));
  //  binWidths.push_back(timeDataHist->GetBinWidth(i));
  //  if(timePDFHist->GetBinContent(i) != 0)
  //    ratios.push_back(timeDataHist->GetBinContent(i)/timePDFHist->GetBinContent(i));
  //  else
  //    ratios.push_back(0);
  //  ratioErrors.push_back(0.5*timeDataHist->GetBinError(i)/timeDataHist->GetBinContent(i));
  //  //cout << "ratios: " << timePDFHist->GetBinContent(i)/timeDataHist->GetBinContent(i)
  //  //  << " binCenter: " << timeDataHist->GetBinCenter(i) << endl;

  //}
  //TGraphErrors timeRatioGraph(binCenters.size(),&(*binCenters.begin()),&(*ratios.begin()),&(*binWidths.begin()),&(*ratioErrors.begin()));
  //timeRatioGraph.SetTitle("Data/PDF: time");
  //timeRatioGraph.Draw("ap");
  //combinedCanvasRatio->Write();
  

  //Loop over hists: energyHist, timeHist and calculate the likelihood
  float logLikelihood = 0;
  float logLikelihoodBack = 0;
  float logLikelihoodSignal = 0;
  for(int entry = 0; entry < dedxTimeData->numEntries(); ++entry)
  {
    RooArgSet temp = *dedxTimeData->get(entry);
    crystalDeDx = temp.getRealValue("crystalDeDx");
    crystalTime = temp.getRealValue("crystalTime");
    //std::cout << "crystalDeDx: " << crystalDeDx.getVal() << " crystalTime: " << crystalTime.getVal() << std::endl;
    //std::cout << "-LogLikelihood = " << -1*log(dedxAndTimeModel.getVal(new RooArgSet(crystalDeDx,crystalTime))) << std::endl;
    //std::cout << "-LogLikelihoodBack = " << -1*log(dedxAndTimeBackModel.getVal(new RooArgSet(crystalDeDx,crystalTime))) << std::endl;
    logLikelihood-=log(dedxAndTimeModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
    logLikelihoodBack-=log(dedxAndTimeBackModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
    logLikelihoodSignal-=log(dedxAndTimeSignalModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
    //std::cout << "-LogLikelihood+LogLikelihoodBack = " << logLikelihood-logLikelihoodBack << std::endl;
  }
  
  //cout << "Beginning binned fit" << endl;
  //landauDeDxMuonMean = 14.1;
  //landauDeDxMuonSigma = 2.183;
  //gaussianTimeMuonMean = 2.33;
  //crystalDeDx.setBins(33);
  //crystalTime.setBins(125);
  //cout << "crystalDeDxBins: " << crystalDeDx.getBins() << endl;
  //// Create binned dataset
  //RooDataHist* dh = new RooDataHist("dh","binned version of dedxTimeData",RooArgSet(crystalDeDx,crystalTime),*dedxTimeData);
  ////RooChi2Var chi2("chi2","chi2",dedxAndTimeModel,*dh) ;
  ////RooMinuit m2(chi2);
  //RooNLLVar nll2("nll2","nll2",dedxAndTimeModel,*dh,DataError(RooAbsData::SumW2));
  //RooMinuit m2(nll2);
  //m2.migrad();
  //m2.hesse();
  //RooFitResult* r2 = m2.save();
  //r2->Print("v");


  //XXX:Create plot of likelihood vs sigFrac
  vector<float> sigFracs;
  vector<float> likelihoods;
  for(float i=0; i < 1.01; i+=0.01)
  {
    double totLikelihood = 0;
    sigFracs.push_back(i);
    sigFrac = i;
    for(int entry = 0; entry < dedxTimeData->numEntries(); ++entry)
    {
      RooArgSet temp = *dedxTimeData->get(entry);
      crystalDeDx = temp.getRealValue("crystalDeDx");
      crystalTime = temp.getRealValue("crystalTime");
      totLikelihood-=log(dedxAndTimeModel.getVal(new RooArgSet(crystalDeDx,crystalTime)));
    }
    likelihoods.push_back(totLikelihood);
  }
  TCanvas* likelihoodPlotCanvas = new TCanvas("llPlotCanvas","llPlotCanvas",1,1,500,500);
  likelihoodPlotCanvas->cd();
  TGraph* likelihoodPlot = new TGraph(sigFracs.size(),&(*sigFracs.begin()),&(*likelihoods.begin()));
  likelihoodPlot->GetXaxis()->SetTitle("signalFraction");
  likelihoodPlot->GetYaxis()->SetTitle("-LogLikelihood");
  likelihoodPlot->Draw("alp");
  //likelihoodPlotCanvas->Print("plotLikelihoodsCurve.png");
  likelihoodPlotCanvas->Write();
  
  outputFile->Close();
  f->Close();

  cout << "-LogLikelihood for SB model: " << logLikelihood << " -LogLikelihood for B only model: " << logLikelihoodBack
    << " -LogLikelihood for S only model: " << logLikelihoodSignal << endl;
  cout << "-LogLikelihood difference (B-SB; + indicates SB more likely): " << logLikelihoodBack-logLikelihood << endl;
  cout << "-LogLikelihood difference (S-SB; + indicates SB more likely): " << logLikelihoodSignal-logLikelihood << endl;
  
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
