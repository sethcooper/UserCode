// -*- C++ -*-
//
// Package:    SimpleTimeEnergyGraphs
// Class:      SimpleTimeEnergyGraphs
// 
/**\class SimpleTimeEnergyGraphs SimpleTimeEnergyGraphs.cc Producers/SimpleTimeEnergyGraphs/src/SimpleTimeEnergyGraphs.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  
//         Created:  Tue Oct 13 14:56:06 CEST 2009
// $Id: SimpleTimeEnergyGraphs.cc,v 1.1 2010/03/10 08:19:27 scooper Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

//
// class decleration
//

class SimpleTimeEnergyGraphs : public edm::EDAnalyzer {
   public:
      explicit SimpleTimeEnergyGraphs(const edm::ParameterSet&);
      ~SimpleTimeEnergyGraphs();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
      edm::InputTag EBRecHitCollection_;
      edm::InputTag EERecHitCollection_;
      edm::InputTag EBUncalibRecHitCollection_;
      edm::InputTag EEUncalibRecHitCollection_;

      TFile* file_;
      TH1F* timeOfHitsEBps_;
      TH1F* timeOfHitsEB_;
      TH1F* energyOfHitsEB_;
      TH1F* timeOfHitsEE_;
      TH1F* energyOfHitsEE_;

      TH2F* timeVsAmplitudeEB_;
      TH2F* timeVsAmplitudeEE_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
SimpleTimeEnergyGraphs::SimpleTimeEnergyGraphs(const edm::ParameterSet& iConfig) :
  EBRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EBRecHitCollection")),
  EERecHitCollection_ (iConfig.getParameter<edm::InputTag>("EERecHitCollection")),
  EBUncalibRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EBUncalibRecHitCollection")),
  EEUncalibRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EEUncalibRecHitCollection"))
{
   //now do what ever initialization is needed
   file_ = new TFile("simpleTimeEnergyGraphs.root","RECREATE");
   file_->cd();

   timeOfHitsEBps_ = new TH1F("timeOfHitsEBps", "Time of hits EB [ps]",2000,-100,100);
   timeOfHitsEB_ = new TH1F("timeOfHitsEB", "Time of hits EB [ns]",10000,-5,5);
   energyOfHitsEB_ = new TH1F("energyOfHitsEB", "Energy of hits EB [GeV]",2500,0,100);
   timeOfHitsEE_ = new TH1F("timeOfHitsEE", "Time of hits EE [ns]",10000,-5,5);
   energyOfHitsEE_ = new TH1F("energyOfHitsEE", "Energy of hits EE [GeV]",2500,0,100);

   timeVsAmplitudeEB_ = new TH2F("timeVsAmplitudeEB","Time vs. amplitude of hits EB;ADC;ns",1250,0,2500,1000,-25,25);
   timeVsAmplitudeEE_ = new TH2F("timeVsAmplitudeEE","Time vs. amplitude of hits EE;ADC;ns",1250,0,2500,1000,-25,25);
}


SimpleTimeEnergyGraphs::~SimpleTimeEnergyGraphs()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
SimpleTimeEnergyGraphs::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

  // RecHits
  Handle<EBRecHitCollection> ebRecHits;
  iEvent.getByLabel(EBRecHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
  {
    std::cout << "Cannot get ebRecHits from event!" << std::endl;
    return;
  }
  if(!ebRecHits->size() > 0)
    std::cout << "EBRecHits size is 0!" << std::endl;
  Handle<EERecHitCollection> eeRecHits;
  iEvent.getByLabel(EERecHitCollection_,eeRecHits);
  if(!eeRecHits.isValid())
  {
    std::cout << "Cannot get eeRecHits from event!" << std::endl;
    return;
  }
  if(!eeRecHits->size() > 0)
    std::cout << "EERecHits size is 0!" << std::endl;
  // UncalibRecHits
  Handle<EBUncalibratedRecHitCollection> ebURecHits;
  iEvent.getByLabel(EBUncalibRecHitCollection_,ebURecHits);
  if(!ebURecHits.isValid())
  {
    std::cout << "Cannot get ebURecHits from event!" << std::endl;
    return;
  }
  if(!ebURecHits->size() > 0)
    std::cout << "EBUncalibratedRecHits size is 0!" << std::endl;
  Handle<EEUncalibratedRecHitCollection> eeURecHits;
  iEvent.getByLabel(EEUncalibRecHitCollection_,eeURecHits);
  if(!eeURecHits.isValid())
  {
    std::cout << "Cannot get eeURecHits from event!" << std::endl;
    return;
  }
  if(!eeURecHits->size() > 0)
    std::cout << "EEUncalibratedRecHits size is 0!" << std::endl;

  // Loop over EBRecHits
  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    timeOfHitsEBps_->Fill(recHitItr->time()*1000);
    timeOfHitsEB_->Fill(recHitItr->time());
    energyOfHitsEB_->Fill(recHitItr->energy());
    // Find UncalibHit
    EcalUncalibratedRecHitCollection::const_iterator uHitItr = ebURecHits->begin();
    while(uHitItr != ebURecHits->end() && uHitItr->id() != recHitItr->id())
      ++uHitItr;
    if(uHitItr != ebURecHits->end())
      timeVsAmplitudeEB_->Fill(uHitItr->amplitude(),recHitItr->time());
    else
      continue;
    std::cout << "Time of EBRecHit: " << recHitItr->time() << " ns, energy: "
      << recHitItr->energy() << " GeV, amplitude: " << uHitItr->amplitude()
      << std::endl;
  }
  // Loop over EERecHits
  for(EERecHitCollection::const_iterator recHitItr = eeRecHits->begin();
      recHitItr != eeRecHits->end(); ++recHitItr)
  {
    timeOfHitsEE_->Fill(recHitItr->time());
    energyOfHitsEE_->Fill(recHitItr->energy());
    // Find UncalibHit
    EcalUncalibratedRecHitCollection::const_iterator uHitItr = eeURecHits->begin();
    while(uHitItr != eeURecHits->end() && uHitItr->id() != recHitItr->id())
      ++uHitItr;
    if(uHitItr != eeURecHits->end())
      timeVsAmplitudeEE_->Fill(uHitItr->amplitude(),recHitItr->time());
    else
      continue;
    std::cout << "Time of EERecHit: " << recHitItr->time() << " ns, energy: "
      << recHitItr->energy() << " GeV, amplitude: " << uHitItr->amplitude()
      << std::endl;
  }

}


// ------------ method called once each job just before starting event loop  ------------
void 
SimpleTimeEnergyGraphs::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SimpleTimeEnergyGraphs::endJob() {
  file_->cd();

  timeOfHitsEBps_->Write();
  timeOfHitsEB_->Write();
  energyOfHitsEB_->Write();
  timeVsAmplitudeEB_->Write();
  timeOfHitsEE_->Write();
  energyOfHitsEE_->Write();
  timeVsAmplitudeEE_->Write();

  file_->Close();
  
}

//define this as a plug-in
DEFINE_FWK_MODULE(SimpleTimeEnergyGraphs);
