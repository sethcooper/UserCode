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

#include <iostream>
#include <vector>
#include "TFile.h"
#include "TH1.h"

using namespace std;
using namespace edm;

class EcalPedHistDumperModule: public EDAnalyzer
{ 
  public:
    EcalPedHistDumperModule(const ParameterSet& ps);   
    ~EcalPedHistDumperModule();

  protected:
    void analyze(const Event & e, const  EventSetup& c);
    void beginJob(const EventSetup& c);
    void endJob(void);

  private:
    string intToString(int num);

    int first_ic_;
    bool inputIsOk_;
    string fileName_;
    vector<int> listChannels_;
    vector<int> listSamples_;
    vector<int> listSupermodules_;
    map<string,TH1F*> mapHistos_;
    TFile * root_file_;
};


EcalPedHistDumperModule::EcalPedHistDumperModule(const ParameterSet& ps)
{  
  fileName_ = ps.getUntrackedParameter<string>("fileName", string("ecalPedDigiDump") );
  first_ic_ = 0;
  vector<int> v = vector<int>();
  v.push_back(1);
  listSupermodules_ = ps.getUntrackedParameter<vector<int> >("listSupermodules", v);
  listChannels_ = ps.getUntrackedParameter<vector<int> >("listChannels", v);
  v.clear();
  // Get samples to plot (default to 1,2,3)
  v.push_back(0);
  v.push_back(1);
  v.push_back(2);
  listSamples_ = ps.getUntrackedParameter<vector<int> >("listSamples", v);
  
  inputIsOk_ = true;
  vector<int>::iterator intIter;
  // Verify crystal numbers are valid
  for (intIter = listChannels_.begin(); intIter != listChannels_.end(); intIter++)
  {  
      if ( ((*intIter) < 1)||(1700 < (*intIter)) )       
      {  
	cout << "[EcalPedHistDumperModule] ic value: " << (*intIter) << " found in listChannels. "
		  << " Valid range is 1-1700. Returning." << endl;
	inputIsOk_ = false;
	return;
      }
      // Initialize with the first channel of the list
      if (!first_ic_) 
        first_ic_ = (*intIter);
      
      //Put 3 histos (1 per gain) for the channel into the map
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
      mapHistos_.insert(make_pair(name1,new TH1F(name1.c_str(),title1.c_str(),75,175.0,250.0)));
      mapHistos_.insert(make_pair(name2,new TH1F(name2.c_str(),title2.c_str(),75,175.0,250.0)));
      mapHistos_.insert(make_pair(name3,new TH1F(name3.c_str(),title3.c_str(),75,175.0,250.0)));
  }
  
  // Verify supermodule numbers are valid
  for (intIter = listSupermodules_.begin(); intIter != listSupermodules_.end(); intIter++)
  {  
      if ( ((*intIter) < 1)||(36 < (*intIter)) )
      {  
	cout << "[EcalPedHistDumperModule] ism value: " << (*intIter) << " found in listSupermodules. "
		  << " Valid range is 1-36. Returning." << endl;
	inputIsOk_ = false;
	return;
      }
  }
  
  // Verify sample numbers are valid
  for (intIter = listSamples_.begin(); intIter != listSamples_.end(); intIter++)
  {  
      if ( ((*intIter) < 1)||(36 < (*intIter)) )
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

void EcalPedHistDumperModule::beginJob(const EventSetup& c)
{
}

void EcalPedHistDumperModule::endJob(void)
{
 if(inputIsOk_)
 { 
  for(vector<int>::iterator itr = listSupermodules_.begin(); itr!=listSupermodules_.end(); itr++)
  {
    fileName_ += (string("_iEB") + intToString(*itr));
  }
  fileName_ += ".graph.root";

  TFile root_file_(fileName_.c_str() , "RECREATE");
  TH1F* hist = 0;
  //Loop over channels; write histos and directory structure
  for (vector<int>::iterator itr = listChannels_.begin(); itr!=listChannels_.end(); itr++)
  {
    string chnl = intToString(*itr);
    string name1 = "Cry";
    name1.append(chnl+"Gain1");
    string name2 = "Cry";
    name2.append(chnl+"Gain6");
    string name3 = "Cry";
    name3.append(chnl+"Gain12");
    hist = mapHistos_[name1];
    // This is a sanity check only
    if(hist!=0)
    {
      root_file_.mkdir(chnl.insert(0,"Cry_").c_str());
      root_file_.cd(chnl.c_str());
      hist->Write();
      hist = mapHistos_[name2];
      hist->Write();
      hist = mapHistos_[name3];
      hist->Write();
      root_file_.cd();
    }
    else
      cerr << "EcalPedHistDumperModule: Error: This shouldn't happen!" << endl;
  }
  root_file_.Close();
 }
}


string EcalPedHistDumperModule::intToString(int num)
{
  //
  // outputs the number into the string stream and then flushes
  // the buffer (makes sure the output is put into the stream)
  //
  ostringstream myStream; //creates an ostringstream object
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}


void EcalPedHistDumperModule::analyze( const Event & e, const  EventSetup& c)
{
  if (!inputIsOk_)
    return;

  // Retrieve crystal data from event
  Handle<EBDigiCollection>  digis;
  e.getByLabel("ecalEBunpacker", "ebDigis", digis);

  // Loop over digis
  for ( EBDigiCollection::const_iterator digiItr= digis->begin();digiItr != digis->end(); ++digiItr )
  {
    int ic = EBDetId((*digiItr).id()).ic();
    int ism = EBDetId((*digiItr).id()).ism();

    //Select desired supermodules only
    vector<int>::iterator icIter;
    icIter = find(listSupermodules_.begin(), listSupermodules_.end(), ism);
    if (icIter == listSupermodules_.end())
      continue;

    // Select desired channels only
    icIter = find(listChannels_.begin(), listChannels_.end(), ic);
    if (icIter == listChannels_.end())
      continue;

    // Get the adc counts from the selected samples and fill the corresponding histogram
    // Must subtract 1 from user-given sample list (e.g., user's sample 1 -> sample 0)
    for (vector<int>::iterator itr = listSamples_.begin(); itr!=listSamples_.end(); itr++)
    {
      string chnl = intToString(ic);
      string name1 = "Cry";
      name1.append(chnl+"Gain1");
      string name2 = "Cry";
      name2.append(chnl+"Gain6");
      string name3 = "Cry";
      name3.append(chnl+"Gain12");
      TH1F* hist = 0;
      if((*digiItr).sample(*itr-1).gainId()==3)
        hist = mapHistos_[name1];
      if((*digiItr).sample(*itr-1).gainId()==2)
        hist = mapHistos_[name2];
      if((*digiItr).sample(*itr-1).gainId()==1)
        hist = mapHistos_[name3];
      if(hist!=0)
        hist->Fill((*digiItr).sample(*itr-1).adc());
      else
        cerr << "EcalPedHistDumper: Error: This shouldn't happen!" << endl;
    }
  }
}

DEFINE_FWK_MODULE(EcalPedHistDumperModule);
