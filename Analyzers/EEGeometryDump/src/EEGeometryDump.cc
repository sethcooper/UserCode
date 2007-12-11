//
// Package:    EEGeometryDump
// Class:      EEGeometryDump
// 
/**\class EEGeometryDump EEGeometryDump.cc GeometryDump/EEGeometryDump/src/EEGeometryDump.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Ted Ritchie Kolberg
//         Created:  Tue Jul 10 10:18:55 CEST 2007
//
// Modified by Seth I. Cooper, Mon Jul 16 4:14:00 CEST 2007


// system include files
#include <memory>
#include <vector>
#include <math.h>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/DetId/interface/DetId.h"

//
// class declaration
//

class EEGeometryDump:public edm::EDAnalyzer {
public:
  explicit EEGeometryDump(const edm::ParameterSet &);
  ~EEGeometryDump();


private:
  virtual void beginJob(const edm::EventSetup & iSetup);
  virtual void analyze(const edm::Event &, const edm::EventSetup &);
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
EEGeometryDump::EEGeometryDump(const edm::ParameterSet & iConfig)
{
  //now do what ever initialization is needed

  std::cout << "ctor" << std::endl;

}


EEGeometryDump::~EEGeometryDump()
{

  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)

}


// ------------ method called to for each event  ------------
void
 EEGeometryDump::analyze(const edm::Event & iEvent,
			 const edm::EventSetup & iSetup)
{
  using namespace edm;

}


// ------------ method called once each job just before starting event loop  ------------
void EEGeometryDump::beginJob(const edm::EventSetup & iSetup)
{
  using namespace std;
  using namespace edm;
  
  // make EE DetIDs for whole EE+

  EEDetId eeId;

  ESHandle < CaloGeometry > geoHandle;
  iSetup.get < IdealGeometryRecord > ().get(geoHandle);
  const CaloSubdetectorGeometry *geometry_p =
      geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);

  EBDetId myId1  = EBDetId(19, 1, 1);
  EBDetId myId2  = EBDetId(19,20, 1);

  const CaloSubdetectorGeometry *geometry_pp =
    geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);

  const CaloCellGeometry *cell1 = geometry_pp->getGeometry(myId1);
  GlobalPoint p1 = (dynamic_cast <const TruncatedPyramid *>  (cell1))->getPosition(0);
  cout <<  "myId1" << p1.eta() << '\t' << p1.phi() << endl;

  const CaloCellGeometry *cell2 = geometry_pp->getGeometry(myId2);
  GlobalPoint p2 = (dynamic_cast <const TruncatedPyramid *> (cell2))->getPosition(0);
  cout <<  "myId2" << p2.eta() << '\t' << p2.phi() << endl;

  for (int i = 86; i <= 95; i++) 
  {
    for (int j = 46; j <= 55; j++) 
    {
      if (eeId.validDetId(i, j, +1)) 
      {
        eeId = EEDetId(i, j, +1);
        const CaloCellGeometry *cell_p = geometry_p->getGeometry(eeId);

        //Get position of center of crystal
        GlobalPoint p = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
        
        int ieta, iphi;
       // if(p.eta() < 1.740)
       // {
          ieta = (int) ceil(p.eta()/0.087);
          iphi = (int) ceil(p.phi()*(180.0/3.141592)); //5.0);
      //  }
      //  else
      //  {
      //    ieta = (int) ceil(p.eta()/0.090);
      //    iphi = (int) ceil(p.phi()*(180.0/3.141592)/10.0);
      //  }
        cout << i << " " << j << " " << p.eta() << " "  << p.phi() << endl;
      }
    }
  }
}

// ------------ method called once each job just after ending the event loop  ------------
void EEGeometryDump::endJob()
{
}

DEFINE_FWK_MODULE(EEGeometryDump);
