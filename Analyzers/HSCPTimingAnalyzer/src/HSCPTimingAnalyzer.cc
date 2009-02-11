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
// $Id: HSCPTimingAnalyzer.cc,v 1.1 2009/02/05 17:27:17 scooper Exp $
//
//


// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGraph.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

// *** for TrackAssociation
#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/Common/interface/Handle.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"

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

      // ----------member data ---------------------------
      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag EBUncalibRecHits_;
      edm::InputTag EBDigiCollection_;

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

      TProfile* recHitMaxEnergyShapeProfile_;
      TGraph* recHitMaxEnergyShapeGraph_;

      std::vector<double> sampleNumbers_;
      std::vector<double> amplitudes_;

      TrackDetectorAssociator trackAssociator_;
      TrackAssociatorParameters trackParameters_;

      double minTrackPt_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//
edm::Service<TFileService> fileService;

//
// constructors and destructor
//
HSCPTimingAnalyzer::HSCPTimingAnalyzer(const edm::ParameterSet& iConfig) :
  EBHitCollection_ (iConfig.getParameter<edm::InputTag>("EBRecHitCollection")),
  EBUncalibRecHits_ (iConfig.getParameter<edm::InputTag>("EBUncalibRecHits")),
  EBDigiCollection_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
  minTrackPt_ (iConfig.getParameter<double>("minimumTrackPt"))
{
   //now do what ever initialization is needed
   simHitsEnergyHist_ = fileService->make<TH1D>("simHitsEnergy","Energy of sim hits [GeV]",200,0,2);
   simHitsTimeHist_ = fileService->make<TH1D>("simHitsTime","Time of sim hits [ns]",500,-25,25);
   recHitTimeSimHitTimeHist_ = fileService->make<TH1D>("recHitTimeSimHitTime","RecHit time - simHit time [ns]",500,-25,25);
   recHitTimeHist_ = fileService->make<TH1D>("recHitTime","RecHit time",500,-25,25);
   recHitEnergyHist_ = fileService->make<TH1D>("recHitEnergy","RecHit energy [GeV]",500,0,2);
   recHitMaxEnergyHist_ = fileService->make<TH1D>("recHitMaxEnergy","Energy of max. ene. recHit [GeV]",500,0,2);
   recHitMaxEnergyTimingHist_ = fileService->make<TH1D>("recHitMaxEnergyTiming","Timing of max ene. recHit [ns]",500,-25,25);
   recHitMaxEnergyShapeProfile_ = fileService->make<TProfile>("recHitMaxEnergyShapeProfile","Shape of max energy hits [ADC]",10,0,10);
   energyOfTrackMatchedHits_ = fileService->make<TH1D>("summedEnergyOfTrackMatchedHits", "Energy of track-matched hits [GeV]",500,0,2);
   timeOfTrackMatchedHits_ = fileService->make<TH1D>("timeOfTrackMatchedHits", "Time of track-matched hits [ns]",500,-25,25);
   timeVsEnergyOfTrackMatchedHits_ = fileService->make<TH2D>("timeVsEnergyOfTrackMatchedHits","Time [ns] vs. energy [GeV] for track-matched hits",500,0,2,500,-25,25);
   crossedEnergy_ = fileService->make<TH1D>("crossedEcalEnergy", "Total energy of track-crossing xtals [GeV]",500,0,2);

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

  Handle<PCaloHitContainer> ebSimHits;
  iEvent.getByLabel("g4SimHits", "EcalHitsEB", ebSimHits);
  if(!ebSimHits.isValid())
  {
    std::cout << "Cannot get simHits from event!" << std::endl;
    return;
  }

  Handle<EBRecHitCollection> ebRecHits;
  Handle<EERecHitCollection> eeRecHits;  //TODO: implement EE?
  iEvent.getByLabel(EBHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
  {
    std::cout << "Cannot get ebRecHits from event!" << std::endl;
    return;
  }

  Handle<EcalUncalibratedRecHitCollection> EBUncalibRecHitsHandle_;
  iEvent.getByLabel(EBUncalibRecHits_, EBUncalibRecHitsHandle_);
  if(!EBUncalibRecHitsHandle_.isValid())
  {
    std::cout << "Cannot get ebUncalibRecHits from event!" << std::endl;
    return;
  }

  Handle<EBDigiCollection> ebDigis;
  iEvent.getByLabel(EBDigiCollection_,ebDigis);
  if(!ebDigis.isValid())
  {
    std::cout << "Cannot get ebDigis from event!" << std::endl;
    return;
  }

  Handle<reco::TrackCollection> recoTracks;
  iEvent.getByLabel("generalTracks", recoTracks);
  if(!recoTracks.isValid())
  {
    std::cout << "Cannot get generalTracks from event!" << std::endl;
    return;
  }

  //debug
  std::cout << "Processing event " << iEvent.id().event() << std::endl;

  const PCaloHitContainer* phits=0;
  phits = ebSimHits.product();
  for(PCaloHitContainer::const_iterator mySimHit = phits->begin(); mySimHit != phits->end(); mySimHit++)
  {
    simHitsEnergyHist_->Fill(mySimHit->energy());
    simHitsTimeHist_->Fill(mySimHit->time());
    EBDetId simHitId = EBDetId(mySimHit->id());

    EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(mySimHit->id());
    if(thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
    {
      std::cout << "Could not find simHit detId in uncalibRecHitCollection!" << std::endl;
      continue;
    }
    if(thisUncalibRecHit->chi2() < 0)
      continue;

    EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
    while(recHitItr != ebRecHits->end() && (recHitItr->id() != simHitId))
    {
      ++recHitItr;
    }
    if(recHitItr==ebRecHits->end())
    {
      std::cout << "Could not find simHit detId: " << simHitId << "in RecHitCollection!" << std::endl;
      continue;
    }

    recHitTimeSimHitTimeHist_->Fill(25*recHitItr->time()-mySimHit->time());
  }

  double maxEnergy = -9999;
  double bestTime = 0;
  EBDetId maxDetId;
  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(recHitItr->id());
    if(thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
    {
      std::cout << "Could not find recHit detId in uncalibRecHitCollection!" << std::endl;
      continue;
    }
    if(thisUncalibRecHit->chi2() < 0)
      continue;

    recHitTimeHist_->Fill(25*recHitItr->time());
    recHitEnergyHist_->Fill(recHitItr->energy());
    if(recHitItr->energy() > maxEnergy)
    {
      maxEnergy = recHitItr->energy();
      bestTime = 25*recHitItr->time();
      maxDetId = recHitItr->id();
    }
  }
  if(maxEnergy > -9999)
  {
    recHitMaxEnergyHist_->Fill(maxEnergy);
    recHitMaxEnergyTimingHist_->Fill(bestTime);

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

  // Loop over recoTracks and use TrackAssociator
  for(reco::TrackCollection::const_iterator recoTrack = recoTracks->begin(); recoTrack != recoTracks->end(); ++recoTrack)
  {
    std::cout << "RecoTrack found with Pt (GeV): "  << recoTrack->outerPt() << std::endl;
    // XXX: Pt Cut
    if(recoTrack->outerPt() < minTrackPt_)
      continue;

    TrackDetMatchInfo info = trackAssociator_.associate(iEvent, iSetup, *recoTrack, trackParameters_);
    //Debug
    std::cout << "!!Found " << info.crossedEcalIds.size() << " crossed Ecal DetId's" << std::endl;

    double summedEnergy = 0;
    
    for (unsigned int i=0; i<info.crossedEcalIds.size(); i++)
    {
      // only checks for barrel
      if (info.crossedEcalIds[i].det() == DetId::Ecal && info.crossedEcalIds[i].subdetId() == 1)
      {
        EBDetId ebDetId (info.crossedEcalIds[i]);

        //XXX: Chi2 cut
        EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(ebDetId);
        if(thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
        {
          std::cout << "Could not find crossedEcal detId: " << ebDetId << " in uncalibRecHitCollection!" << std::endl;
          continue;
        }
        if(thisUncalibRecHit->chi2() < 0)
          continue;

        EcalRecHitCollection::const_iterator thishit = ebRecHits->find(ebDetId);
        if (thishit == ebRecHits->end()) 
        {
          std::cout << "Could not find crossedEcal detId: " << ebDetId << " in recHitCollection!" << std::endl;
          continue;
        }

        EcalRecHit myhit = (*thishit);
        timeOfTrackMatchedHits_->Fill(25*myhit.time());
        timeVsEnergyOfTrackMatchedHits_->Fill(myhit.energy(),25*myhit.time());
        summedEnergy+=myhiy.energy();
      }
    }
    if(summedEnergy > 0)
      energyOfTrackMatchedHits_->Fill(myhit.energy());
    crossedEnergy_->Fill(info.ecalCrossedEnergy());
  }      
}


// ------------ method called once each job just before starting event loop  ------------
void 
HSCPTimingAnalyzer::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HSCPTimingAnalyzer::endJob()
{
   recHitMaxEnergyShapeGraph_ = fileService->make<TGraph>(sampleNumbers_.size(), &(*sampleNumbers_.begin()),&(*amplitudes_.begin()));
   recHitMaxEnergyShapeGraph_->SetTitle("Shape of max energy hits (ADC)");
   recHitMaxEnergyShapeGraph_->SetName("recHitMaxEnergyShapeGraph");
}

//define this as a plug-in
DEFINE_FWK_MODULE(HSCPTimingAnalyzer);
