#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include <iostream>
#include <sstream>
#include <memory>
#include <fstream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
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
#include "RooMinuit.h"
#include "RooNLLVar.h"

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

  cout << "****************Beginning fit for file: " << infile << endl;

  SetEStyle();
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
  
  //RooDataSet* energyTimeChi2Data = new RooDataSet("energyTimeChi2Data","energyChi2TimeData",energyTimeTNtuple,RooArgSet(energy,time,chi2));
  //TCut chi2Cut1 = "chi2 < 0.96";
  //TCut chi2Cut2 = "chi2 > 0";
  //RooDataSet* energyTimeDataChi2cut = (RooDataSet*) energyTimeChi2Data->reduce(chi2Cut2);
  //TCut cut1 = "energy > 0.4 || time < 15";
  //RooDataSet* energyTimeCutData = (RooDataSet*) energyTimeDataChi2cut->reduce(cut1);
  //RooDataSet* energyTimeData = new RooDataSet("energyTimeData","energyTimeData",energyTimeCutData,RooArgSet(energy,time));
  RooDataSet* energyTimeData = new RooDataSet("energyTimeData","energyTimeData",energyTimeTNtuple,RooArgSet(energy,time));

  
  //TCut energyCut = "energy > 0.320";
  //RooDataSet* energyTimeData = (RooDataSet*) energyTimeDataRaw->reduce(energyCut);

  // 1-D datasets
  RooDataSet* energyData = new RooDataSet("energyData","energyData",energyTimeData,RooArgSet(energy));
  RooDataSet* timeData = new RooDataSet("timeDataRaw","timeDataRaw",energyTimeData,RooArgSet(time));

  int numEntries = energyTimeData->numEntries();
  cout << "****************Created RooDataSet with " << numEntries << " entries!" << endl;

  RooRealVar langausEnergyWidth("langausWidth","landgausWidth",0.01,0,0.1);
  RooRealVar langausEnergyMP("langausMP","langausMP",0.3,0.01,0.5);
  RooRealVar langausEnergyArea("langausArea","langausArea",1.0);
  RooRealVar langausEnergyGSigma("langausGSigma","langausGSigma",0.06,0,0.1);

  RooRealVar gaussianTimeMuonsMean("gmMean","gmMean",2.5,-5.0,5.0);
  RooRealVar gaussianTimeMuonsSigma("gmSigma","gmSigma",3.0,0.01,6.0);

  RooRealVar a("a","a",1.5766); // (0.042 GeV)/E = A/sigma
  RooRealVar b("b","b",0.3527);
  RooFormulaVar timingRes("timingRes","sqrt(b+a/energy^2)",RooArgSet(energy,b,a));
  //name,title,variable,mean,sigma
  //LandauGauss* langausEnergy = new LandauGauss("lanGausEnergy","lanGausEnergy",energy,langausEnergyWidth,langausEnergyMP,langausEnergyArea,langausEnergyGSigma);
  //RooGaussian* gaussianTimeMuons = new RooGaussian("gaussianTimeMuons","gaussianTimeMuons",time,gaussianTimeMuonsMean,timingRes);

  // Make background-only model
  LandauGauss* langausEnergyBack = new LandauGauss("lanGausEnergyBack","lanGausEnergyBack",energy,langausEnergyWidth,langausEnergyMP,langausEnergyArea,langausEnergyGSigma);
  RooGaussian* gaussianTimeMuonsBack = new RooGaussian("gaussianTimeMuonsBack","gaussianTimeMuonsBack",time,gaussianTimeMuonsMean,timingRes);
  RooProdPdf energyAndTimeBackModel("energyAndTimeBackModel","energyAndTimeBackModel",*langausEnergyBack,Conditional(*gaussianTimeMuonsBack,time));

  // 1-D model vars -- background
  //RooRealVar backEnergyArea("E_backArea","E_backArea",75.53,0,100);
  RooRealVar backEnergyArea("E_backArea","E_backArea",1.0);
  RooRealVar backEnergyGSigma("E_backGSigma","E_backGSigma",0.03954,0,0.1);
  RooRealVar backEnergyMP("E_backMP","E_backMP",0.317,0,0.5);
  RooRealVar backEnergyWidth("E_backWidth","E_backWidth",0.01606,0,0.1);
  RooRealVar backTimeMean("T_backMean","T_backMean",2.338,0,5);
  RooRealVar backTimeSigma("T_backSigma","T_backSigma",2.991,0,6); 
  LandauGauss* backEnergyModel1D = new LandauGauss("backEnergyModel1D","backEnergyModel1D",energy,backEnergyWidth,backEnergyMP,backEnergyArea,backEnergyGSigma);
  RooGaussian* backTimeModel1D = new RooGaussian("backTimeModel1D","backTimeModel1D",time,backTimeMean,backTimeSigma);

  // Fit, maximizing signal fraction
  //energyAndTimeBackModel.fitTo(*energyTimeData);

  // Black magic from the Tag and Probe code
  RooFitResult *fitResult = 0;
  //RooAbsData::ErrorType fitError = RooAbsData::SumW2;
  //RooAbsData::ErrorType fitError = RooAbsData::Poisson;
  RooNLLVar nll("nll","nll",energyAndTimeBackModel,*energyTimeData);
  RooMinuit m(nll);
  //m.setErrorLevel(0.5);
  m.setStrategy(2);
  //m.hesse();
  m.migrad();
  //m.hesse();
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

  backEnergyModel1D->fitTo(*energyData);
  backTimeModel1D->fitTo(*timeData);

  TFile output("myLikelihoodOutput_bkgnd.root","RECREATE");

  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,2500,1000);
  combinedCanvas->Divide(3,1);
  combinedCanvas->cd(1);
  //Plot energy curves
  RooPlot* energyFrame = energy.frame();
  energyTimeData->plotOn(energyFrame);
  energyAndTimeBackModel.plotOn(energyFrame);
  energyAndTimeBackModel.paramOn(energyFrame,Layout(0.55));
  energyFrame->Draw("e0");
  combinedCanvas->cd(2);
  //Plot time curves
  RooPlot* timeFrame = time.frame();
  energyTimeData->plotOn(timeFrame);
  energyAndTimeBackModel.plotOn(timeFrame);
  energyAndTimeBackModel.paramOn(timeFrame,Layout(0.55));
  timeFrame->Draw("e0");

  combinedCanvas->cd(3);
  //Make 2-D plot of PDF
  TH1* hh_model = energyAndTimeBackModel.createHistogram("hh_model",energy,Binning(50),YVar(time,Binning(50))) ;
  hh_model->SetLineColor(kBlue);
  //TCanvas* modelCanvas = new TCanvas("2DCanvas","2DCanvas",600,600);
  //modelCanvas->cd();
  hh_model->Draw("surf");
  TH2F* hh1 = energyTimeData->createHistogram(energy,time);
  hh1->Draw("same");
  //energyTimeData->Draw();
  //energyTimeData->Draw("time");
  //Print
  //combinedCanvas->Print("plotLikelihoods.png");
  combinedCanvas->Write();
  hh_model->Write();
  hh1->Write();
  //modelCanvas->Print("plotLikelihoods2D_data.png");

  // Projection 0 < E < 0.2
  TCanvas projectionCanvas1("Energy0to0.2","energy 0 to 0.2 GeV");
  projectionCanvas1.cd();
  energy.setRange("signal",0,0.2);
  //RooPlot* frame = time.frame();
  //energyTimeData->plotOn(frame,CutRange("signal"));
  //energyAndTimeBackModel.plotOn(frame,ProjectionRange("signal"));
  //energyAndTimeBackModel.paramOn(frame);
  //frame->Draw("e0");
  TH1D* energy0to2Hist = energyTimeData->createHistogram(time,energy,"energy>0 && energy<0.2")->ProjectionX();
  energy0to2Hist->Fit("gaus");
  energy0to2Hist->Draw();
  projectionCanvas1.Write();

  // Projection 0.2 < E < 0.4
  TCanvas projectionCanvas2("Energy0.2to0.4","energy 0.2 to 0.4 GeV");
  projectionCanvas2.cd();
  energy.setRange("signal",0.2,0.4);
  //RooPlot* frame2 = time.frame();
  //energyTimeData->plotOn(frame2,CutRange("signal"));
  //energyAndTimeBackModel.plotOn(frame2,ProjectionRange("signal"));
  //energyAndTimeBackModel.paramOn(frame2);
  //frame2->Draw("e0");
  TH1D* energy2to4Hist = energyTimeData->createHistogram(time,energy,"energy>0.2 && energy<0.4")->ProjectionX();
  energy2to4Hist->Fit("gaus");
  energy2to4Hist->Draw();
  projectionCanvas2.Write();

  // Projection 0.2 < E < 0.28
  TCanvas projectionCanvas6("Energy0.2to0.28","energy 0.2 to 0.28 GeV");
  projectionCanvas6.cd();
  energy.setRange("signal",0.2,0.28);
  TH1D* energy2to28Hist = energyTimeData->createHistogram(time,energy,"energy>0.2 && energy<0.28")->ProjectionX();
  energy2to28Hist->Fit("gaus");
  energy2to28Hist->Draw();
  projectionCanvas6.Write();
  // Projection 0.28 < E < 0.4
  TCanvas projectionCanvas7("Energy0.28to0.4","energy 0.28 to 0.4 GeV");
  projectionCanvas7.cd();
  energy.setRange("signal",0.28,0.4);
  TH1D* energy28to4Hist = energyTimeData->createHistogram(time,energy,"energy>0.28 && energy<0.4")->ProjectionX();
  energy28to4Hist->Fit("gaus");
  energy28to4Hist->Draw();
  projectionCanvas7.Write();
  
  // Projection 0.4 < E < 0.6
  TCanvas projectionCanvas3("Energy0.4to0.6","energy 0.4 to 0.6 GeV");
  projectionCanvas3.cd();
  energy.setRange("signal",0.4,0.6);
  //RooPlot* frame3 = time.frame();
  //energyTimeData->plotOn(frame3,CutRange("signal"));
  //energyAndTimeBackModel.plotOn(frame3,ProjectionRange("signal"));
  //energyAndTimeBackModel.paramOn(frame3);
  TH1D* energy4to6Hist = energyTimeData->createHistogram(time,energy,"energy>0.4 && energy<0.6")->ProjectionX();
  energy4to6Hist->Fit("gaus");
  energy4to6Hist->Draw();
  projectionCanvas3.Write();
  
  // Projection 0.6 < E < 0.8
  TCanvas projectionCanvas4("Energy0.6to0.8","energy 0.6 to 0.8 GeV");
  projectionCanvas4.cd();
  energy.setRange("signal",0.6,0.8);
  //RooPlot* frame4 = time.frame();
  //energyTimeData->plotOn(frame4,CutRange("signal"));
  //energyAndTimeBackModel.plotOn(frame4,ProjectionRange("signal"));
  //energyAndTimeBackModel.paramOn(frame4);
  TH1D* energy6to8Hist = energyTimeData->createHistogram(time,energy,"energy>0.6 && energy<0.8")->ProjectionX();
  energy6to8Hist->Fit("gaus");
  energy6to8Hist->Draw();
  projectionCanvas4.Write();
  
  // Projection 0.8 < E < 1.0
  TCanvas projectionCanvas5("Energy0.8to1.0","energy 0.8 to 1.0 GeV");
  projectionCanvas5.cd();
  energy.setRange("signal",0.8,1.0);
  //RooPlot* frame5 = time.frame();
  //energyTimeData->plotOn(frame5,CutRange("signal"));
  //energyAndTimeBackModel.plotOn(frame5,ProjectionRange("signal"));
  //energyAndTimeBackModel.paramOn(frame5);
  TH1D* energy8to10Hist = energyTimeData->createHistogram(time,energy,"energy>0.8 && energy<1.0")->ProjectionX();
  energy8to10Hist->Fit("gaus");
  energy8to10Hist->Draw();
  //frame5->Draw("e0");
  projectionCanvas5.Write();


  // 1-D plots
  TCanvas* combinedCanvas1D = new TCanvas("combinedCanvas1D","combinedCanvas1D",1,1,1800,2000);
  combinedCanvas1D->Divide(1,2);
  combinedCanvas1D->cd(1);
  RooPlot* energyFrame2 = energy.frame();
  energyData->plotOn(energyFrame2);
  backEnergyModel1D->plotOn(energyFrame2);
  backEnergyModel1D->paramOn(energyFrame2,Layout(0.55));
  energyFrame2->Draw("e0");
  combinedCanvas1D->cd(2);
  RooPlot* timeFrame2 = time.frame();
  timeData->plotOn(timeFrame2);
  backTimeModel1D->plotOn(timeFrame2);
  backTimeModel1D->paramOn(timeFrame2,Layout(0.7));
  timeFrame2->Draw("e0");
  //Print
  //combinedCanvas1D->Print("plotLikelihoods1D.png");
  combinedCanvas1D->Write();

  output.Close();

  //Loop over hists: energyHist, timeHist and calculate the likelihood
  //float logLikelihood = 0;
  //float logLikelihoodBack = 0;
  //float ntupEnergy, ntupTime;
  //energyTimeTNtuple->SetBranchAddress("energy",&ntupEnergy);
  //energyTimeTNtuple->SetBranchAddress("time",&ntupTime);
  //for(int entry = 1; entry < energyTimeTNtuple->GetEntries(); entry++)
  //{
  //  energyTimeTNtuple->GetEntry(entry);
  //  energy = ntupEnergy;
  //  time = ntupTime;
  //  logLikelihood+=log(energyAndTimeModel.getVal(new RooArgSet(energy,time)));
  //  logLikelihoodBack+=log(energyAndTimeBackModel.getVal(new RooArgSet(energy,time)));
  //  //std::cout << "LogLikelihood = " << log(energyAndTimeModel.getVal(energyTimeArgSet)) << std::endl;
  //  //std::cout << "LogLikelihoodBack = " << log(energyAndTimeBackModel.getVal(energyTimeArgSet)) << std::endl;
  //}

  //f->Close();

  //cout << "Likelihood for S+B model: " << logLikelihood << " Likelihood for B only model: " << logLikelihoodBack << endl;
  //cout << "Log likelihood difference: " << logLikelihood-logLikelihoodBack << endl;
  //
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
