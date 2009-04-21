/**\class TestBeamTimingAnalyzer

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// $Id: TestBeamTimingAnalyzer.cc,v 1.12 2009/02/03 21:49:35 scooper Exp $
//
//

#include "Analyzers/TestBeamTimingAnalyzer/interface/TestBeamTimingAnalyzer.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "TBDataFormats/EcalTBObjects/interface/EcalTBHodoscopeRecInfo.h"
#include "TBDataFormats/EcalTBObjects/interface/EcalTBTDCRecInfo.h"
#include "TBDataFormats/EcalTBObjects/interface/EcalTBEventHeader.h"
//SIC producer
#include "ESProducers/EcalTimingCorrectionESProducer/interface/EcalTimingCorrection.h"
#include "ESProducers/EcalTimingCorrectionESProducer/interface/EcalTimingCorrectionRcd.h"
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "PhysicsTools/Utilities/interface/deltaR.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

//#include<fstream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//


//========================================================================
TestBeamTimingAnalyzer::TestBeamTimingAnalyzer( const edm::ParameterSet& iConfig ) : xtalInBeam_(0)
//========================================================================
{
   //now do what ever initialization is needed
   rootfile_          = iConfig.getUntrackedParameter<std::string>("rootfile","ecalSimpleTBanalysis.root");
   calibrationFile_   = iConfig.getUntrackedParameter<std::string>("calibrationFile","");
   digiCollection_     = iConfig.getParameter<std::string>("digiCollection");
   digiProducer_       = iConfig.getParameter<std::string>("digiProducer");
   hitCollection_     = iConfig.getParameter<std::string>("hitCollection");
   hitProducer_       = iConfig.getParameter<std::string>("hitProducer");
   hodoRecInfoCollection_     = iConfig.getParameter<std::string>("hodoRecInfoCollection");
   hodoRecInfoProducer_       = iConfig.getParameter<std::string>("hodoRecInfoProducer");
   tdcRecInfoCollection_     = iConfig.getParameter<std::string>("tdcRecInfoCollection");
   tdcRecInfoProducer_       = iConfig.getParameter<std::string>("tdcRecInfoProducer");
   eventHeaderCollection_     = iConfig.getParameter<std::string>("eventHeaderCollection");
   eventHeaderProducer_       = iConfig.getParameter<std::string>("eventHeaderProducer");
   minimumAmpForCalib_       = iConfig.getParameter<double>("minimumAmpForCalib");
   useOddEvents_          = iConfig.getParameter<bool>("useOddEvents");


   std::cout << "TestBeamTimingAnalyzer: fetching hitCollection: " << hitCollection_.c_str()
	<< " produced by " << hitProducer_.c_str() << std::endl;

   if(calibrationFile_=="")
   {
     std::cout << "No calib file specified." << std::endl;
     return;
   }

   std::cout << "Reading file with calibs: " << calibrationFile_ << std::endl;
   ifstream calibFile(edm::FileInPath(calibrationFile_).fullPath().c_str());
   if(calibFile.is_open())
   {
     int hashIndex;
     double calibConst;
     while(calibFile.good())
     {
       calibFile >> hashIndex >> calibConst;
       timingCalibMap_.insert(std::make_pair(hashIndex,calibConst));
     }
   }
   else
   {
     throw cms::Exception("FileNotOpened") << "ERROR: Calib file not opened." << std::endl;
   }
}


//========================================================================
TestBeamTimingAnalyzer::~TestBeamTimingAnalyzer()
//========================================================================
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}

//========================================================================
void
TestBeamTimingAnalyzer::beginJob(const edm::EventSetup& eventSetup) {
//========================================================================

  //SIC
  edm::ESHandle< EcalElectronicsMapping > handle;
  eventSetup.get< EcalMappingRcd >().get(handle);
  ecalElectronicsMap_ = handle.product();
  edm::ESHandle<CaloGeometry> theGeometry;
  edm::ESHandle<CaloSubdetectorGeometry> theBarrelGeometry_handle;
  eventSetup.get<CaloGeometryRecord>().get( theGeometry );
  eventSetup.get<EcalBarrelGeometryRecord>().get("EcalBarrel",theBarrelGeometry_handle);
  geometry_barrel_ = &(*theBarrelGeometry_handle);
  
  names = new std::vector<std::string>();

  // Amplitude vs TDC offset
  h_ampltdc = new TH2F("h_ampltdc","Max Amplitude vs TDC offset", 100,0.,1.,1000, 0., 4000.);

  TDCValue_ = new TH1F("TDCValue","Value of TDC Offset",200,-0.5,1.5);
  recoTime_ = new TH1F("recoTime","Reconstructed time of maxEnergy hit",700,-3.5,3.5);
  deltaTime_ = new TH1F("deltaTime","Reconstructed time - TDC Offset",700,-3.5,3.5);
  distVsTDCHist_ = new TH2F("distVsTDC","DeltaR from center cry vs. TDC Offset",200,-0.5,1.5,20,0,0.35);

  char hname[50];
  char htitle[50];
  for(int i=0;i<50;++i)
  {
    sprintf(hname,"fitVsTDCTimeEnergyBin_%d",i);
    sprintf(htitle,"Fit time vs. TDC time, energy bin %d",i);
    //fitTimeVsTDC_[i] = new TH2F(hname,htitle,200,-0.5,1.5,700,-3.5,3.5);
    fitTimeVsTDC_[i] = new TH2F(hname,htitle,1000,-50,50,1000,-50,50);
    sprintf(hname,"deltaBetCrysEnergyBin_%d",i);
    sprintf(htitle,"Difference between crys, energy bin %d",i);
    deltaBetCrys_[i] = new TH1F(hname,htitle,1000,-50,50);
    sprintf(hname,"PivotCryEnergyBin_%d",i);
    sprintf(htitle,"Pivot Crystal, energy bin %d",i);
    pivotCry_[i] = new TH1F(hname,htitle,25,0,25);
    sprintf(hname,"distVsTimeEnergyBin_%d",i);
    sprintf(htitle,"DeltaR betw. crys vs. time difference, energy bin %d",i);
    distVsTimeHists_[i] = new TH2F(hname,htitle,1000,-50,50,20,0,0.35);
    sprintf(hname,"cryIndicesConsideredEnergyBin_%d",i);
    sprintf(htitle,"Index of Cry2 vs. Cry1 in deltaT graphs, energy bin %d",i);
    cryIndicesConsideredHists_[i] = new TH2F(hname,htitle,25,0,25,25,0,25);
    sprintf(hname,"deltaTvsTDCEnergyBin_%d",i);
    sprintf(htitle,"Time difference vs TDC, energy bin %d",i);
    deltaTvsTDC_[i] = new TH2F(hname,htitle,20,-0.5,1.5,1000,-50,50);
    sprintf(hname,"deltaTvsAverageTEnergyBin_%d",i);
    sprintf(htitle,"Time difference vs Average Time, energy bin %d",i);
    deltaTvsAverageT_[i] = new TH2F(hname,htitle,1000,-50,50,1000,-50,50);
    sprintf(hname,"deltaTCry2EnergyBin_%d",i);
    sprintf(htitle,"Delta T for pivot cry 2 energy bin %d",i);
    deltaTCry2EnergyBin_[i] = new TH1F(hname,htitle,1000,-50,50);
    sprintf(hname,"deltaTCry7EnergyBin_%d",i);
    sprintf(htitle,"Delta T for pivot cry 7 energy bin %d",i);
    deltaTCry7EnergyBin_[i] = new TH1F(hname,htitle,1000,-50,50);
    sprintf(hname,"recoMinusTDCCry7EnergyBin_%d",i);
    sprintf(htitle,"Reco+TDC-1 for pivot cry 7 energy bin %d",i);
    recoMinusTDCCry7EnergyBin_[i] = new TH1F(hname,htitle,1000,-50,50);
    sprintf(hname,"recoMinusTDCCry2EnergyBin_%d",i);
    sprintf(htitle,"Reco+TDC-1 for pivot cry 2 energy bin %d",i);
    recoMinusTDCCry2EnergyBin_[i] = new TH1F(hname,htitle,1000,-50,50);
    //XXX:New SIC
    sprintf(hname,"crysConsideredEtaPhiEnergyBin_%d",i);
    sprintf(htitle,"Eta/phi of cry pairs, energy bin %d",i);
    crysConsideredEtaPhiHists_[i] = new TH2F(hname,htitle,86,1,87,20,1,21);
  }
  //for(int i=0;i<25;i++)
  //{
  //  sprintf(hname,"recoTimeMinusTDCTimeCry_%d",i);
  //  sprintf(htitle,"RecoTime + TDC time -1 for cry %d",i);
  //  recoTimeMinusTDCTimeByCry_[i] = new TH1F(hname,htitle,1000,-50,50);
  //}

  deltaT813Bin8_ = new TH1F("deltaT813_bin8","Time of cry 13 - cry 8, 0.8 to 0.9 GeV",1000,-50,50);
  deltaT813Bin6_ = new TH1F("deltaT813_bin6","Time of cry 13 - cry 8, 0.6 to 0.7 GeV",1000,-50,50);
  deltaT713Bin12_ = new TH1F("deltaT713_bin12","Time of cry 13 - cry 7, 10 to 20 GeV",1000,-50,50);
  deltaT713Bin11_ = new TH1F("deltaT713_bin11","Time of cry 13 - cry 7, 5 to 10 GeV",1000,-50,50);
  
  // Reconstructed energies
  h_tableIsMoving = new TH1F("h_tableIsMoving","TableIsMoving", 100000, 0., 100000.);

  h_bprofx = new TH1F("h_bprofx","Beam Profile X",100,-20.,20.);
  h_bprofy = new TH1F("h_bprofy","Beam Profile Y",100,-20.,20.);

  h_qualx = new TH1F("h_qualx","Beam Quality X",5000,0.,5.);
  h_qualy = new TH1F("h_qualy","Beam Quality X",5000,0.,5.);

  h_slopex = new TH1F("h_slopex","Beam Slope X",500, -5e-4 , 5e-4 );
  h_slopey = new TH1F("h_slopey","Beam Slope Y",500, -5e-4 , 5e-4 );

  for (unsigned int icry=0;icry<25;icry++)
    {       
      sprintf(hname,"h_mapx_%d",icry);
      sprintf(htitle,"Max Amplitude vs X %d",icry);
      h_mapx[icry] = new TH2F(hname,htitle,80,-20,20,1000,0.,4000.);
      sprintf(hname,"h_mapy_%d",icry);
      sprintf(htitle,"Max Amplitude vs Y %d",icry);
      h_mapy[icry] = new TH2F(hname,htitle,80,-20,20,1000,0.,4000.);
    }
  //SIC
  for(int icry=0;icry<25;++icry)
  {
    sprintf(hname,"fitTimeVsTDC_Cry%d",icry);
    sprintf(htitle,"Fitter time Vs. TDC offset, cry %d",icry);
    //timingHistMap[icry] = new TH2F(hname,htitle,200,-0.5,1.5,700,-3.5,3.5);
    timingHistMap[icry] = new TH2F(hname,htitle,1000,-50,50,1000,-50,50);
    sprintf(hname,"amplitude_Cry%d",icry);
    sprintf(htitle,"Amplitude of cry %d",icry);
    amplitudeHistMap[icry] = new TH1F(hname,htitle,1500,0,1500);
    sprintf(hname,"recoTime_Cry%d",icry);
    sprintf(htitle,"Fitted time of cry %d",icry);
    timing1DHistMap[icry] = new TH1F(hname,htitle,1000,-50,50);
  }
  
  h_Shape_ = new TH2F("h_Shape_","Xtal in Beam Shape",250,0,10,350,0,3500);

  occupancyOfMaxEneCry_ = new TH2F("occupancyMaxEneCry","Occupancy of max. energy crystal",360,1,361.,172,-86,86);

}

//========================================================================
void
TestBeamTimingAnalyzer::endJob() {
//========================================================================

  TFile f(rootfile_.c_str(),"RECREATE");

  histoNames_ = new TTree("histoNames","Names of written histos");
  histoNames_->Branch("names","vector<string>",&names);
  histoNames_->Fill();
  histoNames_->Write();

  // Amplitude vs TDC offset
  h_ampltdc->Write(); 

  TDCValue_->Write();
  recoTime_->Write();
  deltaTime_->Write();
  //distVsTDCHist_->SetXTitle("TDC Offset [bx]");
  //distVsTDCHist_->Write();

  for(int i=0;i<50;i++)
  {
    fitTimeVsTDC_[i]->GetXaxis()->SetTitle("TDC Offset [ns]");
    fitTimeVsTDC_[i]->GetYaxis()->SetTitle("Fitted time [ns]");
    fitTimeVsTDC_[i]->Write();
    deltaBetCrys_[i]->GetXaxis()->SetTitle("t1 - t2 [ns]");
    deltaBetCrys_[i]->Write();
    distVsTimeHists_[i]->SetXTitle("DeltaT [ns]");
    distVsTimeHists_[i]->SetYTitle("DeltaR");
    distVsTimeHists_[i]->Write();
    //cryIndicesConsideredHists_[i]->SetXTitle("Index of Cry1 (begin)");
    //cryIndicesConsideredHists_[i]->Write();
    deltaTvsTDC_[i]->SetXTitle("TDC offset");
    deltaTvsTDC_[i]->Write();
    deltaTvsAverageT_[i]->SetXTitle("Average time");
    deltaTvsAverageT_[i]->Write();
    //pivotCry_[i]->SetXTitle("pivot cry index");
    //pivotCry_[i]->Write();
    deltaTCry2EnergyBin_[i]->SetXTitle("DeltaT");
    //deltaTCry2EnergyBin_[i]->Write();
    deltaTCry7EnergyBin_[i]->SetXTitle("DeltaT");
    //deltaTCry7EnergyBin_[i]->Write();
    //recoMinusTDCCry2EnergyBin_[i]->Write();
    //recoMinusTDCCry7EnergyBin_[i]->Write();
    crysConsideredEtaPhiHists_[i]->SetXTitle("i#eta");
    crysConsideredEtaPhiHists_[i]->SetYTitle("i#phi");
    crysConsideredEtaPhiHists_[i]->Write();
  }

  for(std::map<int,TH1F>::const_iterator mapItr = recoTimeMinusTDCTimeByCry_.begin();
      mapItr != recoTimeMinusTDCTimeByCry_.end(); ++mapItr)
  {
    //mapItr->second.SetXTitle("reco + TDC -1 [ns]");
    mapItr->second.Write();
  }
  
  //deltaT713Bin12_->Write();
  //deltaT713Bin11_->Write();
  //deltaT813Bin8_->Write();
  //deltaT813Bin6_->Write();
  
  //// Reconstructed energies
  h_bprofx->Write(); 
  h_bprofy->Write(); 
  h_qualx->Write(); 
  h_qualy->Write(); 
  h_slopex->Write(); 
  h_slopey->Write(); 
  h_Shape_->Write();
  for (unsigned int icry=0;icry<25;icry++)
  {       
    h_mapx[icry]->Write(); 
    h_mapy[icry]->Write(); 
    timingHistMap[icry]->GetXaxis()->SetTitle("TDC Offset (ns)");
    timingHistMap[icry]->GetYaxis()->SetTitle("Fitted time (ns)");
    timingHistMap[icry]->Write();
    amplitudeHistMap[icry]->SetXTitle("ADC");
    amplitudeHistMap[icry]->Write();
    timing1DHistMap[icry]->SetXTitle("reco time [ns]");
    timing1DHistMap[icry]->Write();
  }
  h_tableIsMoving->Write();
  occupancyOfMaxEneCry_->GetXaxis()->SetTitle("iphi");
  occupancyOfMaxEneCry_->GetYaxis()->SetTitle("ieta");
  occupancyOfMaxEneCry_->Write();

  f.Close();
}

//
// member functions
//

//========================================================================
void
TestBeamTimingAnalyzer::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
  //========================================================================

  int ievt = iEvent.id().event();
  if(useOddEvents_)  // Use only odd-numbered events (skip evens)
  {
    if(ievt%2 == 0)
      return;
  }
  else if(ievt%2 != 0) // Otherwise use only even-numbered events (skip odds)
    return;
  
  std::cout << "Examining event " << ievt << std::endl;
  
  using namespace edm;
  using namespace cms;
  using namespace std;

  //TODO: SIC intraTT corrections
  // fetch timing correction
  //edm::ESHandle<EcalTimingCorrection> timingCorrectionP;
  //iSetup.get<EcalTimingCorrectionRcd>().get(timingCorrectionP);
  //const EcalTimingCorrection* timingCorrection = timingCorrectionP.product();

  Handle<EBDigiCollection> pdigis;
  const EBDigiCollection* digis=0;
  iEvent.getByLabel( digiProducer_, digiCollection_,pdigis);
  if(!pdigis.isValid())
  {
    std::cerr << "Error! can't get the product " << digiCollection_.c_str() << std::endl;
    return;
  }
  digis = pdigis.product(); // get a ptr to the product

  // fetch the digis and compute signal amplitude
  Handle<EBUncalibratedRecHitCollection> phits;
  const EBUncalibratedRecHitCollection* hits=0;
  iEvent.getByLabel( hitProducer_, hitCollection_,phits);
  if(!phits.isValid())
  {
    std::cerr << "Error! can't get the product " << hitCollection_.c_str() << std::endl;
    return;
  }
  hits = phits.product(); // get a ptr to the product

  Handle<EcalTBHodoscopeRecInfo> pHodo;
  const EcalTBHodoscopeRecInfo* recHodo=0;
  iEvent.getByLabel( hodoRecInfoProducer_, hodoRecInfoCollection_, pHodo);
  if(!pHodo.isValid())
  {
    std::cerr << "Error! can't get the product " << hodoRecInfoCollection_.c_str() << std::endl;
    return;
  }
  recHodo = pHodo.product(); // get a ptr to the product

  Handle<EcalTBTDCRecInfo> pTDC;
  const EcalTBTDCRecInfo* recTDC=0;
  iEvent.getByLabel( tdcRecInfoProducer_, tdcRecInfoCollection_, pTDC);
  if(!pTDC.isValid())
  {
    std::cerr << "Error! can't get the product " << tdcRecInfoCollection_.c_str() << std::endl;
    return;
  }
  recTDC = pTDC.product(); // get a ptr to the product

  Handle<EcalTBEventHeader> pEventHeader;
  const EcalTBEventHeader* evtHeader=0;
  iEvent.getByLabel( eventHeaderProducer_ , pEventHeader );
  if(!pEventHeader.isValid())
  {
    std::cerr << "Error! can't get the product " << eventHeaderProducer_.c_str() << std::endl;
    return;
  }
  evtHeader = pEventHeader.product(); // get a ptr to the product

  if (hits->size() == 0)
    return;

  if (evtHeader->tableIsMoving())
    h_tableIsMoving->Fill(evtHeader->eventNumber());

  //XXX:  I DON'T CARE ABOUT THE XTAL IN BEAM!  ONLY THE MAX ENERGY XTAL!
  //// Crystal hit by beam
  //if (xtalInBeam_.null())
  //  {
  //    xtalInBeam_ = EBDetId(1,evtHeader->crystalInBeam(),EBDetId::SMCRYSTALMODE);
  //    std::cout<< "Xtal In Beam is " << xtalInBeam_.ic() << std::endl;
  //  }
  //else if (xtalInBeam_ != EBDetId(1,evtHeader->crystalInBeam(),EBDetId::SMCRYSTALMODE))
  //  return;

  if (evtHeader->tableIsMoving())
    return;

  //XXX:  Here I'm cutting to see only the edges of the TDC range.
  //if(recTDC->offset() < 0.9 && recTDC->offset() > 0.1)
  //  return;
    
  EBDetId maxHitId(0); 
  float maxHit= -999999.;
  float maxHitTime=-99999;

  for(EBUncalibratedRecHitCollection::const_iterator ithit = hits->begin(); ithit != hits->end(); ++ithit) 
  {
    if (ithit->amplitude()>=maxHit)
    {
      maxHit=ithit->amplitude();
      maxHitTime = ithit->jitter();
      maxHitId=ithit->id();
    }
  }

  //    if (maxHitId==EBDetId(0))
  //      return;

  //   EBDetId maxHitId(1,704,EBDetId::SMCRYSTALMODE); 

  //Find EBDetId in a 5x5 Matrix (to be substituted by the Selector code)
  // Something like 
  // EBFixedWindowSelector<EcalUncalibratedRecHit> Simple5x5Matrix(hits,maxHitId,5,5);
  // std::vector<EcalUncalibratedRecHit> Energies5x5 = Simple5x5Matrix.getHits();

  // SIC
  // Fill occupancy for max energy cry
  occupancyOfMaxEneCry_->Fill(maxHitId.iphi(),maxHitId.ieta());

  //   //TODO: SIC mod: timing calibrations by crystal; note that these should be in BX

  //SIC PLOTS
  int numEnergyBins = 40;  // MUST change next three array values by hand
  double energyBins[40] = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1,2,3,4,5,7.5,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180};
  double ampBins[40];
  map<int,vector<int> > histBinToCrysMap;  //histogram bin and vector of crys in that bin
  vector<int> cryVector;

  double avgTime = -9999;
  int numCrysInAvg = 0;
  for(int i=32;i<numEnergyBins;++i)
    energyBins[i]=0;
  for(int i=0;i<numEnergyBins;++i)
  {
    ampBins[i] = energyBins[i]/0.035;
    string title = "Fit time vs. TDC time, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    fitTimeVsTDC_[i]->SetTitle(title.c_str());
    title = "Difference in time between crys, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    deltaBetCrys_[i]->SetTitle(title.c_str());
    title = "DeltaR vs. time difference, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    distVsTimeHists_[i]->SetTitle(title.c_str());
    title = "Index of cry2 vs. cry1, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    cryIndicesConsideredHists_[i]->SetTitle(title.c_str());
    title = "Pivot cry, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    pivotCry_[i]->SetTitle(title.c_str());
    title = "DeltaT cry 2, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    deltaTCry2EnergyBin_[i]->SetTitle(title.c_str());
    title = "DeltaT cry 7, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    deltaTCry7EnergyBin_[i]->SetTitle(title.c_str());
    title = "Reco+TDC-1 cry 7, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    recoMinusTDCCry7EnergyBin_[i]->SetTitle(title.c_str());
    title = "Reco+TDC-1 cry 2, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    recoMinusTDCCry2EnergyBin_[i]->SetTitle(title.c_str());
    title = "Eta/phi of cry pairs, "+doubleToString(energyBins[i])+
      " GeV to "+doubleToString(energyBins[i+1])+" GeV";
    crysConsideredEtaPhiHists_[i]->SetTitle(title.c_str());
  }
  //const CaloCellGeometry* centerCell = geometry_barrel_->getGeometry(Xtals5x5[12]);
  //GlobalPoint centerCryPt = (dynamic_cast <const TruncatedPyramid *>  (centerCell))->getPosition(0);
  //double centerCryTime = timing[12];
  // figure out which crys are in which energy bins; compute avgTime
  for(EcalUncalibratedRecHitCollection::const_iterator uncalibHitItr = hits->begin();
      uncalibHitItr != hits->end(); ++uncalibHitItr) 
  {
    cryVector.clear();
    if(uncalibHitItr->chi2() > 0)  // Imposing chi2 cut
    {
      int cryHash = ((EBDetId)uncalibHitItr->id()).hashedIndex();
      cryVector.push_back(cryHash);
      double cryTime = 25*uncalibHitItr->jitter();
      map<int,double>::const_iterator calibItr = timingCalibMap_.find(cryHash);
      if(calibItr != timingCalibMap_.end())  // uncalibRecHit hashedIndex is found in timingCalibMap from calibFile
        cryTime-=calibItr->second;
      else if(timingCalibMap_.size() > 0)  // hashedIndex not found, but there were calibs applied
      {
        cryTime-=1000;  // this is an uncalibrated cry
        timingCalibMap_.insert(std::make_pair(cryHash,-1000));  // and put it in the map
      }
      
      double cryAmp = uncalibHitItr->amplitude();
      //timing1DHistMap[icry]->Fill(25*cryTime);
      //amplitudeHistMap[icry]->Fill(cryAmp);
      // Fill TDC hists
      if(recTDC && cryAmp > minimumAmpForCalib_)  //TODO: AmpCut valid for EB only!
      {
        //const CaloCellGeometry *cell1 = geometry_barrel_->getGeometry(uncalibHitItr->id());
        //GlobalPoint p1 = (dynamic_cast <const TruncatedPyramid *>  (cell1))->getPosition(0);
        //double dist = reco::deltaR(p1,centerCryPt);
        //distVsTDCHist_->Fill(recTDC->offset(),dist);
        //timingHistMap[icry]->Fill(25*recTDC->offset(),25*cryTime);
        //TODO: Now only crys with timing > 15 ns or < 10 ns are allowed in the plot
        //    (excluding ripple region)
        //if(25*timing[icry]>15 || 25*timing[icry]<10)
        string hname = "recoTimeMinusTDCTimeCry_";
        hname+=intToString(cryHash);
        string htitle = "RecoTime + TDC time - i for cry ";
        htitle+=intToString(cryHash);
        TH1F temp(hname.c_str(),htitle.c_str(),1000,-50,50);
        pair<map<int,TH1F >::iterator,bool> retVal;
        retVal = recoTimeMinusTDCTimeByCry_.insert(make_pair(cryHash,temp));
        if(retVal.second) //insertion was done
          names->push_back(hname);
        map<int,TH1F>::iterator iter;
        iter = recoTimeMinusTDCTimeByCry_.find(cryHash);
        if(iter != recoTimeMinusTDCTimeByCry_.end())
        {
          //For SM6 data
          //iter->second.Fill(25*(cryTime+recTDC->offset()-1));
          //For SM13 data
          //iter->second.Fill(cryTime+25*recTDC->offset()-15);
          //For SM12 data
          iter->second.Fill(cryTime+25*recTDC->offset()-17);
        }
      }
      //Bin by energy
      int histNum = -1;
      if(cryAmp > 0)
      {
        for(int i=0;i<numEnergyBins-1;++i)
        {
          if(cryAmp >= ampBins[i] && cryAmp < ampBins[i+1])
            histNum = i;
        }
        //if(histNum==-1) continue;
        pair<map<int,vector<int> >::iterator,bool> ret;
        ret = histBinToCrysMap.insert(make_pair(histNum,cryVector));
        if(!ret.second)  // no insert done; element exists
        {
          //debug
          //cout << "Element for bin " << histNum << " exists; adding cry " << icry << endl;
          map<int,vector<int> >::iterator itr;
          itr = histBinToCrysMap.find(histNum);
          if(itr != histBinToCrysMap.end())
            itr->second.push_back(((EBDetId)uncalibHitItr->id()).hashedIndex());
        } 
        avgTime+=cryTime;
        numCrysInAvg++;
      }
    }
  }
  if(numCrysInAvg > 0) 
    avgTime/=numCrysInAvg;

  //Fill hists involving TDC info
  if (recTDC)
  {
    h_ampltdc->Fill(recTDC->offset(),maxHit);
    TDCValue_->Fill(recTDC->offset());
    recoTime_->Fill(maxHitTime);
    deltaTime_->Fill(maxHitTime-recTDC->offset());
    //fitTimeVsTDC_->Fill(recTDC->offset(),maxHitTime);
  }

  for(map<int,vector<int> >::const_iterator itr = histBinToCrysMap.begin();
      itr != histBinToCrysMap.end(); ++itr)
  {
    //debug
    //cout << "ENergy bin: " << itr->first << endl;
    vector<int> crys = itr->second;
    EBDetId firstHitInBinId = EBDetId::unhashIndex(*crys.begin());
    int tower1 = ecalElectronicsMap_->getElectronicsId(firstHitInBinId).towerId();
    // Find correct uncalibRecHit
    EBUncalibratedRecHitCollection::const_iterator firstHitInBin = hits->begin();
    while(firstHitInBin != hits->end() && firstHitInBin->id() != firstHitInBinId)
      firstHitInBin++;
    if(firstHitInBin==hits->end())
    {
      std::cout << "ERROR: Cannot find uncalibRecHit matching Id of first cry (hash " << *crys.begin() 
        << ") in energy bin for hist number: "
        << itr->first << std::endl;
      continue;
    }
    const CaloCellGeometry *cell2 = geometry_barrel_->getGeometry(firstHitInBinId);
    GlobalPoint p2 = (dynamic_cast <const TruncatedPyramid *>  (cell2))->getPosition(0);
    double firstHitTime = 25*firstHitInBin->jitter();
    map<int,double>::const_iterator calibItr = timingCalibMap_.find(firstHitInBinId.hashedIndex());
    if(calibItr != timingCalibMap_.end())
      firstHitTime-=calibItr->second;

    // Loop over crys in the same energy bin
    for(vector<int>::const_iterator vecItr = crys.begin(); vecItr != crys.end(); ++vecItr)
    {
      //DEBUG
      //std::cout << "LOOP OVER CYS IN SAME E BIN" << std::endl;
      EBDetId currentHitInBinId = EBDetId::unhashIndex(*vecItr);
      int tower2 = ecalElectronicsMap_->getElectronicsId(currentHitInBinId).towerId();
      // Find correct uncalibRecHit
      EBUncalibratedRecHitCollection::const_iterator currentHitInBin = hits->begin();
      while(currentHitInBin != hits->end() && currentHitInBin->id() != currentHitInBinId)
        currentHitInBin++;
      //DEBUG
      //std::cout << "FOUND CORRECT UCALIB HIT" << std::endl;
      if(currentHitInBin==hits->end())
      {
        std::cout << "ERROR: Cannot find uncalibRecHit matching Id of a cry (hash " << *vecItr << 
          ") in energy bin for hist number: "
          << itr->first << std::endl;
        continue;
      }
      double secondHitTime = 25*currentHitInBin->jitter();
      map<int,double>::const_iterator calibItr = timingCalibMap_.find(currentHitInBinId.hashedIndex());
      if(calibItr != timingCalibMap_.end())
        secondHitTime-=calibItr->second;

      fitTimeVsTDC_[itr->first]->Fill(25*recTDC->offset(),secondHitTime);
      //TODO:  Now I have turned ON the sameTT requirement!
      if(vecItr != crys.begin() && tower2==tower1) //Require that they are in the same tower
      {
        //debug
        //cout << "Crystal " << Xtals5x5[*vecItr] << " is in the same tower as crystal: " << Xtals5x5[*crys.begin()]
        //  << endl;
        double statCorrection = sqrt(2);
        deltaBetCrys_[itr->first]->Fill((secondHitTime-firstHitTime)/statCorrection);
        const CaloCellGeometry *cell1 = geometry_barrel_->getGeometry(currentHitInBinId);
        GlobalPoint p1 = (dynamic_cast <const TruncatedPyramid *>  (cell1))->getPosition(0);
        double dist = reco::deltaR(p1,p2);
        distVsTimeHists_[itr->first]->Fill((secondHitTime-firstHitTime)/statCorrection,dist);
        //cryIndicesConsideredHists_[itr->first]->Fill(*crys.begin(),*vecItr);
        crysConsideredEtaPhiHists_[itr->first]->Fill(firstHitInBinId.ieta(),firstHitInBinId.iphi());
        crysConsideredEtaPhiHists_[itr->first]->Fill(currentHitInBinId.ieta(),currentHitInBinId.iphi());
        deltaTvsTDC_[itr->first]->Fill(recTDC->offset(),(secondHitTime-firstHitTime)/statCorrection);
        deltaTvsAverageT_[itr->first]->Fill(12.5*(secondHitTime+firstHitTime),secondHitTime-firstHitTime);
        //pivotCry_[itr->first]->Fill(*crys.begin());

      }
    }
  }

  if (recHodo)
  {
    float x=recHodo->posX();
    float y=recHodo->posY();
    float xslope=recHodo->slopeX();
    float yslope=recHodo->slopeY();
    float xqual=recHodo->qualX();
    float yqual=recHodo->qualY();

    //Filling beam profiles
    h_bprofx->Fill(x);
    h_bprofy->Fill(y);
    h_qualx->Fill(xqual);
    h_qualy->Fill(yqual);
    h_slopex->Fill(xslope);
    h_slopey->Fill(yslope);

  }
}

//========================================================================
std::string
TestBeamTimingAnalyzer::doubleToString(double num)
//========================================================================
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str());
}


//========================================================================
std::string
TestBeamTimingAnalyzer::intToString(int num)
//========================================================================
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str());
}


DEFINE_FWK_MODULE(TestBeamTimingAnalyzer);
