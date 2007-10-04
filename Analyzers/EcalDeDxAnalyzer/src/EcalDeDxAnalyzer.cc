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
// $Id: EcalDeDxAnalyzer.cc,v 1.4 2007/10/03 22:16:53 scooper Exp $
//
//


// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "TH1F.h"
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

      int findTrack(const EcalRecHit &seed, const reco::TrackCollection *tracks, const CaloSubdetectorGeometry* geometry_p);
      double dPhi(double phi1, double phi2);
      double dR(double eta1, double phi1, double eta2, double phi2);


      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag trackProducer_;
      TH2F* energyVsMomentum_;
      TH1F* energyHist_;
      TH1F* momentumHist_;
     
      double dRSuperClusterTrack_;
      double minTrackPt_;
      
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
   trackProducer_   = ps.getParameter<edm::InputTag>("trackProducer");
   
   energyVsMomentum_ = new TH2F("energy_vs_momentum","energy dep. in ecal vs. momentum",400,0,100,24,0,1.5);
   energyHist_ = new TH1F("energy_in_ecal","energy dep. in ecal",24,0,1.5);
   momentumHist_ = new TH1F("tracker_outer_momentum","tracker outer momentum",400,0,100);
   
   dRSuperClusterTrack_ = 0.2;
   minTrackPt_ = 1.0;
}


EcalDeDxAnalyzer::~EcalDeDxAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
   //delete energyVsMomentum_;

}


//
// member functions
//

// ------------ method called to for each event  ------------
void EcalDeDxAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  Handle<reco::TrackCollection> trackHandle;
  Handle<EBRecHitCollection> EBHits;
  Handle<EERecHitCollection> EEHits;
  try
  {
    iEvent.getByLabel(trackProducer_, trackHandle);
    iEvent.getByLabel(EBHitCollection_, EBHits);
    iEvent.getByLabel(EEHitCollection_, EEHits);
  }
  catch(cms::Exception& ex)
  {
    edm::LogError("EcalDeDxAnalyzer") << "Error! Can't get collections";
    throw ex; 
  }

  //TODO: Preshower hits??

  
  //Setup geometry
  ESHandle<CaloGeometry> geoHandle;
  iSetup.get <IdealGeometryRecord> ().get(geoHandle);
  const CaloSubdetectorGeometry *geometry_endcap = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
  const CaloSubdetectorGeometry *geometry_barrel = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);

  // Loop over the RecHits, first EBRechits
  for(EBRecHitCollection::const_iterator recItr = EBHits->begin(); recItr != EBHits->end(); ++recItr)
  {
    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_barrel);
    if(trackFound != -1)
    {
      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound); 
      math::XYZVector pv = track->outerMomentum();
      float p = (float)sqrt(pow(pv.x(),2)+(pv.y(),2)+(pv.z(),2));
      float energy = (recItr->energy());
      cout << "EBRecHit Energy: " << energy << " Momentum: " << p << endl;
      energyVsMomentum_->Fill(p, energy);
      energyHist_->Fill(energy);
      momentumHist_->Fill(p);
    }
  }

  // Now loop over the EERecHits
  for(EERecHitCollection::const_iterator recItr = EEHits->begin(); recItr != EEHits->end(); ++recItr)
  {
    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_endcap);
    if(trackFound != -1)
    {
      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound); 
      math::XYZVector pv = track->outerMomentum();
      float p = (float)sqrt(pow(pv.x(),2)+(pv.y(),2)+(pv.z(),2));
      float energy = (recItr->energy());
      cout << "EERecHit Energy: " << energy << " Momentum: " << p << endl;
      energyVsMomentum_->Fill(p, energy);
      energyHist_->Fill(energy);
      momentumHist_->Fill(p);
    }
  }
   
}


int EcalDeDxAnalyzer::findTrack(const EcalRecHit &seed, const reco::TrackCollection *tracks, const CaloSubdetectorGeometry* geometry_p)
{
  int retTrack = -1;

  // See if we have a track with pT > minTrackPt_ and dR < dRSuperClusterTrack_
  unsigned int numTracks = tracks->size();
  double highestPt = 0;

  // Get position of the EERecHit
  const CaloCellGeometry *cell_p = geometry_p->getGeometry(seed.id());
  // Center of xtal
  GlobalPoint p = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);

  for(unsigned int j = 0; j < numTracks; j++)
  {
    if(dR((*tracks)[j].eta(), (*tracks)[j].phi(), p.eta(), p.phi()) < dRSuperClusterTrack_)
    {
      double trackPt = (*tracks)[j].pt();
      if((trackPt > minTrackPt_) && (trackPt > highestPt))
      {
        retTrack = j;
        highestPt = trackPt;
      }
    }
  }

  return retTrack;
}

double EcalDeDxAnalyzer::dPhi(double phi1, double phi2)
{
  double diff = fabs(phi2 - phi1);
  double corr = 2*acos(-1.) - diff;
  if (diff < acos(-1.)){ return diff;} else { return corr;}
}

double EcalDeDxAnalyzer::dR(double eta1, double phi1, double eta2, double phi2)
{
  return sqrt(((eta1-eta2)*(eta1-eta2)) + (dPhi(phi1, phi2) * dPhi(phi1, phi2)));
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
  energyHist_->Write();
  momentumHist_->Write();
  
  a.Write();
  a.Close();

}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalDeDxAnalyzer);
