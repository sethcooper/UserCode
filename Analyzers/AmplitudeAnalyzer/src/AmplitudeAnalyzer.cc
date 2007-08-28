// -*- C++ -*-
//
// Package:    AmplitudeAnalyzer
// Class:      AmplitudeAnalyzer
// 
/**\class AmplitudeAnalyzer AmplitudeAnalyzer.cc MyCode/AmplitudeAnalyzer/src/AmplitudeAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER ()
//         Created:  Wed Aug 22 18:57:08 CEST 2007
// $Id: AmplitudeAnalyzer.cc,v 1.13 2007/08/28 18:50:46 scooper Exp $
//
//


// system include files
#include <memory>
#include <string>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

//
// class decleration
//

class AmplitudeAnalyzer : public edm::EDAnalyzer {
   public:
      explicit AmplitudeAnalyzer(const edm::ParameterSet&);
      ~AmplitudeAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      edm::InputTag EBDigiCollection_;
      edm::InputTag  EcalRecHitCollection_;
      edm::InputTag EcalUncalibratedRecHitCollection_;

      TH1F* adcHisto_;
      TH1F* recEhisto_;
      TH2F* rawAdcVsRecAdc_;
      
      TH1F* adcFitHisto_;
      TH1F* recFitHisto_;
      TH2F* rawAdcVsRecAdcFit_;
      
      TH1F* adcNoFitHisto_;
      TH1F* recNoFitHisto_;
      TH2F* rawAdcVsRecAdcNoFit_;
        
      // ----------member data ---------------------------
};

using namespace std;
using namespace edm;
using namespace cms;

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
AmplitudeAnalyzer::AmplitudeAnalyzer(const ParameterSet& ps)
{
   //now do what ever initialization is needed

  EBDigiCollection_ = ps.getParameter<InputTag>("EBDigiCollection");
  EcalUncalibratedRecHitCollection_ = ps.getParameter<InputTag>("EcalUncalibratedRecHitCollection");
  
  //total histos
  adcHisto_ = new TH1F("adc counts","adc counts",50,-25,25);
  recEhisto_ = new TH1F("rec energy","rec energy",50,-25,25);
  rawAdcVsRecAdc_ = new TH2F("raw and rec adc","raw and rec adc",50,-25,25,50,-25,25);
  // fit only histos
  adcFitHisto_ = new TH1F("adc counts for fit","adc counts for fit",50,-25,25);
  recFitHisto_ = new TH1F("rec energy for fit","adc energy for fit",50,-25,25);
  rawAdcVsRecAdcFit_ = new TH2F("raw and rec adc for fit","raw and rec adc for fit",50,-25,25,50,-25,25);
  // amp5 only histos
  adcNoFitHisto_ = new TH1F("adc counts no fit","adc counts no fit",50,-25,25);
  recNoFitHisto_ = new TH1F("rec energy no fit","adc energy no fit",50,-25,25);
  rawAdcVsRecAdcNoFit_ = new TH2F("raw and rec adc no fit","raw and rec adc no fit",50,-25,25,50,-25,25);
}


AmplitudeAnalyzer::~AmplitudeAnalyzer()
{
  delete adcHisto_;
  delete recEhisto_;
  delete rawAdcVsRecAdc_;
  delete adcFitHisto_;
  delete recFitHisto_;
  delete rawAdcVsRecAdcFit_;
  delete adcNoFitHisto_;
  delete recNoFitHisto_;
  delete rawAdcVsRecAdcNoFit_;
}


//
// member functions
//

// ------------ method called to for each event  ------------
void AmplitudeAnalyzer::analyze(const Event& e, const EventSetup& iSetup)
{
  bool isFitted = false;
  Handle<EBDigiCollection> digis;
  e.getByLabel(EBDigiCollection_, digis);
  Handle<EcalUncalibratedRecHitCollection> hits;
  e.getByLabel(EcalUncalibratedRecHitCollection_, hits);

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

    EBDataFrame dataframe = (*digiItr);
    EBDetId id = dataframe.id();
    EcalMGPASample sample = dataframe.sample(4);
    if(sample.gainId()==1)
    {
      int adc = sample.adc();
      double pedestal = (double(dataframe.sample(0).adc()) + double(dataframe.sample(1).adc()))/2.;
      float amp = (float) adc-pedestal;
      adcHisto_->Fill(amp);
      if(amp < 8) 
        adcNoFitHisto_->Fill(amp);
      else 
      {
        adcFitHisto_->Fill(amp);
        isFitted = true;
      }
      
      EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->find(id);
      EcalUncalibratedRecHit hit = (*hitItr);

      float R_ene = hit.amplitude();
      recEhisto_->Fill(R_ene);
      if(isFitted) recFitHisto_->Fill(R_ene);
      else recNoFitHisto_->Fill(R_ene);
      
      rawAdcVsRecAdc_->Fill(amp,R_ene);
      if(isFitted) rawAdcVsRecAdcFit_->Fill(amp,R_ene);
      else rawAdcVsRecAdcNoFit_->Fill(amp,R_ene);
    }
   // for (int i = 0; i < 10; i++) {
   //   EcalMGPASample sample = dataframe.sample(i);
   //   int adc = sample.adc();
   //   //if(adc<30 && adc>0)
   //   //  cout << "ADC: " << adc << endl;
   //   //int gainid = sample.gainId();
   // }
  
  
//  for ( EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr ) {
//    EcalUncalibratedRecHit hit = (*hitItr);
//    EBDetId id = hit.id();
//    //float chi2    = hit.chi2();
//    //float jiitter   = hit.jitter();
//    //cout << "Rec energy: " << R_ene << endl;
    
  }
}


// ------------ method called once each job just before starting event loop  ------------
void AmplitudeAnalyzer::beginJob(const EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
AmplitudeAnalyzer::endJob() {
  TFile a("test.root","RECREATE");
  recEhisto_->Write();
  adcHisto_->Write();
  rawAdcVsRecAdc_->Write();
  
  recFitHisto_->Write();
  adcFitHisto_->Write();
  rawAdcVsRecAdcFit_->Write();
  
  recNoFitHisto_->Write();
  adcNoFitHisto_->Write();
  rawAdcVsRecAdcNoFit_->Write();
  a.Write();
  a.Close();
}

//define this as a plug-in
DEFINE_FWK_MODULE(AmplitudeAnalyzer);
