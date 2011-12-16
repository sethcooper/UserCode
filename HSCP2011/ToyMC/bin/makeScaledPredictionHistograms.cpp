#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TPaveText.h" 
#include "TRandom3.h"
#include "TProfile.h"
#include "TDirectory.h"
#include "TArray.h"

//RooFit
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooHistPdf.h"
#include "RooPlot.h"
#include "RooBinning.h"
#include "RooAddPdf.h"
#include "RooMinuit.h"
#include "RooNLLVar.h"
#include "RooMsgService.h"

#include "TSystem.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <vector>
#include <fstream>

namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}

#if !defined(__CINT__) && !defined(__MAKECINT__)

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/PythonProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"

#endif

#include "commonFunctions.h"

// globals
int minNoM = 0;
int maxNoM = 0;
float minEta = 0;
float maxEta = 0;
int numIasBins = 0;

// helper functions
int getEtaSliceFromLowerEta(float lowerEta)
{
  return lowerEta/0.2;
}
//
int getNoMSliceFromLowerNoM(int lowerNoM)
{
  return (lowerNoM-5)/2;
}
//
int getGlobalBinFromNomSliceEtaSliceIasBin(int nomSlice, int etaSlice, int iasBin)
{
  // ias bin will be from 1-d hist, so it goes from 1-400

  //int numNoMBins = getNoMSliceFromLowerNoM(maxNoM) + 1;
  int numEtaBins = getEtaSliceFromLowerEta(maxEta);

  int multFactorEtaSlice = numIasBins;
  int multFactorNoMSlice = multFactorEtaSlice*numEtaBins;

  return multFactorNoMSlice*nomSlice + multFactorEtaSlice*etaSlice + iasBin;
}
//
int getGlobalBinMax()
{
  int numNoMBins = getNoMSliceFromLowerNoM(maxNoM) + 1;
  int numEtaBins = getEtaSliceFromLowerEta(maxEta);

  int multFactorEtaSlice = numIasBins;
  int multFactorNoMSlice = multFactorEtaSlice*numEtaBins;

  return multFactorNoMSlice*(numNoMBins-1) + multFactorEtaSlice*(numEtaBins-1) + numIasBins;
}



// ****** main
int main(int argc, char ** argv)
{
  using namespace RooFit;
  using namespace std;

  // load framework libraries
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  // parse arguments
  if(argc < 2)
  {
    cout << "Usage : " << argv[0] << " [parameters.py]" << endl;
    return -1;
  }

  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  // now get each parameterset
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("makeScaledPredictionHistograms");
  // mass cut to use for the high-p high-Ih (search region) ias dist
  double massCut_ (ana.getParameter<double>("MassCut"));
  string backgroundPredictionRootFilename_ (ana.getParameter<string>("BackgroundPredictionInputRootFile"));
  string signalRootFilename_ (ana.getParameter<string>("SignalInputRootFile"));
  string outputRootFilename_ (ana.getParameter<string>("OutputRootFile"));
  // dE/dx calibration
  double dEdx_k (ana.getParameter<double>("dEdx_k"));
  double dEdx_c (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  double integratedLumi (ana.getParameter<double>("IntegratedLuminosity")); // 1/pb
  //double signalCrossSection (ana.getParameter<double>("SignalCrossSection")); // pb

  // TODO configurable nom/eta limits
  // NB: always use upper edge of last bin for both
  minNoM = 5;
  maxNoM = 20;
  minEta = 0.0;
  maxEta = 1.6;


  // define observables
  //TODO move to something like common functions
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);
  //TODO add mass in initial dataset generation?

  TFile* outputRootFile = new TFile(outputRootFilename_.c_str(),"recreate");
  TFile* backgroundPredictionRootFile = TFile::Open(backgroundPredictionRootFilename_.c_str());
  TFile* signalRootFile = TFile::Open(signalRootFilename_.c_str());

  //ofstream xmlOutputFileStream("xmlFragment.xml");
  //if(!xmlOutputFileStream.is_open())
  //{
  //  cout << "Unable to open output file" << endl;
  //  return -2;
  //}


  //TDirectory* scaledBGPredDir = outputRootFile->mkdir("scaledBackgroundPredictions");
  //TDirectory* normedSignalPredDir = outputRootFile->mkdir("normalizedSignalPredictions");
  // get roodataset from signal file
  RooDataSet* rooDataSetAllSignal = (RooDataSet*)signalRootFile->Get("rooDataSetAll");
  if(!rooDataSetAllSignal)
  {
    cout << "Problem with RooDataSet named rooDataSetAll in signal file " <<
      signalRootFilename_.c_str() << endl;
    return -3;
  }

  int numSignalTracksTotal = rooDataSetAllSignal->numEntries();
  // construct D region for signal
  string pSearchCutString = "rooVarP>";
  pSearchCutString+=floatToString(pSidebandThreshold);
  string ihSearchCutString = "rooVarIh>";
  ihSearchCutString+=floatToString(ihSidebandThreshold);
  RooDataSet* regionD1DataSetSignal = (RooDataSet*)rooDataSetAllSignal->reduce(Cut(pSearchCutString.c_str()));
  RooDataSet* regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->reduce(Cut(ihSearchCutString.c_str()));
  //int numSignalTracksInDRegion = regionDDataSetSignal->numEntries();

  // get the background A region entries hist
  string fullPath = "entriesInARegion";
  TH2F* aRegionBackgroundEntriesHist = (TH2F*)backgroundPredictionRootFile->Get(fullPath.c_str());
  if(!aRegionBackgroundEntriesHist)
  {
    cout << "Cannot proceed further: no A region entries hist found (histogram " <<
      fullPath << ") in file: " << backgroundPredictionRootFilename_ << ")." << endl;
    return -3;
  }
  // get the background B region hist
  fullPath = "entriesInBRegion";
  TH2F* bRegionBackgroundEntriesHist = (TH2F*)backgroundPredictionRootFile->Get(fullPath.c_str());
  if(!bRegionBackgroundEntriesHist)
  {
    cout << "Cannot proceed further: no B region entries hist found (histogram " <<
      fullPath << ") in file: " << backgroundPredictionRootFilename_ << ")." << endl;
    return -3;
  }
  // get the background C region hist
  fullPath = "entriesInCRegion";
  TH2F* cRegionBackgroundEntriesHist = (TH2F*)backgroundPredictionRootFile->Get(fullPath.c_str());
  if(!cRegionBackgroundEntriesHist)
  {
    cout << "Cannot proceed further: no C region entries hist found (histogram " <<
      fullPath << ") in file: " << backgroundPredictionRootFilename_ << ")." << endl;
    return -3;
  }


  TH1F* iasBackgroundPredictionMassCutNoMSliceHist = 0;
  TH1F* iasSignalMassCutNoMSliceHist = 0;
  vector<TH1F> bgHistsToUse;

  // for histograms already made
  //string dirName="iasPredictionsVariableBins";
  //string getHistName+="iasPredictionVarBinHist";
  string dirName="iasPredictionsFixedBins";
  string bgHistNameEnd="iasPredictionFixedHist";

  // look at BG predictions to determine unrolled hist binning
  for(int lowerNoM = minNoM; lowerNoM < maxNoM; lowerNoM+=2)
  {
    for(float lowerEta = minEta; lowerEta < maxEta; lowerEta+=0.2)
    {
      // get this eta/nom hist
      string getHistName = getHistNameBeg(lowerNoM,lowerEta);
      getHistName+=bgHistNameEnd;
      string fullPath = dirName;
      fullPath+="/";
      fullPath+=getHistName;

      iasBackgroundPredictionMassCutNoMSliceHist =
        (TH1F*)backgroundPredictionRootFile->Get(fullPath.c_str());

      // do the rest if the hist is found and integral > 0
      if(iasBackgroundPredictionMassCutNoMSliceHist
          && iasBackgroundPredictionMassCutNoMSliceHist->Integral() > 0)
      {
        // set ias bins or check hist consistency
        if(numIasBins < 1)
          numIasBins = iasBackgroundPredictionMassCutNoMSliceHist->GetNbinsX();
        else if(numIasBins != iasBackgroundPredictionMassCutNoMSliceHist->GetNbinsX())
        {
          cout << "ERROR: A histogram earlier had : " << numIasBins << " x bins and this one ("
            << iasBackgroundPredictionMassCutNoMSliceHist->GetName() << ") has : "
            << iasBackgroundPredictionMassCutNoMSliceHist->GetNbinsX() << " x bins...exiting."
            << endl;
          return -10;
        }
        //// debug
        //cout << "Found hist: " << iasBackgroundPredictionMassCutNoMSliceHist->GetName()
        //  << " with integral = " << iasBackgroundPredictionMassCutNoMSliceHist->Integral()
        //  << " pushing onto the vector." << endl;
        bgHistsToUse.push_back(*iasBackgroundPredictionMassCutNoMSliceHist);
      }
      else
      {
        //cout << "WARNING: no input found for this slice (histogram " <<
        //  fullPath << ") in file: " << backgroundPredictionRootFilename_ << ")." << 
        //  " Skipping prediction. " << endl;
      }
    }
  }

  // initialize the global unrolled hists
  int numGlobalBins = numIasBins*bgHistsToUse.size();
  cout << "INFO: numIasBins = " << numIasBins << endl;
  cout << "INFO: numBGHistsToUse = " << bgHistsToUse.size() << endl;
  cout << "INFO: initializing unrolled histogram: bins=" << numGlobalBins << endl;
  TH1F* backgroundAllNoMAllEtaUnrolledHist = new TH1F("backgroundAllNoMAllEtaUnrolledHist",
      "unrolled background hist",numGlobalBins,1,numGlobalBins+1);
  TH1F* signalAllNoMAllEtaUnrolledHist = new TH1F("signalAllNoMAllEtaUnrolledHist",
      "unrolled signal hist",numGlobalBins,1,numGlobalBins+1);

  // loop over hists to use
  int iteratorPos = 0;
  for(vector<TH1F>::iterator histItr = bgHistsToUse.begin();
      histItr != bgHistsToUse.end(); ++histItr)
  {
    string bgHistName = string(histItr->GetName());
    string bgHistNameBeg = bgHistName.substr(0,bgHistName.size()-bgHistNameEnd.size());
    int lowerNoM = getLowerNoMFromHistName(bgHistName,bgHistNameEnd);
    float lowerEta = getLowerEtaFromHistName(bgHistName,bgHistNameEnd);
    string sigHistName = bgHistNameBeg;
    //sigHistName+="iasSignalVarBinHist";
    sigHistName+="iasSignalFixedHist";
    string iasSignalHistTitle = "Ias of signal";
    iasSignalHistTitle+=getHistTitleEnd(lowerNoM,lowerEta,massCut_);

    // debug
    //cout << " Looking at hist: " << bgHistName << " lowerNoM = " << lowerNoM
    //  << " lowerEta = " << lowerEta << endl;

    // copy binning and limits from background pred hist
    iasSignalMassCutNoMSliceHist =
      (TH1F*)histItr->Clone();
    iasSignalMassCutNoMSliceHist->Reset();
    iasSignalMassCutNoMSliceHist->SetName(sigHistName.c_str());
    iasSignalMassCutNoMSliceHist->SetTitle(iasSignalHistTitle.c_str());


    // construct D region for signal in this eta/nom slice
    string nomCutString = "rooVarNoMias==";
    nomCutString+=intToString(lowerNoM);
    nomCutString+="||rooVarNoMias==";
    nomCutString+=intToString(lowerNoM+1);
    RooDataSet* nomCutDRegionDataSetSignal =
      (RooDataSet*)regionDDataSetSignal->reduce(Cut(nomCutString.c_str()));
    string etaCutString = "rooVarEta>";
    etaCutString+=floatToString(lowerEta);
    etaCutString+="&&rooVarEta<";
    etaCutString+=floatToString(lowerEta+0.2);
    RooDataSet* etaCutNomCutDRegionDataSetSignal =
      (RooDataSet*)nomCutDRegionDataSetSignal->reduce(Cut(etaCutString.c_str()));
    //int numSignalTracksInDRegionThisSlice = etaCutNomCutDRegionDataSetSignal->numEntries();

    // construct signal prediction hist (with mass cut)
    const RooArgSet* argSet = etaCutNomCutDRegionDataSetSignal->get();
    RooRealVar* iasData = (RooRealVar*)argSet->find(rooVarIas.GetName());
    RooRealVar* ihData = (RooRealVar*)argSet->find(rooVarIh.GetName());
    RooRealVar* pData = (RooRealVar*)argSet->find(rooVarP.GetName());
    for(int evt=0; evt < etaCutNomCutDRegionDataSetSignal->numEntries(); ++evt)
    {
      etaCutNomCutDRegionDataSetSignal->get(evt);
      // apply mass cut
      float massSqr = (ihData->getVal()-dEdx_c)*pow(pData->getVal(),2)/dEdx_k;
      if(massSqr < 0)
        continue;
      else if(sqrt(massSqr) >= massCut_)
        iasSignalMassCutNoMSliceHist->Fill(iasData->getVal());
    }
    double numSignalTracksInDRegionMassCutThisSlice = iasSignalMassCutNoMSliceHist->Integral();
    // adjust for bin width ratio if variable bins used
    for(int bin=1; bin<=iasSignalMassCutNoMSliceHist->GetNbinsX(); ++bin)
    {
      double binWidthRatio =
        iasSignalMassCutNoMSliceHist->GetBinWidth(1)/
        iasSignalMassCutNoMSliceHist->GetBinWidth(bin);
      double binc = iasSignalMassCutNoMSliceHist->GetBinContent(bin);
      double bine = iasSignalMassCutNoMSliceHist->GetBinError(bin);
      iasSignalMassCutNoMSliceHist->SetBinContent(bin,binWidthRatio*binc);
      iasSignalMassCutNoMSliceHist->SetBinError(bin,binWidthRatio*bine);
    }

    // figure out overall normalization this slice, background from ABCD
    double bgEntriesInARegionThisSlice =
      aRegionBackgroundEntriesHist->GetBinContent(aRegionBackgroundEntriesHist->FindBin(lowerEta,lowerNoM));
    double bgEntriesInBRegionThisSlice = 
      bRegionBackgroundEntriesHist->GetBinContent(bRegionBackgroundEntriesHist->FindBin(lowerEta,lowerNoM));
    double bgEntriesInCRegionThisSlice = 
      cRegionBackgroundEntriesHist->GetBinContent(cRegionBackgroundEntriesHist->FindBin(lowerEta,lowerNoM));
    double fractionOfBGTracksPassingMassCutThisSlice =
      histItr->Integral()/bgEntriesInBRegionThisSlice;
    double bgTracksInDThisSlice =
      bgEntriesInBRegionThisSlice*bgEntriesInCRegionThisSlice/bgEntriesInARegionThisSlice;
    double numBackgroundTracksInDRegionPassingMassCutThisSlice =
      bgTracksInDThisSlice*fractionOfBGTracksPassingMassCutThisSlice;

    // figure out overall normalization for signal this slice
    // must normalize as if sigma were 1 to measure the cross section
    double signalCrossSection = 1;
    double totalSignalTracksThisSlice = integratedLumi*signalCrossSection;
    double fractionOfSigTracksInDRegionPassingMassCutThisSlice =
      numSignalTracksInDRegionMassCutThisSlice/(double)numSignalTracksTotal;
    double numSignalTracksInDRegionPassingMassCutThisSlice =
      fractionOfSigTracksInDRegionPassingMassCutThisSlice*totalSignalTracksThisSlice;

    // output
    //cout << "Slice: lowerNoM = " << lowerNoM << " lowerEta = " << lowerEta << endl;
    //cout << "bg entries this slice: A = " << bgEntriesInARegionThisSlice << " B = " << 
    //  bgEntriesInBRegionThisSlice << " C = " << bgEntriesInCRegionThisSlice << " ==> D = " <<
    //  bgTracksInDThisSlice << " and fraction of bg tracks passing mass cut this slice = " <<
    //  fractionOfBGTracksPassingMassCutThisSlice << " ==> norm factor = " <<
    //  numBackgroundTracksInDRegionPassingMassCutThisSlice << endl;

    // normalize BG hist
    histItr->Sumw2();
    double bgNormFactor =
      numBackgroundTracksInDRegionPassingMassCutThisSlice/
      histItr->Integral();
    histItr->Scale(bgNormFactor);
    // normalize sig hist
    iasSignalMassCutNoMSliceHist->Sumw2();
    double sigNormFactor = numSignalTracksInDRegionPassingMassCutThisSlice/
      iasSignalMassCutNoMSliceHist->Integral();
    iasSignalMassCutNoMSliceHist->Scale(sigNormFactor);

    //int thisNoMSlice = getNoMSliceFromLowerNoM(lowerNoM);
    //int thisEtaSlice = getEtaSliceFromLowerEta(lowerEta);
    // loop over ias bins, fill unrolled hists
    for(int iasBin=1; iasBin < numIasBins+1; ++iasBin)
    {
      int globalBinIndex = iteratorPos*numIasBins+iasBin;
      double binc = histItr->GetBinContent(iasBin);
      double bine = histItr->GetBinError(iasBin);
      backgroundAllNoMAllEtaUnrolledHist->SetBinContent(globalBinIndex,binc);
      backgroundAllNoMAllEtaUnrolledHist->SetBinError(globalBinIndex,bine);

      binc = iasSignalMassCutNoMSliceHist->GetBinContent(iasBin);
      bine = iasSignalMassCutNoMSliceHist->GetBinError(iasBin);
      signalAllNoMAllEtaUnrolledHist->SetBinContent(globalBinIndex,binc);
      signalAllNoMAllEtaUnrolledHist->SetBinError(globalBinIndex,bine);
    }

    // write the histograms
    outputRootFile->cd();
    //scaledBGPredDir->cd();
    histItr->Write();
    //normedSignalPredDir->cd();
    iasSignalMassCutNoMSliceHist->Write();

    // write xml fragment
    //string sampleName = "background_nom";
    //sampleName+=intToString(lowerNoM);
    //sampleName+="_eta";
    //sampleName+=floatToString(10*lowerEta);
    //xmlOutputFileStream <<      
    //  "<Sample Name=\"" << sampleName << "\" HistoPath=\"\" NormalizeByTheory=\"False\" HistoName=\""
    //  << histItr->GetName() << "\">"
    //  << endl << 
    //  "</Sample>" << endl;


    iteratorPos++;
    // cleanup
    delete nomCutDRegionDataSetSignal;
    delete etaCutNomCutDRegionDataSetSignal;
  }

  //xmlOutputFileStream.close();
  outputRootFile->cd();
  backgroundAllNoMAllEtaUnrolledHist->Write();
  signalAllNoMAllEtaUnrolledHist->Write();
  outputRootFile->Close();

}

