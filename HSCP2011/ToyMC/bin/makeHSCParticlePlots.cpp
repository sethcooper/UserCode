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
  double dEdx_k (ana.getParameter<double>("dEdx_k"));
  double dEdx_c (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  bool matchToHSCP (ana.getParameter<bool>("MatchToHSCP"));
  bool isMC (ana.getParameter<bool>("IsMC"));

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  TH1F* pDistributionHist = fs.make<TH1F>("pDistribution","P;GeV",200,0,2000);
  pDistributionHist->Sumw2();
  TH1F* ptDistributionHist = fs.make<TH1F>("ptDistribution","Pt;GeV",200,0,2000);
  ptDistributionHist->Sumw2();
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
  pVsIhTitle+=";MeV/cm;GeV";
  pVsIhDistributionHist = fs.make<TH2F>(pVsIhName.c_str(),pVsIhTitle.c_str(),400,0,10,100,0,1000);
  pVsIhDistributionHist->Sumw2();
  // p vs Ias
  TH2F* pVsIasHist = fs.make<TH2F>("pVsIas","P vs Ias (SearchRegion+MassCut);;GeV",20,0,1,40,0,1000);
  pVsIasHist->Sumw2();
  TH2F* trackEtaVsPHist = fs.make<TH2F>("trackEtaVsP","Track #eta vs. p (SearchRegion+MassCut);GeV",4000,0,2000,24,0,2.4);
  trackEtaVsPHist->Sumw2();
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
  // ias NoM
  TH1F* iasNoMHist = fs.make<TH1F>("iasNoM","NoM (ias)",50,0,50);
  // ToF SB - p vs Ias
  TH2F* pVsIasToFSBHist;
  std::string pVsIasToFSBName = "trackPvsIasToFSB";
  std::string pVsIasToFSBTitle="Track P vs ias (ToF SB: #beta>1.075)";
  pVsIasToFSBTitle+=";;GeV";
  pVsIasToFSBHist = fs.make<TH2F>(pVsIasToFSBName.c_str(),pVsIasToFSBTitle.c_str(),400,0,1,100,0,1000);
  pVsIasToFSBHist->Sumw2();
  // ToF SB - p vs Ih
  TH2F* pVsIhToFSBHist;
  std::string pVsIhToFSBName = "trackPvsIhToFSB";
  std::string pVsIhToFSBTitle="Track P vs ih (ToF SB: #beta>1.075)";
  pVsIhToFSBTitle+=";MeV/cm;GeV";
  pVsIhToFSBHist = fs.make<TH2F>(pVsIhToFSBName.c_str(),pVsIhToFSBTitle.c_str(),400,0,10,100,0,1000);
  pVsIhToFSBHist->Sumw2();
  // Ih vs Ias
  TH2F* ihVsIasHist = fs.make<TH2F>("ihVsIas","Ih vs. Ias;;MeV/cm",400,0,1,400,0,10);
  ihVsIasHist->Sumw2();
  // p vs NoM
  TH2F* pVsNoMHist = fs.make<TH2F>("pVsNoM","Track P vs. NoM (Ias);;GeV",50,0,50,100,0,1000);
  // p vs NoM, central eta only
  TH2F* pVsNoMCentralEtaHist = fs.make<TH2F>("pVsNoMCentralEta","Track P vs. NoM (Ias), |#eta| < 0.9;;GeV",50,0,50,100,0,1000);
  // NoH vs NoM
  TH2F* nohVsNoMHist = fs.make<TH2F>("nohVsNoM","Number of hits vs. NoM (Ias);NoM;NoH",50,0,50,50,0,50);
  // NoH vs NoM, central eta
  TH2F* nohVsNoMCentralEtaHist = fs.make<TH2F>("nohVsNoMCentral","Number of hits vs. NoM (Ias), |#eta| < 0.9;NoM;NoH",50,0,50,50,0,50);
  // p vs NoH
  TH2F* pVsNoHHist = fs.make<TH2F>("pVsNoH","Track P vs. NoH;;GeV",50,0,50,100,0,1000);
  // p vs NoH, central eta only
  TH2F* pVsNoHCentralEtaHist = fs.make<TH2F>("pVsNoHCentralEta","Track P vs. NoH, |#eta| < 0.9;;GeV",50,0,50,100,0,1000);
  // p vs relPerr
  TH2F* pVsRelPerrHist = fs.make<TH2F>("pVsRelPerr","Track P vs. #Deltap/p;;GeV",100,0,1,100,0,1000);
  // p vs relPerr, central eta only
  TH2F* pVsRelPerrCentralEtaHist = fs.make<TH2F>("pVsRelPerrCentralEta","Track P vs. #Deltap/p, |#eta| < 0.9;;GeV",100,0,1,100,0,1000);

  // RooFit observables and dataset
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIp("rooVarIp","ip",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarPt("rooVarPt","pt",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);
  RooDataSet* rooDataSetAll = fs.make<RooDataSet>("rooDataSetAll","rooDataSetAll",RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,
        rooVarNoMias,rooVarEta,rooVarIp));


  int numEventsPassingTrigger = 0;
  int numEventsWithOneTrackPassingPreselection = 0;
  int numEventsWithOneTrackOverIhCut = 0;
  int numEventsWithOneTrackOverIasCut = 0;
  int numEventsWithOneTrackOverIhAndIasCuts = 0;

  int numTracksPassingTrigger = 0;
  int numTracksPassingPreselection = 0;
  int numTracksInSearchRegion = 0;

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
        if(isMC)
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

        if(!passesTrigger(ev))
          continue;

        numEventsPassingTrigger++;

        int numTracksPassingPreselectionThisEvent = 0;
        int numTracksPassingIhCutThisEvent = 0;
        int numTracksPassingIasCutThisEvent = 0;
        int numTracksPassingIhAndIasCutsThisEvent = 0;
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
          if(isMC && matchToHSCP)
          {
            int ClosestGen;
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)
              continue;
            NChargedHSCP = HowManyChargedHSCP(genColl);
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

          // apply preselections, not considering ToF
          if(!passesPreselection(hscp,dedxSObj,dedxMObj,tof,dttof,csctof,ev,false))
            continue;

          numTracksPassingPreselection++;
          numTracksPassingPreselectionThisEvent++;

          if(ih > 3.5)
            numTracksPassingIhCutThisEvent++;
          if(ias > 0.35)
            numTracksPassingIasCutThisEvent++;
          if(ih > 3.5 && ias > 0.35)
            numTracksPassingIhAndIasCutsThisEvent++;

          if(trackP > pSidebandThreshold && ih > ihSidebandThreshold)
          {
            numTracksInSearchRegion++;
            if(isMC)
            {
              float massSqr = (ih-dEdx_c)*pow(trackP,2)/dEdx_k;
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
          if(trackP <= pSidebandThreshold)
            pVsIhDistributionBRegionHist->Fill(ih,trackP);

          // fill some overall hists (already passed preselection)
          pDistributionHist->Fill(trackP);
          ptDistributionHist->Fill(trackPt);
          trackEtaVsPHist->Fill(trackP,fabs(trackEta));
          pVsIasHist->Fill(ias,trackP);
          pVsIasDistributionHist->Fill(ias,trackP);
          pVsIhDistributionHist->Fill(ih,trackP);
          iasNoMHist->Fill(iasNoM);
          ihVsIasHist->Fill(ias,ih);
          pVsNoMHist->Fill(iasNoM,trackP);
          if(fabs(trackEta) < 0.9)
            pVsNoMCentralEtaHist->Fill(iasNoM,trackP);
          nohVsNoMHist->Fill(iasNoM,trackNoH);
          if(fabs(trackEta) < 0.9)
            nohVsNoMCentralEtaHist->Fill(iasNoM,trackNoH);
          pVsNoHHist->Fill(trackNoH,trackP);
          if(fabs(trackEta) < 0.9)
            pVsNoHCentralEtaHist->Fill(trackNoH,trackP);
          pVsRelPerrHist->Fill(trackPtErr/trackPt,trackP);
          if(fabs(trackEta) < 0.9)
            pVsRelPerrCentralEtaHist->Fill(trackPtErr/trackP,trackP);


          // fill roodataset
          //TODO include TOF at some point?
          rooVarIas = ias;
          rooVarIp = ip;
          rooVarIh = ih;
          rooVarP = trackP;
          rooVarPt = trackPt;
          rooVarNoMias = iasNoM;
          rooVarEta = fabs(trackEta);
          rooDataSetAll->add(RooArgSet(rooVarIas,rooVarIh,rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIp));

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
          pVsIasToFSBHist->Fill(ias,trackP);
          pVsIhToFSBHist->Fill(ih,trackP);

        }

        if(numTracksPassingPreselectionThisEvent > 0)
          numEventsWithOneTrackPassingPreselection++;
        if(numTracksPassingIhCutThisEvent > 0)
          numEventsWithOneTrackOverIhCut++;
        if(numTracksPassingIasCutThisEvent > 0)
          numEventsWithOneTrackOverIasCut++;
        if(numTracksPassingIhAndIasCutsThisEvent > 0)
          numEventsWithOneTrackOverIhAndIasCuts++;
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
  std::cout << "Found " << numTracksInSearchRegion << " tracks in search region ( P > " <<
    pSidebandThreshold << ", ih > " << ihSidebandThreshold << " )." << std::endl;
  std::cout << "Added " << rooDataSetAll->numEntries() << " tracks into the dataset." << std::endl;


  // testing
  if(isMC)
  {
    std::cout << std::endl;
    std::cout << "Number of initial MC events: " << GetInitialNumberOfMCEvent(inputHandler_.files())
      << std::endl;
    std::cout << "fraction of events passing trigger selection: " << numEventsPassingTrigger/(float)GetInitialNumberOfMCEvent(inputHandler_.files())
      << std::endl << "tracks passing preselection/track passing trigger: " <<
      numTracksPassingPreselection/(float)numTracksPassingTrigger << std::endl;
  }


}

