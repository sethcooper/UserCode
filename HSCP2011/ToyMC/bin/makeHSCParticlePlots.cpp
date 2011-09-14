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

#include "TSystem.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}

#if !defined(__CINT__) && !defined(__MAKECINT__)

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
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

// ****** helper functions
std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}

//
std::string floatToString(float num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}
//
int findFineNoMBin(int nom)
{
  if(nom >= 30) return 29;

  return nom-1;
}

// ****** main
int main(int argc, char ** argv)
{
  // load framework libraries
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  // initialize command line parser
  optutl::CommandLineParser parser("Analyze FWLite Histograms");

  // set option defaults
  parser.integerValue ("maxEvents"  ) = -1;
  parser.integerValue ("outputEvery") = 10000;
  parser.stringValue  ("outputFile" ) = "makeHSCParticlePlots.root";
  // must specify input files

  // parse arguments
  parser.parseArguments(argc, argv);
  int maxEvents_ = parser.integerValue("maxEvents");
  unsigned int outputEvery_ = parser.integerValue("outputEvery");
  std::string outputFile_ = parser.stringValue("outputFile");
  std::vector<std::string> inputFiles_ = parser.stringVector("inputFiles");

  // preselections
  const int minTrackNoH = 11;
  //const float minTrackValidFraction = 0.8;
  //const int minValidPixelHits = 2;
  //const int minIasNoM = 5;
  //const int minTofNdof = 8;
  //const int minTofNdofDt = 6;
  //const int minTofNdofCsc = 6;
  //const int minTrackQualityMask = 2;
  //const float maxTrackChi2OverNdf = 5.0;
  //const float minTrackPt = 35.0;
  //const float minIas = 0.0;
  //const float minIh = 3.0;
  //const float minTofInvBeta = 1.0;
  //const float maxTofIntBetaErr = 0.07;
  //const int minNumVertices = 1;
  //const float maxV3D = 2.0;
  //const float maxTrackEtIso = 50;
  //const float maxCalEOverPIso = 0.3;
  //const float maxTrackPtErr = 0.25;
  //const float maxTrackEta = 2.5;

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputFile_.c_str());
  // declare binning and histos
  const int nNomBins = 30;
  // p bins
  const int nPBins = 200;
  const int pBinLow = 0;
  const int pBinHigh = 1000;
  // Ih bins
  const int nIhBins = 200;
  const int ihBinLow = 0;
  const int ihBinHigh = 20;
  // Ias bins
  const int nIasBins = 100;
  const int iasBinLow = 0;
  const int iasBinHigh = 1;
  // eta Bins
  const int nEtaBins = 25;
  const int etaBinLow = 0;
  const float etaBinHigh = 2.5;

  // no (pre)selections
  TH2F* dedxIhEta2dInNoMBins[nNomBins]; // 1-30 NoM; dE/dx, eta
  TH2F* dedxIasEta2dInNoMBins[nNomBins]; // 1-30 NoM; dE/dx, eta
  // track hits preselection
  TH2F* dedxIhEta2dInNoMBinsTrackHitPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  TH2F* dedxIasEta2dInNoMBinsTrackHitPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// track valid frac presel
  //TH2F* dedxIhEta2dInNoMBinsTrackValidFracPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsTrackValidFracPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// pixel hits presel
  //TH2F* dedxIhEta2dInNoMBinsPixelHitPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsPixelHitPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// track quality
  //TH2F* dedxIhEta2dInNoMBinsTrackQualityMaskPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsTrackQualityMaskPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// track chi2/ndf
  //TH2F* dedxIhEta2dInNoMBinsTrackChi2NdfPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsTrackChi2NdfPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// num vertices
  //TH2F* dedxIhEta2dInNoMBinsNumVerticesPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsNumVerticesPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// V3D
  //TH2F* dedxIhEta2dInNoMBinsV3DPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsV3DPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// track Et iso
  //TH2F* dedxIhEta2dInNoMBinsTrackEtIsoPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsTrackEtIsoPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// cal E/p iso
  //TH2F* dedxIhEta2dInNoMBinsCalEPIsoPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsCalEPIsoPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //// pt error
  //TH2F* dedxIhEta2dInNoMBinsTrackPtErrPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  //TH2F* dedxIasEta2dInNoMBinsTrackPtErrPresel[nNomBins]; // 1-30 NoM; dE/dx, eta
  // book 'em
  std::string histNameIhBeg = "dedxIh2d";
  std::string histNameIasBeg = "dedxIas2d";
  std::string histTitleIhBeg = "dE/dx Ih 2D ";
  std::string histTitleIasBeg = "dE/dx Ias 2D ";
  for(int i=0; i<30; ++i)
  {
    std::string histNameIh = "dedxIh2dInNoMBin";
    histNameIh+=intToString(i+1);
    std::string histTitleIh = "dE/dx Ih 2D in NoM bin ";
    histTitleIh+=intToString(i+1);
    histTitleIh+=";MeV/cm;#eta";
    std::string histNameIas = "dedxIas2dInNoMBin";
    histNameIas+=intToString(i+1);
    std::string histTitleIas = "dE/dx Ias 2D in NoM bin ";
    histTitleIas+=intToString(i+1);
    histTitleIh+=";;#eta";
    // track hit presel
    std::string histNameIhTrackHitsPresel = histNameIhBeg;
    histNameIhTrackHitsPresel+="TrackHitPreselNomBin";
    histNameIhTrackHitsPresel+=intToString(i+1);
    std::string histTitleIhTrackHitsPresel = histTitleIhBeg;
    histTitleIhTrackHitsPresel+=" after track hit > ";
    histTitleIhTrackHitsPresel+=intToString(minTrackNoH);
    histTitleIhTrackHitsPresel+=" in NoM bin ";
    histTitleIhTrackHitsPresel+=intToString(i+1);
    histTitleIhTrackHitsPresel+=";MeV/cm;#eta";
    std::string histNameIasTrackHitsPresel = histNameIasBeg;
    histNameIasTrackHitsPresel+="TrackHitPreselNomBin";
    histNameIasTrackHitsPresel+=intToString(i+1);
    std::string histTitleIasTrackHitsPresel = histTitleIasBeg;
    histTitleIasTrackHitsPresel+=" after track hit > ";
    histTitleIasTrackHitsPresel+=intToString(minTrackNoH);
    histTitleIasTrackHitsPresel+=" in NoM bin ";
    histTitleIasTrackHitsPresel+=intToString(i+1);
    histTitleIasTrackHitsPresel+=";;#eta";

    dedxIhEta2dInNoMBins[i] = fs.make<TH2F>(histNameIh.c_str(),histTitleIh.c_str(),nIhBins,ihBinLow,ihBinHigh,nEtaBins,etaBinLow,etaBinHigh);
    dedxIasEta2dInNoMBins[i] = fs.make<TH2F>(histNameIas.c_str(),histTitleIas.c_str(),nIasBins,iasBinLow,iasBinHigh,nEtaBins,etaBinLow,etaBinHigh);
    // track hit presel
    dedxIhEta2dInNoMBinsTrackHitPresel[i] = fs.make<TH2F>(histNameIhTrackHitsPresel.c_str(),histTitleIhTrackHitsPresel.c_str(),nIhBins,ihBinLow,ihBinHigh,nEtaBins,etaBinLow,etaBinHigh);
    dedxIasEta2dInNoMBinsTrackHitPresel[i] = fs.make<TH2F>(histNameIasTrackHitsPresel.c_str(),histTitleIasTrackHitsPresel.c_str(),nIasBins,iasBinLow,iasBinHigh,nEtaBins,etaBinLow,etaBinHigh);
  }

  // chain the input files
  fwlite::ChainEvent ev(inputFiles_);

  int ievt = 0;
  for (ev.toBegin(); ! ev.atEnd(); ++ev, ++ievt)
  {
    // break loop if maximal number of events is reached 
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
    // simple event counter
    if(outputEvery_!=0 ? (ievt>0 && ievt%outputEvery_==0) : false) 
      std::cout << "  processing event: " << ievt << std::endl;

    fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
    hscpCollHandle.getByLabel(ev,"HSCParticleProducer");
    if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
    const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

    fwlite::Handle<reco::DeDxDataValueMap> dEdxSCollH;
    dEdxSCollH.getByLabel(ev, "dedxASmi");
    if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");continue;}

    fwlite::Handle<reco::DeDxDataValueMap> dEdxMCollH;
    dEdxMCollH.getByLabel(ev, "dedxHarm2");
    if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

    // loop over HSCParticles in this event
    for(unsigned int c=0;c<hscpColl.size();c++)
    {
      susybsm::HSCParticle hscp  = hscpColl[c];
      reco::MuonRef  muon  = hscp.muonRef();
      reco::TrackRef track = hscp.trackRef();
      if(track.isNull())continue;

      const reco::DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
      const reco::DeDxData& dedxMObj  = dEdxMCollH->get(track.key());
      int ihNoM = findFineNoMBin(dedxMObj.numberOfMeasurements());
      int iasNoM = findFineNoMBin(dedxSObj.numberOfMeasurements());
      float ih = dedxMObj.dEdx();
      float ias = dedxSObj.dEdx();

      dedxIhEta2dInNoMBins[ihNoM]->Fill(ih,fabs(track->eta()));
      dedxIasEta2dInNoMBins[iasNoM]->Fill(ias,fabs(track->eta()));
      // track hit presel
      if(track->found() > minTrackNoH)
      {
        dedxIhEta2dInNoMBinsTrackHitPresel[ihNoM]->Fill(ih,fabs(track->eta()));
        dedxIasEta2dInNoMBinsTrackHitPresel[iasNoM]->Fill(ias,fabs(track->eta()));
      }
    }
  }
}

