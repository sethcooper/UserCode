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
#include <set>

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

struct EventInfo
{
  double runNumber;
  double eventNumber;
  double lumiSection;

  EventInfo()
  {
    runNumber = 0;
    eventNumber = 0;
    lumiSection = 0;
  }

  EventInfo(double runNum, double lumiNum, double eventNum) :
    runNumber(runNum),  eventNumber(eventNum), lumiSection(lumiNum)
  {
  }

  bool operator==(const EventInfo& comp) const
  {
    return (comp.runNumber==runNumber) && (comp.eventNumber==eventNumber)
      && (comp.lumiSection==lumiSection);
  }

  bool operator<(const EventInfo& comp) const
  {
    // run number < ; run numbers equal but lumi < ; run, lumis equal but event <
    return (runNumber<comp.runNumber) ||
      (runNumber==comp.runNumber && lumiSection < comp.lumiSection) ||
      (runNumber==comp.runNumber && lumiSection==comp.lumiSection && eventNumber < comp.eventNumber);
  }
};

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
// add the mass, p/pt, and ih SB thresholds to the output filename automatically
std::string generateFileNameEnd(double massCut, double pSideband, double ptSideband, bool usePt, double ihSideband,
    double iasSideband, bool useIas)
{
  std::string fileNameEnd = ".massCut";
  fileNameEnd+=intToString(((int)massCut));
  if(usePt)
  {
    fileNameEnd+=".pt";
    fileNameEnd+=intToString(((int)ptSideband));
  }
  else
  {
    fileNameEnd+=".p";
    fileNameEnd+=intToString(((int)pSideband));
  }
  if(useIas)
  {
    fileNameEnd+=".ias";
    // replace "." with "p" for the ih
    std::string iasSBStr = floatToString(iasSideband);
    iasSBStr.replace(iasSBStr.find("."),1,"p");
    fileNameEnd+=iasSBStr;
  }
  else
  {
    fileNameEnd+=".ih";
    // replace "." with "p" for the ih
    std::string ihSBStr = floatToString(ihSideband);
    ihSBStr.replace(ihSBStr.find("."),1,"p");
    fileNameEnd+=ihSBStr;
  }

  fileNameEnd+=".root";
  return fileNameEnd;
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
  double ptSidebandThreshold (ana.getParameter<double>("PtSidebandThreshold"));
  bool usePtForSideband (ana.getParameter<bool>("UsePtForSideband"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  double integratedLumi (ana.getParameter<double>("IntegratedLuminosity")); // 1/pb
  //double signalCrossSectionForEff (ana.getParameter<double>("SignalCrossSectionForEff")); // pb
  double iasCutForEffAcc (ana.getParameter<double>("IasCutForEfficiency"));
  bool useIasForSideband (ana.getParameter<bool>("UseIasForSideband"));

  std::cout << "Using dE/dx k: " << dEdx_k << " c: " << dEdx_c << std::endl;

  // TODO configurable nom/eta limits
  // NB: always use upper edge of last bin for both
  minNoM = 5;
  maxNoM = 22;
  minEta = 0.0;
  maxEta = 1.6;


  // define observables
  //TODO move to something like common functions
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);
  RooRealVar rooVarRun("rooVarRun","run",0,4294967295);
  RooRealVar rooVarLumiSection("rooVarLumiSection","lumiSection",0,4294967295);
  RooRealVar rooVarEvent("rooVarEvent","event",0,4294967295);
  //TODO add mass in initial dataset generation?
  RooRealVar rooVarNumGenHSCPEvents("rooVarNumGenHSCPEvents","numGenHSCPEvents",0,5e6);
  RooRealVar rooVarNumGenHSCPTracks("rooVarNumGenHSCPTracks","numGenHSCPTracks",0,5e6);
  RooRealVar rooVarNumGenChargedHSCPTracks("rooVarNumGenChargedHSCPTracks","numGenChargedHSCPTracks",0,5e6);
  RooRealVar rooVarSignalEventCrossSection("rooVarSignalEventCrossSection","signalEventCrossSection",0,100); // pb

  //string fileNameEnd = generateFileNameEnd(massCut_,pSidebandThreshold,ptSidebandThreshold,usePtForSideband,ihSidebandThreshold);
  string fileNameEnd = "";
  TFile* outputRootFile = new TFile((outputRootFilename_+fileNameEnd).c_str(),"recreate");
  TFile* backgroundPredictionRootFile = TFile::Open(backgroundPredictionRootFilename_.c_str());
  TFile* signalRootFile = TFile::Open(signalRootFilename_.c_str());

  //TDirectory* scaledBGPredDir = outputRootFile->mkdir("scaledBackgroundPredictions");
  //TDirectory* normedSignalPredDir = outputRootFile->mkdir("normalizedSignalPredictions");
  // get roodataset from signal file
  //RooDataSet* rooDataSetAllSignal = (RooDataSet*)signalRootFile->Get("rooDataSetCandidates");
  RooDataSet* rooDataSetAllSignal = (RooDataSet*)signalRootFile->Get("rooDataSetOneCandidatePerEvent");
  bool countEvents  = true;
  if(!rooDataSetAllSignal)
  {
    cout << "Problem with RooDataSet named rooDataSetCandidates in signal file " <<
      signalRootFilename_.c_str() << endl;
    return -3;
  }
  // get roodataset with gen information from signal file
  RooDataSet* rooDataSetGenSignal = (RooDataSet*)signalRootFile->Get("rooDataSetGenHSCPTracks");
  if(!rooDataSetGenSignal)
  {
    cout << "Problem with RooDataSet named rooDataSetGenSignal in signal file " <<
      signalRootFilename_.c_str() << endl;
    return -3;
  }

  int numSignalTracksTotal = rooDataSetAllSignal->numEntries();
  // construct D region for signal
  string pSearchCutString = "rooVarP>";
  pSearchCutString+=floatToString(pSidebandThreshold);
  string ptSearchCutString = "rooVarPt>";
  ptSearchCutString+=floatToString(ptSidebandThreshold);
  string ihSearchCutString = "rooVarIh>";
  ihSearchCutString+=floatToString(ihSidebandThreshold);
  string iasSearchCutString = "rooVarIas>";
  iasSearchCutString+=floatToString(iasCutForEffAcc);
  RooDataSet* regionD1DataSetSignal;
  RooDataSet* regionDDataSetSignal;
  //if(usePtForSideband)
  //  regionD1DataSetSignal = (RooDataSet*)rooDataSetAllSignal->reduce(Cut(ptSearchCutString.c_str()));
  //else
  //  regionD1DataSetSignal = (RooDataSet*)rooDataSetAllSignal->reduce(Cut(pSearchCutString.c_str()));
  // SIC MAR 5 -- don't do P cut for signal D region -- will be taken care of by mass cut later
  regionD1DataSetSignal = (RooDataSet*) rooDataSetAllSignal->Clone();
  if(useIasForSideband)
    regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->reduce(Cut(iasSearchCutString.c_str()));
  else
    regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->reduce(Cut(ihSearchCutString.c_str()));
  //DEBUG TESTING ONLY
  //RooDataSet* regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->Clone();
  delete regionD1DataSetSignal;
  int numSignalTracksInDRegion = regionDDataSetSignal->numEntries();

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


  vector<TH1F> bgHistsToUse;
  //vector<TH1F> bRegionHistsToUse;
  //vector<TH1F> cRegionHistsToUse;

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
      TH1F* iasBackgroundPredictionMassCutNoMSliceHist = 0;
      //TH1F* bRegionHist = 0;
      //TH1F* cRegionHist = 0;
      // get this eta/nom hist
      string getHistName = getHistNameBeg(lowerNoM,lowerEta);
      getHistName+=bgHistNameEnd;
      string fullPath = dirName;
      fullPath+="/";
      fullPath+=getHistName;

      iasBackgroundPredictionMassCutNoMSliceHist =
        (TH1F*)backgroundPredictionRootFile->Get(fullPath.c_str());

      // do the rest if the hist is found and integral > 0
      if(iasBackgroundPredictionMassCutNoMSliceHist)
          //&& iasBackgroundPredictionMassCutNoMSliceHist->Integral() > 0)
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
        // now get the b region hist
        //string getHistName = dirName;
        //getHistName+="/";
        //getHistName+=getHistNameBeg(lowerNoM,lowerEta);
        //getHistName+="bRegionHist";
        //bRegionHist = (TH1F*)backgroundPredictionRootFile->Get(getHistName.c_str());
        //if(bRegionHist && bRegionHist->Integral() > 0)
        //  bRegionHistsToUse.push_back(*bRegionHist);
        //// now get the c region hist
        //getHistName = dirName;
        //getHistName+="/";
        //getHistName+=getHistNameBeg(lowerNoM,lowerEta);
        //getHistName+="cRegionHist";
        //cRegionHist = (TH1F*)backgroundPredictionRootFile->Get(getHistName.c_str());
        //if(cRegionHist && cRegionHist->Integral() > 0)
        //  cRegionHistsToUse.push_back(*cRegionHist);
      }
      else
      {
        cout << "WARNING: no input found for this slice (histogram " <<
          fullPath << ") in file: " << backgroundPredictionRootFilename_ << ")." << 
          " Excluded. " << endl;
      }
      delete iasBackgroundPredictionMassCutNoMSliceHist;
      //delete bRegionHist;
      //delete cRegionHist;
    }
  }

  // initialize the global unrolled hists
  int numGlobalBins = numIasBins*bgHistsToUse.size();
  cout << "INFO: numIasBins = " << numIasBins << endl;
  cout << "INFO: numBGHistsToUse = " << bgHistsToUse.size() << endl;
  cout << "INFO: initializing unrolled histogram: bins=" << numGlobalBins << endl << endl;
  TH1F* backgroundAllNoMAllEtaUnrolledHist = new TH1F("backgroundAllNoMAllEtaUnrolledHist",
      "unrolled background hist",numGlobalBins,1,numGlobalBins+1);
  TH1F* backgroundAllNoMAllEtaUnrolledPlusOneSigmaHist = new TH1F("backgroundAllNoMAllEtaUnrolledPlusOneSigmaHist",
      "unrolled background hist (shape +1 sigma)",numGlobalBins,1,numGlobalBins+1);
  TH1F* backgroundAllNoMAllEtaUnrolledMinusOneSigmaHist = new TH1F("backgroundAllNoMAllEtaUnrolledMinusOneSigmaHist",
      "unrolled background hist (shape -1 sigma)",numGlobalBins,1,numGlobalBins+1);
  TH1F* signalAllNoMAllEtaUnrolledHist = new TH1F("signalAllNoMAllEtaUnrolledHist",
      "unrolled signal hist",numGlobalBins,1,numGlobalBins+1);
  TH2F* sigEffOverIasCutVsSliceHist = new TH2F("sigEffOverIasCutVsSlice","Sig eff. over ias cut;#eta;nom",12,0,2.4,9,5,23);
  sigEffOverIasCutVsSliceHist->GetYaxis()->SetNdivisions(509,false);
  TH1F* sigEffOverIasCutHist = new TH1F("sigEffOverIasCut","Signal eff. over ias cut",100,0,1);
  TH2F* backExpOverIasCutVsSliceHist = new TH2F("backExpOverIasCutVsSlice","Exp. background over ias cut;#eta;nom",12,0,2.4,9,5,23);
  backExpOverIasCutVsSliceHist->GetYaxis()->SetNdivisions(509,false);
  TH2F* backExpVsSliceHist = new TH2F("backExpVsSlice","Exp. background;#eta;nom",12,0,2.4,9,5,23);
  backExpVsSliceHist->GetYaxis()->SetNdivisions(509,false);
  TH1F* backExpOverIasCutHist = new TH1F("backExpOverIasCut","Exp. background over ias cut",100,0,1);
  TH2F* entriesSignalHist = new TH2F("entriesSignal","Signal tracks over mass cut in slice;#eta;nom",12,0,2.4,9,5,23);
  entriesSignalHist->GetYaxis()->SetNdivisions(509,false);
  TH2F* entriesSignalIasHist = new TH2F("entriesSignalIas","Signal tracks over mass/ias cuts in slice;#eta;nom",12,0,2.4,9,5,23);
  entriesSignalIasHist->GetYaxis()->SetNdivisions(509,false);

  double backgroundTracksOverIasCut = 0;
  double backgroundTracksOverIasCutErrorSqr = 0;
  double backgroundTracksOverIasCutNoApprox = 0;
  double backgroundTracksOverIasCutNoApproxErrorSqr = 0;
  int numBkOverIasCut = 0;
  double signalTracksOverIasCut = 0;
  double signalTracksTotal = 0;
  int signalEventsOverIasCut = 0;
  int numSignalTracksInDRegionPassingMassCut = 0;
  set<EventInfo> selectedSignalEventsSet;
  const RooArgSet* genArgSet = rooDataSetGenSignal->get();
  RooRealVar* numGenHSCPTracksRooVar = (RooRealVar*)genArgSet->find(rooVarNumGenHSCPTracks.GetName());
  RooRealVar* numGenHSCPEventsRooVar = (RooRealVar*)genArgSet->find(rooVarNumGenHSCPEvents.GetName());
  rooDataSetGenSignal->get(0);
  signalTracksTotal = numGenHSCPTracksRooVar->getVal();

  // loop over hists to use
  int iteratorPos = 0;
  for(vector<TH1F>::iterator histItr = bgHistsToUse.begin();
      histItr != bgHistsToUse.end(); ++histItr)
  {
    // delete this and the matching delete at the bottom las
    //TH1F* iasBackgroundPredictionMassCutNoMSliceHist = 0;
    TH1F* iasSignalMassCutNoMSliceHist = 0;
    TH1F* iasSignalMassCutNoMSliceForEffHist = 0;
    string bgHistName = string(histItr->GetName());
    string bgHistNameBeg = bgHistName.substr(0,bgHistName.size()-bgHistNameEnd.size());
    int lowerNoM = getLowerNoMFromHistName(bgHistName,bgHistNameEnd);
    float lowerEta = getLowerEtaFromHistName(bgHistName,bgHistNameEnd);
    string sigHistName = bgHistNameBeg;
    //sigHistName+="iasSignalVarBinHist";
    sigHistName+="iasSignalFixedHist";
    string iasSignalHistTitle = "Ias of signal";
    iasSignalHistTitle+=getHistTitleEnd(lowerNoM,lowerEta,massCut_);
    // get b region ias histogram
    string iasHistName = "bRegionHistograms/";
    iasHistName+=getHistNameBeg(lowerNoM,lowerEta);
    iasHistName+="iasBRegionHist";
    TH1F* iasHist = (TH1F*)backgroundPredictionRootFile->Get(iasHistName.c_str());
    // get c region tracks passing mass cut in ias bins
    string cRegionTracksOverMassCutProfileName="cRegionTracksOverIasCutInIasBins/";
    cRegionTracksOverMassCutProfileName+=getHistNameBeg(lowerNoM,lowerEta);
    cRegionTracksOverMassCutProfileName+="tracksInCOverMassCutProfile";
    TProfile* cRegionTracksOverMassCutProfile = (TProfile*)backgroundPredictionRootFile->Get(cRegionTracksOverMassCutProfileName.c_str());

    // debug
    cout << " Looking at hist: " << bgHistName << " lowerNoM = " << lowerNoM
      << " lowerEta = " << lowerEta << endl;

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
    std::string etaCutString = "(rooVarEta>";
    etaCutString+=floatToString(lowerEta);
    etaCutString+="&&rooVarEta<";
    std::string upperEtaLimit;
    //XXX TESTING eta 1.5 max
    if(10*lowerEta==14)
      upperEtaLimit=floatToString(1.5);
    else
      upperEtaLimit=floatToString(lowerEta+0.2);
    etaCutString+=upperEtaLimit;
    etaCutString+=")||(rooVarEta>-";
    etaCutString+=upperEtaLimit;
    etaCutString+="&&rooVarEta<-";
    etaCutString+=floatToString(lowerEta);
    etaCutString+=")";
    RooDataSet* etaCutNomCutDRegionDataSetSignal =
      (RooDataSet*)nomCutDRegionDataSetSignal->reduce(Cut(etaCutString.c_str()));
    int numSignalTracksInDRegionThisSlice = etaCutNomCutDRegionDataSetSignal->numEntries();
    cout << "numSignalTracksInDRegionThisSlice: " << numSignalTracksInDRegionThisSlice << endl;

    // construct signal prediction hist (with mass cut)
    const RooArgSet* argSet = etaCutNomCutDRegionDataSetSignal->get();
    RooRealVar* iasData = (RooRealVar*)argSet->find(rooVarIas.GetName());
    RooRealVar* nomIasData = (RooRealVar*)argSet->find(rooVarNoMias.GetName());
    RooRealVar* etaData = (RooRealVar*)argSet->find(rooVarEta.GetName());
    RooRealVar* ihData = (RooRealVar*)argSet->find(rooVarIh.GetName());
    RooRealVar* pData = (RooRealVar*)argSet->find(rooVarP.GetName());
    RooRealVar* eventNumData = (RooRealVar*)argSet->find(rooVarEvent.GetName());
    RooRealVar* lumiSecData = (RooRealVar*)argSet->find(rooVarLumiSection.GetName());
    RooRealVar* runNumData = (RooRealVar*)argSet->find(rooVarRun.GetName());
    for(int evt=0; evt < etaCutNomCutDRegionDataSetSignal->numEntries(); ++evt)
    {
      etaCutNomCutDRegionDataSetSignal->get(evt);
      if(fabs(etaData->getVal()) > 1.5) continue;
      // apply mass cut
      float massSqr = (ihData->getVal()-dEdx_c)*pow(pData->getVal(),2)/dEdx_k;
      if(massSqr < 0)
        continue;
      else if(sqrt(massSqr) >= massCut_)
      {
        iasSignalMassCutNoMSliceHist->Fill(iasData->getVal());
        numSignalTracksInDRegionPassingMassCut++;
        entriesSignalHist->Fill(fabs(etaData->getVal())+0.1,nomIasData->getVal());
        if(iasData->getVal() > iasCutForEffAcc)
        {
          // if track over ias cut, put the event in the set (one track per event kept)
          // there should only be one track per event at this stage anyway
          EventInfo evtInfo(runNumData->getVal(),lumiSecData->getVal(),eventNumData->getVal());
          pair<set<EventInfo>::iterator,bool> ret;
          ret = selectedSignalEventsSet.insert(evtInfo);
          if(ret.second)
          {
            //cout << "INFO: insert event - run: " << runNumData->getVal() << " lumiSec: " << lumiSecData->getVal()
            //  << " eventNum: " << eventNumData->getVal() << endl;
            signalEventsOverIasCut++;
            entriesSignalIasHist->Fill(fabs(etaData->getVal())+0.1,nomIasData->getVal());
          }
        }
      }
    }
    double numSignalTracksInDRegionMassCutThisSlice = iasSignalMassCutNoMSliceHist->Integral();
    // adjust for bin width ratio if variable bins used
    //for(int bin=1; bin<=iasSignalMassCutNoMSliceHist->GetNbinsX(); ++bin)
    //{
    //  double binWidthRatio =
    //    iasSignalMassCutNoMSliceHist->GetBinWidth(1)/
    //    iasSignalMassCutNoMSliceHist->GetBinWidth(bin);
    //  double binc = iasSignalMassCutNoMSliceHist->GetBinContent(bin);
    //  double bine = iasSignalMassCutNoMSliceHist->GetBinError(bin);
    //  iasSignalMassCutNoMSliceHist->SetBinContent(bin,binWidthRatio*binc);
    //  iasSignalMassCutNoMSliceHist->SetBinError(bin,binWidthRatio*bine);
    //}
    iasSignalMassCutNoMSliceForEffHist = (TH1F*) iasSignalMassCutNoMSliceHist->Clone();

    // figure out overall normalization this slice, background from ABCD
    // if using one track per event, this is the number of events in the slice passing mass in D region
    double bgEntriesInARegionThisSlice =
      aRegionBackgroundEntriesHist->GetBinContent(aRegionBackgroundEntriesHist->FindBin(lowerEta+0.1,lowerNoM+1));
    //double bgEntriesInBRegionThisSlice = 
    //  bRegionBackgroundEntriesHist->GetBinContent(bRegionBackgroundEntriesHist->FindBin(lowerEta+0.1,lowerNoM+1));
    double bgEntriesInCRegionThisSlice = 
      cRegionBackgroundEntriesHist->GetBinContent(cRegionBackgroundEntriesHist->FindBin(lowerEta+0.1,lowerNoM+1));
    //double fractionOfBGTracksPassingMassCutThisSlice = (bgEntriesInBRegionThisSlice > 0) ?
    //  histItr->Integral()/bgEntriesInBRegionThisSlice : 0;
    //double bgTracksInDThisSlice = (bgEntriesInARegionThisSlice > 0) ?
    //  bgEntriesInBRegionThisSlice*bgEntriesInCRegionThisSlice/bgEntriesInARegionThisSlice : 0;
    //double numBackgroundTracksInDRegionPassingMassCutThisSlice =
    //  bgTracksInDThisSlice*fractionOfBGTracksPassingMassCutThisSlice;
    int etaBinCRegionEntriesHist = cRegionBackgroundEntriesHist->GetXaxis()->FindBin(lowerEta+0.1);
    int cRegionNoMSummedEntries = cRegionBackgroundEntriesHist->Integral(
        etaBinCRegionEntriesHist,etaBinCRegionEntriesHist,1,cRegionBackgroundEntriesHist->GetNbinsY());
        
    //errors
    //double fractionOfBGTracksPassingMassCutThisSliceError =
    //  fractionOfBGTracksPassingMassCutThisSlice*
    //  sqrt(pow(sqrt(histItr->Integral())/histItr->Integral(),2)+
    //      pow(sqrt(bgEntriesInBRegionThisSlice)/bgEntriesInBRegionThisSlice,2));
    //double bgTracksInDThisSliceError = bgTracksInDThisSlice*
    //  sqrt(pow(sqrt(bgEntriesInARegionThisSlice)/bgEntriesInARegionThisSlice,2)+
    //      pow(sqrt(bgEntriesInBRegionThisSlice)/bgEntriesInBRegionThisSlice,2)+
    //      pow(sqrt(bgEntriesInCRegionThisSlice)/bgEntriesInCRegionThisSlice,2));
    //double numBackgroundTracksInDRegionPassingMassCutThisSliceError =
    //  (fractionOfBGTracksPassingMassCutThisSlice > 0) ?
    //  numBackgroundTracksInDRegionPassingMassCutThisSlice*
    //  sqrt(pow(bgTracksInDThisSliceError/bgTracksInDThisSlice,2)+
    //      pow(fractionOfBGTracksPassingMassCutThisSliceError/fractionOfBGTracksPassingMassCutThisSlice,2)): 1;
    //
    // get typical number of c region tracks above mass cut
    double averageCTracksAboveMassCut = 0;
    int numberOfBinsInCTracksAboveMassCutSum = 0;
    for(int bin=1; bin < cRegionTracksOverMassCutProfile->GetNbinsX(); ++bin)
    {
      double binc = cRegionTracksOverMassCutProfile->GetBinContent(bin);
      if(binc > 0)
      {
        averageCTracksAboveMassCut+=binc;
        numberOfBinsInCTracksAboveMassCutSum++;
      }
    }
    if(numberOfBinsInCTracksAboveMassCutSum > 0)
      averageCTracksAboveMassCut/=numberOfBinsInCTracksAboveMassCutSum;
    int startBin = iasHist->FindBin(iasCutForEffAcc);
    double totalBkOverIas = 0;
    for(int bin=startBin; bin <= iasHist->GetNbinsX(); ++bin)
      totalBkOverIas+=iasHist->GetBinContent(bin);
    // output
    cout << "Slice: lowerNoM = " << lowerNoM << " lowerEta = " << lowerEta << endl;
    //cout << "bg entries this slice: A = " << bgEntriesInARegionThisSlice << " B = " << 
    //  bgEntriesInBRegionThisSlice << " C = " << bgEntriesInCRegionThisSlice << " ==> D = " <<
    //  bgTracksInDThisSlice << endl;
    //cout << "INFO: SumBkOverIas = " << totalBkOverIas << endl;
    //cout << "INFO: average Cj over mass cut = " << averageCTracksAboveMassCut << endl;
    //cout << "INFO: total C over NoM slices = " << cRegionNoMSummedEntries << endl;
    // error output
    // have to include SUM[Cj > mass cut] error here
    //cout << "INFO: numBackgroundTracksInDRegionPassingMassCutThisSlice: " <<
    //  numBackgroundTracksInDRegionPassingMassCutThisSlice <<
    //  " +/- " << sqrt(relativeSliceErrorSqr)*numBackgroundTracksInDRegionPassingMassCutThisSlice <<
    //  endl;
    //backgroundTracksOverIasCutErrorSqr+=
    //  pow(numBackgroundTracksInDRegionPassingMassCutThisSlice*sqrt(relativeSliceErrorSqr),2);
    //double sliceError = sqrt(totalBkOverIas);
    //if(bgEntriesInARegionThisSlice > 0)
    //  backgroundTracksOverIasCutErrorSqr+=
    //    pow(sliceError*averageCTracksAboveMassCut*bgEntriesInCRegionThisSlice/
    //        (bgEntriesInARegionThisSlice*cRegionNoMSummedEntries),2);
    numBkOverIasCut+=totalBkOverIas;
    // include c tracks passing mass cut error
    double sliceError = 0;
    if(totalBkOverIas > 0)
      sliceError+= 1/totalBkOverIas;
    if(averageCTracksAboveMassCut > 0)
      sliceError+= 1/averageCTracksAboveMassCut;
    if(bgEntriesInCRegionThisSlice > 0)
      sliceError+= 1/bgEntriesInCRegionThisSlice;
    if(bgEntriesInARegionThisSlice > 0)
      sliceError+= 1/bgEntriesInARegionThisSlice;
    if(cRegionNoMSummedEntries > 0)
      sliceError+= 1/cRegionNoMSummedEntries;
    // take sqrt
    sliceError = sqrt(sliceError);
    if(bgEntriesInARegionThisSlice > 0)
      backgroundTracksOverIasCutErrorSqr+=
        pow(sliceError*totalBkOverIas*averageCTracksAboveMassCut*bgEntriesInCRegionThisSlice/
            (bgEntriesInARegionThisSlice*cRegionNoMSummedEntries),2);
    // central value
    if(bgEntriesInARegionThisSlice > 0)
      backgroundTracksOverIasCut+=
        totalBkOverIas*averageCTracksAboveMassCut*bgEntriesInCRegionThisSlice/
        (bgEntriesInARegionThisSlice*cRegionNoMSummedEntries);

    // figure out overall normalization for signal this slice
    // must normalize as if sigma were 1 to measure the cross section
    double signalCrossSection = 1;
    double totalSignalTracksThisSlice = integratedLumi*signalCrossSection; //TODO rename this var...
    //double fractionOfSigTracksInDRegionPassingMassCutThisSlice =
    //  numSignalTracksInDRegionMassCutThisSlice/(double)numSignalTracksTotal;
    double fractionOfSigTracksInDRegionPassingMassCutThisSlice =
      numSignalTracksInDRegionMassCutThisSlice/(double)numGenHSCPTracksRooVar->getVal(); // includes trigger eff.
    if(countEvents)
      fractionOfSigTracksInDRegionPassingMassCutThisSlice =
        numSignalTracksInDRegionMassCutThisSlice/(double)numGenHSCPEventsRooVar->getVal();
    double numSignalTracksInDRegionPassingMassCutThisSlice =
      fractionOfSigTracksInDRegionPassingMassCutThisSlice*totalSignalTracksThisSlice;
    signalTracksOverIasCut+=iasSignalMassCutNoMSliceHist->Integral(
        iasSignalMassCutNoMSliceHist->FindBin(iasCutForEffAcc),
        iasSignalMassCutNoMSliceHist->GetNbinsX());

    //// figure out nominal sig norm for efficiency value
    //double totalSignalTracksThisSliceEff = integratedLumi*signalCrossSectionForEff;
    //double fractionOfSigTracksInDRegionPassingMassCutThisSliceEff =
    //  numSignalTracksInDRegionMassCutThisSlice/(double)numSignalTracksTotal;
    //double numSignalTracksInDRegionPassingMassCutThisSliceEff =
    //  fractionOfSigTracksInDRegionPassingMassCutThisSliceEff*totalSignalTracksThisSliceEff;
    //double sigNormFactorEff =
    //  numSignalTracksInDRegionPassingMassCutThisSliceEff/
    //  iasSignalMassCutNoMSliceHist->Integral();
    //iasSignalMassCutNoMSliceForEffHist->Scale(sigNormFactorEff);
    // FIX below if wanted
    //signalAcceptanceIasCut+=iasSignalMassCutNoMSliceForEffHist->Integral(
    //    iasSignalMassCutNoMSliceForEffHist->FindBin(iasCutForEffAcc),
    //    iasSignalMassCutNoMSliceForEffHist->GetNbinsX())/iasSignalMassCutNoMSliceForEffHist->Integral();

    //cout << "INFO: fraction of bg tracks passing mass cut this slice = " <<
    //  fractionOfBGTracksPassingMassCutThisSlice << endl << 
    //  " ==> numBackgroundTracksInDRegionPassingMassCutThisSlice = " <<
    //  numBackgroundTracksInDRegionPassingMassCutThisSlice << 
    //  " and integral = " << histItr->Integral() << " so bgNormFactor = ";
    //if(histItr->Integral() > 0)
    //  cout << numBackgroundTracksInDRegionPassingMassCutThisSlice/histItr->Integral();
    //else
    //  cout << "0";
    //cout << endl;
    //cout << "INFO: sum Bk over iasCut = " <<
    //  histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX()) << endl;
    //cout << "INFO: ias cut frac = " <<
    //  histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX())/histItr->Integral() << endl;

    //cout << "first Ias bin this slice: " << iteratorPos*numIasBins << endl;
    double sigTracksOverIasCutThisSlice = 
      iasSignalMassCutNoMSliceHist->Integral(
          iasSignalMassCutNoMSliceHist->FindBin(iasCutForEffAcc),
          iasSignalMassCutNoMSliceHist->GetNbinsX());
    double sigTracksTotalThisSlice = iasSignalMassCutNoMSliceHist->Integral();
    //cout << "INFO: signal tracks total this slice: " <<
    //  sigTracksTotalThisSlice <<
    //  " fraction passing mass cut this slice: " <<
    //  fractionOfSigTracksInDRegionPassingMassCutThisSlice <<
    //  " signal tracks over ias cut this slice: " <<
    //  sigTracksOverIasCutThisSlice <<
    //  " (normed) signal tracks passing mass cut this slice: " << 
    //  numSignalTracksInDRegionPassingMassCutThisSlice <<
    //  " integral: " <<
    //  iasSignalMassCutNoMSliceHist->Integral() <<
    //  endl << endl;
    cout << "Signal tracks over Ias cut this slice: " << sigTracksOverIasCutThisSlice << endl;

    sigEffOverIasCutVsSliceHist->Fill(lowerEta+0.1,lowerNoM,sigTracksOverIasCutThisSlice/sigTracksTotalThisSlice);
    sigEffOverIasCutHist->Fill(sigTracksOverIasCutThisSlice/sigTracksTotalThisSlice);


    // normalize BG hist
    //double iasShapeIntegral = (histItr->Integral() > 0)  ?
    //  histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX())/histItr->Integral() : 0;
    //double bgTracksInDThisSliceError = (bgEntriesInARegionThisSlice > 0) ?
    //  sqrt(iasShapeIntegral*bgEntriesInBRegionThisSlice)*bgEntriesInCRegionThisSlice/bgEntriesInARegionThisSlice: 0; // ignore A,C errors
    //double numBackgroundTracksInDRegionPassingMassCutThisSliceError =
    //  fractionOfBGTracksPassingMassCutThisSlice*bgTracksInDThisSliceError;
    //backgroundTracksOverIasCutErrorSqr+=pow(numBackgroundTracksInDRegionPassingMassCutThisSliceError,2);
    
    //XXX mar 1 no longer needed
    //histItr->Sumw2();
    //double bgNormFactor = (histItr->Integral() > 0) ?
    //  numBackgroundTracksInDRegionPassingMassCutThisSlice/
    //  histItr->Integral(): 0;
    //histItr->Scale(bgNormFactor);
    //

    //TODO: handle the hists with zero background in systematic
    // (assume an entry in the lowest Ias bin)
  
    //if(lowerNoM==11 && (int)(lowerEta*10)==4)
    //{
    //  cout << "backgroundTracksOverIasCut for NoM 11 eta 0.4: " << 
    //    histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX()) << endl;
    //}
    //XXX SIC FEB 20 switching to new background prediction at least for testing
    cout << "Background tracks over Ias/mass cuts this slice: " << 
      histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX()) << endl;
    backgroundTracksOverIasCutNoApprox+=histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX());
    // compute error this slice -- assuming error in each ias bin is uncorrelated (if based only on Bk then it is)
    double errorSqrThisSlice = 0;
    for(int bin=histItr->FindBin(iasCutForEffAcc); bin <= histItr->GetNbinsX(); ++bin)
      errorSqrThisSlice+=pow(histItr->GetBinError(bin),2);
    backgroundTracksOverIasCutNoApproxErrorSqr+=errorSqrThisSlice;
    backExpOverIasCutVsSliceHist->Fill(
        lowerEta+0.1,lowerNoM,histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX()));
    backExpVsSliceHist->Fill(lowerEta+0.1,lowerNoM,histItr->Integral());
    backExpOverIasCutHist->Fill(histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX()));
    // normalize sig hist
    iasSignalMassCutNoMSliceHist->Sumw2();
    double sigNormFactor = (iasSignalMassCutNoMSliceHist->Integral() > 0) ?
      numSignalTracksInDRegionPassingMassCutThisSlice/iasSignalMassCutNoMSliceHist->Integral() : 0;
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
      // plus one sigma
      backgroundAllNoMAllEtaUnrolledPlusOneSigmaHist->SetBinContent(globalBinIndex,binc+bine);
      backgroundAllNoMAllEtaUnrolledPlusOneSigmaHist->SetBinError(globalBinIndex,bine);
      // minus one sigma
      if(binc-bine < 0)
        bine = 0.3*binc; // if 1-sigma shift is negative, shift down to 30% of nominal
      backgroundAllNoMAllEtaUnrolledMinusOneSigmaHist->SetBinContent(globalBinIndex,binc-bine);
      backgroundAllNoMAllEtaUnrolledMinusOneSigmaHist->SetBinError(globalBinIndex,bine);

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

    iteratorPos++;
    // cleanup
    delete nomCutDRegionDataSetSignal;
    delete etaCutNomCutDRegionDataSetSignal;
    //delete iasBackgroundPredictionMassCutNoMSliceHist;
    delete iasSignalMassCutNoMSliceHist;
    delete iasSignalMassCutNoMSliceForEffHist;
    delete iasHist;
  }

  // now do the standard analysis style prediction
  TH1F* etaBRegionHist = (TH1F*) backgroundPredictionRootFile->Get("etaBRegionHist");
  TH1F* etaARegionHist = (TH1F*) backgroundPredictionRootFile->Get("etaARegionHist");
  TH2F* pEtaCRegionHist = (TH2F*) backgroundPredictionRootFile->Get("pEtaCRegionHist");
  TH2F* pEtaCRegionOrigHist = (TH2F*) pEtaCRegionHist->Clone();
  pEtaCRegionOrigHist->SetName("pEtaCRegionOrig");
  TH1F* ihBRegionTotalHist = (TH1F*) backgroundPredictionRootFile->Get("ihBRegionTotalHist");
  double aRegionInt = etaARegionHist->Integral();
  double aRegionIntError = sqrt(aRegionInt);
  double bRegionInt = ihBRegionTotalHist->Integral();
  double bRegionIntError = sqrt(bRegionInt);
  double cRegionInt = pEtaCRegionHist->Integral();
  double cRegionIntError = sqrt(cRegionInt);
  double dPredTotal = 0;
  double dPredTotalError = 1;
  if(aRegionInt <= 0)
    std::cout << "ERROR: cannot do standard analysis prediction." << endl;
  else
  {
    dPredTotal = bRegionInt*cRegionInt/aRegionInt;
    dPredTotalError = dPredTotal*
      sqrt(pow(aRegionIntError/aRegionInt,2)+pow(bRegionIntError/bRegionInt,2)+pow(cRegionIntError/cRegionInt,2));
  }

  std::cout << "A = " << aRegionInt << " +/- " << aRegionIntError << std::endl <<
               "B = " << bRegionInt << " +/- " << bRegionIntError << std::endl <<
               "C = " << cRegionInt << " +/- " << cRegionIntError << std::endl <<
               "dPredTotal = " << dPredTotal << " +/- " << dPredTotalError << std::endl;

  // normalize
  etaBRegionHist->Scale(1.0/etaBRegionHist->Integral());
  etaARegionHist->Scale(1.0/etaARegionHist->Integral());

  for(int bin=0; bin <= pEtaCRegionHist->GetXaxis()->GetNbins(); ++bin)
  {
    double weight = 0;
    if(etaARegionHist->GetBinContent(bin) > 0)
    {
      weight = etaBRegionHist->GetBinContent(bin)/etaARegionHist->GetBinContent(bin);
      std::cout << "eta = " << etaARegionHist->GetBinCenter(bin) << " weight = " << weight << std::endl;
    }

    for(int y=0; y <= pEtaCRegionHist->GetYaxis()->GetNbins(); ++y)
      pEtaCRegionHist->SetBinContent(bin,y,pEtaCRegionHist->GetBinContent(bin,y)*weight);
  }
  //std::cout << "NOT doing eta reweighting." << std::endl;
  TH1D* reweightedPInCRegionHist = pEtaCRegionHist->ProjectionY("reweightedPInCRegion");
  reweightedPInCRegionHist->Sumw2();
  ihBRegionTotalHist->Sumw2();
  reweightedPInCRegionHist->Scale(1.0/reweightedPInCRegionHist->Integral());
  ihBRegionTotalHist->Scale(1.0/ihBRegionTotalHist->Integral());
  //std::cout << "Entries in reweightedPInCRegionHist = " << reweightedPInCRegionHist->GetEntries() << std::endl;

  // construct mass dist
  TH1F* massHist = new TH1F("massHist","predicted mass",200,0,2000);
  TH1F* massErrorHist = new TH1F("massErrorHist","predicted mass error",200,0,2000);
  for(int x=0; x <= reweightedPInCRegionHist->GetNbinsX(); ++x)
  {
    //std::cout << "DEBUG: reweightedPInCRegionHist bin = " << x << " binContent = " << reweightedPInCRegionHist->GetBinContent(x)
    //  << std::endl;
    if(reweightedPInCRegionHist->GetBinContent(x) <= 0.0)
      continue;
    double thisP = reweightedPInCRegionHist->GetBinCenter(x);
    for(int y=0; y <= ihBRegionTotalHist->GetNbinsX(); ++y)
    {
      //std::cout << "DEBUG: ihBRegionTotalHist bin = " << y << " binContent = " << ihBRegionTotalHist->GetBinContent(y)
      //  << std::endl;
      if(ihBRegionTotalHist->GetBinContent(y) <= 0.0)
        continue;
      double thisIh = ihBRegionTotalHist->GetBinCenter(y);
      double prob = reweightedPInCRegionHist->GetBinContent(x) * ihBRegionTotalHist->GetBinContent(y);
      double probError = prob*sqrt(
          pow(reweightedPInCRegionHist->GetBinError(x)/reweightedPInCRegionHist->GetBinContent(x),2)+
          pow(ihBRegionTotalHist->GetBinError(y)/ihBRegionTotalHist->GetBinContent(y),2));
      //std::cout << "\tDEBUG: prob = " << prob << std::endl;
      if(prob <= 0 || isnan(prob))
        continue;
      double massSqr = (thisIh-dEdx_c)*pow(thisP,2)/dEdx_k;
      //std::cout << "\tDEBUG: mass^2 = " << massSqr << std::endl;
      if(massSqr >= 0)
      {
        massHist->Fill(sqrt(massSqr),prob);
        massErrorHist->Fill(sqrt(massSqr),probError);
        //std::cout << "Fill mass = " << sqrt(massSqr) << " prob: " << prob << std::endl;
        //std::cout << "Fill mass = " << sqrt(massSqr) << " probError: " << probError << std::endl;
      }
    }
  }

  for(int x=0; x <= massHist->GetNbinsX(); ++x)
  {
    double scaledMass = massHist->GetBinContent(x) * dPredTotal;
    massHist->SetBinContent(x,scaledMass);
    if(massHist->GetBinContent(x) > 0)
      massErrorHist->SetBinContent(x,
          scaledMass*sqrt(pow(massErrorHist->GetBinContent(x)/massHist->GetBinContent(x),2)+
            pow(dPredTotalError/dPredTotal,2)));
      //massErrorHist->SetBinContent(x,massHist->GetBinContent(x)*dPredTotalError);
    else
      massErrorHist->SetBinContent(x,0);
    //cout << "DEBUG: massError this bin (" << x << ") = " << 
    //    scaledMass*sqrt(pow(massErrorHist->GetBinContent(x)/massHist->GetBinContent(x),2)+
    //      pow(dPredTotalError/dPredTotal,2)) << endl;
    //if(isnan(tmpH_Mass    ->GetBinContent(x) * PE_P)){printf("%f x %f\n",tmpH_Mass    ->GetBinContent(x),PE_P); fflush(stdout);exit(0);}
  }

  cout << endl << endl
    << "Number of background tracks in mass hist over ias cut (std ana method): "
    << massHist->Integral()
    << endl
    << "Number of background tracks expected over ias and mass cut (std ana method): "
    << massHist->Integral(massHist->FindBin(massCut_),massHist->GetNbinsX());
  double errorSqrSum = 0;
  for(int i=massHist->FindBin(massCut_); i <=massHist->GetNbinsX(); ++i)
    errorSqrSum+=pow(massErrorHist->GetBinContent(i),2);
  double massPredError = sqrt(errorSqrSum);
  cout << " +/- " << massPredError
       << endl << endl;
  // end of standard analysis style prediction


  cout << endl << endl << "Ias cut = " << iasCutForEffAcc << endl << "\tfound " << backgroundTracksOverIasCutNoApprox
    << " +/- " << sqrt(backgroundTracksOverIasCutNoApproxErrorSqr)
    << " background tracks over ias cut from ias pred hist" << endl 
    << backgroundTracksOverIasCut
    << " +/- " << sqrt(backgroundTracksOverIasCutErrorSqr)
    << " tracks over ias cut from approximation" << endl
    << numBkOverIasCut << " Bk tracks over ias cut"
    << endl << endl
    << signalEventsOverIasCut << " signal events over ias cut" << endl
    << signalEventsOverIasCut/numGenHSCPEventsRooVar->getVal() << " signal efficiency (event level) or" << endl 
    << signalTracksOverIasCut/signalTracksTotal << " signal efficiency (track level) with this ias cut. " << endl << endl;

  cout << "original generated events: " << numGenHSCPEventsRooVar->getVal() << " events over ias cut: " << signalEventsOverIasCut
    << endl;
  cout << "original generated tracks: " << numGenHSCPTracksRooVar->getVal() << " tracks over ias cut: " << signalTracksOverIasCut
    << endl;
  cout << "orig signal tracks passing trig/presel: " << numSignalTracksTotal << endl << " orig signal tracks in D region: "
    << numSignalTracksInDRegion << endl << " original signal tracks in D region passing mass cut: "
    << numSignalTracksInDRegionPassingMassCut
    << endl;
  cout << "(eff) signal tracks passing trig/presel: " << numSignalTracksTotal/numGenHSCPTracksRooVar->getVal() << endl
    << " (eff) signal tracks in D region: " << numSignalTracksInDRegion/numGenHSCPTracksRooVar->getVal() << endl
    << " (eff) signal tracks in D region passing mass cut: " << numSignalTracksInDRegionPassingMassCut/numGenHSCPTracksRooVar->getVal()
    << endl;

  outputRootFile->cd();
  etaBRegionHist->Write();
  etaARegionHist->Write();
  pEtaCRegionHist->Write();
  pEtaCRegionOrigHist->Write();
  ihBRegionTotalHist->Write();
  reweightedPInCRegionHist->Write();
  massHist->Write();
  backgroundAllNoMAllEtaUnrolledHist->Write();
  backgroundAllNoMAllEtaUnrolledPlusOneSigmaHist->Write();
  backgroundAllNoMAllEtaUnrolledMinusOneSigmaHist->Write();
  signalAllNoMAllEtaUnrolledHist->Write();
  sigEffOverIasCutVsSliceHist->Write();
  sigEffOverIasCutHist->Write();
  backExpOverIasCutVsSliceHist->Write();
  backExpVsSliceHist->Write();
  backExpOverIasCutHist->Write();
  entriesSignalHist->Write();
  entriesSignalIasHist->Write();
  outputRootFile->Close();

  //backgroundPredictionRootFile->Close();
  //signalRootFile->Close();

  if(backgroundAllNoMAllEtaUnrolledHist->Integral() <= 0)
    return -1;
}

