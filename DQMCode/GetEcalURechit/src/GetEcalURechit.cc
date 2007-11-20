// -*- C++ -*-
//
// Package:    GetEcalURechit
// Class:      GetEcalURechit
// 
/**\class GetEcalURechit GetEcalURechit.cc Tool/GetEcalURechit/src/GetEcalURechit.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Giovanni FRANZONI
//         Created:  Tue Aug 28 11:46:22 CEST 2007
// $Id$
//
//


// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"


#include "TFile.h"
#include "TH1F.h"



//
// class decleration
//

using namespace cms;
using namespace edm;
using namespace std;

class GetEcalURechit : public edm::EDAnalyzer {
   public:
      explicit GetEcalURechit(const edm::ParameterSet&);
      ~GetEcalURechit();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

  
    // ----------member data ---------------------------

  edm::InputTag EcalUncalibratedRecHitCollection_;
  int ievt_;

  std::vector<int> maskedList_;
  
  TH1F  * amplitude;
  TFile   * file;

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
GetEcalURechit::GetEcalURechit(const edm::ParameterSet& iConfig)
{
   //now do what ever initialization is needed

  EcalUncalibratedRecHitCollection_ = iConfig.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection");
  
  maskedList_ = iConfig.getUntrackedParameter<vector<int> >("maskedChannels", maskedList_);

  ievt_ =0;
  amplitude = new TH1F ("UncalibRechitsAmpli","UncalibRechitsAmpli",4096,0,4095);

}


GetEcalURechit::~GetEcalURechit()
{

  // file              = new TFile("out.root","RECREATE");
  file              = TFile::Open("out.root","RECREATE");
  file       ->cd();
  amplitude  -> Write();
  file            ->Close();

  //  delete amplitude;
  //  delete file;

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
GetEcalURechit::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
  
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif

   ievt_ ++;


   Handle<EcalUncalibratedRecHitCollection> hits;

   try {
     
     iEvent.getByLabel(EcalUncalibratedRecHitCollection_, hits);
     int neh = hits->size();
     LogDebug("GetEcalURechit") << "event " << ievt_ << " hits collection size " << neh;
     
   }
   
   catch ( exception& ex) {

    LogWarning("GetEcalURechit") << EcalUncalibratedRecHitCollection_ << " not available";

  }


    for ( EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr ) {

      EcalUncalibratedRecHit hit = (*hitItr);
      EBDetId id = hit.id();

      int ic = id.ic();
      int ie = (ic-1)/20 + 1;
      int ip = (ic-1)%20 + 1;
           
      // this is for ped runs only, to get gain12
      //if (ievt_ < 300) return;

      float ampli = hit.amplitude();
      float jitter = hit.jitter();

      //LogWarning("GetEcalURechit") << " hit amplitude " << ampli;
      //LogWarning("GetEcalURechit") << " hit jitter " << jitter;

      vector<int>::iterator result;
      result = find( maskedList_.begin(), maskedList_.end(), ic);    
      if  (result != maskedList_.end()) 
	{
	  LogWarning("GetEcalURechit") << "skipping uncalRecHit for channel: " << ic << " with amplitude " << ampli ;
	  return;
	}      
      
      if (ampli > 9 )

      	{ 
	  LogWarning("GetEcalURechit") << "channel: " << ic << "  ampli: " << ampli << " jitter " << jitter
				       << " iEvent: " << iEvent.id().event() << " crudeEvent: " <<    ievt_;
	}
      amplitude -> Fill(ampli);      
    }

}


// ------------ method called once each job just before starting event loop  ------------
void 
GetEcalURechit::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
GetEcalURechit::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(GetEcalURechit);
