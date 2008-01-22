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
// $Id: StrangeEventAnalyzer.cc,v 1.1 2008/01/21 16:04:18 scooper Exp $
//
//


// system include files
#include <memory>
#include <map>

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
#include "CaloOnlineTools/EcalTools/interface/EcalFedMap.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"

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
      
      TH2F* delta1VsDelta2Hist_;
      TH2F* maxSampleIndexVsDelta1Hist_;
      TH2F* maxSampleIndexVsDelta2Hist_;
      TH2F* ampVsDelta1Hist_;
      TH2F* ampVsDelta2Hist_;
      TH2F* ampVsMaxSampleIndexHist_;
      TH2F* ampVsChi2Hist_;

      TFile* file_;

      int abscissa[10];
      int ordinate[10];
      int fedIds[36];
      std::map<int,TH2F*> fedIdChannelHistMap_;
      std::map<int,TH2F*> fedIdMonsterOccupancyHistMap_;
      std::map<int,TH2F*> fedIdCosmicOccupancyHistMap_;
      EcalFedMap* fedMap_;
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

   delta1VsDelta2Hist_ = new TH2F("delta01vsDelta12","sample1-sample0 vs. sample2-sample1",200,0,200,200,0,200);
   maxSampleIndexVsDelta1Hist_ = new TH2F("maxSampleIndexVsDelta1","max. sample index vs. sample1-sample0",200,0,200,10,0,10);
   maxSampleIndexVsDelta2Hist_ = new TH2F("maxSampleIndexVsDelta2","max. sample index vs. sample2-sample1",200,0,200,10,0,10);
   ampVsDelta1Hist_ = new TH2F("ampVsDelta01","max-min amplitude vs. sample1-sample0",200,0,200,2000,0,2000);
   ampVsDelta2Hist_ = new TH2F("ampVsDelta12","max-min amplitude vs. sample2-sample1",200,0,200,2000,0,2000);
   ampVsMaxSampleIndexHist_ = new TH2F("ampVsMaxSampleIndex","max-min amplitude vs. index of max sample",10,0,10,2000,0,2000);
   ampVsChi2Hist_ = new TH2F("ampVsChi2","Max-min amplitudes vs. chi-squares",2000,-500,1000,2000,0,2000);

   for (int i=0; i<10; i++)
     abscissa[i] = i;

   fedMap_ = new EcalFedMap();
}


StrangeEventAnalyzer::~StrangeEventAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

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
   int eventNum = iEvent.id().event();
   //vector<EcalUncalibratedRecHit> sampleHitsPastCut;
   
   Handle<EcalUncalibratedRecHitCollection> sampleHits;
   iEvent.getByLabel(MaxSampleUncalibratedRecHitCollection_, sampleHits);
   //LogDebug("StrangeEventAnalyzer") << "event: " << eventNum << " fitted hits collection size: " << sampleHits->size();
   Handle<EcalUncalibratedRecHitCollection> fittedHits;
   iEvent.getByLabel(FittedUncalibratedRecHitCollection_, fittedHits);
   //LogDebug("StrangeEventAnalyzer") << "event: " << eventNum << " fitted hits collection size: " << fittedHits->size();
   Handle<EBDigiCollection>  digis;
   iEvent.getByLabel(EBDigis_, digis);

   // Loop over the max-min hits
   for(EcalUncalibratedRecHitCollection::const_iterator hitItr = sampleHits->begin(); hitItr != sampleHits->end(); ++hitItr)
   {
     EcalUncalibratedRecHit hit = (*hitItr);
     float amplitude = hit.amplitude();
     maxMinAmpHist_->Fill(amplitude);
     EBDetId hitDetId = hit.id();
     //sampleHitsPastCut.push_back(hit);

     // Get the Fedid
     auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);
     EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId(hitDetId);
     int FEDid = 600+elecId.dccId();

     //TODO: maybe fix this so that occupancy plots don't always get generated for each dcc in data?
     if(fedIds[FEDid-601]==0)
     {
       string name = "ampVsChannel_"+fedMap_->getSliceFromFed(FEDid);
       string title = "Max-min amplitudes vs. channel for "+fedMap_->getSliceFromFed(FEDid);
       fedIdChannelHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),2000,0,1700,2000,0,2000);
       
       name = "monsterOccupancy_"+fedMap_->getSliceFromFed(FEDid);
       title = "Monster occupancy for "+fedMap_->getSliceFromFed(FEDid);
       fedIdMonsterOccupancyHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),85,0,85,20,0,20);
     
       
       name = "cosmicOccupancy_"+fedMap_->getSliceFromFed(FEDid);
       title = "Cosmic occupancy for "+fedMap_->getSliceFromFed(FEDid);
       fedIdCosmicOccupancyHistMap_[FEDid] = new TH2F(name.c_str(),title.c_str(),85,0,85,20,0,20);
       
       fedIds[FEDid-601]++;
     }
     
     fedIdChannelHistMap_[FEDid]->Fill(amplitude,hitDetId.ic());
     
     if(amplitude > 13)
     {
       // Hit is probably a signal
       numHitsWithActivity++;
       EBDigiCollection::const_iterator digiItr= digis->begin();
       while(digiItr != digis->end() && digiItr->id() != hitItr->id())
       {
         digiItr++;
       }
       if(digiItr != digis->end())
       {
         EBDataFrame df(*digiItr); 
         int delta1 = abs(df.sample(1).adc()-df.sample(0).adc());
         int delta2 = abs(df.sample(2).adc()-df.sample(1).adc());
         
         if(delta1 > 3 || delta2 > 3)
         {
           //signal could be a monster
           for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
           {
             ordinate[i] = df.sample(i).adc();
           }
           TGraph oneGraph(10,abscissa,ordinate);
           string name = "monster_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());
           string title = "monster event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());

           oneGraph.SetTitle(title.c_str());
           oneGraph.SetName(name.c_str());
           oneGraph.Write();
           fedIdMonsterOccupancyHistMap_[FEDid]->Fill(hitDetId.ieta(), hitDetId.iphi());
         }
         else
         {
           // could it be a cosmic hit?
           // check the min/max amp. sample indices
           int maxSampleAmp = -1000;
           int maxSampleIndex = -1;
           int minSampleAmp = 10000;
           int minSampleIndex = -1;
           for(int i=0; i<10; ++i)
           {
             if(df.sample(i) > maxSampleAmp)
             {
               maxSampleAmp = df.sample(i);
               maxSampleIndex = i;
             }
             if(df.sample(i) < minSampleAmp)
             {
               minSampleAmp = df.sample(i);
               minSampleIndex = i;
             }
           }
           minSampleIndexCosmicsHist_->Fill(minSampleIndex);
           
           if(!(maxSampleIndex > 6 || maxSampleIndex < 4))
           {
             // cosmic?
             if(minSampleIndex < 3)
             {
               // probable cosmic
               for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
               {
                 ordinate[i] = df.sample(i).adc();
               }
               TGraph oneGraph(10,abscissa,ordinate);
               string name = "cosmic_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());
               string title = "cosmic event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());

               oneGraph.SetTitle(title.c_str());
               oneGraph.SetName(name.c_str());
               oneGraph.Write();
               fedIdCosmicOccupancyHistMap_[FEDid]->Fill(hitDetId.ieta(), hitDetId.iphi());
             }
             else
             {
               // another monster
               for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
               {
                 ordinate[i] = df.sample(i).adc();
               }
               TGraph oneGraph(10,abscissa,ordinate);
               string name = "monster_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());
               string title = "monster event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());
               oneGraph.SetTitle(title.c_str());
               oneGraph.SetName(name.c_str());
               oneGraph.Write();
               fedIdMonsterOccupancyHistMap_[FEDid]->Fill(hitDetId.ieta(), hitDetId.iphi());
             }
           }
           else
           {
             // yet another monster
             for (int i=0; (unsigned int)i< (*digiItr).size(); ++i )
             {
               ordinate[i] = df.sample(i).adc();
             }
             TGraph oneGraph(10,abscissa,ordinate);
             string name = "monster_ev" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());
             string title = "monster event" + intToString(eventNum) + "_ic" + intToString(((EBDetId)hit.id()).ic());
             oneGraph.SetTitle(title.c_str());
             oneGraph.SetName(name.c_str());
             oneGraph.Write();
             fedIdMonsterOccupancyHistMap_[FEDid]->Fill(hitDetId.ieta(), hitDetId.iphi());
           }
         }
       }
     }
   }
   

   // Loop over the sample hits passing the cut
//   for(vector<EcalUncalibratedRecHit>::const_iterator hitItr = sampleHitsPastCut.begin(); hitItr != sampleHitsPastCut.end(); ++hitItr)
//   {
//     EcalUncalibratedRecHitCollection::const_iterator fittedHitItr = fittedHits->begin();
//     // find the hit passing the amp. cut in the fitted hits collection
//     while(fittedHitItr != fittedHits->end() && fittedHitItr->id() != hitItr->id())
//     {
//       fittedHitItr++;
//     }
//     if(fittedHitItr != fittedHits->end())
//     {
//       //LogInfo("StrangeEventAnalyzer") << "Event:" << eventNum << " Amplitude:" << hitItr->amplitude()
//       //  << " Chi2:" << fittedHitItr->chi2() << " iSM:" << 
//       //  ((EBDetId)hitItr->id()).ism() << " ic:"  << ((EBDetId)hitItr->id()).ic();
//       ampVsChi2Hist_->Fill(fittedHitItr->chi2(),hitItr->amplitude());
//     }
//   }

   // Loop over all alpha-beta fitted hits
//   for(EcalUncalibratedRecHitCollection::const_iterator hitItr = fittedHits->begin(); hitItr != fittedHits->end(); ++hitItr)
//   {
//     EcalUncalibratedRecHit hit = (*hitItr);
//     chi2Hist_->Fill(hit.chi2());
//   } 
   
   int badDeltaHits = 0;
   for(EBDigiCollection::const_iterator digiItr= digis->begin();digiItr != digis->end(); ++digiItr)
   {
     EBDataFrame df(*digiItr);
     int maxSampleAmp = -1000;
     int maxSampleIndex = -1;
     int minSampleAmp = 10000;
     int minSampleIndex = -1;
     for(int i=0; i<10; ++i)
     {
       if(df.sample(i) > maxSampleAmp)
       {
         maxSampleAmp = df.sample(i);
         maxSampleIndex = i;
       }
       if(df.sample(i) < minSampleAmp)
       {
         minSampleAmp = df.sample(i);
         minSampleIndex = i;
       }
     }
     maxSampleIndexHist_->Fill(maxSampleIndex);
     minSampleIndexHist_->Fill(minSampleIndex);
     
     int delta1 = abs(df.sample(1)-df.sample(0));
     int delta2 = abs(df.sample(2)-df.sample(1));
     
     delta1Hist_->Fill(delta1);
     delta2Hist_->Fill(delta2);
     delta1VsDelta2Hist_->Fill(delta2,delta1);
     maxSampleIndexVsDelta2Hist_->Fill(delta2,maxSampleIndex);
     maxSampleIndexVsDelta1Hist_->Fill(delta1,maxSampleIndex);
     
     EcalUncalibratedRecHitCollection::const_iterator sampleHitItr = sampleHits->begin();
     while(sampleHitItr != sampleHits->end() && sampleHitItr->id() != digiItr->id())
     {
       sampleHitItr++;
     }
     if(sampleHitItr != sampleHits->end())
     {
       float amplitude = sampleHitItr->amplitude();
       ampVsDelta2Hist_->Fill(delta2, amplitude);
       ampVsDelta1Hist_->Fill(delta1, amplitude);
       ampVsMaxSampleIndexHist_->Fill(maxSampleIndex,amplitude);
     }

     
     if(delta1 > 3 || delta2 > 3)
     {
       //LogInfo("StrangeEventAnalyzer") << "Event:" << eventNum << " cry:" << ((EBDetId)digiItr->id()).ic() <<
       // " delta 01:" << delta1 << " delta12:" << delta2;
       badDeltaHits++;
     }
     //else
       //LogInfo("StrangeEventAnalyzer") << "Good event:" << eventNum << " cry:" << ((EBDetId)digiItr->id()).ic();
   }
   
   LogInfo("StrangeEventAnalyzer") << "Event:" << eventNum << " num hits with activity:" << numHitsWithActivity;
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
  
  delta1VsDelta2Hist_->Write();
  maxSampleIndexVsDelta1Hist_->Write();
  maxSampleIndexVsDelta2Hist_->Write();
  ampVsDelta1Hist_->Write();
  ampVsDelta2Hist_->Write();
  ampVsMaxSampleIndexHist_->Write();
  ampVsChi2Hist_->Write();

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
