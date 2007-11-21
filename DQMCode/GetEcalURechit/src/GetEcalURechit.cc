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
// $Id: GetEcalURechit.cc,v 1.1 2007/11/20 09:56:50 scooper Exp $
//
//


// system include files
#include <memory>
#include <vector>
#include <map>
#include <set>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"

#include "TFile.h"
#include "TH1F.h"



//
// class decleration
//

class GetEcalURechit : public edm::EDAnalyzer {
   public:
      explicit GetEcalURechit(const edm::ParameterSet&);
      ~GetEcalURechit();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::string intToString(int num);
  
    // ----------member data ---------------------------

  edm::InputTag EcalUncalibratedRecHitCollection_;
  int ievt_;
  int runNum_;
  std::string fileName_;
  std::string headerProducer_;

    
  std::vector<int> maskedChannels_;
  std::vector<int> maskedFEDs_;
  
  std::map<int,TH1F*> FEDHistMap_;
  
  std::set<int> theRealFedSet_;

  TFile* file;

};

using namespace cms;
using namespace edm;
using namespace std;

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
GetEcalURechit::GetEcalURechit(const edm::ParameterSet& iConfig) :
  EcalUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection")),
  headerProducer_ (iConfig.getParameter<std::string>("headerProducer")),
  fileName_ (iConfig.getUntrackedParameter<std::string>("fileName", std::string("getURecHit"))),
  runNum_(-1)
{
  maskedChannels_.push_back(-1);
  maskedFEDs_.push_back(-1);
  
  maskedChannels_ = (iConfig.getUntrackedParameter<vector<int> >("maskedChannels", maskedChannels_)),
  maskedFEDs_ = (iConfig.getUntrackedParameter<vector<int> >("maskedFEDs", maskedFEDs_)),
  ievt_ =0;
  vector<int>::iterator result;
  
  // initialize map of FEDid and histogram
  for(int i=601; i<655; ++i)
  {
    vector<int>::const_iterator result = find(maskedFEDs_.begin(), maskedFEDs_.end(), i);
    if(result!=maskedFEDs_.end())
      continue;
    string title = "UncalibRecHitsAmplitude_all_crys_FED_"+intToString(i);
    string name = "UCalibRecHitsAmpli_FED_"+intToString(i);
    FEDHistMap_.insert(make_pair(i,new TH1F (name.c_str(),title.c_str(),4096,0,4095)));
  }
  
}


GetEcalURechit::~GetEcalURechit()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
GetEcalURechit::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    
  edm::Handle<EcalRawDataCollection> DCCHeaders;
  try {
    iEvent.getByLabel (headerProducer_, DCCHeaders);
  } catch ( std::exception& ex ) {
    edm::LogError ("EcalPedOffset") << "Error! can't get the product " 
      << headerProducer_.c_str();
    return;
  }

  for (EcalRawDataCollection::const_iterator headerItr= DCCHeaders->begin();
      headerItr != DCCHeaders->end (); 
      ++headerItr) 
  {
    int FEDid = 600+headerItr->id();
    theRealFedSet_.insert(FEDid);
  }

  ievt_++;
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);
  Handle<EcalUncalibratedRecHitCollection> hits;

  try
  {
    iEvent.getByLabel(EcalUncalibratedRecHitCollection_, hits);
    int neh = hits->size();
    LogDebug("GetEcalURechit") << "event " << ievt_ << " hits collection size " << neh;
  }
  catch ( exception& ex)
  {
    LogWarning("GetEcalURechit") << EcalUncalibratedRecHitCollection_ << " not available";
  }

  for (EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr)
  {
    EcalUncalibratedRecHit hit = (*hitItr);
    int ic = 0;
    //EEDetId eeDet;
    EBDetId ebDet;
    EcalElectronicsId elecId;
    //cout << "Subdetector field is: " << hit.id().subdetId() << endl;
    ebDet = hit.id();
    //TODO: make it work for endcap FEDs also
    //if(ebDet.isValid())
    //{
      ic = ebDet.hashedIndex();
      elecId = ecalElectronicsMap->getElectronicsId(ebDet);
    //}
    //else
    //{
    //  eeDet = hit.id();
    //  if(!eeDet.isValid())
    //  {
    //    LogWarning("GetEcalURechit") << "Unable to find hashedIndex for hit in event " << ievt_;
    //    continue;
    //  }
    //  else
    //  {
    //    ic = eeDet.hashedIndex();
    //    elecId = ecalElectronicsMap->getElectronicsId(eeDet);
    //  }
    //}
    
    int FEDid = 600+elecId.dccId();

    float ampli = hit.amplitude();
    float jitter = hit.jitter();

    vector<int>::iterator result;
    result = find(maskedFEDs_.begin(), maskedFEDs_.end(), FEDid);
    if(result != maskedFEDs_.end())
    {
      LogWarning("GetEcalURechit") << "skipping uncalRecHit for FED " << FEDid << " ; amplitude " << ampli;
      return;
    }      

    // this is for ped runs only, to get gain12
    //if (ievt_ < 300) return;

    //LogWarning("GetEcalURechit") << " hit amplitude " << ampli;
    //LogWarning("GetEcalURechit") << " hit jitter " << jitter;

    result = find(maskedChannels_.begin(), maskedChannels_.end(), ic);    
    if  (result != maskedChannels_.end()) 
    {
      LogWarning("GetEcalURechit") << "skipping uncalRecHit for channel: " << ic << " with amplitude " << ampli ;
      continue; //TODO: return or continue? (changed to continue by SIC)
    }      

    if (ampli > 9 )
    { 
      LogWarning("GetEcalURechit") << "channel: " << ic << "  ampli: " << ampli << " jitter " << jitter
        << " iEvent: " << iEvent.id().event() << " crudeEvent: " <<    ievt_ << " FED: " << FEDid;
    }
    FEDHistMap_[FEDid]->Fill(ampli);
  }

  if(runNum_==-1)
    runNum_ = iEvent.id().run();
}


// ------------ method called once each job just before starting event loop  ------------
void 
GetEcalURechit::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
GetEcalURechit::endJob()
{
  string filename = fileName_+intToString(runNum_)+".root";
  file = TFile::Open(filename.c_str(),"RECREATE");
  
  for(set<int>::const_iterator FEDitr = theRealFedSet_.begin(); FEDitr != theRealFedSet_.end(); ++FEDitr)
  {
    TH1F* hist = 0;
    vector<int>::iterator result;
    result = find(maskedFEDs_.begin(), maskedFEDs_.end(), *FEDitr);
    if(result!=maskedFEDs_.end())
      continue;
    else
    {
      cout << "DEBUG, FED: " << *FEDitr << endl;
      hist = FEDHistMap_[*FEDitr];
      if(hist!=0)
        hist->Write();
    }
  }

  file->Close();
}


std::string GetEcalURechit::intToString(int num)
{
    using namespace std;
    ostringstream myStream; //creates an ostringstream object
    myStream << num << flush;
    return(myStream.str()); //returns the string form of the stringstream object
}

//define this as a plug-in
DEFINE_FWK_MODULE(GetEcalURechit);
