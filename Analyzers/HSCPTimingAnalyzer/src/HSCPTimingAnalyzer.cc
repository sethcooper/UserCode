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
// $Id$
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
#include "TProfile.h"
#include "TGraph.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

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

      TProfile* recHitMaxEnergyShapeProfile_;
      TGraph* recHitMaxEnergyShapeGraph_;

      std::vector<double> sampleNumbers_;
      std::vector<double> amplitudes_;

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
  EBDigiCollection_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection"))
{
   //now do what ever initialization is needed
   simHitsEnergyHist_ = fileService->make<TH1D>("simHitsEnergy","Energy of sim hits",200,0,2);
   simHitsTimeHist_ = fileService->make<TH1D>("simHitsTime","Time of sim hits",500,-25,25);
   recHitTimeSimHitTimeHist_ = fileService->make<TH1D>("recHitTimeSimHitTime","RecHit time - simHit time",500,-25,25);
   recHitTimeHist_ = fileService->make<TH1D>("recHitTime","RecHit time",500,-25,25);
   recHitEnergyHist_ = fileService->make<TH1D>("recHitEnergy","RecHit energy (GeV)",500,-1,1);
   recHitMaxEnergyHist_ = fileService->make<TH1D>("recHitMaxEnergy","Energy of max. ene. recHit (GeV)",500,-1,1);
   recHitMaxEnergyTimingHist_ = fileService->make<TH1D>("recHitMaxEnergyTiming","Timing of max ene. recHit (ns)",500,-25,25);
   recHitMaxEnergyShapeProfile_ = fileService->make<TProfile>("recHitMaxEnergyShapeProfile","Shape of max energy hits (ADC)",10,0,10);
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
    return;

  Handle<EBRecHitCollection> ebRecHits;
  Handle<EERecHitCollection> eeRecHits;  //TODO: implement EE?
  iEvent.getByLabel(EBHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
    return;

  Handle<EcalUncalibratedRecHitCollection> EBUncalibRecHitsHandle_;
  iEvent.getByLabel(EBUncalibRecHits_, EBUncalibRecHitsHandle_);
  if(!EBUncalibRecHitsHandle_.isValid())
    return;

  Handle<EBDigiCollection> ebDigis;
  iEvent.getByLabel(EBDigiCollection_,ebDigis);
  if(!ebDigis.isValid())
    return;

  //Handle<reco::TrackCollection> trackHandle;
  //iEvent.getByLabel("generalTracks", trackHandle);
  //if(!trackHandle.isValid())
  //  return;


  const PCaloHitContainer* phits=0;
  phits = ebSimHits.product();
  for(PCaloHitContainer::const_iterator mySimHit = phits->begin(); mySimHit != phits->end(); mySimHit++)
  {
    simHitsEnergyHist_->Fill(mySimHit->energy());
    simHitsTimeHist_->Fill(mySimHit->time());
    EBDetId simHitId = EBDetId(mySimHit->id());

    EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(mySimHit->id());
    if(thisUncalibRecHit->chi2() < 0 || thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
      continue;

    EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
    while(recHitItr != ebRecHits->end() && (recHitItr->id() != simHitId))
    {
      ++recHitItr;
    }
    if(recHitItr==ebRecHits->end())
      continue;

    recHitTimeSimHitTimeHist_->Fill(25*recHitItr->time()-mySimHit->time());
  }

  double maxEnergy = -9999;
  double bestTime = 0;
  EBDetId maxDetId;
  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(recHitItr->id());
    if(thisUncalibRecHit->chi2() < 0 || thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
      continue;

    recHitTimeHist_->Fill(25*recHitItr->time());
    recHitEnergyHist_->Fill(recHitItr->energy());
    if(recHitItr->energy() > maxEnergy)
    {
      maxEnergy = recHitItr->energy();
      bestTime = recHitItr->time();
      maxDetId = recHitItr->id();
    }
  }
  if(maxEnergy > -9999)
  {
    recHitMaxEnergyHist_->Fill(maxEnergy);
    recHitMaxEnergyTimingHist_->Fill(25*bestTime);

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
