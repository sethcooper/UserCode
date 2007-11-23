// -*- C++ -*-
//
// Package:   MIPDumper 
// Class:     MIPDumper 
// 
/**\class MIPDumper MIPDUmper.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER
//         Created:  Th Nov 22 5:46:22 CEST 2007
// $Id: MIPDumper.cc,v 1.1 2007/11/23 12:53:06 scooper Exp $
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
// class declaration
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
  edm::InputTag EBDigis_;
  int ievt_;
  int runNum_;
  double threshold_;
  std::string fileName_;

  std::set<EBDetId> listAllChannels;
    
  int side;

  int abscissa[10];
  int ordinate[10];
  
  std::vector<TGraph> graphs;
  std::vector<int> maskedChannels_;
  std::vector<int> maskedFEDs_;
  
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
  EBDigis_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
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
    
  //Handle<EcalRawDataCollection> DCCHeaders;
  //try {
  //  iEvent.getByLabel(headerProducer_, DCCHeaders);
  //} catch ( std::exception& ex ) {
  //  edm::LogError ("EcalPedOffset") << "Error! can't get the product " 
  //    << headerProducer_;
  //  return;
  //}

  ievt_++;
  int graphCount = 0;
  //We only want the 3x3's for this event...
  listAllChannels.clear();
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
      
      int ism = ebDet.ism();

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
          //listAllChannels.insert(EBDetId(ieta_c,iphi_c));
          EBDetId toInsert = EBDetId(ism, ic_c,1);
          //pair<set<EBDetId>::iterator,bool> retVal =
          listAllChannels.insert(toInsert);
          //if(retVal.second)
          //  cout << "Insertion of ic:" << ic_c << "successful." << endl;
          //else
          //  cout << "Insertion of ic:" << ic_c << "failed." << endl;
          //cout << "EBDet:" << toInsert << endl;
          //cout << "RetVal:" << *(retVal.first) << endl;
          
        }
      }
    }
  }

  // retrieving crystal data from Event
  edm::Handle<EBDigiCollection>  digis;
  iEvent.getByLabel(EBDigis_, digis);

  for(std::set<EBDetId>::const_iterator chnlItr = listAllChannels.begin(); chnlItr!= listAllChannels.end(); ++chnlItr)
  {
      //int ic     = EBDetId((*chnlItr).id()).ic();
      //int ieb    = EBDetId((*digiItr).id()).ism();
      //EBDetId det = (*chnlItr).id();
      //find digi we need  -- can't get find() to work; need DataFrame(DetId det) to work? 
      //TODO: use find()

    EBDigiCollection::const_iterator digiItr = digis->begin();
    while(digiItr != digis->end() && ((*digiItr).id()!=*chnlItr))
    {
      //EBDetId det = (*digiItr).id();
      
      //if(det==(*chnlItr))
      //  break;
      ++digiItr;
    }
    if(digiItr==digis->end())
      continue;

//EBDigiCollection::const_iterator digiItr;
//    for(EBDigiCollection::const_iterator digiItr = digis->begin();
//        digiItr!=digis->end(); ++digiItr)
//    {
//      if(EBDetId((*digiItr).id())==*chnlItr)
//        break;
//    }
//    
//    if(digiItr==digis->end())
//      continue;

    int ic = (*chnlItr).ic();
    int ism = (*chnlItr).ism();

    for (int i=0; i< (*digiItr).size() ; ++i ) {
      EBDataFrame df( *digiItr );
      ordinate[i] = df.sample(i).adc();
    }

    TGraph oneGraph(10, abscissa,ordinate);
    std::string title;
    title = "Graph_ev" + intToString( ievt_ ) + "_ic" + intToString( ic )+ "_iSM" + intToString(ism);
    oneGraph.SetTitle(title.c_str());
    oneGraph.SetName(title.c_str());
    graphs.push_back(oneGraph);
    graphCount++;
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
  file->cd();
  
  int graphCount = 0;
  std::vector<TGraph>::iterator gr_it;
  for (gr_it = graphs.begin(); gr_it !=  graphs.end(); gr_it++ )
  {
    graphCount++;
    if(graphCount % 1000 == 0)
      LogInfo("MIPDumper") << "Writing out graph " << graphCount << " of "
        << graphs.size(); 
    
    (*gr_it).Write();
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
