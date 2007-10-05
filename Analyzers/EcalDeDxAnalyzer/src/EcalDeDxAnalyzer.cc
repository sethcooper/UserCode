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
// $Id: EcalDeDxAnalyzer.cc,v 1.6 2007/10/05 18:25:18 scooper Exp $
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
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "TH1D.h"
#include "TH2D.h"
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
      TH2D* energyVsMomentum_;
      TH1D* energyHist_;
      TH1D* momentumHist_;
      TH1D* highestEnergyHist_;
      TH1D* bozoClusterEnergyHist_;
      
      edm::InputTag simTrackContainer_;
      TH1D* simTrackEtaHist_;
      TH1D* simTrackPhiHist_;
      TH1D* ecalHitEtaHist_;
      TH1D* ecalHitPhiHist_;
        
      double dRSuperClusterTrack_;
      double minTrackP_;
      
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
   EBHitCollection_   = ps.getParameter<edm::InputTag>("EBRecHitCollection");
   EEHitCollection_   = ps.getParameter<edm::InputTag>("EERecHitCollection");
   trackProducer_     = ps.getParameter<edm::InputTag>("trackProducer");
   minTrackP_         = ps.getParameter<double>("minTrackP");
   simTrackContainer_ = ps.getParameter<edm::InputTag>("simTrackContainer");
   
   energyVsMomentum_ = new TH2D("energy_vs_momentum","energy dep. in ecal vs. momentum",8000,0,2000,100,0,1.5);
   energyHist_ = new TH1D("energy_in_ecal","energy dep. in ecal",100,0,1.5);
   momentumHist_ = new TH1D("tracker_outer_momentum","tracker outer momentum",8000,0,2000);
   highestEnergyHist_ = new TH1D("highest_energy_recHits","highest energy track-matched RecHits",100,0,1.5);
   bozoClusterEnergyHist_ = new TH1D("bozo_cluster_recHits","track-matched bozo clusters",100,0,1.5);
   
   
   simTrackEtaHist_ = new TH1D("simTrackEta","Eta of MC tracks",80,-4,4);
   simTrackPhiHist_ = new TH1D("simTrackPhi","Phi of MC tracks",120,-6.4,6.4);
   ecalHitEtaHist_ = new TH1D("ecalHitEta","Eta of hits in ecal",60,-3,3);
   ecalHitPhiHist_ = new TH1D("ecalHitPhi","Phi of hits in ecal",120,-6.4,6.4);
   
   dRSuperClusterTrack_ = 0.2;
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
  Handle<SimTrackContainer> simTracks;
  
  try
  {
    iEvent.getByLabel(trackProducer_, trackHandle);
    iEvent.getByLabel(EBHitCollection_, EBHits);
    iEvent.getByLabel(EEHitCollection_, EEHits);
    iEvent.getByLabel(simTrackContainer_, simTracks);
  }
  catch(cms::Exception& ex)
  {
    edm::LogError("EcalDeDxAnalyzer") << "Error! Can't get collections";
    throw ex; 
  }

  //TODO: Preshower hits??

  for(SimTrackContainer::const_iterator trackItr = simTracks->begin(); trackItr != simTracks->end(); ++trackItr)
  {
    double eta = trackItr->trackerSurfacePosition().eta();
    double phi = trackItr->trackerSurfacePosition().phi();
    simTrackEtaHist_->Fill(eta);
    simTrackPhiHist_->Fill(phi);
  }
  
  
  //Setup geometry
  ESHandle<CaloGeometry> geoHandle;
  iSetup.get <IdealGeometryRecord> ().get(geoHandle);
  const CaloSubdetectorGeometry *geometry_endcap = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
  const CaloSubdetectorGeometry *geometry_barrel = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);

  float highestE = -1;
  // Loop over the RecHits, first EBRechits
  // TODO: Really we should look at tracks first, since hits could be anything
  for(EBRecHitCollection::const_iterator recItr = EBHits->begin(); recItr != EBHits->end(); ++recItr)
  {
    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_barrel);
    if(trackFound != -1)
    {
      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound); 
      double p = track->outerP();
      float energy = (recItr->energy());
      cout << "EBRecHit Energy: " << energy << " Momentum: " << p << endl;
      const CaloCellGeometry *cell_p = geometry_barrel->getGeometry((*recItr).id());
      // Center of xtal
      GlobalPoint pt = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
      if(energy > highestE)
        highestE = energy;
      energyVsMomentum_->Fill(p, energy);
      energyHist_->Fill(energy);
      momentumHist_->Fill(p);
      ecalHitEtaHist_->Fill(pt.eta());
      ecalHitPhiHist_->Fill(pt.phi());
    }
  }
  if(highestE > -1)
    highestEnergyHist_->Fill(highestE);
  
  highestE = -1;
  // Now loop over the EERecHits
  for(EERecHitCollection::const_iterator recItr = EEHits->begin(); recItr != EEHits->end(); ++recItr)
  {
    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_endcap);
    if(trackFound != -1)
    {
      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound); 
      double p = track->outerP();
      float energy = (recItr->energy());
      cout << "EERecHit Energy: " << energy << " Momentum: " << p << endl;
      const CaloCellGeometry *cell_p = geometry_endcap->getGeometry((*recItr).id());
      // Center of xtal
      GlobalPoint pt = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
      if(energy > highestE)
        highestE = energy;
      energyVsMomentum_->Fill(p, energy);
      energyHist_->Fill(energy);
      momentumHist_->Fill(p);
      ecalHitEtaHist_->Fill(pt.eta());
      ecalHitPhiHist_->Fill(pt.phi());
    }
  }
  if(highestE > -1)
    highestEnergyHist_->Fill(highestE);
   
}


int EcalDeDxAnalyzer::findTrack(const EcalRecHit &seed, const reco::TrackCollection *tracks, const CaloSubdetectorGeometry* geometry_p)
{
  int retTrack = -1;

  // See if we have a track with pT > minTrackPt_ and dR < dRSuperClusterTrack_
  unsigned int numTracks = tracks->size();
  double highestP = 0;

  // Get position of the EERecHit
  const CaloCellGeometry *cell_p = geometry_p->getGeometry(seed.id());
  // Center of xtal
  GlobalPoint p = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);

  for(unsigned int j = 0; j < numTracks; j++)
  {
    if(dR((*tracks)[j].eta(), (*tracks)[j].phi(), p.eta(), p.phi()) < dRSuperClusterTrack_)
    {
      double trackP = (*tracks)[j].outerP();
      if((trackP > minTrackP_) && (trackP > highestP))
      {
        retTrack = j;
        highestP = trackP;
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
  highestEnergyHist_->Write();
  bozoClusterEnergyHist_->Write();
  
  simTrackEtaHist_->Write();
  simTrackPhiHist_->Write();
  ecalHitEtaHist_->Write();
  ecalHitPhiHist_->Write();
  
  a.Write();
  a.Close();

}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalDeDxAnalyzer);
