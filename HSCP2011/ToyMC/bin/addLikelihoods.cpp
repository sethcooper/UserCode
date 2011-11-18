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
  // NLL B/sat hist
  string nllBSatHistName="nllBSatHist";
  std::string nllBSatHistTitle = "-2*ln(B/sat)";
  TH1F* nllBSatHist = fs.make<TH1F>(nllBSatHistName.c_str(),nllBSatHistTitle.c_str(),100,0,1000);
  // NLL sat only hist
  string nllSatOnlyHistName="nllSatonlyHist";
  std::string nllSatOnlyHistTitle = "NLL sat only";
  TH1F* nllSatOnlyHist = fs.make<TH1F>(nllSatOnlyHistName.c_str(),nllSatOnlyHistTitle.c_str(),10000,-5000,5000);
  // NLL B only hist
  string nllBOnlyHistName="nllBonlyHist";
  std::string nllBOnlyHistTitle = "NLL B only";
  TH1F* nllBOnlyHist = fs.make<TH1F>(nllBOnlyHistName.c_str(),nllBOnlyHistTitle.c_str(),10000,-5000,5000);
  // NLL S only hist
  string nllSonlyHistName="nllSonlyHist";
  std::string nllSonlyHistTitle = "NLL S only";
  TH1F* nllSOnlyHist = fs.make<TH1F>(nllSonlyHistName.c_str(),nllSonlyHistTitle.c_str(),10000,-5000,5000);
  // NLL SB hist
  string nllSBHistName="nllSBHist";
  std::string nllSBHistTitle = "NLL SB";
  TH1F* nllSBHist = fs.make<TH1F>(nllSBHistName.c_str(),nllSBHistTitle.c_str(),10000,-5000,5000);
  // NLL S+B/B
  string nllSBOverBHistName="lepQHist";
  std::string nllSBOverBHistTitle = "-2*ln(L(s+b)/L(b));-2 ln Q";
  TH1F* nllSBOverBHist = fs.make<TH1F>(nllSBOverBHistName.c_str(),nllSBOverBHistTitle.c_str(),800,-1000,1000);
  // profile LR
  std::string profileLRHistName="profileLRHist";
  std::string profileLRHistTitle = "-2*ln(L(#mu)/L(#hat{#mu}));q_{#mu}";
  TH1F* profileLRHist = fs.make<TH1F>(profileLRHistName.c_str(),profileLRHistTitle.c_str(),2000,-100,100);
  // points per trial
  TH1F* pointsPerTrialHist = fs.make<TH1F>("pointsPerTrialHist","Points per trial",20000,0,20000);
  // events in last bin vs. NLL SB/B
  TH2F* eventsInLastBinVsNLLSBOverBHist = fs.make<TH2F>("eventsInLastBinVsNLLSBOverB",
      "Events in last bin vs. NLL -2*ln(SB/B)",800,-1000,1000,100,0,100);
  // events total vs. NLL SB/B
  //TH2F* eventsTotalVsNLLSBOverBHist = fs.make<TH2F>("eventsInTotalVsNLLSBOverB",
  //    "Events total vs. NLL -2*ln(SB/B)",800,-1000,1000,100,0,100);
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

  vector<double> nllSsums;
  vector<double> nllBsums;
  vector<double> nllSBsums;
  vector<double> nllSatsums;
  vector<double> nllSBFitsums;
  vector<double> profileLRsums;

  vector<int> pointsPerTrial;
  vector<double> expectedBgEvtsLastBinSums;
  vector<double> expectedSigEvtsLastBinSums;
  vector<double> expectedBgEvtsTotalSums;
  vector<double> expectedSigEvtsTotalSums;
  vector<int> actualEvtsLastBinSums;
  vector<int> signalEvtsLastBinSums;

  const int numNomSlices = 9;
  const int numEtaSlices = 12;

  // nll values
  RooRealVar sigNLLRooVar("sigNLLRooVar","signal NLL",-100000,100000);
  RooRealVar backNLLRooVar("backNLLRooVar","background NLL",-100000,100000);
  RooRealVar sigBackNLLRooVar("sigBackNLLRooVar","signal+background NLL",-100000,100000);
  RooRealVar satModelNLLRooVar("satModelNLLRooVar","sat model NLL",-100000,100000);
  RooRealVar sigBackFitNLLRooVar("sigBackFitNLLRooVar","signal+background ML fit",-100000,100000);
  RooRealVar maxLSigFracRooVar("maxLSigFracRooVar","max L fit signal frac",0,1);

  RooRealVar expectedBackgroundEventsInLastBinRooVar("expectedBackgroundEventsInLastBinRooVar","expected bg events in last bin",0,20000);
  RooRealVar expectedSignalEventsInLastBinRooVar("expectedSignalEventsInLastBinRooVar","expected signal events in last bin",0,20000);
  RooRealVar expectedBackgroundEventsTotalRooVar("expectedBackgroundEventsTotalRooVar","expected bg events",0,50000);
  RooRealVar expectedSignalEventsTotalRooVar("expectedSignalEventsTotalRooVar","expected signal events",0,20000);
  RooRealVar actualEventsInLastBinRooVar("actualEventsInLastBinRooVar","actual events in last bin",0,20000);
  RooRealVar signalEventsInLastBinRooVar("signalEventsInLastBinRooVar","signal events in last bin",0,0,20000);


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

        double sigFracExp = expSigEventsTotal->getVal()/(double)(expSigEventsTotal->getVal()+expBgEventsTotal->getVal());

        //if(nllSsums.size() < (unsigned int)thisDataSet->numEntries())
        //XXX possible bug above?
        if(nllSsums.size() < (unsigned int)i+1) // make an entry for this trial if there isn't one already
        {
          nllSsums.push_back(nllS->getVal());
          nllBsums.push_back(nllB->getVal());
          nllSBsums.push_back(nllSB->getVal());
          nllSatsums.push_back(nllSat->getVal());
          nllSBFitsums.push_back(nllSBFit->getVal());
          if(maxLSigFrac->getVal() <= sigFracExp)
            profileLRsums.push_back(2*(nllSB->getVal()-nllSBFit->getVal()));
          else
            profileLRsums.push_back(0);
          pointsPerTrial.push_back(1);
          expectedBgEvtsLastBinSums.push_back(expBgEventsLastBin->getVal());
          expectedSigEvtsLastBinSums.push_back(expSigEventsLastBin->getVal());
          expectedBgEvtsTotalSums.push_back(expBgEventsTotal->getVal());
          expectedSigEvtsTotalSums.push_back(expSigEventsTotal->getVal());
          actualEvtsLastBinSums.push_back(actEventsLastBin->getVal());
          signalEvtsLastBinSums.push_back(sigEventsLastBin->getVal());
        }
        else
        {
          nllSsums[i]+=nllS->getVal();
          nllBsums[i]+=nllB->getVal();
          nllSBsums[i]+=nllSB->getVal();
          nllSatsums[i]+=nllSat->getVal();
          nllSBFitsums[i]+=nllSBFit->getVal();
          if(maxLSigFrac->getVal() <= sigFracExp)
            profileLRsums[i]+=(2*(nllSB->getVal()-nllSBFit->getVal()));
          pointsPerTrial[i]++;
          expectedBgEvtsLastBinSums[i]+=expBgEventsLastBin->getVal();
          expectedSigEvtsLastBinSums[i]+=expSigEventsLastBin->getVal();
          expectedBgEvtsTotalSums[i]+=expBgEventsTotal->getVal();
          expectedSigEvtsTotalSums[i]+=expSigEventsTotal->getVal();
          actualEvtsLastBinSums[i]+=actEventsLastBin->getVal();
          signalEvtsLastBinSums[i]+=sigEventsLastBin->getVal();
        }
        if(i==0)
        {
          expBgEventsInLastBinHist->Fill(eta+0.1,nom,expBgEventsLastBin->getVal());
          expSigEventsInLastBinHist->Fill(eta+0.1,nom,expSigEventsLastBin->getVal());
          expBgEventsInDRegionHist->Fill(eta+0.1,nom,expBgEventsTotal->getVal());
          expSigEventsInDRegionHist->Fill(eta+0.1,nom,expSigEventsTotal->getVal());
          if(expBgEventsLastBin->getVal() > 0)
            sOverRootBExpInLastBinHist->Fill(eta,nom,expSigEventsLastBin->getVal()/(double)sqrt(expBgEventsLastBin->getVal()));
          if(expBgEventsTotal->getVal() > 0)
            sOverRootBExpInDRegionHist->Fill(eta,nom,expSigEventsTotal->getVal()/(double)sqrt(expBgEventsTotal->getVal()));
          cout << "trial: " << i << " file: " << inFile->GetName() <<
            "\n\t nllSonly: " << nllS->getVal()
            << "\n\t nllBOnly: " << nllB->getVal() << " nllSB: " << nllSB->getVal()
            << "\n\t nllSBOverB: " << 2*(nllSB->getVal()-nllB->getVal())
            << "\n\t nllBSat: " << 2*(nllB->getVal()-nllSat->getVal())
            << "\n\t eventsInLastBin: " << actEventsLastBin->getVal()
            << "\n\t expectedBgEventsInLastBin: " << expBgEventsLastBin->getVal()
            << "\n\t expectedSigEventsInLastBin: " << expSigEventsLastBin->getVal()
            << endl;
        }
      }
    }
  }

  if(expectedBgEvtsLastBinSums.size() < 1)
  {
    std::cout << "ERROR: unable to find any information from any input file.  Stopping." << std::endl;
    return -10;
  }

  std::cout << "INFO: expected BG #events in last bin=" << expectedBgEvtsLastBinSums[0]
    << std::endl;
  std::cout << "INFO: expected Sig #events in last bin=" << expectedSigEvtsLastBinSums[0]
    << std::endl;
  std::cout << "INFO: expected BG #events total=" << expectedBgEvtsTotalSums[0]
    << std::endl;
  std::cout << "INFO: expected Sig #events total=" << expectedSigEvtsTotalSums[0]
    << std::endl;


  vector<double> qMuVec;
  // plot histograms
  for(unsigned int i=0; i<nllSsums.size(); ++i)
  {
    nllSOnlyHist->Fill(nllSsums[i]);
    nllBOnlyHist->Fill(nllBsums[i]);
    nllSatOnlyHist->Fill(nllSatsums[i]);
    nllSBHist->Fill(nllSBsums[i]);
    nllSBOverBHist->Fill(2*(nllSBsums[i]-nllBsums[i]));
    nllBSatHist->Fill(2*(nllBsums[i]-nllSatsums[i]));
    profileLRHist->Fill(profileLRsums[i]);
    qMuVec.push_back(profileLRsums[i]);
    pointsPerTrialHist->Fill(pointsPerTrial[i]);
    eventsInLastBinVsNLLSBOverBHist->Fill(2*(nllSBsums[i]-nllBsums[i]),actualEvtsLastBinSums[i]);
    //eventsTotalVsNLLSBOverBHist->Fill(2*(nllSBsums[i]-nllBsums[i]),actualEvtsTotalSums[i]);

    //if(2*(nllSBsums[i]-nllBsums[i]) < -200)
    //{
    //  cout << "STRANGE EVENT INFO: trial: " << i << " nllSonly: " << nllSsums[i]
    //    << " nllBOnly: " << nllBsums[i] << " nllSB: " << nllSBsums[i]
    //    << " nllSBOverB: " << 2*(nllSBsums[i]-nllBsums[i])
    //    << " nllBSat: " << 2*(nllBsums[i]-nllSatsums[i])
    //    << endl;

    //}
  }

  // median q_mu
  sort(qMuVec.begin(),qMuVec.end());
  cout << "Median q_mu value: " << qMuVec.at(qMuVec.size()/2+1) << endl;

}


