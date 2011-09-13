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
  std::cout << "Usage: getNumTracksInPSlices <fileWithReadHscpTreeTkPlots>" << std::endl;
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
  std::cout << "Calculating number of tracks passing selections with dE/dx > 3 MeV/cm in P slices" << std::endl;
  std::cout << std::endl;

  double numTracks[8];
  for(int i=0; i<8;++i)
    numTracks[i] = 0;

  TFile* myRootFile = new TFile(infile);
  // Use matched hists for HSCP
  TH1F* pSlice5Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice5");
  pSlice5Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[0] = pSlice5Hist->Integral();

  TH1F* pSlice6Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice6");
  pSlice6Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[1] = pSlice6Hist->Integral();

  TH1F* pSlice7Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice7");
  pSlice7Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[2] = pSlice7Hist->Integral();

  TH1F* pSlice8Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice8");
  pSlice8Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[3] = pSlice8Hist->Integral();

  TH1F* pSlice9Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice9");
  pSlice9Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[4] = pSlice9Hist->Integral();

  TH1F* pSlice10Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice10");
  pSlice10Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[5] = pSlice10Hist->Integral();

  TH1F* pSlice11Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice11");
  pSlice11Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[6] = pSlice11Hist->Integral();

  TH1F* pSlice12Hist = (TH1F*) myRootFile->Get("TkDeDxMatchedEstPSlices/trackDeDxMatchedE1PSlice12");
  pSlice12Hist->GetXaxis()->SetRangeUser(3,25);
  numTracks[7] = pSlice12Hist->Integral();

  std::cout << "MATCHED HISTS:" << std::endl;
  std::cout << "P:\t100-125\t125-150\t150-175\t175-200\t200-300\t300-400\t400-500\t>500" << std::endl;
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
  TH1F* pSlice5UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice5");
  pSlice5UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[0] = pSlice5UnmatchedHist->Integral();

  TH1F* pSlice6UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice6");
  pSlice6UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[1] = pSlice6UnmatchedHist->Integral();

  TH1F* pSlice7UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice7");
  pSlice7UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[2] = pSlice7UnmatchedHist->Integral();

  TH1F* pSlice8UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice8");
  pSlice8UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[3] = pSlice8UnmatchedHist->Integral();

  TH1F* pSlice9UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice9");
  pSlice9UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[4] = pSlice9UnmatchedHist->Integral();

  TH1F* pSlice10UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice10");
  pSlice10UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[5] = pSlice10UnmatchedHist->Integral();

  TH1F* pSlice11UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice11");
  pSlice11UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[6] = pSlice11UnmatchedHist->Integral();

  TH1F* pSlice12UnmatchedHist = (TH1F*) myRootFile->Get("TkDeDxEstDiscPSlices/trackDeDxE1PSlice12");
  pSlice12UnmatchedHist->GetXaxis()->SetRangeUser(3,25);
  numTracksUnmatched[7] = pSlice12UnmatchedHist->Integral();

  std::cout << "UNMATCHED HISTS:" << std::endl;
  std::cout << "P:\t100-125\t125-150\t150-175\t175-200\t200-300\t300-400\t400-500\t>500" << std::endl;
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

