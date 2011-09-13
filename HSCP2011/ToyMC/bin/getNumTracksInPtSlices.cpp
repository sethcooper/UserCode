#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TStyle.h"
#include "TColor.h"
#include "TLegend.h"
#include "TPaveStats.h"

#include <iostream>

// Usage
void usage()
{
  std::cout << "Usage: getNumTracksInPtSlices <fileWithReadHscpTreeTkPlots>" << std::endl;
}

// Main
int main(int argc, char* argv[])
{

  char* infile = argv[1];
  if (!infile)
  {
    std::cout << " No input file specified !" << std::endl;
    usage();
    return -1;
  }

  std::cout << std::endl << "Processing file: " << infile << std::endl;
  std::cout << "Calculating number of tracks passing selections with dE/dx > 3 MeV/cm in Pt slices" << std::endl;
  std::cout << std::endl;

  double numTracks[8];
  for(int i=0; i<8;++i)
    numTracks[i] = 0;

  TFile* myRootFile = new TFile(infile);
  // Use matched hists for HSCP
  TH1F* ptSlice5Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice5");
  ptSlice5Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[0] = ptSlice5Hist->Integral();

  TH1F* ptSlice6Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice6");
  ptSlice6Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[1] = ptSlice6Hist->Integral();

  TH1F* ptSlice7Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice7");
  ptSlice7Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[2] = ptSlice7Hist->Integral();

  TH1F* ptSlice8Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice8");
  ptSlice8Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[3] = ptSlice8Hist->Integral();

  TH1F* ptSlice9Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice9");
  ptSlice9Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[4] = ptSlice9Hist->Integral();

  TH1F* ptSlice10Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice10");
  ptSlice10Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[5] = ptSlice10Hist->Integral();

  TH1F* ptSlice11Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice11");
  ptSlice11Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[6] = ptSlice11Hist->Integral();

  TH1F* ptSlice12Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPtSlices/trackDeDxMatchedE1PtSlice12");
  ptSlice12Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[7] = ptSlice12Hist->Integral();

  std::cout << "Pt:\t100-125\t125-150\t150-175\t175-200\t200-300\t300-400\t400-500\t>500" << std::endl;
  double total = 0;
  for(int i=0; i<8; ++i)
  {
    std::cout << "\t";
    std::cout << numTracks[i];
    total+=numTracks[i];
  }
  std::cout << std::endl;

  std::cout << "Total: " << total << std::endl;

  double numTracksUnmatched[8];
  for(int i=0; i<8;++i)
    numTracksUnmatched[i] = 0;

  // Use unmatched hists for muon
  TH1F* ptSlice5UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice5");
  ptSlice5UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[0] = ptSlice5UnmatchedHist->Integral();

  TH1F* ptSlice6UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice6");
  ptSlice6UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[1] = ptSlice6UnmatchedHist->Integral();

  TH1F* ptSlice7UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice7");
  ptSlice7UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[2] = ptSlice7UnmatchedHist->Integral();

  TH1F* ptSlice8UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice8");
  ptSlice8UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[3] = ptSlice8UnmatchedHist->Integral();

  TH1F* ptSlice9UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice9");
  ptSlice9UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[4] = ptSlice9UnmatchedHist->Integral();

  TH1F* ptSlice10UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice10");
  ptSlice10UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[5] = ptSlice10UnmatchedHist->Integral();

  TH1F* ptSlice11UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice11");
  ptSlice11UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[6] = ptSlice11UnmatchedHist->Integral();

  TH1F* ptSlice12UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPtSlices/trackDeDxE1PtSlice12");
  ptSlice12UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[7] = ptSlice12UnmatchedHist->Integral();

  std::cout << "UNMATCHED HISTS:" << std::endl;
  std::cout << "Pt:\t100-125\t125-150\t150-175\t175-200\t200-300\t300-400\t400-500\t>500" << std::endl;
  double totalUnmatched = 0;
  for(int i=0; i<8; ++i)
  {
    std::cout << "\t";
    std::cout << numTracksUnmatched[i];
    totalUnmatched+=numTracksUnmatched[i];
  }
  std::cout << std::endl;

  std::cout << "Total: " << totalUnmatched << std::endl;
}

