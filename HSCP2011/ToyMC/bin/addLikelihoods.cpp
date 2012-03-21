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
#include "RooMCStudy.h"

#include "TSystem.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <vector>

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
//TODO: consolidate these in a single place (something like common functions)
// nll values
RooRealVar sigNLLRooVar("sigNLLRooVar","signal NLL",0,-100000,100000);
RooRealVar backNLLRooVar("backNLLRooVar","background NLL",-100000,100000);
RooRealVar sigBackNLLRooVar("sigBackNLLRooVar","signal+background NLL",-100000,100000);
RooRealVar satModelNLLRooVar("satModelNLLRooVar","sat model NLL",-100000,100000);
RooRealVar sigBackFitNLLRooVar("sigBackFitNLLRooVar","signal+background ML fit",-100000,100000);
RooRealVar maxLSigFracRooVar("maxLSigFracRooVar","max L fit signal frac",0,1);
RooRealVar assumedTotalSigTracksRooVar("assumedTotalSigTracksRooVar","assumed total signal tracks",0,1e4);
RooRealVar assumedSigTracksThisSliceRooVar("assumedSigTracksThisSliceRooVar","assumed signal tracks this slice",0,1e4);
RooRealVar trialIndexRooVar("trialIndex","trial index",0,1e8);
// exp events (tracks)
RooRealVar expectedBackgroundEventsInLastBinRooVar("expectedBackgroundEventsInLastBinRooVar","expected bg events in last bin",0,20000);
RooRealVar expectedSignalEventsInLastBinRooVar("expectedSignalEventsInLastBinRooVar","expected signal events in last bin",0,20000);
RooRealVar expectedBackgroundEventsThisSliceRooVar("expectedBackgroundEventsThisSliceRooVar","expected bg events this slice",0,50000);
RooRealVar expectedSignalEventsThisSliceRooVar("expectedSignalEventsThisSliceRooVar","expected signal events this slice",0,20000);
RooRealVar expectedTotalSignalFractionRooVar("expectedTotalSignalFractionRooVar","expected total signal fraction",0,1);
RooRealVar numberOfTrialsRooVar("numberOfTrialsRooVar","number of trials",0,1e8);
// observed events
RooRealVar actualEventsInLastBinRooVar("actualEventsInLastBinRooVar","actual events in last bin",0,20000);
RooRealVar signalEventsInLastBinRooVar("signalEventsInLastBinRooVar","signal events in last bin",0,0,20000);
//TODO move to something like common functions
//RooRealVar rooVarIas("rooVarIas","ias",0,1);


struct SampleHistos
{
  // NLL B/sat hist
  TH1F* nllBSatHist;
  // NLL sat only hist
  TH1F* nllSatOnlyHist;
  // NLL B only hist
  TH1F* nllBOnlyHist;
  // NLL S only hist
  TH1F* nllSOnlyHist;
  // NLL SB hist
  TH1F* nllSBHist;
  // NLL S+B/B
  TH1F* nllSBOverBHist;
  // profile LR
  TH1F* profileLRHist;
  // points per trial
  TH1F* pointsPerTrialHist;
  // events in last bin vs. NLL SB/B
  TH2F* eventsInLastBinVsNLLSBOverBHist;
  // events total vs. NLL SB/B
  TH2F* eventsTotalVsNLLSBOverBHist;
};

struct NLLSums
{
  int numTrials;
  // below, each entry is one trial
  //std::vector<double> nllSsums;
  std::vector<double> nllBsums;
  std::vector<double> nllSBsums;
  std::vector<double> nllSatsums;
  std::vector<double> nllSBFitsums;
  std::vector<double> profileLRsums;
  std::vector<int> pointsPerTrial;
  std::vector<double> expectedBgEvtsLastBinSums;
  std::vector<double> expectedSigEvtsLastBinSums;
  std::vector<double> expectedBgEvtsThisSliceSums;
  std::vector<double> expectedSigEvtsThisSliceSums;
  std::vector<int> actualEvtsLastBinSums;
  std::vector<int> signalEvtsLastBinSums;
  //std::vector<int> actualEvtsTotalSums;
  //std::vector<int> signalEvtsTotalSums;

  NLLSums() : numTrials(0)
  {
  }

};

struct SampleSums // sums over all nom/eta slices for each trial
{
  float totalSignalFraction;
  int totalSignalTracks;

  // map assumed (total) number of signal tracks to its sums
  std::map<int,NLLSums> nllSumsMap;

  SampleSums() : totalSignalFraction(0), totalSignalTracks(0)
  {
  }

};


// helper functions
int getNoMFromFilename(std::string str)
{
  //'file:/afs/cern.ch/user/s/scooper/scratch0/cmssw/MakeIasDistributionsBatch/src/HSCP2011/ToyMC/bin/FARM_ToyMC_nov12_0signalTracks_10799backgroundTracks/outputs/toyMCoutput_NoM5eta0.root',
  int posNoM = str.find("NoM");
  int posEta = str.find("eta");
  std::string nomString = str.substr(posNoM+3,posEta-posNoM+3);
  return atoi(nomString.c_str());
}
//
int getEtaFromFilename(std::string str)
{
  int posEta = str.find("eta");
  int posDotRoot = str.find(".root");
  std::string etaString = str.substr(posEta+3,posDotRoot-posEta+3);
  return atoi(etaString.c_str());
}
//
int getNumTotalSignalTracksInSamplesThisFile(std::string str)
{
  int posSigTracks = str.find("sigTracks");
  int posBgTracks = str.find("bgTracks_");
  std::string numSigTracks = str.substr(posBgTracks+9,posSigTracks-posBgTracks+9);
  return atoi(numSigTracks.c_str());
}
// slice is what's used in doToyMC
//int getNoMSliceFromNoM(int lowerNoM) -- lower NoM is the NoM slice
//int getLowerNoMFromNoMSlice(int nomSlice)
//
int getEtaSliceFromLowerEta(float eta)
{
  return eta*10;
}
//
float getLowerEtaFromEtaSlice(int etaSlice)
{
  return (etaSlice/2)*0.2;
}
// index is the array index
int getNoMIndexFromLowerNoM(int NoM)
{
  int nomIndex = NoM-5;
  return nomIndex/2;
}
//
int getLowerNoMFromNoMIndex(int nomIndex)
{
  return 2*(nomIndex)+5;
}
//
int getEtaIndexFromLowerEta(float eta)
{
  return (eta*10)/2;
}
//
float getLowerEtaFromEtaIndex(int etaIndex)
{
  return (etaIndex*2)/10.0;
}
//
void initHistos(int assumedSignalTracks, SampleHistos& histos, bool isSignal, TFileDirectory& tFileDir)
{
  std::string assuSigTracksStr = intToString(assumedSignalTracks);
  std::string titleEnd = assuSigTracksStr+" assumed signal tracks";
  std::string nameEnd = assuSigTracksStr+"assumedSignalTracks";
  // NLL B/sat hist
  std::string nllBSatHistName="nllBSatBGOnly";
  std::string nllBSatHistTitle = "-2*ln(B/sat), bg only samples, for ";
  // NLL sat only hist
  std::string nllSatOnlyHistName="nllSatonlyBGOnly";
  std::string nllSatOnlyHistTitle = "NLL sat only, bg only samples, for ";
  // NLL B only hist
  std::string nllBOnlyHistName="nllBonlyBGOnly";
  std::string nllBOnlyHistTitle = "NLL B only, bg only samples, for ";
  // NLL S only hist
  std::string nllSonlyHistName="nllSonlyBGOnly";
  std::string nllSonlyHistTitle = "NLL S only, bg only samples, for ";
  // NLL SB hist
  std::string nllSBHistName="nllSBBGOnly";
  std::string nllSBHistTitle = "NLL SB, bg only samples, for ";
  // NLL S+B/B
  std::string nllSBOverBHistName="lepQBGOnly";
  std::string nllSBOverBHistTitle = "-2*ln(L(s+b)/L(b)), bg only samples, for ";
  // profile LR
  std::string profileLRHistName="profileLRBGOnly";
  std::string profileLRHistTitle = "-2*ln(L(#mu)/L(#hat{#mu})), bg only samples, for ";
  // points per trial
  std::string pointsPerTrialHistName="pointsPerTrialBGOnly";
  std::string pointsPerTrialHistTitle="Points per trial, bg only samples, for ";
  // events in last bin vs. NLL SB/B
  std::string eventsInLastBinVsNLLSBOverBHistName="eventsInLastBinVsNLLSBOverBBGOnly";
  std::string eventsInLastBinVsNLLSBOverBHistTitle="Events in last bin vs. NLL -2*ln(SB/B), bg only samples, for ";
  // events total vs. NLL SB/B
  std::string eventsTotalVsNLLSBOverBHistName="eventsInTotalVsNLLSBOverBBGOnly";
  std::string eventsTotalVsNLLSBOverBHistTitle="Events total vs. NLL -2*ln(SB/B), bg only samples, for ";
  if(isSignal)
  {
    titleEnd = assuSigTracksStr+" signal tracks";
    nameEnd = assuSigTracksStr+"signalTracks";
    nllBSatHistName="nllBSatSPlusB";
    nllBSatHistTitle = "-2*ln(B/sat), s+b samples, for ";
    nllSatOnlyHistName="nllSatonlySPlusB";
    nllSatOnlyHistTitle = "NLL sat only, s+b samples, for ";
    nllBOnlyHistName="nllBonlySPlusB";
    nllBOnlyHistTitle = "NLL B only, s+b samples, for ";
    nllSonlyHistName="nllSonlySPlusB";
    nllSonlyHistTitle = "NLL S only, s+b samples, for ";
    nllSBHistName="nllSBSPlusB";
    nllSBHistTitle = "NLL SB, s+b samples, for ";
    nllSBOverBHistName="lepQSPlusB";
    nllSBOverBHistTitle = "-2*ln(L(s+b)/L(b)), s+b samples, for ";
    profileLRHistName="profileLRSPlusB";
    profileLRHistTitle = "-2*ln(L(#mu)/L(#hat{#mu})), s+b samples, for ";
    pointsPerTrialHistName="pointsPerTrialSPlusB";
    pointsPerTrialHistTitle="Points per trial, s+b samples, for ";
    eventsInLastBinVsNLLSBOverBHistName="eventsInLastBinVsNLLSBOverBSPlusB";
    eventsInLastBinVsNLLSBOverBHistTitle="Events in last bin vs. NLL -2*ln(SB/B), s+b samples, for ";
    eventsTotalVsNLLSBOverBHistName="eventsInTotalVsNLLSBOverBSPlusB";
    eventsTotalVsNLLSBOverBHistTitle="Events total vs. NLL -2*ln(SB/B), s+b samples, for ";
  }
  nllBSatHistName+=nameEnd;
  nllBSatHistTitle+=titleEnd;
  nllSatOnlyHistName+=nameEnd;
  nllSatOnlyHistTitle+=titleEnd;
  nllBOnlyHistName+=nameEnd;
  nllBOnlyHistTitle+=titleEnd;
  nllSonlyHistName+=nameEnd;
  nllSonlyHistTitle+=titleEnd;
  nllSBHistName+=nameEnd;
  nllSBHistTitle+=titleEnd;
  nllSBOverBHistName+=nameEnd;
  nllSBOverBHistTitle+=titleEnd;
  nllSBOverBHistTitle+=";-2 ln Q";
  profileLRHistName+=nameEnd;
  profileLRHistTitle+=titleEnd;
  profileLRHistTitle+=";q_{#mu}";
  pointsPerTrialHistName+=nameEnd;
  pointsPerTrialHistTitle+=titleEnd;
  eventsInLastBinVsNLLSBOverBHistName+=nameEnd;
  eventsInLastBinVsNLLSBOverBHistTitle+=titleEnd;
  eventsTotalVsNLLSBOverBHistName+=nameEnd;
  eventsTotalVsNLLSBOverBHistTitle+=titleEnd;

  // init hists
  histos.nllBSatHist = tFileDir.make<TH1F>(nllBSatHistName.c_str(),nllBSatHistTitle.c_str(),100,0,1000);
  histos.nllSatOnlyHist = tFileDir.make<TH1F>(nllSatOnlyHistName.c_str(),nllSatOnlyHistTitle.c_str(),10000,-5000,5000);
  histos.nllBOnlyHist = tFileDir.make<TH1F>(nllBOnlyHistName.c_str(),nllBOnlyHistTitle.c_str(),10000,-5000,5000);
  histos.nllSOnlyHist = tFileDir.make<TH1F>(nllSonlyHistName.c_str(),nllSonlyHistTitle.c_str(),10000,-5000,5000);
  histos.nllSBHist = tFileDir.make<TH1F>(nllSBHistName.c_str(),nllSBHistTitle.c_str(),10000,-5000,5000);
  histos.nllSBOverBHist = tFileDir.make<TH1F>(nllSBOverBHistName.c_str(),nllSBOverBHistTitle.c_str(),800,-1000,1000);
  histos.profileLRHist = tFileDir.make<TH1F>(profileLRHistName.c_str(),profileLRHistTitle.c_str(),2000,-100,100);
  histos.pointsPerTrialHist = tFileDir.make<TH1F>(pointsPerTrialHistName.c_str(),
      pointsPerTrialHistTitle.c_str(),20000,0,20000);
  histos.eventsInLastBinVsNLLSBOverBHist = tFileDir.make<TH2F>(eventsInLastBinVsNLLSBOverBHistName.c_str(),
      eventsInLastBinVsNLLSBOverBHistTitle.c_str(),800,-1000,1000,100,0,100);
  histos.eventsTotalVsNLLSBOverBHist = tFileDir.make<TH2F>(eventsTotalVsNLLSBOverBHistName.c_str(),
      eventsTotalVsNLLSBOverBHistTitle.c_str(),800,-1000,1000,100,0,100);
}
//
void fillSampleSums(SampleSums& sampleSums, int trialIndex, int assumedSignalTracks,
    float nllB, float nllSB, float nllSat, float nllSBFit, float maxLSigFrac,
    float sigFracAssumedThisSlice, float expBgEventsLastBin, float expSigEventsLastBin,
    float expBgEventsThisSlice, float expSigEventsThisSlice, float actEventsLastBin,
    float sigEventsLastBin, float expTotalSignalFraction)
{
  //debug
  //std::cout << "fillSampleSums()" << std::endl;

  // if no sums object for this assumedSignalTracks value, make it
  if(sampleSums.nllSumsMap.find(assumedSignalTracks)==sampleSums.nllSumsMap.end())
  {
    //debug
    //std::cout << "\tno sums object for assumedSignalTracks = " << assumedSignalTracks <<
    //  " so let's make it." << std::endl;
    sampleSums.nllSumsMap.insert(pair<int,NLLSums>(assumedSignalTracks,NLLSums()));
  }

  if(sampleSums.nllSumsMap[assumedSignalTracks].numTrials==0)
  {
    sampleSums.totalSignalFraction = expTotalSignalFraction;
    if(expTotalSignalFraction > 0) // it's an S+B sample
      sampleSums.totalSignalTracks = assumedSignalTracks;
  }

  //debug
  //std::cout << "\tTrialIndex = " << trialIndex << std::endl;

  // make entries for this trial if needed 
  if(sampleSums.nllSumsMap[assumedSignalTracks].numTrials < trialIndex+1)
  {
    //debug
    //std::cout << "\tno entry in the sum vectors for this trial, so pushing back" << std::endl;
    //std::cout << "\tNumTrials = " << sampleSums.nllSumsMap[assumedSignalTracks].numTrials << std::endl;
    //sampleSums.nllSumsMap[assumedSignalTracks].nllSsums.push_back(nllS);
    sampleSums.nllSumsMap[assumedSignalTracks].nllBsums.push_back(nllB);
    sampleSums.nllSumsMap[assumedSignalTracks].nllSBsums.push_back(nllSB);
    sampleSums.nllSumsMap[assumedSignalTracks].nllSatsums.push_back(nllSat);
    sampleSums.nllSumsMap[assumedSignalTracks].nllSBFitsums.push_back(nllSBFit);
    if(maxLSigFrac <= sigFracAssumedThisSlice)
      sampleSums.nllSumsMap[assumedSignalTracks].profileLRsums.push_back(2*(nllSB-nllSBFit));
    else
      sampleSums.nllSumsMap[assumedSignalTracks].profileLRsums.push_back(0);
    sampleSums.nllSumsMap[assumedSignalTracks].pointsPerTrial.push_back(1);
    sampleSums.nllSumsMap[assumedSignalTracks].expectedBgEvtsLastBinSums.push_back(expBgEventsLastBin);
    sampleSums.nllSumsMap[assumedSignalTracks].expectedSigEvtsLastBinSums.push_back(expSigEventsLastBin);
    sampleSums.nllSumsMap[assumedSignalTracks].expectedBgEvtsThisSliceSums.push_back(expBgEventsThisSlice);
    sampleSums.nllSumsMap[assumedSignalTracks].expectedSigEvtsThisSliceSums.push_back(expSigEventsThisSlice);
    sampleSums.nllSumsMap[assumedSignalTracks].actualEvtsLastBinSums.push_back(actEventsLastBin);
    sampleSums.nllSumsMap[assumedSignalTracks].signalEvtsLastBinSums.push_back(sigEventsLastBin);
    //sampleSums.nllSumsMap[assumedSignalTracks].actualEvtsTotalSums.push_back(actEventsTotal);
    //sampleSums.nllSumsMap[assumedSignalTracks].signalEvtsTotalSums.push_back(sigEventsTotal);
    sampleSums.nllSumsMap[assumedSignalTracks].numTrials++;
  }
  else
  {
    //debug
    //std::cout << "\tshould be an entry in the sum vectors for this trial" << std::endl;
    //std::cout << "\tNumTrials = " << sampleSums.nllSumsMap[assumedSignalTracks].numTrials << std::endl;

    //sampleSums.nllSumsMap[assumedSignalTracks].nllSsums[trialIndex]+=nllS;
    sampleSums.nllSumsMap[assumedSignalTracks].nllBsums[trialIndex]+=nllB;
    sampleSums.nllSumsMap[assumedSignalTracks].nllSBsums[trialIndex]+=nllSB;
    sampleSums.nllSumsMap[assumedSignalTracks].nllSatsums[trialIndex]+=nllSat;
    sampleSums.nllSumsMap[assumedSignalTracks].nllSBFitsums[trialIndex]+=nllSBFit;
    if(maxLSigFrac <= sigFracAssumedThisSlice)
    {
      //debug
      //cout << "INFO: adding profile NLL: " << 2*(nllSB-nllSBFit) << " for this trial. " <<
      //  " nllSB: " << nllSB << " nllSBFit: " << nllSBFit << " maxLSigFrac: " << maxLSigFrac <<
      //  " sigFracAssumedThisSlice: " << sigFracAssumedThisSlice << endl;

      sampleSums.nllSumsMap[assumedSignalTracks].profileLRsums[trialIndex]+=(2*(nllSB-nllSBFit));
    }
    sampleSums.nllSumsMap[assumedSignalTracks].pointsPerTrial[trialIndex]++;
    sampleSums.nllSumsMap[assumedSignalTracks].expectedBgEvtsLastBinSums[trialIndex]+=expBgEventsLastBin;
    sampleSums.nllSumsMap[assumedSignalTracks].expectedSigEvtsLastBinSums[trialIndex]+=expSigEventsLastBin;
    sampleSums.nllSumsMap[assumedSignalTracks].expectedBgEvtsThisSliceSums[trialIndex]+=expBgEventsThisSlice;
    sampleSums.nllSumsMap[assumedSignalTracks].expectedSigEvtsThisSliceSums[trialIndex]+=expSigEventsThisSlice;
    sampleSums.nllSumsMap[assumedSignalTracks].actualEvtsLastBinSums[trialIndex]+=actEventsLastBin;
    sampleSums.nllSumsMap[assumedSignalTracks].signalEvtsLastBinSums[trialIndex]+=sigEventsLastBin;
    //sampleSums.nllSumsMap[assumedSignalTracks].actualEvtsTotalSums[trialIndex]+=actEventsTotal;
    //sampleSums.nllSumsMap[assumedSignalTracks].signalEvtsTotalSums[trialIndex]+=sigEventsTotal;
  }
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
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return -1;
  }

  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process);
  fwlite::OutputFiles outputHandler_(process);
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("addLikelihoods");
  //// mass cut to use for the high-p high-Ih (search region) ias dist
  //double massCutIasHighPHighIh_ (ana.getParameter<double>("MassCut"));
  //// dE/dx calibration
  //double dEdx_k (ana.getParameter<double>("dEdx_k"));
  //double dEdx_c (ana.getParameter<double>("dEdx_c"));
  //// definition of sidebands/search region
  //double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  //double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  int minNoM_ (ana.getParameter<int>("MinNoM"));
  double maxEta_ (ana.getParameter<double>("MaxEta"));
  int reportEvery_ = 10000; //TODO: put this in the python
  
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  // sampleSums objects
  SampleSums bgOnlySamplesSums;
  vector<SampleSums> sigAndBgSamplesSumsVec; // one for each sigFrac

  //TODO fix this
  //// for both s+b and b samples, same hists from s+b samples
  //TH2F* sOverRootBExpInDRegionHist = fs.make<TH2F>("sOverRootBExpInDRegion",
  //    "Expected s/sqrt(b) in D region;#eta;nom",12,0,2.4,9,5,23);
  //TH2F* expBgEventsInDRegionHist = fs.make<TH2F>("expBgEventsInDRegion",
  //    "Expected bg tracks in D region;#eta;nom",12,0,2.4,9,5,23);
  //TH2F* expSigEventsInDRegionHist = fs.make<TH2F>("expSigEventsInDRegion",
  //    "Expected sig tracks in D region;#eta;nom",12,0,2.4,9,5,23);
  //TH2F* expBgEventsInLastBinHist = fs.make<TH2F>("expBgEventsInLastBin",
  //    "Expected bg tracks in last bin;#eta;nom",12,0,2.4,9,5,23);
  //TH2F* expSigEventsInLastBinHist = fs.make<TH2F>("expSigEventsInLastBin",
  //    "Expected sig tracks in last bin;#eta;nom",12,0,2.4,9,5,23);
  //TH2F* sOverRootBExpInLastBinHist = fs.make<TH2F>("sOverRootBExpInLastBinRegion",
  //    "Expected s/sqrt(b) in last bin;#eta;nom",12,0,2.4,9,5,23);

  const int numNomSlices = 8; // 5-19
  const int numEtaSlices = 12; // 0-2.4

  unsigned int numSigBackgroundSamples = 0;
  unsigned int lastSigBackIFilePlusOne = 0;

  // loop over files
  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile)
  {
    // open input file (can be located on castor)
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if(!inFile)
      continue;

    std::cout << "-----------------------------" << std::endl;
    std::cout << "Reading file: " << inFile->GetName() << std::endl;


    // now each input file corresponds to one signal fraction
    // all NoM/eta slices are included in the file
    for(int nomIndex = 0; nomIndex < numNomSlices; ++nomIndex)
    {
      for(int etaIndex = 0; etaIndex < numEtaSlices; ++etaIndex)
      {
        int lowerNoM = getLowerNoMFromNoMIndex(nomIndex);
        float lowerEta = getLowerEtaFromEtaIndex(etaIndex);
        if(lowerNoM < minNoM_) continue;
        if(lowerEta+0.2 > maxEta_) continue;

        string expDatasetName = "expectedEventsDatasets/";
        expDatasetName+=getHistNameBeg(lowerNoM,lowerEta);
        expDatasetName+="expectedEvents";
        RooDataSet* thisExpDataSet = (RooDataSet*) inFile->Get(expDatasetName.c_str());
        if(!thisExpDataSet)
        {
          cout << "\tUnable to find roodataset called " << expDatasetName << endl;
          continue;
        }
        string obsDatasetName ="observedEventsDatasets/";
        obsDatasetName+= getHistNameBeg(lowerNoM,lowerEta);
        obsDatasetName+="observedEvents";
        RooDataSet* thisObsDataSet = (RooDataSet*) inFile->Get(obsDatasetName.c_str());
        if(!thisObsDataSet)
        {
          cout << "\tUnable to find roodataset called " << obsDatasetName << endl;
          continue;
        }
        string nllDatasetName ="nllDatasets/";
        nllDatasetName+=getHistNameBeg(lowerNoM,lowerEta);
        nllDatasetName+="nllValues";
        RooDataSet* thisNllDataSet = (RooDataSet*) inFile->Get(nllDatasetName.c_str());
        if(!thisNllDataSet)
        {
          cout << "\tUnable to find roodataset called " << nllDatasetName << endl;
          continue;
        }
        // now we have the exp/obs/nll datasets for this eta/NoM

        cout << "\tlooking at lowerNoM: " << lowerNoM << " lowerEta: " << lowerEta << endl;

        // load up the expected variables
        const RooArgSet* expArgSet = thisExpDataSet->get();
        RooRealVar* expBgEventsLastBin = (RooRealVar*)expArgSet->find(expectedBackgroundEventsInLastBinRooVar.GetName());
        RooRealVar* expSigEventsLastBin = (RooRealVar*)expArgSet->find(expectedSignalEventsInLastBinRooVar.GetName());
        RooRealVar* expBgEventsThisSlice = (RooRealVar*)expArgSet->find(expectedBackgroundEventsThisSliceRooVar.GetName());
        RooRealVar* expSigEventsThisSlice = (RooRealVar*)expArgSet->find(expectedSignalEventsThisSliceRooVar.GetName());
        RooRealVar* expTotalSignalFraction = (RooRealVar*)expArgSet->find(expectedTotalSignalFractionRooVar.GetName());
        //RooRealVar* numberOfTrials = (RooRealVar*)expArgSet->find(numberOfTrialsRooVar.GetName());
        thisExpDataSet->get(0);
        // load up the obs variables
        const RooArgSet* obsArgSet = thisObsDataSet->get();
        RooRealVar* actEventsLastBin = (RooRealVar*)obsArgSet->find(actualEventsInLastBinRooVar.GetName());
        RooRealVar* sigEventsLastBin = (RooRealVar*)obsArgSet->find(signalEventsInLastBinRooVar.GetName());
        // load up the nll variables
        const RooArgSet* nllArgSet = thisNllDataSet->get();
        //RooRealVar* sigNLL = (RooRealVar*)nllArgSet->find(sigNLLRooVar.GetName());
        RooRealVar* backNLL = (RooRealVar*)nllArgSet->find(backNLLRooVar.GetName());
        RooRealVar* sigBackNLL = (RooRealVar*)nllArgSet->find(sigBackNLLRooVar.GetName());
        RooRealVar* satModelNLL = (RooRealVar*)nllArgSet->find(satModelNLLRooVar.GetName());
        RooRealVar* sigBackFitNLL = (RooRealVar*)nllArgSet->find(sigBackFitNLLRooVar.GetName());
        RooRealVar* maxLSigFrac = (RooRealVar*)nllArgSet->find(maxLSigFracRooVar.GetName());
        RooRealVar* assumedTotalSigTracks = (RooRealVar*)nllArgSet->find(assumedTotalSigTracksRooVar.GetName());
        RooRealVar* assumedSigTracksThisSlice = (RooRealVar*)nllArgSet->find(assumedSigTracksThisSliceRooVar.GetName());
        RooRealVar* trialIndex = (RooRealVar*)nllArgSet->find(trialIndexRooVar.GetName());

        int numTotalSignalTracksThisFile = getNumTotalSignalTracksInSamplesThisFile(inFile->GetName());
        bool isSignalAndBackground = false;
        if(expTotalSignalFraction->getVal() > 0)
        {
          isSignalAndBackground = true;
          //debug
          //cout << "iFile+1: " << iFile+1 << " lastSigBackIFile: " << lastSigBackIFilePlusOne << 
          //  " numSigBackgroundSamples: " << numSigBackgroundSamples << endl;
          if(iFile+1 > lastSigBackIFilePlusOne)
          {
            numSigBackgroundSamples++;
            lastSigBackIFilePlusOne = iFile+1;
            //debug
          //cout << "(2) iFile: " << iFile+1 << " lastSigBackIFile: " << lastSigBackIFilePlusOne << 
          //  " numSigBackgroundSamples: " << numSigBackgroundSamples << endl;
          }
        }
        //debug
        //cout << "expTotalSignalFraction" << expTotalSignalFraction->getVal() << endl;
        //cout << "isSignalAndBackground? " << isSignalAndBackground << endl;

        //// get the PDFs
        //string bgHistPdfName = "iasBackgroundPredictions/";
        //bgHistPdfName+=getHistNameBeg(lowerNoM,lowerEta);
        //bgHistPdfName+="iasPredictionVarBinHistHistPdfBG";
        //RooHistPdf* backgroundPredictionHistPdf = (RooHistPdf*)inFile->Get(bgHistPdfName.c_str());
        //if(!backgroundPredictionHistPdf)
        //{
        //  cout << "Unable to find roohistPdf " << bgHistPdfName << " in this file: " <<
        //    inFile->GetName() << endl;
        //  continue;
        //}
        //string sigHistPdfName = "iasSignalPredictions/";
        //sigHistPdfName+=getHistNameBeg(lowerNoM,lowerEta);
        //sigHistPdfName+="iasSignalVarBinHistHistPdfSig";
        //RooHistPdf* signalPredictionHistPdf = (RooHistPdf*)inFile->Get(sigHistPdfName.c_str());
        //if(!signalPredictionHistPdf)
        //{
        //  cout << "Unable to find roohistPdf " << sigHistPdfName << " in this file: " <<
        //    inFile->GetName() << endl;
        //  continue;
        //}
        //RooRealVar fsig("fsig","signal frac",0,1);
        //RooHistPdf* iasBackgroundHistPdfForSBModel = (RooHistPdf*)backgroundPredictionHistPdf->Clone();
        //RooAddPdf sigBackPdf("sigBackPdf","sigBackPdf",RooArgList(*signalPredictionHistPdf,
        //      *iasBackgroundHistPdfForSBModel),fsig);

        //debug
        //cout << "Looping over trials" << endl;

        // loop over the trials
        int nllCounter = 0;
        thisNllDataSet->get(0);
        for(int i=0; i < thisObsDataSet->numEntries(); ++i)
        {
          if(reportEvery_!=0 ? ((i+1)>0 && (i+1)%reportEvery_==0) : false)
            cout << "  getting  trial: " << i+1 << " of " << thisObsDataSet->numEntries() << endl;
          // get the obs data
          thisObsDataSet->get(i);
          // look at all the different assumed signal tracks scenarios in each sample

          //debug
          //if(i>995)
          //  cout << "(loop over events) TrialIndex = " << trialIndex->getVal() << " i = " << i << " nllCounter = " << nllCounter << endl;

          while(trialIndex->getVal()==i && nllCounter < thisNllDataSet->numEntries())
          {
            //debug
            //if(i>995)
            //  cout << "(while) TrialIndex = " << trialIndex->getVal() << " i = " << i << " nllCounter = " << nllCounter << endl;

            // compute signal fraction this slice
            float assumedSignalFractionThisSlice = assumedSigTracksThisSlice->getVal() /
              (double)(assumedSigTracksThisSlice->getVal()+expBgEventsThisSlice->getVal());

            if(!isSignalAndBackground)
            {

              fillSampleSums(bgOnlySamplesSums,i,assumedTotalSigTracks->getVal(),
                  backNLL->getVal(),sigBackNLL->getVal(),satModelNLL->getVal(),
                  sigBackFitNLL->getVal(),maxLSigFrac->getVal(),
                  assumedSignalFractionThisSlice,expBgEventsLastBin->getVal(),expSigEventsLastBin->getVal(),
                  expBgEventsThisSlice->getVal(),expSigEventsThisSlice->getVal(),actEventsLastBin->getVal(),
                  sigEventsLastBin->getVal(),expTotalSignalFraction->getVal());
              //debug
              if(i==0 && assumedTotalSigTracks->getVal() == 11)
                cout << "BG only (assumed sig tracks 11): " << " slice eta = " << lowerEta << " lower NoM = " <<
                  lowerNoM << " sigBackFitNLL = " << sigBackFitNLL->getVal() <<
                  " sigBackNLL = " << sigBackNLL->getVal() <<
                  " maxLSigFrac = " << maxLSigFrac->getVal() << endl;
            }
            else 
            {
              //debug
              //cout << "size of sigAndBgSamplesSumsVec: " << sigAndBgSamplesSumsVec.size() <<
              //  " numSigBackgroundSamples: " << numSigBackgroundSamples <<
              //  "  " << endl;

              // only make an entry for the assumed S+B fraction = actual gen S+B fraction
              if(numTotalSignalTracksThisFile == assumedTotalSigTracks->getVal())
              {
                //debug
                //cout << "numTotalSignalTracksThisFile = " << numTotalSignalTracksThisFile <<
                //  " assumedTotalSigTracks here = " << assumedTotalSigTracks->getVal() << endl;
                if(sigAndBgSamplesSumsVec.size() < numSigBackgroundSamples)
                  sigAndBgSamplesSumsVec.push_back(SampleSums());
                //debug
                //cout << "fill SB sample sums for element: " << sigAndBgSamplesSumsVec.size()-1 <<
                //  " trial: " << i << endl;

                fillSampleSums(sigAndBgSamplesSumsVec[sigAndBgSamplesSumsVec.size()-1],
                    i,assumedTotalSigTracks->getVal(),
                    backNLL->getVal(),sigBackNLL->getVal(),satModelNLL->getVal(),
                    sigBackFitNLL->getVal(),maxLSigFrac->getVal(),
                    assumedSignalFractionThisSlice,expBgEventsLastBin->getVal(),expSigEventsLastBin->getVal(),
                    expBgEventsThisSlice->getVal(),expSigEventsThisSlice->getVal(),actEventsLastBin->getVal(),
                    sigEventsLastBin->getVal(),expTotalSignalFraction->getVal());
              //debug
              if(i==0 && assumedTotalSigTracks->getVal() == 11)
                cout << "SIG+BG: (assumed sig tracks 11)" << " slice eta = " << lowerEta << " lower NoM = " <<
                  lowerNoM << " sigBackFitNLL = " << sigBackFitNLL->getVal() <<
                  " sigBackNLL = " << sigBackNLL->getVal() <<
                  " maxLSigFrac = " << maxLSigFrac->getVal() << endl;
              }
            }

            nllCounter++;
            thisNllDataSet->get(nllCounter);
          }


          if(i==0)
          {
            //TODO FIXME fill these
            //if(isSignalAndBackground)
            //{
            //  expBgEventsInLastBinHist->Fill(lowerEta+0.1,lowerNoM,expBgEventsLastBin->getVal());
            //  expSigEventsInLastBinHist->Fill(lowerEta+0.1,lowerNoM,expSigEventsLastBin->getVal());
            //  expBgEventsInDRegionHist->Fill(lowerEta+0.1,lowerNoM,expBgEventsTotal->getVal());
            //  expSigEventsInDRegionHist->Fill(lowerEta+0.1,lowerNoM,expSigEventsTotal->getVal());
            //  if(expBgEventsLastBin->getVal() > 0)
            //    sOverRootBExpInLastBinHist->Fill(lowerEta,lowerNoM,expSigEventsLastBin->getVal()/(double)sqrt(expBgEventsLastBin->getVal()));
            //  if(expBgEventsTotal->getVal() > 0)
            //    sOverRootBExpInDRegionHist->Fill(lowerEta,lowerNoM,expSigEventsTotal->getVal()/(double)sqrt(expBgEventsTotal->getVal()));
            //}

            cout << "\ttrial: " << i << endl;
            cout << "\tlowerNoM: " << lowerNoM << " lowerEta: " << lowerEta << endl;
            //cout << "\t nllSonly: " << nllS->getVal()<< endl;
            cout << "\t nllBOnly: " << backNLL->getVal() << " nllSB: " << sigBackNLL->getVal()<< endl;
            cout << "\t nllSBOverB: " << 2*(sigBackNLL->getVal()-backNLL->getVal())<< endl;
            cout << "\t nllBSat: " << 2*(backNLL->getVal()-satModelNLL->getVal())<< endl;
            cout << "\t eventsInLastBin: " << actEventsLastBin->getVal()<< endl;
            cout << "\t expectedBgEventsInLastBin: " << expBgEventsLastBin->getVal()<< endl;
            cout << "\t expectedSigEventsInLastBin: " << expSigEventsLastBin->getVal()<< endl;
          }
        }

        // delete pointers
        delete thisExpDataSet;
        delete thisObsDataSet;
        delete thisNllDataSet;
        //delete expArgSet;
        //delete expBgEventsLastBin;
        //delete expSigEventsLastBin;
        //delete expBgEventsThisSlice;
        //delete expSigEventsThisSlice;
        //delete expTotalSignalFraction;
       // //delete numberOfTrials;
       // delete obsArgSet;
       // delete actEventsLastBin;
       // delete sigEventsLastBin;
       // delete nllArgSet;
       // delete backNLL;
       // delete sigBackNLL;
       // delete satModelNLL;
       // delete sigBackFitNLL;
       // delete maxLSigFrac;
       // delete assumedTotalSigTracks;
       // delete trialIndex;
        //delete backgroundPredictionHistPdf;
        //delete signalPredictionHistPdf;
        //delete iasBackgroundHistPdfForSBModel;
      }
    }
  }


  if(bgOnlySamplesSums.nllSumsMap[0].expectedBgEvtsLastBinSums.size() < 1)
  {
    std::cout << "ERROR: didn't find any information from any B-only input file.  Stopping." << std::endl;
    return -3;
  }
  //if(sigAndBgSamplesSumsVec[0].nllSumsMap[0].expectedBgEvtsLastBinSums.size() < 1)
  if(sigAndBgSamplesSumsVec.size() < 1)
  {
    std::cout << "ERROR: didn't find any information from any S+B input file.  Stopping." << std::endl;
    //cout << " INFO: numTrials = " << sigAndBgSamplesSumsVec[0].nllSumsMap[0].numTrials <<
    //  " totalSignalFraction = " << sigAndBgSamplesSumsVec[0].totalSignalFraction << 
    //  " totalSignalTracks = " << sigAndBgSamplesSumsVec[0].totalSignalTracks << endl;
    //  //" nllBSums[0] = " << sigAndBgSamplesSumsVec[0].nllSumsMap[0].nllBsums[0] << endl;
    return -2;
  }


  cout << "INFO: assumed sig tracks 11: trial 0: expected BG #events in last bin sum=" <<
    bgOnlySamplesSums.nllSumsMap[11].expectedBgEvtsLastBinSums[0] << endl;
  cout << "INFO: assumed sig tracks 11: trial 0: expected BG #events this slice sum=" <<
    bgOnlySamplesSums.nllSumsMap[11].expectedBgEvtsThisSliceSums[0] << endl;
  // loop over different S+B samples read in
  for(vector<SampleSums>::const_iterator itr = sigAndBgSamplesSumsVec.begin();
      itr != sigAndBgSamplesSumsVec.end(); ++itr)
  {
    SampleSums thisSBSampleSums = *itr;
    cout << "INFO: S+B sample signal fraction = " << 
      thisSBSampleSums.totalSignalFraction << endl;
    cout << "\tINFO: S+B sample signal tracks total = " << thisSBSampleSums.totalSignalTracks << endl;
    cout << "\tINFO: trial 0: expected Sig #events in last bin sum=" <<
      thisSBSampleSums.nllSumsMap.begin()->second.expectedSigEvtsLastBinSums[0] << endl;
    cout << "\tINFO: trial 0: expected Sig #events this slice sum=" <<
      thisSBSampleSums.nllSumsMap.begin()->second.expectedSigEvtsThisSliceSums[0] << endl;
  }

  // hists for BG only samples
  TFileDirectory backgroundOnlyDir = fs.mkdir("backgroundOnlySamples");
  map<int,SampleHistos> backgroundSampleHistosMap;

  map<int, vector<double> > qMuBgOnlyMap;
  map<int, float> medianQMuZeroMap;
  // plot histograms -- b only first
  for(map<int,NLLSums>::const_iterator nllMapItr = bgOnlySamplesSums.nllSumsMap.begin();
      nllMapItr != bgOnlySamplesSums.nllSumsMap.end(); ++nllMapItr)
  {
    cout << "looking at background sample with assumed signal tracks = " << nllMapItr->first <<
      endl;

    backgroundSampleHistosMap.insert(pair<int,SampleHistos>(nllMapItr->first,SampleHistos()));
    initHistos(nllMapItr->first,backgroundSampleHistosMap[nllMapItr->first],false,backgroundOnlyDir);
    SampleHistos backgroundSampleHistos = backgroundSampleHistosMap[nllMapItr->first];
    qMuBgOnlyMap.insert(pair<int,vector<double> >(nllMapItr->first,vector<double>()));

    // loop over trials
    for(unsigned int i=0; i<nllMapItr->second.nllBsums.size(); ++i)
    {
      //double nllSsum = nllMapItr->second.nllSsums[i];
      double nllBsum = nllMapItr->second.nllBsums[i];
      double nllSatsum = nllMapItr->second.nllSatsums[i];
      double nllSBsum = nllMapItr->second.nllSBsums[i];
      double profileLRsum = nllMapItr->second.profileLRsums[i];
      double pointsPerTrial = nllMapItr->second.pointsPerTrial[i];
      double eventsInLastBin = nllMapItr->second.actualEvtsLastBinSums[i];
      //double totalEvts = nllMapItr->second.actualEvtsTotalSums[i];

      //backgroundSampleHistos.nllSOnlyHist->Fill(nllSsum);
      backgroundSampleHistos.nllBOnlyHist->Fill(nllBsum);
      backgroundSampleHistos.nllSatOnlyHist->Fill(nllSatsum);
      backgroundSampleHistos.nllSBHist->Fill(nllSBsum);
      backgroundSampleHistos.nllSBOverBHist->Fill(2*(nllSBsum-nllBsum));
      backgroundSampleHistos.nllBSatHist->Fill(2*(nllBsum-nllSatsum));
      backgroundSampleHistos.profileLRHist->Fill(profileLRsum);
      backgroundSampleHistos.pointsPerTrialHist->Fill(pointsPerTrial);
      backgroundSampleHistos.eventsInLastBinVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),eventsInLastBin);
      //backgroundSampleHistos.eventsTotalVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),totalEvts);

      //cout << "Pushing back profileLRsum = " << profileLRsum << endl;
      qMuBgOnlyMap[nllMapItr->first].push_back(profileLRsum);
    }

    // median q_mu
    double medianQmuZero = 0;
    sort(qMuBgOnlyMap[nllMapItr->first].begin(),qMuBgOnlyMap[nllMapItr->first].end());
    medianQmuZero = qMuBgOnlyMap[nllMapItr->first].at(qMuBgOnlyMap[nllMapItr->first].size()/2+1);
    medianQMuZeroMap.insert(pair<int,float>(nllMapItr->first,medianQmuZero));
    cout << "Median q_mu|0 value: " << medianQmuZero << endl;
  
  }

  // hists for S+B samples
  vector<SampleHistos> signalAndBackgroundSampleHistosVec;
  // loop over different S+B samples read in
  for(vector<SampleSums>::const_iterator itr = sigAndBgSamplesSumsVec.begin();
      itr != sigAndBgSamplesSumsVec.end(); ++itr)
  {
    SampleSums thisSBSampleSums = *itr;
    // each SampleSums should only have one element in the nll map
    if(thisSBSampleSums.nllSumsMap.size() != 1)
      cout << "ERROR: thisSBSampleSums nllSumsMap size is : " <<
        thisSBSampleSums.nllSumsMap.size() << " which is not 1" << endl;
    map<int,NLLSums>::iterator nllMapItr = thisSBSampleSums.nllSumsMap.begin();

    signalAndBackgroundSampleHistosVec.push_back(SampleHistos());
    string histoDirName = "signalAndBackgroundSamplesHists_sigTracks";
    histoDirName+=intToString(thisSBSampleSums.totalSignalTracks);
    TFileDirectory signalBackgroundDir = fs.mkdir(histoDirName.c_str());
    initHistos(nllMapItr->first,signalAndBackgroundSampleHistosVec.back(),true,signalBackgroundDir);

    // looping over trials
    for(unsigned int i=0; i<nllMapItr->second.nllBsums.size(); ++i)
    {
      //double nllSsum = nllMapItr->second.nllSsums[i];
      double nllBsum = nllMapItr->second.nllBsums[i];
      double nllSatsum = nllMapItr->second.nllSatsums[i];
      double nllSBsum = nllMapItr->second.nllSBsums[i];
      double profileLRsum = nllMapItr->second.profileLRsums[i];
      double pointsPerTrial = nllMapItr->second.pointsPerTrial[i];
      double eventsInLastBin = nllMapItr->second.actualEvtsLastBinSums[i];
      //double totalEvts = nllMapItr->second.actualEvtsTotalSums[i];
      //std::cout << "profileLR sum: " << nllMapItr->second.nllSsums[i] << std::endl;

      //signalAndBackgroundSampleHistosVec.back().nllSOnlyHist->Fill(nllSsum);
      signalAndBackgroundSampleHistosVec.back().nllBOnlyHist->Fill(nllBsum);
      signalAndBackgroundSampleHistosVec.back().nllSatOnlyHist->Fill(nllSatsum);
      signalAndBackgroundSampleHistosVec.back().nllSBHist->Fill(nllSBsum);
      signalAndBackgroundSampleHistosVec.back().nllSBOverBHist->Fill(2*(nllSBsum-nllBsum));
      signalAndBackgroundSampleHistosVec.back().nllBSatHist->Fill(2*(nllBsum-nllSatsum));
      signalAndBackgroundSampleHistosVec.back().profileLRHist->Fill(profileLRsum);
      signalAndBackgroundSampleHistosVec.back().pointsPerTrialHist->Fill(pointsPerTrial);
      signalAndBackgroundSampleHistosVec.back().eventsInLastBinVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),eventsInLastBin);
      //signalAndBackgroundSampleHistosVec.back().eventsTotalVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),totalEvts);
    }

    // p-value
    TH1F* sigBackProfileLRHist = signalAndBackgroundSampleHistosVec.back().profileLRHist;
    map<int,float>::const_iterator itr = medianQMuZeroMap.find(nllMapItr->first);
    if(itr==medianQMuZeroMap.end())
      cout << "ERROR: number of signal tracks = " << nllMapItr->first << 
        " not found in the background QMuZero map!" << endl;

    int medianQMuZeroBin = sigBackProfileLRHist->FindBin(itr->second);
    int maxBin = sigBackProfileLRHist->GetNbinsX();
    double pvalue = sigBackProfileLRHist->Integral(medianQMuZeroBin,maxBin)/sigBackProfileLRHist->Integral();
    std::cout << "signal fraction: " << thisSBSampleSums.totalSignalFraction
      << " total signal tracks: " << thisSBSampleSums.totalSignalTracks
      << " p-value : " << pvalue
      << " this upper limit on signal fraction has CL = " << 1-pvalue << std::endl;
  }


}


