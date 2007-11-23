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
// $Id: GetEcalURechit.cc,v 1.4 2007/11/21 18:38:23 scooper Exp $
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
#include "TGraph.h"



//
// class decleration
//

class MIPDumper : public edm::EDAnalyzer {
   public:
      explicit MIPDumper(const edm::ParameterSet&);
      ~MIPDumper();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::string intToString(int num);
  
    // ----------member data ---------------------------

  edm::InputTag EcalUncalibratedRecHitCollection_;
  edm::InputTag headerProducer_;
  int ievt_;
  int runNum_;
  double threshold_;
  std::string fileName_;

  std::vector<int> listAllChannels;
    
  int side;

  int abscissa[10];
  int ordinate[10];
  
  std::vector<TGraph> graphs;
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
MIPDumper::MIPDumper(const edm::ParameterSet& iConfig) :
  EcalUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection")),
  headerProducer_ (iConfig.getParameter<edm::InputTag>("headerProducer")),
  runNum_(-1),
  threshold_ (iConfig.getUntrackedParameter<double>("amplitudeThreshold", 12.0)),
  fileName_ (iConfig.getUntrackedParameter<std::string>("fileName", std::string("mipDumper")))
{
  side                = iConfig.getUntrackedParameter< int >("side", 3);
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
    TH1F* hist = new TH1F (name.c_str(),title.c_str(),4096,0,4095);
    hist->SetDirectory(0);
    FEDHistMap_.insert(make_pair(i,hist));
  }
  
  for (int i=0; i<10; i++)        abscissa[i] = i;
}


MIPDumper::~MIPDumper()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
MIPDumper::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    
  Handle<EcalRawDataCollection> DCCHeaders;
  try {
    iEvent.getByLabel(headerProducer_, DCCHeaders);
  } catch ( std::exception& ex ) {
    edm::LogError ("EcalPedOffset") << "Error! can't get the product " 
      << headerProducer_;
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
    LogDebug("MIPDumper") << "event " << ievt_ << " hits collection size " << neh;
  }
  catch ( exception& ex)
  {
    LogWarning("MIPDumper") << EcalUncalibratedRecHitCollection_ << " not available";
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
      ic = ebDet.ic();
      elecId = ecalElectronicsMap->getElectronicsId(ebDet);
    //}
    //else
    //{
    //  eeDet = hit.id();
    //  if(!eeDet.isValid())
    //  {
    //    LogWarning("MIPDumper") << "Unable to find hashedIndex for hit in event " << ievt_;
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
      LogWarning("MIPDumper") << "skipping uncalRecHit for FED " << FEDid << " ; amplitude " << ampli;
      return;
    }      

    // this is for ped runs only, to get gain12
    //if (ievt_ < 300) return;

    //LogWarning("MIPDumper") << " hit amplitude " << ampli;
    //LogWarning("MIPDumper") << " hit jitter " << jitter;

    result = find(maskedChannels_.begin(), maskedChannels_.end(), ic);    
    if  (result != maskedChannels_.end()) 
    {
      LogWarning("MIPDumper") << "skipping uncalRecHit for channel: " << ic << " with amplitude " << ampli ;
      continue; //TODO: return or continue? (changed to continue by SIC)
    }      

    if (ampli > threshold_ )
    { 
      LogWarning("MIPDumper") << "channel: " << ic << "  ampli: " << ampli << " jitter " << jitter
        << " iEvent: " << iEvent.id().event() << " crudeEvent: " <<    ievt_ << " FED: " << FEDid;

      // retrieving crystal data from Event
      edm::Handle<EBDigiCollection>  digis;
      iEvent.getByLabel("ecalEBunpacker", "ebDigis", digis);

      int ieta = (ic-1)/20   +1;
      int iphi = (ic-1)%20 +1;

      int hside = (side/2);

      for (int u = (-hside) ; u<=hside; u++)
      {
        for (int v = (-hside) ; v<=hside; v++)
        {  
          int ieta_c = ieta + u;
          int iphi_c = iphi + v;

          if (ieta_c < 1 || 85 < ieta_c) continue;
          if (iphi_c < 1 || 20 < iphi_c)  continue;

          int ic_c = (ieta_c-1) *20 + iphi_c;
          listAllChannels.push_back ( ic_c ) ;
        }
      }

      //find digi we need

      for ( EBDigiCollection::const_iterator digiItr= digis->begin();digiItr != digis->end(); 
          ++digiItr ) {
        {

          int ic     = EBDetId((*digiItr).id()).ic();
          //int ieb    = EBDetId((*digiItr).id()).ism();
          EBDetId det2 = (*digiItr).id();
          EcalElectronicsId elecId2 = ecalElectronicsMap->getElectronicsId(det2);
          int FEDid2 = 600+elecId2.dccId();
          if (FEDid2 != FEDid) return;

          // selecting desired channels only
          std::vector<int>::iterator icIter;
          icIter     = find( listAllChannels.begin() , listAllChannels.end() , ic);
          if (icIter == listAllChannels.end()) { continue; }

          for (int i=0; i< (*digiItr).size() ; ++i ) {
            EBDataFrame df( *digiItr );
            ordinate[i] = df.sample(i).adc();
          }

          TGraph oneGraph(10, abscissa,ordinate);
          std::string title;
          title = "Graph_ev" + intToString( ievt_ ) + "_ic" + intToString( ic )+ "_FED" + intToString(FEDid);
          oneGraph.SetTitle(title.c_str());
          oneGraph.SetName(title.c_str());
          graphs.push_back(oneGraph);

        }
      }
    }
    FEDHistMap_[FEDid]->Fill(ampli);
  }

  if(runNum_==-1)
    runNum_ = iEvent.id().run();
}


// ------------ method called once each job just before starting event loop  ------------
void 
MIPDumper::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MIPDumper::endJob()
{
  string filename = fileName_+intToString(runNum_)+".root";
  file = TFile::Open(filename.c_str(),"RECREATE");
  
  std::vector<TGraph>::iterator gr_it;
  for ( gr_it = graphs.begin(); gr_it !=  graphs.end(); gr_it++ )      (*gr_it).Write();

  for(set<int>::const_iterator FEDitr = theRealFedSet_.begin(); FEDitr != theRealFedSet_.end(); ++FEDitr)
  {
    TH1F* hist = 0;
    vector<int>::iterator result;
    result = find(maskedFEDs_.begin(), maskedFEDs_.end(), *FEDitr);
    if(result!=maskedFEDs_.end())
      continue;
    else
    {
      hist = FEDHistMap_[*FEDitr];
      if(hist!=0)
        hist->Write();
    }
  }

  file->Close();
}


std::string MIPDumper::intToString(int num)
{
    using namespace std;
    ostringstream myStream; //creates an ostringstream object
    myStream << num << flush;
    return(myStream.str()); //returns the string form of the stringstream object
}

//define this as a plug-in
DEFINE_FWK_MODULE(MIPDumper);
