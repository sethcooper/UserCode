// -*- C++ -*-
//
// Package:    EcalTimeCalibAnalyzer
// Class:      EcalTimeCalibAnalyzer
// 
/**\class EcalTimeCalibAnalyzer EcalTimeCalibAnalyzer.cc Analyzers/EcalTimeCalibAnalyzer/src/EcalTimeCalibAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Th May 28 17:00:01 CDT 2009
// $Id$
//
//


// system include files
#include <memory>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
// Geometry
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"



//
// class decleration
//

class EcalTimeCalibAnalyzer : public edm::EDAnalyzer {
   public:
      explicit EcalTimeCalibAnalyzer(const edm::ParameterSet&);
      ~EcalTimeCalibAnalyzer();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      bool readCalibrationFile();
      std::string doubleToString(double num);
      double timecorr(const CaloSubdetectorGeometry *geometry_p, DetId id);

      // ----------member data ---------------------------
      std::string infile_;
      const EcalElectronicsMapping* ecalElectronicsMap_;
      static edm::Service<TFileService> fileService_;
      static double energyBins[40];

      edm::InputTag EBRecHitCollection_;
      edm::InputTag EERecHitCollection_;
      edm::InputTag EBUncalibRecHitCollection_;

      bool applyCalibration_;
      
      std::map<int,float> hashToCalibMapEB_;
      std::map<int,float> hashToCalibMapEE_;
      std::map<int,float> hashToCalibErrMapEB_;
      std::map<int,float> hashToCalibErrMapEE_;

      TH1F* diffBetweenCrysSameTTHist_;
      TH2F* rawTimingMapEB_;
      TH2F* rawTimingMapEventEB_[21];
      TH1F* rawTimingHistEventEB_[21];
      TH1F* rawTimingHistEB_;
      TH1F* diffBetweenCrysSameTTSameTCalibHist_;
      TH1F* amplitudeHist_;
      TH1F* deltaBetCrys_[50];
      TH2F* avgTimeVsEventHist_;

      //special cry hists
      TH1F* timingCry1Hist_;
      TH1F* timingCry2Hist_;
      TH1F* timingCry3Hist_;
      TH1F* timingCry4Hist_;
      TH1F* amplitudeCry1Hist_;
      TH1F* amplitudeCry2Hist_;
      TH1F* amplitudeCry3Hist_;
      TH1F* amplitudeCry4Hist_;
      TH2F* energyVsTimeCry1Hist_;
      TH2F* energyVsTimeCry2Hist_;
      TH2F* energyVsTimeCry3Hist_;
      TH2F* energyVsTimeCry4Hist_;
      
      TH2F* energyVsTimeCry5Hist_;
      TH2F* energyVsTimeCry6Hist_;
      TH2F* energyVsTimeCry7Hist_;
      TH2F* energyVsTimeCry8Hist_;
      TH2F* energyVsTimeCry9Hist_;
      TH2F* energyVsTimeCry10Hist_;
      TH2F* energyVsTimeCry11Hist_;
      TH2F* energyVsTimeCry12Hist_;
      TH2F* energyVsTimeCry13Hist_;
      TH2F* energyVsTimeCry14Hist_;

      TProfile* timeByEtaProf_;
      TH1F* timeByEtaN5Hist_;
      TH1F* timeByEtaN30Hist_;
      TH1F* timeByEtaN80Hist_;
      TH1F* timeByEtaP15Hist_;
      TH1F* timeByEtaP50Hist_;

      TH1F* EBMinusMeasuredExpectedHist;
      TH1F* EBPlusMeasuredExpectedHist;
      TH1F* EBMeasuredExpectedHist;
      TH2F* EBMeasExpPeakMap;

      TH1F* EBMeasuredExpectedMagicRingHist;
      TH1F* EBMeasuredExpectedAbsTime3nsHist;
      
      int numEnergyBins;
      int ievt;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//
edm::Service<TFileService> EcalTimeCalibAnalyzer::fileService_;
double EcalTimeCalibAnalyzer::energyBins[40] = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,2,3,4,5,7.5,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180};


//
// constructors and destructor
//
EcalTimeCalibAnalyzer::EcalTimeCalibAnalyzer(const edm::ParameterSet& iConfig) :
  infile_ (iConfig.getUntrackedParameter<std::string>("calibrationFile","")),
  EBRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalRecHitCollectionEB")),
  EERecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalRecHitCollectionEE")),
  EBUncalibRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EcalUncalibRecHitCollectionEB")),
  applyCalibration_ (iConfig.getUntrackedParameter<bool>("applyCalibration",false))
{
  
  ievt = 0;
  bool success = readCalibrationFile();
  if(!success)
  {
    std::cout << "An error occurred reading the calibration file " <<
      infile_ << std::endl;
    return;
  }

  diffBetweenCrysSameTTHist_ = fileService_->make<TH1F>("diffBetCrysSameTT","Timing difference between pairs of crys, same TT [ns]",2000,-10,10);
  rawTimingMapEB_ = fileService_->make<TH2F>("rawTimingMapEB","Timing of RecHits, raw [ns]",360,1,361,172,-86,86);
  rawTimingHistEB_ = fileService_->make<TH1F>("rawTimingHistEB","Timing of RecHits, raw [ns]",200,-100,100);
  diffBetweenCrysSameTTSameTCalibHist_ = fileService_->make<TH1F>("diffBetCrysSameTTSameTCalib","Timing difference between pairs of crys, same TT, #Deltat_calib < 0.5 ns",2000,-10,10);
  amplitudeHist_ = fileService_->make<TH1F>("recHitEnergy","Energy of RecHits",200,0,20);
  avgTimeVsEventHist_ = fileService_->make<TH2F>("avgTimeVsEvent","Average time of all channels [ns] vs. event number",20,0,20,200,-100,100);
  timingCry1Hist_ = fileService_->make<TH1F>("timingOfcry1","Timing of cry i#eta-10,i#phi30 hits, [ns]",2000,-100,100);
  timingCry2Hist_ = fileService_->make<TH1F>("timingOfcry2","Timing of cry i#eta-2,i#phi113 hits, [ns]",2000,-100,100);
  timingCry3Hist_ = fileService_->make<TH1F>("timingOfcry3","Timing of cry i#eta-20,i#phi269 hits, [ns]",2000,-100,100);
  timingCry4Hist_ = fileService_->make<TH1F>("timingOfcry4","Timing of cry i#eta-45,i#phi29 hits, [ns]",2000,-100,100);
  amplitudeCry1Hist_ = fileService_->make<TH1F>("recHitEnergyCry1","Energy of RecHits cry 1",200,0,20);
  amplitudeCry2Hist_ = fileService_->make<TH1F>("recHitEnergyCry2","Energy of RecHits cry 2",200,0,20);
  amplitudeCry3Hist_ = fileService_->make<TH1F>("recHitEnergyCry3","Energy of RecHits cry 3",200,0,20);
  amplitudeCry4Hist_ = fileService_->make<TH1F>("recHitEnergyCry4","Energy of RecHits cry 4",200,0,20);
  energyVsTimeCry1Hist_ = fileService_->make<TH2F>("energyVsTimeCry1","RecHit energy vs. time cry 1",2000,-100,100,200,0,20);
  energyVsTimeCry2Hist_ = fileService_->make<TH2F>("energyVsTimeCry2","RecHit energy vs. time cry 2",2000,-100,100,200,0,20);
  energyVsTimeCry3Hist_ = fileService_->make<TH2F>("energyVsTimeCry3","RecHit energy vs. time cry 3",2000,-100,100,200,0,20);
  energyVsTimeCry4Hist_ = fileService_->make<TH2F>("energyVsTimeCry4","RecHit energy vs. time cry 4",2000,-100,100,200,0,20);
  energyVsTimeCry5Hist_ = fileService_->make<TH2F>("energyVsTimeCry5","RecHit energy vs. time cry 5",2000,-100,100,200,0,20);
  energyVsTimeCry6Hist_ = fileService_->make<TH2F>("energyVsTimeCry6","RecHit energy vs. time cry 6",2000,-100,100,200,0,20);
  energyVsTimeCry7Hist_ = fileService_->make<TH2F>("energyVsTimeCry7","RecHit energy vs. time cry 7",2000,-100,100,200,0,20);
  energyVsTimeCry8Hist_ = fileService_->make<TH2F>("energyVsTimeCry8","RecHit energy vs. time cry 8",2000,-100,100,200,0,20);
  energyVsTimeCry9Hist_ = fileService_->make<TH2F>("energyVsTimeCry9","RecHit energy vs. time cry 9",2000,-100,100,200,0,20);
  energyVsTimeCry10Hist_ = fileService_->make<TH2F>("energyVsTimeCry10","RecHit energy vs. time cry 10",2000,-100,100,200,0,20);
  energyVsTimeCry11Hist_ = fileService_->make<TH2F>("energyVsTimeCry11","RecHit energy vs. time cry 11",2000,-100,100,200,0,20);
  energyVsTimeCry12Hist_ = fileService_->make<TH2F>("energyVsTimeCry12","RecHit energy vs. time cry 12",2000,-100,100,200,0,20);
  energyVsTimeCry13Hist_ = fileService_->make<TH2F>("energyVsTimeCry13","RecHit energy vs. time cry 13",2000,-100,100,200,0,20);
  energyVsTimeCry14Hist_ = fileService_->make<TH2F>("energyVsTimeCry14","RecHit energy vs. time cry 14",2000,-100,100,200,0,20);
  timeByEtaProf_ = fileService_->make<TProfile>("timeByEtaProf","Time in each i#eta ring",172,-86,86);
  timeByEtaN80Hist_ = fileService_->make<TH1F>("timeByEtaN80","Time for crystals in i#eta==-80",200,0,100);
  timeByEtaN30Hist_ = fileService_->make<TH1F>("timeByEtaN30","Time for crystals in i#eta==-30",200,0,100);
  timeByEtaN5Hist_ = fileService_->make<TH1F>("timeByEtaN5","Time for crystals in i#eta==-5",200,0,100);
  timeByEtaP15Hist_ = fileService_->make<TH1F>("timeByEtaP15","Time for crystals in i#eta==15",200,0,100);
  timeByEtaP50Hist_ = fileService_->make<TH1F>("timeByEtaP50","Time for crystals in i#eta==50",200,0,100);
  
  EBMinusMeasuredExpectedHist = fileService_->make<TH1F>("EBMinusMeasuredExpected","Measured - expected time for crys in EB- [ns]",4000,-20,20);
  EBPlusMeasuredExpectedHist = fileService_->make<TH1F>("EBPlusMeasuredExpected","Measured - expected time for crys in EB+ [ns]",4000,-20,20);
  EBMeasuredExpectedHist = fileService_->make<TH1F>("EBMeasuredExpected","Measured - expected time for crys in EB [ns]",4000,-20,20);
  EBMeasExpPeakMap = fileService_->make<TH2F>("EBMeasuredExpectedPeakMap","Crys in +/- 1 ns around 0 in measured-expected time",360,1,361,172,-86,86);
  
  EBMeasuredExpectedMagicRingHist = fileService_->make<TH1F>("EBMeasuredExpectedMagicRing","Measured - expected time for crys in magic ring [ns]",4000,-20,20);
  EBMeasuredExpectedAbsTime3nsHist = fileService_->make<TH1F>("EBMeasuredExpectedABsTime3ns","Measured - expected time for crys with +/- abs time within 3 ns [ns]",4000,-20,20);
  
  for(int i=0; i<21; ++i)
  {
    std::string histName = "rawTimingMapEB_event"+doubleToString((double)i);
    rawTimingMapEventEB_[i] = fileService_->make<TH2F>(histName.c_str(),"Timing of RecHits, raw [ns]",360,1,361,172,-86,86);
    histName = "rawTimingHistEB_event"+doubleToString((double)i);
    rawTimingHistEventEB_[i] = fileService_->make<TH1F>(histName.c_str(),"Timing of RecHits, raw [ns]",200,-100,100);
  }
  
  // Set up the energy-binned hists
  numEnergyBins = 40;

  char hname[50];
  char htitle[50];
  for(int i=32;i<numEnergyBins;++i)
    energyBins[i]=0;
  for(int i=0;i<numEnergyBins;++i)
  {
    std::string title;
    title = "Difference in time between crys, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    sprintf(hname,"deltaBetCrysEnergyBin_%d",i);
    sprintf(htitle,"Difference between crys, energy bin %d",i);
    deltaBetCrys_[i] = fileService_->make<TH1F>(hname,htitle,10000,-50,50);
    deltaBetCrys_[i]->SetTitle(title.c_str());
  }
}


EcalTimeCalibAnalyzer::~EcalTimeCalibAnalyzer()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalTimeCalibAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  ievt++;  

  Handle<EcalRecHitCollection> EBhits;
  Handle<EcalRecHitCollection> EEhits;
  Handle<EcalUncalibratedRecHitCollection> EBUncalibHits;
  iEvent.getByLabel(EBRecHitCollection_, EBhits);
  iEvent.getByLabel(EERecHitCollection_, EEhits);
  iEvent.getByLabel(EBUncalibRecHitCollection_, EBUncalibHits);

  //Geometry information
  edm::ESHandle<CaloGeometry> geoHandle;
  iSetup.get<CaloGeometryRecord>().get(geoHandle);
  const CaloSubdetectorGeometry *geometry_pEB = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
  const CaloSubdetectorGeometry *geometry_pEE = geoHandle->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);

  vector<EcalRecHit> hitsInEnergyBin[40];
  float averageTime = 0;
  int numHits = 0;

  vector<int> ebEtaFEDTTHashes[171][36][68]; // eta,FED,TTId, leaving eta==0 empty
  for (EcalRecHitCollection::const_iterator hitItr = EBhits->begin(); hitItr != EBhits->end(); ++hitItr)
  {
    //TODO: Apply some cuts?  Chi2 cut?
    if(hitItr->energy() < 0.750)  // 0.750 GeV energy cut
      continue;
    
    EBDetId det = hitItr->id();
    
    //XXX: Chi2 cut
    EBUncalibratedRecHitCollection::const_iterator uncalibHit = EBUncalibHits->find(det);
    if(uncalibHit==EBUncalibHits->end())
    {
      cout << "This should not happen." << endl;
      continue;
    }
    else if(!(uncalibHit->chi2() > 0))
      continue;
    
    EcalElectronicsId elecId = ecalElectronicsMap_->getElectronicsId(det);
    int FEDid = 600+elecId.dccId();
    int TTid = elecId.towerId();
    ebEtaFEDTTHashes[det.ieta()+85][FEDid-610][TTid-1].push_back(det.hashedIndex());
  
    rawTimingMapEB_->Fill(det.iphi(),det.ieta(),25*hitItr->time());
    rawTimingMapEventEB_[ievt]->Fill(det.iphi(),det.ieta(),25*hitItr->time());
    rawTimingHistEB_->Fill(25*hitItr->time());
    rawTimingHistEventEB_[ievt]->Fill(25*hitItr->time());
    amplitudeHist_->Fill(hitItr->energy());
    float thisTimeCalib = 0;
    map<int,float>::const_iterator thisCalibMapItr = hashToCalibMapEB_.find(det.hashedIndex());
    if(thisCalibMapItr!=hashToCalibMapEB_.end() && applyCalibration_)
    {
      thisTimeCalib = thisCalibMapItr->second;
    }

    if(hitItr->energy() > 5)
    {
      float newTime = thisTimeCalib+25*hitItr->time();
      timeByEtaProf_->Fill(det.ieta(),newTime);
      if(det.ieta()==-5)
        timeByEtaN5Hist_->Fill(newTime);
      else if(det.ieta()==-30)
        timeByEtaN30Hist_->Fill(newTime);
      else if(det.ieta()==-80)
        timeByEtaN80Hist_->Fill(newTime);
      else if(det.ieta()==15)
        timeByEtaP15Hist_->Fill(newTime);
      else if(det.ieta()==50)
        timeByEtaP50Hist_->Fill(newTime);
    }
    

    //Special cry hists
    if(det.ieta()==-10&&det.iphi()==30)
    {
      timingCry1Hist_->Fill(25*hitItr->time());
      amplitudeCry1Hist_->Fill(hitItr->energy());
      energyVsTimeCry1Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==-2&&det.iphi()==113)
    {
      timingCry2Hist_->Fill(25*hitItr->time());
      amplitudeCry2Hist_->Fill(hitItr->energy());
      energyVsTimeCry2Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==20&&det.iphi()==269)
    {
      timingCry3Hist_->Fill(25*hitItr->time());
      amplitudeCry3Hist_->Fill(hitItr->energy());
      energyVsTimeCry3Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==45&&det.iphi()==29)
    {
      timingCry4Hist_->Fill(25*hitItr->time());
      amplitudeCry4Hist_->Fill(hitItr->energy());
      energyVsTimeCry4Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==-10&&det.iphi()==343)
    {
      energyVsTimeCry5Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==-17&&det.iphi()==201)
    {
      energyVsTimeCry6Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==-1&&det.iphi()==326)
    {
      energyVsTimeCry7Hist_->Fill(25*hitItr->time(),hitItr->energy());
      std::cout << "IC: 6; ievt=" << ievt << " energy[GeV]=" << hitItr->energy()
        << " time[ns]=" << 25*hitItr->time() << " chi2=" << uncalibHit->chi2() << std::endl;
    }
    else if(det.ieta()==-24&&det.iphi()==219)
    {
      energyVsTimeCry8Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==-34&&det.iphi()==84)
    {
      energyVsTimeCry9Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==84&&det.iphi()==272)
    {
      energyVsTimeCry10Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==75&&det.iphi()==37)
    {
      energyVsTimeCry11Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==55&&det.iphi()==195)
    {
      energyVsTimeCry12Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==30&&det.iphi()==12)
    {
      energyVsTimeCry13Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    else if(det.ieta()==2&&det.iphi()==252)
    {
      energyVsTimeCry14Hist_->Fill(25*hitItr->time(),hitItr->energy());
    }
    
    averageTime+=25*hitItr->time();
    numHits++;
    //// Bin up by energy
    //for(int i=0;i<numEnergyBins-1;++i)
    //{
    //  if(hitItr->energy() >= energyBins[i] && hitItr->energy() < energyBins[i+1])
    //  {
    //    EcalRecHit hit = *hitItr;
    //    hitsInEnergyBin[i].push_back(hit);
    //    break;
    //  }
    //}
  }
  avgTimeVsEventHist_->Fill(ievt,averageTime/numHits);

  // Loop over the eta
  for(int ieta=0;ieta < 171; ++ieta)
  {
    // Loop over the FED
    for(int ifed=0;ifed < 36; ++ifed)
    {
      // Loop over the TT
      for(int itt=0; itt < 68; ++itt)
      {
        vector<int> thisVector = ebEtaFEDTTHashes[ieta][ifed][itt];
        if(thisVector.size() < 2)
          continue;
        vector<int>::const_iterator vecItr = thisVector.begin();
        EBDetId firstDet = EBDetId::unhashIndex(*vecItr);
        EBRecHitCollection::const_iterator firstHit = EBhits->find(firstDet);
        if(firstHit==EBhits->end())
        {
          cout << "Error: This shouldn't happen..." << endl;
          continue;
        }
        float firstCryTime = 25*firstHit->time();
        float timeCalib1 = 0;
        map<int,float>::const_iterator calibMapItr = hashToCalibMapEB_.find(*vecItr);
        if(calibMapItr!=hashToCalibMapEB_.end())
        {
          timeCalib1 = calibMapItr->second;
          if(applyCalibration_)
            firstCryTime+=timeCalib1;
        }
        else if(applyCalibration_)
          continue; // reject uncalibrated crys

        // figure out which energyBin it's in
        int firstCryEnergyBin = -1;
        for(int i=0;i<numEnergyBins-1;++i)
        {
          if(firstHit->energy() >= energyBins[i] && firstHit->energy() < energyBins[i+1])
          {
            firstCryEnergyBin = i;
            break;
          }
        }
        
        // Loop over the vector
        for(vector<int>::const_iterator thisCryItr = vecItr+1; thisCryItr != thisVector.end(); ++thisCryItr)
        {
          EBDetId thisDet = EBDetId::unhashIndex(*thisCryItr);
          EBRecHitCollection::const_iterator thisHit = EBhits->find(thisDet);
          if(thisHit==EBhits->end())
          {
            cout << "Error: This shouldn't happen (2)..." << endl;
            continue;
          }

          bool fillEnergyBinHist = false;
          // figure out which energyBin it's in
          int thisEnergyBin = -2;
          for(int i=0;i<numEnergyBins-1;++i)
          {
            if(thisHit->energy() >= energyBins[i] && thisHit->energy() < energyBins[i+1])
            {
              thisEnergyBin = i;
              break;
            }
          }
          if(thisEnergyBin==firstCryEnergyBin)
            fillEnergyBinHist = true;
          
          float thisCryTime = 25*thisHit->time();
          float timeCalib = 0;
          // Find calib
          map<int,float>::const_iterator calibMapItr = hashToCalibMapEB_.find(*thisCryItr);
          if(calibMapItr!=hashToCalibMapEB_.end())
          {
            timeCalib = calibMapItr->second;
            if(applyCalibration_)
              thisCryTime+=timeCalib;
          }
          else if (applyCalibration_)
            continue; // reject uncalibrated crys

          diffBetweenCrysSameTTHist_->Fill(thisCryTime-firstCryTime);
          if(fabs(timeCalib1-timeCalib)<0.5 && timeCalib!=0 && timeCalib1!=0)
            diffBetweenCrysSameTTSameTCalibHist_->Fill(thisCryTime-firstCryTime);
          if(fillEnergyBinHist)
            deltaBetCrys_[thisEnergyBin]->Fill(thisCryTime-firstCryTime);
        }
      }
    }
  }
  //XXX: Figure out the expected time
   double averagetimeEB = 0.0;
   int numberinaveEB = 0;
   for(EcalUncalibratedRecHitCollection::const_iterator ithit = EBUncalibHits->begin(); ithit != EBUncalibHits->end(); ++ithit)
   {
     EBDetId anid(ithit->id()); 
     EcalElectronicsId elecId = ecalElectronicsMap_->getElectronicsId(anid);
     //int DCCid = elecId.dccId();
     //int SMind = anid.ic();

     if(ithit->chi2()> -1. && ithit->chi2()<10000. && ithit->amplitude()> 25 )// make sure fit has converged 
     {
       double extrajit = timecorr(geometry_pEB,anid);
       double mytime = ithit->jitter() + extrajit+5.0;
       averagetimeEB += mytime;
       numberinaveEB++;
     }  
   }

   std::map<int,float> cryHashesToExpectedTimesEB;
   if (numberinaveEB > 0 ) averagetimeEB /= double (numberinaveEB); 
   for(EcalUncalibratedRecHitCollection::const_iterator ithit = EBUncalibHits->begin(); ithit != EBUncalibHits->end(); ++ithit)
   {
     //XXX: 5 GeV cut (143 ADC counts)
     if(ithit->chi2()> -1. && ithit->chi2()<10000. && ithit->amplitude()> 60)//30 )// make sure fit has converged 
     {
       double extrajit = timecorr(geometry_pEB,ithit->id());
       double mytime = ithit->jitter() + extrajit+5.0;
       mytime += 5.0 - averagetimeEB;
       // mytime should now be the difference of the expected and the observed
       float timeCalib = 0;
       // Find calib
       EBDetId detId = (EBDetId) ithit->id();
       map<int,float>::const_iterator calibMapItr = hashToCalibMapEB_.find(detId.hashedIndex());
       if(calibMapItr!=hashToCalibMapEB_.end())
       {
         timeCalib = calibMapItr->second;
         if(applyCalibration_)
           mytime+=timeCalib/25.;
       }
       else
         continue; // skip uncalibrated crys

       mytime-=5;
       mytime*=25;
       // EB- hist
       if(detId.ieta() < 0)
         EBMinusMeasuredExpectedHist->Fill(mytime);
       else if(detId.ieta() > 0)
         EBPlusMeasuredExpectedHist->Fill(mytime);

       if(detId.ieta()==-64)
         EBMeasuredExpectedMagicRingHist->Fill(mytime);
       if(detId.ieta() >= 58 && detId.ieta() <= 79)
         EBMeasuredExpectedAbsTime3nsHist->Fill(mytime);

       EBMeasuredExpectedHist->Fill(mytime);
       if(mytime < -3)
       {
         EBMeasExpPeakMap->Fill(detId.iphi(),detId.ieta());
       }
     }
   }
  //debug
  std::cout << "End of analyze()" << std::endl;

//  // Loop over the energy-binned hists
//  for(int i=0; i < numEnergyBins; ++i)
//  {
//    //debug
//    //cout << "getting first cry iter/time" << endl;
//    if(hitsInEnergyBin[i].size() < 2)
//      continue;
//    vector<EcalRecHit>::const_iterator firstCryItr = hitsInEnergyBin[i].begin();
//    float firstCryTime = 25*firstCryItr->time();
//    if(applyCalibration_)
//    {
//      map<int,float>::const_iterator calibMapItr = hashToCalibMapEB_.find(EBDetId(firstCryItr->id()).hashedIndex());
//      if(calibMapItr!=hashToCalibMapEB_.end())
//        firstCryTime+=calibMapItr->second;
//    }
//    
//    for(vector<EcalRecHit>::const_iterator thisCryItr = firstCryItr+1;
//        thisCryItr != hitsInEnergyBin[i].end(); ++thisCryItr)
//    {
//      float thisCryTime = 25*thisCryItr->time();
//      if(applyCalibration_)
//      {
//        map<int,float>::const_iterator calibMapItr2 = hashToCalibMapEB_.find(EBDetId(thisCryItr->id()).hashedIndex());
//        if(calibMapItr2!=hashToCalibMapEB_.end())
//          thisCryTime+=calibMapItr2->second;
//      }
//      
//      deltaBetCrys_[i]->Fill(thisCryTime-firstCryTime);
//    }
//  }



    
 //   vector<int>::const_iterator vecItr = thisEtaVector.begin();
 //   int firstCryHash = (*vecItr);
 //   EBDetId firstDet = EBDetId::unhashIndex(firstCryHash);
 //   EcalElectronicsId firstElecId = ecalElectronicsMap_->getElectronicsId(firstDet);
 //   int firstFEDid = 600+firstElecId.dccId();
 //   int firstTTid = firstElecId.towerId();
 //   EBRecHitCollection::const_iterator firstHit = EBhits.find(firstDet);
 //   if(firstHit==EBhits.end())
 //     cout << "Error: This shouldn't happen..." << endl;
 //   map<int,float>::const_iterator calibMapItr = hashToCalibMapEB_.find(firstCryHash);
 //   float timeCalib1 = 0;
 //   if(calibMapItr!=hashToCalibMapEB_.end())
 //     timeCalib1 = calibMapItr->second();
 //   float firstCryTime = applyCalibration_ ? firstHit->time()+timeCalib1 : firstHit->time();
 //   // Loop over the other crys in the eta bin
 //   for(vecItr+=1;vecItr != thisEtaVector.end(); ++vecItr)
 //   {
 //     EBDetId currentDet = EBDetId::unhashIndex(*vecItr);
 //     EcalElectronicsId currentElecId = ecalElectronicsMap_->getElectronicsId(firstDet);
 //     int currentFEDid = 600+currentElecId.dccId();
 //     int currentTTid = currentElecId.towerId();
 //     if(currentFEDid==firstFEDid && currentTTId==firstTTid)
 //     {
 //       EBRecHitCollection::const_iterator currentHit = EBhits.find(currentDet);
 //       if(currentHit==EBhits.end())
 //         cout << "Error: This shouldn't happen (2)..." << endl;
 //       map<int,float>::const_iterator calibMapItr2 = hashToCalibMapEB_.find(*vecItr);
 //       float timeCalib2 = 0;
 //       if(calibMapItr2!=hashToCalibMapEB_.end())
 //         timeCalib2 = calibMapItr2->second();
 //       float currentCryTime = applyCalibration_ ? currentHit->time()+timeCalib2 : currentHit->time();
 //       
 //       diffBetweenCrysSameTTHist_->Fill(currentCryTime-firstCryTime);
 //     }

 //   }
 // }
    

}

// -------------- read calib file ------------------------------
bool EcalTimeCalibAnalyzer::readCalibrationFile()
{
  ifstream calibFile;
  std::cout << "Attempting to open file: " << infile_ << std::endl;
  calibFile.open(infile_.c_str(),ifstream::in);
  if(calibFile.good())
  {
    int hashIndex;
    double calibConst, calibConstErr;
    std::string subDet;

    while(calibFile.good())
    { 
      calibFile >>  subDet >> hashIndex >> calibConst >> calibConstErr;
      if(subDet=="EB")
      {
        hashToCalibMapEB_.insert(std::make_pair<int,float>(hashIndex,calibConst));
        hashToCalibErrMapEB_.insert(std::make_pair<int,float>(hashIndex,calibConstErr));
      }
      else if(subDet=="EE")
      {
        hashToCalibMapEE_.insert(std::make_pair<int,float>(hashIndex,calibConst));
        hashToCalibErrMapEE_.insert(std::make_pair<int,float>(hashIndex,calibConstErr));
        //EEDetId det = EEDetId::unhashIndex(hashIndex);
        //EcalElectronicsId elecId = ecalElectronicsMap_->getElectronicsId(det);
        //FEDid = 600+elecId.dccId();
      }
      else
        std::cout << "Couldn't find in EB/EE hash: " << hashIndex << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: Calib file not opened." << std::endl;
    return false;
  }

  return true;
}

// ------------ method called once each job just before starting event loop  ------------
void 
EcalTimeCalibAnalyzer::beginJob(const edm::EventSetup& c)
{
  edm::ESHandle< EcalElectronicsMapping > handle;
  c.get< EcalMappingRcd >().get(handle);
  ecalElectronicsMap_ = handle.product();

}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalTimeCalibAnalyzer::endJob() {
//  outfile_ = new TFile("ecalTimeCalibGraphs.root","RECREATE");
//  fedAvgProfile_->Write();
//  outfile_->Close();
  std::cout << "Endjob()" << std::endl;
}

// ------------ time correction ----------------------------------------------------------
double EcalTimeCalibAnalyzer::timecorr(const CaloSubdetectorGeometry *geometry_p, DetId id)
{
   double time = 0.0;

   bool inEB = true;
   if ((id.det() == DetId::Ecal) && (id.subdetId() == EcalEndcap)) {
      inEB = false;
   }
   
   const CaloCellGeometry *thisCell = geometry_p->getGeometry(id);
   GlobalPoint position = thisCell->getPosition();
   
   double speedlight = 0.299792458; //in meters/ns
   
   double EBradius_ = 1.4;
   
   double z = position.z()/100.;
   //Correct Ecal IP readout time assumption
   if (inEB){
   
      int ieta = (EBDetId(id)).ieta() ;

      //eta2zmap_[ieta]=z;
	  //double zz=0.0;
	  /*
      if (ieta > 65 )  zz=5.188213395;
	  else if (ieta > 45 )  zz=2.192428069;
	  else if (ieta > 25 )  zz=0.756752107;
	  else if (ieta > 1 ) zz=0.088368264;
	  else if (ieta > -26 )  zz=0.088368264;
	  else if (ieta > -45 )  zz=0.756752107;
	  else if (ieta > -65 ) zz=2.192428069;
	  else zz=5.188213395;
	  */
	  /*
	  if (ieta > 65 )  zz=5.06880196;
	  else if (ieta > 45 )  zz=2.08167184;
	  else if (ieta > 25 )  zz=0.86397025;
	  else if (ieta > 1 ) zz=0.088368264;
	  else if (ieta > -26 )  zz=0.088368264;
	  else if (ieta > -45 )  zz=0.86397025;
	  else if (ieta > -65 ) zz=2.08167184;
	  else zz=5.06880196;
          */
      double change = (pow(EBradius_*EBradius_+z*z,0.5)-EBradius_)/speedlight;
      ///double change = (pow(EBradius_*EBradius_+zz,0.5)-EBradius_)/speedlight;
	  time += change;
	  
	  //std::cout << " Woohoo... z is " << z << " ieta is " << (EBDetId(id)).ieta() << std::endl;
	  //std::cout << " Subtracting " << change << std::endl;
   }
   
   if (!inEB){
      double x = position.x()/100.;
      double y = position.y()/100.;
	  double change = (pow(x*x+y*y+z*z,0.5)-EBradius_)/speedlight;
	  //double change = (pow(z*z,0.5)-EBradius_)/speedlight; //Assuming they are all the same length...
	  time += change; //Take this out for the time being
	  
	  //std::cout << " Woohoo... z is " << z << " ieta is " << (EBDetId(id)).ieta() << std::endl;
	  //std::cout << " Subtracting " << change << std::endl;
   }
   ///speedlight = (0.299792458*(1.0-.08));
   //Correct out the BH or Beam-shot assumption
   //XXX: ALWAYS USING PLUS CORRECTION HERE
   bool bhplus_ = true;
      time += ((bhplus_) ? (z/speedlight) :  (-z/speedlight) );
	  //std::cout << " Adding " << z/speedlight << std::endl;


   return (time/25.-1.5);
   //XXX: Changed by SIC, June 25 2009
   //return (time/25.);
}

//========================================================================
std::string
EcalTimeCalibAnalyzer::doubleToString(double num)
//========================================================================
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str());
}


//define this as a plug-in
DEFINE_FWK_MODULE(EcalTimeCalibAnalyzer);
