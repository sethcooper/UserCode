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

#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooPlot.h"

#include "TSystem.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <vector>
#include <string>

namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}

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
#include "DataFormats/Math/interface/deltaR.h"

#include "commonFunctions.h"

// struct to hold before preselection plots
struct PlotStruct
{
  TH1F* pDistributionHist;
  TH1F* ptDistributionHist;
  TH2F* pVsIhHist;
  TH2F* pVsIasHist;
  TH2F* trackEtaVsPHist;
  TH1F* iasNoMHist;
  TH2F* pVsIasToFSBHist;
  TH2F* pVsIhToFSBHist;
  TH2F* ihVsIasHist;
  TH2F* pVsNoMHist;
  TH2F* pVsNoMCentralEtaHist;
  TH2F* pVsNoMEtaSlice1Hist;
  TH2F* pVsNoMEtaSlice2Hist;
  TH2F* pVsNoMEtaSlice3Hist;
  TH2F* pVsNoMEtaSlice4Hist;
  TH2F* pVsNoMEtaSlice5Hist;
  TH2F* pVsNoMEtaSlice6Hist;
  TH2F* pVsNoMEtaSlice7Hist;
  TH2F* pVsNoMEtaSlice8Hist;
  TH2F* nohVsNoMHist;
  TH2F* nohVsNoMCentralEtaHist;
  TH2F* pVsNoHHist;
  TH2F* pVsNoHCentralEtaHist;
  TH2F* pVsRelPerrHist;
  TH2F* pVsRelPerrCentralEtaHist;

  PlotStruct(fwlite::TFileService& fs, std::string nameString, std::string titleString)
  {
    std::string dirName = "Plots";
    dirName+=nameString;
    TFileDirectory plotsDir = fs.mkdir(dirName.c_str());
    // p
    std::string pName = "pDistribution";
    pName+=nameString;
    std::string pTitle = "P ";
    pTitle+=titleString;
    pTitle+=";GeV";
    pDistributionHist = plotsDir.make<TH1F>(pName.c_str(),pTitle.c_str(),200,0,2000);
    pDistributionHist->Sumw2();
    // pt
    std::string ptName = "ptDistribution";
    ptName+=nameString;
    std::string ptTitle = "Pt ";
    ptTitle+=titleString;
    ptTitle+=";GeV";
    ptDistributionHist = plotsDir.make<TH1F>(ptName.c_str(),ptTitle.c_str(),200,0,2000);
    ptDistributionHist->Sumw2();
    // p vs Ih
    std::string pVsIhName = "trackPvsIh";
    pVsIhName+=nameString;
    std::string pVsIhTitle="Track P vs ih ";
    pVsIhTitle+=titleString;
    pVsIhTitle+=";MeV/cm;GeV";
    pVsIhHist = plotsDir.make<TH2F>(pVsIhName.c_str(),pVsIhTitle.c_str(),400,0,10,100,0,1000);
    pVsIhHist->Sumw2();
    // p vs Ias
    std::string pVsIasName = "pVsIas";
    pVsIasName+=nameString;
    std::string pVsIasTitle = "P vs Ias ";
    pVsIasTitle+=titleString;
    pVsIasTitle+=";;GeV";
    pVsIasHist = plotsDir.make<TH2F>(pVsIasName.c_str(),pVsIasTitle.c_str(),20,0,1,40,0,1000);
    pVsIasHist->Sumw2();
    // eta vs p
    std::string etaVsPName = "trackEtaVsP";
    etaVsPName+=nameString;
    std::string etaVsPTitle = "Track #eta vs. p ";
    etaVsPTitle+=titleString;
    etaVsPTitle+=";GeV";
    trackEtaVsPHist = plotsDir.make<TH2F>(etaVsPName.c_str(),etaVsPTitle.c_str(),4000,0,2000,24,0,2.4);
    trackEtaVsPHist->Sumw2();
    // ias NoM
    std::string iasNoMName = "iasNoM";
    iasNoMName+=nameString;
    std::string iasNoMTitle = "NoM (ias) ";
    iasNoMTitle+=titleString;
    iasNoMHist = plotsDir.make<TH1F>(iasNoMName.c_str(),iasNoMTitle.c_str(),50,0,50);
    // ToF SB - p vs Ias
    std::string pVsIasToFSBName = "trackPvsIasToFSB";
    pVsIasToFSBName+=nameString;
    std::string pVsIasToFSBTitle="Track P vs ias (ToF SB: #beta>1.075) ";
    pVsIasToFSBTitle+=titleString;
    pVsIasToFSBTitle+=";;GeV";
    pVsIasToFSBHist = plotsDir.make<TH2F>(pVsIasToFSBName.c_str(),pVsIasToFSBTitle.c_str(),400,0,1,100,0,1000);
    pVsIasToFSBHist->Sumw2();
    // ToF SB - p vs Ih
    std::string pVsIhToFSBName = "trackPvsIhToFSB";
    pVsIhToFSBName+=nameString;
    std::string pVsIhToFSBTitle="Track P vs ih (ToF SB: #beta>1.075) ";
    pVsIhToFSBTitle+=titleString;
    pVsIhToFSBTitle+=";MeV/cm;GeV";
    pVsIhToFSBHist = plotsDir.make<TH2F>(pVsIhToFSBName.c_str(),pVsIhToFSBTitle.c_str(),400,0,10,100,0,1000);
    pVsIhToFSBHist->Sumw2();
    // Ih vs Ias
    std::string ihVsIasName = "ihVsIas";
    ihVsIasName+=nameString;
    std::string ihVsIasTitle = "Ih vs. Ias ";
    ihVsIasTitle+=titleString;
    ihVsIasTitle+=";MeV/cm";
    ihVsIasHist = plotsDir.make<TH2F>(ihVsIasName.c_str(),ihVsIasTitle.c_str(),400,0,1,400,0,10);
    ihVsIasHist->Sumw2();
    // p vs NoM
    std::string pVsNoMName = "pVsNoM";
    pVsNoMName+=nameString;
    std::string pVsNoMTitle = "Track P vs. NoM (Ias) ";
    pVsNoMTitle+=titleString;
    pVsNoMTitle+=";;GeV";
    pVsNoMHist = plotsDir.make<TH2F>(pVsNoMName.c_str(),pVsNoMTitle.c_str(),50,0,50,100,0,1000);
    // p vs NoM, central eta only
    std::string pVsNoMCentralEtaName = "pVsNoMCentralEta";
    pVsNoMCentralEtaName+=nameString;
    std::string pVsNoMCentralEtaTitle = "Track P vs. NoM (Ias) ";
    pVsNoMCentralEtaTitle+=titleString;
    pVsNoMCentralEtaTitle+=";;GeV";
    pVsNoMCentralEtaHist = plotsDir.make<TH2F>(pVsNoMCentralEtaName.c_str(),pVsNoMCentralEtaTitle.c_str(),50,0,50,100,0,1000);
    // p vs NoM, eta slices
    std::string pVsNoMEtaSliceBaseName = "pVsNoM";
    pVsNoMEtaSliceBaseName+=nameString;
    std::string pVsNoMEtaSliceBaseTitle = "Track P vs. NoM (Ias)";
    pVsNoMEtaSliceBaseTitle+=titleString;

    pVsNoMEtaSlice1Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice1").c_str(),(pVsNoMEtaSliceBaseTitle+" |#eta| < 0.2;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice2Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice2").c_str(),(pVsNoMEtaSliceBaseTitle+" 0.2 < |#eta| < 0.4;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice3Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice3").c_str(),(pVsNoMEtaSliceBaseTitle+" 0.4 < |#eta| < 0.6;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice4Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice4").c_str(),(pVsNoMEtaSliceBaseTitle+" 0.6 < |#eta| < 0.8;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice5Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice5").c_str(),(pVsNoMEtaSliceBaseTitle+" 0.8 < |#eta| < 1.0;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice6Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice6").c_str(),(pVsNoMEtaSliceBaseTitle+" 1.0 < |#eta| < 1.2;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice7Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice7").c_str(),(pVsNoMEtaSliceBaseTitle+" 1.2 < |#eta| < 1.4;;GeV").c_str(),50,0,50,100,0,1000);
    pVsNoMEtaSlice8Hist = plotsDir.make<TH2F>((pVsNoMEtaSliceBaseName+"EtaSlice8").c_str(),(pVsNoMEtaSliceBaseTitle+" 1.4 < |#eta| < 1.6;;GeV").c_str(),50,0,50,100,0,1000);
    // NoH vs NoM
    std::string nohVsNoMName = "nohVsNoM";
    nohVsNoMName+=nameString;
    std::string nohVsNoMTitle = "Number of hits vs. NoM (Ias) ";
    nohVsNoMTitle+=titleString;
    nohVsNoMTitle+=";NoM;NoH";
    nohVsNoMHist = plotsDir.make<TH2F>(nohVsNoMName.c_str(),nohVsNoMTitle.c_str(),50,0,50,50,0,50);
    // NoH vs NoM, central eta
    std::string nohVsNoMCentralName = "nohVsNoMCentral";
    nohVsNoMCentralName+=nameString;
    std::string nohVsNoMCentralTitle = "Number of hits vs. NoM (Ias) ";
    nohVsNoMCentralTitle+=titleString;
    nohVsNoMCentralTitle+=", |#eta| < 0.9;NoM;NoH";
    nohVsNoMCentralEtaHist = plotsDir.make<TH2F>(nohVsNoMCentralName.c_str(),nohVsNoMCentralTitle.c_str(),50,0,50,50,0,50);
    // p vs NoH
    std::string pVsNoHName = "pVsNoH";
    pVsNoHName+=nameString;
    std::string pVsNoHTitle = "Track P vs. NoH ";
    pVsNoHTitle+=titleString;
    pVsNoHTitle+=";;GeV";
    pVsNoHHist = plotsDir.make<TH2F>(pVsNoHName.c_str(),pVsNoHTitle.c_str(),50,0,50,100,0,1000);
    // p vs NoH, central eta only
    std::string pVsNoHCentralEtaName = "pVsNoHCentralEta";
    pVsNoHCentralEtaName+=nameString;
    std::string pVsNoHCentralEtaTitle = "Track P vs. NoH ";
    pVsNoHCentralEtaTitle+=titleString;
    pVsNoHCentralEtaTitle+=", |#eta| < 0.9;;GeV";
    pVsNoHCentralEtaHist = plotsDir.make<TH2F>(pVsNoHCentralEtaName.c_str(),pVsNoHCentralEtaTitle.c_str(),50,0,50,100,0,1000);
    // p vs relPerr
    std::string pVsRelPerrName = "pVsRelPerr";
    pVsRelPerrName+=nameString;
    std::string pVsRelPerrTitle = "Track P vs. #Deltap/p ";
    pVsRelPerrTitle+=titleString;
    pVsRelPerrTitle+=";;GeV";
    pVsRelPerrHist = plotsDir.make<TH2F>(pVsRelPerrName.c_str(),pVsRelPerrTitle.c_str(),100,0,1,100,0,1000);
    // p vs relPerr, central eta only
    std::string pVsRelPerrCentralEtaName = "pVsRelPerrCentralEta";
    pVsRelPerrCentralEtaName+=nameString;
    std::string pVsRelPerrCentralEtaTitle = "Track P vs. #Deltap/p ";
    pVsRelPerrCentralEtaTitle+=titleString;
    pVsRelPerrCentralEtaTitle+=", |#eta| < 0.9;;GeV";
    pVsRelPerrCentralEtaHist = plotsDir.make<TH2F>(pVsRelPerrCentralEtaName.c_str(),pVsRelPerrCentralEtaTitle.c_str(),100,0,1,100,0,1000);
  }
};

// adapted from AnalysisStep234.C (GetGenHSCPBeta)
int getNumGenHSCP(const std::vector<reco::GenParticle>& genColl, bool onlyCharged)
{
  int numGenHscp = 0;
  for(unsigned int g=0;g<genColl.size();g++)
  {
    if(genColl[g].pt()<5)continue;
    if(genColl[g].status()!=1)continue;
    int AbsPdg=abs(genColl[g].pdgId());
    if(AbsPdg<1000000)continue;
    if(onlyCharged && (AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324))continue; //Skip neutral gluino RHadrons
    if(onlyCharged && (AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333))continue;  //skip neutral stop RHadrons
    //if(beta1<0){beta1=genColl[g].p()/genColl[g].energy();}else if(beta2<0){beta2=genColl[g].p()/genColl[g].energy();return;}
    numGenHscp++;
  }
  return numGenHscp;
}




// ****** main
int main(int argc, char ** argv)
{
  // load framework libraries
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  // parse arguments
  if(argc < 2)
  {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  using namespace RooFit;

  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process);
  fwlite::OutputFiles outputHandler_(process);
  int maxEvents_(inputHandler_.maxEvents());
  // now get each parameterset
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("makeHSCParticlePlots");
  double scaleFactor_(ana.getParameter<double>("ScaleFactor"));
  // mass cut to use for the high-p high-Ih (search region) ias dist
  double massCutIasHighPHighIh_ (ana.getParameter<double>("MassCut"));
  // dE/dx calibration
  double dEdx_k_ (ana.getParameter<double>("dEdx_k"));
  double dEdx_c_ (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold_ (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold_ (ana.getParameter<double>("IhSidebandThreshold"));
  bool matchToHSCP_ (ana.getParameter<bool>("MatchToHSCP"));
  bool isMC_ (ana.getParameter<bool>("IsMC"));
  double signalEventCrossSection_ (ana.getParameter<double>("SignalEventCrossSection")); // pb

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  // p vs Ih - SR
  TH2F* pVsIhDistributionSearchRegionHist;
  std::string pVsIhSearchRegionName = "trackPvsIhSearchRegion";
  std::string pVsIhSearchRegionTitle="Track P vs ih (SearchRegion+MassCut)";
  pVsIhSearchRegionTitle+=";MeV/cm;GeV";
  pVsIhDistributionSearchRegionHist = fs.make<TH2F>(pVsIhSearchRegionName.c_str(),pVsIhSearchRegionTitle.c_str(),400,0,10,100,0,1000);
  pVsIhDistributionSearchRegionHist->Sumw2();
  // p vs Ih - B region
  TH2F* pVsIhDistributionBRegionHist;
  std::string pVsIhBRegionName = "trackPvsIhBRegion";
  std::string pVsIhBRegionTitle="Track P vs ih (low P Region)";
  pVsIhBRegionTitle+=";MeV/cm;GeV";
  pVsIhDistributionBRegionHist = fs.make<TH2F>(pVsIhBRegionName.c_str(),pVsIhBRegionTitle.c_str(),400,0,10,100,0,1000);
  pVsIhDistributionBRegionHist->Sumw2();
  // p vs Ias
  TH2F* pVsIasSearchRegionHist = fs.make<TH2F>("pVsIasSearchRegion","P vs Ias (SearchRegion+MassCut);;GeV",20,0,1,40,0,1000);
  pVsIasSearchRegionHist->Sumw2();
  TH2F* trackEtaVsPSearchRegionHist = fs.make<TH2F>("trackEtaVsPSearchRegion","Track #eta vs. p (SearchRegion+MassCut);GeV",4000,0,2000,24,0,2.4);
  trackEtaVsPSearchRegionHist->Sumw2();
  // num hscp gen per event
  TH1F* numHSCPGenPerEventHist = fs.make<TH1F>("numHSCPGenPerEvent","Number of gen HSCPs per event",4,0,4);
  // num hscp seen per event
  TH1F* numHSCPSeenPerEventHist = fs.make<TH1F>("numHSCPSeenPerEvent","Number of seen HSCPs per event",4,0,4);
  // num tracks passing presel in A region per event
  TH1F* numTracksPassingPreselARegionPerEventHist = fs.make<TH1F>("numTracksPassingPreselARegionPerEventHist",
      "Tracks passing preselection in A region per event",10,0,10);
  // num tracks passing presel in B region per event
  TH1F* numTracksPassingPreselBRegionPerEventHist = fs.make<TH1F>("numTracksPassingPreselBRegionPerEventHist",
      "Tracks passing preselection in B region per event",10,0,10);
  // num tracks passing presel in C region per event
  TH1F* numTracksPassingPreselCRegionPerEventHist = fs.make<TH1F>("numTracksPassingPreselCRegionPerEventHist",
      "Tracks passing preselection in C region per event",10,0,10);
  // search region (only filled for MC)
  TH1F* pDistributionSearchRegionHist = fs.make<TH1F>("pDistributionSearchRegion","SearchRegion+MassCutP;GeV",200,0,2000);
  pDistributionSearchRegionHist->Sumw2();
  TH1F* ptDistributionSearchRegionHist = fs.make<TH1F>("ptDistributionSearchRegion","SearchRegion+MassCutPt;GeV",200,0,2000);
  ptDistributionSearchRegionHist->Sumw2();
  // p vs Ias
  TH2F* pVsIasDistributionSearchRegionHist;
  std::string pVsIasSearchRegionName = "trackPvsIasSearchRegion";
  std::string pVsIasSearchRegionTitle="Track P vs ias (SearchRegion+MassCut)";
  pVsIasSearchRegionTitle+=";;GeV";
  pVsIasDistributionSearchRegionHist = fs.make<TH2F>(pVsIasSearchRegionName.c_str(),pVsIasSearchRegionTitle.c_str(),400,0,1,100,0,1000);
  pVsIasDistributionSearchRegionHist->Sumw2();

  PlotStruct afterPreselectionPlots(fs,"AfterPreselection","after preselection");
  PlotStruct beforePreselectionPlots(fs,"BeforePreselection","before preselection");

  // RooFit observables and dataset
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIp("rooVarIp","ip",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarPt("rooVarPt","pt",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",-2.5,2.5);
  RooRealVar rooVarRun("rooVarRun","run",0,4294967295);
  RooRealVar rooVarLumiSection("rooVarLumiSection","lumiSection",0,4294967295);
  RooRealVar rooVarEvent("rooVarEvent","event",0,4294967295);
  RooDataSet* rooDataSetCandidates = fs.make<RooDataSet>("rooDataSetCandidates","rooDataSetCandidates",
      RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarRun,rooVarLumiSection,rooVarEvent));
      //RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
  // Ip no longer included
  RooDataSet* rooDataSetOneCandidatePerEvent = fs.make<RooDataSet>("rooDataSetOneCandidatePerEvent",
      "rooDataSetOneCandidatePerEvent",
      RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarRun,rooVarLumiSection,rooVarEvent));

  // Dataset for IasShift
  RooDataSet* rooDataSetIasShift = fs.make<RooDataSet>("rooDataSetIasShift","rooDataSetIasShift",
      RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
  // Dataset for IhShift
  RooDataSet* rooDataSetIhShift = fs.make<RooDataSet>("rooDataSetIhShift","rooDataSetIhShift",
      RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
  // Dataset for PtShift
  RooDataSet* rooDataSetPtShift = fs.make<RooDataSet>("rooDataSetPtShift","rooDataSetPtShift",
      RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));

  // RooDataSet for number of original tracks, etc.
  RooRealVar rooVarNumGenHSCPEvents("rooVarNumGenHSCPEvents","numGenHSCPEvents",0,5e6);
  RooRealVar rooVarNumGenHSCPTracks("rooVarNumGenHSCPTracks","numGenHSCPTracks",0,5e6);
  RooRealVar rooVarNumGenChargedHSCPTracks("rooVarNumGenChargedHSCPTracks","numGenChargedHSCPTracks",0,5e6);
  RooRealVar rooVarSignalEventCrossSection("rooVarSignalEventCrossSection","signalEventCrossSection",0,100); // pb
  RooDataSet* rooDataSetGenHSCPTracks = fs.make<RooDataSet>("rooDataSetGenHSCPTracks","rooDataSetGenHSCPTracks",
      RooArgSet(rooVarNumGenHSCPEvents,rooVarNumGenHSCPTracks,rooVarNumGenChargedHSCPTracks,rooVarSignalEventCrossSection));


  int numEventsPassingTrigger = 0;
  int numEventsWithOneTrackPassingPreselection = 0;
  int numEventsWithOneTrackOverIhCut = 0;
  int numEventsWithOneTrackOverIasCut = 0;
  int numEventsWithOneTrackOverIhAndIasCuts = 0;

  int numGenHSCPTracks = 0;
  int numGenChargedHSCPTracks = 0;
  int numGenHSCPEvents = 0;
  int numTracksPassingTrigger = 0;
  int numTracksPassingPreselection = 0;
  int numTracksInSearchRegion = 0;

  int numEventsWithOneHSCPGen = 0;
  int numEventsWithTwoHSCPGen = 0;
  int numEventsWithOneHSCPSeenMuonTrigger = 0;
  int numEventsWithTwoHSCPSeenMuonTrigger = 0;
  int numEventsWithOneHSCPSeenMetTrigger = 0;
  int numEventsWithTwoHSCPSeenMetTrigger = 0;

  // loop the events
  int ievt = 0;
  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile)
  {
    // open input file (can be located on castor)
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if( inFile )
    {
      fwlite::Event ev(inFile);
      for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt)
      {
        double tempIas = -1;
        double tempIh = 0;
        double tempP = 0;
        double tempPt = 0;
        int tempNoMias = 0;
        double tempEta = 0;
        double tempRun = 0;
        double tempLumiSection = 0;
        double tempEvent = 0;

        // break loop if maximal number of events is reached 
        if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
        // simple event counter
        if(inputHandler_.reportAfter()!=0 ? (ievt>0 && ievt%inputHandler_.reportAfter()==0) : false) 
          std::cout << "  processing event: " << ievt << std::endl;

        fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
        hscpCollHandle.getByLabel(ev,"HSCParticleProducer");
        if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
        const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

        fwlite::Handle<reco::DeDxDataValueMap> dEdxSCollH;
        dEdxSCollH.getByLabel(ev, "dedxASmi");
        if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");continue;}

        fwlite::Handle<reco::DeDxDataValueMap> dEdxIpCollH;
        dEdxIpCollH.getByLabel(ev, "dedxProd");
        if(!dEdxIpCollH.isValid()){printf("Invalid dEdx Selection (Ip) collection\n");continue;}

        fwlite::Handle<reco::DeDxDataValueMap> dEdxMCollH;
        dEdxMCollH.getByLabel(ev, "dedxHarm2");
        if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

        // ToF
        fwlite::Handle<reco::MuonTimeExtraMap> TOFCollH;
        TOFCollH.getByLabel(ev, "muontiming","combined");
        if(!TOFCollH.isValid()){printf("Invalid TOF collection\n");continue;}

        fwlite::Handle<reco::MuonTimeExtraMap> TOFDTCollH;
        TOFDTCollH.getByLabel(ev, "muontiming","dt");
        if(!TOFDTCollH.isValid()){printf("Invalid DT TOF collection\n");continue;}

        fwlite::Handle<reco::MuonTimeExtraMap> TOFCSCCollH;
        TOFCSCCollH.getByLabel(ev, "muontiming","csc");
        if(!TOFCSCCollH.isValid()){printf("Invalid CSC TOF collection\n");continue;}

        // MC Gen Info
        fwlite::Handle< std::vector<reco::GenParticle> > genCollHandle;
        std::vector<reco::GenParticle> genColl;
        if(isMC_)
        {
          genCollHandle.getByLabel(ev, "genParticles");
          if(!genCollHandle.isValid()){printf("GenParticle Collection NotFound\n");continue;}
          genColl = *genCollHandle;
        }
        else
        {
          genCollHandle.getByLabel(ev, "genParticles");
          if(genCollHandle.isValid())
          {
            std::cout << "ERROR: GenParticle Collection Found when MC not indicated" << std::endl;
            return -5;
          }
        }

        // Look for gen HSCP tracks if MC
        if(isMC_)
        {
          int numGenHSCPThisEvent = getNumGenHSCP(genColl,false); // consider also neutrals
          int numGenChargedHSCPThisEvent = getNumGenHSCP(genColl,true); // consider only charged HSCP
          numGenHSCPTracks+=numGenHSCPThisEvent;
          numGenChargedHSCPTracks+=numGenChargedHSCPThisEvent;
          if(numGenHSCPThisEvent>=1)
            numEventsWithOneHSCPGen++;
          if(numGenHSCPThisEvent>=2)
            numEventsWithTwoHSCPGen++;
          numHSCPGenPerEventHist->Fill(numGenHSCPThisEvent);

          int numSeenHSCPThisEvt = 0;
          // attempt match track to gen hscp, look at gen/detected hscps
          for(unsigned int c=0;c<hscpColl.size();c++)
          {
            susybsm::HSCParticle hscp  = hscpColl[c];
            reco::MuonRef  muon  = hscp.muonRef();
            reco::TrackRef track = hscp.trackRef();
            if(track.isNull())
              continue;

            int ClosestGen;
            // match to gen hscp
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)
              continue;
            else
              numSeenHSCPThisEvt++;

          }
          numHSCPSeenPerEventHist->Fill(numSeenHSCPThisEvt);

          // check triggers
          if(passesTrigger(ev,true,false)) // consider mu trig only
          {
            if(numSeenHSCPThisEvt==1)
              numEventsWithOneHSCPSeenMuonTrigger++;
            if(numSeenHSCPThisEvt==2)
              numEventsWithTwoHSCPSeenMuonTrigger++;
          }
          if(passesTrigger(ev,false,true)) // consider MET trig only
          {
            if(numSeenHSCPThisEvt==1)
              numEventsWithOneHSCPSeenMetTrigger++;
            if(numSeenHSCPThisEvt==2)
              numEventsWithTwoHSCPSeenMetTrigger++;
          }
        }


        numGenHSCPEvents++;

        double lumiSection = ev.id().luminosityBlock();
        double runNumber = ev.id().run();
        double eventNumber = ev.id().event();
        // ignore real data taken with tigher RPC trigger (355.227/pb) -- from Analysis_Samples.h
        //if(!isMC_ && runNumber < 165970)
        //  continue;

        // check trigger
        if(!passesTrigger(ev))
          continue;

        numEventsPassingTrigger++;

        int numTracksPassingPreselectionThisEvent = 0;
        int numTracksPassingIhCutThisEvent = 0;
        int numTracksPassingIasCutThisEvent = 0;
        int numTracksPassingIhAndIasCutsThisEvent = 0;
        int numTracksPassingPreselectionARegionThisEvent = 0;
        int numTracksPassingPreselectionBRegionThisEvent = 0;
        int numTracksPassingPreselectionCRegionThisEvent = 0;
        // loop over HSCParticles in this event
        for(unsigned int c=0;c<hscpColl.size();c++)
        {
          susybsm::HSCParticle hscp  = hscpColl[c];
          reco::MuonRef  muon  = hscp.muonRef();
          reco::TrackRef track = hscp.trackRef();
          if(track.isNull())
            continue;

          numTracksPassingTrigger++;

          const reco::DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
          const reco::DeDxData& dedxMObj  = dEdxMCollH->get(track.key());
          const reco::DeDxData& dedxIpObj  = dEdxIpCollH->get(track.key());
          const reco::MuonTimeExtra* tof  = &TOFCollH->get(hscp.muonRef().key());
          const reco::MuonTimeExtra* dttof  = &TOFDTCollH->get(hscp.muonRef().key());
          const reco::MuonTimeExtra* csctof  = &TOFCSCCollH->get(hscp.muonRef().key());

          // for signal MC
          int NChargedHSCP = 0;
          if(isMC_ && matchToHSCP_)
          {
            int ClosestGen;
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)
              continue;
            NChargedHSCP = getNumGenHSCP(genColl,true);
          }

          float ih = dedxMObj.dEdx();
          float ias = dedxSObj.dEdx();
          float ip = dedxIpObj.dEdx();
          float trackP = track->p();
          float trackPt = track->pt();
          int iasNoM = dedxSObj.numberOfMeasurements();
          float trackEta = track->eta();
          int trackNoH = track->found();
          float trackPtErr = track->ptError();

          // systematics datasets for MC
          //TODO: FIXME: apply shift, then preselect/choose highest ias, then add to dataset
          if(isMC_)
          {
            TRandom3 myRandom;
            //TODO include TOF at some point?
            rooVarIas = ias;
            rooVarIp = ip;
            rooVarIh = ih;
            rooVarP = trackP;
            rooVarPt = trackPt;
            rooVarNoMias = iasNoM;
            rooVarEta = trackEta;
            // ias shift
            double newIas = ias + myRandom.Gaus(0,0.083) + 0.015; // from YK results Nov 21 2011 hypernews thread
            rooVarIas = newIas;
            rooDataSetIasShift->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
            rooVarIas = ias; // reset to original value
            // ih shift
            double newIh = ih*1.036; // from SIC results, Nov 10 2011 HSCP meeting
            rooVarIh = newIh;
            rooDataSetIhShift->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
            rooVarIh = ih; // reset to original value
            // pt shift (and p shift) -- from MU-10-004-001 -- SIC report Jun 7 2011 HSCP meeting
            double newInvPt = 1/trackPt+0.000236-0.000135*pow(trackEta,2)+track->charge()*0.000282*TMath::Sin(track->phi()-1.337);
            rooVarPt = 1/newInvPt;
            rooVarP = rooVarPt.getVal()/TMath::Sin(track->theta());
            rooDataSetPtShift->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
            rooVarPt = trackPt; // reset
            rooVarP = trackP; // reset
          }

          // apply preselections, not considering ToF
          if(!passesPreselection(hscp,dedxSObj,dedxMObj,tof,dttof,csctof,ev,false))
            continue;

          numTracksPassingPreselection++;
          numTracksPassingPreselectionThisEvent++;
          if(ih < ihSidebandThreshold_ && trackP < pSidebandThreshold_)
            numTracksPassingPreselectionARegionThisEvent++;
          else if(trackP < pSidebandThreshold_)
            numTracksPassingPreselectionBRegionThisEvent++;
          else if(ih < ihSidebandThreshold_)
            numTracksPassingPreselectionCRegionThisEvent++;

          if(ih > 3.5)
            numTracksPassingIhCutThisEvent++;
          if(ias > 0.35)
            numTracksPassingIasCutThisEvent++;
          if(ih > 3.5 && ias > 0.35)
            numTracksPassingIhAndIasCutsThisEvent++;

          if(trackP > pSidebandThreshold_ && ih > ihSidebandThreshold_)
          {
            numTracksInSearchRegion++;
            if(isMC_)
            {
              float massSqr = (ih-dEdx_c_)*pow(trackP,2)/dEdx_k_;
              if(massSqr < 0)
                continue;
              else if(sqrt(massSqr) >= massCutIasHighPHighIh_)
              {
                pDistributionSearchRegionHist->Fill(trackP);
                ptDistributionSearchRegionHist->Fill(trackPt);
                trackEtaVsPSearchRegionHist->Fill(trackP,fabs(trackEta));
                pVsIasSearchRegionHist->Fill(ias,trackP);
                pVsIasDistributionSearchRegionHist->Fill(ias,trackP);
                pVsIhDistributionSearchRegionHist->Fill(ih,trackP);
              }
            }
          }

          // A/B region
          if(trackP <= pSidebandThreshold_)
            pVsIhDistributionBRegionHist->Fill(ih,trackP);

          // fill some overall hists (already passed preselection)
          afterPreselectionPlots.pDistributionHist->Fill(trackP);
          afterPreselectionPlots.ptDistributionHist->Fill(trackPt);
          afterPreselectionPlots.trackEtaVsPHist->Fill(trackP,fabs(trackEta));
          afterPreselectionPlots.pVsIasHist->Fill(ias,trackP);
          afterPreselectionPlots.pVsIhHist->Fill(ih,trackP);
          afterPreselectionPlots.iasNoMHist->Fill(iasNoM);
          afterPreselectionPlots.ihVsIasHist->Fill(ias,ih);
          afterPreselectionPlots.pVsNoMHist->Fill(iasNoM,trackP);
          if(fabs(trackEta) < 0.9)
            afterPreselectionPlots.pVsNoMCentralEtaHist->Fill(iasNoM,trackP);
          if(fabs(trackEta) < 0.2)
            afterPreselectionPlots.pVsNoMEtaSlice1Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 0.4)
            afterPreselectionPlots.pVsNoMEtaSlice2Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 0.6)
            afterPreselectionPlots.pVsNoMEtaSlice3Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 0.8)
            afterPreselectionPlots.pVsNoMEtaSlice4Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 1.0)
            afterPreselectionPlots.pVsNoMEtaSlice5Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 1.2)
            afterPreselectionPlots.pVsNoMEtaSlice6Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 1.4)
            afterPreselectionPlots.pVsNoMEtaSlice7Hist->Fill(iasNoM,trackP);
          else if(fabs(trackEta) < 1.6)
            afterPreselectionPlots.pVsNoMEtaSlice8Hist->Fill(iasNoM,trackP);

          afterPreselectionPlots.nohVsNoMHist->Fill(iasNoM,trackNoH);
          if(fabs(trackEta) < 0.9)
            afterPreselectionPlots.nohVsNoMCentralEtaHist->Fill(iasNoM,trackNoH);
          afterPreselectionPlots.pVsNoHHist->Fill(trackNoH,trackP);
          if(fabs(trackEta) < 0.9)
            afterPreselectionPlots.pVsNoHCentralEtaHist->Fill(trackNoH,trackP);
          afterPreselectionPlots.pVsRelPerrHist->Fill(trackPtErr/trackPt,trackP);
          if(fabs(trackEta) < 0.9)
            afterPreselectionPlots.pVsRelPerrCentralEtaHist->Fill(trackPtErr/trackP,trackP);


          // fill roodataset
          //TODO include TOF at some point?
          rooVarIas = ias;
          rooVarIp = ip;
          rooVarIh = ih;
          rooVarP = trackP;
          rooVarPt = trackPt;
          rooVarNoMias = iasNoM;
          rooVarEta = trackEta;
          rooVarLumiSection = lumiSection;
          rooVarRun = runNumber;
          rooVarEvent = eventNumber;
          //rooDataSetCandidates->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));
          //XXX TURN OFF BELOW TO MAKE ONLY ONE ENTRY PER EVENT DATASET ONLY
          rooDataSetCandidates->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarLumiSection,
                rooVarRun,rooVarEvent));
          if(ias > tempIas)
          {
            tempIas = ias;
            tempIh = ih;
            tempP = trackP;
            tempPt = trackPt;
            tempNoMias = iasNoM;
            tempEta = trackEta;
            tempLumiSection = lumiSection;
            tempRun = runNumber;
            tempEvent = eventNumber;
          }

          //// now consider the ToF
          //if(!passesPreselection(hscp,dedxSObj,dedxMObj,tof,dttof,csctof,ev,true))
          //  continue;

          // require muon ref when accessing ToF, or the collections will be null
          if(hscp.muonRef().isNull())
            continue;
          // ndof check
          if(tof->nDof() < 8 && (dttof->nDof() < 6 || csctof->nDof() < 6) )
            continue;
          // select 1/beta low --> beta high
          if(tof->inverseBeta() >= 0.93)
            continue;
          // Max ToF err
          if(tof->inverseBetaErr() > 0.07)
            continue;

          // now we should have a sample where 1/beta < 0.93 --> beta > 1.075
          afterPreselectionPlots.pVsIasToFSBHist->Fill(ias,trackP);
          afterPreselectionPlots.pVsIhToFSBHist->Fill(ih,trackP);

        } // done looking at HSCParticle collection

        if(numTracksPassingPreselectionThisEvent > 0)
        {
          rooVarIas = tempIas;
          rooVarIh = tempIh;
          rooVarP = tempP;
          rooVarPt = tempPt;
          rooVarNoMias = tempNoMias;
          rooVarEta = tempEta;
          rooVarLumiSection = tempLumiSection;
          rooVarRun = tempRun;
          rooVarEvent = tempEvent;
          rooDataSetOneCandidatePerEvent->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarLumiSection,
                rooVarRun,rooVarEvent));
        }

        if(numTracksPassingPreselectionThisEvent > 0)
          numEventsWithOneTrackPassingPreselection++;
        if(numTracksPassingIhCutThisEvent > 0)
          numEventsWithOneTrackOverIhCut++;
        if(numTracksPassingIasCutThisEvent > 0)
          numEventsWithOneTrackOverIasCut++;
        if(numTracksPassingIhAndIasCutsThisEvent > 0)
          numEventsWithOneTrackOverIhAndIasCuts++;

      numTracksPassingPreselARegionPerEventHist->Fill(numTracksPassingPreselectionARegionThisEvent);
      numTracksPassingPreselBRegionPerEventHist->Fill(numTracksPassingPreselectionBRegionThisEvent);
      numTracksPassingPreselCRegionPerEventHist->Fill(numTracksPassingPreselectionCRegionThisEvent);

      } // loop over events

      // close input file
      inFile->Close();
    }
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
  }
  // end of file/event loop
  
  afterPreselectionPlots.pDistributionHist->Scale(scaleFactor_);

  rooVarNumGenHSCPEvents = numGenHSCPEvents;
  rooVarNumGenHSCPTracks = numGenHSCPTracks;
  rooVarNumGenChargedHSCPTracks = numGenChargedHSCPTracks;
  rooVarSignalEventCrossSection = signalEventCrossSection_;
  rooDataSetGenHSCPTracks->add(
      RooArgSet(rooVarNumGenHSCPEvents,rooVarNumGenHSCPTracks,rooVarNumGenChargedHSCPTracks,rooVarSignalEventCrossSection));

  std::cout << "Found: " << numEventsPassingTrigger << " events passing trigger selection." <<
    std::endl;
  std::cout << "Found: " << numEventsWithOneTrackPassingPreselection <<
    " events with at least one track passing preselection." << std::endl;
  std::cout << "Found: " << numEventsWithOneTrackOverIhCut <<
    " events with at least one track passing ih > 3.5" << std::endl;
  std::cout << "Found: " << numEventsWithOneTrackOverIasCut <<
    " events with at least one track passing ias > 0.35" << std::endl;
  std::cout << "Found: " << numEventsWithOneTrackOverIhAndIasCuts <<
    " events with at least one track passing ih > 3.5 and ias > 0.35" << std::endl;
  std::cout << std::endl;
  std::cout << "Found: " << numTracksPassingTrigger << " tracks passing trigger." <<
    std::endl;
  std::cout << "Found: " << numTracksPassingPreselection << " tracks passing preselections." <<
    std::endl;
  std::cout << "Added " << rooDataSetCandidates->numEntries() << " tracks into the dataset." << std::endl;


  // testing
  if(isMC_)
  {
    std::cout << std::endl;
    std::cout << "Number of initial MC events: " << numGenHSCPEvents
      << std::endl;

    std::cout << "fraction of events passing trigger selection: " << numEventsPassingTrigger/(float)numGenHSCPEvents
      << std::endl
      << std::endl << "tracks passing preselection/track passing trigger: " <<
      numTracksPassingPreselection/(float)numTracksPassingTrigger << std::endl;

    std::cout << "Found " << numTracksInSearchRegion << " tracks in search region ( P > " <<
      pSidebandThreshold_ << ", ih > " << ihSidebandThreshold_ << " )." << std::endl;

    std::cout << std::endl;

    std::cout << "Num events with 1 HSCP gen: " << numEventsWithOneHSCPGen << std::endl <<
      "\tnum events with 1 HSCP seen (Mu trigger): " << numEventsWithOneHSCPSeenMuonTrigger <<
      " eff = " << numEventsWithOneHSCPSeenMuonTrigger/(float)numEventsWithOneHSCPGen << std::endl <<
      "\tnum events with 1 HSCP seen (MET trigger): " << numEventsWithOneHSCPSeenMetTrigger <<
      " eff = " << numEventsWithOneHSCPSeenMetTrigger/(float)numEventsWithOneHSCPGen <<
      std::endl << "Num events with 2 HSCP gen: " << numEventsWithTwoHSCPGen << std::endl <<
      "\tnum events with 2 HSCP seen (Mu Trigger) : " << numEventsWithTwoHSCPSeenMuonTrigger <<
      " eff = " << numEventsWithTwoHSCPSeenMuonTrigger/(float)numEventsWithTwoHSCPGen << std::endl <<
      "\tnum events with 2 HSCP seen (MET Trigger) : " << numEventsWithTwoHSCPSeenMetTrigger <<
      " eff = " << numEventsWithTwoHSCPSeenMetTrigger/(float)numEventsWithTwoHSCPGen << std::endl;

  }


}

