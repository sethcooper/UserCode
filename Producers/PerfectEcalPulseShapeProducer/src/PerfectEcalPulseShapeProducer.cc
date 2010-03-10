// -*- C++ -*-
//
// Package:    PerfectEcalPulseShapeProducer
// Class:      PerfectEcalPulseShapeProducer
// 
/**\class PerfectEcalPulseShapeProducer PerfectEcalPulseShapeProducer.cc Producers/PerfectEcalPulseShapeProducer/src/PerfectEcalPulseShapeProducer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  
//         Created:  Tue Oct 13 13:57:12 CEST 2009
// $Id: PerfectEcalPulseShapeProducer.cc,v 1.1 2010/03/10 08:20:28 scooper Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EcalMGPASample.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"

//
// class decleration
//

class PerfectEcalPulseShapeProducer : public edm::EDProducer {
   public:
      explicit PerfectEcalPulseShapeProducer(const edm::ParameterSet&);
      ~PerfectEcalPulseShapeProducer();

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      void makeEBDataFrame(EBDataFrame* df, double amplitude);
      void makeEEDataFrame(EEDataFrame* df, double amplitude);
      
      // ----------member data ---------------------------
      double amplitudeEB_;
      double amplitudeEE_;

      bool scanAmplitudes_;
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
PerfectEcalPulseShapeProducer::PerfectEcalPulseShapeProducer(const edm::ParameterSet& iConfig) :
  amplitudeEB_ (iConfig.getUntrackedParameter<double>("amplitudeEB",500)),
  amplitudeEE_ (iConfig.getUntrackedParameter<double>("amplitudeEE",500)),
  scanAmplitudes_ (iConfig.getUntrackedParameter<bool>("scanAmplitudes",false))
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
*/
  produces<EBDigiCollection>("ebDigis");
  produces<EEDigiCollection>("eeDigis");
   //now do what ever other initialization is needed
  
}


PerfectEcalPulseShapeProducer::~PerfectEcalPulseShapeProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
PerfectEcalPulseShapeProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   std::auto_ptr<EBDigiCollection> productDigisEB(new EBDigiCollection);
   productDigisEB->reserve(1000);
   std::auto_ptr<EEDigiCollection> productDigisEE(new EEDigiCollection);
   productDigisEE->reserve(1000);

   // So the max on sample 5 should have amplitude=amplitudeEB_
   // Use pedestal = 200 ADC counts
   // EB
   for(int i=0; i<2000;++i)
   {
     EBDetId det = EBDetId::unhashIndex(i);
     if(det==EBDetId()) // Check for valid EBDetId
       continue;

     productDigisEB->push_back(*(&det));
     EBDataFrame* df = new EBDataFrame(productDigisEB->back());
     if(!scanAmplitudes_)
       makeEBDataFrame(df,amplitudeEB_);
     else
       makeEBDataFrame(df,amplitudeEB_+i);
   }
   
   productDigisEB->sort();
   iEvent.put(productDigisEB,"ebDigis");
 

   // EE
   for(int i=0; i<2000;++i)
   {
     EEDetId det = EEDetId::unhashIndex(i);
     if(det==EEDetId()) // Check for valid EEDetId
       continue;

     productDigisEE->push_back(*(&det));
     EEDataFrame* df = new EEDataFrame(productDigisEE->back());
     if(!scanAmplitudes_)
       makeEEDataFrame(df,amplitudeEE_);
     else
       makeEEDataFrame(df,amplitudeEE_+i);
   }
   
   productDigisEE->sort();
   iEvent.put(productDigisEE,"eeDigis");
}

// ------------ make EBDataFrame from ideal pulse shape with given amplitude  -----------
void PerfectEcalPulseShapeProducer::makeEBDataFrame(EBDataFrame* df, double amplitude)
{
  int adc = (int) 200+2.95346e-06*amplitude; // bin 10
  EcalMGPASample sample0(adc,1); // gainId==1 --> gain12
  df->setSample(0,sample0);

  adc = (int) 200+4.31832e-05*amplitude; // bin 35
  EcalMGPASample sample1(adc,1); // gainId==1 --> gain12
  df->setSample(1,sample1);

  adc = (int) 200-8.29046e-06*amplitude; // bin 60
  EcalMGPASample sample2(adc,1); // gainId==1 --> gain12
  df->setSample(2,sample2);

  adc = (int) 200+0.0149157*amplitude; // bin 85
  EcalMGPASample sample3(adc,1); // gainId==1 --> gain12
  df->setSample(3,sample3);

  adc = (int) 200+0.774163*amplitude; // bin 110
  EcalMGPASample sample4(adc,1); // gainId==1 --> gain12
  df->setSample(4,sample4);

  adc = (int) 200+1*amplitude; // bin 135 of EBPulseShape
  EcalMGPASample sample5(adc,1); // gainId==1 --> gain12
  df->setSample(5,sample5);

  adc = (int) 200+0.882056*amplitude; // bin 160
  EcalMGPASample sample6(adc,1); // gainId==1 --> gain12
  df->setSample(6,sample6);

  adc = (int) 200+0.667643*amplitude; // bin 185
  EcalMGPASample sample7(adc,1); // gainId==1 --> gain12
  df->setSample(7,sample7);

  adc = (int) 200+0.469623*amplitude; // bin 210
  EcalMGPASample sample8(adc,1); // gainId==1 --> gain12
  df->setSample(8,sample8);

  adc = (int) 200+0.316079*amplitude; 
  EcalMGPASample sample9(adc,1); // gainId==1 --> gain12
  df->setSample(9,sample9);
}

// ------------ make EEDataFrame from ideal pulse shape with given amplitude  -----------
void PerfectEcalPulseShapeProducer::makeEEDataFrame(EEDataFrame* df, double amplitude)
{
  int adc = (int) 200+0.000153345449*amplitude; // bin 23 normed
  EcalMGPASample sample0(adc,1); // gainId==1 --> gain12
  df->setSample(0,sample0);

  adc = (int) 200+8.15346302e-05*amplitude; // bin 48 normed
  EcalMGPASample sample1(adc,1); // gainId==1 --> gain12
  df->setSample(1,sample1);

  adc = (int) 200-6.20859935e-05*amplitude; // bin 73 normed
  EcalMGPASample sample2(adc,1); // gainId==1 --> gain12
  df->setSample(2,sample2);

  adc = (int) 200+0.11723996*amplitude; // bin 98 normed
  EcalMGPASample sample3(adc,1); // gainId==1 --> gain12
  df->setSample(3,sample3);

  adc = (int) 200+0.75718243*amplitude; // bin 123 normed
  EcalMGPASample sample4(adc,1); // gainId==1 --> gain12
  df->setSample(4,sample4);

  adc = (int) 200+1*amplitude; // bin 148 of EEPulseShape normed
  EcalMGPASample sample5(adc,1); // gainId==1 --> gain12
  df->setSample(5,sample5);

  adc = (int) 200+0.896619634*amplitude; // bin 173 normed
  EcalMGPASample sample6(adc,1); // gainId==1 --> gain12
  df->setSample(6,sample6);

  adc = (int) 200+0.686076247*amplitude; // bin 198 normed
  EcalMGPASample sample7(adc,1); // gainId==1 --> gain12
  df->setSample(7,sample7);

  adc = (int) 200+0.490747601*amplitude; // bin 223 normed
  EcalMGPASample sample8(adc,1); // gainId==1 --> gain12
  df->setSample(8,sample8);

  adc = (int) 200+0.343317363*amplitude; // bin 248 normed
  EcalMGPASample sample9(adc,1); // gainId==1 --> gain12
  df->setSample(9,sample9);
}

// ------------ method called once each job just before starting event loop  ------------
void 
PerfectEcalPulseShapeProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
PerfectEcalPulseShapeProducer::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(PerfectEcalPulseShapeProducer);
