/**
 * Module which outputs a root file of ADC counts (all three gains)
 *   of user-selected channels (defaults to channel 1) for 
 *   user-selected samples (defaults to samples 1,2,3) for
 *   user-selected supermodules.
 * 
 * \author S. Cooper
 *
 */

#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include <DataFormats/EcalDigi/interface/EcalDigiCollections.h>
#include <DataFormats/EcalDetId/interface/EcalDetIdCollections.h>
#include <DataFormats/EcalDigi/interface/EcalTriggerPrimitiveDigi.h>
#include <DataFormats/EcalDigi/interface/EcalTriggerPrimitiveSample.h>
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"

#include <iostream>
#include <vector>
#include <set>
#include "TFile.h"
#include "TH1.h"
#include "TDirectory.h"

typedef std::map<std::string,TH1F*> stringHistMap;

class EcalPedHistDumperModule: public edm::EDAnalyzer
{ 
  public:
    EcalPedHistDumperModule(const edm::ParameterSet& ps);   
    ~EcalPedHistDumperModule();

  protected:
    void analyze(const edm::Event & e, const  edm::EventSetup& c);
    void beginJob(const edm::EventSetup& c);
    void endJob(void);

  private:
    std::string intToString(int num);
    void readEBdigis(edm::Handle<EBDigiCollection> digis);
    void readEEdigis(edm::Handle<EEDigiCollection> digis);
    void initHists(int FED);

    int runNum_;
    bool inputIsOk_;
    bool allFEDsSelected_;
    std::string fileName_;
    std::string barrelDigiCollection_;
    std::string endcapDigiCollection_;
    std::string digiProducer_;
    std::string headerProducer_;
    std::vector<int> listChannels_;
    std::vector<int> listSamples_;
    std::vector<int> listFEDs_;
    std::map<int,stringHistMap> FEDsAndHistMaps_;
    std::set<int> theRealFedSet_;
    TFile * root_file_;
};


EcalPedHistDumperModule::EcalPedHistDumperModule(const edm::ParameterSet& ps) :
  runNum_(-1),
  fileName_ (ps.getUntrackedParameter<std::string>("fileName", std::string("ecalPedDigiDump"))),
  barrelDigiCollection_ (ps.getParameter<std::string> ("EBdigiCollection")),
  endcapDigiCollection_ (ps.getParameter<std::string> ("EEdigiCollection")),
  digiProducer_ (ps.getParameter<std::string> ("digiProducer")),
  headerProducer_ (ps.getParameter<std::string> ("headerProducer"))
{
  using namespace std;

  //for(int i=601; i<655; ++i)
  //{
  //  listDefaults.push_back(i);
  //} 
  listFEDs_ = ps.getParameter<vector<int> >("listFEDs");
  
  // Apply no selection if -1 is passed
  if(listFEDs_[0]==-1)
  {
    allFEDsSelected_ = true;
    //debug
    //cerr << "no selection on FEDs!" << endl;
    //inputIsOk_=false;
    //return;
    //listFEDs_ = listDefaults;
  }
  else
  {
    allFEDsSelected_ = false;
    // Verify FED numbers are valid
    for (vector<int>::const_iterator intIter = listFEDs_.begin(); intIter != listFEDs_.end(); intIter++)
    {  
      if ( ((*intIter) < 601)||(654 < (*intIter)) )
      {  
        cout << "[EcalPedHistDumperModule] FED value: " << (*intIter) << " found in listFEDs. "
          << " Valid range is 601-654. Returning." << endl;
        inputIsOk_ = false;
        return;
      }
      else
        theRealFedSet_.insert(*intIter);
    }
  } 

  vector<int> listDefaults = vector<int>();
  listDefaults.clear();
  for(int i=1; i<1701; ++i)
  {
    listDefaults.push_back(i);
  }
  listChannels_ = ps.getUntrackedParameter<vector<int> >("listChannels", listDefaults);
  listDefaults.clear();
  // Get samples to plot (default to 1,2,3)
  listDefaults.push_back(0);
  listDefaults.push_back(1);
  listDefaults.push_back(2);
  listSamples_ = ps.getUntrackedParameter<vector<int> >("listSamples", listDefaults);
  
  inputIsOk_ = true;
  vector<int>::iterator intIter;
  
  // Verify crystal numbers are valid
  for (intIter = listChannels_.begin(); intIter != listChannels_.end(); ++intIter)
  {  
      if ( ((*intIter) < 1)||(1700 < (*intIter)) )       
      {  
	cout << "[EcalPedHistDumperModule] ic value: " << (*intIter) << " found in listChannels. "
		  << " Valid range is 1-1700. Returning." << endl;
	inputIsOk_ = false;
	return;
      }
  }
  // Verify sample numbers are valid
  for (intIter = listSamples_.begin(); intIter != listSamples_.end(); intIter++)
  {  
      if ( ((*intIter) < 1)||(10 < (*intIter)) )
      {  
	cout << "[EcalPedHistDumperModule] sample number: " << (*intIter) << " found in listSamples. "
		  << " Valid range is 1-10. Returning." << endl;
	inputIsOk_ = false;
	return;
      }
  }

}

EcalPedHistDumperModule::~EcalPedHistDumperModule() 
{
}

void EcalPedHistDumperModule::beginJob(const edm::EventSetup& c)
{
}

void EcalPedHistDumperModule::endJob(void)
{
  using namespace std;
  if(inputIsOk_)
  { 
    //debug
    //cout << "endJob:creating root file!" << endl;
    
    fileName_ += "_"+intToString(runNum_)+".graph.root";

    TFile root_file_(fileName_.c_str() , "RECREATE");
    //Loop over FEDs first
    for(set<int>::const_iterator FEDitr = theRealFedSet_.begin(); FEDitr != theRealFedSet_.end(); ++FEDitr)
    {
      string dir = intToString(*FEDitr);
      TDirectory* FEDdir = gDirectory->mkdir(dir.c_str());
      FEDdir->cd();
      //root_file_.mkdir(dir.c_str());
      //root_file_.cd(dir.c_str());
      map<string,TH1F*> mapHistos = FEDsAndHistMaps_[*FEDitr];
      
      //debug 
      //cout << "FED:" << *FEDitr << endl;
      //cout << "Size of histMap:" << mapHistos.size() << endl;
      // All mapHistos entries have size 15 since they are initialized above
      //if(mapHistos.size()==0)
      //{
      //  cout << "detected empty hist map for FED:" << *FEDitr <<"; deleting dir" << endl;
      //  FEDdir->rmdir(dir.c_str());
      //  continue;
      //}
      
      //Loop over channels; write histos and directory structure
      for (vector<int>::const_iterator itr = listChannels_.begin(); itr!=listChannels_.end(); itr++)
      {
        //debug
        //cout << "loop over channels" << endl;
        
        TH1F* hist = 0;
        string chnl = intToString(*itr);
        string name1 = "Cry";
        name1.append(chnl+"Gain1");
        string name2 = "Cry";
        name2.append(chnl+"Gain6");
        string name3 = "Cry";
        name3.append(chnl+"Gain12");
        hist = mapHistos[name1];
        // This is a sanity check only
        if(hist!=0)
        {
          string cryDirName = "Cry_"+chnl;
          TDirectory* cryDir = FEDdir->mkdir(cryDirName.c_str());
          hist->SetDirectory(cryDir);
          hist->Write();
          hist = mapHistos[name2];
          hist->SetDirectory(cryDir);
          hist->Write();
          hist = mapHistos[name3];
          hist->SetDirectory(cryDir);
          hist->Write();
          root_file_.cd(dir.c_str());
        }
        else
        {
          cerr << "EcalPedHistDumperModule: Error: This shouldn't happen!" << endl;
        }
      }
      root_file_.cd();
    }
    root_file_.Close();
  }
}

void EcalPedHistDumperModule::analyze(const edm::Event& e, const edm::EventSetup& c)
{
  using namespace std;
  using namespace edm;

  if (!inputIsOk_)
    return;
  
  // loop over the headers, this is to detect missing FEDs if all are selected
  if(allFEDsSelected_)
  {
    edm::Handle<EcalRawDataCollection> DCCHeaders;
    try {
      e.getByLabel (headerProducer_, DCCHeaders);
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
  }

  // loop over fed list and make sure that there are histo maps
  for(set<int>::const_iterator fedItr = theRealFedSet_.begin(); fedItr != theRealFedSet_.end(); ++fedItr)
  {
    if(FEDsAndHistMaps_.find(*fedItr)==FEDsAndHistMaps_.end())
      initHists(*fedItr);
  }
  
  //debug
  //cout << "analyze...input is ok? " << inputIsOk_ << endl;
  
  bool barrelDigisFound = true;
  bool endcapDigisFound = true;
  // get the barrel digis
  // (one digi for each crystal)
  // TODO; SIC: fix this behavior
  Handle<EBDigiCollection> barrelDigis;
  try {
    e.getByLabel (digiProducer_, barrelDigiCollection_, barrelDigis);
  } catch ( std::exception& ex ) 
  {
    edm::LogError ("EcalPedOffset") << "Error! can't get the product " 
      << barrelDigiCollection_.c_str();
    barrelDigisFound = false;
  }
  // get the endcap digis
  // (one digi for each crystal)
  // TODO; SIC: fix this behavior
  Handle<EEDigiCollection> endcapDigis;
  try {
    e.getByLabel (digiProducer_, endcapDigiCollection_, endcapDigis);
  } catch ( std::exception& ex ) 
  {
    edm::LogError ("EcalPedOffset") << "Error! can't get the product " 
      << endcapDigiCollection_.c_str();
    endcapDigisFound = false;
  }
  
  if(barrelDigisFound)
    readEBdigis(barrelDigis);
  if(endcapDigisFound)
    readEEdigis(endcapDigis);
  if(!barrelDigisFound && !endcapDigisFound)
    edm::LogError ("EcalPedOffset") << "No digis found in the event!";
  
  if(runNum_==-1)
    runNum_ = e.id().run();
}

// insert the 3-entry hist map into the map keyed by FED number
void EcalPedHistDumperModule::initHists(int FED)
{
  using namespace std;
  //using namespace edm;

  std::map<string,TH1F*> histMap;
  //debug
  //cout << "Initializing map for FED:" << *FEDitr << endl;
  for (vector<int>::const_iterator intIter = listChannels_.begin(); intIter != listChannels_.end(); ++intIter)
  { 
    //Put 3 histos (1 per gain) for the channel into the map
    string FEDid = intToString(FED);
    string chnl = intToString(*intIter);
    string title1 = "Gain1 ADC Counts for channel ";
    title1.append(chnl);
    string name1 = "Cry";
    name1.append(chnl+"Gain1");
    string title2 = "Gain6 ADC Counts for channel ";
    title2.append(chnl);
    string name2 = "Cry";
    name2.append(chnl+"Gain6");
    string title3 = "Gain12 ADC Counts for channel ";
    title3.append(chnl);
    string name3 = "Cry";
    name3.append(chnl+"Gain12");
    histMap.insert(make_pair(name1,new TH1F(name1.c_str(),title1.c_str(),75,175.0,250.0)));
    histMap[name1]->SetDirectory(0);
    histMap.insert(make_pair(name2,new TH1F(name2.c_str(),title2.c_str(),75,175.0,250.0)));
    histMap[name2]->SetDirectory(0);
    histMap.insert(make_pair(name3,new TH1F(name3.c_str(),title3.c_str(),75,175.0,250.0)));
    histMap[name3]->SetDirectory(0);
  }
  FEDsAndHistMaps_.insert(make_pair(FED,histMap));
}


void EcalPedHistDumperModule::readEBdigis(edm::Handle<EBDigiCollection> digis)
{
  using namespace std;
  using namespace edm;
  //debug
  //cout << "readEBdigis" << endl;
  
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);

  // Loop over digis
  for (EBDigiCollection::const_iterator digiItr= digis->begin();digiItr != digis->end(); ++digiItr )
  {
    EBDetId detId = EBDetId(digiItr->id());
    EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(detId);
    int FEDid = 600+elecId.dccId();
    int crystalId = detId.ic();

    //debug
    //cout << "FEDid:" << FEDid << " cryId:" << crystalId << endl;
    //cout << "FEDid:" << FEDid << endl;
    //Select desired supermodules only
    vector<int>::iterator icIter;
    icIter = find(listFEDs_.begin(), listFEDs_.end(), FEDid);
    if (icIter == listFEDs_.end())
      continue;

    // Select desired channels only
    icIter = find(listChannels_.begin(), listChannels_.end(), crystalId);
    if (icIter == listChannels_.end())
      continue;

    // Get the adc counts from the selected samples and fill the corresponding histogram
    // Must subtract 1 from user-given sample list (e.g., user's sample 1 -> sample 0)
    for (vector<int>::iterator itr = listSamples_.begin(); itr!=listSamples_.end(); itr++)
    {
      map<string,TH1F*> mapHistos = FEDsAndHistMaps_[FEDid];
      string chnl = intToString(crystalId);
      string name1 = "Cry";
      name1.append(chnl+"Gain1");
      string name2 = "Cry";
      name2.append(chnl+"Gain6");
      string name3 = "Cry";
      name3.append(chnl+"Gain12");
      TH1F* hist = 0;
      if((*digiItr).sample(*itr-1).gainId()==3)
        hist = mapHistos[name1];
      if((*digiItr).sample(*itr-1).gainId()==2)
        hist = mapHistos[name2];
      if((*digiItr).sample(*itr-1).gainId()==1)
        hist = mapHistos[name3];
      if(hist!=0)
        hist->Fill((*digiItr).sample(*itr-1).adc());
      else
        cerr << "EcalPedHistDumper: Error: This shouldn't happen!" << endl;
    }
  }
}


void EcalPedHistDumperModule::readEEdigis(edm::Handle<EEDigiCollection> digis)
{
  using namespace std;
  using namespace edm;
  //debug
  //cout << "readEEdigis" << endl;
  
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);

  // Loop over digis
  for (EEDigiCollection::const_iterator digiItr= digis->begin();digiItr != digis->end(); ++digiItr )
  {
    EEDetId detId = EEDetId(digiItr->id());
    EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(detId);
    int FEDid = 600+elecId.dccId();
    int crystalId = 10000*FEDid+100*elecId.towerId()+5*(elecId.stripId()-1)+elecId.xtalId();

    //Select desired FEDs only
    vector<int>::iterator icIter;
    icIter = find(listFEDs_.begin(), listFEDs_.end(), FEDid);
    if (icIter == listFEDs_.end())
      continue;

    // Select desired channels only
    icIter = find(listChannels_.begin(), listChannels_.end(), crystalId);
    if (icIter == listChannels_.end())
      continue;

    // Get the adc counts from the selected samples and fill the corresponding histogram
    // Must subtract 1 from user-given sample list (e.g., user's sample 1 -> sample 0)
    for (vector<int>::iterator itr = listSamples_.begin(); itr!=listSamples_.end(); itr++)
    {
      map<string,TH1F*> mapHistos = FEDsAndHistMaps_[FEDid];
      string chnl = intToString(crystalId);
      string name1 = "Cry";
      name1.append(chnl+"Gain1");
      string name2 = "Cry";
      name2.append(chnl+"Gain6");
      string name3 = "Cry";
      name3.append(chnl+"Gain12");
      TH1F* hist = 0;
      if((*digiItr).sample(*itr-1).gainId()==3)
        hist = mapHistos[name1];
      if((*digiItr).sample(*itr-1).gainId()==2)
        hist = mapHistos[name2];
      if((*digiItr).sample(*itr-1).gainId()==1)
        hist = mapHistos[name3];
      if(hist!=0)
        hist->Fill((*digiItr).sample(*itr-1).adc());
      else
        cerr << "EcalPedHistDumper: Error: This shouldn't happen!" << endl;
    }
  }
}


std::string EcalPedHistDumperModule::intToString(int num)
{
  using namespace std;
  //
  // outputs the number into the string stream and then flushes
  // the buffer (makes sure the output is put into the stream)
  //
  ostringstream myStream; //creates an ostringstream object
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}


DEFINE_FWK_MODULE(EcalPedHistDumperModule);
