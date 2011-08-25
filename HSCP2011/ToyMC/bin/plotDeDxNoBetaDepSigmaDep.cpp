#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TF1.h"
#include "TProfile.h"
#include "Math/WrappedMultiTF1.h"
#include "Fit/Fitter.h"
#include "Fit/BinData.h"
#include "Fit/Chi2FCN.h"
#include "HFitInterface.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
//#include <map>
#include <iomanip>


std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}


// ********* MAIN
int main(int argc, char** argv)
{
  using namespace std;

  char* inputFile = argv[1];
  if(!inputFile)
  {
    cout << "Error: Missing input file." << endl;
    return -1;
  }

  gROOT->Macro("../rootstyle.C");

  TFile* inputTFile = new TFile(inputFile);
  inputTFile->cd();

  TFile* outputTFile = new TFile("plotDeDxNoBetaDepSigmaDep.root","recreate");

  int nPBins = 200;
  const int nNomBins = 30;
  const int nEtaBins = 25;

  TH1F* dedxNoBetaDepHists[nEtaBins][nNomBins];
  TGraphErrors* meanVsNoMGraphsEtaSlices[nEtaBins];
  TGraphErrors* sigmaVsNoMGraphsEtaSlices[nEtaBins];

  // fit func for NoM dependence
  TF1* myNomDepFunc = new TF1("myNomDepFunc","[0]/TMath::Sqrt(x/[1])",5,25);
  // fit func for gaussians
  TF1* myGaus = new TF1("myGaus","gaus(0)",-5,5);
  TH3F* rawHist;

  string dir = "dedxHistsInNomBinsMatched";
  // loop over nom bins
  for(int nomBin=1; nomBin < nNomBins+1; ++nomBin)
  {
    inputTFile->cd();
    string name = "dedxE1NoPBiasMatchedNomBin";
    name+=intToString(nomBin);
    string fullpath = dir;
    fullpath+="/";
    fullpath+=name;
    rawHist = (TH3F*) inputTFile->Get(fullpath.c_str());

    // loop over eta bins
    for(int etaBin=1; etaBin<nEtaBins+1; ++etaBin)
    {
      rawHist->GetZaxis()->SetRange(etaBin,etaBin);
      // set beta/p range (limit saturation effects)
      int minPbin = 135; // higher p (beta) only
      if(etaBin < 6)
        minPbin = 100;
      rawHist->GetYaxis()->SetRange(minPbin,nPBins);

      string projName = "x";
      projName+="_EtaBin";
      projName+=intToString(etaBin);
      dedxNoBetaDepHists[etaBin-1][nomBin-1] = (TH1F*)rawHist->Project3D(projName.c_str());
      string histName = "dedxE1NoPBiasMatchedEtaBin";
      histName+=intToString(etaBin);
      histName+="NomBin";
      histName+=intToString(nomBin);
      dedxNoBetaDepHists[etaBin-1][nomBin-1]->SetName(histName.c_str());
    }
  }

  double sigmaChi2OverNdfs[25];

  cout << setiosflags(ios::left);
  cout << setw(8) << "etaBin" << setw(12) << "chi2" << setw(6) << "Ndf" << setw(6) << "chi2/Ndf" << endl;
  // loop over the newly-stored hists -- eta bins first
  for(int etaBin=1; etaBin<nEtaBins+1; ++etaBin)
  {
    vector<double> means;
    vector<double> sigmas;
    vector<double> meanEs;
    vector<double> sigmaEs;
    vector<double> noms;
    vector<double> nomEs;

    // make output dir
    outputTFile->cd();
    string name2 = "dedxE1NoPBiasMatchedInEtaBin";
    name2+=intToString(etaBin);
    TDirectory* dedxE1MatchedEtaBinDir = outputTFile->mkdir(name2.c_str());
    dedxE1MatchedEtaBinDir->cd();

    // loop over nom bins, fit make graphs
    for(int nomBin=1; nomBin < nNomBins+1; ++nomBin)
    {
      if(dedxNoBetaDepHists[etaBin-1][nomBin-1]->GetEntries() > 25)
      {
        //myGaus->SetParameter(0,500);
        myGaus->SetParameter(1,0);
        myGaus->SetParameter(2,0.5);
        //XXX limit sigma to be positive?
        myGaus->SetParLimits(2,0,1);
        TFitResultPtr r = dedxNoBetaDepHists[etaBin-1][nomBin-1]->Fit("myGaus","SQR");
        int ret = r;
        if(ret==0)
        {
          means.push_back(r->Parameter(1));
          sigmas.push_back(r->Parameter(2));
          meanEs.push_back(r->ParError(1));
          sigmaEs.push_back(r->ParError(2));
          noms.push_back(nomBin);
          nomEs.push_back(0);
        }
        dedxNoBetaDepHists[etaBin-1][nomBin-1]->GetXaxis()->SetTitle("Ih_meas-Ih_fit+res [MeV/cm]");
        dedxNoBetaDepHists[etaBin-1][nomBin-1]->Write();
      }
    }
    // Make graphs, fit, write
    string meanGraphName="meanVsNoMEtaSlice";
    meanGraphName+=intToString(etaBin);
    meanVsNoMGraphsEtaSlices[etaBin-1] = new TGraphErrors(noms.size(),&(*noms.begin()),&(*means.begin()),&(*nomEs.begin()),&(*meanEs.begin()));
    meanVsNoMGraphsEtaSlices[etaBin-1]->SetName(meanGraphName.c_str());
    meanVsNoMGraphsEtaSlices[etaBin-1]->SetTitle(meanGraphName.c_str());
    meanVsNoMGraphsEtaSlices[etaBin-1]->GetXaxis()->SetTitle("NoM");
    meanVsNoMGraphsEtaSlices[etaBin-1]->GetYaxis()->SetTitle("Ih_meas-Ih_fit+res [MeV/cm]");
    meanVsNoMGraphsEtaSlices[etaBin-1]->Write();
    TCanvas tMean;
    tMean.cd();
    meanVsNoMGraphsEtaSlices[etaBin-1]->GetYaxis()->SetRangeUser(-0.75,0.75);
    meanVsNoMGraphsEtaSlices[etaBin-1]->Draw("ap");
    string canName = "meanIhMeasMinusIhFitPlusResVsNoMfit_etaBin";
    canName+=intToString(etaBin);
    tMean.Print((canName+".png").c_str());

    string sigmaGraphName="sigmaVsNoMEtaSlice";
    sigmaGraphName+=intToString(etaBin);
    sigmaVsNoMGraphsEtaSlices[etaBin-1] = new TGraphErrors(noms.size(),&(*noms.begin()),&(*sigmas.begin()),&(*nomEs.begin()),&(*sigmaEs.begin()));
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->SetName(sigmaGraphName.c_str());
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->SetTitle(sigmaGraphName.c_str());
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->GetXaxis()->SetTitle("NoM");
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->GetYaxis()->SetTitle("#sigma(Ih_meas-Ih_fit+res) [MeV/cm]");

    unsigned int nomNormIndex = 0;
    int nomToNormalizeTo = 10;
    if(etaBin>=15)
      nomToNormalizeTo = 12;
    while(nomNormIndex<noms.size() && noms[nomNormIndex] != nomToNormalizeTo)
      ++nomNormIndex;
    if(nomNormIndex>=noms.size())
    {
      cout << "ERROR: couldn't find nom==10 in this eta slice: " << etaBin << endl;
      continue;
    }
    myNomDepFunc->SetParameter(0,sigmas[nomNormIndex]);
    myNomDepFunc->SetParameter(1,nomToNormalizeTo);

    myNomDepFunc->SetChisquare(sigmaVsNoMGraphsEtaSlices[etaBin-1]->Chisquare(myNomDepFunc));
    myNomDepFunc->SetNDF(sigmaVsNoMGraphsEtaSlices[etaBin-1]->GetN()-1);
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->GetListOfFunctions()->Add(myNomDepFunc);
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->Write();

    TCanvas tSig;
    tSig.cd();
    sigmaVsNoMGraphsEtaSlices[etaBin-1]->Draw("ap");
    myNomDepFunc->Draw("same");
    canName = "sigmaIhMeasMinusIhFitPlusResVsNoMfit_etaBin";
    canName+=intToString(etaBin);
    tSig.Print((canName+".png").c_str());
    sigmaChi2OverNdfs[etaBin-1] = myNomDepFunc->GetChisquare()/myNomDepFunc->GetNDF();
    cout << setw(8) << etaBin << setw(12) << sigmaVsNoMGraphsEtaSlices[etaBin-1]->Chisquare(myNomDepFunc) << setw(6) << sigmaVsNoMGraphsEtaSlices[etaBin-1]->GetN()-1;
    cout << setw(6) << sigmaChi2OverNdfs[etaBin-1] << endl;

  }



  outputTFile->cd();

}

