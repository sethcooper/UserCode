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

#include <iomanip>
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
  // beta
  TH1F* hscpBetaDistribution = fs.make<TH1F>("hscpBetaDistribution","HSCP Gen #beta",20,0,1);
  // eta
  TH1F* hscpEtaDistribution = fs.make<TH1F>("hscpEtaDistribution","HSCP Gen #eta",300,-3,3);
  // pt
  TH1F* hscpPtDistribution = fs.make<TH1F>("hscpPtDistribution","HSCP Gen Pt",100,0,2000);
  // p
  TH1F* hscpPDistribution = fs.make<TH1F>("hscpPDistribution","HSCP Gen P",100,0,2000);
  // p vs Ias
  TH2F* pVsIasDistributionSearchRegionHist;
  std::string pVsIasSearchRegionName = "trackPvsIasSearchRegion";
  std::string pVsIasSearchRegionTitle="Track P vs ias (SearchRegion+MassCut)";
  pVsIasSearchRegionTitle+=";;GeV";
  pVsIasDistributionSearchRegionHist = fs.make<TH2F>(pVsIasSearchRegionName.c_str(),pVsIasSearchRegionTitle.c_str(),400,0,1,100,0,1000);
  pVsIasDistributionSearchRegionHist->Sumw2();
  // pt vs Ias
  TH2F* ptVsIasHist;
  std::string ptVsIasName = "trackPtvsIasSignal";
  std::string ptVsIasTitle="Track Pt vs Ias (signal, trig, no presel)";
  ptVsIasTitle+=";;GeV";
  ptVsIasHist = fs.make<TH2F>(ptVsIasName.c_str(),ptVsIasTitle.c_str(),400,0,1,200,0,1000);
  ptVsIasHist->Sumw2();
  // pt vs Ias -- mass cut
  TH2F* ptVsIasMassCutHist;
  std::string ptVsIasMassCutName = "trackPtvsIasMassCutSignal";
  std::string ptVsIasMassCutTitle="Track Pt vs Ias after mass cut (signal, trig, no presel)";
  ptVsIasMassCutTitle+=";;GeV";
  ptVsIasMassCutHist = fs.make<TH2F>(ptVsIasMassCutName.c_str(),ptVsIasMassCutTitle.c_str(),400,0,1,200,0,1000);
  ptVsIasMassCutHist->Sumw2();

  PlotStruct afterPreselectionPlots(fs,"AfterPreselection","after preselection");
  PlotStruct beforePreselectionPlots(fs,"BeforePreselection","before preselection");

  // Trees for keeping track of the data
  HSCPTreeContent treeContent;
  TTree* allCandidatesTree = fs.make<TTree>("allCandidatesTree","allCandidatesTree");
  setBranches(allCandidatesTree,treeContent);

  TTree* oneCandidateTree = fs.make<TTree>("oneCandidatesTree","oneCandidatesTree");
  setBranches(oneCandidateTree,treeContent);

  TTree* iasShiftTree = fs.make<TTree>("iasShiftTree","iasShiftTree");
  setBranches(iasShiftTree,treeContent);

  TTree* ihShiftTree = fs.make<TTree>("ihShiftTree","ihShiftTree");
  setBranches(ihShiftTree,treeContent);

  TTree* ptShiftTree = fs.make<TTree>("ptShiftTree","ptShiftTree");
  setBranches(ptShiftTree,treeContent);
  // HSCP only
  TH1F* numGenHSCPEventsHist = fs.make<TH1F>("numGenHSCPEvents","num gen hscp events",1,0,1);
  TH1F* numGenHSCPTracksHist = fs.make<TH1F>("numGenHSCPTracks","num gen hscp tracks",1,0,1);
  TH1F* numGenChargedHSCPTracksHist = fs.make<TH1F>("numGenChargedHSCPTracks","num gen charged hscp tracks",1,0,1);
  TH1F* signalEventCrossSectionHist = fs.make<TH1F>("signalEventCrossSection","signal event cross section",1,0,1);


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
        int tempNoMih = 0;
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

          int ClosestGen = -1;
          int numSeenHSCPThisEvt = 0;
          // attempt match track to gen hscp, look at gen/detected hscps
          for(unsigned int c=0;c<hscpColl.size();c++)
          {
            susybsm::HSCParticle hscp  = hscpColl[c];
            reco::MuonRef  muon  = hscp.muonRef();
            reco::TrackRef track = hscp.trackRef();
            if(track.isNull())
              continue;

            // match to gen hscp
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)
              continue;
            else
              numSeenHSCPThisEvt++;

          }
          numHSCPSeenPerEventHist->Fill(numSeenHSCPThisEvt);
          // loop over gen particles
          for(unsigned int g=0;g<genColl.size();g++)
          {
            if(genColl[g].pt()<5)continue;
            if(genColl[g].status()!=1)continue;
            int AbsPdg=abs(genColl[g].pdgId());
            if(AbsPdg<1000000)continue;
            //if(onlyCharged && (AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324))continue; //Skip neutral gluino RHadrons
            //if(onlyCharged && (AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333))continue;  //skip neutral stop RHadrons
            //if(beta1<0){beta1=genColl[g].p()/genColl[g].energy();}else if(beta2<0){beta2=genColl[g].p()/genColl[g].energy();return;}
            //XXX FIXME
            //float genMass = genColl[g].generated_mass();
            //double genP = sqrt(pow(genColl[g].momentum().px(),2) + pow(genColl[g].momentum().py(),2) + pow(genColl[g].momentum().pz(),2) );
            //hscpBetaDistribution->Fill(genP/sqrt(pow(genP,2)+pow(genMass,2)));
            //hscpEtaDistribution->Fill(genColl[g].eta());
            //hscpPtDistribution->Fill(genColl[g].pt());
            //hscpPDistribution->Fill(genP);
          }

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
        treeContent.lumiSection = lumiSection;
        treeContent.runId = runNumber;
        treeContent.eventId = eventNumber;
        //XXX ignore real data taken with tighter RPC trigger (355.227/pb) -- from Analysis_Samples.h
        if(!isMC_ && runNumber < 165970)
          continue;

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
          int ihNoM = dedxMObj.numberOfMeasurements();
          float trackEta = track->eta();
          int trackNoH = track->found();
          float trackPtErr = track->ptError();
          treeContent.ias = ias;
          treeContent.ih = ih;
          treeContent.ip = ip;
          treeContent.p = trackP;
          treeContent.pt = trackPt;
          treeContent.nomIas = iasNoM;
          treeContent.nomIh = ihNoM;
          treeContent.eta = trackEta;

          // systematics datasets for MC
          //TODO: FIXME: apply shift, then preselect/choose highest ias, then add to dataset
          if(isMC_)
          {
            TRandom3 myRandom;
            //TODO include TOF at some point?
            // ias shift
            double newIas = ias + myRandom.Gaus(0,0.083) + 0.015; // from YK results Nov 21 2011 hypernews thread
            treeContent.ias = newIas;
            iasShiftTree->Fill();
            treeContent.ias = ias; // reset to original value
            // ih shift
            double newIh = ih*1.036; // from SIC results, Nov 10 2011 HSCP meeting
            treeContent.ih = newIh;
            ihShiftTree->Fill();
            treeContent.ih = ih; // reset to original value
            // pt shift (and p shift) -- from MU-10-004-001 -- SIC report Jun 7 2011 HSCP meeting
            double newInvPt = 1/trackPt+0.000236-0.000135*pow(trackEta,2)+track->charge()*0.000282*TMath::Sin(track->phi()-1.337);
            treeContent.pt = 1/newInvPt;
            treeContent.p = rooVarPt.getVal()/TMath::Sin(track->theta());
            ptShiftTree->Fill();
            treeContent.p = trackPt; // reset
            treeContent.pt = trackP; // reset
          }

          if(isMC_)
          {
            ptVsIasHist->Fill(ias,trackPt);
            float massSqr = (ih-dEdx_c_)*pow(trackP,2)/dEdx_k_;
            if(sqrt(massSqr) >= massCutIasHighPHighIh_)
              ptVsIasMassCutHist->Fill(ias,trackPt);
          }


          // apply preselections, not considering ToF
          if(!passesPreselection(hscp,dedxSObj,dedxMObj,tof,dttof,csctof,ev,false,beforePreselectionPlots))
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
          float massSqr = (ih-dEdx_c_)*pow(trackP,2)/dEdx_k_;
          if(massSqr >= 0)
            afterPreselectionPlots.massHist->Fill(sqrt(massSqr));
          // fill pt vs ias only for ABC regions
          if(!(ias > 0.1 && trackPt > 50))
            afterPreselectionPlots.ptVsIasHist->Fill(ias,trackPt);
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


          allCandidatesTree->Fill();
          if(ias > tempIas)
          {
            tempIas = ias;
            tempIh = ih;
            tempP = trackP;
            tempPt = trackPt;
            tempNoMias = iasNoM;
            tempNoMih = ihNoM;
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
          treeContent.ias = tempIas;
          treeContent.ih = tempIh;
          treeContent.p = tempP;
          treeContent.pt = tempPt;
          treeContent.nomIas = tempNoMias;
          treeContent.nomIh = tempNoMih;
          treeContent.eta = tempEta;
          treeContent.lumiSection = tempLumiSection;
          treeContent.runId = tempRun;
          treeContent.eventId = tempEvent;
          oneCandidateTree->Fill();
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

  numGenHSCPEventsHist->Fill(0.5,numGenHSCPEvents);
  numGenHSCPTracksHist->Fill(0.5,numGenHSCPTracks);
  numGenChargedHSCPTracksHist->Fill(0.5,numGenChargedHSCPTracks);
  signalEventCrossSectionHist->Fill(0.5,signalEventCrossSection_);

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
  std::cout << "Added " << allCandidatesTree->GetEntries() << " tracks into the dataset." << std::endl;


  double totalTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(1);

  if(isMC_)
  {
    totalTracks = numGenHSCPTracks;
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
  double triggeredTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(1);
  double nomIhTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(2);
  double nohTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(3);
  double validFracTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(4);
  double pixelHitTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(5);
  double nomIasTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(6);
  double qualityMaskTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(7);
  double chi2NdfTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(8);
  double etaTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(9);
  double ptErrTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(10);
  double v3dTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(11);
  double trackIsoTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(12);
  double caloIsoTracks = beforePreselectionPlots.tracksVsPreselectionsHist->GetBinContent(13);
  std::cout << endl << endl;
  // preselection table
  std::cout << "Table of preselections" << std::endl;
  std::cout << "-------------------------------------------" << std::endl;
  std::cout << setw(10) << "Cut " << setw(10) << "Number" << setw(15) << "Percentage" << std::endl;
  std::cout << "-------------------------------------------" << std::endl;
  if(isMC_)
    std::cout << setw(10) << "Gen " << setw(10) << totalTracks << setw(15) << "100%" << std::endl;
  std::cout << setw(10) << "Trigger " << setw(10) << triggeredTracks << setw(15) << 100*triggeredTracks/totalTracks << std::endl;
  std::cout << "-------------------------------------------" << std::endl;
  std::cout << setw(10) << "NoM Ih " << setw(10) << nomIhTracks << setw(15) << 100*nomIhTracks/triggeredTracks << std::endl;
  std::cout << setw(10) << "NoH " << setw(10) << nohTracks << setw(15) << 100*nohTracks/nomIhTracks << std::endl;
  std::cout << setw(10) << "validFrac " << setw(10) << validFracTracks << setw(15) << 100*validFracTracks/nohTracks << std::endl;
  std::cout << setw(10) << "pixelHit " << setw(10) << pixelHitTracks << setw(15) << 100*pixelHitTracks/validFracTracks << std::endl;
  std::cout << setw(10) << "NoM Ias " << setw(10) << nomIasTracks << setw(15) << 100*nomIasTracks/pixelHitTracks << std::endl;
  std::cout << setw(10) << "QualMask " << setw(10) << qualityMaskTracks << setw(15) << 100*qualityMaskTracks/nomIasTracks << std::endl;
  std::cout << setw(10) << "Chi2Ndf " << setw(10) << chi2NdfTracks << setw(15) << 100*chi2NdfTracks/qualityMaskTracks << std::endl;
  std::cout << setw(10) << "Eta " << setw(10) << etaTracks << setw(15) << 100*etaTracks/chi2NdfTracks << std::endl;
  std::cout << setw(10) << "PtErr " << setw(10) << ptErrTracks << setw(15) << 100*ptErrTracks/etaTracks << std::endl;
  std::cout << setw(10) << "v3D " << setw(10) << v3dTracks << setw(15) << 100*v3dTracks/ptErrTracks << std::endl;
  std::cout << setw(10) << "trackIso " << setw(10) << trackIsoTracks << setw(15) << 100*trackIsoTracks/v3dTracks << std::endl;
  std::cout << setw(10) << "caloIso " << setw(10) << caloIsoTracks << setw(15) << 100*caloIsoTracks/trackIsoTracks << std::endl;
  std::cout << "-------------------------------------------" << std::endl;
  std::cout << setw(10) << "All " << setw(10) << caloIsoTracks << setw(15) << 100*caloIsoTracks/triggeredTracks << std::endl;


}

