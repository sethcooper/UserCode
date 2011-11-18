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

struct SampleSums
{
  std::vector<double> nllSsums;
  std::vector<double> nllBsums;
  std::vector<double> nllSBsums;
  std::vector<double> nllSatsums;
  std::vector<double> nllSBFitsums;
  std::vector<double> profileLRsums;
  std::vector<int> pointsPerTrial;
  std::vector<double> expectedBgEvtsLastBinSums;
  std::vector<double> expectedSigEvtsLastBinSums;
  std::vector<double> expectedBgEvtsTotalSums;
  std::vector<double> expectedSigEvtsTotalSums;
  std::vector<int> actualEvtsLastBinSums;
  std::vector<int> signalEvtsLastBinSums;
  std::vector<int> actualEvtsTotalSums;
  std::vector<int> signalEvtsTotalSums;

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
int numTotalSignalTracksInSamplesThisFile(std::string str)
{
  int posSigTracks = str.find("sigTracks");
  int posBgTracks = str.find("bgTracks_");
  std::string numSigTracks = str.substr(posBgTracks+9,posSigTracks-posBgTracks+9);
  return atoi(numSigTracks.c_str());
}
//
int getNoMSliceFromNoM(int NoM)
{
  int nomSlice = NoM-5;
  return nomSlice/2;
}
//
int getNoMFromNoMSlice(int nomSlice)
{
  return 2*(nomSlice)+5;
}
//
void initHistos(SampleHistos* histos, bool isSignal, TFileDirectory& tFileDir)
{
  // NLL B/sat hist
  std::string nllBSatHistName="nllBSatBGOnlyHist";
  std::string nllBSatHistTitle = "-2*ln(B/sat), bg only samples";
  // NLL sat only hist
  std::string nllSatOnlyHistName="nllSatonlyBGOnlyHist";
  std::string nllSatOnlyHistTitle = "NLL sat only, bg only samples";
  // NLL B only hist
  std::string nllBOnlyHistName="nllBonlyBGOnlyHist";
  std::string nllBOnlyHistTitle = "NLL B only, bg only samples";
  // NLL S only hist
  std::string nllSonlyHistName="nllSonlyBGOnlyHist";
  std::string nllSonlyHistTitle = "NLL S only, bg only samples";
  // NLL SB hist
  std::string nllSBHistName="nllSBBGOnlyHist";
  std::string nllSBHistTitle = "NLL SB, bg only samples";
  // NLL S+B/B
  std::string nllSBOverBHistName="lepQBGOnlyHist";
  std::string nllSBOverBHistTitle = "-2*ln(L(s+b)/L(b)), bg only samples;-2 ln Q";
  // profile LR
  std::string profileLRHistName="profileLRBGOnlyHist";
  std::string profileLRHistTitle = "-2*ln(L(#mu)/L(#hat{#mu})), bg only samples;q_{#mu}";
  // points per trial
  std::string pointsPerTrialHistName="pointsPerTrialBGOnlyHist";
  std::string pointsPerTrialHistTitle="Points per trial, bg only samples";
  // events in last bin vs. NLL SB/B
  std::string eventsInLastBinVsNLLSBOverBHistName="eventsInLastBinVsNLLSBOverBBGOnly";
  std::string eventsInLastBinVsNLLSBOverBHistTitle="Events in last bin vs. NLL -2*ln(SB/B), bg only samples";
  // events total vs. NLL SB/B
  std::string eventsTotalVsNLLSBOverBHistName="eventsInTotalVsNLLSBOverBBGOnly";
  std::string eventsTotalVsNLLSBOverBHistTitle="Events total vs. NLL -2*ln(SB/B), bg only samples";
  if(isSignal)
  {
    nllBSatHistName="nllBSatSPlusBHist";
    nllBSatHistTitle = "-2*ln(B/sat), s+b samples";
    nllSatOnlyHistName="nllSatonlySPlusBHist";
    nllSatOnlyHistTitle = "NLL sat only, s+b samples";
    nllBOnlyHistName="nllBonlySPlusBHist";
    nllBOnlyHistTitle = "NLL B only, s+b samples";
    nllSonlyHistName="nllSonlySPlusBHist";
    nllSonlyHistTitle = "NLL S only, s+b samples";
    nllSBHistName="nllSBSPlusBHist";
    nllSBHistTitle = "NLL SB, s+b samples";
    nllSBOverBHistName="lepQSPlusBHist";
    nllSBOverBHistTitle = "-2*ln(L(s+b)/L(b)), s+b samples;-2 ln Q";
    profileLRHistName="profileLRSPlusBHist";
    profileLRHistTitle = "-2*ln(L(#mu)/L(#hat{#mu})), s+b samples;q_{#mu}";
    pointsPerTrialHistName="pointsPerTrialSPlusBHist";
    pointsPerTrialHistTitle="Points per trial, s+b samples";
    eventsInLastBinVsNLLSBOverBHistName="eventsInLastBinVsNLLSBOverBSPlusB";
    eventsInLastBinVsNLLSBOverBHistTitle="Events in last bin vs. NLL -2*ln(SB/B), s+b samples";
    eventsTotalVsNLLSBOverBHistName="eventsInTotalVsNLLSBOverBSPlusB";
    eventsTotalVsNLLSBOverBHistTitle="Events total vs. NLL -2*ln(SB/B), s+b samples";
  }

  // init hists
  histos->nllBSatHist = tFileDir.make<TH1F>(nllBSatHistName.c_str(),nllBSatHistTitle.c_str(),100,0,1000);
  histos->nllSatOnlyHist = tFileDir.make<TH1F>(nllSatOnlyHistName.c_str(),nllSatOnlyHistTitle.c_str(),10000,-5000,5000);
  histos->nllBOnlyHist = tFileDir.make<TH1F>(nllBOnlyHistName.c_str(),nllBOnlyHistTitle.c_str(),10000,-5000,5000);
  histos->nllSOnlyHist = tFileDir.make<TH1F>(nllSonlyHistName.c_str(),nllSonlyHistTitle.c_str(),10000,-5000,5000);
  histos->nllSBHist = tFileDir.make<TH1F>(nllSBHistName.c_str(),nllSBHistTitle.c_str(),10000,-5000,5000);
  histos->nllSBOverBHist = tFileDir.make<TH1F>(nllSBOverBHistName.c_str(),nllSBOverBHistTitle.c_str(),800,-1000,1000);
  histos->profileLRHist = tFileDir.make<TH1F>(profileLRHistName.c_str(),profileLRHistTitle.c_str(),2000,-100,100);
  histos->pointsPerTrialHist = tFileDir.make<TH1F>(pointsPerTrialHistName.c_str(),
      pointsPerTrialHistTitle.c_str(),20000,0,20000);
  histos->eventsInLastBinVsNLLSBOverBHist = tFileDir.make<TH2F>(eventsInLastBinVsNLLSBOverBHistName.c_str(),
      eventsInLastBinVsNLLSBOverBHistTitle.c_str(),800,-1000,1000,100,0,100);
  histos->eventsTotalVsNLLSBOverBHist = tFileDir.make<TH2F>(eventsTotalVsNLLSBOverBHistName.c_str(),
      eventsTotalVsNLLSBOverBHistTitle.c_str(),800,-1000,1000,100,0,100);
}
//
void fillSampleSums(SampleSums* sampleSums,const RooArgSet& nllArgSet,
    const RooArgSet& expObsArgSet, int sampleNum)
{
  //TODO: consolidate these in a single place (something like common functions)
  // nll values
  RooRealVar sigNLLRooVar("sigNLLRooVar","signal NLL",-100000,100000);
  RooRealVar backNLLRooVar("backNLLRooVar","background NLL",-100000,100000);
  RooRealVar sigBackNLLRooVar("sigBackNLLRooVar","signal+background NLL",-100000,100000);
  RooRealVar satModelNLLRooVar("satModelNLLRooVar","sat model NLL",-100000,100000);
  RooRealVar sigBackFitNLLRooVar("sigBackFitNLLRooVar","signal+background ML fit",-100000,100000);
  RooRealVar maxLSigFracRooVar("maxLSigFracRooVar","max L fit signal frac",0,1);
  // exp events (tracks)
  RooRealVar expectedBackgroundEventsInLastBinRooVar("expectedBackgroundEventsInLastBinRooVar","expected bg events in last bin",0,20000);
  RooRealVar expectedSignalEventsInLastBinRooVar("expectedSignalEventsInLastBinRooVar","expected signal events in last bin",0,20000);
  RooRealVar expectedBackgroundEventsTotalRooVar("expectedBackgroundEventsTotalRooVar","expected bg events",0,50000);
  RooRealVar expectedSignalEventsTotalRooVar("expectedSignalEventsTotalRooVar","expected signal events",0,20000);
  RooRealVar actualEventsInLastBinRooVar("actualEventsInLastBinRooVar","actual events in last bin",0,20000);
  RooRealVar signalEventsInLastBinRooVar("signalEventsInLastBinRooVar","signal events in last bin",0,0,20000);

  RooRealVar nllS = *(RooRealVar*)nllArgSet.find(sigNLLRooVar.GetName());
  RooRealVar nllB = *(RooRealVar*)nllArgSet.find(backNLLRooVar.GetName());
  RooRealVar nllSB = *(RooRealVar*)nllArgSet.find(sigBackNLLRooVar.GetName());
  RooRealVar nllSat = *(RooRealVar*)nllArgSet.find(satModelNLLRooVar.GetName());
  RooRealVar nllSBFit = *(RooRealVar*)nllArgSet.find(sigBackFitNLLRooVar.GetName());
  RooRealVar maxLSigFrac = *(RooRealVar*)nllArgSet.find(maxLSigFracRooVar.GetName());
  RooRealVar expBgEventsLastBin = *(RooRealVar*)expObsArgSet.find(expectedBackgroundEventsInLastBinRooVar.GetName());
  RooRealVar expSigEventsLastBin = *(RooRealVar*)expObsArgSet.find(expectedSignalEventsInLastBinRooVar.GetName());
  RooRealVar expBgEventsTotal = *(RooRealVar*)expObsArgSet.find(expectedBackgroundEventsTotalRooVar.GetName());
  RooRealVar expSigEventsTotal = *(RooRealVar*)expObsArgSet.find(expectedSignalEventsTotalRooVar.GetName());
  RooRealVar actEventsLastBin = *(RooRealVar*)expObsArgSet.find(actualEventsInLastBinRooVar.GetName());
  RooRealVar sigEventsLastBin = *(RooRealVar*)expObsArgSet.find(signalEventsInLastBinRooVar.GetName());

  double sigFracExp = expSigEventsTotal.getVal()/(double)(expSigEventsTotal.getVal()+expBgEventsTotal.getVal());

  if(sampleSums->nllSsums.size() < (unsigned int) sampleNum+1)
  {
    // make an entry for this trial if there isn't one already
    sampleSums->nllSsums.push_back(nllS.getVal());
    sampleSums->nllBsums.push_back(nllB.getVal());
    sampleSums->nllSBsums.push_back(nllSB.getVal());
    sampleSums->nllSatsums.push_back(nllSat.getVal());
    sampleSums->nllSBFitsums.push_back(nllSBFit.getVal());
    if(maxLSigFrac.getVal() <= sigFracExp)
      sampleSums->profileLRsums.push_back(2*(nllSB.getVal()-nllSBFit.getVal()));
    else
      sampleSums->profileLRsums.push_back(0);
    sampleSums->pointsPerTrial.push_back(1);
    sampleSums->expectedBgEvtsLastBinSums.push_back(expBgEventsLastBin.getVal());
    sampleSums->expectedSigEvtsLastBinSums.push_back(expSigEventsLastBin.getVal());
    sampleSums->expectedBgEvtsTotalSums.push_back(expBgEventsTotal.getVal());
    sampleSums->expectedSigEvtsTotalSums.push_back(expSigEventsTotal.getVal());
    sampleSums->actualEvtsLastBinSums.push_back(actEventsLastBin.getVal());
    sampleSums->signalEvtsLastBinSums.push_back(sigEventsLastBin.getVal());
    //sampleSums->actualEvtsTotalSums.push_back(actEventsTotal.getVal());
    //sampleSums->signalEvtsTotalSums.push_back(sigEventsTotal.getVal());
  }
  else
  {
    sampleSums->nllSsums[sampleNum]+=nllS.getVal();
    sampleSums->nllBsums[sampleNum]+=nllB.getVal();
    sampleSums->nllSBsums[sampleNum]+=nllSB.getVal();
    sampleSums->nllSatsums[sampleNum]+=nllSat.getVal();
    sampleSums->nllSBFitsums[sampleNum]+=nllSBFit.getVal();
    if(maxLSigFrac.getVal() <= sigFracExp)
      sampleSums->profileLRsums[sampleNum]+=(2*(nllSB.getVal()-nllSBFit.getVal()));
    sampleSums->pointsPerTrial[sampleNum]++;
    sampleSums->expectedBgEvtsLastBinSums[sampleNum]+=expBgEventsLastBin.getVal();
    sampleSums->expectedSigEvtsLastBinSums[sampleNum]+=expSigEventsLastBin.getVal();
    sampleSums->expectedBgEvtsTotalSums[sampleNum]+=expBgEventsTotal.getVal();
    sampleSums->expectedSigEvtsTotalSums[sampleNum]+=expSigEventsTotal.getVal();
    sampleSums->actualEvtsLastBinSums[sampleNum]+=actEventsLastBin.getVal();
    sampleSums->signalEvtsLastBinSums[sampleNum]+=sigEventsLastBin.getVal();
    //sampleSums->actualEvtsTotalSums[sampleNum]+=actEventsTotal.getVal();
    //sampleSums->signalEvtsTotalSums[sampleNum]+=sigEventsTotal.getVal();
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
  

  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  // hists for BG only samples
  SampleHistos* backgroundSampleHistos = new SampleHistos;
  TFileDirectory backgroundOnlyDir = fs.mkdir("backgroundOnlySamples");
  initHistos(backgroundSampleHistos,false,backgroundOnlyDir);
  // hists for S+B samples
  SampleHistos* signalAndBackgroundSampleHistos = new SampleHistos;
  TFileDirectory signalBackgroundDir = fs.mkdir("signalAndBackgroundSamples");
  initHistos(signalAndBackgroundSampleHistos,true,signalBackgroundDir);

  // for both s+b and b samples, same hists from s+b samples
  TH2F* sOverRootBExpInDRegionHist = fs.make<TH2F>("sOverRootBExpInDRegion",
      "Expected s/sqrt(b) in D region;#eta;nom",12,0,2.4,9,5,23);
  TH2F* expBgEventsInDRegionHist = fs.make<TH2F>("expBgEventsInDRegion",
      "Expected bg tracks in D region;#eta;nom",12,0,2.4,9,5,23);
  TH2F* expSigEventsInDRegionHist = fs.make<TH2F>("expSigEventsInDRegion",
      "Expected sig tracks in D region;#eta;nom",12,0,2.4,9,5,23);
  TH2F* expBgEventsInLastBinHist = fs.make<TH2F>("expBgEventsInLastBin",
      "Expected bg tracks in last bin;#eta;nom",12,0,2.4,9,5,23);
  TH2F* expSigEventsInLastBinHist = fs.make<TH2F>("expSigEventsInLastBin",
      "Expected sig tracks in last bin;#eta;nom",12,0,2.4,9,5,23);
  TH2F* sOverRootBExpInLastBinHist = fs.make<TH2F>("sOverRootBExpInLastBinRegion",
      "Expected s/sqrt(b) in last bin;#eta;nom",12,0,2.4,9,5,23);

  SampleSums* bgOnlySamplesSums = new SampleSums;
  SampleSums* sigAndBgSamplesSums = new SampleSums;

  //const int numNomSlices = 9;
  //const int numEtaSlices = 12;

  //TODO: consolidate these in a single place (something like common functions)
  // nll values
  RooRealVar sigNLLRooVar("sigNLLRooVar","signal NLL",-100000,100000);
  RooRealVar backNLLRooVar("backNLLRooVar","background NLL",-100000,100000);
  RooRealVar sigBackNLLRooVar("sigBackNLLRooVar","signal+background NLL",-100000,100000);
  RooRealVar satModelNLLRooVar("satModelNLLRooVar","sat model NLL",-100000,100000);
  RooRealVar sigBackFitNLLRooVar("sigBackFitNLLRooVar","signal+background ML fit",-100000,100000);
  RooRealVar maxLSigFracRooVar("maxLSigFracRooVar","max L fit signal frac",0,1);
  // exp events (tracks)
  RooRealVar expectedBackgroundEventsInLastBinRooVar("expectedBackgroundEventsInLastBinRooVar","expected bg events in last bin",0,20000);
  RooRealVar expectedSignalEventsInLastBinRooVar("expectedSignalEventsInLastBinRooVar","expected signal events in last bin",0,20000);
  RooRealVar expectedBackgroundEventsTotalRooVar("expectedBackgroundEventsTotalRooVar","expected bg events",0,50000);
  RooRealVar expectedSignalEventsTotalRooVar("expectedSignalEventsTotalRooVar","expected signal events",0,20000);
  RooRealVar actualEventsInLastBinRooVar("actualEventsInLastBinRooVar","actual events in last bin",0,20000);
  RooRealVar signalEventsInLastBinRooVar("signalEventsInLastBinRooVar","signal events in last bin",0,0,20000);

  // loop over files
  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile)
  {
    // open input file (can be located on castor)
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if( inFile )
    {
      // figure out nom/eta from filename
      int nom = getNoMFromFilename(std::string(inFile->GetName()));
      int etaSlice = getEtaFromFilename(std::string(inFile->GetName()))/2;
      float eta = etaSlice*0.2;
      if(nom < minNoM_) continue;
      if(eta+0.2 > maxEta_) continue;

      std::cout << "-----------------------------" << std::endl;
      std::cout << "File: " << inFile->GetName() << std::endl;
      //debug
      std::cout << "numTotalSignalTracksInSamplesThisFile: " << 
        numTotalSignalTracksInSamplesThisFile(std::string(inFile->GetName())) <<
        std::endl;

      RooDataSet* thisNLLDataSet = (RooDataSet*) inFile->Get("nllValues");
      if(!thisNLLDataSet)
      {
        cout << "Unable to find roodataset called nllValues in this file: " <<
          inFile->GetName() << endl;
        continue;
      }
      RooDataSet* thisExpObsDataSet = (RooDataSet*) inFile->Get("expectedObservedEvents");
      if(!thisExpObsDataSet)
      {
        cout << "Unable to find roodataset called expectedObservedEvents in this file: " <<
          inFile->GetName() << endl;
        continue;
      }

      bool isSignalAndBackground = true;
      if(numTotalSignalTracksInSamplesThisFile(std::string(inFile->GetName())) <= 0)
        isSignalAndBackground = false;

      const RooArgSet* nllArgSet = thisNLLDataSet->get();
      RooRealVar* nllS = (RooRealVar*)nllArgSet->find(sigNLLRooVar.GetName());
      RooRealVar* nllB = (RooRealVar*)nllArgSet->find(backNLLRooVar.GetName());
      RooRealVar* nllSB = (RooRealVar*)nllArgSet->find(sigBackNLLRooVar.GetName());
      RooRealVar* nllSat = (RooRealVar*)nllArgSet->find(satModelNLLRooVar.GetName());
      RooRealVar* nllSBFit = (RooRealVar*)nllArgSet->find(sigBackFitNLLRooVar.GetName());
      RooRealVar* maxLSigFrac = (RooRealVar*)nllArgSet->find(maxLSigFracRooVar.GetName());
      const RooArgSet* expObsArgSet = thisExpObsDataSet->get();
      RooRealVar* expBgEventsLastBin = (RooRealVar*)expObsArgSet->find(expectedBackgroundEventsInLastBinRooVar.GetName());
      RooRealVar* expSigEventsLastBin = (RooRealVar*)expObsArgSet->find(expectedSignalEventsInLastBinRooVar.GetName());
      RooRealVar* expBgEventsTotal = (RooRealVar*)expObsArgSet->find(expectedBackgroundEventsTotalRooVar.GetName());
      RooRealVar* expSigEventsTotal = (RooRealVar*)expObsArgSet->find(expectedSignalEventsTotalRooVar.GetName());
      RooRealVar* actEventsLastBin = (RooRealVar*)expObsArgSet->find(actualEventsInLastBinRooVar.GetName());
      RooRealVar* sigEventsLastBin = (RooRealVar*)expObsArgSet->find(signalEventsInLastBinRooVar.GetName());

      // loop over dataset (looping over the trials)
      for(int i=0; i < thisNLLDataSet->numEntries(); ++i)
      {
         thisNLLDataSet->get(i);
         thisExpObsDataSet->get(i);

        if(isSignalAndBackground)
          fillSampleSums(sigAndBgSamplesSums,*nllArgSet,*expObsArgSet,i);
        else
          fillSampleSums(bgOnlySamplesSums,*nllArgSet,*expObsArgSet,i);

        if(i==0)
        {
          if(isSignalAndBackground)
          {
            expBgEventsInLastBinHist->Fill(eta+0.1,nom,expBgEventsLastBin->getVal());
            expSigEventsInLastBinHist->Fill(eta+0.1,nom,expSigEventsLastBin->getVal());
            expBgEventsInDRegionHist->Fill(eta+0.1,nom,expBgEventsTotal->getVal());
            expSigEventsInDRegionHist->Fill(eta+0.1,nom,expSigEventsTotal->getVal());
            if(expBgEventsLastBin->getVal() > 0)
              sOverRootBExpInLastBinHist->Fill(eta,nom,expSigEventsLastBin->getVal()/(double)sqrt(expBgEventsLastBin->getVal()));
            if(expBgEventsTotal->getVal() > 0)
              sOverRootBExpInDRegionHist->Fill(eta,nom,expSigEventsTotal->getVal()/(double)sqrt(expBgEventsTotal->getVal()));
          }

          cout << "trial: " << i << endl;
          cout << " file: " << inFile->GetName() << endl;
          cout << "\n\t nllSonly: " << nllS->getVal()<< endl;
          cout << "\n\t nllBOnly: " << nllB->getVal() << " nllSB: " << nllSB->getVal()<< endl;
          cout << "\n\t nllSBOverB: " << 2*(nllSB->getVal()-nllB->getVal())<< endl;
          cout << "\n\t nllBSat: " << 2*(nllB->getVal()-nllSat->getVal())<< endl;
          cout << "\n\t eventsInLastBin: " << actEventsLastBin->getVal()<< endl;
          cout << "\n\t expectedBgEventsInLastBin: " << expBgEventsLastBin->getVal()<< endl;
          cout << "\n\t expectedSigEventsInLastBin: " << expSigEventsLastBin->getVal()<< endl;
            //<< endl;
        }
      }
    }
  }


  if(sigAndBgSamplesSums->expectedBgEvtsLastBinSums.size() < 1)
  {
    std::cout << "ERROR: unable to find any information from any S+B input file.  Stopping." << std::endl;
    return -2;
  }
  if(bgOnlySamplesSums->expectedBgEvtsLastBinSums.size() < 1)
  {
    std::cout << "ERROR: unable to find any information from any B-only input file.  Stopping." << std::endl;
    return -3;
  }


  std::cout << "INFO: expected BG #events in last bin=" << bgOnlySamplesSums->expectedBgEvtsLastBinSums[0]
    << std::endl;
  std::cout << "INFO: expected Sig #events in last bin=" << bgOnlySamplesSums->expectedSigEvtsLastBinSums[0]
    << std::endl;
  std::cout << "INFO: expected BG #events total=" << bgOnlySamplesSums->expectedBgEvtsTotalSums[0]
    << std::endl;
  std::cout << "INFO: expected Sig #events total=" << bgOnlySamplesSums->expectedSigEvtsTotalSums[0]
    << std::endl;


  vector<double> qMuBgOnlyVec;
  // plot histograms -- b only first
  for(unsigned int i=0; i<bgOnlySamplesSums->nllSsums.size(); ++i)
  {
    double nllSsum = bgOnlySamplesSums->nllSsums[i];
    double nllBsum = bgOnlySamplesSums->nllBsums[i];
    double nllSatsum = bgOnlySamplesSums->nllSatsums[i];
    double nllSBsum = bgOnlySamplesSums->nllSBsums[i];
    double profileLRsum = bgOnlySamplesSums->profileLRsums[i];
    double pointsPerTrial = bgOnlySamplesSums->pointsPerTrial[i];
    double eventsInLastBin = bgOnlySamplesSums->actualEvtsLastBinSums[i];
    //double totalEvts = bgOnlySamplesSums->actualEvtsTotalSums[i];

    backgroundSampleHistos->nllSOnlyHist->Fill(nllSsum);
    backgroundSampleHistos->nllBOnlyHist->Fill(nllBsum);
    backgroundSampleHistos->nllSatOnlyHist->Fill(nllSatsum);
    backgroundSampleHistos->nllSBHist->Fill(nllSBsum);
    backgroundSampleHistos->nllSBOverBHist->Fill(2*(nllSBsum-nllBsum));
    backgroundSampleHistos->nllBSatHist->Fill(2*(nllBsum-nllSatsum));
    backgroundSampleHistos->profileLRHist->Fill(profileLRsum);
    backgroundSampleHistos->pointsPerTrialHist->Fill(pointsPerTrial);
    backgroundSampleHistos->eventsInLastBinVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),eventsInLastBin);
    //backgroundSampleHistos->eventsTotalVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),totalEvts);

    qMuBgOnlyVec.push_back(profileLRsum);
  }
  // plot histograms -- sb
  for(unsigned int i=0; i<sigAndBgSamplesSums->nllSsums.size(); ++i)
  {
    double nllSsum = sigAndBgSamplesSums->nllSsums[i];
    double nllBsum = sigAndBgSamplesSums->nllBsums[i];
    double nllSatsum = sigAndBgSamplesSums->nllSatsums[i];
    double nllSBsum = sigAndBgSamplesSums->nllSBsums[i];
    double profileLRsum = sigAndBgSamplesSums->profileLRsums[i];
    double pointsPerTrial = sigAndBgSamplesSums->pointsPerTrial[i];
    double eventsInLastBin = sigAndBgSamplesSums->actualEvtsLastBinSums[i];
    //double totalEvts = sigAndBgSamplesSums->actualEvtsTotalSums[i];
    //std::cout << "profileLR sum: " << sigAndBgSamplesSums->nllSsums[i] << std::endl;

    signalAndBackgroundSampleHistos->nllSOnlyHist->Fill(nllSsum);
    signalAndBackgroundSampleHistos->nllBOnlyHist->Fill(nllBsum);
    signalAndBackgroundSampleHistos->nllSatOnlyHist->Fill(nllSatsum);
    signalAndBackgroundSampleHistos->nllSBHist->Fill(nllSBsum);
    signalAndBackgroundSampleHistos->nllSBOverBHist->Fill(2*(nllSBsum-nllBsum));
    signalAndBackgroundSampleHistos->nllBSatHist->Fill(2*(nllBsum-nllSatsum));
    signalAndBackgroundSampleHistos->profileLRHist->Fill(profileLRsum);
    signalAndBackgroundSampleHistos->pointsPerTrialHist->Fill(pointsPerTrial);
    signalAndBackgroundSampleHistos->eventsInLastBinVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),eventsInLastBin);
    //signalAndBackgroundSampleHistos->eventsTotalVsNLLSBOverBHist->Fill(2*(nllSBsum-nllBsum),totalEvts);
  }

  // median q_mu
  double medianQmuZero = 0;
  sort(qMuBgOnlyVec.begin(),qMuBgOnlyVec.end());
  medianQmuZero = qMuBgOnlyVec.at(qMuBgOnlyVec.size()/2+1);
  cout << "Median q_mu|0 value: " << medianQmuZero << endl;

  // p-value
  TH1F* sigBackProfileLRHist = signalAndBackgroundSampleHistos->profileLRHist;
  int medianQMuZeroBin = sigBackProfileLRHist->FindBin(medianQmuZero);
  int maxBin = sigBackProfileLRHist->GetNbinsX();
  double pvalue = sigBackProfileLRHist->Integral(medianQMuZeroBin,maxBin)/sigBackProfileLRHist->Integral();
  std::cout << "p-value of for this signal fraction: " << pvalue
    << " this upper limit on signal fraction has CL = " << 1-pvalue << std::endl;
    

}


