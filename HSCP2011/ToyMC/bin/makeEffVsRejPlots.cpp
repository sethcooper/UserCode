#include "TH1F.h"
#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TColor.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

void SetEStyle() {
  TStyle* EStyle = new TStyle("EStyle", "E's not Style");

  //set the background color to white
  //EStyle->SetFillColor(kWhite);
  EStyle->SetHistFillColor(kWhite);
  EStyle->SetFrameFillColor(kWhite);
  EStyle->SetFrameFillStyle(0);
  EStyle->SetFillStyle(kWhite);
  EStyle->SetCanvasColor(kWhite);
  EStyle->SetPadColor(kWhite);
  EStyle->SetTitleFillColor(kWhite);
  EStyle->SetStatColor(kWhite);

  //dont put a colored frame around the plots
  EStyle->SetFrameBorderMode(0);
  EStyle->SetCanvasBorderMode(0);
  EStyle->SetPadBorderMode(0);

  //use the primary color palette
  EStyle->SetPalette(1);
  //Code i found for sweet palette
  const Int_t NRGBs = 255;
  const Int_t NCont = 25;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  EStyle->SetNumberContours(NCont);


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
  //EStyle->SetOptStat(1111111);
  EStyle->SetOptStat(2222211);

  //Stat box width/height
  EStyle->SetStatW(0.2);                
  EStyle->SetStatH(0.2); 
  
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


std::string intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

std::string floatToString(float num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

int main (int argc, char** argv)
{
  using namespace std;

  char* inFileSig = argv[1];
  char* inFileBack = argv[2];

  if(!inFileSig)
  {
    cout << " No input signal file specified !" << endl;
    return -1;
  }
  if(!inFileBack)
  {
    cout << " No input background file specified !" << endl;
    return -1;
  }

  SetEStyle();
  // fill signal hists
  TH1F* d1BetaSlicesSig[5];
  TH1F* d2BetaSlicesSig[5];
  TH1F* d3BetaSlicesSig[5];
  TH1F* e1BetaSlicesSig[5];
  TH1F* e2BetaSlicesSig[5];
  TH1F* e3BetaSlicesSig[5];
  TFile* tFileSig = new TFile(inFileSig);
  tFileSig->cd();
  d1BetaSlicesSig[0] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice1");
  d1BetaSlicesSig[1] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice2");
  d1BetaSlicesSig[2] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice3");
  d1BetaSlicesSig[3] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice4");
  d1BetaSlicesSig[4] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice5");
  d2BetaSlicesSig[0] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice1");
  d2BetaSlicesSig[1] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice2");
  d2BetaSlicesSig[2] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice3");
  d2BetaSlicesSig[3] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice4");
  d2BetaSlicesSig[4] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice5");
  d3BetaSlicesSig[0] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice1");
  d3BetaSlicesSig[1] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice2");
  d3BetaSlicesSig[2] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice3");
  d3BetaSlicesSig[3] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice4");
  d3BetaSlicesSig[4] = (TH1F*)tFileSig->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice5");
  e1BetaSlicesSig[0] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice1");
  e1BetaSlicesSig[1] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice2");
  e1BetaSlicesSig[2] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice3");
  e1BetaSlicesSig[3] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice4");
  e1BetaSlicesSig[4] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice5");
  e2BetaSlicesSig[0] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice1");
  e2BetaSlicesSig[1] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice2");
  e2BetaSlicesSig[2] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice3");
  e2BetaSlicesSig[3] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice4");
  e2BetaSlicesSig[4] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice5");
  e3BetaSlicesSig[0] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice1");
  e3BetaSlicesSig[1] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice2");
  e3BetaSlicesSig[2] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice3");
  e3BetaSlicesSig[3] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice4");
  e3BetaSlicesSig[4] = (TH1F*)tFileSig->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice5");

  // fill background hists
  //TH1F* d1BetaSlicesBack[5];
  //TH1F* d2BetaSlicesBack[5];
  //TH1F* d3BetaSlicesBack[5];
  //TH1F* e1BetaSlicesBack[5];
  //TH1F* e2BetaSlicesBack[5];
  //TH1F* e3BetaSlicesBack[5];
  TFile* tFileBack = new TFile(inFileBack);
  tFileBack->cd();

  TH1F* backTkDeDxD1Hist = (TH1F*)tFileBack->Get("trackDeDxD1");
  TH1F* backTkDeDxD2Hist = (TH1F*)tFileBack->Get("trackDeDxD2");
  TH1F* backTkDeDxD3Hist = (TH1F*)tFileBack->Get("trackDeDxD3");
  TH1F* backTkDeDxE1Hist = (TH1F*)tFileBack->Get("trackDeDxE1");
  TH1F* backTkDeDxE2Hist = (TH1F*)tFileBack->Get("trackDeDxE2");
  TH1F* backTkDeDxE3Hist = (TH1F*)tFileBack->Get("trackDeDxE3");
  //d1BetaSlicesBack[0] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice1");
  //d1BetaSlicesBack[1] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice2");
  //d1BetaSlicesBack[2] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice3");
  //d1BetaSlicesBack[3] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice4");
  //d1BetaSlicesBack[4] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD1BetaSlice5");
  //d2BetaSlicesBack[0] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice1");
  //d2BetaSlicesBack[1] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice2");
  //d2BetaSlicesBack[2] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice3");
  //d2BetaSlicesBack[3] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice4");
  //d2BetaSlicesBack[4] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD2BetaSlice5");
  //d3BetaSlicesBack[0] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice1");
  //d3BetaSlicesBack[1] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice2");
  //d3BetaSlicesBack[2] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice3");
  //d3BetaSlicesBack[3] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice4");
  //d3BetaSlicesBack[4] = (TH1F*)tFileBack->Get("TkDeDxDiscBetaSlices/trackDeDxD3BetaSlice5");
  //e1BetaSlicesBack[0] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice1");
  //e1BetaSlicesBack[1] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice2");
  //e1BetaSlicesBack[2] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice3");
  //e1BetaSlicesBack[3] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice4");
  //e1BetaSlicesBack[4] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE1BetaSlice5");
  //e2BetaSlicesBack[0] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice1");
  //e2BetaSlicesBack[1] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice2");
  //e2BetaSlicesBack[2] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice3");
  //e2BetaSlicesBack[3] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice4");
  //e2BetaSlicesBack[4] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE2BetaSlice5");
  //e3BetaSlicesBack[0] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice1");
  //e3BetaSlicesBack[1] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice2");
  //e3BetaSlicesBack[2] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice3");
  //e3BetaSlicesBack[3] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice4");
  //e3BetaSlicesBack[4] = (TH1F*)tFileBack->Get("TkDeDxEstBetaSlices/trackDeDxE3BetaSlice5");

  // output
  TFile* outputTFile = new TFile("effVsRejPlot.root","recreate");
  outputTFile->cd();
  TGraph* effVsCutValD1BetaSliceGraphs[5];
  TGraph* effVsCutValD2BetaSliceGraphs[5];
  TGraph* effVsCutValD3BetaSliceGraphs[5];
  TGraph* effVsCutValE1BetaSliceGraphs[5];
  TGraph* effVsCutValE2BetaSliceGraphs[5];
  TGraph* effVsCutValE3BetaSliceGraphs[5];
  //TGraph* rejVsCutValD1BetaSliceGraphs[5];
  //TGraph* rejVsCutValD2BetaSliceGraphs[5];
  //TGraph* rejVsCutValD3BetaSliceGraphs[5];
  //TGraph* rejVsCutValE1BetaSliceGraphs[5];
  //TGraph* rejVsCutValE2BetaSliceGraphs[5];
  //TGraph* rejVsCutValE3BetaSliceGraphs[5];
  TGraph* rejVsCutValD1;
  TGraph* rejVsCutValD2;
  TGraph* rejVsCutValD3;
  TGraph* rejVsCutValE1;
  TGraph* rejVsCutValE2;
  TGraph* rejVsCutValE3;
  // Graphs for eff vs rej
  TGraph* efficiencyVsRejectionD1Graph;
  TGraph* efficiencyVsRejectionD2Graph;
  TGraph* efficiencyVsRejectionD3Graph;
  TGraph* efficiencyVsRejectionE1Graph;
  TGraph* efficiencyVsRejectionE2Graph;
  TGraph* efficiencyVsRejectionE3Graph;

  // calculate quantities
  vector<double> cutVals;
  vector<double> effByCutValD1BetaSlices[5];
  vector<double> effByCutValD2BetaSlices[5];
  vector<double> effByCutValD3BetaSlices[5];
  vector<double> effByCutValE1BetaSlices[5];
  vector<double> effByCutValE2BetaSlices[5];
  vector<double> effByCutValE3BetaSlices[5];
  //vector<float> rejByCutValD1BetaSlices[5];
  //vector<float> rejByCutValD2BetaSlices[5];
  //vector<float> rejByCutValD3BetaSlices[5];
  //vector<float> rejByCutValE1BetaSlices[5];
  //vector<float> rejByCutValE2BetaSlices[5];
  //vector<float> rejByCutValE3BetaSlices[5];
  vector<double> rejByCutValD1;
  vector<double> rejByCutValD2;
  vector<double> rejByCutValD3;
  vector<double> rejByCutValE1;
  vector<double> rejByCutValE2;
  vector<double> rejByCutValE3;

  // Apply loose cut on the variable and calculate
  int numBins = 100;

  for(int i=0; i<50; ++i) // loop over cut values
  {
    float cutVal = sqrt(i/50.0);
    int cutValDbin = cutVal*numBins+1;
    int cutValEbin = i;
    //debug
    //cout << "i: " << i << " cutvalDbin: " << cutValDbin << " cutValEbin: " << cutValEbin << endl;
    cutVals.push_back(cutVal);

    // do back graphs
    rejByCutValD1.push_back(-1*log(backTkDeDxD1Hist->Integral(cutValDbin,101)/(double)backTkDeDxD1Hist->Integral()));
    rejByCutValD2.push_back(-1*log(backTkDeDxD2Hist->Integral(cutValDbin,101)/(double)backTkDeDxD2Hist->Integral()));
    rejByCutValD3.push_back(-1*log(backTkDeDxD3Hist->Integral(cutValDbin,101)/(double)backTkDeDxD3Hist->Integral()));
    rejByCutValE1.push_back(-1*log(backTkDeDxE1Hist->Integral(cutValEbin,101)/(double)backTkDeDxE1Hist->Integral()));
    rejByCutValE2.push_back(-1*log(backTkDeDxE2Hist->Integral(cutValEbin,101)/(double)backTkDeDxE2Hist->Integral()));
    rejByCutValE3.push_back(-1*log(backTkDeDxE3Hist->Integral(cutValEbin,101)/(double)backTkDeDxE3Hist->Integral()));

    effByCutValD1BetaSlices[0].push_back(d1BetaSlicesSig[0]->Integral(cutValDbin,101)/d1BetaSlicesSig[0]->Integral());
    effByCutValD2BetaSlices[0].push_back(d2BetaSlicesSig[0]->Integral(cutValDbin,101)/d2BetaSlicesSig[0]->Integral());
    effByCutValD3BetaSlices[0].push_back(d3BetaSlicesSig[0]->Integral(cutValDbin,101)/d3BetaSlicesSig[0]->Integral());
    effByCutValE1BetaSlices[0].push_back(e1BetaSlicesSig[0]->Integral(cutValEbin,101)/e1BetaSlicesSig[0]->Integral());
    effByCutValE2BetaSlices[0].push_back(e2BetaSlicesSig[0]->Integral(cutValEbin,101)/e2BetaSlicesSig[0]->Integral());
    effByCutValE3BetaSlices[0].push_back(e3BetaSlicesSig[0]->Integral(cutValEbin,101)/e3BetaSlicesSig[0]->Integral());
    //rejByCutValD1BetaSlices[0].push_back(d1BetaSlicesBack[0]->Integral(cutValDbin,101)/d1BetaSlicesBack[0]->Integral());
    //rejByCutValD2BetaSlices[0].push_back(d2BetaSlicesBack[0]->Integral(cutValDbin,101)/d2BetaSlicesBack[0]->Integral());
    //rejByCutValD3BetaSlices[0].push_back(d3BetaSlicesBack[0]->Integral(cutValDbin,101)/d3BetaSlicesBack[0]->Integral());
    //rejByCutValE1BetaSlices[0].push_back(e1BetaSlicesBack[0]->Integral(cutValEbin,101)/e1BetaSlicesBack[0]->Integral());
    //rejByCutValE2BetaSlices[0].push_back(e2BetaSlicesBack[0]->Integral(cutValEbin,101)/e2BetaSlicesBack[0]->Integral());
    //rejByCutValE3BetaSlices[0].push_back(e3BetaSlicesBack[0]->Integral(cutValEbin,101)/e3BetaSlicesBack[0]->Integral());

    effByCutValD1BetaSlices[1].push_back(d1BetaSlicesSig[1]->Integral(cutValDbin,101)/d1BetaSlicesSig[1]->Integral());
    effByCutValD2BetaSlices[1].push_back(d2BetaSlicesSig[1]->Integral(cutValDbin,101)/d2BetaSlicesSig[1]->Integral());
    effByCutValD3BetaSlices[1].push_back(d3BetaSlicesSig[1]->Integral(cutValDbin,101)/d3BetaSlicesSig[1]->Integral());
    effByCutValE1BetaSlices[1].push_back(e1BetaSlicesSig[1]->Integral(cutValEbin,101)/e1BetaSlicesSig[1]->Integral());
    effByCutValE2BetaSlices[1].push_back(e2BetaSlicesSig[1]->Integral(cutValEbin,101)/e2BetaSlicesSig[1]->Integral());
    effByCutValE3BetaSlices[1].push_back(e3BetaSlicesSig[1]->Integral(cutValEbin,101)/e3BetaSlicesSig[1]->Integral());
    //rejByCutValD1BetaSlices[1].push_back(d1BetaSlicesBack[1]->Integral(cutValDbin,101)/d1BetaSlicesBack[1]->Integral());
    //rejByCutValD2BetaSlices[1].push_back(d2BetaSlicesBack[1]->Integral(cutValDbin,101)/d2BetaSlicesBack[1]->Integral());
    //rejByCutValD3BetaSlices[1].push_back(d3BetaSlicesBack[1]->Integral(cutValDbin,101)/d3BetaSlicesBack[1]->Integral());
    //rejByCutValE1BetaSlices[1].push_back(e1BetaSlicesBack[1]->Integral(cutValEbin,101)/e1BetaSlicesBack[1]->Integral());
    //rejByCutValE2BetaSlices[1].push_back(e2BetaSlicesBack[1]->Integral(cutValEbin,101)/e2BetaSlicesBack[1]->Integral());
    //rejByCutValE3BetaSlices[1].push_back(e3BetaSlicesBack[1]->Integral(cutValEbin,101)/e3BetaSlicesBack[1]->Integral());

    effByCutValD1BetaSlices[2].push_back(d1BetaSlicesSig[2]->Integral(cutValDbin,101)/d1BetaSlicesSig[2]->Integral());
    effByCutValD2BetaSlices[2].push_back(d2BetaSlicesSig[2]->Integral(cutValDbin,101)/d2BetaSlicesSig[2]->Integral());
    effByCutValD3BetaSlices[2].push_back(d3BetaSlicesSig[2]->Integral(cutValDbin,101)/d3BetaSlicesSig[2]->Integral());
    effByCutValE1BetaSlices[2].push_back(e1BetaSlicesSig[2]->Integral(cutValEbin,101)/e1BetaSlicesSig[2]->Integral());
    effByCutValE2BetaSlices[2].push_back(e2BetaSlicesSig[2]->Integral(cutValEbin,101)/e2BetaSlicesSig[2]->Integral());
    effByCutValE3BetaSlices[2].push_back(e3BetaSlicesSig[2]->Integral(cutValEbin,101)/e3BetaSlicesSig[2]->Integral());
    //rejByCutValD1BetaSlices[2].push_back(d1BetaSlicesBack[2]->Integral(cutValDbin,101)/d1BetaSlicesBack[2]->Integral());
    //rejByCutValD2BetaSlices[2].push_back(d2BetaSlicesBack[2]->Integral(cutValDbin,101)/d2BetaSlicesBack[2]->Integral());
    //rejByCutValD3BetaSlices[2].push_back(d3BetaSlicesBack[2]->Integral(cutValDbin,101)/d3BetaSlicesBack[2]->Integral());
    //rejByCutValE1BetaSlices[2].push_back(e1BetaSlicesBack[2]->Integral(cutValEbin,101)/e1BetaSlicesBack[2]->Integral());
    //rejByCutValE2BetaSlices[2].push_back(e2BetaSlicesBack[2]->Integral(cutValEbin,101)/e2BetaSlicesBack[2]->Integral());
    //rejByCutValE3BetaSlices[2].push_back(e3BetaSlicesBack[2]->Integral(cutValEbin,101)/e3BetaSlicesBack[2]->Integral());

    effByCutValD1BetaSlices[3].push_back(d1BetaSlicesSig[3]->Integral(cutValDbin,101)/d1BetaSlicesSig[3]->Integral());
    effByCutValD2BetaSlices[3].push_back(d2BetaSlicesSig[3]->Integral(cutValDbin,101)/d2BetaSlicesSig[3]->Integral());
    effByCutValD3BetaSlices[3].push_back(d3BetaSlicesSig[3]->Integral(cutValDbin,101)/d3BetaSlicesSig[3]->Integral());
    effByCutValE1BetaSlices[3].push_back(e1BetaSlicesSig[3]->Integral(cutValEbin,101)/e1BetaSlicesSig[3]->Integral());
    effByCutValE2BetaSlices[3].push_back(e2BetaSlicesSig[3]->Integral(cutValEbin,101)/e2BetaSlicesSig[3]->Integral());
    effByCutValE3BetaSlices[3].push_back(e3BetaSlicesSig[3]->Integral(cutValEbin,101)/e3BetaSlicesSig[3]->Integral());
    //rejByCutValD1BetaSlices[3].push_back(d1BetaSlicesBack[3]->Integral(cutValDbin,101)/d1BetaSlicesBack[3]->Integral());
    //rejByCutValD2BetaSlices[3].push_back(d2BetaSlicesBack[3]->Integral(cutValDbin,101)/d2BetaSlicesBack[3]->Integral());
    //rejByCutValD3BetaSlices[3].push_back(d3BetaSlicesBack[3]->Integral(cutValDbin,101)/d3BetaSlicesBack[3]->Integral());
    //rejByCutValE1BetaSlices[3].push_back(e1BetaSlicesBack[3]->Integral(cutValEbin,101)/e1BetaSlicesBack[3]->Integral());
    //rejByCutValE2BetaSlices[3].push_back(e2BetaSlicesBack[3]->Integral(cutValEbin,101)/e2BetaSlicesBack[3]->Integral());
    //rejByCutValE3BetaSlices[3].push_back(e3BetaSlicesBack[3]->Integral(cutValEbin,101)/e3BetaSlicesBack[3]->Integral());

    effByCutValD1BetaSlices[4].push_back(d1BetaSlicesSig[4]->Integral(cutValDbin,101)/d1BetaSlicesSig[4]->Integral());
    effByCutValD2BetaSlices[4].push_back(d2BetaSlicesSig[4]->Integral(cutValDbin,101)/d2BetaSlicesSig[4]->Integral());
    effByCutValD3BetaSlices[4].push_back(d3BetaSlicesSig[4]->Integral(cutValDbin,101)/d3BetaSlicesSig[4]->Integral());
    effByCutValE1BetaSlices[4].push_back(e1BetaSlicesSig[4]->Integral(cutValEbin,101)/e1BetaSlicesSig[4]->Integral());
    effByCutValE2BetaSlices[4].push_back(e2BetaSlicesSig[4]->Integral(cutValEbin,101)/e2BetaSlicesSig[4]->Integral());
    effByCutValE3BetaSlices[4].push_back(e3BetaSlicesSig[4]->Integral(cutValEbin,101)/e3BetaSlicesSig[4]->Integral());
    //rejByCutValD1BetaSlices[4].push_back(d1BetaSlicesBack[4]->Integral(cutValDbin,101)/d1BetaSlicesBack[4]->Integral());
    //rejByCutValD2BetaSlices[4].push_back(d2BetaSlicesBack[4]->Integral(cutValDbin,101)/d2BetaSlicesBack[4]->Integral());
    //rejByCutValD3BetaSlices[4].push_back(d3BetaSlicesBack[4]->Integral(cutValDbin,101)/d3BetaSlicesBack[4]->Integral());
    //rejByCutValE1BetaSlices[4].push_back(e1BetaSlicesBack[4]->Integral(cutValEbin,101)/e1BetaSlicesBack[4]->Integral());
    //rejByCutValE2BetaSlices[4].push_back(e2BetaSlicesBack[4]->Integral(cutValEbin,101)/e2BetaSlicesBack[4]->Integral());
    //rejByCutValE3BetaSlices[4].push_back(e3BetaSlicesBack[4]->Integral(cutValEbin,101)/e3BetaSlicesBack[4]->Integral());
  }

  // Make the graphs
  for(int i=0; i < 5; ++i) // loop over beta slices
  {
    effVsCutValD1BetaSliceGraphs[i] = new TGraph(effByCutValD1BetaSlices[i].size(),&(*cutVals.begin()),&(*effByCutValD1BetaSlices[i].begin()));
    effVsCutValD2BetaSliceGraphs[i] = new TGraph(effByCutValD2BetaSlices[i].size(),&(*cutVals.begin()),&(*effByCutValD2BetaSlices[i].begin()));
    effVsCutValD3BetaSliceGraphs[i] = new TGraph(effByCutValD3BetaSlices[i].size(),&(*cutVals.begin()),&(*effByCutValD3BetaSlices[i].begin()));
    effVsCutValE1BetaSliceGraphs[i] = new TGraph(effByCutValE1BetaSlices[i].size(),&(*cutVals.begin()),&(*effByCutValE1BetaSlices[i].begin()));
    effVsCutValE2BetaSliceGraphs[i] = new TGraph(effByCutValE2BetaSlices[i].size(),&(*cutVals.begin()),&(*effByCutValE2BetaSlices[i].begin()));
    effVsCutValE3BetaSliceGraphs[i] = new TGraph(effByCutValE3BetaSlices[i].size(),&(*cutVals.begin()),&(*effByCutValE3BetaSlices[i].begin()));
  }

  rejVsCutValD1 = new TGraph(rejByCutValD1.size(),&(*cutVals.begin()),&(*rejByCutValD1.begin()));
  rejVsCutValD2 = new TGraph(rejByCutValD2.size(),&(*cutVals.begin()),&(*rejByCutValD2.begin()));
  rejVsCutValD3 = new TGraph(rejByCutValD3.size(),&(*cutVals.begin()),&(*rejByCutValD3.begin()));
  rejVsCutValE1 = new TGraph(rejByCutValE1.size(),&(*cutVals.begin()),&(*rejByCutValE1.begin()));
  rejVsCutValE2 = new TGraph(rejByCutValE2.size(),&(*cutVals.begin()),&(*rejByCutValE2.begin()));
  rejVsCutValE3 = new TGraph(rejByCutValE3.size(),&(*cutVals.begin()),&(*rejByCutValE3.begin()));

  outputTFile->cd();
  for(int i=0; i < 5; ++i)
  {
    float betaMin = i/5.0;
    float betaMax = betaMin+0.2;
    string betaString = "Beta";
    betaString+=floatToString(betaMin);
    betaString+="to";
    betaString+=floatToString(betaMax);

    string titleEffD1 = "effVsCutValD1From";
    titleEffD1+=betaString;
    effVsCutValD1BetaSliceGraphs[i]->SetNameTitle(titleEffD1.c_str(),titleEffD1.c_str());
    effVsCutValD1BetaSliceGraphs[i]->Write();
    string titleEffD2 = "effVsCutValD2From";
    titleEffD2+=betaString;
    effVsCutValD2BetaSliceGraphs[i]->SetNameTitle(titleEffD2.c_str(),titleEffD2.c_str());
    effVsCutValD2BetaSliceGraphs[i]->Write();
    string titleEffD3 = "effVsCutValD3From";
    titleEffD3+=betaString;
    effVsCutValD3BetaSliceGraphs[i]->SetNameTitle(titleEffD3.c_str(),titleEffD3.c_str());
    effVsCutValD3BetaSliceGraphs[i]->Write();
    string titleEffE1 = "effVsCutValE1From";
    titleEffE1+=betaString;
    effVsCutValE1BetaSliceGraphs[i]->SetNameTitle(titleEffE1.c_str(),titleEffE1.c_str());
    effVsCutValE1BetaSliceGraphs[i]->Write();
    string titleEffE2 = "effVsCutValE2From";
    titleEffE2+=betaString;
    effVsCutValE2BetaSliceGraphs[i]->SetNameTitle(titleEffE2.c_str(),titleEffE2.c_str());
    effVsCutValE2BetaSliceGraphs[i]->Write();
    string titleEffE3 = "effVsCutValE3From";
    titleEffE3+=betaString;
    effVsCutValE3BetaSliceGraphs[i]->SetNameTitle(titleEffE3.c_str(),titleEffE3.c_str());
    effVsCutValE3BetaSliceGraphs[i]->Write();
  }

  rejVsCutValD1->SetNameTitle("rejVsCutValD1","rejVsCutValD1");
  rejVsCutValD2->SetNameTitle("rejVsCutValD2","rejVsCutValD2");
  rejVsCutValD3->SetNameTitle("rejVsCutValD3","rejVsCutValD3");
  rejVsCutValE1->SetNameTitle("rejVsCutValE1","rejVsCutValE1");
  rejVsCutValE2->SetNameTitle("rejVsCutValE2","rejVsCutValE2");
  rejVsCutValE3->SetNameTitle("rejVsCutValE3","rejVsCutValE3");
  rejVsCutValD1->Write();
  rejVsCutValD2->Write();
  rejVsCutValD3->Write();
  rejVsCutValE1->Write();
  rejVsCutValE2->Write();
  rejVsCutValE3->Write();

  TCanvas t;
  t.cd();
  efficiencyVsRejectionD1Graph = new TGraph(rejByCutValD1.size(),&(*rejByCutValD1.begin()),&(*effByCutValD1BetaSlices[4].begin()));
  efficiencyVsRejectionD1Graph->SetTitle("discriminator 1");
  efficiencyVsRejectionD1Graph->GetXaxis()->SetTitle("-log(background efficiency)");
  efficiencyVsRejectionD1Graph->GetYaxis()->SetTitle("signal efficiency");
  efficiencyVsRejectionD1Graph->Draw("ap*");
  TLegend* tleg = new TLegend(0.2,0.2,0.4,0.4);
  tleg->AddEntry(efficiencyVsRejectionD1Graph,"#beta > 0.8","p");
  TGraph* temp = new TGraph(rejByCutValD1.size(),&(*rejByCutValD1.begin()),&(*effByCutValD1BetaSlices[3].begin()));
  temp->SetMarkerColor(2);
  temp->Draw("p*,same");
  tleg->AddEntry(temp,"0.8 > #beta > 0.6","p");
  TGraph* temp2 = new TGraph(rejByCutValD1.size(),&(*rejByCutValD1.begin()),&(*effByCutValD1BetaSlices[2].begin()));
  temp2->SetMarkerColor(3);
  temp2->Draw("p*,same");
  tleg->AddEntry(temp2,"0.6 > #beta > 0.4","p");
  TGraph* temp3 = new TGraph(rejByCutValD1.size(),&(*rejByCutValD1.begin()),&(*effByCutValD1BetaSlices[1].begin()));
  temp3->SetMarkerColor(4);
  temp3->Draw("p*,same");
  tleg->AddEntry(temp3,"0.4 > #beta > 0.2","p");
  tleg->Draw("same");
  t.SetName("efficiencyVsRejectionD1");
  t.SetTitle("efficiencyVsRejectionD1");
  t.Write();
  t.Print("effVsRejDisc1.png");

  TCanvas t2;
  t2.cd();
  efficiencyVsRejectionD2Graph = new TGraph(rejByCutValD2.size(),&(*rejByCutValD2.begin()),&(*effByCutValD2BetaSlices[4].begin()));
  efficiencyVsRejectionD2Graph->SetTitle("discriminator 2");
  efficiencyVsRejectionD2Graph->GetXaxis()->SetTitle("-log(background efficiency)");
  efficiencyVsRejectionD2Graph->GetYaxis()->SetTitle("signal efficiency");
  efficiencyVsRejectionD2Graph->Draw("ap*");
  tleg = new TLegend(0.2,0.2,0.4,0.4);
  tleg->AddEntry(efficiencyVsRejectionD1Graph,"#beta > 0.8","p");
  temp = new TGraph(rejByCutValD2.size(),&(*rejByCutValD2.begin()),&(*effByCutValD2BetaSlices[3].begin()));
  temp->SetMarkerColor(2);
  temp->Draw("p*,same");
  tleg->AddEntry(temp,"0.8 > #beta > 0.6","p");
  temp2 = new TGraph(rejByCutValD2.size(),&(*rejByCutValD2.begin()),&(*effByCutValD2BetaSlices[2].begin()));
  temp2->SetMarkerColor(3);
  temp2->Draw("p*,same");
  tleg->AddEntry(temp2,"0.6 > #beta > 0.4","p");
  temp3 = new TGraph(rejByCutValD2.size(),&(*rejByCutValD2.begin()),&(*effByCutValD2BetaSlices[1].begin()));
  temp3->SetMarkerColor(4);
  temp3->Draw("p*,same");
  tleg->AddEntry(temp3,"0.4 > #beta > 0.2","p");
  tleg->Draw("same");
  t2.SetName("efficiencyVsRejectionD2");
  t2.SetTitle("efficiencyVsRejectionD2");
  t2.Write();
  t2.Print("effVsRejDisc2.png");

  TCanvas t3;
  t3.cd();
  efficiencyVsRejectionD3Graph = new TGraph(rejByCutValD3.size(),&(*rejByCutValD3.begin()),&(*effByCutValD3BetaSlices[4].begin()));
  efficiencyVsRejectionD3Graph->SetTitle("discriminator 3");
  efficiencyVsRejectionD3Graph->GetXaxis()->SetTitle("-log(background efficiency)");
  efficiencyVsRejectionD3Graph->GetYaxis()->SetTitle("signal efficiency");
  efficiencyVsRejectionD3Graph->Draw("ap*");
  tleg = new TLegend(0.2,0.2,0.4,0.4);
  tleg->AddEntry(efficiencyVsRejectionD1Graph,"#beta > 0.8","p");
  temp = new TGraph(rejByCutValD3.size(),&(*rejByCutValD3.begin()),&(*effByCutValD3BetaSlices[3].begin()));
  temp->SetMarkerColor(2);
  temp->Draw("p*,same");
  tleg->AddEntry(temp,"0.8 > #beta > 0.6","p");
  temp2 = new TGraph(rejByCutValD3.size(),&(*rejByCutValD3.begin()),&(*effByCutValD3BetaSlices[2].begin()));
  temp2->SetMarkerColor(3);
  temp2->Draw("p*,same");
  tleg->AddEntry(temp2,"0.6 > #beta > 0.4","p");
  temp3 = new TGraph(rejByCutValD3.size(),&(*rejByCutValD3.begin()),&(*effByCutValD3BetaSlices[1].begin()));
  temp3->SetMarkerColor(4);
  temp3->Draw("p*,same");
  tleg->AddEntry(temp3,"0.4 > #beta > 0.2","p");
  tleg->Draw("same");
  t3.SetName("efficiencyVsRejectionD3");
  t3.SetTitle("efficiencyVsRejectionD3");
  t3.Write();
  t3.Print("effVsRejDisc3.png");

  TCanvas t4;
  t4.cd();
  efficiencyVsRejectionE1Graph = new TGraph(rejByCutValE1.size(),&(*rejByCutValE1.begin()),&(*effByCutValE1BetaSlices[4].begin()));
  efficiencyVsRejectionE1Graph->SetTitle("estimator 1");
  efficiencyVsRejectionE1Graph->GetXaxis()->SetTitle("-log(background efficiency)");
  efficiencyVsRejectionE1Graph->GetYaxis()->SetTitle("signal efficiency");
  efficiencyVsRejectionE1Graph->Draw("ap*");
  tleg = new TLegend(0.2,0.2,0.4,0.4);
  tleg->AddEntry(efficiencyVsRejectionD1Graph,"#beta > 0.8","p");
  temp = new TGraph(rejByCutValE1.size(),&(*rejByCutValE1.begin()),&(*effByCutValE1BetaSlices[3].begin()));
  temp->SetMarkerColor(2);
  temp->Draw("p*,same");
  tleg->AddEntry(temp,"0.8 > #beta > 0.6","p");
  temp2 = new TGraph(rejByCutValE1.size(),&(*rejByCutValE1.begin()),&(*effByCutValE1BetaSlices[2].begin()));
  temp2->SetMarkerColor(3);
  temp2->Draw("p*,same");
  tleg->AddEntry(temp2,"0.6 > #beta > 0.4","p");
  temp3 = new TGraph(rejByCutValE1.size(),&(*rejByCutValE1.begin()),&(*effByCutValE1BetaSlices[1].begin()));
  temp3->SetMarkerColor(4);
  temp3->Draw("p*,same");
  tleg->AddEntry(temp3,"0.4 > #beta > 0.2","p");
  tleg->Draw("same");
  t4.SetName("efficiencyVsRejectionE1");
  t4.SetTitle("efficiencyVsRejectionE1");
  t4.Write();
  t4.Print("effVsRejEst1.png");

  TCanvas t5;
  t5.cd();
  efficiencyVsRejectionE2Graph = new TGraph(rejByCutValE2.size(),&(*rejByCutValE2.begin()),&(*effByCutValE2BetaSlices[4].begin()));
  efficiencyVsRejectionE2Graph->SetTitle("estimator 2");
  efficiencyVsRejectionE2Graph->GetXaxis()->SetTitle("-log(background efficiency)");
  efficiencyVsRejectionE2Graph->GetYaxis()->SetTitle("signal efficiency");
  efficiencyVsRejectionE2Graph->Draw("ap*");
  tleg = new TLegend(0.2,0.2,0.4,0.4);
  tleg->AddEntry(efficiencyVsRejectionD1Graph,"#beta > 0.8","p");
  temp = new TGraph(rejByCutValE2.size(),&(*rejByCutValE2.begin()),&(*effByCutValE2BetaSlices[3].begin()));
  temp->SetMarkerColor(2);
  temp->Draw("p*,same");
  tleg->AddEntry(temp,"0.8 > #beta > 0.6","p");
  temp2 = new TGraph(rejByCutValE2.size(),&(*rejByCutValE2.begin()),&(*effByCutValE2BetaSlices[2].begin()));
  temp2->SetMarkerColor(3);
  temp2->Draw("p*,same");
  tleg->AddEntry(temp2,"0.6 > #beta > 0.4","p");
  temp3 = new TGraph(rejByCutValE2.size(),&(*rejByCutValE2.begin()),&(*effByCutValE2BetaSlices[1].begin()));
  temp3->SetMarkerColor(4);
  temp3->Draw("p*,same");
  tleg->AddEntry(temp3,"0.4 > #beta > 0.2","p");
  tleg->Draw("same");
  t5.SetName("efficiencyVsRejectionE2");
  t5.SetTitle("efficiencyVsRejectionE2");
  t5.Write();
  t5.Print("effVsRejEst2.png");

  TCanvas t6;
  t6.cd();
  efficiencyVsRejectionE3Graph = new TGraph(rejByCutValE3.size(),&(*rejByCutValE3.begin()),&(*effByCutValE3BetaSlices[4].begin()));
  efficiencyVsRejectionE3Graph->GetXaxis()->SetTitle("-log(background efficiency)");
  efficiencyVsRejectionE3Graph->GetYaxis()->SetTitle("signal efficiency");
  efficiencyVsRejectionE3Graph->SetTitle("estimator 3");
  efficiencyVsRejectionE3Graph->Draw("ap*");
  tleg = new TLegend(0.2,0.2,0.4,0.4);
  tleg->AddEntry(efficiencyVsRejectionD1Graph,"#beta > 0.8","p");
  temp = new TGraph(rejByCutValE3.size(),&(*rejByCutValE3.begin()),&(*effByCutValE3BetaSlices[3].begin()));
  temp->SetMarkerColor(2);
  temp->Draw("p*,same");
  tleg->AddEntry(temp,"0.8 > #beta > 0.6","p");
  temp2 = new TGraph(rejByCutValE3.size(),&(*rejByCutValE3.begin()),&(*effByCutValE3BetaSlices[2].begin()));
  temp2->SetMarkerColor(3);
  temp2->Draw("p*,same");
  tleg->AddEntry(temp2,"0.6 > #beta > 0.4","p");
  temp3 = new TGraph(rejByCutValE3.size(),&(*rejByCutValE3.begin()),&(*effByCutValE3BetaSlices[1].begin()));
  temp3->SetMarkerColor(4);
  temp3->Draw("p*,same");
  tleg->AddEntry(temp3,"0.4 > #beta > 0.2","p");
  tleg->Draw("same");
  t6.SetName("efficiencyVsRejectionE3");
  t6.SetTitle("efficiencyVsRejectionE3");
  t6.Write();
  t6.Print("effVsRejEst3.png");

  // Do eff vs. rejection, all D's, each beta slice
  TCanvas betaSliceDCanvases[5];
  for(int i=0; i < 5; ++i)
  {
    betaSliceDCanvases[i].cd();
    string name = "effVsRejDiscsBeta";
    float betaMin = i/5.0;
    float betaMax = betaMin+0.2;
    name+=floatToString(betaMin);
    name+="to";
    name+=floatToString(betaMax);
    TGraph* effVsRejDBetaSliceGraph = new TGraph(rejByCutValD1.size(),&(*rejByCutValD1.begin()),&(*effByCutValD1BetaSlices[i].begin()));
    effVsRejDBetaSliceGraph->GetXaxis()->SetTitle("-log(background efficiency)");
    effVsRejDBetaSliceGraph->GetYaxis()->SetTitle("signal efficiency");
    effVsRejDBetaSliceGraph->SetTitle(name.c_str());
    effVsRejDBetaSliceGraph->Draw("ap*");
    tleg = new TLegend(0.2,0.2,0.4,0.4);
    tleg->AddEntry(effVsRejDBetaSliceGraph,"Disc1","p");
    temp = new TGraph(rejByCutValD2.size(),&(*rejByCutValD2.begin()),&(*effByCutValD2BetaSlices[i].begin()));
    temp->SetMarkerColor(2);
    temp->Draw("p*,same");
    tleg->AddEntry(temp,"Disc2","p");
    temp2 = new TGraph(rejByCutValD3.size(),&(*rejByCutValD3.begin()),&(*effByCutValD3BetaSlices[i].begin()));
    temp2->SetMarkerColor(3);
    temp2->Draw("p*,same");
    tleg->AddEntry(temp2,"Disc3","p");
    tleg->Draw("same");
    betaSliceDCanvases[i].SetName(name.c_str());
    betaSliceDCanvases[i].SetTitle(name.c_str());
    betaSliceDCanvases[i].Write();
    string pngName = name;
    pngName+=".png";
    string cName = "effVsRejDiscsBetaSlice";
    cName+=intToString(i);
    betaSliceDCanvases[i].Print(pngName.c_str());
    betaSliceDCanvases[i].SetName(cName.c_str());
    cName+=".C";
    betaSliceDCanvases[i].Print(cName.c_str());
  }
  // Do eff vs. rejection, all E's, each beta slice
  TCanvas betaSliceECanvases[5];
  for(int i=0; i < 5; ++i)
  {
    betaSliceECanvases[i].cd();
    string name = "effVsRejEstsBeta";
    float betaMin = i/5.0;
    float betaMax = betaMin+0.2;
    name+=floatToString(betaMin);
    name+="to";
    name+=floatToString(betaMax);
    ////debug
    //cout << "Entries in rejByCutValE1 for beta slice " << i << ": " << endl;;
    //for(unsigned int j=0; j < rejByCutValE1.size(); ++j)
    //{
    //  cout << "\t" << rejByCutValE1[j] << endl;
    //}
    //cout << "Entries in effByCutValE1 for beta slice " << i << ": " << endl;;
    //for(unsigned int j=0; j < effByCutValE1BetaSlices[i].size(); ++j)
    //{
    //  cout << "\t" << effByCutValE1BetaSlices[i][j] << endl;
    //}
    //cout << "Entries in effByCutValE2 for beta slice " << i << ": " << endl;;
    //for(unsigned int j=0; j < effByCutValE2BetaSlices[i].size(); ++j)
    //{
    //  cout << "\t" << effByCutValE2BetaSlices[i][j] << endl;
    //}
    //cout << "Entries in effByCutValE3 for beta slice " << i << ": " << endl;;
    //for(unsigned int j=0; j < effByCutValE3BetaSlices[i].size(); ++j)
    //{
    //  cout << "\t" << effByCutValE3BetaSlices[i][j] << endl;
    //}
    //// end debug
    TGraph* effVsRejEBetaSliceGraph = new TGraph(rejByCutValE1.size(),&(*rejByCutValE1.begin()),&(*effByCutValE1BetaSlices[i].begin()));
    effVsRejEBetaSliceGraph->GetXaxis()->SetTitle("-log(background efficiency)");
    effVsRejEBetaSliceGraph->GetYaxis()->SetTitle("signal efficiency");
    effVsRejEBetaSliceGraph->SetTitle(name.c_str());
    effVsRejEBetaSliceGraph->Draw("ap*");
    tleg = new TLegend(0.15,0.2,0.35,0.4);
    tleg->AddEntry(effVsRejEBetaSliceGraph,"Est1","p");
    temp = new TGraph(rejByCutValE2.size(),&(*rejByCutValE2.begin()),&(*effByCutValE2BetaSlices[i].begin()));
    temp->SetMarkerColor(2);
    temp->Draw("p*,same");
    tleg->AddEntry(temp,"Est2","p");
    temp2 = new TGraph(rejByCutValE3.size(),&(*rejByCutValE3.begin()),&(*effByCutValE3BetaSlices[i].begin()));
    temp2->SetMarkerColor(3);
    temp2->Draw("p*,same");
    tleg->AddEntry(temp2,"Est3","p");
    tleg->Draw("same");
    betaSliceECanvases[i].SetName(name.c_str());
    betaSliceECanvases[i].SetTitle(name.c_str());
    betaSliceECanvases[i].Write();
    string pngName = name;
    pngName+=".png";
    string cName = "effVsRejEstsBetaSlice";
    cName+=intToString(i);
    betaSliceECanvases[i].Print(pngName.c_str());
    betaSliceECanvases[i].SetName(cName.c_str());
    cName+=".C";
    betaSliceECanvases[i].Print(cName.c_str());
  }
}
