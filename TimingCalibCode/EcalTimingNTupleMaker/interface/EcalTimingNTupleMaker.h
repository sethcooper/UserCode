#ifndef EcalTimingNTupleMaker_H
#define EcalTimingNTupleMaker_H
/**\class EcalTimingNTupleMaker

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>

*/

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDetId/interface/EcalTrigTowerDetId.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"

#include <string>
#include "TProfile.h"
#include "TProfile2D.h"

#include "TGraphErrors.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH3F.h"
#include "TTree.h"
#include "TFile.h"
#include <vector>

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"


//#include<fstream>
//#include<map>
//#include<stl_pair>

//

class EcalTimingNTupleMaker : public edm::EDAnalyzer {
   public:
      explicit EcalTimingNTupleMaker( const edm::ParameterSet& );
      ~EcalTimingNTupleMaker();


      virtual void analyze(  edm::Event const&,   edm::EventSetup const& );
      virtual void beginJob(edm::EventSetup const&);
      virtual void endJob();
	  double timecorr(const CaloSubdetectorGeometry *geometry_p, DetId id);
 private:
      
      std::string rootfile_;
      std::string hitCollection_;
      std::string hitCollectionEE_;
      std::string hitProducer_;
      std::string hitProducerEE_;
      std::string digiProducer_;

      edm::InputTag recHitCollection_;
      edm::InputTag recHitCollectionEE_;
      
      float ampl_thr_;
      int min_num_ev_;
      int sm_;
      std::string txtFileName_;
      std::string txtFileForChGroups_;
      //std::string pndiodeProducer_;
      std::vector<double> sMAves_;
      std::vector<double> sMCorr_;
      
      TProfile* amplProfileConv_[54][4];
      TProfile* absoluteTimingConv_[54][4];

      TProfile* amplProfileAll_[54][4];
      TProfile* absoluteTimingAll_[54][4];
      
      TProfile* Chi2ProfileConv_[54][4];
      
      TProfile* relativeTimingBlueConv_[54];

      TGraphErrors* ttTiming_[54];
      TGraphErrors* ttTimingAll_;
      TGraphErrors* ttTimingRel_[54];
      TGraphErrors* ttTimingAllRel_;
      TGraphErrors* ttTimingAllSMChng_;
      
      TGraph* lasershiftVsTime_[54];
      TH2F* lasershiftVsTimehist_[54];
      TH1F* lasershiftLM_[54];
      TH1F* lasershift_;
      
      TH3F* ttTimingEtaPhi_;
      TH3F* ttTimingEtaPhiRel_;
      TH3F* chTimingEtaPhi_;
      TH3F* chTimingEtaPhiRel_;
	    
      TProfile* ttTimingEta_;
      TProfile* ttTimingEtaRel_;
      TProfile* chTimingEta_;
      TProfile* chTimingEtaRel_;
	  
      TProfile* ttTimingEtaEEP_;
      TProfile* ttTimingEtaRelEEP_;
	  
      TProfile* ttTimingEtaEEM_;
      TProfile* ttTimingEtaRelEEM_;
	  
      TH3F* chTimingEtaPhiEEP_;
      TH3F* chTimingEtaPhiEEM_;
      TH3F* chTimingEtaPhiRelEEP_;
      TH3F* chTimingEtaPhiRelEEM_;
      
      TH3F* ttTimingEtaPhiEEP_;
      TH3F* ttTimingEtaPhiEEM_;
      TH3F* ttTimingEtaPhiRelEEP_;
      TH3F* ttTimingEtaPhiRelEEM_;
      
      TH1F* timeCry1[54]; 
      TH1F* timeCry2[54]; 
      TH1F* timeRelCry1[54]; 
      TH1F* timeRelCry2[54]; 
      
      TH1F* aveRelXtalTime_;
      TH1F* aveRelXtalTimebyDCC_[54];
      TH2F* aveRelXtalTimeVsAbsTime_;
      
      TProfile2D* fullAmpProfileEB_;
      TProfile2D* fullAmpProfileEEP_;
      TProfile2D* fullAmpProfileEEM_;
      
      double timerunstart_;
      double timerunlength_;
	  
      TH1F* lasersPerEvt;

      const EcalElectronicsMapping* ecalElectronicsMap_;
 
      int ievt_;
      int numGoodEvtsPerSM_[54];
	  
      static const int numXtals = 15480;
  
      //Allows for running the job on a file
      bool fromfile_;
      std::string fromfilename_;   
	  
	  //Correct for Timing 
      bool corrtimeEcal;
      bool corrtimeBH;
      bool bhplus_;
      double EBradius_;
	  
      double allave_;
	  
      bool writetxtfiles_;
	  
      bool correctAVE_;
      std::map<int, double> eta2zmap_;

      TTree* eventTimingInfoTree_;
      int cryHashesEB_[61200];
      int cryHashesEE_[14648];
      float cryTimesEB_[61200];
      float cryTimesEE_[14648];
      float cryTimeErrorsEB_[61200];
      float cryTimeErrorsEE_[14648];
      float cryAmpsEB_[61200];
      float cryAmpsEE_[14648];
      int numEBcrys_;
      int numEEcrys_;
      float correctionToSample5EB_;
      float correctionToSample5EEP_;
      float correctionToSample5EEM_;

      TFile* f_;
      
};



#endif
