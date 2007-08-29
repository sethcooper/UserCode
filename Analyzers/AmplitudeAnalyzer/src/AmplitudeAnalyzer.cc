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
// $Id: AmplitudeAnalyzer.cc,v 1.18 2007/08/28 21:45:52 scooper Exp $
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

      TH1F* maxSampleHisto_;
      
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

  maxSampleHisto_ = new TH1F("max_sample_num","number of maximum sample",10,0,9);
  
  //total histos
  adcHisto_ = new TH1F("total_adc_counts","total adc counts",50,-25,25);
  recEhisto_ = new TH1F("total_rec_energy","total rec energy",50,-25,25);
  rawAdcVsRecAdc_ = new TH2F("total_raw_rec","total raw and rec adc",50,-25,25,50,-25,25);
  // fit only histos
  adcFitHisto_ = new TH1F("fit_adc","adc counts for fit",50,-25,25);
  recFitHisto_ = new TH1F("fit_recE","rec energy for fit",50,-25,25);
  rawAdcVsRecAdcFit_ = new TH2F("fit_raw_rec","raw and rec adc for fit",50,-25,25,50,-25,25);
  // amp5 only histos
  adcNoFitHisto_ = new TH1F("no_fit_adc","adc counts no fit",50,-25,25);
  recNoFitHisto_ = new TH1F("no_fit_recE","rec energy no fit",50,-25,25);
  rawAdcVsRecAdcNoFit_ = new TH2F("no_fit_raw_rec","raw and rec adc no fit",50,-25,25,50,-25,25);
}


AmplitudeAnalyzer::~AmplitudeAnalyzer()
{
  delete maxSampleHisto_;
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
  bool isFitted, isValid;
  Handle<EBDigiCollection> digis;
  e.getByLabel(EBDigiCollection_, digis);
  Handle<EcalUncalibratedRecHitCollection> hits;
  e.getByLabel(EcalUncalibratedRecHitCollection_, hits);

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr )
  {
    isValid = true;
    isFitted = false;
    EBDataFrame dataFrame = (*digiItr);
    EBDetId id = dataFrame.id();
    double pedestal = (double(dataFrame.sample(0).adc()) + double(dataFrame.sample(1).adc()))/2.;
    double frame[10];
    EcalMGPASample sample;
    double maxsample = -1;
    int imax = -1;
    for(int iSample=0; iSample<10; ++iSample)
    { 
      //create frame in adc gain 12 equivalent
      int GainId = dataFrame.sample(iSample).gainId();

      if(GainId==1){
        frame[iSample] = double(dataFrame.sample(iSample).adc())-pedestal;
      }
      else {
        //frame[iSample] = (double(dataFrame.sample(iSample).adc())-pedestals[GainId-1])*gainRatios[GainId-1];
        std::cout << "Oops!  No gainRatios, can't deal with this!" << std::endl;
        isValid  = false;
        break;
      }
      //if (GainId == 0 ) GainId = 3;//Fix ME this should be before frame construction for saturated channels
      //if (GainId != gainId0) iGainSwitch = 1;
      if( frame[iSample]>maxsample ) {
        maxsample = frame[iSample];
        imax = iSample;
      }
    }
    //sample = dataFrame.sample(imax);
    //int adc = sample.adc();
    //double pedestal = (double(dataframe.sample(0).adc()) + double(dataframe.sample(1).adc()))/2.;
    
    maxSampleHisto_->Fill(imax);
    
    if(!isValid) break;
    float amp = (float) maxsample;
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
    
//  }
}


// ------------ method called once each job just before starting event loop  ------------
void AmplitudeAnalyzer::beginJob(const EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
AmplitudeAnalyzer::endJob() {
  TFile a("test.root","RECREATE");
  
  maxSampleHisto_->Write();
  
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
