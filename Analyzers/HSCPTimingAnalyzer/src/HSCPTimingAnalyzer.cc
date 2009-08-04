// -*- C++ -*-
//
// Package:    HSCPTimingAnalyzer
// Class:      HSCPTimingAnalyzer
// 
/**\class HSCPTimingAnalyzer HSCPTimingAnalyzer.cc Analyzers/HSCPTimingAnalyzer/src/HSCPTimingAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER
//         Created:  Wed Dec 17 23:20:43 CET 2008
// $Id: HSCPTimingAnalyzer.cc,v 1.6 2009/06/02 15:25:44 scooper Exp $
//
//


// system include files
#include <memory>
#include <vector>
#include <map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

//
#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
// Geometry
#include "DataFormats/GeometryVector/interface/GlobalPoint.h" 
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/EcalAlgo/interface/EcalBarrelGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "RecoCaloTools/Navigation/interface/CaloNavigator.h"

// Magnetic field
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

// *** for TrackAssociation
#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/Common/interface/Handle.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "TrackingTools/Records/interface/DetIdAssociatorRecord.h"
#include "TrackingTools/TrackAssociator/interface/DetIdAssociator.h"
#include "TrackingTools/TrackAssociator/interface/CachedTrajectory.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixStateInfo.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"


// class decleration
//

class HSCPTimingAnalyzer : public edm::EDAnalyzer {
   public:
      explicit HSCPTimingAnalyzer(const edm::ParameterSet&);
      ~HSCPTimingAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::vector<SteppingHelixStateInfo> calcEcalDeposit(const FreeTrajectoryState* glbInnerState,
          const FreeTrajectoryState* glbOuterState,
          const FreeTrajectoryState* innInnerState,
          const FreeTrajectoryState* innOuterState,
          const TrackAssociatorParameters& parameters);
      std::vector<SteppingHelixStateInfo> calcDeposit(const FreeTrajectoryState* glbInnerState,
          const FreeTrajectoryState* glbOuterState,
          const FreeTrajectoryState* innInnerState,
          const FreeTrajectoryState* innOuterState,
          const TrackAssociatorParameters& parameters,
          const DetIdAssociator& associator);
      int getDetailedTrackLengthInXtals(std::map<int,float>& XtalInfo, 
          std::map<int,float>& muonCrossedXtalMap, 
          double& totalLengthCurved,
          GlobalPoint& internalPointCurved, 
          GlobalPoint& externalPointCurved,
          const CaloGeometry* theGeometry, 
          const CaloTopology * theTopology, 
          const std::vector<SteppingHelixStateInfo>& neckLace);
      void addStepToXtal(std::map<int,float>& xtalsMap,
          std::map<int,float>& muonCrossedXtalMap,
          DetId aDetId,
          float step);
      std::vector<DetId> matrixDetId(const CaloTopology* topology, 
          DetId id, 
          int ixMin,
          int ixMax,
          int iyMin,
          int iyMax);


      // ----------member data ---------------------------
      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag EBDigiCollection_;
      edm::InputTag muonCollection_;
      edm::InputTag trackCollection_;
      edm::ESHandle<DetIdAssociator> ecalDetIdAssociator_;
      edm::ESHandle<CaloGeometry> theCaloGeometry_;
      edm::ESHandle<GlobalTrackingGeometry> theTrackingGeometry_;
      edm::ESHandle<MagneticField> bField_;

      TH1D* simHitsTimeHist_;
      TH1D* simHitsEnergyHist_;
      TH1D* recHitTimeSimHitTimeHist_;
      TH1D* recHitTimeHist_;
      TH1D* recHitEnergyHist_;
      TH1D* recHitMaxEnergyHist_;
      TH1D* recHitMaxEnergyTimingHist_;
      TH1D* energyOfTrackMatchedHits_;
      TH1D* timeOfTrackMatchedHits_;
      TH2D* timeVsEnergyOfTrackMatchedHits_;
      TH1D* crossedEnergy_;
      TH2D* recHitTimeVsSimHitTime_;
      TH1D* simHitsPerEventHist_;
      TH2D* recHitTimeSimHitTimeVsEnergy_;
      TH1D* recHitsPerTrackHist_;
      TH1F* deDx3x3Hist_;
      TH1F* deDx3x3CorrectedHist_;
      TH1F* deDx5x5Hist_;
      TH1F* deDx5x5CorrectedHist_;
      TH1F* numCrysIn5x5Hist_;
      TH1F* numCrysIn3x3Hist_;

      TH1F* deDxHist_;
      TH1F* trackLengthPerCryHist_;
      TH1F* numMatchedCrysPerEventHist_;
      TH2F* numCrysCrossedVsNumHitsFoundHist_;
      TH2F* deDxVsMomHist_;
      TH2F* hit1EnergyVsHit2EnergyHist_;
      TH2F* hit1LengthVsHit2LengthHist_;
      TH2F* hit1DeDxVsHit2DeDxHist_;

      TH1D* muonEcalMaxEnergyTimingHist_;
      
      TProfile* recHitMaxEnergyShapeProfile_;
      TGraph* recHitMaxEnergyShapeGraph_;

      TTree* energyAndTimeNTuple_;

      TFile* file_;

      std::vector<double> sampleNumbers_;
      std::vector<double> amplitudes_;

      TrackDetectorAssociator trackAssociator_;
      TrackAssociatorParameters trackParameters_;

      double minTrackPt_;
      std::string fileName_;
      double contCorr_;

      float cryEnergy_;
      float cryTime_;
      float cryChi2_;
      float cryTrackLength_;
      float muonPt_;
      int numCrysCrossed_;
      int eventNum_;
      
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//
//edm::Service<TFileService> fileService;

//
// constructors and destructor
//
HSCPTimingAnalyzer::HSCPTimingAnalyzer(const edm::ParameterSet& iConfig) :
  EBHitCollection_ (iConfig.getParameter<edm::InputTag>("EBRecHitCollection")),
  EEHitCollection_ (iConfig.getParameter<edm::InputTag>("EERecHitCollection")),
  EBDigiCollection_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
  muonCollection_ (iConfig.getParameter<edm::InputTag> ("muonCollection")),
  trackCollection_ (iConfig.getParameter<edm::InputTag> ("trackCollection")),
  minTrackPt_ (iConfig.getParameter<double>("minimumTrackPt")),
  fileName_ (iConfig.getUntrackedParameter<std::string>("RootFileName","hscpTimingAnalyzer.root")),
  contCorr_ (iConfig.getUntrackedParameter<double>("containmentCorrection",1.09))
{
   //now do what ever initialization is needed
   file_ = new TFile(fileName_.c_str(),"RECREATE");
   //gROOT->cd();  // Causes Root Fill error after ~ 8k events
   file_->cd();

   simHitsEnergyHist_ = new TH1D("simHitsEnergy","Energy of sim hits [GeV]",2500,0,10);
   simHitsTimeHist_ = new TH1D("simHitsTime","Time of sim hits [ns]",500,-25,25);
   recHitTimeSimHitTimeHist_ = new TH1D("recHitTimeSimHitTime","RecHit time - simHit time [ns]",500,-25,25);
   recHitTimeHist_ = new TH1D("recHitTime","RecHit time",500,-25,25);
   recHitEnergyHist_ = new TH1D("recHitEnergy","RecHit energy [GeV]",2500,0,10);
   recHitMaxEnergyHist_ = new TH1D("recHitMaxEnergy","Energy of max. ene. recHit [GeV]",2500,0,10);
   recHitMaxEnergyTimingHist_ = new TH1D("recHitMaxEnergyTiming","Timing of max ene. recHit [ns]",500,-25,25);
   recHitMaxEnergyShapeProfile_ = new TProfile("recHitMaxEnergyShapeProfile","Shape of max energy hits [ADC]",10,0,10);
   energyOfTrackMatchedHits_ = new TH1D("energyOfTrackMatchedHits", "Energy of track-matched hits [GeV]",2500,0,10);
   timeOfTrackMatchedHits_ = new TH1D("timeOfTrackMatchedHits", "Time of track-matched hits [ns]",500,-25,25);
   timeVsEnergyOfTrackMatchedHits_ = new TH2D("timeVsEnergyOfTrackMatchedHits","Time [ns] vs. energy [GeV] for track-matched hits",2500,0,10,500,-25,25);
   crossedEnergy_ = new TH1D("crossedEcalEnergy", "Total energy of track-crossing xtals [GeV]",2500,0,10);
   recHitTimeVsSimHitTime_ = new TH2D("recHitTimeVsSimHitTime","recHitTime vs. simHitTime [ns]",500,-25,25,500,-25,25);
   simHitsPerEventHist_ = new TH1D("simHitsPerEvent","Number of SimHits per event",25,0,25);
   recHitTimeSimHitTimeVsEnergy_ = new TH2D("recHitTimeSimHitTimeVsEnergy","recHitTime-simHitTime vs. RecHit energy",2500,0,10,500,-25,25);
   recHitsPerTrackHist_ = new TH1D("ecalRecHitsPerTrack","Number of Ecal RecHits per track",10,0,10);

   deDxHist_ = new TH1F("dedxHist","dE/dx of each crystal;dE/dx [MeV/cm]",10000,0,5000);
   deDxVsMomHist_ = new TH2F("dedxVsMomHist","dE/dx of each crystal;p [GeV/c];dE/#rho dx [MeV cm^{2}/g]",500,0,500,20,0,20);
   trackLengthPerCryHist_ = new TH1F("trackLengthPerCry","Track length in each crystal; [cm]",500,0,50);
   numMatchedCrysPerEventHist_ = new TH1F("numMatchedCrysPerEvent","Number of track-matched (crossed) crystals per event",10,0,10);
   numCrysCrossedVsNumHitsFoundHist_ = new TH2F("crysCrossedVsHitsFound","Number of crystals crossed vs. number of RecHits found",10,0,10,10,0,10);
   hit1EnergyVsHit2EnergyHist_ = new TH2F("hit1EnergyVsHit2Energy","Hit 1 energy vs. hit 2 energy [GeV]",500,0,5,500,0,5);
   hit1LengthVsHit2LengthHist_ = new TH2F("hit1LengthVsHit2Length","Hit 1 length vs. hit 2 length [cm]",500,0,50,500,0,50);
   hit1DeDxVsHit2DeDxHist_ = new TH2F("hit1DeDxVsHit2DeDx","Hit 1 dE/dx vs. hit 2 dE/dx [MeV/cm]",200,0,100,200,0,100);

   deDx3x3Hist_ = new TH1F("dedx3x3Hist","dE/dx of crystals using 3x3 cluster energy [MeV/cm]",10000,0,5000);
   deDx3x3CorrectedHist_ = new TH1F("dedx3x3CorrectedHist","dE/dx of crystals using 3x3 cluster energy with cont. corr. [MeV/cm]",10000,0,5000);
   deDx5x5Hist_ = new TH1F("dedx5x5Hist","dE/dx of crystals using 5x5 cluster energy [MeV/cm]",10000,0,5000);
   deDx5x5CorrectedHist_ = new TH1F("dedx5x5CorrectedHist","dE/dx of crystals using 5x5 cluster energy with cont. corr. [MeV/cm]",10000,0,5000);
   numCrysIn5x5Hist_ = new TH1F("numCrysIn5x5","Number of recHits in 5x5 around main cry",25,1,26);
   numCrysIn3x3Hist_ = new TH1F("numCrysIn3x3","Number of recHits in 3x3 around main cry",9,1,10);
   
   muonEcalMaxEnergyTimingHist_ = new TH1D("muonEcalRecHitMaxEnergyTiming","Timing of max ene. recHit from muon [ns]",500,-25,25);
   
   //energyAndTimeNTuple_ = fileService->make<TNtuple>("energyAndTimeNTuple","Energy and Time for track-matched RecHits","energy:time");
   // Let's make an entry for each track-matched crystal we find in the event
   // For crossed RecHits that are missing, insert -1,-1,-1 for energy/time/chi2 ?
   energyAndTimeNTuple_ = new TTree("energyAndTimeNTuple","Energy and Time for track-matched RecHits");
   energyAndTimeNTuple_->Branch("crystalEnergy",&cryEnergy_,"cryEnergy/F");
   energyAndTimeNTuple_->Branch("crystalTime",&cryTime_,"cryTime/F");
   energyAndTimeNTuple_->Branch("crystalChi2",&cryChi2_,"cryChi2/F");
   energyAndTimeNTuple_->Branch("crystalTrackLength",&cryTrackLength_,"cryTrackLength/F");
   energyAndTimeNTuple_->Branch("muonPt",&muonPt_,"muonPt/F");
   energyAndTimeNTuple_->Branch("numCrysCrossed",&numCrysCrossed_,"numCrysCrossed/I");
   energyAndTimeNTuple_->Branch("eventNum",&eventNum_,"eventNum/I");


   // TrackAssociator parameters
   edm::ParameterSet trkParameters = iConfig.getParameter<edm::ParameterSet>("TrackAssociatorParameters");
   trackParameters_.loadParameters(trkParameters);
   trackAssociator_.useDefaultPropagator();

}


HSCPTimingAnalyzer::~HSCPTimingAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
HSCPTimingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  eventNum_ = iEvent.id().event();

  Handle<PCaloHitContainer> ebSimHits;
  iEvent.getByLabel("g4SimHits", "EcalHitsEB", ebSimHits);
  if(!ebSimHits.isValid())
  {
    std::cout << "Cannot get simHits from event!" << std::endl;
    return;
  }
  if(!ebSimHits->size() > 0)
  {
    std::cout << "EBSimHits size is 0!" << std::endl;
    return;
  }

  Handle<EBRecHitCollection> ebRecHits;
  iEvent.getByLabel(EBHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
  {
    std::cout << "Cannot get ebRecHits from event!" << std::endl;
    return;
  }
  if(!ebSimHits->size() > 0)
  {
    std::cout << "EBRecHits size is 0!" << std::endl;
    return;
  }
  Handle<EERecHitCollection> eeRecHits;  //TODO: implement EE?
  iEvent.getByLabel(EEHitCollection_,eeRecHits);
  if(!eeRecHits.isValid())
  {
    std::cout << "Cannot get eeRecHits from event" << std::endl;
    //return;
  }

  Handle<EBDigiCollection> ebDigis;
  iEvent.getByLabel(EBDigiCollection_,ebDigis);
  if(!ebDigis.isValid())
  {
    std::cout << "Cannot get ebDigis from event!" << std::endl;
    return;
  }
  if(!ebSimHits->size() > 0)
  {
    std::cout << "EBDigiss size is 0!" << std::endl;
    return;
  }

  Handle<reco::TrackCollection> recoTracks;
  iEvent.getByLabel(trackCollection_, recoTracks);
  if(!recoTracks.isValid())
  {
    std::cout << "Cannot get RecoTracks named " << trackCollection_ << " from event!" << std::endl;
    return;
  }
  if(!ebSimHits->size() > 0)
  {
    std::cout << "GeneralTracks size is 0!" << std::endl;
    return;
  }
  
  Handle<reco::MuonCollection> pMuons;
  iEvent.getByLabel(muonCollection_, pMuons);
  const reco::MuonCollection* theMuons = pMuons.product();
  if (!pMuons.isValid())
  {
    std::cout << muonCollection_ << " not available" << std::endl;
    return;
  }

  // Get the calorimeter geometry 
  iSetup.get<CaloGeometryRecord>().get(theCaloGeometry_);
  if (!theCaloGeometry_.isValid()) 
    throw cms::Exception("FatalError") << "Unable to find IdealGeometryRecord in event!\n";
  const CaloGeometry* theGeometry = theCaloGeometry_.product();
  // Topology
  edm::ESHandle<CaloTopology> pCaloTopology;
  iSetup.get<CaloTopologyRecord>().get(pCaloTopology);
  const CaloTopology* theCaloTopology = pCaloTopology.product();
  // Get the tracking Geometry
  iSetup.get<GlobalTrackingGeometryRecord>().get(theTrackingGeometry_);
  if (!theTrackingGeometry_.isValid()) 
    throw cms::Exception("FatalError") << "Unable to find GlobalTrackingGeometryRecord in event!\n";
  // EcalDetIdAssociator
  iSetup.get<DetIdAssociatorRecord>().get("EcalDetIdAssociator", ecalDetIdAssociator_);
  // Get the Bfield
  iSetup.get<IdealMagneticFieldRecord>().get(bField_);
    
  // Xtal - TkLength map
  // TODO: Implement for EE (endcap) as well...
  std::map<int,float> XtalMapCurved ;
  std::map<int,float> muonCrossedXtalCurvedMap; // HashedIndex to trackLength
  
  TrajectoryStateTransform tsTransform;

  //debug
  //std::cout << "Processing event " << iEvent.id().event() << std::endl;

  int numSimHits = 0;
  const PCaloHitContainer* phits=0;
  phits = ebSimHits.product();
  for(PCaloHitContainer::const_iterator mySimHit = phits->begin(); mySimHit != phits->end(); mySimHit++)
  {
    simHitsEnergyHist_->Fill(mySimHit->energy());
    simHitsTimeHist_->Fill(mySimHit->time());
    EBDetId simHitId = EBDetId(mySimHit->id());
    //std::cout << "SimHit DetId found: " << simHitId << std::endl;
    //std::cout << "SimHit hashedIndex: " << simHitId.hashedIndex() << std::endl;
    //std::cout << "SimHit energy: " << mySimHit->energy() << " time: " << mySimHit->time()
    //  << std::endl;
    numSimHits++;

    EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
    while(recHitItr != ebRecHits->end() && (recHitItr->id() != simHitId))
    {
      ++recHitItr;
    }
    if(recHitItr==ebRecHits->end())
    {
      //XXX: Cmmented out for debugging ease, Aug 3 2009
      //std::cout << "Could not find simHit detId: " << simHitId << "in RecHitCollection!" << std::endl;
      continue;
    }
    if(recHitItr->chi2Prob() < 0)
      continue;

    recHitTimeSimHitTimeHist_->Fill(25*recHitItr->time()-mySimHit->time());
    recHitTimeVsSimHitTime_->Fill(mySimHit->time(),25*recHitItr->time());
    recHitTimeSimHitTimeVsEnergy_->Fill(0.97*recHitItr->energy(),25*recHitItr->time()-mySimHit->time());
    //std::cout << "    SIMHIT MATCHED to recHit detId: " << (EBDetId)recHitItr->id() << std::endl;
    //std::cout << "    RECHIT TIME: " << 25*recHitItr->time() << " ENERGY: " << recHitItr->energy() << std::endl;
  }
  simHitsPerEventHist_->Fill(numSimHits);

  double maxEnergy = -9999;
  double bestTime = 0;
  EBDetId maxDetId;
  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    if(recHitItr->chi2Prob() < 0)
      continue;

    recHitTimeHist_->Fill(25*recHitItr->time());
    recHitEnergyHist_->Fill(0.97*recHitItr->energy());
    if(recHitItr->energy() > maxEnergy)
    {
      maxEnergy = 0.97*recHitItr->energy();
      bestTime = 25*recHitItr->time();
      maxDetId = recHitItr->id();
    }
  }
  if(maxEnergy > -9999)
  {
    //SIC TODO TEST XXX
    //if(maxEnergy>1)
    //{
      recHitMaxEnergyHist_->Fill(maxEnergy);
      recHitMaxEnergyTimingHist_->Fill(bestTime);
    //}

    EBDigiCollection::const_iterator digiItr = ebDigis->begin();
    while(digiItr != ebDigis->end() && digiItr->id() != maxDetId)
      ++digiItr;
    if(digiItr==ebDigis->end())
      return;
    EBDataFrame df(*digiItr);
    double pedestal = 200;
    //if(df.sample(0).gainId()!=1 || df.sample(1).gainId()!=1) return; //goes to the next digi
    //else {
    //  pedestal = (double)(df.sample(0).adc()+df.sample(1).adc())/(double)2;
    //}
    for (int i=0; (unsigned int)i< digiItr->size(); ++i ) {
      double gain = 12.;
      if(df.sample(i).gainId()==1)
        gain = 1.;
      else if(df.sample(i).gainId()==2)
        gain = 2.;

      sampleNumbers_.push_back(i);
      amplitudes_.push_back(pedestal+(df.sample(i).adc()-pedestal)*gain);
      recHitMaxEnergyShapeProfile_->Fill(i,pedestal+(df.sample(i).adc()-pedestal)*gain);
    }
  }


  // Now loop over Reco::Muon
  for(reco::MuonCollection::const_iterator MUit = theMuons->begin(); MUit != theMuons->end () ; ++MUit)
  {
    //XXX: Cut on GlobalMuon criteria
    if(!MUit->isGlobalMuon()) continue;

    //std::cout << "RecoTrack found with Pt (GeV): "  << recoTrack->outerPt() << std::endl;
    // XXX: Pt Cut
    if(MUit->globalTrack()->outerPt() < minTrackPt_)
      continue;
    
    //XXX SIC DEBUG
    reco::MuonEnergy myMuonEnergy = MUit->calEnergy();
    DetId ecalDet = myMuonEnergy.ecal_id();
    std::cout << "HSCPTIMINGANALYZER: EBRecHits size=" << ebRecHits->size() << std::endl;
    std::cout << "HSCPTIMINGANALYZER: EERecHits size=" << eeRecHits->size() << std::endl;
    if(ecalDet.subdetId()==EcalEndcap)
      std::cout << "!!Muon found ECAL DetId " << EEDetId((ecalDet)) << " energy: " << myMuonEnergy.emMax <<
        " time: " << myMuonEnergy.ecal_time << std::endl;
    else if(ecalDet.subdetId()==EcalBarrel)
      std::cout << "!!Muon found ECAL DetId " << EBDetId((ecalDet)) << " energy: " << myMuonEnergy.emMax <<
        " time: " << myMuonEnergy.ecal_time << std::endl;
    else
      std::cout << "!!Strange DetId found in muon with subdetId: " << ecalDet.subdetId() << std::endl;

    muonEcalMaxEnergyTimingHist_->Fill(myMuonEnergy.ecal_time);


    
    // Fill tree branch
    muonPt_ = MUit->globalTrack()->outerPt();
    
    const reco::TrackRef muonOuterTrack = MUit->outerTrack();
    const reco::TrackRef muonGlobalTrack = MUit->globalTrack();
    const reco::TrackRef muonInnerTrack = MUit->innerTrack();

    FreeTrajectoryState glbInnerState = tsTransform.innerFreeState(*muonGlobalTrack, &*bField_);
    FreeTrajectoryState glbOuterState = tsTransform.outerFreeState(*muonGlobalTrack, &*bField_);
    FreeTrajectoryState innInnerState = tsTransform.innerFreeState(*muonInnerTrack, &*bField_);
    FreeTrajectoryState innOuterState = tsTransform.outerFreeState(*muonInnerTrack, &*bField_);
    
    float pointX, pointY, pointZ;
    float directionX, directionY, directionZ;
    pointX = (float)muonInnerTrack->outerPosition().x();
    pointY = (float)muonInnerTrack->outerPosition().y();
    pointZ = (float)muonInnerTrack->outerPosition().z();
    directionX = (float)muonInnerTrack->outerMomentum().x();
    directionY = (float)muonInnerTrack->outerMomentum().y();
    directionZ = (float)muonInnerTrack->outerMomentum().z();

    GlobalPoint point(pointX, pointY, pointZ);
    GlobalVector direction(directionX, directionY, directionZ);

    // Build set of points in Ecal (necklace) using the propagator
    std::vector<SteppingHelixStateInfo> neckLace;
    neckLace = calcEcalDeposit(&glbInnerState, &glbOuterState,&innInnerState, &innOuterState, trackParameters_);

    // Initialize variables to be filled by the track-length function
    double totalLengthCurved = 0.;
    GlobalPoint internalPointCurved(0., 0., 0.);
    GlobalPoint externalPointCurved(0., 0., 0.);
    
    if(neckLace.size() > 1)
    {
      getDetailedTrackLengthInXtals(XtalMapCurved,
          muonCrossedXtalCurvedMap,
          totalLengthCurved,
          internalPointCurved,
          externalPointCurved,
          & (*theGeometry),
          & (*theCaloTopology),
          neckLace);
    }
    
    numCrysCrossed_ = muonCrossedXtalCurvedMap.size();
    numMatchedCrysPerEventHist_->Fill(numCrysCrossed_);
    double crossedEnergy = 0;
    double crossedLength = 0;
    int numRecHitsFound = 0;
    // Info
    std::cout << "\t Number of crossedCrys this muon: " << numCrysCrossed_ << std::endl;
    //TODO: We probably only want to use the max energy hit here.  But anyway this logic needs to be thought through.
    for(std::map<int,float>::const_iterator mapIt = muonCrossedXtalCurvedMap.begin();
        mapIt != muonCrossedXtalCurvedMap.end(); ++mapIt)
    {
      EBDetId ebDetId(mapIt->first);
      double trackLengthInXtal = mapIt->second;
      //std::cout << "\t Cry EBDetId: " << ebDetId << " trackLength: " << trackLengthInXtal << std::endl;
      // Fill TTree branch trackLength
      cryTrackLength_ = trackLengthInXtal;
      
      EcalRecHitCollection::const_iterator thisHit = ebRecHits->find(ebDetId);
      if (thisHit == ebRecHits->end()) 
      {
        std::cout << "\t Could not find crossedEcal detId: " << ebDetId << " in EBRecHitCollection!" << std::endl;
        // Fill TTree branch arrays
        cryTime_ = -999;
        cryEnergy_ = -999;
        cryChi2_ = -999;
        energyAndTimeNTuple_->Fill();
        continue;
      }

      //XXX: SIC debug
      std::cout << "\t My track-matched EBDetId: " << ebDetId << " energy: " << thisHit->energy() << " time: " <<
        thisHit->time() << std::endl;

      //XXX: Chi2 cut
      if(thisHit->chi2Prob() < 0)
        continue;

      //TODO: Make a check on the recoFlag of the RecHit
      // Especially if the hit is out of time so we can use the ratio method amplitude...
      // But right now we are just using both amp/time from ratio method
      numRecHitsFound++;
      crossedEnergy+=0.97*thisHit->energy();
      crossedLength+=trackLengthInXtal;
      deDxHist_->Fill(1000*0.97*thisHit->energy()/trackLengthInXtal);
      trackLengthPerCryHist_->Fill(trackLengthInXtal);
      timeOfTrackMatchedHits_->Fill(thisHit->time());
      energyOfTrackMatchedHits_->Fill(0.97*thisHit->energy());
      timeVsEnergyOfTrackMatchedHits_->Fill(0.97*thisHit->energy(),thisHit->time());
      deDxVsMomHist_->Fill(muonInnerTrack->outerP(),1000*0.97*thisHit->energy()/(8.3*trackLengthInXtal));

      // Make 3x3 energy
      float energy5x5 = 0;
      int num5x5crys = 0;
      float energy3x3 = 0;
      int num3x3crys = 0;
      const CaloSubdetectorTopology* ebTopology = theCaloTopology->getSubdetectorTopology(DetId::Ecal,ebDetId.subdetId());
      std::vector<DetId> S9aroundMax;
      std::vector<DetId> S25aroundMax;
      ebTopology->getWindow(ebDetId,3,3).swap(S9aroundMax);
      ebTopology->getWindow(ebDetId,5,5).swap(S25aroundMax);
      for(int icry=0; icry < 25; ++icry)
      {
        if(S25aroundMax[icry].subdetId() == EcalBarrel)
        {
          EBRecHitCollection::const_iterator itrechit = ebRecHits->find(S25aroundMax[icry]);
          if(itrechit!=ebRecHits->end() && itrechit->chi2Prob() > 0)
          {
            energy5x5+=0.97*itrechit->energy();
            ++num5x5crys;
            if(find(S9aroundMax.begin(),S9aroundMax.end(),S25aroundMax[icry]) != S9aroundMax.end())
            {
              energy3x3+=0.97*itrechit->energy();
              ++num3x3crys;
            }
          }
        }
      }
      deDx3x3Hist_->Fill(1000*energy3x3/trackLengthInXtal);
      deDx3x3CorrectedHist_->Fill(contCorr_*1000*energy3x3/trackLengthInXtal);  // Apply containment correction factor
      deDx5x5Hist_->Fill(1000*energy5x5/trackLengthInXtal);
      deDx5x5CorrectedHist_->Fill(contCorr_*1000*energy3x3/trackLengthInXtal);  // Apply containment correction factor
      numCrysIn5x5Hist_->Fill(num5x5crys);
      numCrysIn3x3Hist_->Fill(num3x3crys);

      // Fill TTree branch arrays
      cryTime_ = thisHit->time();
      cryEnergy_ = thisHit->energy();
      cryChi2_ = thisHit->chi2Prob();
      energyAndTimeNTuple_->Fill();
    }
    numCrysCrossedVsNumHitsFoundHist_->Fill(numRecHitsFound,numCrysCrossed_);

    // Fill hit1 vs. hit2 hists
    if(numRecHitsFound==2)
    {
      std::map<int,float>::const_iterator firstCryItr = muonCrossedXtalCurvedMap.begin();
      std::map<int,float>::const_iterator secondCryItr = firstCryItr++;
      
      EcalRecHitCollection::const_iterator firstHit = ebRecHits->find(EBDetId(firstCryItr->first));
      if(firstHit!=ebRecHits->end())
      {
        EcalRecHitCollection::const_iterator secondHit = ebRecHits->find(EBDetId(secondCryItr->first));
        if(secondHit!=ebRecHits->end())
        {
          hit1EnergyVsHit2EnergyHist_->Fill(0.97*secondHit->energy(),0.97*firstHit->energy());
          hit1LengthVsHit2LengthHist_->Fill(secondCryItr->second,firstCryItr->second);
          hit1DeDxVsHit2DeDxHist_->Fill(1000*0.97*secondHit->energy()/secondCryItr->second,1000*0.97*firstHit->energy()/firstCryItr->second);
        }
      }
    }
  }
}

// ------------ calcEcalDeposit method --------------------------------------------------
std::vector<SteppingHelixStateInfo> HSCPTimingAnalyzer::calcEcalDeposit(const FreeTrajectoryState* glbInnerState,
    const FreeTrajectoryState* glbOuterState,
    const FreeTrajectoryState* innInnerState,
    const FreeTrajectoryState* innOuterState,
    const TrackAssociatorParameters& parameters)
{
  return calcDeposit(glbInnerState, glbOuterState, innInnerState, innOuterState,
                     parameters, *ecalDetIdAssociator_);
}


// ------------ calcDeposit method --------------------------------------------------
std::vector<SteppingHelixStateInfo> HSCPTimingAnalyzer::calcDeposit(const FreeTrajectoryState* glbInnerState,
    const FreeTrajectoryState* glbOuterState,
    const FreeTrajectoryState* innInnerState,
    const FreeTrajectoryState* innOuterState,
    const TrackAssociatorParameters& parameters,
    const DetIdAssociator& associator)
{
  if (! parameters.useEcal && ! parameters.useCalo && ! parameters.useHcal &&
      ! parameters.useHO && ! parameters.useMuon )
    throw cms::Exception("ConfigurationError") << "Configuration error! No subdetector was selected for the track association.";

  // Set some params
  double minR = associator.volume().minR () ;
  double minZ = associator.volume().minZ () ;
  double maxR = associator.volume().maxR () ;
  double maxZ = associator.volume().maxZ () ;

  // Define the TrackOrigin (where the propagation starts)
  double innerR = sqrt (glbInnerState->position().x() * glbInnerState->position().x() + 
      glbInnerState->position().y() * glbInnerState->position().y() );
  double outerR = sqrt (glbOuterState->position().x() * glbOuterState->position().x() + 
      glbOuterState->position().y() * glbOuterState->position().y() );

  SteppingHelixStateInfo glbTrackInnerOrigin (*glbInnerState);
  SteppingHelixStateInfo glbTrackOuterOrigin (*glbOuterState);
  SteppingHelixStateInfo innTrackInnerOrigin (*innInnerState);
  SteppingHelixStateInfo innTrackOuterOrigin (*innOuterState);
  SteppingHelixStateInfo tempInnerOrigin;
  SteppingHelixStateInfo tempOuterOrigin;

  if( (innerR > 129.) && (outerR > 129.) )
  {
    tempInnerOrigin = innTrackInnerOrigin;
    tempOuterOrigin = innTrackOuterOrigin;
  }

  if( (innerR < 129.) && (outerR > 129.) )
  {
    tempInnerOrigin = glbTrackInnerOrigin;
    tempOuterOrigin = glbTrackInnerOrigin;
  }

  if( (innerR > 129.) && (outerR < 129.) )
  {
    tempInnerOrigin = glbTrackOuterOrigin;
    tempOuterOrigin = glbTrackOuterOrigin;
  }

  if( (innerR < 129.) && (outerR < 129.) )
  {
    if( glbInnerState->position().y() > glbOuterState->position().y() )
    {
      tempInnerOrigin = glbTrackInnerOrigin;
      tempOuterOrigin = glbTrackOuterOrigin;
    }

    else
    {
      tempInnerOrigin = glbTrackOuterOrigin;
      tempOuterOrigin = glbTrackInnerOrigin;
    }
  }

  glbTrackInnerOrigin = tempInnerOrigin;
  glbTrackOuterOrigin = tempOuterOrigin;

  // Define Propagator
  SteppingHelixPropagator* prop = new SteppingHelixPropagator (&*bField_, alongMomentum);
  prop -> setMaterialMode(false);
  prop -> applyRadX0Correction(true);

  // Build the necklace
  CachedTrajectory neckLace;
  neckLace.setStateAtIP(glbTrackOuterOrigin);
  neckLace.reset_trajectory();
  neckLace.setPropagator(prop);
  //neckLace.setPropagationStep(10.);
  //XXX: SIC mod, June 2 2009
  neckLace.setPropagationStep(0.1);
  neckLace.setMinDetectorRadius(minR);
  neckLace.setMinDetectorLength(minZ*2.);
  neckLace.setMaxDetectorRadius(maxR);
  neckLace.setMaxDetectorLength(maxZ*2.);  
  //std::cout << "calcEcalDeposits::minDetectorRadius = " << minR << std::endl;
  //std::cout << "calcEcalDeposits::maxDetectorRadius = " << maxR << std::endl;
  //std::cout << "calcEcalDeposits::minDetectorLength = " << minZ << std::endl;
  //std::cout << "calcEcalDeposits::maxDetectorLength = " << maxZ << std::endl;

  // Propagate track
  bool isPropagationSuccessful = neckLace.propagateAll(glbTrackOuterOrigin);

  if (!isPropagationSuccessful)
  {
    std::cout << ">>>>>> calcEcalDeposits::propagateAll::failed " << "<<<<<<" << std::endl;
    std::cout << "innerOrigin = " << glbTrackInnerOrigin.position() << "   innerR = " << innerR << std::endl; 
    std::cout << "outerOrigin = " << glbTrackOuterOrigin.position() << "   outerR = " << outerR << std::endl; 
    return std::vector<SteppingHelixStateInfo> () ;
  }

  std::vector<SteppingHelixStateInfo> complicatePoints;
  neckLace.getTrajectory(complicatePoints, associator.volume(), 500);
  //std::cerr << "necklace size = " << complicatePoints.size() << std::endl;

  return complicatePoints;
}
      
// ------------ getDetailedTrackLength in Xtals -----------------------------------------
int HSCPTimingAnalyzer::getDetailedTrackLengthInXtals(std::map<int,float>& XtalInfo, 
    std::map<int,float>& muonCrossedXtalMap, 
    double& totalLengthCurved,
    GlobalPoint& internalPointCurved, 
    GlobalPoint& externalPointCurved,
    const CaloGeometry* theGeometry, 
    const CaloTopology * theTopology, 
    const std::vector<SteppingHelixStateInfo>& neckLace)
{
  GlobalPoint origin(0., 0., 0.);
  internalPointCurved = origin;
  externalPointCurved = origin;

  bool firstPoint = false;
  const CaloSubdetectorGeometry *theSubdetGeometry = theGeometry->getSubdetectorGeometry(DetId::Ecal,1);

  for(std::vector<SteppingHelixStateInfo>::const_iterator itr = (neckLace.begin() + 1); itr != neckLace.end(); ++itr)
  {
    GlobalPoint probe_gp = (*itr).position();
    std::vector<DetId> surroundingMatrix;
    EBDetId closestDetIdToProbe((theSubdetGeometry->getClosestCell(probe_gp)).rawId());

    // check if the probe is inside the xtal
    if(theGeometry->getSubdetectorGeometry(closestDetIdToProbe)->getGeometry(closestDetIdToProbe)->inside(probe_gp))
    {
      double step = ((*itr).position()-(*(itr-1)).position()).mag();
      addStepToXtal(XtalInfo, muonCrossedXtalMap, closestDetIdToProbe,step);
      totalLengthCurved += step;

      if (firstPoint == false)
      {
        internalPointCurved = probe_gp ;
        firstPoint = true ;
      }
      externalPointCurved = probe_gp ;
    }
    else
    {
      // 3x3 matrix surrounding the probe
      surroundingMatrix = matrixDetId( theTopology, closestDetIdToProbe, -1, 1, -1, 1 );

      for( unsigned int k=0; k<surroundingMatrix.size(); ++k ) {
        if(theGeometry->getSubdetectorGeometry(surroundingMatrix.at(k))->getGeometry(surroundingMatrix.at(k))->inside(probe_gp))  
        {
          double step = ((*itr).position() - (*(itr-1)).position()).mag();
          addStepToXtal(XtalInfo, muonCrossedXtalMap, surroundingMatrix[k],step);
          totalLengthCurved += step;

          if (firstPoint == false)
          {
            internalPointCurved = probe_gp ;
            firstPoint = true ;
          }
          externalPointCurved = probe_gp ;
        }
      }
      // clear neighborhood matrix
      surroundingMatrix.clear();
    }
  }
  return 0;
}

// ------------ addStepToXtal -----------------------------------------------------------
void HSCPTimingAnalyzer::addStepToXtal(std::map<int,float>& xtalsMap,
    std::map<int,float>& muonCrossedXtalMap,
    DetId aDetId,
    float step)
{
  std::map<int,float>::iterator xtal = xtalsMap.find(aDetId.rawId());
  if (xtal!=xtalsMap.end())
    ((*xtal).second)+=step;
  else
    xtalsMap.insert(std::pair<int,float>(aDetId.rawId(),step));

  xtal = muonCrossedXtalMap.find(aDetId.rawId());
  if (xtal!= muonCrossedXtalMap.end())
    ((*xtal).second)+=step;
  else
    muonCrossedXtalMap.insert(std::pair<int,float>(aDetId.rawId(),step));

}

// ------------ matrixDetId method ------------------------------------------------------
std::vector<DetId> HSCPTimingAnalyzer::matrixDetId(const CaloTopology* topology, 
    DetId id, 
    int ixMin,
    int ixMax,
    int iyMin,
    int iyMax)
{
  CaloNavigator<DetId> cursor = CaloNavigator<DetId>(id, topology->getSubdetectorTopology(id));
  std::vector<DetId> v;
  for(int i = ixMin; i<=ixMax; ++i)
  {
    for(int j = iyMin; j <= iyMax; ++j)
    {
      //cout << "i = " << i << "; j = " << j << endl;
      cursor.home();
      cursor.offsetBy(i, j);
      if(*cursor != DetId(0))
        v.push_back(*cursor);
    }
  }
  return v;
}
                      

// ------------ method called once each job just before starting event loop  -------------
void 
HSCPTimingAnalyzer::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HSCPTimingAnalyzer::endJob()
{
   recHitMaxEnergyShapeGraph_ = new TGraph(sampleNumbers_.size(), &(*sampleNumbers_.begin()),&(*amplitudes_.begin()));
   recHitMaxEnergyShapeGraph_->SetTitle("Shape of max energy hits (ADC)");
   recHitMaxEnergyShapeGraph_->SetName("recHitMaxEnergyShapeGraph");

   file_->cd();

   simHitsTimeHist_->Write();
   simHitsEnergyHist_->Write();
   recHitTimeSimHitTimeHist_->Write();
   recHitTimeHist_->Write();
   recHitEnergyHist_->Write();
   recHitMaxEnergyHist_->Write();
   recHitMaxEnergyTimingHist_->Write();
   energyOfTrackMatchedHits_->Write();
   timeOfTrackMatchedHits_->Write();
   timeVsEnergyOfTrackMatchedHits_->Write();
   crossedEnergy_->Write();
   recHitTimeVsSimHitTime_->Write();
   simHitsPerEventHist_->Write();
   recHitTimeSimHitTimeVsEnergy_->Write();
   recHitsPerTrackHist_->Write();
   recHitMaxEnergyShapeProfile_->Write();
   recHitMaxEnergyShapeGraph_->Write();

   deDxHist_->Write();
   trackLengthPerCryHist_->Write();
   numMatchedCrysPerEventHist_->Write();
   numCrysCrossedVsNumHitsFoundHist_->Write();
   deDxVsMomHist_->Write();
   hit1EnergyVsHit2EnergyHist_->Write();
   hit1LengthVsHit2LengthHist_->Write();
   hit1DeDxVsHit2DeDxHist_->Write();

   deDx3x3Hist_->Write();
   deDx3x3CorrectedHist_->Write();
   deDx5x5Hist_->Write();
   deDx5x5CorrectedHist_->Write();
   numCrysIn5x5Hist_->Write();
   numCrysIn3x3Hist_->Write();

   muonEcalMaxEnergyTimingHist_->Write();
   
   energyAndTimeNTuple_->Write();
   file_->Close();
}

//define this as a plug-in
DEFINE_FWK_MODULE(HSCPTimingAnalyzer);
