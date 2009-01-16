/**\class TestBeamTimingAnalyzer

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// $Id: TestBeamTimingAnalyzer.cc,v 1.7 2008/12/19 17:47:44 scooper Exp $
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


   std::cout << "TestBeamTimingAnalyzer: fetching hitCollection: " << hitCollection_.c_str()
	<< " produced by " << hitProducer_.c_str() << std::endl;

}


//========================================================================
TestBeamTimingAnalyzer::~TestBeamTimingAnalyzer()
//========================================================================
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  // Amplitude vs TDC offset
//   if (h_ampltdc)
//   delete h_ampltdc;
  
//   // Reconstructed energies
//   delete h_e1x1;
//   delete h_e3x3; 
//   delete h_e5x5; 
  
//   delete h_bprofx; 
//   delete h_bprofy; 
  
//   delete h_qualx; 
//   delete h_qualy; 
  
//   delete h_slopex; 
//   delete h_slopey; 
  
//   delete h_mapx; 
//   delete h_mapy; 

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
  }
  for(int i=0;i<25;i++)
  {
    sprintf(hname,"recoTimeMinusTDCTimeCry_%d",i);
    sprintf(htitle,"RecoTime + TDC time -1 for cry %d",i);
    recoTimeMinusTDCTimeByCry_[i] = new TH1F(hname,htitle,1000,-50,50);
  }

  deltaT813Bin8_ = new TH1F("deltaT813_bin8","Time of cry 13 - cry 8, 0.8 to 0.9 GeV",1000,-50,50);
  deltaT813Bin6_ = new TH1F("deltaT813_bin6","Time of cry 13 - cry 8, 0.6 to 0.7 GeV",1000,-50,50);
  deltaT713Bin12_ = new TH1F("deltaT713_bin12","Time of cry 13 - cry 7, 10 to 20 GeV",1000,-50,50);
  deltaT713Bin11_ = new TH1F("deltaT713_bin11","Time of cry 13 - cry 7, 5 to 10 GeV",1000,-50,50);
  
  // Reconstructed energies
  h_tableIsMoving = new TH1F("h_tableIsMoving","TableIsMoving", 100000, 0., 100000.);

  h_e1x1 = new TH1F("h_e1x1","E1x1 energy", 1000, 0., 4000.);
  h_e3x3 = new TH1F("h_e3x3","E3x3 energy", 1000, 0., 4000.);
  h_e5x5 = new TH1F("h_e5x5","E5x5 energy", 1000, 0., 4000.);

  h_e1x1_center = new TH1F("h_e1x1_center","E1x1 energy", 1000, 0., 4000.);
  h_e3x3_center = new TH1F("h_e3x3_center","E3x3 energy", 1000, 0., 4000.);
  h_e5x5_center = new TH1F("h_e5x5_center","E5x5 energy", 1000, 0., 4000.);

  h_e1e9 = new TH1F("h_e1e9","E1/E9 ratio", 600, 0., 1.2);
  h_e1e25 = new TH1F("h_e1e25","E1/E25 ratio", 600, 0., 1.2);
  h_e9e25 = new TH1F("h_e9e25","E9/E25 ratio", 600, 0., 1.2);

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
  
  h_e1e9_mapx = new TH2F("h_e1e9_mapx","E1/E9 vs X",80,-20,20,600,0.,1.2);
  h_e1e9_mapy = new TH2F("h_e1e9_mapy","E1/E9 vs Y",80,-20,20,600,0.,1.2);

  h_e1e25_mapx = new TH2F("h_e1e25_mapx","E1/E25 vs X",80,-20,20,600,0.,1.2);
  h_e1e25_mapy = new TH2F("h_e1e25_mapy","E1/E25 vs Y",80,-20,20,600,0.,1.2);

  h_e9e25_mapx = new TH2F("h_e9e25_mapx","E9/E25 vs X",80,-20,20,600,0.,1.2);
  h_e9e25_mapy = new TH2F("h_e9e25_mapy","E9/E25 vs Y",80,-20,20,600,0.,1.2);

  h_Shape_ = new TH2F("h_Shape_","Xtal in Beam Shape",250,0,10,350,0,3500);

  occupancyOfMaxEneCry_ = new TH2F("occupancyMaxEneCry","Occupancy of max. energy crystal",360,1,361.,172,-86,86);

}

//========================================================================
void
TestBeamTimingAnalyzer::endJob() {
//========================================================================

  TFile f(rootfile_.c_str(),"RECREATE");

  // Amplitude vs TDC offset
  h_ampltdc->Write(); 

  TDCValue_->Write();
  recoTime_->Write();
  deltaTime_->Write();
  distVsTDCHist_->SetXTitle("TDC Offset [bx]");
  distVsTDCHist_->Write();

  for(int i=0;i<50;i++)
  {
    fitTimeVsTDC_[i]->GetXaxis()->SetTitle("TDC Offset [ns]");
    fitTimeVsTDC_[i]->GetYaxis()->SetTitle("Fitted time [ns]");
    fitTimeVsTDC_[i]->Write();
    deltaBetCrys_[i]->GetXaxis()->SetTitle("Cry time - avg. (ns)");
    deltaBetCrys_[i]->Write();
    distVsTimeHists_[i]->SetXTitle("DeltaT [ns]");
    distVsTimeHists_[i]->SetYTitle("DeltaR");
    distVsTimeHists_[i]->Write();
    cryIndicesConsideredHists_[i]->SetXTitle("Index of Cry1 (begin)");
    cryIndicesConsideredHists_[i]->Write();
    deltaTvsTDC_[i]->SetXTitle("TDC offset");
    deltaTvsTDC_[i]->Write();
    deltaTvsAverageT_[i]->SetXTitle("Average time");
    deltaTvsAverageT_[i]->Write();
    pivotCry_[i]->SetXTitle("pivot cry index");
    pivotCry_[i]->Write();
    deltaTCry2EnergyBin_[i]->SetXTitle("DeltaT");
    deltaTCry2EnergyBin_[i]->Write();
    deltaTCry7EnergyBin_[i]->SetXTitle("DeltaT");
    deltaTCry7EnergyBin_[i]->Write();
    recoMinusTDCCry2EnergyBin_[i]->Write();
    recoMinusTDCCry7EnergyBin_[i]->Write();
  }
  for(int i=0;i<25;i++)
  {
    recoTimeMinusTDCTimeByCry_[i]->SetXTitle("reco + TDC -1 [ns]");
    recoTimeMinusTDCTimeByCry_[i]->Write();
  }
  
  deltaT713Bin12_->Write();
  deltaT713Bin11_->Write();
  deltaT813Bin8_->Write();
  deltaT813Bin6_->Write();
  
  //// Reconstructed energies
  h_e1x1->Write(); 
  h_e3x3->Write(); 
  h_e5x5->Write(); 
  h_e1x1_center->Write(); 
  h_e3x3_center->Write(); 
  h_e5x5_center->Write(); 
  h_e1e9->Write(); 
  h_e1e25->Write(); 
  h_e9e25->Write(); 
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
  h_e1e9_mapx->Write(); 
  h_e1e9_mapy->Write(); 
  h_e1e25_mapx->Write(); 
  h_e1e25_mapy->Write(); 
  h_e9e25_mapx->Write(); 
  h_e9e25_mapy->Write(); 
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

   using namespace edm;
   using namespace cms;
   using namespace std;
   
   //SIC intraTT corrections
   // fetch timing correction
   edm::ESHandle<EcalTimingCorrection> timingCorrectionP;
   iSetup.get<EcalTimingCorrectionRcd>().get(timingCorrectionP);
   const EcalTimingCorrection* timingCorrection = timingCorrectionP.product();

   Handle<EBDigiCollection> pdigis;
   const EBDigiCollection* digis=0;
   try {
     //std::cout << "TestBeamTimingAnalyzer::analyze getting product with label: " << digiProducer_.c_str()<< " prodname: " << digiCollection_.c_str() << endl;
     iEvent.getByLabel( digiProducer_, digiCollection_,pdigis);
     digis = pdigis.product(); // get a ptr to the product
     //iEvent.getByLabel( hitProducer_, phits);
   } catch ( std::exception& ex ) {
     std::cerr << "Error! can't get the product " << digiCollection_.c_str() << std::endl;
   }

   // fetch the digis and compute signal amplitude
   Handle<EBUncalibratedRecHitCollection> phits;
   const EBUncalibratedRecHitCollection* hits=0;
   try {
     //std::cout << "TestBeamTimingAnalyzer::analyze getting product with label: " << hitProducer_.c_str()<< " prodname: " << hitCollection_.c_str() << endl;
     iEvent.getByLabel( hitProducer_, hitCollection_,phits);
     hits = phits.product(); // get a ptr to the product
     //iEvent.getByLabel( hitProducer_, phits);
   } catch ( std::exception& ex ) {
     std::cerr << "Error! can't get the product " << hitCollection_.c_str() << std::endl;
   }

   Handle<EcalTBHodoscopeRecInfo> pHodo;
   const EcalTBHodoscopeRecInfo* recHodo=0;
   try {
     //std::cout << "TestBeamTimingAnalyzer::analyze getting product with label: " << hodoRecInfoProducer_.c_str()<< " prodname: " << hodoRecInfoCollection_.c_str() << endl;
     iEvent.getByLabel( hodoRecInfoProducer_, hodoRecInfoCollection_, pHodo);
     recHodo = pHodo.product(); // get a ptr to the product
   } catch ( std::exception& ex ) {
     std::cerr << "Error! can't get the product " << hodoRecInfoCollection_.c_str() << std::endl;
   }

   Handle<EcalTBTDCRecInfo> pTDC;
   const EcalTBTDCRecInfo* recTDC=0;
   try {
     //std::cout << "TestBeamTimingAnalyzer::analyze getting product with label: " << tdcRecInfoProducer_.c_str()<< " prodname: " << tdcRecInfoCollection_.c_str() << endl;
     iEvent.getByLabel( tdcRecInfoProducer_, tdcRecInfoCollection_, pTDC);
     recTDC = pTDC.product(); // get a ptr to the product
   } catch ( std::exception& ex ) {
     std::cerr << "Error! can't get the product " << tdcRecInfoCollection_.c_str() << std::endl;
   }

   Handle<EcalTBEventHeader> pEventHeader;
   const EcalTBEventHeader* evtHeader=0;
   try {
     //std::cout << "TestBeamTimingAnalyzer::analyze getting product with label: " << eventHeaderProducer_.c_str()<< " prodname: " << eventHeaderCollection_.c_str() << endl;
     iEvent.getByLabel( eventHeaderProducer_ , pEventHeader );
     evtHeader = pEventHeader.product(); // get a ptr to the product
   } catch ( std::exception& ex ) {
     std::cerr << "Error! can't get the product " << eventHeaderProducer_.c_str() << std::endl;
   }
   
   if (!hits)
     return;

   if (!recTDC)
     return;

   if (!recHodo)
     return;

   if (!evtHeader)
     return;

   if (hits->size() == 0)
     return;

   if (evtHeader->tableIsMoving())
     h_tableIsMoving->Fill(evtHeader->eventNumber());

   //TODO:  I DON'T CARE ABOUT THE XTAL IN BEAM!  ONLY THE MAX ENERGY XTAL!
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
   //TODO
   //WARNING
   //HERE I HARDCODE A CRYSTAL, THIS WILL ONLY WORK FOR SPECIFIC DATA
   
   //SM6 data
   if(!(maxHitId.iphi()==15 && maxHitId.ieta()==11))
     return;

   //SM13 data -- 1st 100k events
   //if(!(maxHitId.iphi()==9 && maxHitId.ieta()==12))
   //  return;
   //SM13 data -- 2nd 100k events
   //if(!(maxHitId.iphi()==9 && maxHitId.ieta()==13))
   //  return;


   EBDetId Xtals5x5[25];
   for (unsigned int icry=0;icry<25;icry++)
     {
       unsigned int row = icry / 5;
       unsigned int column= icry %5;
      try
	{
	  //int ieta=xtalInBeam_.ieta()+column-2;
          //TODO:  FIX IT SO THAT WE ARE CENTERED AROUND THE CRYSTAL WE KNOW!
          //int iphi=xtalInBeam_.iphi()+row-2;
	  int ieta=maxHitId.ieta()+column-2;
          int iphi=maxHitId.iphi()+row-2;
          EBDetId tempId(ieta, iphi,EBDetId::ETAPHIMODE);
	  if (tempId.ism()==1) 
	    Xtals5x5[icry]=tempId;
	  else
            Xtals5x5[icry]=EBDetId(0);
	  ///       std::cout << "** Xtal in the matrix **** row " << row  << ", column " << column << ", xtal " << Xtals5x5[icry].ic() << std::endl;
	}
      catch ( std::runtime_error &e )
	{
	  Xtals5x5[icry]=EBDetId(0);   
        }
     } 


   
   bool gain_switch = false;
   double samples_save[10]; for(int i=0; i < 10; ++i) samples_save[i]=0.0;
   double gain_save[10];    for(int i=0; i < 10; ++i) gain_save[i]=0.0;
   
   // find the rechit corresponding digi and the max sample
   
   EBDigiCollection::const_iterator myDg = digis->begin();
   while(myDg->id() != Xtals5x5[6] && myDg != digis->end())
     myDg++;

   int sMax = -1;
   double eMax = 0.;
   if (myDg != digis->end())
     {
       EBDataFrame myDigi = (*myDg);
       for (int sample = 0; sample < myDigi.size(); ++sample)
	 {
	   double analogSample = myDigi.sample(sample).adc();
	   double gainSample   = myDigi.sample(sample).gainId();
	   samples_save[sample] = analogSample;
	   gain_save[sample]    = gainSample;
	   //  std::cout << analogSample << " ";
	   if ( eMax < analogSample )
	     {
	       eMax = analogSample;
	       sMax = sample;
	     }
	   if(gainSample != 1) gain_switch = true;
	 }
       // std::cout << std::endl;
       float chi2 = (hits->find(Xtals5x5[6]))->chi2();
       if(chi2 > 0)
       {
         for(int i =0; i < 10; ++i) {
           h_Shape_->Fill(double(i)+recTDC->offset(),samples_save[i]);
         }
       }
     }
   //else
   //  throw cms::Exception("Exception") << "Can't find maxHitId in digis" << std::endl;

   double amplitude[25];
   double timing[25];
   
   double amplitude3x3=0;  
   double amplitude5x5=0;  
   
   //SIC mod: timing calibrations by crystal; note that these are in nanoseconds
   //double timingCalibration[25] = {-1.0072,-0.0208481,0.849423,0.450663,-46.7874,0.237886,1.04807,0.118216,-0.0699673,0.0734554,0.742545,
   //                                0.0762089,-1.04483,-0.574597,0.725996,0.0385395,-0.182683,-0.962116,-0.415584,0.0744227,-1.73904,
   //                                -0.120794,-0.489065,-0.135128,-7.3209};
   //    
   //                                
   //                                
   double timingCalibration[25] = {-1.00584 ,
     -0.0208175,
     0.849423 ,
     0.450663 ,
     -46.7874 ,
     0.237886 ,
     1.04807  ,
     0.118216 ,
     -0.0699673,
     0.0734554,
     0.742545 ,
     0.0762089,
     -1.04483 ,
     -0.574597,
     0.725996 ,
     0.0385382,
     -0.182674,
     -0.962115,
     -0.415584,
     0.0744227,
     -1.77231 ,
     -0.120792,
     -0.489065,
     -0.135128,
     -6.23559};
   for(int i=0; i<25; ++i)
   {
     // convert to BX
     timingCalibration[i]/=25.0;
   }
   
   for (unsigned int icry=0;icry<25;icry++)
     {
       if (!Xtals5x5[icry].null())
	 {
	   amplitude[icry]=(hits->find(Xtals5x5[icry]))->amplitude();
           float chi2 = (hits->find(Xtals5x5[icry]))->chi2();
           //TODO: ONLY GET THE TIMING OF CRYS WITH MAX SAMPLE = 4
           if(chi2 > 0)
           {
             double time = (hits->find(Xtals5x5[icry]))->jitter();
             // Apply intraTT timing correction
             EcalElectronicsId elecId = ecalElectronicsMap_->getElectronicsId(Xtals5x5[icry]);
             time-=(timingCorrection->getMap().find(std::make_pair(elecId.stripId(),elecId.xtalId())))->second;
             //TODO: SIC mod!  Apply the timing "calibration"
             //time-=timingCalibration[icry];
             time+=timingCalibration[icry];

             //Below done using run 35, cry 12 as best fit line
             //double calibration = 0.0301*((25*time-23.49)/-0.9699)-1.51;
             //time-=(calibration/25);
             //if(chi2==1005)
             //{
             //  //time*=0.926;
             //  time*=0.85;
             //  time+=0.08;
             //}
             //else if(chi2==1006)
             //  time+=0.04; // 1 ns in bx

             timing[icry] = time;
           }
           else
             timing[icry] = -9999;
	   amplitude5x5 += amplitude[icry];
	   // Is in 3x3?
	   if ( icry == 6  || icry == 7  || icry == 8 ||
		icry == 11 || icry == 12 || icry ==13 ||
		icry == 16 || icry == 17 || icry ==18   )
	     {
	       amplitude3x3+=amplitude[icry];
	     }
	 }
     }


   h_e1x1->Fill(amplitude[12]);
   h_e3x3->Fill(amplitude3x3);
   h_e5x5->Fill(amplitude5x5);

   h_e1e9->Fill(amplitude[12]/amplitude3x3);
   h_e1e25->Fill(amplitude[12]/amplitude5x5);
   h_e9e25->Fill(amplitude3x3/amplitude5x5);

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
   }
   const CaloCellGeometry* centerCell = geometry_barrel_->getGeometry(Xtals5x5[12]);
   GlobalPoint centerCryPt = (dynamic_cast <const TruncatedPyramid *>  (centerCell))->getPosition(0);
   double centerCryTime = timing[12];
   // figure out which crys are in which energy bins; compute avgTime
   for (int icry=0;icry<25;icry++)
   {
     //TODO
     //Hardcoded crystals to skip!
     //if(icry==7) continue;
     
     cryVector.clear();
     cryVector.push_back(icry);  // vector of id's in 5x5 matrix
     if(timing[icry] != -9999)  // Imposing chi2 cut of above
     {
       timing1DHistMap[icry]->Fill(25*timing[icry]);
       amplitudeHistMap[icry]->Fill(amplitude[icry]);
       // Fill TDC hists
       if(recTDC)
       {
         const CaloCellGeometry *cell1 = geometry_barrel_->getGeometry(Xtals5x5[icry]);
         GlobalPoint p1 = (dynamic_cast <const TruncatedPyramid *>  (cell1))->getPosition(0);
         double dist = reco::deltaR(p1,centerCryPt);
         distVsTDCHist_->Fill(recTDC->offset(),dist);
         timingHistMap[icry]->Fill(25*recTDC->offset(),25*timing[icry]);
         recoTimeMinusTDCTimeByCry_[icry]->Fill(25*(timing[icry]+recTDC->offset()-1));
       }
       //Bin by energy
       double amp = amplitude[icry];
       int histNum = -1;
       if(amp > 0)
       {
         for(int i=0;i<numEnergyBins-1;++i)
         {
           if(amp >= ampBins[i] && amp < ampBins[i+1])
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
             itr->second.push_back(icry);
         } 
         //else //debug only
         //{
         //  //debug
         //  cout << "New element for bin " << histNum << " inserted with cry " << icry << endl;
         //}
         avgTime+=timing[icry];
         numCrysInAvg++;
       }
     }
   }
   if(numCrysInAvg > 0) 
     avgTime/=numCrysInAvg;
   
   //Fill hists involving TDC info
   if (recTDC)
   {
     h_ampltdc->Fill(recTDC->offset(),amplitude[12]);
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
     int tower1 = (ecalElectronicsMap_->getElectronicsId(Xtals5x5[*crys.begin()])).towerId();
     // Loop over crys in the same energy bin
     for(vector<int>::const_iterator vecItr = crys.begin(); vecItr != crys.end(); ++vecItr)
     {
       int tower2 = (ecalElectronicsMap_->getElectronicsId(Xtals5x5[*vecItr])).towerId();
       fitTimeVsTDC_[itr->first]->Fill(25*recTDC->offset(),25*timing[*vecItr]);
       //TODO:  Now I have turned ON the sameTT requirement!
       if(vecItr != crys.begin() && tower2==tower1) //Require that they are in the same tower
       {
         //debug
         //cout << "Crystal " << Xtals5x5[*vecItr] << " is in the same tower as crystal: " << Xtals5x5[*crys.begin()]
         //  << endl;
         double statCorrection = sqrt(2);
         deltaBetCrys_[itr->first]->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
         const CaloCellGeometry *cell1 = geometry_barrel_->getGeometry(Xtals5x5[*vecItr]);
         GlobalPoint p1 = (dynamic_cast <const TruncatedPyramid *>  (cell1))->getPosition(0);
         const CaloCellGeometry *cell2 = geometry_barrel_->getGeometry(Xtals5x5[*crys.begin()]);
         GlobalPoint p2 = (dynamic_cast <const TruncatedPyramid *>  (cell2))->getPosition(0);
         double dist = reco::deltaR(p1,p2);
         distVsTimeHists_[itr->first]->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection,dist);
         cryIndicesConsideredHists_[itr->first]->Fill(*crys.begin(),*vecItr);
         deltaTvsTDC_[itr->first]->Fill(recTDC->offset(),25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
         deltaTvsAverageT_[itr->first]->Fill(12.5*(timing[*vecItr]+timing[*crys.begin()]),25*(timing[*vecItr]-timing[*crys.begin()]));
         pivotCry_[itr->first]->Fill(*crys.begin());
         if(*crys.begin()==2)
         {
           deltaTCry2EnergyBin_[itr->first]->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
           recoMinusTDCCry2EnergyBin_[itr->first]->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection+25*recTDC->offset()-1);
         }
         else if(*crys.begin()==7)
         {
           deltaTCry7EnergyBin_[itr->first]->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
           recoMinusTDCCry7EnergyBin_[itr->first]->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection+25*recTDC->offset()-1);
           if(*vecItr==13)
           {
             if(itr->first==11)
               deltaT713Bin11_->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
             else if(itr->first==12)
               deltaT713Bin12_->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
           }
         }
         else if(*crys.begin()==8)
         {
           if(*vecItr==13)
           {
             if(itr->first==6)
               deltaT813Bin6_->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
             else if(itr->first==8)
               deltaT813Bin8_->Fill(25*(timing[*vecItr]-timing[*crys.begin()])/statCorrection);
           }
         }

       }
     }
   }
     
     //for(int icry=0;icry<25;++icry)
     //{
     //  if(timing[icry] != -9999)  //chi2 cut
     //  {
     //    timingHistMap[icry]->Fill(recTDC->offset(),timing[icry]);
     //    if(histNums[icry] != -9999) //check for those crys that have good timing and bad ampli
     //      fitTimeVsTDC_[histNums[icry]]->Fill(recTDC->offset(),timing[icry]);
     //  }
     //}
   
   //for(unsigned int ibin=0;ibin < numCrysInBin.size(); ++ibin)
   //{
   //  if(numCrysInBin[ibin] > 0)
   //  {
   //    double valToFill = 25*(timing[icry])

   //  }
   //}
   
   //debug
   //cout << "2) Attempting to fill hist number " << histNum << " with timing delta " << valToFill << endl;
   //if(histNum >=0 && histNum <= 49)
   //  deltaBetCrys_[histNum]->Fill(valToFill);
   
   
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

       //Fill central events

       
       if ( fabs(x + 2.5) < 2.5 && fabs(y + 0.5) < 2.5)
	 {
	   h_e1x1_center->Fill(amplitude[12]);
	   h_e3x3_center->Fill(amplitude3x3);
	   h_e5x5_center->Fill(amplitude5x5);
	   
	   h_e1e9->Fill(amplitude[12]/amplitude3x3);
	   h_e1e25->Fill(amplitude[12]/amplitude5x5);
	   h_e9e25->Fill(amplitude3x3/amplitude5x5);
	 }

       for (unsigned int icry=0;icry<25;icry++)
	 {       
	   h_mapx[icry]->Fill(x,amplitude[icry]);
	   h_mapy[icry]->Fill(y,amplitude[icry]);
	 }

       h_e1e9_mapx->Fill(x,amplitude[12]/amplitude3x3);
       h_e1e9_mapy->Fill(y,amplitude[12]/amplitude3x3);

       h_e1e25_mapx->Fill(x,amplitude[12]/amplitude5x5);
       h_e1e25_mapy->Fill(y,amplitude[12]/amplitude5x5);

       h_e9e25_mapx->Fill(x,amplitude3x3/amplitude5x5);
       h_e9e25_mapy->Fill(y,amplitude3x3/amplitude5x5);
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


DEFINE_FWK_MODULE(TestBeamTimingAnalyzer);
