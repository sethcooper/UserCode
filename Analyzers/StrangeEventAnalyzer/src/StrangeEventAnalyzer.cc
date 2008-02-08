// -*- C++ -*-
//
// Package:    StrangeEventAnalyzer
// Class:      StrangeEventAnalyzer
// 
/**\class StrangeEventAnalyzer StrangeEventAnalyzer.cc Analyzers/StrangeEventAnalyzer/src/StrangeEventAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Tue Jan  8 16:47:23 CST 2008
// $Id: StrangeEventAnalyzer.cc,v 1.5 2008/01/29 21:39:06 scooper Exp $
//
//


// system include files
#include <memory>
#include <map>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "EcalFedMap.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TProfile.h"
#include "TDirectory.h"

//
// class decleration
//

class StrangeEventAnalyzer : public edm::EDAnalyzer {
   public:
      explicit StrangeEventAnalyzer(const edm::ParameterSet&);
      ~StrangeEventAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      std::string intToString(int);

      // ----------member data ---------------------------
      edm::InputTag MaxSampleUncalibratedRecHitCollection_;
      edm::InputTag FittedUncalibratedRecHitCollection_;
      edm::InputTag EBDigis_;
      TH1F* chi2Hist_;
      TH1F* maxMinAmpHist_;
      TH1F* delta1Hist_;
      TH1F* delta2Hist_;
      TH1F* maxSampleIndexHist_;
      TH1F* minSampleIndexHist_;
      TH1F* minSampleIndexCosmicsHist_;
      TH1F* numDataframesWithActivityHist_;
      TH1F* numMonstersInEventHist_;
      TH1F* numCosmicsInEventHist_;
      
      TH2F* delta1VsDelta2Hist_;
      TH2F* maxSampleIndexVsDelta1Hist_;
      TH2F* maxSampleIndexVsDelta2Hist_;
      TH2F* ampVsDelta1Hist_;
      TH2F* ampVsDelta2Hist_;
      TH2F* ampVsMaxSampleIndexHist_;
      TH2F* ampVsChi2Hist_;
      TH2F* barrelMonsterOccupancyHist_;
      TH2F* barrelCosmicOccupancyHist_;
      TH2F* barrelMonsterOccupancyTTHist_;
      TH2F* barrelCosmicOccupancyTTHist_;
      TH2F* numCosmicsSNPsEventHist_;
      TH2F* EB17ShapeHist_;
      TH2F* EB16ShapeHist_;

      TFile* file_;
      TDirectory* plotsDir;

      int abscissa[10];
      int ordinate[10];
      int fedIds[54];
      std::map<int,TH2F*> fedIdChannelHistMap_;
      std::map<int,TH2F*> fedIdMonsterOccupancyHistMap_;
      std::map<int,TH2F*> fedIdCosmicOccupancyHistMap_;
      EcalFedMap* fedMap_;
      std::vector<int> maskedChannels_;
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
StrangeEventAnalyzer::StrangeEventAnalyzer(const edm::ParameterSet& iConfig) :
MaxSampleUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("MaxSampleUncalibratedRecHitCollection")),
FittedUncalibratedRecHitCollection_ (iConfig.getParameter<edm::InputTag>("FittedUncalibratedRecHitCollection")),
EBDigis_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection"))
{
   file_ = new TFile("test.root","RECREATE");
   TH1::AddDirectory(false);

   chi2Hist_ = new TH1F("chi2","Alpha-Beta fitted chi-squared values",2000,-500,1000);
   maxMinAmpHist_ = new TH1F("maxMinAmp","Max-min sample amplitude",2000,0,2000);
   delta1Hist_ = new TH1F("deltaSample0Sample1","sample1-sample0",200,0,200);
   delta2Hist_ = new TH1F("deltaSample1Sample2","sample2-sample1",200,0,200);
   maxSampleIndexHist_ = new TH1F("maxSampleIndex","Index of max. amplitude sample",10,0,10);
   minSampleIndexHist_ = new TH1F("minSampleIndex","Index of min. amplitude sample",10,0,10);
   minSampleIndexCosmicsHist_ = new TH1F("minSampleIndexCosmics","Index of min. amplitude sample in possible cosmic signals",10,0,10);
   numDataframesWithActivityHist_ = new TH1F("numDFsWithActivity","Number of dataframes with activity per event",1700,0,1700);
   //WARNING: max range is adjusted so the half the barrel can be full of cosmics/monsters
   numMonstersInEventHist_ = new TH1F("SNPsInEvent","Number of SNPs per event",30600,0,30600);
   numCosmicsInEventHist_ = new TH1F("CosmicsInEvent","Number of cosmics per event",30600,0,30600);
   
   delta1VsDelta2Hist_ = new TH2F("delta01vsDelta12","sample1-sample0 vs. sample2-sample1",200,0,200,200,0,200);
   maxSampleIndexVsDelta1Hist_ = new TH2F("maxSampleIndexVsDelta1","max. sample index vs. sample1-sample0",200,0,200,10,0,10);
   maxSampleIndexVsDelta2Hist_ = new TH2F("maxSampleIndexVsDelta2","max. sample index vs. sample2-sample1",200,0,200,10,0,10);
   ampVsDelta1Hist_ = new TH2F("ampVsDelta01","max-min amplitude vs. sample1-sample0",200,0,200,2000,0,2000);
   ampVsDelta2Hist_ = new TH2F("ampVsDelta12","max-min amplitude vs. sample2-sample1",200,0,200,2000,0,2000);
   ampVsMaxSampleIndexHist_ = new TH2F("ampVsMaxSampleIndex","max-min amplitude vs. index of max sample",10,0,10,2000,0,2000);
   ampVsChi2Hist_ = new TH2F("ampVsChi2","Max-min amplitudes vs. chi-squares",2000,-500,1000,2000,0,2000);
   barrelMonsterOccupancyHist_ = new TH2F("barrelSNPOccupancy","SNP Occupancy",85,1,86,360,0,360);
   barrelCosmicOccupancyHist_ = new TH2F("barrelCosmicOccupancy","Cosmic Occupancy",85,1,86,360,0,360);
   barrelMonsterOccupancyTTHist_ = new TH2F("barrelSNPOccupancyTT","SNP Occupancy (TT binning)",34,-86,86,72,-10,350);
   barrelCosmicOccupancyTTHist_ = new TH2F("barrelCosmicOccupancyTT","Cosmic Occupancy (TT binning)",34,-86,86,72,-10,350);
   numCosmicsSNPsEventHist_ = new TH2F("cosmicsSNPsInEvent","numCosmicDataframes vs. numSNPDataframes/event",100,0,100,100,0,100);
   EB17ShapeHist_ = new TH2F("SNPSignalShapeEB17Cry677","Signal pulse shape for EB+17, ic677",10,0,10,10,0,1);
   EB16ShapeHist_ = new TH2F("SNPSignalShapeEB16Cry1427","Signal pulse shape for EB+16, ic1427",10,0,10,10,0,1);


   
   for (int i=0; i<10; i++)
     abscissa[i] = i;

   for (int i=0; i<54; i++)
     fedIds[i] = 0;
  
   fedMap_ = new EcalFedMap();
   std::vector<int> listDefaults;
   listDefaults.push_back(-1);
       
   maskedChannels_ = iConfig.getUntrackedParameter<std::vector<int> >("maskedChannels", listDefaults);
   plotsDir = file_->mkdir("Plots");
}


StrangeEventAnalyzer::~StrangeEventAnalyzer()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
void
StrangeEventAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace std;

   int numHitsWithActivity = 0;
   int numCosmicsInEvent = 0;
   int numMonstersInEvent = 0;
   
   int eventNum = iEvent.id().event();
   //vector<EcalUncalibratedRecHit> sampleHitsPastCut;
   
   Handle<EcalUncalibratedRecHitCollection> sampleHits;
   iEvent.getByLabel(MaxSampleUncalibratedRecHitCollection_, sampleHits);
   cout << "event: " << eventNum << " sample hits collection size: " << sampleHits->size() << endl;
   //Handle<EcalUncalibratedRecHitCollection> fittedHits;
   //iEvent.getByLabel(FittedUncalibratedRecHitCollection_, fittedHits);
   Handle<EBDigiCollection>  digis;
   iEvent.getByLabel(EBDigis_, digis);
   cout << "event: " << eventNum << " digi collection size: " << digis->size() << endl;

   auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);

   // Loop over the max-min hits
   for(EcalUncalibratedRecHitCollection::const_iterator hitItr = sampleHits->begin(); hitItr != sampleHits->end(); ++hitItr)
   {
     EcalUncalibratedRecHit hit = (*hitItr);
     float amplitude = hit.amplitude();
     maxMinAmpHist_->Fill(amplitude);
     EBDetId hitDetId = hit.id();
     //sampleHitsPastCut.push_back(hit);

     // Get the Fedid
     EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(hitDetId);
     int FEDid = 600+elecId.dccId();
     string SMname = fedMap_->getSliceFromFed(FEDid);
     // Debugging cout
     //cout << "Examining hit from evt:" << intToString(eventNum)
     //  << " ic:" << intToString(((EBDetId)hit.id()).ic()) << " ieta:" << intToString(hitDetId.ieta())
     //  << " iphi:" << intToString(hitDetId.iphi())
     //  << " SM:" << SMname << endl;

     vector<int>::iterator result;
     result = find(maskedChannels_.begin(), maskedChannels_.end(), hitDetId.hashedIndex());
     if  (result != maskedChannels_.end())
     {
       LogWarning("EcalMipGraphs") << "skipping uncalRecHit for channel: " << hitDetId.ic() << " in fed: " << FEDid << " with amplitude " << amplitude;
       continue;
     }

     //TODO: maybe fix this so that occupancy plots don't always get generated for each dcc in data?
     if(fedIds[FEDid-601]==0)
     {
       string name = "ampVsChannel_"+fedMap_->getSliceFromFed(FEDid);
       string title = "Max-min amplitudes vs. channel for "+fedMap_->getSliceFromFed(FEDid);
       fedIdChannelHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),1700,1,1701,2000,0,2000);
       
       name = "SNPOccupancy_"+fedMap_->getSliceFromFed(FEDid);
       title = "SNP occupancy for "+fedMap_->getSliceFromFed(FEDid);
       fedIdMonsterOccupancyHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),85,1,86,20,1,21);
       
       name = "cosmicOccupancy_"+fedMap_->getSliceFromFed(FEDid);
       title = "Cosmic occupancy for "+fedMap_->getSliceFromFed(FEDid);
       fedIdCosmicOccupancyHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),85,1,86,20,1,21);
       
       fedIds[FEDid-601]++;
     }
     fedIdChannelHistMap_[FEDid]->Fill(hitDetId.ic(),amplitude);

     //TODO: If we want any pedestal signals in hists, must play with this option here...
     //TODO: make this a parameter
     if(amplitude < 13)
       continue;

     // Hit is probably a signal
     numHitsWithActivity++;
     EBDigiCollection::const_iterator digiItr= digis->begin();
     while(digiItr != digis->end() && digiItr->id() != hitItr->id())
     {
       digiItr++;
     }
     if(digiItr == digis->end())
       continue;

     EBDataFrame df(*digiItr); 
     int delta1 = abs(df.sample(1).adc()-df.sample(0).adc());
     int delta2 = abs(df.sample(2).adc()-df.sample(1).adc());

     // check the min/max amp. sample indices
     // I moved this from inside the if with the deltas
     int maxSampleAmp = -1000;
     int maxSampleIndex = -1;
     int minSampleAmp = 10000;
     int minSampleIndex = -1;
     for(int i=0; i<10; ++i)
     {
       if(df.sample(i).adc() > maxSampleAmp)
       {
         maxSampleAmp = df.sample(i).adc();
         maxSampleIndex = i;
       }
       if(df.sample(i).adc() < minSampleAmp)
       {
         minSampleAmp = df.sample(i).adc();
         minSampleIndex = i;
       }
     }
     minSampleIndexCosmicsHist_->Fill(minSampleIndex);
     maxSampleIndexHist_->Fill(maxSampleIndex);
     minSampleIndexHist_->Fill(minSampleIndex);
     delta1Hist_->Fill(delta1);
     delta2Hist_->Fill(delta2);
     delta1VsDelta2Hist_->Fill(delta2,delta1);
     maxSampleIndexVsDelta2Hist_->Fill(delta2,maxSampleIndex);
     maxSampleIndexVsDelta1Hist_->Fill(delta1,maxSampleIndex);
     
     if(delta1 < 3 || delta2 < 3)
     {
       // could it be a cosmic hit?

       if(!(maxSampleIndex > 6 || maxSampleIndex < 4) && minSampleIndex < 3)
       {
         // probable cosmic
         for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
         {
           ordinate[i] = df.sample(i).adc();
         }
         TGraph oneGraph(10,abscissa,ordinate);
         plotsDir->cd();
         string name = "cosmic_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic()) + "_" + SMname;
         string title = "cosmic event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic()) +"_"+SMname;
         oneGraph.SetTitle(title.c_str());
         oneGraph.SetName(name.c_str());
         oneGraph.Write();
         file_->cd();
         fedIdCosmicOccupancyHistMap_[FEDid]->Fill(hitDetId.ietaSM(), hitDetId.iphiSM());
         barrelCosmicOccupancyHist_->Fill(hitDetId.ieta(), hitDetId.iphi());
         barrelCosmicOccupancyTTHist_->Fill(hitDetId.ieta(),hitDetId.iphi());
         numCosmicsInEvent++;
         cout << "Cosmic written; max/min amp samples. " << "evt:" << intToString(eventNum) 
           << " ic:" << intToString(((EBDetId)hit.id()).ic()) << " SM:" << SMname
           << " ieta:" << hitDetId.ieta() << " iphi:" << hitDetId.iphi() 
           << " hash:" << hitDetId.hashedIndex() << endl;
           //<< " ietaSM:" << hitDetId.ietaSM() << " iphiSM:" << hitDetId.iphiSM() << endl;

       }
       else // when max/min sample check is out of range, but deltas are ok
       {
         // monster
         for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
         {
           ordinate[i] = df.sample(i).adc();
         }
         TGraph oneGraph(10,abscissa,ordinate);
         plotsDir->cd();
         string name = "snp_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic())+"_"+SMname;
         string title = "snp event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic())+"_"+SMname;
         oneGraph.SetTitle(title.c_str());
         oneGraph.SetName(name.c_str());
         oneGraph.Write();
         file_->cd();
         fedIdMonsterOccupancyHistMap_[FEDid]->Fill(hitDetId.ietaSM(), hitDetId.iphiSM());
         barrelMonsterOccupancyHist_->Fill(hitDetId.ieta(),hitDetId.iphi());
         barrelMonsterOccupancyTTHist_->Fill(hitDetId.ieta(),hitDetId.iphi());
         numMonstersInEvent++;
         cout << "SNP written; max/min amp samples. " << "evt:" <<intToString(eventNum) 
           << " ic:" << intToString(((EBDetId)hit.id()).ic()) << " SM:" << SMname
           << " ieta:" << hitDetId.ieta() << " iphi:" << hitDetId.iphi() 
           << " hash:" << hitDetId.hashedIndex() << endl;
           //<< " ietaSM:" << hitDetId.ietaSM() << " iphiSM:" << hitDetId.iphiSM() << endl;
       }
     }
     else // when deltas are out of range
     {
       // monster
       for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
       {
         ordinate[i] = df.sample(i).adc();
       }
       TGraph oneGraph(10,abscissa,ordinate);
       plotsDir->cd();
       string name = "snp_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic())+"_"+SMname;
       string title = "snp event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic())+"_"+SMname;
       oneGraph.SetTitle(title.c_str());
       oneGraph.SetName(name.c_str());
       oneGraph.Write();
       file_->cd();
       fedIdMonsterOccupancyHistMap_[FEDid]->Fill(hitDetId.ietaSM(), hitDetId.iphiSM());
       barrelMonsterOccupancyHist_->Fill(hitDetId.ieta(),hitDetId.iphi());
       barrelMonsterOccupancyTTHist_->Fill(hitDetId.ieta(),hitDetId.iphi());
       numMonstersInEvent++;
       cout << "SNP written; delta out of range. " << "evt:" <<intToString(eventNum) 
         << " ic:" << intToString(((EBDetId)hit.id()).ic()) << " SM:" << SMname
         << " ieta:" << hitDetId.ieta() << " iphi:" << hitDetId.iphi() 
         << " hash:" << hitDetId.hashedIndex() << endl;
         //<< " ietaSM:" << hitDetId.ietaSM() << " iphiSM:" << hitDetId.iphiSM() << endl;
     }
   }

   numMonstersInEventHist_->Fill(numMonstersInEvent);
   numCosmicsInEventHist_->Fill(numCosmicsInEvent);
   numCosmicsSNPsEventHist_->Fill(numCosmicsInEvent,numMonstersInEvent);
   numDataframesWithActivityHist_->Fill(numHitsWithActivity);
}


// ------------ method called once each job just before starting event loop  ------------
void 
StrangeEventAnalyzer::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
StrangeEventAnalyzer::endJob()
{
  using namespace std;
  file_->cd();
  
  chi2Hist_->Write();
  maxMinAmpHist_->Write();
  delta1Hist_->Write();
  delta2Hist_->Write();
  maxSampleIndexHist_->Write();
  minSampleIndexHist_->Write();
  minSampleIndexCosmicsHist_->Write();
  numDataframesWithActivityHist_->Write();
  numCosmicsInEventHist_->Write();
  numMonstersInEventHist_->Write();
  
  delta1VsDelta2Hist_->Write();
  maxSampleIndexVsDelta1Hist_->Write();
  maxSampleIndexVsDelta2Hist_->Write();
  ampVsDelta1Hist_->Write();
  ampVsDelta2Hist_->Write();
  ampVsMaxSampleIndexHist_->Write();
  ampVsChi2Hist_->Write();
  barrelCosmicOccupancyHist_->Write();
  barrelMonsterOccupancyHist_->Write();
  barrelCosmicOccupancyTTHist_->Write();
  barrelMonsterOccupancyTTHist_->Write();
  numCosmicsSNPsEventHist_->Write();

  // now iterate through the maps and write the hists
  for(map<int,TH2F*>::const_iterator itr = fedIdChannelHistMap_.begin();
      itr != fedIdChannelHistMap_.end(); ++itr)
  {
    itr->second->Write();
  }
  for(map<int,TH2F*>::const_iterator itr = fedIdMonsterOccupancyHistMap_.begin();
      itr != fedIdMonsterOccupancyHistMap_.end(); ++itr)
  {
    itr->second->Write();
  }
  for(map<int,TH2F*>::const_iterator itr = fedIdCosmicOccupancyHistMap_.begin();
      itr != fedIdCosmicOccupancyHistMap_.end(); ++itr)
  {
    itr->second->Write();
  }
  
  file_->Write();
  file_->Close();
}

std::string StrangeEventAnalyzer::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

//define this as a plug-in
DEFINE_FWK_MODULE(StrangeEventAnalyzer);
