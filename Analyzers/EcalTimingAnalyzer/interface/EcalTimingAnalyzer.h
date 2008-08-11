// -*- C++ -*-
//
// Package:   EcalTimingAnalyzer 
// Class:     EcalTimingAnalyzer 
// 
/**\class EcalTimingAnalyzer EcalTimingAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER
//         Created:  Th Nov 22 5:46:22 CEST 2007
// $Id: EcalTimingAnalyzer.h,v 1.9 2008/07/31 09:31:41 torimoto Exp $
//
//


// system include files
#include <memory>
#include <vector>
#include <map>
#include <set>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GtPsbWord.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"

#include "CaloOnlineTools/EcalTools/interface/EcalFedMap.h"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TGraph.h"
#include "TNtuple.h"


// *** for TrackAssociation
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/Handle.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
// ***

//
// class declaration
//

class EcalTimingAnalyzer : public edm::EDAnalyzer {
   public:
      explicit EcalTimingAnalyzer(const edm::ParameterSet&);
      ~EcalTimingAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::string intToString(int num);
      void initHists(int);

  // ----------member data ---------------------------
  
  edm::InputTag ecalRecHitCollection_;
  edm::InputTag barrelClusterCollection_;
  edm::InputTag endcapClusterCollection_;
  edm::InputTag l1GTReadoutRecTag_;
  edm::InputTag l1GMTReadoutRecTag_;
  
  int runNum_;
  double histRangeMax_, histRangeMin_;
  double minTimingAmp_;
  double minHighEnergy_;
  
  double *ttEtaBins;
  double *modEtaBins;
  std::string fileName_;
  bool runInFileName_;

  double startTime_, runTimeLength_;
  int numTimingBins_; 
  
  TH1F* allFedsHist_;
  TH1F* allFedsenergyHist_;
  TH1F* allFedsTimingHist_;
  TH1F* allFedsFrequencyHist_;
  TH1F* allFedsiPhiProfileHist_;
  TH1F* allFedsiEtaProfileHist_;
  TH1F* numberofCosmicsHist_;
  TH1F* numberofCosmicsWTrackHist_;
  TH1F* numberofGoodEvtFreq_;

  TH2F* allOccupancy_; //New file to do eta-phi occupancy
  TH2F* TrueOccupancy_; //New file to do eta-phi occupancy
  TH2F* allOccupancyCoarse_; //New file to do eta-phi occupancy
  TH2F* TrueOccupancyCoarse_; //New file to do eta-phi occupancy

  TH2F* allFedsTimingVsFreqHist_;
  TH2F* allFedsTimingVsAmpHist_;
  TH2F* allFedsTimingPhiHist_;
  TH2F* allFedsTimingEtaHist_;
  TH2F* allFedsTimingPhiEbpHist_;
  TH2F* allFedsTimingPhiEbmHist_;
  TH3F* allFedsTimingPhiEtaHist_;
  TH3F* allFedsTimingTTHist_;
  TH2F* allFedsTimingLMHist_;

  TH1F* allFedsTimingEbpHist_;
  TH1F* allFedsTimingEbmHist_;
  TH1F* allFedsTimingEbpTopHist_;
  TH1F* allFedsTimingEbmTopHist_;
  TH1F* allFedsTimingEbpBottomHist_;
  TH1F* allFedsTimingEbmBottomHist_;

  TH2F* allOccupancyECAL_; 
  TH2F* allOccupancyCoarseECAL_; 
  TH2F* allOccupancyExclusiveECAL_; 
  TH2F* allOccupancyCoarseExclusiveECAL_; 
  TH1F* allFedsTimingHistECAL_;
  TH3F* allFedsTimingPhiEtaHistECAL_;
  TH3F* allFedsTimingTTHistECAL_;
  TH2F* allFedsTimingLMHistECAL_;

  TH1F* allFedsTimingHistHCAL_;
  TH1F* allFedsTimingHistCSC_;
  TH1F* allFedsTimingHistRPC_;
  TH1F* allFedsTimingHistDT_;
  TH1F* allFedsTimingHistEcalMuon_;
  
  TH1F* triggerHist_;
  TH1F* triggerExclusiveHist_;

  TH1F* runNumberHist_;
  TH1F* deltaRHist_;
  TH1F* deltaEtaHist_;
  TH1F* deltaPhiHist_;
  TH1F* ratioAssocTracksHist_;
  TH1F* ratioAssocClustersHist_;
  TH2F* deltaEtaDeltaPhiHist_;
  TH2F* seedTrackPhiHist_;
  TH2F* seedTrackEtaHist_;

  // track association
  TH2F* trackAssoc_muonsEcal_;
  TrackDetectorAssociator trackAssociator_;
  TrackAssociatorParameters trackParameters_;
  TH1F* numberofCrossedEcalIdsHist_;

  // top & bottom
  TH1F* numberofCosmicsTopBottomHist_;
  int cosmicCounterTopBottom_;

  EcalFedMap* fedMap_;

  TFile* file;
  
  int naiveEvtNum_; 
  int cosmicCounter_;

  std::vector<int> l1Accepts_;
  std::vector<std::string> l1Names_;

};
