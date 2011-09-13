#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLegend.h"
#include "TPaveStats.h"

#include <iostream>

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
  EStyle->SetHistLineWidth(2);
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
  EStyle->SetStatX(0.95);
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

// Usage
void usage()
{
  std::cout << "Usage: makeNLLFromToyMC <fileWithSignalAndBackgroundGenSamples> <fileWithBackgroundOnlyGenSamples"
    << std::endl;
}

// Main
int main(int argc, char* argv[])
{

  char* infileSigBack = argv[1];
  if (!infileSigBack)
  {
    std::cout << " No input file (signal+back) specified !" << std::endl;
    usage();
    return -1;
  }
  char* infileBack = argv[2];
  if (!infileBack)
  {
    std::cout << " No input file (back only) specified !" << std::endl;
    usage();
    return -1;
  }

  SetEStyle();

  TFile* mySBRootFile = new TFile(infileSigBack);
  TH1F* nllHistSB = (TH1F*) mySBRootFile->Get("nllSBHist");
  TH1F* nllSignalHistSB = (TH1F*) mySBRootFile->Get("nllSignalHist");
  TH1F* nllBackHistSB = (TH1F*) mySBRootFile->Get("nllBackHist");
  nllHistSB->SetName("nllHistSB");
  nllSignalHistSB->SetName("nllSignalHistSB");
  nllBackHistSB->SetName("nllBackHistSB");

  TFile* myBRootFile = new TFile(infileBack);
  TH1F* nllHistB = (TH1F*) myBRootFile->Get("nllSBHist");
  TH1F* nllSignalHistB = (TH1F*) myBRootFile->Get("nllSignalHist");
  TH1F* nllBackHistB = (TH1F*) myBRootFile->Get("nllBackHist");
  nllHistB->SetName("nllHistB");
  nllSignalHistB->SetName("nllSignalHistB");
  nllBackHistB->SetName("nllBackHistB");


  TCanvas t;
  t.cd();

  nllBackHistB->Rebin();
  nllBackHistB->Rebin();
  nllBackHistB->Rebin();
  nllBackHistB->Rebin();
  nllBackHistB->GetXaxis()->SetRangeUser(0,2000);
  nllBackHistB->Draw();

  //gStyle->SetStatW(0.15);
  //gStyle->SetStatH(0.25);
  //gStyle->SetStatX(0.98);
  nllBackHistSB->SetLineColor(2);
  nllBackHistSB->SetTitle("NLL");
  nllBackHistSB->Rebin();
  nllBackHistSB->Rebin();
  nllBackHistSB->Rebin();
  nllBackHistSB->Rebin();
  nllBackHistSB->GetYaxis()->SetRangeUser(0,1.2*nllBackHistB->GetMaximum());
  nllBackHistSB->GetXaxis()->SetRangeUser(0,2000);
  nllBackHistSB->Draw();
  //gPad->Update();
  //TPaveStats *stats = (TPaveStats*)nllBackHistSB->GetListOfFunctions()->FindObject("stats");
  //stats->Draw("same");

  //gStyle->SetStatX(0.825);
  //nllBackHistB->Draw("sames");
  nllBackHistB->Draw("same");

  TLegend* leg = new TLegend(0.4,0.8,0.6,0.9);
  leg->AddEntry("nllBackHistSB","NLL:BG PDF,sig+bg evts", "l");
  leg->AddEntry("nllBackHistB","NLL:BG PDF,bg evts", "l");
  //leg->AddEntry("nllBackHistSB","NLL:BG PDF,ToF sideband", "l");
  //leg->AddEntry("nllBackHistB","NLL:BG PDF,Low P sideband", "l");
  leg->SetBorderSize(0);
  leg->Draw("same");

  t.Print("makeNLLFromToyMC.output.png");
  t.Print("makeNLLFromToyMC.output.C");


}
