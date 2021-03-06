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
// $Id: EcalDeDxAnalyzer.cc,v 1.17 2008/03/12 09:09:44 scooper Exp $
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
      //int findTrack(const EcalRecHit &seed, const edm::SimTrackContainer *tracks, const CaloSubdetectorGeometry* geometry_p);
      double dPhi(double phi1, double phi2);
      double dR(double eta1, double phi1, double eta2, double phi2);
      BozoCluster makeBozoCluster(const EcalRecHit &seed, const edm::Handle<EBRecHitCollection> &hits);
      std::string floatToString(float f);
      math::XYZPoint propagateTrack(const reco::Track track);
      math::XYZPoint propagateTrack(const TrackingParticle track);
      math::XYZPoint propagateThroughEcal(const reco::Track track);
      math::XYZPoint propagateThroughEcal(const TrackingParticle track);
      //math::XYZPoint EcalDeDxAnalyzer::propagateTrack(const edm::SimTrack track);
      GlobalPoint localPosToGlobalPos(const PSimHit hit);
      GlobalVector localMomToGlobalMom(const PSimHit hit);
      /** Hard-wired numbers defining the surfaces on which the crystal front faces lie. */
      //static float barrelRadius() {return 129.f;} //p81, p50, ECAL TDR
      static float barrelRadius() {return 131.5f;} //changed 6/12/08
      //static float barrelOuterRadius() {return 152.f;} //p50, ECAL TDR
      //static float barrelOuterRadius() {return 153.5f;} //changed 6/12/08
      static float barrelOuterRadius() {return 148.7f;} //changed 6/12/08
      static float barrelHalfLength() {return 270.9f;} //p81, p50, ECAL TDR
      static float endcapRadius() {return 171.1f;} // fig 3.26, p81, ECAL TDR
      static float endcapZ() {return 320.5f;} // fig 3.26, p81, ECAL TDR
      static void initializeSurfaces();
      static void check() {if (!theInit_) initializeSurfaces();}
      static ReferenceCountingPointer<BoundCylinder>  theBarrel_;
      static ReferenceCountingPointer<BoundCylinder>  theOuterBarrel_;
      static ReferenceCountingPointer<BoundDisk>      theNegativeEtaEndcap_;
      static ReferenceCountingPointer<BoundDisk>      thePositiveEtaEndcap_;
      static const BoundCylinder& barrel()        { check(); return *theBarrel_;}
      static const BoundCylinder& outerBarrel()        { check(); return *theOuterBarrel_;}
      static const BoundDisk& negativeEtaEndcap() { check(); return *theNegativeEtaEndcap_;}
      static const BoundDisk& positiveEtaEndcap() { check(); return *thePositiveEtaEndcap_;}
      static bool theInit_;

      edm::ESHandle<MagneticField> theMF_;
      //TrackAssociatorBase* associatorByHits_;
      TrajectoryStateOnSurface stateAtECAL_;
      TrajectoryStateOnSurface stateAfterECAL_;
      mutable PropagatorWithMaterial* forwardPropagator_ ;
      PropagationDirection dir_;
      const TrackerGeometry *trackerGeom;

      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag trackProducer_;
      edm::InputTag trackingParticleCollection_;
      edm::InputTag simTrackContainer_;
      TH2D* energyVsMomentum_;
      TH1D* energyHist_;
      TH1D* energyFitterHist_;
      TH1D* momentumHist_;
      TH1D* highestEnergyHist_;
      TH1D* bozoClusterEnergyHist_;
      TH2D* energyVsBetaHist_;
      TH2D* hitEtaPhiHist_;
      TH2D* deDxVsBetaHist_;
      TH2D* clusterDeDxVsBetaHist_;
      TH2D* deDxVsGammaBetaHist_;
      TH1D* simTrackEtaHist_;
      TH1D* simTrackPhiHist_;
      TH1D* ecalHitEtaHist_;
      TH1D* ecalHitPhiHist_;
      TH1D* cluster3x3Hist_;
      TH1D* cluster5x5Hist_;
      TH1D* simHitsEnergyHist_;
      TH1D* simHitsTimeHist_;
      TH1D* simTrackRecoTrackDeltaEtaHist_;
      TH1D* simTrackRecoTrackDeltaPhiHist_;
      TH1D* timingHist_;
      TH1D* fittedTimingHist_;
      TH2D* fitVsWeightsHist_;
      TH1D* chi2Fitter_;
      TH2D* fitTimeVsChi2_;
      TH2D* phiVsTime_;

      TGraph* deDxVsBetaGraph_;
      TF1* betheBlochKFunct;
      TFile* a;
      
      double dRHitTrack_;
      double minTrackPt_;
      double mcParticleMass_;
      double trackerHitMinP_;
      
      std::vector<double> vx;
      std::vector<double> vy;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//
ReferenceCountingPointer<BoundCylinder>  EcalDeDxAnalyzer::theBarrel_ = 0;
ReferenceCountingPointer<BoundCylinder>  EcalDeDxAnalyzer::theOuterBarrel_ = 0;
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
  std::cout << "Constructor for EcalDeDxAnalyzer" << std::endl;
  
   //now do what ever initialization is needed
   EBHitCollection_            = ps.getParameter<edm::InputTag>("EBRecHitCollection");
   EEHitCollection_            = ps.getParameter<edm::InputTag>("EERecHitCollection");
   trackProducer_              = ps.getParameter<edm::InputTag>("trackProducer");
   minTrackPt_                 = ps.getParameter<double>("minTrackPt");
   simTrackContainer_          = ps.getParameter<edm::InputTag>("simTrackContainer");
   trackingParticleCollection_ = ps.getParameter<edm::InputTag>("trackingParticleCollection");
   mcParticleMass_             = ps.getUntrackedParameter<double>("mcParticleMass",-1.0);
   dRHitTrack_                 = ps.getParameter<double>("dRTrack");
   trackerHitMinP_             = ps.getParameter<double>("trackerMinHitP_");
   
   a = new TFile("test.root","RECREATE");
   
   TH1::AddDirectory(false);
   
   energyVsMomentum_ = new TH2D("energy_vs_momentum","energy dep. in ecal vs. momentum",8000,0,2000,100,0,1.5);
   energyHist_ = new TH1D("energy_in_ecal","energy dep. in ecal",1000,0,10.0);
   energyFitterHist_ = new TH1D("energy_in_ecal_fitter","energy dep. in ecal (fitter)",1000,0,10.0);
   momentumHist_ = new TH1D("tracker_outer_momentum","tracker outer momentum",8000,0,2000);
   highestEnergyHist_ = new TH1D("highest_energy_recHits","highest energy track-matched RecHits",100,0,1.5);
   bozoClusterEnergyHist_ = new TH1D("bozo_cluster_recHits","track-matched bozo clusters",100,0,1.5);
   energyVsBetaHist_ = new TH2D("energy_vs_beta","energy dep. in ecal vs. beta",100,0,1,100,0,1.5);
   hitEtaPhiHist_ = new TH2D("hit_eta_Phi","eta and phi of ecal RecHits",300,-1.5,1.5,720,-3.14,3.14);
   cluster3x3Hist_ = new TH1D("3x3_cluster_energy","E3x3 energy", 100, 0, 1.5);
   cluster5x5Hist_ = new TH1D("5x5_cluster_energy","E5x5 energy", 100, 0, 1.5);
   deDxVsBetaHist_ = new TH2D("dedx_vs_beta","dE/dx vs. beta",25,0,1,60,0,.06);
   //TODO: How to define path length of particle in detector for a cluster??
   clusterDeDxVsBetaHist_ = new TH2D("cluster_dedx_vs_beta","dE/dx from cluster vs. beta",100,0,1,60,0,.06);
   deDxVsGammaBetaHist_ = new TH2D("dedx_vs_gamma_beta","dE/dx vs. beta*gamma",400,0,100,60,0,.06);
   
   // e vs. beta hist only valid when we know the mass (i.e., protons)!
   betheBlochKFunct = new TF1("betheBlochK","(1/[0])*(1/pow(x,2))",0.5,1);
   betheBlochKFunct->SetParNames("K");
   betheBlochKFunct->SetParameter(0,150);
   
   simTrackEtaHist_ = new TH1D("simTracksEta","Eta of MC tracks",80,-4,4);
   simTrackPhiHist_ = new TH1D("simTracksPhi","Phi of MC tracks",120,-6.4,6.4);
   simHitsEnergyHist_ = new TH1D("simHitsEnergy","Energy of sim hits",200,0,2);
   simHitsTimeHist_ = new TH1D("simHitsTime","Time of sim hits",500,-25,25);
   ecalHitEtaHist_ = new TH1D("ecalHitsEta","Eta of hits in ecal",600,-3,3);
   ecalHitPhiHist_ = new TH1D("ecalHitsPhi","Phi of hits in ecal",720,-3.14,3.14);
   simTrackRecoTrackDeltaEtaHist_ = new TH1D("tracksDeltaEta","Delta eta of sim tracks and reco tracks",1000,-.01,.01);
   simTrackRecoTrackDeltaPhiHist_ = new TH1D("tracksDeltaPhi","Delta phi of sim tracks and reco tracks",2000,-.04,.04);
   timingHist_ = new TH1D("timingHist","Time of max sample",500,-25,25);
   fittedTimingHist_ = new TH1D("fittedTimingHist","Time of max sample (fitter)",500,-25,25);
   fitVsWeightsHist_ = new TH2D("fitVsWeights","fittedTime vs. weightsTime",500,-25,25,500,-25,25);
   chi2Fitter_ = new TH1D("chi2Fitter","Chi2 of the fitted hits",400,-200,200);
   fitTimeVsChi2_ = new TH2D("fitTimeVsChi2","chi2 vs. fitted time",500,-25,25,400,-200,200);
   phiVsTime_ = new TH2D("phiVsTime","phi vs. reco time",500,-25,25,720,-3.14,3.14);
   
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
  
  //Added for fitter timing
  Handle<EBRecHitCollection> EBFittedHits;
  Handle<EERecHitCollection> EEFittedHits;
  Handle<EcalUncalibratedRecHitCollection> EcalUncalibHits;

  try
  {
    iEvent.getByLabel(trackProducer_, trackHandle);
    iEvent.getByLabel(EBHitCollection_, EBHits);
    iEvent.getByLabel(EEHitCollection_, EEHits);
    iEvent.getByLabel(simTrackContainer_, simTracks);
    //Added for fitter timing
    iEvent.getByLabel(edm::InputTag("ecalRecHit:ecalFittedRecHitsEB"), EBFittedHits);
    iEvent.getByLabel(edm::InputTag("ecalRecHit:ecalFittedRecHitsEE"), EEFittedHits);
    iEvent.getByLabel(edm::InputTag("ecalFixedAlphaBetaFitUncalibRecHit:EcalUncalibRecHitsEB"), EcalUncalibHits);
  }
  catch(cms::Exception& ex)
  {
    edm::LogError("EcalDeDxAnalyzer") << "Error! Can't get collections";
    throw ex; 
  }

  cout << "Number of reco tracks: " << trackHandle.product()->size() << endl;
  cout << "Number of EB hits: " << EBHits.product()->size() << endl;
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
  //const CaloSubdetectorGeometry *geometry_endcap = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
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
    
    cout << "PDG id:" << trackItr->pdgId() << endl;
   
    //TODO: Make this an honest-to-goodness Pt cut (in GeV)
    if(hit->pabs() > trackerHitMinP_)
    {
      double eta = localPosToGlobalPos(*hit).eta();
      double phi = localPosToGlobalPos(*hit).phi();
      //cout << "PDG ID:" << trackItr->pdgId() << endl;
      //cout << "DEBUG1--line 301" << endl;
      //cout << "eta: " << eta << " phi: " << phi << endl;
      simTrackEtaHist_->Fill(eta);
      //cout << "Filled eta" << endl;
      simTrackPhiHist_->Fill(phi);
      //cout << "Filled phi" << endl;
      simTrackCount++;
      //cout << "DEBUG1.5" << endl;
      math::XYZPoint ecalSimTrackPos = propagateTrack(*trackItr);
      //cout << "DEBUG2" << endl;
      math::XYZPoint ecalRecoTrackPos = math::XYZPoint(0,0,0);
      if(trackHandle.product()->size()!=0)
      {
        math::XYZPoint ecalRecoTrackPos = propagateTrack((*trackHandle.product())[0]);
        //cout << "DEBUG3" << endl;
        simTrackRecoTrackDeltaPhiHist_->Fill(ecalSimTrackPos.Phi()-ecalRecoTrackPos.Phi());
        simTrackRecoTrackDeltaEtaHist_->Fill(ecalSimTrackPos.Eta()-ecalRecoTrackPos.Eta());
      }
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
    simHitsTimeHist_->Fill(myHit->time());
  }
  
  
  const reco::TrackCollection* tracks = trackHandle.product();
  int numTracks = tracks->size();
 
  //debug 
  //cout << "loop over tracks" << endl;
  
  // Loop over tracks
  for(int j = 0; j < numTracks; j++)
  {
    float highestE = -1;
    double bestDR = 10;
    reco::TrackRef bestTrack;
    double trackMatchedP = 0;
    EcalRecHit highestEHit;
    EcalRecHit bestHit;
    //int tracksFound = 0;
    //bool isSingleCryTrack = false;

    double trackPt = (*tracks)[j].outerPt();
    if(trackPt < minTrackPt_)
      continue;

    //Propagate tracks
    math::XYZPoint ecalHitPt = propagateTrack((*tracks)[j]);
    math::XYZPoint outerEcalHitPt = propagateThroughEcal((*tracks)[j]);

    for(EBRecHitCollection::const_iterator recItr = EBHits->begin(); recItr != EBHits->end(); ++recItr)
    {
      // Get position of the RecHit
      const CaloCellGeometry *cell_p = geometry_barrel->getGeometry(recItr->id());
      // Center of xtal
      GlobalPoint xtalCenter = (dynamic_cast<const TruncatedPyramid*>(cell_p))->getPosition(0);

      double dRtrack = dR(ecalHitPt.eta(), ecalHitPt.phi(), xtalCenter.eta(), xtalCenter.phi());
      //debug 
      //cout << "after examining dR..." << endl;
      if(dRtrack < dRHitTrack_ && dRtrack < bestDR)
      {
        //debug 
        //cout << "new best track found!" << endl;
        bestDR = dRtrack;
        bestHit = *recItr;
        trackMatchedP = (*tracks)[j].outerP();
        if(recItr->energy() > highestE)
        {
          //debug 
          //cout << "new highest energy hit found!" << endl;
          highestE = recItr->energy();
          highestEHit = (*recItr);
        }
      }
    }
    // Now fill histos and stuff
    if(bestDR < 10)  // make sure we have a track-matched hit, or else geometry will seg fault
    {
      //Now these hits are for all track-matched hits
      ecalHitEtaHist_->Fill(ecalHitPt.eta());
      ecalHitPhiHist_->Fill(ecalHitPt.phi());
      hitEtaPhiHist_->Fill(ecalHitPt.eta(),ecalHitPt.phi());
      //debug 
      //cout << "creating caloCellGeom..." << endl;
      const CaloCellGeometry *cell_p = geometry_barrel->getGeometry(bestHit.id());
      //debug 
      //cout << "after creating caloCell..." << endl;
      const TruncatedPyramid* cryPyramid = dynamic_cast<const TruncatedPyramid*>(cell_p);
      GlobalPoint ecalSurfaceHitGlobPt(ecalHitPt.x(), ecalHitPt.y(), ecalHitPt.z());
      //TODO: correction for ecal outer radius variation
      GlobalPoint ecalOuterHitGlobPt(outerEcalHitPt.x(), outerEcalHitPt.y(), outerEcalHitPt.z());
      //cout << "Is the surface hit point inside the best hit crystal? " << cryPyramid->inside(ecalSurfaceHitGlobPt) << endl;
      //cout << "Is the outer hit point inside the best hit crystal? " << cryPyramid->inside(ecalOuterHitGlobPt) << endl;

      //if(cryPyramid->inside(ecalOuterHitGlobPt))  //if the track exits the best-matched crystal
      if(cryPyramid->inside(ecalOuterHitGlobPt))
      {
        double energy = bestHit.energy();
        energyVsMomentum_->Fill(trackMatchedP, energy);
        energyHist_->Fill(energy);
        //Added for fitter energy
        EBRecHitCollection::const_iterator fittedHit = EBFittedHits->begin();
        while(fittedHit != EBFittedHits->end() && fittedHit->id() != bestHit.id())
          fittedHit++;
        if(fittedHit != EBFittedHits->end())
          energyFitterHist_->Fill(fittedHit->energy());
        //Added for fitter timing
        if(bestHit.energy() > 0.320) // SIC 7/21/08
        {
          //find matching uncalibrechit
          double chi2 = -2011;
          EcalUncalibratedRecHitCollection::const_iterator uncalibHit = EcalUncalibHits->begin();
          while(uncalibHit != EcalUncalibHits->end() && uncalibHit->id() != bestHit.id())
            uncalibHit++;
          if(uncalibHit != EcalUncalibHits->end())
          {
            chi2 = uncalibHit->chi2();
            chi2Fitter_->Fill(chi2);
          }
          timingHist_->Fill(bestHit.time());
          EBRecHitCollection::const_iterator fittedHit = EBFittedHits->begin();
          while(fittedHit != EBFittedHits->end() && fittedHit->id() != bestHit.id())
            fittedHit++;
          if(fittedHit != EBFittedHits->end())
          {
            fittedTimingHist_->Fill(25*fittedHit->time());
            fitVsWeightsHist_->Fill(bestHit.time(),25*fittedHit->time());
            phiVsTime_->Fill(25*fittedHit->time(),ecalHitPt.phi());
            if(chi2 != -2011)
              fitTimeVsChi2_->Fill(25*fittedHit->time(),chi2);
          }
        }
        momentumHist_->Fill(trackMatchedP);
        if(mcParticleMass_ > -1)
        {
          double beta = trackMatchedP/sqrt(pow(trackMatchedP,2)+pow(mcParticleMass_,2));
          double gamma = 1/sqrt(1-beta*beta);
          cout << "Beta is: " << beta << endl;
          energyVsBetaHist_->Fill(beta, energy);
          if(beta > .5) //energy/23.0 > .003)
          {
            deDxVsGammaBetaHist_->Fill(beta*gamma,energy/23.0);
            deDxVsBetaHist_->Fill(beta,energy/23.0);
            cout << "Added point (e/l,beta): " << energy/23.0 << "," << beta << endl;
            vx.push_back(beta);
            vy.push_back(energy/23.0);  //TODO: This is only a very rough crystal length approximation
            //      Can we fix using propagator?
          }

        }
      }
    }
  }

  


//  // Loop over the RecHits, first EBRechits
//  // TODO: Should we look at tracks first instead?
//  for(EBRecHitCollection::const_iterator recItr = EBHits->begin(); recItr != EBHits->end(); ++recItr)
//  {
//    int trackFound = findTrack(*recItr, trackHandle.product(), geometry_barrel);
//    if(trackFound != -1)
//    {
//      tracksFound++;
//      const reco::TrackRef track = edm::Ref<reco::TrackCollection>(trackHandle, trackFound);
//      // Get position of the RecHit
//      const CaloCellGeometry *cell_p = geometry_barrel->getGeometry(recItr->id());
//      // Center of xtal
//      GlobalPoint xtalCenter = (dynamic_cast<const TruncatedPyramid*>(cell_p))->getPosition(0);
//      //debug
//      //cout << "track-matched xtal:" << *(dynamic_cast<const TruncatedPyramid*>(cell_p)) << endl;
//      math::XYZPoint ecalHitPos = propagateTrack(*track);
//      double dRtrack = dR(ecalHitPos.eta(), ecalHitPos.phi(), xtalCenter.eta(), xtalCenter.phi());
//      if(dRtrack < bestDR)
//      {
//        bestDR = dRtrack;
//        bestTrack = track;
//        bestHit = *recItr;
//      }
//
//      double p = track->outerP();
//      float energy = (recItr->energy());
//      if(energy > highestE)
//      {
//        highestE = energy;
//        highestEHit = (*recItr);
//        trackMatchedP = p;
//      }
//    }
//  }
      //double p = 0;
      //Get e, p from MC for zero B field
  cout << "Number of simTracks:" << simTracks->size() << endl;
      //for(SimTrackContainer::const_iterator simTrack = simTracks->begin(); simTrack != simTracks->end(); ++simTrack)
      //{
        //if((simTrack->type())==2212)
        //{
         // p = (sqrt(pow((simTrack->momentum()).px(),2)+pow((simTrack->momentum()).py(),2)
         //       +pow((simTrack->momentum()).pz(),2)));
        //}
      //}

//      int simTrackFound = findTrack(*recItr, simTracks, geometry_barrel);
//      if(simTrackFound != -1 && mcParticleMass_ > -1)
//      {
//        const SimTrack MCtrack = (*simTracks)[simTrackFound];
//        pFromMC = (sqrt(pow((MCtrack->momentum()).px(),2)+pow((MCtrack->momentum()).py(),2)
//                +pow((MCtrack->momentum()).pz(),2)));
//        
//        double beta = 1/sqrt(K*energy/23.0);
//        double betaSim = pFromMC/sqrt(pow(pFromMC,2)+pow(mcParticleMass_,2));
//        
//        deltaPHist_->Fill(betaSim-beta);
//      }

//  if(tracksFound==1)  //for now, only fill if we have 1 and only 1 track-matched RecHit
//  {                   //TODO: two or more cry analysis using path length/tracking info
//    double p = bestTrack->outerP();
//    float energy = bestHit.energy();
//    const CaloCellGeometry *cell_p = geometry_barrel->getGeometry(bestHit.id());
//    // Center of xtal
//    GlobalPoint pt = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
//    cout << "Best EBRecHit Energy: " << energy << " Best recoTrack Momentum: " << p << endl;
//    energyVsMomentum_->Fill(p, energy);
//    energyHist_->Fill(energy);
//    momentumHist_->Fill(p);
//    ecalHitEtaHist_->Fill(pt.eta());
//    ecalHitPhiHist_->Fill(pt.phi());
//    hitEtaPhiHist_->Fill(pt.eta(),pt.phi());
//    if(mcParticleMass_ > -1)
//    {
//      double beta = p/sqrt(pow(p,2)+pow(mcParticleMass_,2));
//      double gamma = 1/sqrt(1-beta*beta);
//      cout << "Beta is: " << beta << endl;
//      energyVsBetaHist_->Fill(beta, energy);
//      if(beta > .5) //energy/23.0 > .003)
//      {
//        deDxVsGammaBetaHist_->Fill(beta*gamma,energy/23.0);
//        deDxVsBetaHist_->Fill(beta,energy/23.0);
//        cout << "Added point (e/l,beta): " << energy/23.0 << "," << beta << endl;
//        vx.push_back(beta);
//        vy.push_back(energy/23.0);  //TODO: This is only a very rough crystal length approximation
//        //      Can we fix using propagator?
//      }
//    }
//  }
//  else
//    cout << "Event contains " << tracksFound << " track-matched recHits" << endl;

//  BozoCluster cluster;
//  if(highestE > -1)
//  {
//    cout << "Highest Energy Hit: " << highestE << endl;
//    highestEnergyHist_->Fill(highestE);
//    cluster = makeBozoCluster(highestEHit, EBHits);
//  }
//  if(cluster.getEnergy() != -1 && mcParticleMass_ > -1)
//  {
//
//    double beta = trackMatchedP/sqrt(pow(trackMatchedP,2)+pow(mcParticleMass_,2));
//    clusterDeDxVsBetaHist_->Fill(beta, cluster.getEnergy()/23.0);
//  }

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
  // Get position of the RecHit
  const CaloCellGeometry *cell_p = geometry_p->getGeometry(seed.id());
  // Center of xtal
  GlobalPoint p = (dynamic_cast<const TruncatedPyramid*>(cell_p))->getPosition(0);

  double lowestdR = 1000;
  for(unsigned int j = 0; j < numTracks; j++)
  {
    //Propagate tracks
    math::XYZPoint ecalHitPt = propagateTrack((*tracks)[j]);
    double dRtrack = dR(ecalHitPt.eta(), ecalHitPt.phi(), p.eta(), p.phi());
    //double dRtrack = dR((*tracks)[j].eta(), (*tracks)[j].phi(), p.eta(), p.phi());
    //std::cout << "dR of track to hit: " << dRtrack << std::endl;
    if(dRtrack < dRHitTrack_)
    {
      double trackPt = (*tracks)[j].outerPt();
      if((trackPt > minTrackPt_) && (dRtrack < lowestdR))
      {
        retTrack = j;
        lowestdR = dRtrack;
      }
    }
  }

  return retTrack;
}


//---------------------------------------------------------------------------------------------------
//int EcalDeDxAnalyzer::findTrack(const EcalRecHit &seed, const edm::SimTrackContainer *tracks,
//                                const CaloSubdetectorGeometry* geometry_p)
//{
//  int retTrack = -1;
//
//  // See if we have a track with pT > minTrackPt_ and dR < dRHitTrack
//  unsigned int numTracks = tracks->size();
//  double lowestdR = 1000;
//  // Get position of the RecHit
//  const CaloCellGeometry *cell_p = geometry_p->getGeometry(seed.id());
//  // Center of xtal
//  GlobalPoint p = (dynamic_cast<const TruncatedPyramid*>(cell_p))->getPosition(0);
//
//  for(unsigned int j = 0; j < numTracks; j++)
//  {
//    //Propagate tracks
//    math::XYZPoint ecalHitPt = propagateTrack((*tracks)[j]);
//    double dRtrack = dR(ecalHitPt.eta(), ecalHitPt.phi(), p.eta(), p.phi());
//    //double dRtrack = dR((*tracks)[j].eta(), (*tracks)[j].phi(), p.eta(), p.phi());
//    //std::cout << "dR of track to hit: " << dRtrack << std::endl;
//    if(dRtrack < dRHitTrack)
//    {
//      double trackPt = (*tracks)[j].outerPt();
//      if((trackPt > minTrackPt_) && (dRtrack < lowestdR))
//      {
//        retTrack = j;
//        lowestdR = dRtrack;
//      }
//    }
//  }
//
//  return retTrack;
//}


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
math::XYZPoint EcalDeDxAnalyzer::propagateThroughEcal(const reco::Track track)
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

  stateAfterECAL_ = forwardPropagator_->propagate(FTS, outerBarrel());

  if(!stateAfterECAL_.isValid() || ( stateAfterECAL_.isValid() && fabs(stateAfterECAL_.globalPosition().eta() ) >1.479 ))
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
  if (stateAfterECAL_.isValid()) ecalImpactPosition = stateAfterECAL_.globalPosition();
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
math::XYZPoint EcalDeDxAnalyzer::propagateThroughEcal(const TrackingParticle track)
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


  stateAfterECAL_ = forwardPropagator_->propagate( FTS, outerBarrel() ) ;

  if (!stateAfterECAL_.isValid() || ( stateAfterECAL_.isValid() && fabs(stateAfterECAL_.globalPosition().eta() ) >1.479 )  )
  {
    if ( FTS.position().eta() > 0.)
    {
      stateAfterECAL_= forwardPropagator_->propagate( FTS, positiveEtaEndcap());
    }
    else
    {
      stateAfterECAL_= forwardPropagator_->propagate( FTS, negativeEtaEndcap());
    }
  }

  math::XYZPoint ecalImpactPosition(0.,0.,0.);
  if ( stateAfterECAL_.isValid() ) ecalImpactPosition = stateAfterECAL_.globalPosition();
  return ecalImpactPosition;
}

//---------------------------------------------------------------------------------------------------
//math::XYZPoint EcalDeDxAnalyzer::propagateTrack(const edm::SimTrack track)
//{
//  std::vector<PSimHit> simHits = track.trackPSimHit();
//  if ( simHits.size() == 0 ) return math::XYZPoint(0.,0.,0.);
//  std::vector<PSimHit>::const_iterator hit=simHits.end();
//  hit--;
//
//  GlobalPoint hitPos = localPosToGlobalPos(*hit);
//  GlobalVector hitMom = localMomToGlobalMom(*hit);
//
//  TrackCharge ch=track.charge();
//  const FreeTrajectoryState FTS (hitPos, hitMom , ch  ,  &(*theMF_) );   
//  //std::cout << " fts position " << FTS.position()  << " momentum " <<  FTS.momentum()  << std::endl;   
//
//
//  stateAtECAL_ = forwardPropagator_->propagate( FTS, barrel() ) ;
//
//  if (!stateAtECAL_.isValid() || ( stateAtECAL_.isValid() && fabs(stateAtECAL_.globalPosition().eta() ) >1.479 )  )
//  {
//    if ( FTS.position().eta() > 0.)
//    {
//      stateAtECAL_= forwardPropagator_->propagate( FTS, positiveEtaEndcap());
//    }
//    else
//    {
//      stateAtECAL_= forwardPropagator_->propagate( FTS, negativeEtaEndcap());
//    }
//  }
//
//  math::XYZPoint ecalImpactPosition(0.,0.,0.);
//  if ( stateAtECAL_.isValid() ) ecalImpactPosition = stateAtECAL_.globalPosition();
//  return ecalImpactPosition;
//}


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
  //std::cout << "beginJob" << std::endl;
  setup.get<IdealMagneticFieldRecord>().get(theMF_);
  forwardPropagator_ = new PropagatorWithMaterial ( dir_ = alongMomentum, mcParticleMass_, &(*theMF_) );
}

// ------------ method called once each job just after ending the event loop  ------------
void EcalDeDxAnalyzer::endJob() {
  using namespace std;
  
  a->cd();
  
  betheBlochKFunct->Write();
  
  TProfile* deDxProfile_ = (TProfile*) (deDxVsBetaHist_->ProfileX());
  deDxProfile_->SetTitle("Fitted dE/dx profile");
  deDxProfile_->SetName("dEdxProfileFitted");
  deDxProfile_->Fit("betheBlochK","R");
  deDxProfile_->Write();

  TProfile* deDxGammaBetaProfile_ = (TProfile*) (deDxVsGammaBetaHist_->ProfileX());
  deDxGammaBetaProfile_->Write();
  
  TGraph* maxDeDxVsBeta;
  vector<double> xBetaVec;
  vector<double> yMaxDeDxVec;

  for(int i=0; i<25; i++)
  {
    string title = "DeDxBetaHist"+floatToString(i*.25)+"_"+floatToString((i+1)*.25);
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
  deDxVsBetaGraph_->Fit("betheBlochK","R");
  deDxVsBetaGraph_->SetTitle("DeDx vs. beta (all hits)");
  deDxVsBetaGraph_->SetName("dedxVsBetaFittedGraph");
  deDxVsBetaGraph_->Write();
  deDxVsGammaBetaHist_->Write();
  //deDxVsBetaHist_->Fit("betheBlochK");
  deDxVsBetaHist_->Write();
  energyVsMomentum_->Write();
  energyHist_->Write();
  energyFitterHist_->Write();
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
  simHitsTimeHist_->Write();
  simTrackRecoTrackDeltaEtaHist_->Write();
  simTrackRecoTrackDeltaPhiHist_->Write();
  timingHist_->Write();
  fittedTimingHist_->Write();
  fitVsWeightsHist_->GetXaxis()->SetTitle("weights");
  fitVsWeightsHist_->GetYaxis()->SetTitle("fitter");
  fitVsWeightsHist_->Write();
  chi2Fitter_->Write();
  fitTimeVsChi2_->Write();
  phiVsTime_->Write();
  
  a->Write();
  a->Close();
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
  
  theOuterBarrel_ = new BoundCylinder( Surface::PositionType(0,0,0), rot, 
      SimpleCylinderBounds( barrelOuterRadius()-epsilon, 
        barrelOuterRadius()+epsilon, 
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
