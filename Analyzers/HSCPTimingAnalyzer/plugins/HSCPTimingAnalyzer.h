// -*- C++ -*-
//
// Package:    HSCPTimingAnalyzer
// Class:      HSCPTimingAnalyzer
// 
/**\class HSCPTimingAnalyzer HSCPTimingAnalyzer.cc Analyzers/HSCPTimingAnalyzer/plugins/HSCPTimingAnalyzer.h

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER
//         Created:  Wed May 19 14:54:43 CET 2010
// $Id: HSCPTimingAnalyzer.h,v 1.2 2010/06/30 15:09:15 scooper Exp $
//
//


// system include files
#include <memory>
#include <vector>
#include <map>
#include <string>

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
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TGraph.h"
#include "TNtuple.h"
#include "TTree.h"
#include "TFile.h"
#include "TROOT.h"

// DataFormats
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
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

#include "Analyzers/HSCPTimingAnalyzer/interface/HSCPEcalTreeContent.h"

// Auxiliary class
class towerEner {   
 public:
  float eRec_, etRec_ ;
  int tpgEmul_[5] ;
  int tpgADC_; 
  int iphi_, ieta_, nbXtal_, spike_ ;
  towerEner()
    : eRec_(0), etRec_(0), tpgADC_(0),  
    iphi_(-999), ieta_(-999), nbXtal_(0), spike_(0)
  { 
    for (int i=0 ; i<5 ; i ++) tpgEmul_[i] = 0 ; 
  }
};


// main class declaration
//

class HSCPTimingAnalyzer : public edm::EDAnalyzer {
   public:
      explicit HSCPTimingAnalyzer(const edm::ParameterSet&);
      ~HSCPTimingAnalyzer();


   private:
      virtual void beginJob();
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::vector<SteppingHelixStateInfo> calcEcalDeposit(const FreeTrajectoryState* tkInnerState,
          const TrackAssociatorParameters& parameters);
      std::vector<SteppingHelixStateInfo> calcDeposit(const FreeTrajectoryState* tkInnerState,
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
      std::string intToString(int num);


      // ----------member data ---------------------------
      edm::InputTag EBHitCollection_;
      edm::InputTag EEHitCollection_;
      edm::InputTag EBDigiCollection_;
      edm::InputTag muonCollection_;
      edm::InputTag trackCollection_;
      edm::InputTag EBUncalibRecHits_;
      edm::InputTag EEUncalibRecHits_;
      edm::InputTag jetCollection_;

      // SIC July 9 2010 for L1 trigger studies
      edm::InputTag isoEmSource_;
      edm::InputTag nonIsoEmSource_;
      edm::InputTag cenJetSource_;
      edm::InputTag forJetSource_;
      edm::InputTag tauJetSource_;
      edm::InputTag muonSource_;
      edm::InputTag etMissSource_;
      edm::InputTag htMissSource_;
      edm::InputTag hfRingsSource_;
      edm::InputTag particleMapSource_;
      // July 20 2010
      edm::InputTag tpCollection_;

      edm::ESHandle<DetIdAssociator> ecalDetIdAssociator_;
      edm::ESHandle<CaloGeometry> theCaloGeometry_;
      edm::ESHandle<GlobalTrackingGeometry> theTrackingGeometry_;
      edm::ESHandle<MagneticField> bField_;

      // Histograms
      TH1D* simHitsTimeHist_;
      TH1D* simHitsEnergyHist_;
      TH1D* recHitTimeSimHitTimeHist_;
      TH1D* recHitTimeHist_;
      TH1D* recHitEnergyHist_;
      TH1D* recHitMaxEnergyHist_;
      TH1D* recHitMinEnergyHist_;
      TH1D* recHitMaxEnergyTimingHist_;
      //TH1D* energyOfTrackMatchedHits_;
      //TH1D* timeOfTrackMatchedHits_;
      //TH2D* timeVsEnergyOfTrackMatchedHits_;
      TH1F* ptOfTracksWithCrossedCrysHist_;

      TH1F* timeOfTrackMatchedHitsEB1GeVcut_;
      TH1F* timeOfTrackMatchedHitsEB3GeVcut_;
      TH1F* timeOfTrackMatchedHitsEB5GeVcut_;
      TH1F* timeOfTrackMatchedHitsEE1GeVcut_;
      TH1F* timeOfTrackMatchedHitsEE3GeVcut_;
      TH1F* timeOfTrackMatchedHitsEE5GeVcut_;
      
      TH1D* energyOfTrackMatchedHitsEB_;
      TH1D* timeOfTrackMatchedHitsEB_;
      TH2D* timeVsEnergyOfTrackMatchedHitsEB_;

      TH1D* energyOfTrackMatchedHitsEE_;
      TH1D* timeOfTrackMatchedHitsEE_;
      TH2D* timeVsEnergyOfTrackMatchedHitsEE_;

      TH2F* timeChi2VsEnergyOfTrackMatchedHitsEB_;
      TH2F* timeChi2VsEnergyOfTrackMatchedHitsEE_;

      TH1F* outOfTimeEnergyOfTrackMatchedHitsEB_;
      TH1F* outOfTimeEnergyOfTrackMatchedHitsEE_;

      TH2D* timeVsDeDxOfTrackMatchedHits_;
      TH1D* crossedEnergy_;
      TH1D* crossedLength_;
      TH2D* recHitTimeVsSimHitTime_;
      TH1D* simHitsPerEventHist_;
      TH2D* recHitTimeSimHitTimeVsEnergy_;
      TH1D* recHitsPerTrackHist_;
      TH1F* deDx3x3Hist_;
      TH1F* deDx3x3CorrectedHist_;
      TH1F* deDx5x5Hist_;
      TH1F* deDx5x5CorrectedHist_;
      TH1F* numCrysIn3x3Hist_;
      TH1F* numCrysIn5x5Hist_;
      TH1F* numCrysIn7x7Hist_;
      TH1F* numCrysIn9x9Hist_;
      TH1F* numCrysIn11x11Hist_;
      TH1F* numCrysIn15x15Hist_;
      TH1F* numCrysIn19x19Hist_;
      TH1F* numCrysIn23x23Hist_;
      TH1F* numCrysIn27x27Hist_;
      TH1F* numCrysIn31x31Hist_;

      TH1F* timeError999EnergyHist_;

      TH1F* deDxHist_;
      TH1F* trackLengthPerCryHist_;
      TH1F* numMatchedCrysPerEventHist_;
      TH1F* deDxMaxEnergyCryHist_;
      TH1F* deDxMinEnergyCryHist_;
      TH1F* deDxTotalHist_;
      //TODO: separate into EE/EB; make more general
      TH1F* singleCryCrossedEnergy_;
      TH1F* singleCryCrossedTime_;
      TH1F* singleCryCrossedDeDx_;
      TH1F* singleCryCrossedTrackLength_;
      TH1F* singleCryCrossedChi2_;

      //TProfile* energyFractionInTrackMatchedXtalsProfile_;
      
      TH2F* numCrysCrossedVsNumHitsFoundHist_;
      TH2F* deDxVsMomHist_;
      TH2F* hit1EnergyVsHit2EnergyHist_;
      TH2F* hit1LengthVsHit2LengthHist_;
      TH2F* hit1DeDxVsHit2DeDxHist_;

      TH2F* energyVsDeDxHist_;
      TH1F* energyInDeDxSlice_[25];

      TH1D* muonEcalMaxEnergyTimingHist_;
      TH1D* muonEcalMaxEnergyHist_;
      
      TH1F* energy1OverEnergy9Hist_;
      TH1F* energy1OverEnergy25Hist_;
      TH1F* energy1OverEnergy49Hist_;
      TH1F* energy1OverEnergy81Hist_;
      TH1F* energy1OverEnergy121Hist_;
      TH1F* energy1OverEnergy225Hist_;
      TH1F* energy1OverEnergy361Hist_;
      TH1F* energy1OverEnergy529Hist_;
      TH1F* energy1OverEnergy729Hist_;
      TH1F* energy1OverEnergy961Hist_;

      TH1F* energyCrossedOverEnergy9Hist_;
      TH1F* energyCrossedOverEnergy25Hist_;
      TH1F* energyCrossedOverEnergy49Hist_;
      TH1F* energyCrossedOverEnergy81Hist_;
      TH1F* energyCrossedOverEnergy121Hist_;
      TH1F* energyCrossedOverEnergy225Hist_;
      TH1F* energyCrossedOverEnergy361Hist_;
      TH1F* energyCrossedOverEnergy529Hist_;
      TH1F* energyCrossedOverEnergy729Hist_;
      TH1F* energyCrossedOverEnergy961Hist_;

      TH2F* energy1VsE1OverE9Hist_;
      TH2F* energy9VsE1OverE9Hist_;
      
      TH1F* timePullDistEBHist_;
      TH1F* timePullDistEEHist_;

      TH2F* TsigTvsArmsEBHist_;
      TH2F* TsigTvsArmsEEHist_;
      
      //TProfile* recHitMaxEnergyShapeProfile_;
      TGraph* recHitMaxEnergyShapeGraph_;

      TH1F* dRJetHSCPHist_;
      TH1F* ptJetHSCPHist_;

      // L1 hists
      TH1F* L1IsoEmDrHist_;
      TH1F* L1IsoEmEnergyHist_;
      TH1F* L1NonIsoEmDrHist_;
      TH1F* L1JetCenDrHist_;
      TH1F* L1JetForDrHist_;
      TH1F* L1JetTauDrHist_;
      TH1F* L1MuDrHist_;
      TH1F* L1METDrHist_;
      TH1F* L1MHTDrHist_;
      TH1F* L1HFRingDrHist_;

      TH1F* L1ClosestL1ObjTypeHist_;
      TH1F* L1ClosestL1ObjDrHist_;

      TH2F* L1DrVsObjTypeHist_;

      // End of hists


      TFile* file_;

      std::vector<double> sampleNumbers_;
      std::vector<double> amplitudes_;

      TrackDetectorAssociator trackAssociator_;
      TrackAssociatorParameters trackParameters_;

      double minTrackPt_;
      std::string fileName_;
      double contCorr_;
      double energyCut_;

      HSCPEcalTreeContent myTreeVariables_;
      TTree* myTree_;
      
};

