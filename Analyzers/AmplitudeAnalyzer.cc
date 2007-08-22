
//
// Package:    AmplitudeAnalyzer
// Class:      AmplitudeAnalyzer
// 

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
//
//
// Original Author:  Seth I. Cooper
//         Created:  Wed Aug 22 18:08:35 CEST 2007
//


// system include files

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

using namespace std;
using namespace edm;

class AmplitudeAnalyzer:public EDAnalyzer {
public:
  explicit AmplitudeAnalyzer(const ParameterSet&);
  ~AmplitudeAnalyzer();


private:
  virtual void beginJob(const EventSetup& iSetup);
  virtual void analyze(const Event&, const EventSetup&);
  virtual void endJob();

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
AmplitudeAnalyzer::AmplitudeAnalyzer(const ParameterSet & iConfig)
{
  //now do what ever initialization is needed

}


AmplitudeAnalyzer::~AmplitudeAnalyzer()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


// ------------ method called to for each event  ------------
void AmplitudeAnalyzer::analyze(const Event& e, const EventSetup& iSetup)
{

}


// ------------ method called once each job just before starting event loop  ------------
void AmplitudeAnalyzer::beginJob(const EventSetup& iSetup)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void AmplitudeAnalyzer::endJob()
{
}

DEFINE_FWK_MODULE(AmplitudeAnalyzer);

