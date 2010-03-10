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
// $Id$
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
      
      // ----------member data ---------------------------
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
PerfectEcalPulseShapeProducer::PerfectEcalPulseShapeProducer(const edm::ParameterSet& iConfig)
{
   //register your products
/* Examples
   produces<ExampleData2>();

   //if do put with a label
   produces<ExampleData2>("label");
*/
  produces<EBDigiCollection>("ebDigis");
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

   //Make the digis, set amplitude to 300 ADC for now --> about 11.4 GeV
   // So the max on sample 5 should have amplitude 500 ADC
   for(int i=1000; i<2001;++i)
   {
     EBDetId det = EBDetId::unhashIndex(i);
     if(det==EBDetId())
       continue;

     productDigisEB->push_back(*(&det));
     
     EBDataFrame* df = new EBDataFrame(productDigisEB->back());
     int adc = (int) 200+2.95346e-06*300; // bin 10
     EcalMGPASample sample0(adc,1); // gainId==1 --> gain12
     df->setSample(0,sample0);

     adc = (int) 200+4.31832e-05*300; // bin 35
     EcalMGPASample sample1(adc,1); // gainId==1 --> gain12
     df->setSample(1,sample1);

     adc = (int) 200-8.29046e-06*300; // bin 60
     EcalMGPASample sample2(adc,1); // gainId==1 --> gain12
     df->setSample(2,sample2);

     adc = (int) 200+0.0149157*300; // bin 85
     EcalMGPASample sample3(adc,1); // gainId==1 --> gain12
     df->setSample(3,sample3);

     adc = (int) 200+0.774163*300; // bin 110
     EcalMGPASample sample4(adc,1); // gainId==1 --> gain12
     df->setSample(4,sample4);

     adc = (int) 200+1*300; // bin 135 of EBPulseShape
     EcalMGPASample sample5(adc,1); // gainId==1 --> gain12
     df->setSample(5,sample5);

     adc = (int) 200+0.882056*300; // bin 160
     EcalMGPASample sample6(adc,1); // gainId==1 --> gain12
     df->setSample(6,sample6);

     adc = (int) 200+0.667643*300; // bin 185
     EcalMGPASample sample7(adc,1); // gainId==1 --> gain12
     df->setSample(7,sample7);

     adc = (int) 200+0.469623*300; // bin 210
     EcalMGPASample sample8(adc,1); // gainId==1 --> gain12
     df->setSample(8,sample8);

     adc = (int) 200+0.316079*300; 
     EcalMGPASample sample9(adc,1); // gainId==1 --> gain12
     df->setSample(9,sample9);

   }
   
   productDigisEB->sort();
   iEvent.put(productDigisEB,"ebDigis");
 
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
