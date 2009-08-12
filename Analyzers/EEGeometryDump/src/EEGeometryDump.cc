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
#include "Geometry/Records/interface/CaloGeometryRecord.h"
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
  
  ESHandle < CaloGeometry > geoHandle;
  iSetup.get < CaloGeometryRecord > ().get(geoHandle);
  const CaloSubdetectorGeometry *geometry_p =
      geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);

  const CaloSubdetectorGeometry *geometry_pp =
    geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);

  EBDetId myId1  = EBDetId(-15,68);
  //EBDetId myId2  = EBDetId(19,20, 1);

  const CaloCellGeometry *cell_p = geometry_pp->getGeometry(myId1);
  const CaloCellGeometry::CornersVec& cellCorners (cell_p->getCorners());
  for(int i=0; i<7; ++i)
    std::cout << "Corner: " << i << " eta: " << cellCorners[i].eta() << " phi: " 
      << cellCorners[i].phi() << std::endl;

  //for (int ieta = -85; ieta < 86; ++ieta)
  //{
  //  float etaAvgZ = 0;
  //  int numCrys = 0;
  //  for (int iphi = 1; iphi < 361; ++iphi)
  //  {
  //    if (EBDetId::validDetId(ieta,iphi))
  //    {
  //      EBDetId det = EBDetId(ieta,iphi);
  //      const CaloCellGeometry *cell_p = geometry_pp->getGeometry(det);
  //      //Get position of center of crystal
  //      GlobalPoint p = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);
  //      etaAvgZ+=p.z();
  //      numCrys++;
  //      cout << "ieta: " << ieta << " iphi: " << iphi << " eta,phi: " << p.eta() << "," <<
  //        p.phi() << endl;
  //    }
  //  }
  //  etaAvgZ/=numCrys;
  //  //cout << etaAvgZ << "," << endl;
  //}
}

// ------------ method called once each job just after ending the event loop  ------------
void EEGeometryDump::endJob()
{
}

DEFINE_FWK_MODULE(EEGeometryDump);
