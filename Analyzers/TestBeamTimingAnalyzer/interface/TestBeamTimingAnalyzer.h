#ifndef TESTBEAMTIMINGANALYZER_H
#define TESTBEAMTIMINGANALYZER_H

/**\class TestBeamTimingAnalyzer

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// $Id: TestBeamTimingAnalyzer.h,v 1.6 2008/12/19 17:47:43 scooper Exp $
//



// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"

#include <string>
//#include "TTree.h"
#include "TH1.h"
#include "TGraph.h"
#include "TH2.h"
#include<fstream>
#include<map>
//#include<stl_pair>



class TestBeamTimingAnalyzer : public edm::EDAnalyzer {
   public:
      explicit TestBeamTimingAnalyzer( const edm::ParameterSet& );
      ~TestBeamTimingAnalyzer();


      virtual void analyze( const edm::Event&, const edm::EventSetup& );
      virtual void beginJob(const edm::EventSetup&);
      virtual void endJob();
      std::string doubleToString(double num);
      std::string intToString(int num);
   
   private:

      
      std::string rootfile_;
      std::string digiCollection_;
      std::string digiProducer_;
      std::string hitCollection_;
      std::string hitProducer_;
      std::string hodoRecInfoCollection_;
      std::string hodoRecInfoProducer_;
      std::string tdcRecInfoCollection_;
      std::string tdcRecInfoProducer_;
      std::string eventHeaderCollection_;
      std::string eventHeaderProducer_;

      // Amplitude vs TDC offset
      TH2F* h_ampltdc;

      TH1F* TDCValue_;
      TH1F* recoTime_;
      TH1F* deltaTime_;
      TH2F* fitTimeVsTDC_[50];
      TH2F* deltaTvsTDC_[50];
      TH1F* deltaBetCrys_[50];
      TH1F* pivotCry_[50];
      TH2F* distVsTimeHists_[50];
      TH2F* cryIndicesConsideredHists_[50];
      TH2F* deltaTvsAverageT_[50];
      TH1F* deltaTCry2EnergyBin_[50];
      TH1F* deltaTCry7EnergyBin_[50];
      TH1F* recoMinusTDCCry7EnergyBin_[50];
      TH1F* recoMinusTDCCry2EnergyBin_[50];
      std::map<int,TH1F> recoTimeMinusTDCTimeByCry_;
      //TH1F* recoTimeMinusTDCTimeByCry_[25];
      
      TH2F* timingHistMap[25]; 
      TH1F* amplitudeHistMap[25];
      TH1F* timing1DHistMap[25];

      TH1F* deltaT813Bin8_;
      TH1F* deltaT813Bin6_;
      TH1F* deltaT713Bin12_;
      TH1F* deltaT713Bin11_;
      
      TH2F* h_Shape_;
      
      // Reconstructed energies
      TH1F* h_tableIsMoving;
      TH1F* h_e1x1;
      TH1F* h_e3x3; 
      TH1F* h_e5x5; 

      TH1F* h_e1x1_center;
      TH1F* h_e3x3_center; 
      TH1F* h_e5x5_center; 
      
      TH1F* h_e1e9;
      TH1F* h_e1e25;
      TH1F* h_e9e25;

      TH1F* h_bprofx; 
      TH1F* h_bprofy; 
      
      TH1F* h_qualx; 
      TH1F* h_qualy; 
      
      TH1F* h_slopex; 
      TH1F* h_slopey; 
      
      TH2F* h_mapx[25]; 
      TH2F* h_mapy[25]; 

      TH2F* h_e1e9_mapx;
      TH2F* h_e1e9_mapy;

      TH2F* h_e1e25_mapx;
      TH2F* h_e1e25_mapy;

      TH2F* h_e9e25_mapx;
      TH2F* h_e9e25_mapy;

      EBDetId xtalInBeam_;
      TH2F* occupancyOfMaxEneCry_;
      TH2F* distVsTDCHist_;

      const EcalElectronicsMapping* ecalElectronicsMap_;
      const CaloSubdetectorGeometry* geometry_barrel_; 
};



#endif
