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
#include "TRandom3.h"

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
#include "DataFormats/Math/interface/deltaR.h"

#endif

#include "commonFunctions.h"

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
  //double massCutIasHighPHighIh_ (ana.getParameter<double>("MassCut"));
  // dE/dx calibration
  //double dEdx_k (ana.getParameter<double>("dEdx_k"));
  //double dEdx_c (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  bool matchToHSCP (ana.getParameter<bool>("MatchToHSCP"));
  bool isMC (ana.getParameter<bool>("IsMC"));

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  //TFileDirectory dir = fs.mkdir("analyzeBasicPat");
  TH1F* pDistributionHist = fs.make<TH1F>("pDistribution","P;GeV",200,0,2000);
  pDistributionHist->Sumw2();
  // p vs Ias
  TH2F* pVsIasDistributionHist;
  std::string pVsIasName = "trackPvsIas";
  std::string pVsIasTitle="Track P vs ias";
  pVsIasTitle+=";;GeV";
  pVsIasDistributionHist = fs.make<TH2F>(pVsIasName.c_str(),pVsIasTitle.c_str(),400,0,1,100,0,1000);
  pVsIasDistributionHist->Sumw2();
  // p vs Ih
  TH2F* pVsIhDistributionHist;
  std::string pVsIhName = "trackPvsIh";
  std::string pVsIhTitle="Track P vs ih";
  pVsIhTitle+=";;GeV";
  pVsIhDistributionHist = fs.make<TH2F>(pVsIhName.c_str(),pVsIhTitle.c_str(),400,0,10,100,0,1000);
  pVsIhDistributionHist->Sumw2();
  // p vs Ias
  TH2F* pVsIasHist = fs.make<TH2F>("pVsIas","P vs Ias;;GeV",20,0,1,40,0,1000);
  pVsIasHist->Sumw2();
  TH2F* trackEtaVsPHist = fs.make<TH2F>("trackEtaVsP","Track #eta vs. p;GeV",4000,0,2000,24,0,2.4);
  trackEtaVsPHist->Sumw2();

  // RooFit observables and dataset
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);
  RooDataSet* rooDataSetAll = fs.make<RooDataSet>("rooDataSetAll","rooDataSetAll",RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarNoMias,rooVarEta));


  int numTracksInSearchRegion = 0;
  int numEventsPassingTrigger = 0;
  int numTracksPassingPreselection = 0;

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

        fwlite::Handle<reco::DeDxDataValueMap> dEdxMCollH;
        dEdxMCollH.getByLabel(ev, "dedxHarm2");
        if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

        fwlite::Handle< std::vector<reco::GenParticle> > genCollHandle;
        std::vector<reco::GenParticle> genColl;
        if(isMC)
        {
          genCollHandle.getByLabel(ev, "genParticles");
          if(!genCollHandle.isValid()){printf("GenParticle Collection NotFound\n");continue;}
          genColl = *genCollHandle;
        }

        if(!passesTrigger(ev))
          continue;

        numEventsPassingTrigger++;

        // loop over HSCParticles in this event
        for(unsigned int c=0;c<hscpColl.size();c++)
        {
          susybsm::HSCParticle hscp  = hscpColl[c];
          reco::MuonRef  muon  = hscp.muonRef();
          reco::TrackRef track = hscp.trackRef();
          if(track.isNull())
            continue;

          const reco::DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
          const reco::DeDxData& dedxMObj  = dEdxMCollH->get(track.key());

          // for signal MC
          int NChargedHSCP = 0;
          if(isMC && matchToHSCP)
          {
            int ClosestGen;
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)
              continue;
            NChargedHSCP = HowManyChargedHSCP(genColl);
          }

          float ih = dedxMObj.dEdx();
          float ias = dedxSObj.dEdx();
          float trackP = track->p();
          int iasNoM = dedxSObj.numberOfMeasurements();
          float trackEta = track->eta();

          if(!passesPreselection(hscp,dedxSObj,dedxMObj,ev))
            continue;

          numTracksPassingPreselection++;


          //int myFineNoMbin = findFineNoMBin(iasNoM);
          //if(myFineNoMbin<0)
          //  continue;

          if(ih > ihSidebandThreshold && trackP > pSidebandThreshold)
            numTracksInSearchRegion++;

          // fill some overall hists
          pDistributionHist->Fill(trackP);
          trackEtaVsPHist->Fill(trackP,fabs(trackEta));
          pVsIasHist->Fill(ias,trackP);
          pVsIasDistributionHist->Fill(ias,trackP);
          pVsIhDistributionHist->Fill(ih,trackP);

          // fill roodataset
          rooVarIas = ias;
          rooVarIh = ih;
          rooVarP = trackP;
          rooVarNoMias = iasNoM;
          rooVarEta = fabs(trackEta);
          rooDataSetAll->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarNoMias,rooVarEta));
        }
      }
      // close input file
      inFile->Close();
    }
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
  }
  // end of file/event loop
  
  pDistributionHist->Scale(scaleFactor_);

  std::cout << "Found: " << numEventsPassingTrigger << " events passing trigger selection." <<
    std::endl;
  std::cout << "Found: " << numTracksPassingPreselection << " tracks passing preselections." <<
    std::endl;
  std::cout << "Found " << numTracksInSearchRegion << " tracks in search region ( p > " <<
    pSidebandThreshold << ", ih > " << ihSidebandThreshold << " )." << std::endl;


  // testing
  if(isMC)
  {
    std::cout << "Number of initial MC events: " << GetInitialNumberOfMCEvent(inputHandler_.files())
      << std::endl;
  }


}

