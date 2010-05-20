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
// $Id: HSCPTimingAnalyzer.h,v 1.1 2010/05/19 14:54:43 scooper Exp $
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
// class decleration
//

class HSCPTimingAnalyzer : public edm::EDAnalyzer {
   public:
      explicit HSCPTimingAnalyzer(const edm::ParameterSet&);
      ~HSCPTimingAnalyzer();


   private:
      virtual void beginJob();
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::vector<SteppingHelixStateInfo> calcEcalDeposit(const FreeTrajectoryState* glbInnerState,
          const FreeTrajectoryState* glbOuterState,
          const FreeTrajectoryState* innInnerState,
          const FreeTrajectoryState* innOuterState,
          const TrackAssociatorParameters& parameters);
      std::vector<SteppingHelixStateInfo> calcDeposit(const FreeTrajectoryState* glbInnerState,
          const FreeTrajectoryState* glbOuterState,
          const FreeTrajectoryState* innInnerState,
          const FreeTrajectoryState* innOuterState,
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
      edm::ESHandle<DetIdAssociator> ecalDetIdAssociator_;
      edm::ESHandle<CaloGeometry> theCaloGeometry_;
      edm::ESHandle<GlobalTrackingGeometry> theTrackingGeometry_;
      edm::ESHandle<MagneticField> bField_;

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
      TH1F* numCrysIn5x5Hist_;
      TH1F* numCrysIn3x3Hist_;

      TH1F* timeError999EnergyHist_;

      TH1F* deDxHist_;
      TH1F* trackLengthPerCryHist_;
      TH1F* numMatchedCrysPerEventHist_;
      TH1F* deDxMaxEnergyCryHist_;
      TH1F* deDxMinEnergyCryHist_;
      TH1F* deDxTotalHist_;
      //XXX: separate into EE/EB; make more general
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
      TH2F* energy1VsE1OverE9Hist_;
      TH2F* energy9VsE1OverE9Hist_;
      
      TH1F* timePullDistEBHist_;
      TH1F* timePullDistEEHist_;

      TH2F* TsigTvsArmsEBHist_;
      TH2F* TsigTvsArmsEEHist_;
      
      //TProfile* recHitMaxEnergyShapeProfile_;
      TGraph* recHitMaxEnergyShapeGraph_;

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

