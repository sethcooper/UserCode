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
// $Id: EcalDeDxAnalyzer.cc,v 1.11 2007/10/30 16:40:54 scooper Exp $
//
//


// system include files
#include <memory>
#include <string>
#include <sstream>

// user include files
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"
#include "DataFormats/GeometrySurface/interface/SimpleDiskBounds.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TF1.h"
#include "TGraph.h"
#include "TProfile.h"
#include "Analyzers/EcalDeDxAnalyzer/interface/BozoCluster.h"

//
// class decleration
//

class EcalDeDxAnalyzer : public edm::EDAnalyzer {
   public:
      explicit EcalDeDxAnalyzer(const edm::ParameterSet&);
      ~EcalDeDxAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup& setup) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob();

      int findTrack(const EcalRecHit &seed, const reco::TrackCollection *tracks, const CaloSubdetectorGeometry* geometry_p);
      double dPhi(double phi1, double phi2);
      double dR(double eta1, double phi1, double eta2, double phi2);
      BozoCluster makeBozoCluster(const EcalRecHit &seed, const edm::Handle<EBRecHitCollection> &hits);
      std::string floatToString(float f);
      math::XYZPoint propagateTrack(const reco::Track track);
      math::XYZPoint propagateTrack(const TrackingParticle track);
      GlobalPoint localPosToGlobalPos(const PSimHit hit);
      GlobalVector localMomToGlobalMom(const PSimHit hit);
      /** Hard-wired numbers defining the surfaces on which the crystal front faces lie. */
      static float barrelRadius() {return 129.f;} //p81, p50, ECAL TDR
      static float barrelHalfLength() {return 270.9f;} //p81, p50, ECAL TDR
      static float endcapRadius() {return 171.1f;} // fig 3.26, p81, ECAL TDR
      static float endcapZ() {return 320.5f;} // fig 3.26, p81, ECAL TDR
      static void initializeSurfaces();
      static void check() {if (!theInit_) initializeSurfaces();}
      static ReferenceCountingPointer<BoundCylinder>  theBarrel_;
      static ReferenceCountingPointer<BoundDisk>      theNegativeEtaEndcap_;
      static ReferenceCountingPointer<BoundDisk>      thePositiveEtaEndcap_;
      static const BoundCylinder& barrel()        { check(); return *theBarrel_;}
      static const BoundDisk& negativeEtaEndcap() { check(); return *theNegativeEtaEndcap_;}
      static const BoundDisk& positiveEtaEndcap() { check(); return *thePositiveEtaEndcap_;}
      static bool theInit_;

      edm::ESHandle<MagneticField> theMF_;
      //TrackAssociatorBase* associatorByHits_;
      TrajectoryStateOnSurface stateAtECAL_;
      mutable PropagatorWithMaterial* forwardPropagator_ ;
      PropagationDirection dir_;
      const TrackerGeometry *trackerGeom;

      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag trackProducer_;
      edm::InputTag trackingParticleCollection_;
      TH2D* energyVsMomentum_;
      TH1D* energyHist_;
      TH1D* momentumHist_;
      TH1D* highestEnergyHist_;
      TH1D* bozoClusterEnergyHist_;
      TH2D* energyVsBetaHist_;
      TH2D* hitEtaPhiHist_;
      TH2D* deDxVsBetaHist_;
      TH2D* clusterDeDxVsBetaHist_;
      
      TGraph* deDxVsBetaGraph_;
      
      edm::InputTag simTrackContainer_;
      TH1D* simTrackEtaHist_;
      TH1D* simTrackPhiHist_;
      TH1D* ecalHitEtaHist_;
      TH1D* ecalHitPhiHist_;
      TH1D* cluster3x3Hist_;
      TH1D* cluster5x5Hist_;
      TH1D* simHitsEnergyHist_;
      TH1D* simTrackRecoTrackDeltaEtaHist_;
      TH1D* simTrackRecoTrackDeltaPhiHist_;

      
      TF1* betheBlochKFunct;
      
      
      double dRHitTrack;
      double minTrackPt_;
      double mcParticleMass_;
      
      std::vector<double> vx;
      std::vector<double> vy;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//
ReferenceCountingPointer<BoundCylinder>  EcalDeDxAnalyzer::theBarrel_ = 0;
ReferenceCountingPointer<BoundDisk>      EcalDeDxAnalyzer::theNegativeEtaEndcap_ = 0;
ReferenceCountingPointer<BoundDisk>      EcalDeDxAnalyzer::thePositiveEtaEndcap_ = 0;
bool                                     EcalDeDxAnalyzer::theInit_ = false;

//
// static data member definitions
//

//
// constructors and destructor
//
EcalDeDxAnalyzer::EcalDeDxAnalyzer(const edm::ParameterSet& ps)
{
   //now do what ever initialization is needed
   EBHitCollection_            = ps.getParameter<edm::InputTag>("EBRecHitCollection");
   EEHitCollection_            = ps.getParameter<edm::InputTag>("EERecHitCollection");
   trackProducer_              = ps.getParameter<edm::InputTag>("trackProducer");
   minTrackPt_                 = ps.getParameter<double>("minTrackPt");
   simTrackContainer_          = ps.getParameter<edm::InputTag>("simTrackContainer");
   trackingParticleCollection_ = ps.getParameter<edm::InputTag>("trackingParticleCollection");
   mcParticleMass_             = ps.getUntrackedParameter<double>("mcParticleMass",-1.0);
   dRHitTrack                  = ps.getParameter<double>("dRTrack");
   
   energyVsMomentum_ = new TH2D("energy_vs_momentum","energy dep. in ecal vs. momentum",8000,0,2000,100,0,1.5);
   energyHist_ = new TH1D("energy_in_ecal","energy dep. in ecal",100,0,1.5);
   momentumHist_ = new TH1D("tracker_outer_momentum","tracker outer momentum",8000,0,2000);
   highestEnergyHist_ = new TH1D("highest_energy_recHits","highest energy track-matched RecHits",100,0,1.5);
   bozoClusterEnergyHist_ = new TH1D("bozo_cluster_recHits","track-matched bozo clusters",100,0,1.5);
   energyVsBetaHist_ = new TH2D("energy_vs_beta","energy dep. in ecal vs. beta",100,0,1,100,0,1.5);
   hitEtaPhiHist_ = new TH2D("hit_eta_Phi","eta and phi of ecal RecHits",25,-1.5,1.5,50,-3.2,3.2);
   cluster3x3Hist_ = new TH1D("3x3_cluster_energy","E3x3 energy", 100, 0, 1.5);
   cluster5x5Hist_ = new TH1D("5x5_cluster_energy","E5x5 energy", 100, 0, 1.5);
   deDxVsBetaHist_ = new TH2D("dedx_vs_beta","dE/dx vs. beta",10,0,1,60,0,.06);
   //TODO: How to define path length of particle in detector for a cluster??
   clusterDeDxVsBetaHist_ = new TH2D("cluster_dedx_vs_beta","dE/dx from cluster vs. beta",100,0,1,60,0,.06);
   
   // e vs. beta hist only valid when we know the mass (i.e., protons)!
   betheBlochKFunct = new TF1("betheBlochK","(1/[0])*(1/pow(x,2))",0.001,1);
   betheBlochKFunct->SetParNames("K");
   betheBlochKFunct->SetParameter(0,900);
   
   simTrackEtaHist_ = new TH1D("simTracksEta","Eta of MC tracks",80,-4,4);
   simTrackPhiHist_ = new TH1D("simTracksPhi","Phi of MC tracks",120,-6.4,6.4);
   simHitsEnergyHist_ = new TH1D("simHitsEnergy","Energy of sim hits",200,0,2);
   ecalHitEtaHist_ = new TH1D("ecalHitsEta","Eta of hits in ecal",60,-3,3);
   ecalHitPhiHist_ = new TH1D("ecalHitsPhi","Phi of hits in ecal",120,-6.4,6.4);
   simTrackRecoTrackDeltaEtaHist_ = new TH1D("tracksDeltaEta","Delta eta of sim tracks and reco tracks",100,-1,1);
   simTrackRecoTrackDeltaPhiHist_ = new TH1D("tracksDeltaPhi","Delta eta of sim tracks and reco tracks",300,-3.15,3.15);
   
   //dRHitTrack = 1; //0.2;
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

  cout << "Number of reco tracks: " << trackHandle.product()->size() << endl;
  //cout << "Number of sim tracks: " << simTracks->size() << endl;

  //Setup geometry
  ESHandle<CaloGeometry> geoHandle;
  ESHandle<TrackerGeometry> trackerHandle;
  try
  {
    iSetup.get<IdealGeometryRecord>().get(geoHandle);
    iSetup.get<TrackerDigiGeometryRecord>().get(trackerHandle);
  }
  catch (std::exception& ex)
  {
    std::cerr << "Error!  can't get the tracker geometry." << std::endl;
    throw ex;
  }
  const CaloSubdetectorGeometry *geometry_endcap = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
  const CaloSubdetectorGeometry *geometry_barrel = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
  trackerGeom = trackerHandle.product();

  // Get MC Tracking truth
  edm::Handle < TrackingParticleCollection > trackingParticleHandle;
  iEvent.getByLabel(trackingParticleCollection_, trackingParticleHandle);
  TrackingParticleCollection trackingParticles =
    *(trackingParticleHandle.product());
  
  cout << "Number of TrackingParticle Tracks: " << trackingParticles.size() << endl;
     
  //TODO: Preshower hits??
  int simTrackCount = 0;
  //for(SimTrackContainer::const_iterator trackItr = simTracks->begin(); trackItr != simTracks->end(); ++trackItr)
  for(std::vector <TrackingParticle>::iterator trackItr = trackingParticles.begin(); trackItr != trackingParticles.end();
	    ++trackItr) 
  {
    std::vector<PSimHit> simHits = trackItr->trackPSimHit();
    if (simHits.size() == 0) continue;
    std::vector<PSimHit>::const_iterator hit = simHits.end();
    hit--;
    
   
    //TODO: Make this an honest-to-goodness Pt cut (in GeV)
    if(hit->pabs() > 100)
    {
      double eta = localPosToGlobalPos(*hit).eta();
      double phi = localPosToGlobalPos(*hit).phi();
      cout << "PDG ID:" << trackItr->pdgId() << endl;
      cout << "DEBUG1--line 301" << endl;
      simTrackEtaHist_->Fill(eta);
      simTrackPhiHist_->Fill(phi);
      simTrackCount++;
      math::XYZPoint ecalSimTrackPos = propagateTrack(*trackItr);
      cout << "DEBUG2" << endl;
      math::XYZPoint ecalRecoTrackPos = math::XYZPoint(0,0,0);
      if(trackHandle.product()->size()!=0)
        math::XYZPoint ecalRecoTrackPos = propagateTrack((*trackHandle.product())[0]);
      cout << "DEBUG3" << endl;
      simTrackRecoTrackDeltaPhiHist_->Fill(ecalSimTrackPos.Phi()-ecalRecoTrackPos.Phi());
      simTrackRecoTrackDeltaEtaHist_->Fill(ecalSimTrackPos.Eta()-ecalRecoTrackPos.Eta());
    }
  }

  cout << "Number of tracking particles tracks past cut: " << simTrackCount << endl;
  
  Handle<PCaloHitContainer> hits;
  const PCaloHitContainer* phits=0;
  try
  {
    iEvent.getByLabel("g4SimHits", "EcalHitsEB", hits);
    phits = hits.product(); // get a ptr to the product
  }
  catch ( std::exception& ex )
  {
    std::cerr << "Error! can't get the product SimG4Object, EcalHitsEB "<< std::endl;
    throw ex;
  }

  for(PCaloHitContainer::const_iterator myHit = phits->begin(); myHit != phits->end(); myHit++)
  {
    simHitsEnergyHist_->Fill(myHit->energy());
  }
  
  
  float highestE = -1;
  double trackMatchedP;
  EcalRecHit highestEHit;
  // Loop over the RecHits, first EBRechits
  // TODO: Should we look at tracks first instead?
  for(EBRecHitCollection::const_iterator recItr = EBHits->begin(); recItr != EBHits->end(); ++recItr)
  {
    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_barrel);
    if(trackFound != -1)
    {
      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound);
      double p = track->outerP();
      float energy = (recItr->energy());
      const CaloCellGeometry *cell_p = geometry_barrel->getGeometry((*recItr).id());
      // Center of xtal
      GlobalPoint pt = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
      //cout << "EBRecHit Energy: " << energy << " Momentum: " << p << endl;
      if(energy > highestE)
      {
        highestE = energy;
        highestEHit = (*recItr);
        trackMatchedP = p;
      }
      energyVsMomentum_->Fill(p, energy);
      energyHist_->Fill(energy);
      momentumHist_->Fill(p);
      ecalHitEtaHist_->Fill(pt.eta());
      ecalHitPhiHist_->Fill(pt.phi());
      hitEtaPhiHist_->Fill(pt.eta(),pt.phi());
      if(mcParticleMass_ > -1)
      {
        double beta = p/sqrt(pow(p,2)+pow(mcParticleMass_,2));
        energyVsBetaHist_->Fill(beta, energy);
        deDxVsBetaHist_->Fill(beta,energy/23.0);
        if(beta > .5) //energy/23.0 > .003)
        {
          cout << "Adding point (e/l,beta): " << energy/23.0 << "," << beta << endl;
          vx.push_back(beta);
          vy.push_back(energy/23.0);  //TODO: This is only a very rough crystal length approximation
                                      //      Can we fix using propagator?
        }
      }
    }
  }
  
  BozoCluster cluster;
  if(highestE > -1)
  {
    cout << "Highest Energy Hit: " << highestE << endl;
    highestEnergyHist_->Fill(highestE);
    cluster = makeBozoCluster(highestEHit, EBHits);
  }
 if(cluster.getEnergy() != -1 && mcParticleMass_ > -1)
 {

   double beta = trackMatchedP/sqrt(pow(trackMatchedP,2)+pow(mcParticleMass_,2));
   clusterDeDxVsBetaHist_->Fill(beta, cluster.getEnergy()/23.0);
 }
 
//  highestE = -1;
//  // Now loop over the EERecHits
//  for(EERecHitCollection::const_iterator recItr = EEHits->begin(); recItr != EEHits->end(); ++recItr)
//  {
//    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_endcap);
//    if(trackFound != -1)
//    {
//      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound); 
//      double p = track->outerP();
//      float energy = (recItr->energy());
//      cout << "EERecHit Energy: " << energy << " Momentum: " << p << endl;
//      const CaloCellGeometry *cell_p = geometry_endcap->getGeometry((*recItr).id());
//      // Center of xtal
//      GlobalPoint pt = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
//      if(energy > highestE)
//        highestE = energy;
//      energyVsMomentum_->Fill(p, energy);
//      energyHist_->Fill(energy);
//      momentumHist_->Fill(p);
//      ecalHitEtaHist_->Fill(pt.eta());
//      ecalHitPhiHist_->Fill(pt.phi());
//      if(mcParticleMass_ > -1)
//      {
//        double beta = p/sqrt(pow(p,2)+pow(mcParticleMass_,2));
//        energyVsBetaHist_->Fill(beta, energy);
//        vx.push_back(energy/23.0);  //TODO: This is only a very rough crystal length approximation
//        vy.push_back(beta);
//      }
//    }
//  }
//  if(highestE > -1)
//    highestEnergyHist_->Fill(highestE);
}

//---------------------------------------------------------------------------------------------------
int EcalDeDxAnalyzer::findTrack(const EcalRecHit &seed, const reco::TrackCollection *tracks,
                                const CaloSubdetectorGeometry* geometry_p)
{
  int retTrack = -1;

  // See if we have a track with pT > minTrackPt_ and dR < dRHitTrack
  unsigned int numTracks = tracks->size();
  double highestPt = 0;
  // Get position of the RecHit
  const CaloCellGeometry *cell_p = geometry_p->getGeometry(seed.id());
  // Center of xtal
  GlobalPoint p = (dynamic_cast<const TruncatedPyramid*>(cell_p))->getPosition(0);

  for(unsigned int j = 0; j < numTracks; j++)
  {
    //Propagate tracks
    math::XYZPoint ecalHitPt = propagateTrack((*tracks)[j]);
    double dRtrack = dR(ecalHitPt.eta(), ecalHitPt.phi(), p.eta(), p.phi());
    //double dRtrack = dR((*tracks)[j].eta(), (*tracks)[j].phi(), p.eta(), p.phi());
    //std::cout << "dR of track to hit: " << dRtrack << std::endl;
    if(dRtrack < dRHitTrack)
    {
      double trackPt = (*tracks)[j].outerPt();
      if((trackPt > minTrackPt_) && (trackPt > highestPt))
      {
        retTrack = j;
        highestPt = trackPt;
      }
    }
  }

  return retTrack;
}


//---------------------------------------------------------------------------------------------------
math::XYZPoint EcalDeDxAnalyzer::propagateTrack(const reco::Track track)
{
  trackingRecHit_iterator hit = track.recHitsEnd();
  hit--;
  
  if(hit == track.recHitsBegin())
    return math::XYZPoint(0.,0.,0.);

  DetId theDetUnitId((*hit)->geographicalId());
  const GeomDetUnit *theDet =  trackerGeom-> idToDetUnit(theDetUnitId);
  if(theDet==0)
    return math::XYZPoint(0.,0.,0.);

  const BoundPlane& bSurface = theDet->surface();
  GlobalPoint hitPos(bSurface.toGlobal((*hit)->localPosition()).x(),
      bSurface.toGlobal((*hit)->localPosition()).y(),
      bSurface.toGlobal((*hit)->localPosition()).z());
  GlobalVector hitMom(track.outerPx(),track.outerPy(),track.outerPz());

  TrackCharge ch = track.charge();
  const FreeTrajectoryState FTS (hitPos, hitMom, ch, &(*theMF_));
  //std::cout << " fts position " << FTS.position()  << " momentum " <<  FTS.momentum()  << std::endl;

  stateAtECAL_ = forwardPropagator_->propagate(FTS, barrel()) ;

  if(!stateAtECAL_.isValid() || ( stateAtECAL_.isValid() && fabs(stateAtECAL_.globalPosition().eta() ) >1.479 ))
  {
    if(FTS.position().eta() > 0.)
    {
      stateAtECAL_= forwardPropagator_->propagate( FTS, positiveEtaEndcap());
    }
    else
    {
      stateAtECAL_= forwardPropagator_->propagate( FTS, negativeEtaEndcap());
    }
  }  

  math::XYZPoint ecalImpactPosition(0.,0.,0.);
  if (stateAtECAL_.isValid()) ecalImpactPosition = stateAtECAL_.globalPosition();
  return ecalImpactPosition;
}


//---------------------------------------------------------------------------------------------------
math::XYZPoint EcalDeDxAnalyzer::propagateTrack(const TrackingParticle track)
{
  std::vector<PSimHit> simHits = track.trackPSimHit();
  if ( simHits.size() == 0 ) return math::XYZPoint(0.,0.,0.);
  std::vector<PSimHit>::const_iterator hit=simHits.end();
  hit--;

  GlobalPoint hitPos = localPosToGlobalPos(*hit);
  GlobalVector hitMom = localMomToGlobalMom(*hit);

  TrackCharge ch=track.charge();
  const FreeTrajectoryState FTS (hitPos, hitMom , ch  ,  &(*theMF_) );   
  //std::cout << " fts position " << FTS.position()  << " momentum " <<  FTS.momentum()  << std::endl;   


  stateAtECAL_ = forwardPropagator_->propagate( FTS, barrel() ) ;

  if (!stateAtECAL_.isValid() || ( stateAtECAL_.isValid() && fabs(stateAtECAL_.globalPosition().eta() ) >1.479 )  )
  {
    if ( FTS.position().eta() > 0.)
    {
      stateAtECAL_= forwardPropagator_->propagate( FTS, positiveEtaEndcap());
    }
    else
    {
      stateAtECAL_= forwardPropagator_->propagate( FTS, negativeEtaEndcap());
    }
  }

  math::XYZPoint ecalImpactPosition(0.,0.,0.);
  if ( stateAtECAL_.isValid() ) ecalImpactPosition = stateAtECAL_.globalPosition();
  return ecalImpactPosition;
}


//---------------------------------------------------------------------------------------------------
GlobalPoint EcalDeDxAnalyzer::localPosToGlobalPos(const PSimHit hit)
{
  using namespace std;
  DetId theDetUnitId(hit.detUnitId());
  const GeomDetUnit *theDet =  trackerGeom->idToDetUnit(theDetUnitId);
  const BoundPlane& bSurface = theDet->surface();
  GlobalPoint hitPos(bSurface.toGlobal(hit.localPosition()).x(),
      bSurface.toGlobal(hit.localPosition()).y(),
      bSurface.toGlobal(hit.localPosition()).z());
  return hitPos;
}


//---------------------------------------------------------------------------------------------------
GlobalVector EcalDeDxAnalyzer::localMomToGlobalMom(const PSimHit hit)
{
  DetId theDetUnitId(hit.detUnitId());
  const GeomDetUnit *theDet =  trackerGeom->idToDetUnit(theDetUnitId);
  const BoundPlane& bSurface = theDet->surface();
  GlobalVector hitMom(bSurface.toGlobal(hit.momentumAtEntry()).x(),
      bSurface.toGlobal(hit.momentumAtEntry()).y(),
      bSurface.toGlobal(hit.momentumAtEntry()).z()); 
  return hitMom;
}

//---------------------------------------------------------------------------------------------------
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


//---------------------------------------------------------------------------------------------------
// Make a 5x5/3x3 cluster around the seed crystal and plot up the energy
// Currently only works for EB
BozoCluster EcalDeDxAnalyzer::makeBozoCluster(const EcalRecHit &seed, const edm::Handle<EBRecHitCollection> &hits)
{
  EBDetId Xtals5x5[25];
  for (unsigned int icry=0;icry<25;icry++)
  {
    unsigned int row = icry / 5;
    unsigned int column= icry %5;
    try
    {
      int ieta=((EBDetId)seed.id()).ieta()+column-2;
      int iphi=((EBDetId)seed.id()).iphi()+row-2;
      if(EBDetId::validDetId(ieta, iphi))
      {
        EBDetId tempId(ieta, iphi); 
        Xtals5x5[icry]=tempId;
      }
      else
        Xtals5x5[icry]=EBDetId(0);
    }
    catch ( std::runtime_error &e )
    {
      Xtals5x5[icry]=EBDetId(0);   
    }
  }

  double energy[25];
  double energy3x3=0;  
  double energy5x5=0;  

  for (unsigned int icry=0;icry<25;icry++)
  {
    if(!Xtals5x5[icry].null())
    {
      energy[icry]=(hits->find(Xtals5x5[icry]))->energy();
      energy5x5 += energy[icry];
      // Is it in 3x3?
      if ( icry == 6  || icry == 7  || icry == 8 ||
          icry == 11 || icry == 12 || icry ==13 ||
          icry == 16 || icry == 17 || icry ==18   )
      {
        energy3x3+=energy[icry];
      }
    }
  }
  cluster3x3Hist_->Fill(energy3x3);
  cluster5x5Hist_->Fill(energy5x5);
  return BozoCluster(seed, energy3x3);
}


// ------------ method called once each job just before starting event loop  ------------
void EcalDeDxAnalyzer::beginJob(const edm::EventSetup& setup)
{
  setup.get<IdealMagneticFieldRecord>().get(theMF_);
  forwardPropagator_ = new PropagatorWithMaterial ( dir_ = alongMomentum, mcParticleMass_, &(*theMF_) );
}

// ------------ method called once each job just after ending the event loop  ------------
void EcalDeDxAnalyzer::endJob() {
  using namespace std;
  
  TFile a("test.root","RECREATE");

  betheBlochKFunct->Write();
  
  TProfile* deDxProfile_ = (TProfile*) (deDxVsBetaHist_->ProfileX());
  //deDxProfile_->Fit("betheBlochK");
  deDxProfile_->Write();

  TGraph* maxDeDxVsBeta;
  vector<double> xBetaVec;
  vector<double> yMaxDeDxVec;

  for(int i=0; i<10; i++)
  {
    string title = "DeDxBetaHist"+floatToString(i*.1)+"_"+floatToString((i+1)*.1);
    TH1D* deDxBetaProjection_ = (TH1D*) (deDxVsBetaHist_->ProjectionY(title.c_str(),i,i+1));
    deDxBetaProjection_->Write();
    yMaxDeDxVec.push_back((deDxBetaProjection_->GetMaximumBin())*.001);
    xBetaVec.push_back((i+1)*.1);
  }

  maxDeDxVsBeta = new TGraph(xBetaVec.size(), &(*xBetaVec.begin()), &(*yMaxDeDxVec.begin()));
  maxDeDxVsBeta->SetTitle("maxDeDx vs. beta");
  maxDeDxVsBeta->SetName("maxDeDxVsBetaGraph");
  maxDeDxVsBeta->Write();
  
  TProfile* clusterDeDxProfile_ = (TProfile*) (clusterDeDxVsBetaHist_->ProfileX());
  clusterDeDxProfile_->Write();
  
  clusterDeDxVsBetaHist_->Write();
  deDxVsBetaGraph_ = new TGraph(vx.size(), &(*vx.begin()), &(*vy.begin()));
  deDxVsBetaGraph_->Fit("betheBlochK");
  deDxVsBetaGraph_->SetTitle("DeDx vs. beta (all hits)");
  deDxVsBetaGraph_->SetName("dedxVsBetaFittedGraph");
  deDxVsBetaGraph_->Write();
  //deDxVsBetaHist_->Fit("betheBlochK");
  deDxVsBetaHist_->Write();
  energyVsMomentum_->Write();
  energyHist_->Write();
  momentumHist_->Write();
  highestEnergyHist_->Write();
  bozoClusterEnergyHist_->Write();
  energyVsBetaHist_->Write();
  simTrackEtaHist_->Write();
  simTrackPhiHist_->Write();
  ecalHitEtaHist_->Write();
  ecalHitPhiHist_->Write();
  hitEtaPhiHist_->Write();
  cluster3x3Hist_->Write();
  cluster5x5Hist_->Write();
  simHitsEnergyHist_->Write();
  simTrackRecoTrackDeltaEtaHist_->Write();
  simTrackRecoTrackDeltaPhiHist_->Write();
  
  a.Write();
  a.Close();

}


std::string EcalDeDxAnalyzer::floatToString(float f)
{
  using namespace std;
  ostringstream i;
  i << f << flush;
  return i.str();
}


void EcalDeDxAnalyzer::initializeSurfaces()
{
  const float epsilon = 0.001;
  Surface::RotationType rot; // unit rotation matrix

  theBarrel_ = new BoundCylinder( Surface::PositionType(0,0,0), rot, 
      SimpleCylinderBounds( barrelRadius()-epsilon, 
        barrelRadius()+epsilon, 
        -barrelHalfLength(), 
        barrelHalfLength()));

  theNegativeEtaEndcap_ = 
    new BoundDisk( Surface::PositionType( 0, 0, -endcapZ()), rot, 
        SimpleDiskBounds( 0, endcapRadius(), -epsilon, epsilon));

  thePositiveEtaEndcap_ = 
    new BoundDisk( Surface::PositionType( 0, 0, endcapZ()), rot, 
        SimpleDiskBounds( 0, endcapRadius(), -epsilon, epsilon));

  theInit_ = true;


}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalDeDxAnalyzer);
