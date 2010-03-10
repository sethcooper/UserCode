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
// $Id$
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
      edm::InputTag EBHitCollection_;

      TFile* file_;
      TH1F* timeOfHitsEB_;
      TH1F* energyOfHitsEB_;
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
  EBHitCollection_ (iConfig.getParameter<edm::InputTag>("EBRecHitCollection"))
{
   //now do what ever initialization is needed
   file_ = new TFile("simpleTimeEnergyGraphs.root","RECREATE");
   file_->cd();
   timeOfHitsEB_ = new TH1F("timeOfHitsEB", "Time of hits EB [ps]",20000000,-1,1);
   energyOfHitsEB_ = new TH1F("energyOfHitsEB", "Energy of hits EB [GeV]",2500,0,100);

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

  Handle<EBRecHitCollection> ebRecHits;
  iEvent.getByLabel(EBHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
  {
    std::cout << "Cannot get ebRecHits from event!" << std::endl;
    return;
  }
  if(!ebRecHits->size() > 0)
  {
    std::cout << "EBRecHits size is 0!" << std::endl;
    return;
  }

  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    timeOfHitsEB_->Fill(recHitItr->time()/1000);
    energyOfHitsEB_->Fill(recHitItr->energy());
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
  timeOfHitsEB_->Write();
  energyOfHitsEB_->Write();
  file_->Close();
  
}

//define this as a plug-in
DEFINE_FWK_MODULE(SimpleTimeEnergyGraphs);
