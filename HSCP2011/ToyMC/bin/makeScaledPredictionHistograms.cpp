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
std::string generateFileNameEnd(double massCut, double pSideband, double ptSideband, bool usePt, double ihSideband)
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
  fileNameEnd+=".ih";
  // replace "." with "p" for the ih
  std::string ihSBStr = floatToString(ihSideband);
  ihSBStr.replace(ihSBStr.find("."),1,"p");
  fileNameEnd+=ihSBStr;

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
  RooDataSet* regionD1DataSetSignal;
  if(usePtForSideband)
    regionD1DataSetSignal = (RooDataSet*)rooDataSetAllSignal->reduce(Cut(ptSearchCutString.c_str()));
  else
    regionD1DataSetSignal = (RooDataSet*)rooDataSetAllSignal->reduce(Cut(pSearchCutString.c_str()));
  RooDataSet* regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->reduce(Cut(ihSearchCutString.c_str()));
  //DEBUG TESTING ONLY
  //RooDataSet* regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->Clone();
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
      delete iasBackgroundPredictionMassCutNoMSliceHist;
    }
  }

  // initialize the global unrolled hists
  int numGlobalBins = numIasBins*bgHistsToUse.size();
  cout << "INFO: numIasBins = " << numIasBins << endl;
  cout << "INFO: numBGHistsToUse = " << bgHistsToUse.size() << endl;
  cout << "INFO: initializing unrolled histogram: bins=" << numGlobalBins << endl;
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

  double backgroundTracksOverIasCut = 0;
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
    TH1F* iasBackgroundPredictionMassCutNoMSliceHist = 0;
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
    RooRealVar* eventNumData = (RooRealVar*)argSet->find(rooVarEvent.GetName());
    RooRealVar* lumiSecData = (RooRealVar*)argSet->find(rooVarLumiSection.GetName());
    RooRealVar* runNumData = (RooRealVar*)argSet->find(rooVarRun.GetName());
    for(int evt=0; evt < etaCutNomCutDRegionDataSetSignal->numEntries(); ++evt)
    {
      etaCutNomCutDRegionDataSetSignal->get(evt);
      // apply mass cut
      float massSqr = (ihData->getVal()-dEdx_c)*pow(pData->getVal(),2)/dEdx_k;
      if(massSqr < 0)
        continue;
      else if(sqrt(massSqr) >= massCut_)
      {
        iasSignalMassCutNoMSliceHist->Fill(iasData->getVal());
        numSignalTracksInDRegionPassingMassCut++;
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
          }
        }
      }
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
    iasSignalMassCutNoMSliceForEffHist = (TH1F*) iasSignalMassCutNoMSliceHist->Clone();

    // figure out overall normalization this slice, background from ABCD
    // if using one track per event, this is the number of events in the slice passing mass in D region
    double bgEntriesInARegionThisSlice =
      aRegionBackgroundEntriesHist->GetBinContent(aRegionBackgroundEntriesHist->FindBin(lowerEta+0.1,lowerNoM+1));
    double bgEntriesInBRegionThisSlice = 
      bRegionBackgroundEntriesHist->GetBinContent(bRegionBackgroundEntriesHist->FindBin(lowerEta+0.1,lowerNoM+1));
    double bgEntriesInCRegionThisSlice = 
      cRegionBackgroundEntriesHist->GetBinContent(cRegionBackgroundEntriesHist->FindBin(lowerEta+0.1,lowerNoM+1));
    double fractionOfBGTracksPassingMassCutThisSlice =
      histItr->Integral()/bgEntriesInBRegionThisSlice;
    double bgTracksInDThisSlice =
      bgEntriesInBRegionThisSlice*bgEntriesInCRegionThisSlice/bgEntriesInARegionThisSlice;
    double numBackgroundTracksInDRegionPassingMassCutThisSlice =
      bgTracksInDThisSlice*fractionOfBGTracksPassingMassCutThisSlice;

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

    // output
    cout << "Slice: lowerNoM = " << lowerNoM << " lowerEta = " << lowerEta << endl;
    cout << "bg entries this slice: A = " << bgEntriesInARegionThisSlice << " B = " << 
      bgEntriesInBRegionThisSlice << " C = " << bgEntriesInCRegionThisSlice << " ==> D = " <<
      bgTracksInDThisSlice << " and fraction of bg tracks passing mass cut this slice = " <<
      fractionOfBGTracksPassingMassCutThisSlice << " ==> norm factor = " <<
      numBackgroundTracksInDRegionPassingMassCutThisSlice << 
      " and integral = " << histItr->Integral() <<
      " so bgNormFactor = " << numBackgroundTracksInDRegionPassingMassCutThisSlice/histItr->Integral() <<
      endl;
    //cout << "first Ias bin this slice: " << iteratorPos*numIasBins << endl;
    double sigTracksOverIasCutThisSlice = 
      iasSignalMassCutNoMSliceHist->Integral(
          iasSignalMassCutNoMSliceHist->FindBin(iasCutForEffAcc),
          iasSignalMassCutNoMSliceHist->GetNbinsX());
    double sigTracksTotalThisSlice = iasSignalMassCutNoMSliceHist->Integral();
    cout << "INFO: signal tracks total this slice: " <<
      sigTracksTotalThisSlice <<
      " fraction passing mass cut this slice: " <<
      fractionOfSigTracksInDRegionPassingMassCutThisSlice <<
      " signal tracks over ias cut this slice: " <<
      sigTracksOverIasCutThisSlice <<
      " (normed) signal tracks passing mass cut this slice: " << 
      numSignalTracksInDRegionPassingMassCutThisSlice <<
      " integral: " <<
      iasSignalMassCutNoMSliceHist->Integral() <<
      endl;

    sigEffOverIasCutVsSliceHist->Fill(lowerEta+0.1,lowerNoM,sigTracksOverIasCutThisSlice/sigTracksTotalThisSlice);
    sigEffOverIasCutHist->Fill(sigTracksOverIasCutThisSlice/sigTracksTotalThisSlice);


    // normalize BG hist
    histItr->Sumw2();
    if(histItr->Integral() <= 0)
    {
      cout << "ERROR: This histogram (" << histItr->GetName() << ") has integral <= 0. Quitting." << endl;
      return -15;
    }
    double bgNormFactor =
      numBackgroundTracksInDRegionPassingMassCutThisSlice/
      histItr->Integral();
    histItr->Scale(bgNormFactor);
  
    //if(lowerNoM==11 && (int)(lowerEta*10)==4)
    //{
    //  cout << "backgroundTracksOverIasCut for NoM 11 eta 0.4: " << 
    //    histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX()) << endl;
    //}
    backgroundTracksOverIasCut+=histItr->Integral(histItr->FindBin(iasCutForEffAcc),histItr->GetNbinsX());
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
    delete iasBackgroundPredictionMassCutNoMSliceHist;
    delete iasSignalMassCutNoMSliceHist;
    delete iasSignalMassCutNoMSliceForEffHist;
  }

  outputRootFile->cd();
  backgroundAllNoMAllEtaUnrolledHist->Write();
  backgroundAllNoMAllEtaUnrolledPlusOneSigmaHist->Write();
  backgroundAllNoMAllEtaUnrolledMinusOneSigmaHist->Write();
  signalAllNoMAllEtaUnrolledHist->Write();
  sigEffOverIasCutVsSliceHist->Write();
  sigEffOverIasCutHist->Write();
  backExpOverIasCutVsSliceHist->Write();
  backExpVsSliceHist->Write();
  backExpOverIasCutHist->Write();
  outputRootFile->Close();

  cout << endl << endl << "Ias cut = " << iasCutForEffAcc << endl << "\tfound " << backgroundTracksOverIasCut
    << " background tracks over ias cut and " << signalTracksOverIasCut/signalTracksTotal
    << " signal efficiency (track level) or " << signalEventsOverIasCut/numGenHSCPEventsRooVar->getVal()
    << " (event level) with this ias cut." << endl;
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
}

