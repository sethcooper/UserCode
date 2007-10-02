// -*- C++ -*-
//
// Package:    EcalDeDxAnalyzer
// Class:      EcalDeDxAnalyzer
// 
/**\class EcalDeDxAnalyzer EcalDeDxAnalyzer.cc Analyzers/EcalDeDxAnalyzer/src/EcalDeDxAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Thu Sep 27 16:09:01 CDT 2007
// $Id: EcalDeDxAnalyzer.cc,v 1.1 2007/09/27 21:15:01 scooper Exp $
//
//


// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Common/interface/Handle.h"
#include "AnalysisDataFormats/ElectronEfficiency/interface/EmObject.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/DetId/interface/DetId.h"

#include "TH2F.h"
#include "TFile.h"

//
// class decleration
//

class EcalDeDxAnalyzer : public edm::EDAnalyzer {
   public:
      explicit EcalDeDxAnalyzer(const edm::ParameterSet&);
      ~EcalDeDxAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag EMCollection_;
      TH2F* energyVsMomentum_;
      
      // ----------member data ---------------------------
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
EcalDeDxAnalyzer::EcalDeDxAnalyzer(const edm::ParameterSet& ps)
{
   //now do what ever initialization is needed
   EBHitCollection_ = ps.getParameter<edm::InputTag>("EBRecHitCollection");
   EEHitCollection_ = ps.getParameter<edm::InputTag>("EERecHitCollection");
   EMCollection_    = ps.getParameter<edm::InputTag>("EMObjectCollection");

   energyVsMomentum_ = new TH2F("energy_vs_momentum","energy vs. momentum",150,0,150,150,0,150);
}


EcalDeDxAnalyzer::~EcalDeDxAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
   delete energyVsMomentum_;

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalDeDxAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;
   
   // Get EmObjects out of the event
   Handle<EgEff::EmObjectCollection> EMObjs;
   try 
   {
     iEvent.getByLabel(EMCollection_, EMObjs);
   } catch ( std::exception& ex ) {
     cerr << "Error! can't get the product " << EMCollection_.label() << endl;
   }
   
   //TODO: Preshower hits??
   // Get the EB and EE RecHits
   Handle<EBRecHitCollection> EBHits;
   try 
   {
     iEvent.getByLabel(EBHitCollection_, EBHits);
   } catch ( std::exception& ex ) {
     cerr << "Error! can't get the product " << EBHitCollection_.label() << endl;
   }
   
   Handle<EERecHitCollection> EEHits;
   try 
   {
     iEvent.getByLabel(EEHitCollection_, EEHits);
   } catch ( std::exception& ex ) {
     cerr << "Error! can't get the product " << EEHitCollection_.label() << endl;
   }

   // Loop over the EmObjects
   for(EgEff::EmObjectCollection::const_iterator emItr = EMObjs->begin(); emItr != EMObjs->end(); ++emItr)
   {
     // Make sure each EMObject has a track and an associated supercluster
     if(emItr->hasSuperCluster() && emItr->hasTrack())
     {
       // Get the seed BasicCluster
       reco::BasicClusterRef bc = (emItr->superCluster())->seed();
       // Get the vector of hits used in the seed BasicCluster
       vector<DetId> BCHits = bc->getHitsByDetId();
       // Loop over these hits, finding the hit with the highest energy--the seed crystal
       // Have to check both EE and EB hit collections
       float bestEnergy = -1;
       for(vector<DetId>::const_iterator itr = BCHits.begin(); itr != BCHits.end(); ++itr)
       {
         EBRecHitCollection::const_iterator recItr;
         recItr = EBHits->find(*itr);
         if(recItr==EBHits->end())
           recItr = EEHits->find(*itr);
         
         // Make sure we have the correct EB or an EE RecHit
         if(recItr!=EEHits->end())
         {
           if(recItr->energy() > bestEnergy)
             bestEnergy=recItr->energy();
         }
       }
       // Fill our root histogram with the energy, momentum, or whatever
       // But need to recompute the momentum from the track
       math::XYZVector pv = emItr->track()->outerMomentum();
       float p = (float)sqrt(pow(pv.x(),2)+(pv.y(),2)+(pv.z(),2));
       energyVsMomentum_->Fill(p,bestEnergy);
     }
   }
   
}


// ------------ method called once each job just before starting event loop  ------------
void 
EcalDeDxAnalyzer::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalDeDxAnalyzer::endJob() {
  TFile a("test.root","RECREATE");

  energyVsMomentum_->Write();

  a.Write();
  a.Close();

}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalDeDxAnalyzer);
