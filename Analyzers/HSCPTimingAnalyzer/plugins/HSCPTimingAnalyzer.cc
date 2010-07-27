// -*- C++ -*-
//
// Package:    HSCPTimingAnalyzer
// Class:      HSCPTimingAnalyzer
// 
/**\class HSCPTimingAnalyzer HSCPTimingAnalyzer.cc Analyzers/HSCPTimingAnalyzer/plugins/HSCPTimingAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth COOPER
//         Created:  Wed Dec 17 23:20:43 CET 2008
// $Id: HSCPTimingAnalyzer.cc,v 1.2 2010/06/30 15:09:15 scooper Exp $
//
//
#include "Analyzers/HSCPTimingAnalyzer/plugins/HSCPTimingAnalyzer.h"

// Geometry
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"

// For L1 trigger analysis
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1ParticleMap.h"
#include "DataFormats/L1Trigger/interface/L1ParticleMapFwd.h"
#include "DataFormats/L1Trigger/interface/L1HFRings.h"
#include "DataFormats/L1Trigger/interface/L1HFRingsFwd.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//
edm::Service<TFileService> fileService;

bool lessThanRecHitEnergy(const EcalRecHit& hit1, const EcalRecHit& hit2)
{
  if(hit1.energy() < hit2.energy())
    return true;

  return false;
}

//
// constructors and destructor
//
HSCPTimingAnalyzer::HSCPTimingAnalyzer(const edm::ParameterSet& iConfig) :
  EBHitCollection_ (iConfig.getParameter<edm::InputTag>("EBRecHitCollection")),
  EEHitCollection_ (iConfig.getParameter<edm::InputTag>("EERecHitCollection")),
  EBDigiCollection_ (iConfig.getParameter<edm::InputTag>("EBDigiCollection")),
  muonCollection_ (iConfig.getParameter<edm::InputTag>("muonCollection")),
  trackCollection_ (iConfig.getParameter<edm::InputTag>("trackCollection")),
  EBUncalibRecHits_ (iConfig.getParameter<edm::InputTag>("EBUncalibRecHits")),
  EEUncalibRecHits_ (iConfig.getParameter<edm::InputTag>("EEUncalibRecHits")),
  jetCollection_ (iConfig.getParameter<edm::InputTag>("jetCollection")),
  isoEmSource_ (iConfig.getParameter<edm::InputTag>("isolatedEmSource")),
  nonIsoEmSource_ (iConfig.getParameter<edm::InputTag>("nonIsolatedEmSource")),
  cenJetSource_ (iConfig.getParameter<edm::InputTag>("centralJetSource")),
  forJetSource_ (iConfig.getParameter<edm::InputTag>("forwardJetSource")),
  tauJetSource_ (iConfig.getParameter<edm::InputTag>("tauJetSource")),
  muonSource_ (iConfig.getParameter<edm::InputTag>("muonSource")),
  etMissSource_ (iConfig.getParameter<edm::InputTag>("etMissSource")),
  htMissSource_ (iConfig.getParameter<edm::InputTag>("htMissSource")),
  hfRingsSource_ (iConfig.getParameter<edm::InputTag>("hfRingsSource")),
  particleMapSource_ (iConfig.getParameter<edm::InputTag>("particleMapSource")),
  tpCollection_ (iConfig.getParameter<edm::InputTag>("TPCollection")),
  minTrackPt_ (iConfig.getParameter<double>("minimumTrackPt")),
  //fileName_ (iConfig.getUntrackedParameter<std::string>("RootFileName","hscpTimingAnalyzer.root")),
  contCorr_ (iConfig.getUntrackedParameter<double>("containmentCorrection",1.09)),
  energyCut_ (iConfig.getUntrackedParameter<double>("energyCut",0))
{
   //now do what ever initialization is needed
   //file_ = new TFile(fileName_.c_str(),"RECREATE");
   //gROOT->cd();  // Causes Root Fill error after ~ 8k events
   //file_->cd();

   simHitsEnergyHist_ = fileService->make<TH1D>("simHitsEnergy","Energy of sim hits [GeV]",2500,0,10);
   simHitsTimeHist_ = fileService->make<TH1D>("simHitsTime","Time of sim hits [ns]",500,-25,25);
   recHitTimeSimHitTimeHist_ = fileService->make<TH1D>("recHitTimeSimHitTime","RecHit time - simHit time [ns]",500,-25,25);
   recHitTimeHist_ = fileService->make<TH1D>("recHitTime","RecHit time",500,-25,25);
   recHitEnergyHist_ = fileService->make<TH1D>("recHitEnergy","RecHit energy [GeV]",2500,0,10);
   recHitMaxEnergyHist_ = fileService->make<TH1D>("recHitMaxEnergy","Energy of max. ene. recHit [GeV]",2500,0,10);
   recHitMinEnergyHist_ = fileService->make<TH1D>("recHitMinEnergy","Energy of min. ene. recHit [GeV]",2500,0,10);
   recHitMaxEnergyTimingHist_ = fileService->make<TH1D>("recHitMaxEnergyTiming","Timing of max ene. recHit; ns",500,-25,25);
   ptOfTracksWithCrossedCrysHist_ = fileService->make<TH1F>("ptTracksWCrossedCrys","Pt of tracks with at least one crossed cry",300,0,1500);

   //recHitMaxEnergyShapeProfile_ = fileService->make<TProfile>("recHitMaxEnergyShapeProfile","Shape of max energy hits [ADC]",10,0,10);
   //energyOfTrackMatchedHits_ = fileService->make<TH1D>("energyOfTrackMatchedHits", "Energy of track-matched hits [GeV]",2500,0,10);
   energyOfTrackMatchedHitsEB_ = fileService->make<TH1D>("energyOfTrackMatchedHitsEB", "Energy of track-matched hits EB [GeV]",2500,0,10);
   energyOfTrackMatchedHitsEE_ = fileService->make<TH1D>("energyOfTrackMatchedHitsEE", "Energy of track-matched hits EE [GeV]",2500,0,10);
   outOfTimeEnergyOfTrackMatchedHitsEB_ = fileService->make<TH1F>("OOTenergyOfTrackMatchedHitsEB", "OOT Energy of track-matched hits EB [GeV]",2500,0,10);
   outOfTimeEnergyOfTrackMatchedHitsEE_ = fileService->make<TH1F>("OOTenergyOfTrackMatchedHitsEE", "OOT Energy of track-matched hits EE [GeV]",2500,0,10);
   //timeOfTrackMatchedHits_ = fileService->make<TH1D>("timeOfTrackMatchedHits", "Time of track-matched hits [ns]",500,-25,25);
   timeOfTrackMatchedHitsEB_ = fileService->make<TH1D>("timeOfTrackMatchedHitsEB", "Time of track-matched hits EB [ns]",500,-25,25);
   timeOfTrackMatchedHitsEE_ = fileService->make<TH1D>("timeOfTrackMatchedHitsEE", "Time of track-matched hits EE [ns]",500,-25,25);
   //timeVsEnergyOfTrackMatchedHits_ = fileService->make<TH2D>("timeVsEnergyOfTrackMatchedHits","Time [ns] vs. energy [GeV] for track-matched hits",2500,0,10,500,-25,25);
   timeOfTrackMatchedHitsEB1GeVcut_ = fileService->make<TH1F>("timeOfTrackMatchedHitsEB1GeVcut", "Time of track-matched hits EB > 1 GeV [ns]",500,-25,25);
   timeOfTrackMatchedHitsEB3GeVcut_ = fileService->make<TH1F>("timeOfTrackMatchedHitsEB3GeVcut", "Time of track-matched hits EB > 3 GeV [ns]",500,-25,25);
   timeOfTrackMatchedHitsEB5GeVcut_ = fileService->make<TH1F>("timeOfTrackMatchedHitsEB5GeVcut", "Time of track-matched hits EB > 5 GeV [ns]",500,-25,25);
   timeOfTrackMatchedHitsEE1GeVcut_ = fileService->make<TH1F>("timeOfTrackMatchedHitsEE1GeVcut", "Time of track-matched hits EE > 1 GeV [ns]",500,-25,25);
   timeOfTrackMatchedHitsEE3GeVcut_ = fileService->make<TH1F>("timeOfTrackMatchedHitsEE3GeVcut", "Time of track-matched hits EE > 3 GeV [ns]",500,-25,25);
   timeOfTrackMatchedHitsEE5GeVcut_ = fileService->make<TH1F>("timeOfTrackMatchedHitsEE5GeVcut", "Time of track-matched hits EE > 5 GeV [ns]",500,-25,25);
   
   timeVsEnergyOfTrackMatchedHitsEB_ = fileService->make<TH2D>("timeVsEnergyOfTrackMatchedHitsEB","Time [ns] vs. energy [GeV] for track-matched hits EB",2500,0,10,500,-25,25);
   timeVsEnergyOfTrackMatchedHitsEE_ = fileService->make<TH2D>("timeVsEnergyOfTrackMatchedHitsEE","Time [ns] vs. energy [GeV] for track-matched hits EE",2500,0,10,500,-25,25);

   timeChi2VsEnergyOfTrackMatchedHitsEB_ = fileService->make<TH2F>("timeChi2VsEnergyOfTrackMatchedHitsEB","TimeChi2 vs. energy [GeV] for track-matched hits EB",2500,0,10,1000,0,1000);
   timeChi2VsEnergyOfTrackMatchedHitsEE_ = fileService->make<TH2F>("timeChi2VsEnergyOfTrackMatchedHitsEE","TimeChi2 vs. energy [GeV] for track-matched hits EE",2500,0,10,1000,0,1000);

   timeVsDeDxOfTrackMatchedHits_ = fileService->make<TH2D>("timeVsDeDxOfTrackMatchedHits","Time [ns] vs. dE/dx [Mev/cm] for track-matched hits",10000,0,5000,500,-25,25);
   crossedEnergy_ = fileService->make<TH1D>("crossedEcalEnergy", "Total energy of track-crossing xtals [GeV]",2500,0,10);
   crossedLength_ = fileService->make<TH1D>("crossedEcalTrackLength","Total track length in Ecal; [cm]",500,0,50);
   recHitTimeVsSimHitTime_ = fileService->make<TH2D>("recHitTimeVsSimHitTime","recHitTime vs. simHitTime [ns]",500,-25,25,500,-25,25);
   simHitsPerEventHist_ = fileService->make<TH1D>("simHitsPerEvent","Number of SimHits per event",25,0,25);
   recHitTimeSimHitTimeVsEnergy_ = fileService->make<TH2D>("recHitTimeSimHitTimeVsEnergy","recHitTime-simHitTime vs. RecHit energy",2500,0,10,500,-25,25);
   recHitsPerTrackHist_ = fileService->make<TH1D>("ecalRecHitsPerTrack","Number of Ecal RecHits per track",10,0,10);

   deDxHist_ = fileService->make<TH1F>("dedxHist","dE/dx of each crystal;dE/dx [MeV/cm]",10000,0,5000);
   deDxVsMomHist_ = fileService->make<TH2F>("dedxVsMomHist","dE/dx of each crystal;p [GeV/c];dE/#rho dx [MeV cm^{2}/g]",500,0,500,20,0,20);
   trackLengthPerCryHist_ = fileService->make<TH1F>("trackLengthPerCry","Track length in each crystal; [cm]",500,0,50);
   numMatchedCrysPerEventHist_ = fileService->make<TH1F>("numMatchedCrysPerEvent","Number of track-matched (crossed) crystals per event",10,0,10);
   numCrysCrossedVsNumHitsFoundHist_ = fileService->make<TH2F>("crysCrossedVsHitsFound","Number of crystals crossed vs. number of RecHits found",10,0,10,10,0,10);
   hit1EnergyVsHit2EnergyHist_ = fileService->make<TH2F>("hit1EnergyVsHit2Energy","Max energy hit vs. hit 2 energy [GeV]",500,0,5,500,0,5);
   hit1LengthVsHit2LengthHist_ = fileService->make<TH2F>("hit1LengthVsHit2Length","Max energy hit length vs. hit 2 length [cm]",500,0,50,500,0,50);
   hit1DeDxVsHit2DeDxHist_ = fileService->make<TH2F>("hit1DeDxVsHit2DeDx","Hit 1 dE/dx vs. hit 2 dE/dx [MeV/cm]",200,0,100,200,0,100);

   deDx3x3Hist_ = fileService->make<TH1F>("dedx3x3Hist","dE/dx of crystals using 3x3 cluster energy [MeV/cm]",10000,0,5000);
   deDx3x3CorrectedHist_ = fileService->make<TH1F>("dedx3x3CorrectedHist","dE/dx of crystals using 3x3 cluster energy with cont. corr. [MeV/cm]",10000,0,5000);
   deDx5x5Hist_ = fileService->make<TH1F>("dedx5x5Hist","dE/dx of crystals using 5x5 cluster energy [MeV/cm]",10000,0,5000);
   deDx5x5CorrectedHist_ = fileService->make<TH1F>("dedx5x5CorrectedHist","dE/dx of crystals using 5x5 cluster energy with cont. corr. [MeV/cm]",10000,0,5000);
   numCrysIn11x11Hist_ = fileService->make<TH1F>("numCrysIn11x11","Number of recHits in 11x11 around main cry",122,0,122);
   numCrysIn9x9Hist_ = fileService->make<TH1F>("numCrysIn9x9","Number of recHits in 9x9 around main cry",82,0,82);
   numCrysIn7x7Hist_ = fileService->make<TH1F>("numCrysIn7x7","Number of recHits in 7x7 around main cry",50,0,50);
   numCrysIn5x5Hist_ = fileService->make<TH1F>("numCrysIn5x5","Number of recHits in 5x5 around main cry",26,0,26);
   numCrysIn3x3Hist_ = fileService->make<TH1F>("numCrysIn3x3","Number of recHits in 3x3 around main cry",10,0,10);
   numCrysIn15x15Hist_ = fileService->make<TH1F>("numCrysIn15x15","Number of recHits in 15x15 around main cry",226,0,226);
   numCrysIn19x19Hist_ = fileService->make<TH1F>("numCrysIn19x19","Number of recHits in 19x19 around main cry",362,0,362);
   numCrysIn23x23Hist_ = fileService->make<TH1F>("numCrysIn23x23","Number of recHits in 23x23 around main cry",530,0,530);
   numCrysIn27x27Hist_ = fileService->make<TH1F>("numCrysIn27x27","Number of recHits in 27x27 around main cry",730,0,730);
   numCrysIn31x31Hist_ = fileService->make<TH1F>("numCrysIn31x31","Number of recHits in 31x31 around main cry",962,0,962);
   deDxMaxEnergyCryHist_ = fileService->make<TH1F>("deDxMaxEneTrkMatCry","dE/dx of max. energy track-matched cry [MeV/cm]",10000,0,5000);
   deDxMinEnergyCryHist_ = fileService->make<TH1F>("deDxMinEneTrkMatCry","dE/dx of min. energy track-matched cry [MeV/cm]",10000,0,5000);
   deDxTotalHist_ = fileService->make<TH1F>("dedxTotalHist","dE/dx of all track-matched xtals (summed);dE/dx [MeV/cm]",10000,0,5000);
   
   singleCryCrossedEnergy_ = fileService->make<TH1F>("singleCryCrossedEnergy","Energy of recHits from 1 cry crossed evts [GeV]",2500,0,10);
   singleCryCrossedTime_ = fileService->make<TH1F>("singleCryCrossedTiming","Timing of recHits from 1 cry crossed evts [ns]",500,-25,25);
   singleCryCrossedDeDx_ = fileService->make<TH1F>("singleCryCrossedDeDx","dE/dx of recHits from 1 cry crossed evts;dE/dx [MeV/cm]",10000,0,5000);
   singleCryCrossedTrackLength_ = fileService->make<TH1F>("singleCryCrossedTrackLength","Ecal track length from 1 cry crossed evts;length [cm]",2500,0,25);
   singleCryCrossedChi2_ = fileService->make<TH1F>("singleCryCrossedChi2","Chi2 (#sigma_{T}) of recHits from 1 cry crossed evts;#sigma_{T} [ns]",150,0,15);
   
   muonEcalMaxEnergyTimingHist_ = fileService->make<TH1D>("muonEcalRecHitMaxEnergyTiming","Timing of max ene. recHit from muon [ns]",500,-25,25);
   muonEcalMaxEnergyHist_ = fileService->make<TH1D>("muonEcalRecHitMaxEnergy","Energy of max ene. recHit from muon [GeV]",2500,0,10);
   
   energy1OverEnergy9Hist_ = fileService->make<TH1F>("e1OverE9","Energy of matched xtal over 3x3 energy",44,0,1.1);
   energy1OverEnergy25Hist_ = fileService->make<TH1F>("e1OverE25","Energy of matched xtal over 5x5 energy",44,0,1.1);
   energy1OverEnergy49Hist_ = fileService->make<TH1F>("e1OverE49","Energy of matched xtal over 7x7 energy",44,0,1.1);
   energy1OverEnergy81Hist_ = fileService->make<TH1F>("e1OverE81","Energy of matched xtal over 9x9 energy",44,0,1.1);
   energy1OverEnergy121Hist_ = fileService->make<TH1F>("e1OverE121","Energy of matched xtal over 11x11 energy",44,0,1.1);
   energy1OverEnergy225Hist_ = fileService->make<TH1F>("e1OverE225","Energy of matched xtal over 15x15 energy",44,0,1.1);
   energy1OverEnergy361Hist_ = fileService->make<TH1F>("e1OverE361","Energy of matched xtal over 19x19 energy",44,0,1.1);
   energy1OverEnergy529Hist_ = fileService->make<TH1F>("e1OverE529","Energy of matched xtal over 23x23 energy",44,0,1.1);
   energy1OverEnergy729Hist_ = fileService->make<TH1F>("e1OverE729","Energy of matched xtal over 27x27 energy",44,0,1.1);
   energy1OverEnergy961Hist_ = fileService->make<TH1F>("e1OverE961","Energy of matched xtal over 31x31 energy",44,0,1.1);

   energyCrossedOverEnergy9Hist_ = fileService->make<TH1F>("eCrossedOverE9",    "Crossed crystal energy over 3x3 energy",44,0,1.1);
   energyCrossedOverEnergy25Hist_ = fileService->make<TH1F>("eCrossedOverE25",  "Crossed crystal energy over 5x5 energy",44,0,1.1);
   energyCrossedOverEnergy49Hist_ = fileService->make<TH1F>("eCrossedOverE49",  "Crossed crystal energy over 7x7 energy",44,0,1.1);
   energyCrossedOverEnergy81Hist_ = fileService->make<TH1F>("eCrossedOverE81",  "Crossed crystal energy over 9x9 energy",44,0,1.1);
   energyCrossedOverEnergy121Hist_ = fileService->make<TH1F>("eCrossedOverE121","Crossed crystal energy over 11x11 energy",44,0,1.1);
   energyCrossedOverEnergy225Hist_ = fileService->make<TH1F>("eCrossedOverE225","Crossed crystal energy over 15x15 energy",44,0,1.1);
   energyCrossedOverEnergy361Hist_ = fileService->make<TH1F>("eCrossedOverE361","Crossed crystal energy over 19x19 energy",44,0,1.1);
   energyCrossedOverEnergy529Hist_ = fileService->make<TH1F>("eCrossedOverE529","Crossed crystal energy over 23x23 energy",44,0,1.1);
   energyCrossedOverEnergy729Hist_ = fileService->make<TH1F>("eCrossedOverE729","Crossed crystal energy over 27x27 energy",44,0,1.1);
   energyCrossedOverEnergy961Hist_ = fileService->make<TH1F>("eCrossedOverE961","Crossed crystal energy over 31x31 energy",44,0,1.1);

   energy1VsE1OverE9Hist_ = fileService->make<TH2F>("e1VsE1OverE9","Energy of matched xtal vs e1/e9;;[GeV]",44,0,1.1,2500,0,10);
   energy9VsE1OverE9Hist_ = fileService->make<TH2F>("e9VsE1OverE9","Energy of 3x3 xtal array vs e1/e9;;[GeV]",44,0,1.1,2500,0,10);
   
   //energyFractionInTrackMatchedXtalsProfile_ = fileService->make<TProfile>("energyFractionInMatchedXtals","Fraction of crossedEnergy for 2 matched xtals (from most-to-least energy)",5,0,5);
   
   //timeError999EnergyHist_ = fileService->make<TH1F>("energyOfHitsWithError999", "Energy of track-matched hits EB with error 999 [GeV]",2500,0,10);

   timePullDistEBHist_ = fileService->make<TH1F>("pullDistTimeEB","Pull of time, EB",200,-10,10);
   timePullDistEEHist_ = fileService->make<TH1F>("pullDistTimeEE","Pull of time, EE",200,-10,10);

   TsigTvsArmsEBHist_ = fileService->make<TH2F>("TsigTvsArmsEB","t/#sigma_{t} vs. A/1.26 ADC EB",2000,0,1000,200,-10,10);
   TsigTvsArmsEEHist_ = fileService->make<TH2F>("TsigTvsArmsEE","t/#sigma_{t} vs. A/2.87 ADC EE",2000,0,1000,200,-10,10);

   energyVsDeDxHist_ = fileService->make<TH2F>("energyVsDeDx","energyVsDeDx;dE/dx [MeV/cm];energy [GeV]",140,0,70,250,0,1);
   for(int i=0;i<25;++i)
   {
     std::string name="energyInDeDxSlice_";
     name+=intToString(i);
     energyInDeDxSlice_[i] = fileService->make<TH1F>(name.c_str(),name.c_str(),250,0,1);
   }

   dRJetHSCPHist_ = fileService->make<TH1F>("dRJetHSCP","Separation between closest jet and HSCP",300,0,5);
   ptJetHSCPHist_ = fileService->make<TH1F>("ptJetHSCP","Pt of closest jet to HSCP",120,0,600);
   
   // L1 hists
   L1IsoEmDrHist_ = fileService->make<TH1F>("L1IsoEMDr","dR between L1 Iso Em particles and HSCP",300,0,5);
   L1IsoEmEnergyHist_ = fileService->make<TH1F>("L1IsoEMenergy","Energy of closest L1 Iso Em particle to HSCP (dR < 1);GeV",1000,0,10);
   L1NonIsoEmDrHist_ = fileService->make<TH1F>("L1NonIsoEMDr","dR between L1 NonIso Em particles and HSCP",300,0,5);
   L1JetCenDrHist_ = fileService->make<TH1F>("L1JetCenDr","dR between L1 JetCen particles and HSCP",300,0,5);
   L1JetForDrHist_ = fileService->make<TH1F>("L1JetForDr","dR between L1 JetFor particles and HSCP",300,0,5);
   L1JetTauDrHist_ = fileService->make<TH1F>("L1JetTauDr","dR between L1 JetTau particles and HSCP",300,0,5);
   L1MuDrHist_ = fileService->make<TH1F>("L1MuDr","dR between L1 Mu particles and HSCP",300,0,5);
   L1METDrHist_ = fileService->make<TH1F>("L1METDr","dR between L1 MET particles and HSCP",300,0,5);
   L1MHTDrHist_ = fileService->make<TH1F>("L1MHTDr","dR between L1 MHT particles and HSCP",300,0,5);
   L1HFRingDrHist_ = fileService->make<TH1F>("L1HFRingDr","dR between L1 HFRing particles and HSCP",300,0,5);
   L1ClosestL1ObjTypeHist_ = fileService->make<TH1F>("L1ClosestL1ObjType","L1 Obj type of closest L1 Obj (dR <= 1)",9,1,10);
   L1ClosestL1ObjDrHist_ = fileService->make<TH1F>("L1ClosestObjDr","dR between L1 Iso Em particles and HSCP (dR <= 1)",300,0,5);
   L1DrVsObjTypeHist_ = fileService->make<TH2F>("L1DrVsObjType","dR between L1 and HSCP vs. L1 Obj type",9,1,10,300,0,5);
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(1,"IsolatedEM");         
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(2,"NonIsolatedEM");      
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(3,"CentralJet");         
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(4,"ForwardJet");         
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(5,"TauJet");    
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(6,"Muon");  
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(7,"MET"); 
   L1ClosestL1ObjTypeHist_->GetXaxis()->SetBinLabel(8,"MHT");

   // For crossed RecHits that are missing, we should be ok (they will have their initialized values
   myTree_ = fileService->make<TTree>("hscpEcalTree","Ecal information for tracks and RecHits");
   setBranches(myTree_,myTreeVariables_);

   // TrackAssociator parameters
   edm::ParameterSet trkParameters = iConfig.getParameter<edm::ParameterSet>("TrackAssociatorParameters");
   trackParameters_.loadParameters(trkParameters);
   trackAssociator_.useDefaultPropagator();

}


HSCPTimingAnalyzer::~HSCPTimingAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
HSCPTimingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  using namespace l1extra;


  Handle<PCaloHitContainer> ebSimHits;
  iEvent.getByLabel("g4SimHits", "EcalHitsEB", ebSimHits);
  if(!ebSimHits.isValid())
  {
    std::cout << "Cannot get simHits from event!" << std::endl;
    return;
  }
  else if(!ebSimHits->size() > 0)
  {
    std::cout << "EBSimHits size is 0!" << std::endl;
    return;
  }

  Handle<EBRecHitCollection> ebRecHits;
  iEvent.getByLabel(EBHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
  {
    std::cout << "Cannot get ebRecHits from event!" << std::endl;
    return;
  }
  if(!ebRecHits->size() > 0)
  {
    std::cout << "EBRecHits size is 0!" << std::endl;
    return;
  }

  Handle<EcalUncalibratedRecHitCollection> EBUncalibRecHitsHandle_;
  iEvent.getByLabel(EBUncalibRecHits_, EBUncalibRecHitsHandle_);
  if(!EBUncalibRecHitsHandle_.isValid())
  {
    std::cout << "Cannot get ebUncalibRecHits from event!" << std::endl;
    return;
  }
  if(!EBUncalibRecHitsHandle_->size() > 0)
  {
    std::cout << "EBUncalibRecHits size is 0!" << std::endl;
    return;
  }

  Handle<EERecHitCollection> eeRecHits;  //TODO: implement EE?
  iEvent.getByLabel(EEHitCollection_,eeRecHits);
  if(!eeRecHits.isValid())
  {
    std::cout << "Cannot get eeRecHits from event" << std::endl;
    //return;
  }

  Handle<EcalUncalibratedRecHitCollection> EEUncalibRecHitsHandle_;
  iEvent.getByLabel(EEUncalibRecHits_, EEUncalibRecHitsHandle_);
  if(!EEUncalibRecHitsHandle_.isValid())
  {
    std::cout << "Cannot get eeUncalibRecHits from event!" << std::endl;
    //return;
  }
  if(!EEUncalibRecHitsHandle_->size() > 0)
  {
    std::cout << "EEUncalibRecHits size is 0!" << std::endl;
    //return;
  }

  Handle<EBDigiCollection> ebDigis;
  iEvent.getByLabel(EBDigiCollection_,ebDigis);
  if(!ebDigis.isValid())
  {
    std::cout << "Cannot get ebDigis from event!" << std::endl;
    return;
  }
  if(!ebDigis->size() > 0)
  {
    std::cout << "EBDigis size is 0!" << std::endl;
    return;
  }

  Handle<reco::TrackCollection> recoTracks;
  iEvent.getByLabel(trackCollection_, recoTracks);
  if(!recoTracks.isValid())
  {
    std::cout << "Cannot get RecoTracks named " << trackCollection_ << " from event!" << std::endl;
    return;
  }
  if(!recoTracks->size() > 0)
  {
    std::cout << "GeneralTracks size is 0!" << std::endl;
    //XXX See above
    return;
  }
  
  Handle<reco::MuonCollection> pMuons;
  iEvent.getByLabel(muonCollection_, pMuons);
  const reco::MuonCollection* theMuons = pMuons.product();
  if (!pMuons.isValid())
  {
    std::cout << muonCollection_ << " not available" << std::endl;
    return;
  }

  Handle<reco::CaloJetCollection> pJets;
  iEvent.getByLabel(jetCollection_, pJets);
  const reco::CaloJetCollection* theJets = pJets.product();
  if (!pJets.isValid())
  {
    std::cout << jetCollection_ << " not available" << std::endl;
    //return;
  }

  // Get the calorimeter geometry 
  iSetup.get<CaloGeometryRecord>().get(theCaloGeometry_);
  if (!theCaloGeometry_.isValid()) 
    throw cms::Exception("FatalError") << "Unable to find IdealGeometryRecord in event!\n";
  const CaloGeometry* theGeometry = theCaloGeometry_.product();
  // Topology
  edm::ESHandle<CaloTopology> pCaloTopology;
  iSetup.get<CaloTopologyRecord>().get(pCaloTopology);
  const CaloTopology* theCaloTopology = pCaloTopology.product();
  // Get the tracking Geometry
  iSetup.get<GlobalTrackingGeometryRecord>().get(theTrackingGeometry_);
  if (!theTrackingGeometry_.isValid()) 
    throw cms::Exception("FatalError") << "Unable to find GlobalTrackingGeometryRecord in event!\n";
  // EcalDetIdAssociator
  iSetup.get<DetIdAssociatorRecord>().get("EcalDetIdAssociator", ecalDetIdAssociator_);
  // Get the Bfield
  iSetup.get<IdealMagneticFieldRecord>().get(bField_);
    
  // Xtal - TkLength map
  // TODO: Implement for EE (endcap) as well...
  std::map<int,float> XtalMapCurved;
  std::map<int,float> muonCrossedXtalCurvedMap; // rawId to trackLength
  
  TrajectoryStateTransform tsTransform;

  //debug
  std::cout << "Processing event " << iEvent.id().event() << std::endl;

  int numSimHits = 0;
  const PCaloHitContainer* phits=0;
  if(ebSimHits.isValid())
  {
    phits = ebSimHits.product();
    for(PCaloHitContainer::const_iterator mySimHit = phits->begin(); mySimHit != phits->end(); mySimHit++)
    {
      simHitsEnergyHist_->Fill(mySimHit->energy());
      simHitsTimeHist_->Fill(mySimHit->time());
      EBDetId simHitId = EBDetId(mySimHit->id());
      //std::cout << "SimHit DetId found: " << simHitId << std::endl;
      //std::cout << "SimHit hashedIndex: " << simHitId.hashedIndex() << std::endl;
      //std::cout << "SimHit energy: " << mySimHit->energy() << " time: " << mySimHit->time()
      //  << std::endl;
      numSimHits++;

      EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(mySimHit->id());
      if(thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
      {
        //std::cout << "Could not find simHit detId in uncalibRecHitCollection!" << std::endl;
        continue;
      }
      if(thisUncalibRecHit->chi2() < 0)
        continue;

      EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      while(recHitItr != ebRecHits->end() && (recHitItr->id() != simHitId))
      {
        ++recHitItr;
      }
      if(recHitItr==ebRecHits->end())
      {
        // Commented out for debugging ease, Aug 3 2009
        //std::cout << "Could not find simHit detId: " << simHitId << "in RecHitCollection!" << std::endl;
        continue;
      }

      recHitTimeSimHitTimeHist_->Fill(recHitItr->time()-mySimHit->time());
      recHitTimeVsSimHitTime_->Fill(mySimHit->time(),recHitItr->time());
      recHitTimeSimHitTimeVsEnergy_->Fill(recHitItr->energy(),recHitItr->time()-mySimHit->time());
      //std::cout << "    SIMHIT MATCHED to recHit detId: " << (EBDetId)recHitItr->id() << std::endl;
      //std::cout << "    RECHIT TIME: " << 25*recHitItr->time() << " ENERGY: " << recHitItr->energy() << std::endl;
    }
    simHitsPerEventHist_->Fill(numSimHits);
  } // if simhit handle is valid

  //jul23debug
  //cout << "sicjul23debug: Loop over RecHits!" << endl;
  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(recHitItr->id());
    if(thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
    {
      std::cout << "Could not find recHit detId in uncalibRecHitCollection!" << std::endl;
      continue;
    }
    if(thisUncalibRecHit->chi2() < 0)
      continue;

    recHitTimeHist_->Fill(recHitItr->time());
    recHitEnergyHist_->Fill(recHitItr->energy());
  }

  //jul23debug
  //cout << "sicjul23debug: Loop over Tracks! there are " << recoTracks->size() << " tracks!" << endl;
  // Changed Jul 23 2010 to loop on tracks, not muons
  int trackIndex = -1;
  initializeBranches(myTree_,myTreeVariables_);
  myTreeVariables_.eventL1a = iEvent.id().event();
  myTreeVariables_.runNum = iEvent.id().run();
  myTreeVariables_.lumiNum = iEvent.eventAuxiliary().luminosityBlock();
  //jul23debug
  //cout << "sicjul23debug: Filled/initialized some tree branches!" << endl;

  // Now loop over RecoTracks
  for(reco::TrackCollection::const_iterator TKit = recoTracks->begin(); TKit != recoTracks->end () ; ++TKit)
  {
  //jul23debug
    //std::cout << "sicjul23debug: RecoTrack found with Pt (GeV): "  << TKit->outerPt() << std::endl;
    
    //TODO: Remove the hists later
    ////SIC DEBUG -- muon
    //reco::MuonEnergy myMuonEnergy = MUit->calEnergy();
    //DetId ecalDet = myMuonEnergy.ecal_id();
    ////debug
    ////std::cout << "Muon ecalEnergy: " << myMuonEnergy.emMax << " muon ecalTime: " << myMuonEnergy.ecal_time << std::endl;
    ////Energy cut on muon timing hist
    ////if(myMuonEnergy.emMax > 1)
    ////Only plot muons in the endcaps for now and put in a big energy cut
    ////if(ecalDet.subdetId()==EcalEndcap && myMuonEnergy.emMax > 5)
    ////if(myMuonEnergy.emMax > energyCut_)
    ////{
    //  muonEcalMaxEnergyTimingHist_->Fill(myMuonEnergy.ecal_time);
    //  muonEcalMaxEnergyHist_->Fill(myMuonEnergy.emMax);
    ////}

    // Pt Cut
    if(TKit->outerPt() < minTrackPt_)
      continue;

  //jul23debug
  //cout << "\tsicjul23debug: Passed pt cut" << endl;

    //TODO: Change tree content if needed
    // Fill tree branch
    //muonPt_ = MUit->globalTrack()->outerPt();
    FreeTrajectoryState tkInnerState = tsTransform.innerFreeState(*TKit, &*bField_);
    
    // Build set of points in Ecal (necklace) using the propagator
    std::vector<SteppingHelixStateInfo> neckLace;
    neckLace = calcEcalDeposit(&tkInnerState, trackParameters_);

    // Initialize variables to be filled by the track-length function
    double totalLengthCurved = 0.;
    GlobalPoint internalPointCurved(0., 0., 0.);
    GlobalPoint externalPointCurved(0., 0., 0.);
    
    if(neckLace.size() > 1)
    {
      getDetailedTrackLengthInXtals(XtalMapCurved,
          muonCrossedXtalCurvedMap,
          totalLengthCurved,
          internalPointCurved,
          externalPointCurved,
          & (*theGeometry),
          & (*theCaloTopology),
          neckLace);
    }

  //jul23debug
  //cout << "\tsicjul23debug: Passed detailed track length in crys" << endl;
    
    int numCrysCrossed = -1;
    numCrysCrossed = muonCrossedXtalCurvedMap.size();
    // Only look at events where x crys were crossed, for now (used for all-EB samples)
    if(numCrysCrossed < 1)
      return;
    
    trackIndex++;
    numMatchedCrysPerEventHist_->Fill(numCrysCrossed);
    double crossedEnergy = 0;
    double crossedLength = 0;
    int numRecHitsFound = 0;
    myTreeVariables_.numCrys[trackIndex] = numCrysCrossed;
    myTreeVariables_.trackPt[trackIndex] = (float)TKit->pt();
    myTreeVariables_.trackP[trackIndex] = (float)TKit->p();
    myTreeVariables_.trackEta[trackIndex] = (float)TKit->eta();
    myTreeVariables_.trackPhi[trackIndex] = (float)TKit->phi();
    myTreeVariables_.trackDz[trackIndex] = (float)TKit->dz();
    myTreeVariables_.trackD0[trackIndex] = (float)TKit->d0();
    myTreeVariables_.trackQuality[trackIndex] = TKit->qualityMask();
    myTreeVariables_.trackCharge[trackIndex] = TKit->charge();
    myTreeVariables_.trackChi2[trackIndex] = TKit->chi2();
    myTreeVariables_.trackNdof[trackIndex] = TKit->ndof();
    myTreeVariables_.trackNOH[trackIndex] = TKit->found();
    ptOfTracksWithCrossedCrysHist_->Fill(TKit->pt());

  //jul23debug
  //cout << "\tsicjul23debug: Loop over the muonCrossedXtalCurvedMap" << endl;
    // Info
    //std::cout << "\t Number of crossedCrys (my calculation): " << numCrysCrossed << std::endl;
    std::vector<EcalRecHit> trackMatchedRecHits;
    int crystalIndex = -1;
    //TODO: We probably only want to use the max energy hit here.  But anyway this logic needs to be thought through.  Or is it min?
    for(std::map<int,float>::const_iterator mapIt = muonCrossedXtalCurvedMap.begin();
        mapIt != muonCrossedXtalCurvedMap.end(); ++mapIt)
    {
      crystalIndex++;
      EcalRecHitCollection::const_iterator thisHit;
      EcalUncalibratedRecHitCollection::const_iterator thisUncalibRecHit;
      double trackLengthInXtal = 0;

      if(DetId(mapIt->first).subdetId()==EcalBarrel)
      {
        EBDetId ebDetId(mapIt->first);
        trackLengthInXtal = mapIt->second;
        std::cout << "\t Cry EBDetId: " << ebDetId << " trackLength: " << trackLengthInXtal << std::endl;
        myTreeVariables_.isEBcry[trackIndex][crystalIndex] = true;
        myTreeVariables_.hashedIndex[trackIndex][crystalIndex] = ebDetId.hashedIndex();
        myTreeVariables_.cryTrackLength[trackIndex][crystalIndex] = trackLengthInXtal;

        thisHit = ebRecHits->find(ebDetId);
        if(thisHit == ebRecHits->end()) 
        {
          std::cout << "\t Could not find crossedEcal detId: " << ebDetId << " in EBRecHitCollection!" << std::endl;
          continue;
        }

        thisUncalibRecHit =  EBUncalibRecHitsHandle_->find(ebDetId);
        if(thisUncalibRecHit==EBUncalibRecHitsHandle_->end())
        {
          std::cout << "Could not find recHit detId in EBuncalibRecHitCollection!" << std::endl;
          continue;
        }
        myTreeVariables_.cryEnergy[trackIndex][crystalIndex] = thisHit->energy();
        myTreeVariables_.cryAmplitude[trackIndex][crystalIndex] = thisUncalibRecHit->amplitude();
        myTreeVariables_.cryTime[trackIndex][crystalIndex] = thisHit->time();
        myTreeVariables_.cryTimeError[trackIndex][crystalIndex] = 25*thisUncalibRecHit->chi2();
        myTreeVariables_.cryDeDx[trackIndex][crystalIndex] = 1000*thisHit->energy()/trackLengthInXtal;
        //XXX: SIC debug
        std::cout << "\t My track-matched EBDetId: " << ebDetId << " energy: " << thisHit->energy() << " time: " <<
          thisHit->time() << " jitter: " << thisUncalibRecHit->jitter() << " chi2: " << thisUncalibRecHit->chi2() <<  " amplitude(ADC): " << thisUncalibRecHit->amplitude() << std::endl;
      }
      else if(DetId(mapIt->first).subdetId()==EcalEndcap)
      {
        EEDetId eeDetId(mapIt->first);
        trackLengthInXtal = mapIt->second;
        std::cout << "\t Cry EEDetId: " << eeDetId << " trackLength: " << trackLengthInXtal << std::endl;
        myTreeVariables_.isEBcry[trackIndex][crystalIndex] = false;
        myTreeVariables_.hashedIndex[trackIndex][crystalIndex] = eeDetId.hashedIndex();
        myTreeVariables_.cryTrackLength[trackIndex][crystalIndex] = trackLengthInXtal;

        thisHit = eeRecHits->find(eeDetId);
        if(thisHit == eeRecHits->end()) 
        {
          std::cout << "\t Could not find crossedEcal detId: " << eeDetId << " in EERecHitCollection!" << std::endl;
          continue;
        }

        thisUncalibRecHit =  EEUncalibRecHitsHandle_->find(eeDetId);
        if(thisUncalibRecHit==EEUncalibRecHitsHandle_->end())
        {
          std::cout << "Could not find recHit detId in EEuncalibRecHitCollection!" << std::endl;
          continue;
        }
        myTreeVariables_.cryEnergy[trackIndex][crystalIndex] = thisHit->energy();
        myTreeVariables_.cryAmplitude[trackIndex][crystalIndex] = thisUncalibRecHit->amplitude();
        myTreeVariables_.cryTime[trackIndex][crystalIndex] = thisHit->time();
        myTreeVariables_.cryTimeError[trackIndex][crystalIndex] = 25*thisUncalibRecHit->chi2();
        myTreeVariables_.cryDeDx[trackIndex][crystalIndex] = 1000*thisHit->energy()/trackLengthInXtal;
        //XXX: SIC debug
        //std::cout << "\t My track-matched EEDetId: " << eeDetId << " energy: " << thisHit->energy() << " time: " <<
        //  thisHit->time() << " jitter: " << thisUncalibRecHit->jitter() << " chi2: " << thisUncalibRecHit->chi2() <<  " amplitude(ADC): " << thisUncalibRecHit->amplitude() << std::endl;
      }
      else
        continue;
      
      //XXX: Chi2 cut
      //if(thisUncalibRecHit->chi2() < 0)
      //  continue;
      //XXX: SIC TURNED OFF TO TEST NEW RATIO METHOD

      //XXX: Energy cut
      //if(thisHit->energy() < energyCut_)
      //  continue;
      
      //XXX: dE/dx cut
      //if((contCorr_*1000*0.97*thisHit->energy()/trackLengthInXtal)<16)
      //  continue;

      // Push onto vector of track-matched recHits
      trackMatchedRecHits.push_back(*thisHit);

      //TODO: Make a check on the recoFlag of the RecHit
      // Especially if the hit is out of time so we can use the ratio method amplitude...
      // But right now we are just using both amp/time from ratio method
      numRecHitsFound++;
      crossedEnergy+=thisHit->energy();
      crossedLength+=trackLengthInXtal;
      deDxHist_->Fill(thisHit->energy()/trackLengthInXtal);
      trackLengthPerCryHist_->Fill(trackLengthInXtal);

      //energyOfTrackMatchedHits_->Fill(contCorr_*0.97*thisHit->energy());
      //XXX: SIC TO TEST NEW RATIOS METHOD
      if(DetId(mapIt->first).subdetId()==EcalBarrel)
      {
        //XXX: Filter out error==-999
        //if(thisUncalibRecHit->chi2()!=-999)
        //{
          timeVsEnergyOfTrackMatchedHitsEB_->Fill(thisHit->energy(),thisHit->time());
          timeChi2VsEnergyOfTrackMatchedHitsEB_->Fill(thisHit->energy(),25*thisUncalibRecHit->chi2());
          timeOfTrackMatchedHitsEB_->Fill(thisHit->time());
          energyOfTrackMatchedHitsEB_->Fill(thisHit->energy());
          outOfTimeEnergyOfTrackMatchedHitsEB_->Fill(thisHit->outOfTimeEnergy());
          
          if(thisHit->energy()>0.2 && thisUncalibRecHit->chi2() > 0) // Only fill pull for energy > 200 MeV
            timePullDistEBHist_->Fill((thisHit->time()-1)/(25*thisUncalibRecHit->chi2()));
            //timePullDistEBHist_->Fill((thisHit->time()-1)/(37/thisUncalibRecHit->amplitude()/1.2));
          if(thisUncalibRecHit->chi2() > 0)
            TsigTvsArmsEBHist_->Fill(thisUncalibRecHit->amplitude()/1.26,thisHit->time()/(25*thisUncalibRecHit->chi2()));

          if(thisHit->energy() > 1)
            timeOfTrackMatchedHitsEB1GeVcut_->Fill(thisHit->time());
          if(thisHit->energy() > 3)
            timeOfTrackMatchedHitsEB3GeVcut_->Fill(thisHit->time());
          if(thisHit->energy() > 5)
            timeOfTrackMatchedHitsEB5GeVcut_->Fill(thisHit->time());
          //XXX debug
          //std::cout << "EB UncalibRecHit ID: " << EBDetId(thisUncalibRecHit->id()) << " outOfTime amp: "
          //  << thisUncalibRecHit->outOfTimeEnergy() << " regular amp: " << thisUncalibRecHit->amplitude() << std::endl <<
          //  " (WEIGHTAMP-RATIOAMP)/WEIGHTAMP: " << (thisUncalibRecHit->outOfTimeEnergy()-thisUncalibRecHit->amplitude())/thisUncalibRecHit->amplitude()
          //  << " regular energy: " << thisHit->energy() << " outOfTime energy: " << thisHit->outOfTimeEnergy()
          //  << " (WEIGHTENERGY-RATIOENERGY)/WEIGHTENERGY: " << (thisHit->outOfTimeEnergy()-thisHit->energy())/thisHit->energy()
          //  << std::endl;
        //}
      }
      else if(DetId(mapIt->first).subdetId()==EcalEndcap)
      {
        //XXX: Filter out error==-999
        //if(thisUncalibRecHit->chi2()!=-999)
        //{
          timeVsEnergyOfTrackMatchedHitsEE_->Fill(thisHit->energy(),thisHit->time());
          timeChi2VsEnergyOfTrackMatchedHitsEE_->Fill(thisHit->energy(),thisUncalibRecHit->chi2());
          timeOfTrackMatchedHitsEE_->Fill(thisHit->time());
          energyOfTrackMatchedHitsEE_->Fill(thisHit->energy());
          outOfTimeEnergyOfTrackMatchedHitsEE_->Fill(thisHit->outOfTimeEnergy());
          
          if(thisHit->energy()>0.2 && thisUncalibRecHit->chi2() > 0) // Only fill pull for energy > 200 MeV
            timePullDistEEHist_->Fill((thisHit->time()-1)/(25*thisUncalibRecHit->chi2()));
            //timePullDistEEHist_->Fill((thisHit->time()-1)/(37/thisUncalibRecHit->amplitude()/2.0));
          if(thisUncalibRecHit->chi2() > 0)
            TsigTvsArmsEEHist_->Fill(thisUncalibRecHit->amplitude()/2.87,thisHit->time()/(25*thisUncalibRecHit->chi2()));
          
          if(thisHit->energy() > 1)
            timeOfTrackMatchedHitsEE1GeVcut_->Fill(thisHit->time());
          if(thisHit->energy() > 3)
            timeOfTrackMatchedHitsEE3GeVcut_->Fill(thisHit->time());
          if(thisHit->energy() > 5)
            timeOfTrackMatchedHitsEE5GeVcut_->Fill(thisHit->time());
          //XXX debug
          //std::cout << "EE UncalibRecHit ID: " << EEDetId(thisUncalibRecHit->id()) << " outOfTime amp: "
          //  << thisUncalibRecHit->outOfTimeEnergy() << " regular amp: " << thisUncalibRecHit->amplitude() << std::endl <<
          //  " (WEIGHTAMP-RATIOAMP)/WEIGHTAMP: " << (thisUncalibRecHit->outOfTimeEnergy()-thisUncalibRecHit->amplitude())/thisUncalibRecHit->amplitude()
          //  << " regular energy: " << thisHit->energy() << " outOfTime energy: " << thisHit->outOfTimeEnergy()
          //  << " (WEIGHTENERGY-RATIOENERGY)/WEIGHTENERGY: " << (thisHit->outOfTimeEnergy()-thisHit->energy())/thisHit->energy()
          //  << std::endl;
        //}
      }

      //XXX: SIC TO TEST NEW RATIOS METHOD
      //timeVsEnergyOfTrackMatchedHits_->Fill(contCorr_*0.97*thisHit->energy(),thisHit->time());

      timeVsDeDxOfTrackMatchedHits_->Fill(1000*thisHit->energy()/trackLengthInXtal,thisHit->time());
      deDxVsMomHist_->Fill(TKit->outerP(),1000*thisHit->energy()/(8.3*trackLengthInXtal));

      if(numCrysCrossed==1) // only going through this loop once anyway in that case
      {
        singleCryCrossedEnergy_->Fill(contCorr_*0.97*thisHit->energy());
        singleCryCrossedTime_->Fill(thisHit->time());
        singleCryCrossedDeDx_->Fill(contCorr_*1000*0.97*thisHit->energy()/trackLengthInXtal);
        singleCryCrossedTrackLength_->Fill(trackLengthInXtal);
        singleCryCrossedChi2_->Fill(25*thisUncalibRecHit->chi2());
      }

      //XXX: SIC: TURNED OFF FOR NEW RATIOS TESTING
      // Just implement EE later


      // XXX: disable this for now; causing seg faults (probably array out of bounds)
      // SIC March 1 2010
      // Plot slices
      //energyVsDeDxHist_->Fill(cryDeDx_,cryEnergy_);
      //if(cryDeDx_/2.8 < 24)
      //  energyInDeDxSlice_[(int)round(cryDeDx_/2.8)]->Fill(cryEnergy_);
      //else
      //  energyInDeDxSlice_[24]->Fill(cryEnergy_);
    }

    numCrysCrossedVsNumHitsFoundHist_->Fill(numRecHitsFound,numCrysCrossed);
    crossedEnergy_->Fill(crossedEnergy);
    crossedLength_->Fill(crossedLength);
    if(crossedLength > 0)
      deDxTotalHist_->Fill(1000*crossedEnergy/crossedLength);

    if(trackMatchedRecHits.size() < 1)
      return;

    // Sort track-matched RecHit vector
    std::sort(trackMatchedRecHits.begin(),trackMatchedRecHits.end(),lessThanRecHitEnergy);
    //EcalRecHitCollection::const_iterator end = trackMatchedRecHits.end();
    //int counter = 0;
    //for(EcalRecHitCollection::const_iterator hit = trackMatchedRecHits.begin(); hit != end; ++hit)
    //{
    //  energyFractionInTrackMatchedXtalsProfile_->Fill(counter,hit->energy()/crossedEnergy);
    //  ++counter;
    //}
    EcalRecHit maxEnergyHit = *(trackMatchedRecHits.end()-1);
    if(maxEnergyHit.id().subdetId()!=EcalBarrel)
      return;

    EBDetId ebDetId = (EBDetId) maxEnergyHit.id();
    double maxEnergy = maxEnergyHit.energy();
    recHitMaxEnergyHist_->Fill(maxEnergy);
    ////XXX: Only fill timing plot if energy above cut
    //if(maxEnergy > energyCut_)
    recHitMaxEnergyTimingHist_->Fill(maxEnergyHit.time());
    double trackLength1 = muonCrossedXtalCurvedMap[maxEnergyHit.id().rawId()];
    //SIC debug
    //std::cout << "Max hit energy: " << maxEnergy << " trackLength: " << trackLength1 << " dE/dx: " << 1000*maxEnergy/trackLength1 << std::endl;
    deDxMaxEnergyCryHist_->Fill(1000*maxEnergy/trackLength1);
    EcalRecHitCollection::const_iterator minEneHitItr = trackMatchedRecHits.begin();
    while(minEneHitItr->energy() <=0 && minEneHitItr != trackMatchedRecHits.end()-1)
      ++minEneHitItr;
    if(minEneHitItr->id().rawId() != maxEnergyHit.id().rawId())
    {
      EcalRecHit minEnergyHit = *minEneHitItr;
      double minEnergy = minEnergyHit.energy();
      recHitMinEnergyHist_->Fill(minEnergy);
      double trackLength2 = muonCrossedXtalCurvedMap[minEnergyHit.id().rawId()];
      //std::cout << "Min hit energy: " << minEnergy << " trackLength: " << trackLength2 << " dE/dx: " << 1000*minEnergy/trackLength2 << std::endl;
      deDxMinEnergyCryHist_->Fill(1000*minEnergy/trackLength2);
    }

    // Make NxN energies
    float energy3x3 = 0;
    int num3x3crys = 0;
    float energy5x5 = 0;
    int num5x5crys = 0;
    float energy7x7 = 0;
    int num7x7crys = 0;
    float energy9x9 = 0;
    int num9x9crys = 0;
    float energy11x11 = 0;
    int num11x11crys = 0;
    float energy15x15 = 0;
    int num15x15crys = 0;
    float energy19x19 = 0;
    int num19x19crys = 0;
    float energy23x23 = 0;
    int num23x23crys = 0;
    float energy27x27 = 0;
    int num27x27crys = 0;
    float energy31x31 = 0;
    int num31x31crys = 0;
    const CaloSubdetectorTopology* ebTopology = theCaloTopology->getSubdetectorTopology(DetId::Ecal,ebDetId.subdetId());
    std::vector<DetId> S9aroundMax;
    std::vector<DetId> S25aroundMax;
    std::vector<DetId> S49aroundMax;
    std::vector<DetId> S81aroundMax;
    std::vector<DetId> S121aroundMax;
    std::vector<DetId> S225aroundMax;
    std::vector<DetId> S361aroundMax;
    std::vector<DetId> S529aroundMax;
    std::vector<DetId> S729aroundMax;
    std::vector<DetId> S961aroundMax;
    ebTopology->getWindow(ebDetId,3,3).swap(S9aroundMax);
    ebTopology->getWindow(ebDetId,5,5).swap(S25aroundMax);
    ebTopology->getWindow(ebDetId,7,7).swap(S49aroundMax);
    ebTopology->getWindow(ebDetId,9,9).swap(S81aroundMax);
    ebTopology->getWindow(ebDetId,11,11).swap(S121aroundMax);
    ebTopology->getWindow(ebDetId,15,15).swap(S225aroundMax);
    ebTopology->getWindow(ebDetId,19,19).swap(S361aroundMax);
    ebTopology->getWindow(ebDetId,23,23).swap(S529aroundMax);
    ebTopology->getWindow(ebDetId,27,27).swap(S729aroundMax);
    ebTopology->getWindow(ebDetId,31,31).swap(S961aroundMax);
    for(int icry=0; icry < 961; ++icry)
    {
      if(S961aroundMax[icry].det() != DetId::Ecal)
        continue;
      if(S961aroundMax[icry].subdetId() != EcalBarrel)
        continue;

      EBDetId hitId = EBDetId(S961aroundMax[icry]);
      if(hitId.null())
        continue;
      EBRecHitCollection::const_iterator itrechit = ebRecHits->find(hitId);
      if(itrechit==ebRecHits->end())
        continue;

      energy31x31+=itrechit->energy();
      ++num31x31crys;
      if(find(S729aroundMax.begin(),S729aroundMax.end(),S961aroundMax[icry]) != S729aroundMax.end())
      {
        energy27x27+=itrechit->energy();
        ++num27x27crys;
      }
      if(find(S529aroundMax.begin(),S529aroundMax.end(),S961aroundMax[icry]) != S529aroundMax.end())
      {
        energy23x23+=itrechit->energy();
        ++num23x23crys;
      }
      if(find(S361aroundMax.begin(),S361aroundMax.end(),S961aroundMax[icry]) != S361aroundMax.end())
      {
        energy19x19+=itrechit->energy();
        ++num19x19crys;
      }
      if(find(S225aroundMax.begin(),S225aroundMax.end(),S961aroundMax[icry]) != S225aroundMax.end())
      {
        energy15x15+=itrechit->energy();
        ++num15x15crys;
      }
      if(find(S121aroundMax.begin(),S121aroundMax.end(),S961aroundMax[icry]) != S121aroundMax.end())
      {
        energy11x11+=itrechit->energy();
        ++num11x11crys;
      }
      if(find(S81aroundMax.begin(),S81aroundMax.end(),S961aroundMax[icry]) != S81aroundMax.end())
      {
        energy9x9+=itrechit->energy();
        ++num9x9crys;
      }
      if(find(S49aroundMax.begin(),S49aroundMax.end(),S961aroundMax[icry]) != S49aroundMax.end())
      {
        energy7x7+=itrechit->energy();
        ++num7x7crys;
      }
      if(find(S25aroundMax.begin(),S25aroundMax.end(),S961aroundMax[icry]) != S25aroundMax.end())
      {
        energy5x5+=itrechit->energy();
        ++num5x5crys;
      }
      if(find(S9aroundMax.begin(),S9aroundMax.end(),S961aroundMax[icry]) != S9aroundMax.end())
      {
        energy3x3+=itrechit->energy();
        ++num3x3crys;
      }
    }
    //deDx3x3Hist_->Fill(1000*energy3x3/trackLengthInXtal);
    //deDx3x3CorrectedHist_->Fill(1000*energy3x3/trackLengthInXtal);  // don't apply containment correction factor
    //deDx5x5Hist_->Fill(1000*energy5x5/trackLengthInXtal);
    //deDx5x5CorrectedHist_->Fill(1000*energy3x3/trackLengthInXtal);  // don't apply containment correction factor
    numCrysIn31x31Hist_->Fill(num31x31crys);
    numCrysIn27x27Hist_->Fill(num27x27crys);
    numCrysIn23x23Hist_->Fill(num23x23crys);
    numCrysIn19x19Hist_->Fill(num19x19crys);
    numCrysIn15x15Hist_->Fill(num15x15crys);
    numCrysIn11x11Hist_->Fill(num11x11crys);
    numCrysIn9x9Hist_->Fill(num9x9crys);
    numCrysIn7x7Hist_->Fill(num7x7crys);
    numCrysIn5x5Hist_->Fill(num5x5crys);
    numCrysIn3x3Hist_->Fill(num3x3crys);

    energy1OverEnergy9Hist_->Fill(maxEnergy/energy3x3);
    energy1OverEnergy25Hist_->Fill(maxEnergy/energy5x5);
    energy1OverEnergy49Hist_->Fill(maxEnergy/energy7x7);
    energy1OverEnergy81Hist_->Fill(maxEnergy/energy9x9);
    energy1OverEnergy121Hist_->Fill(maxEnergy/energy11x11);
    energy1OverEnergy225Hist_->Fill(maxEnergy/energy15x15);
    energy1OverEnergy361Hist_->Fill(maxEnergy/energy19x19);
    energy1OverEnergy529Hist_->Fill(maxEnergy/energy23x23);
    energy1OverEnergy729Hist_->Fill(maxEnergy/energy27x27);
    energy1OverEnergy961Hist_->Fill(maxEnergy/energy31x31);
    energy1VsE1OverE9Hist_->Fill(maxEnergy/energy3x3,maxEnergy);
    energy9VsE1OverE9Hist_->Fill(maxEnergy/energy3x3,energy3x3);

    energyCrossedOverEnergy9Hist_->Fill(crossedEnergy/energy3x3);
    energyCrossedOverEnergy25Hist_->Fill(crossedEnergy/energy5x5);
    energyCrossedOverEnergy49Hist_->Fill(crossedEnergy/energy7x7);
    energyCrossedOverEnergy81Hist_->Fill(crossedEnergy/energy9x9);
    energyCrossedOverEnergy121Hist_->Fill(crossedEnergy/energy11x11);
    energyCrossedOverEnergy225Hist_->Fill(crossedEnergy/energy15x15);
    energyCrossedOverEnergy361Hist_->Fill(crossedEnergy/energy19x19);
    energyCrossedOverEnergy529Hist_->Fill(crossedEnergy/energy23x23);
    energyCrossedOverEnergy729Hist_->Fill(crossedEnergy/energy27x27);
    energyCrossedOverEnergy961Hist_->Fill(crossedEnergy/energy31x31);

    const CaloSubdetectorGeometry* theBarrelSubdetGeometry = theGeometry->getSubdetectorGeometry(DetId::Ecal,1);
    // Find the closest uncorrected CaloJet
    float smallestDist = 1000;
    float smallestDistPt = 0;
    const CaloCellGeometry *cell_p = theBarrelSubdetGeometry->getGeometry(ebDetId);
    GlobalPoint p = (dynamic_cast <const TruncatedPyramid *> (cell_p))->getPosition(0);

    float cryEta = p.eta();
    float cryPhi = p.phi();
    float cryIEta = ebDetId.ieta();
    float cryIPhi = ebDetId.iphi();
    for(reco::CaloJetCollection::const_iterator jetItr = theJets->begin(); jetItr != theJets->end(); ++jetItr)
    {
      float jetEta = jetItr->eta();
      float jetPhi = jetItr->phi();
      float dR = sqrt(pow(jetEta-cryEta,2)+pow(jetPhi-cryPhi,2));
      if(dR < smallestDist)
      {
        smallestDist = dR;
        smallestDistPt = jetItr->pt();
      }
    }
    // Make hist of dR
    dRJetHSCPHist_->Fill(smallestDist);
    ptJetHSCPHist_->Fill(smallestDistPt);
    // debug
    //std::cout << "dR of closest jet: " << smallestDist << " and Pt: " << smallestDistPt << std::endl;

    // SIC July 9 2010
    // Do the L1 analysis here -- code borrowed from: L1Trigger/L1ExtraFromDigis/src/L1ExtraTestAnalyzer.cc
    float closestDrAll = 1000;
    int typeOfClosestL1All = 0; // refer to logbook July 9 for schema

    // Isolated EM particles
    Handle< L1EmParticleCollection > isoEmColl ;
    iEvent.getByLabel( isoEmSource_, isoEmColl ) ;
    cout << "Number of isolated EM " << isoEmColl->size() << endl ;
    float closestDrL1Em = 1000;
    float closestL1EmEnergy = -1;
    for( L1EmParticleCollection::const_iterator emItr = isoEmColl->begin() ;
        emItr != isoEmColl->end() ;
        ++emItr )
    {
        float etaL1 = emItr->eta();
        float phiL1 = emItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        float et = emItr->et();
        float energy = emItr->energy();
        float pt = emItr->pt();
        float p = emItr->p();
        cout << "EM IsoL1 eta: " << etaL1 << " phi: " << phiL1 << " dR: " << dR 
          << " energy: " << energy << " et: " << et << " pt: " << pt << endl;
        if(dR < closestDrL1Em)
        {
          closestDrL1Em = dR;
          closestL1EmEnergy = energy;
        }
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 1;
        }
    }
    L1IsoEmDrHist_->Fill(closestDrL1Em);
    if(closestDrL1Em < 1)
      L1IsoEmEnergyHist_->Fill(closestL1EmEnergy);
    L1DrVsObjTypeHist_->Fill(1,closestDrL1Em);

    // Non-isolated EM particles
    Handle< L1EmParticleCollection > nonIsoEmColl ;
    iEvent.getByLabel( nonIsoEmSource_, nonIsoEmColl ) ;
    cout << "Number of non-isolated EM " << nonIsoEmColl->size() << endl ;
    float closestDrL1EmNonIso = 1000;
    for( L1EmParticleCollection::const_iterator emItr = nonIsoEmColl->begin() ;
        emItr != nonIsoEmColl->end() ;
        ++emItr )
    {
        float etaL1 = emItr->eta();
        float phiL1 = emItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        float et = emItr->et();
        float energy = emItr->energy();
        float pt = emItr->pt();
        float p = emItr->p();
        cout << "EM NonIsoL1 eta: " << etaL1 << " phi: " << phiL1 << " dR: " << dR
          << " energy: " << energy << " et: " << et << " pt: " << pt << endl;
        if(dR < closestDrL1EmNonIso)
          closestDrL1EmNonIso = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 2;
        }
    }
    L1NonIsoEmDrHist_->Fill(closestDrL1EmNonIso);
    L1DrVsObjTypeHist_->Fill(2,closestDrL1EmNonIso);

    // Jet particles
    Handle< L1JetParticleCollection > cenJetColl ;
    iEvent.getByLabel( cenJetSource_, cenJetColl ) ;
    //cout << "Number of central jets " << cenJetColl->size() << endl ;
    float closestDrL1JetCen = 1000;
    for( L1JetParticleCollection::const_iterator jetItr = cenJetColl->begin() ;
        jetItr != cenJetColl->end() ;
        ++jetItr )
    {
        float etaL1 = jetItr->eta();
        float phiL1 = jetItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        if(dR < closestDrL1JetCen)
          closestDrL1JetCen = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 3;
        }
    }
    L1JetCenDrHist_->Fill(closestDrL1JetCen);
    L1DrVsObjTypeHist_->Fill(3,closestDrL1JetCen);

    Handle< L1JetParticleCollection > forJetColl ;
    iEvent.getByLabel( forJetSource_, forJetColl ) ;
    //cout << "Number of forward jets " << forJetColl->size() << endl ;
    float closestDrL1JetFor = 1000;
    for( L1JetParticleCollection::const_iterator jetItr = forJetColl->begin() ;
        jetItr != forJetColl->end() ;
        ++jetItr )
    {
        float etaL1 = jetItr->eta();
        float phiL1 = jetItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        if(dR < closestDrL1JetFor)
          closestDrL1JetFor = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 4;
        }
    }
    L1JetForDrHist_->Fill(closestDrL1JetFor);
    L1DrVsObjTypeHist_->Fill(4,closestDrL1JetFor);

    Handle< L1JetParticleCollection > tauColl ;
    iEvent.getByLabel( tauJetSource_, tauColl ) ;
    //cout << "Number of tau jets " << tauColl->size() << endl ;
    float closestDrL1JetTau = 1000;
    for( L1JetParticleCollection::const_iterator tauItr = tauColl->begin() ;
        tauItr != tauColl->end() ;
        ++tauItr )
    {
        float etaL1 = tauItr->eta();
        float phiL1 = tauItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        if(dR < closestDrL1JetTau)
          closestDrL1JetTau = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 5;
        }
    }
    L1JetTauDrHist_->Fill(closestDrL1JetTau);
    L1DrVsObjTypeHist_->Fill(5,closestDrL1JetTau);

    // Muon particles
    Handle< L1MuonParticleCollection > muColl ;
    iEvent.getByLabel( muonSource_, muColl ) ;
    //cout << "Number of muons " << muColl->size() << endl ;
    float closestDrL1Mu = 1000;
    for( L1MuonParticleCollection::const_iterator muItr = muColl->begin() ;
        muItr != muColl->end() ;
        ++muItr )
    {
        float etaL1 = muItr->eta();
        float phiL1 = muItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        if(dR < closestDrL1Mu)
          closestDrL1Mu = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 6;
        }
    }
    L1MuDrHist_->Fill(closestDrL1Mu);
    L1DrVsObjTypeHist_->Fill(6,closestDrL1Mu);

    // MET
    Handle< L1EtMissParticleCollection > etMissColl ;
    iEvent.getByLabel( etMissSource_, etMissColl ) ;
    float closestDrL1Etm = 1000;
    for( L1EtMissParticleCollection::const_iterator etmItr = etMissColl->begin() ;
        etmItr != etMissColl->end() ;
        ++etmItr )
    {
        float etaL1 = etmItr->eta();
        float phiL1 = etmItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        if(dR < closestDrL1Etm)
          closestDrL1Etm = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 7;
        }
    }
    L1METDrHist_->Fill(closestDrL1Etm);
    L1DrVsObjTypeHist_->Fill(7,closestDrL1Etm);

    // MHT
    Handle< L1EtMissParticleCollection > htMissColl ;
    iEvent.getByLabel( htMissSource_, htMissColl ) ;
    float closestDrL1Htm = 1000;
    for( L1EtMissParticleCollection::const_iterator htmItr = htMissColl->begin() ;
        htmItr != htMissColl->end() ;
        ++htmItr )
    {
        float etaL1 = htmItr->eta();
        float phiL1 = htmItr->phi();
        float dR = sqrt(pow(etaL1-cryEta,2)+pow(phiL1-cryPhi,2));
        if(dR < closestDrL1Htm)
          closestDrL1Htm = dR;
        if(dR < closestDrAll)
        {
          closestDrAll = dR;
          typeOfClosestL1All = 8;
        }
    }
    L1MHTDrHist_->Fill(closestDrL1Htm);
    L1DrVsObjTypeHist_->Fill(8,closestDrL1Htm);

    // Don't care about HF Rings...
    // HF Rings
    //Handle< L1HFRingsCollection > hfRingsColl ;
    //iEvent.getByLabel( hfRingsSource_, hfRingsColl ) ;
    //cout << "HF Rings:" << endl ;
    //for( int i = 0 ; i < L1HFRings::kNumRings ; ++i )
    //{
    //  cout << "  " << i << ": et sum = "
    //    << hfRingsColl->begin()->hfEtSum( (L1HFRings::HFRingLabels) i )
    //    << ", bit count = "
    //    << hfRingsColl->begin()->hfBitCount( (L1HFRings::HFRingLabels) i )
    //    << endl ;
    //}
    //cout << endl ;

    // Only fill these if the trigger object matches the RecoMuon
    if(closestDrAll <= 1)
    {
      L1ClosestL1ObjDrHist_->Fill(closestDrAll);
      L1ClosestL1ObjTypeHist_->Fill(typeOfClosestL1All);
    }

    // More code to look at the ECAL Trigger Primitives - SIC July 20 2010
    // Code from TPGAnalysis: UserCode/CCEcal/CRUZET2/CaloOnlineTools/EcalTools/plugins/EcalTPGAnalyzer.cc
    
    //// Only do this if the closest L1 object was an EM Object
    //if(typeOfClosestL1All > 2 || typeOfClosestL1All < 1)
    //  continue;
    // Only do this if an EM L1 object was close
    if(closestDrL1Em > 1 && closestDrL1EmNonIso > 1)
      continue;

    cout << "LOOKING AT TP DATA for closest L1 type: " << typeOfClosestL1All << " and dR=" << closestDrAll << endl;
    /////////////////////////// 
    // Get TP data  
    ///////////////////////////
    map<EcalTrigTowerDetId, towerEner> mapTower;
    map<EcalTrigTowerDetId, towerEner>::iterator itTT;
    edm::Handle<EcalTrigPrimDigiCollection> tp;
    iEvent.getByLabel(tpCollection_,tp);
    //std::cout<<"TP collection size="<<tp.product()->size()<<std::endl ;
    for(unsigned int i=0;i<tp.product()->size();i++)
    {
      EcalTriggerPrimitiveDigi d = (*(tp.product()))[i];
      const EcalTrigTowerDetId TPtowid= d.id();
      towerEner tE ;
      tE.iphi_ = TPtowid.iphi() ;
      tE.ieta_ = TPtowid.ieta() ;
      tE.tpgADC_ = (d[0].raw()&0xfff) ;
      mapTower[TPtowid] = tE ;
    }
    // Get EB xtal digi inputs
    EBDataFrame dfMaxEB ;
    for(unsigned int i=0;i<ebDigis.product()->size();i++)
    {
      const EBDataFrame & df = (*(ebDigis.product()))[i];
      const EBDetId & id = df.id();
      const EcalTrigTowerDetId towid = id.tower();
      bool RHfound = true;
      EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      while(recHitItr != ebRecHits->end() && (recHitItr->id() != id))
      {
        ++recHitItr;
      }
      if(recHitItr==ebRecHits->end())
        RHfound=false;

      itTT = mapTower.find(towid) ;
      if(itTT != mapTower.end())
      {
        double theta = theBarrelSubdetGeometry->getGeometry(id)->getPosition().theta() ;
        (itTT->second).etRec_ += recHitItr->energy()*sin(theta) ;
        (itTT->second).eRec_ += recHitItr->energy();
        (itTT->second).nbXtal_++ ;
        bool fill(false) ;
        if(((itTT->second).tpgADC_ & 0xff)>0) fill = true ;   
        if(fill)
        {
          float dR = sqrt(pow(id.ieta()-cryIEta,2)+pow(id.iphi()-cryIPhi,2));
          if(dR < 60 && RHfound)
          {
            cout<<"TP="<<((itTT->second).tpgADC_ & 0xff)<<" ieta="<<towid.ieta()<<" iphi="<<towid.iphi()<<endl ;
            cout<<"\tXtal ieta: " << id.ieta() << " Xtal iphi: " << id.iphi() << endl;
            cout<<"\tXtal energy: " << recHitItr->energy() << " time: " << recHitItr->time() << endl;
            cout<<"\tDistance from max energy HSCP cry in ieta/iphi coordinates: " << dR << endl;

            //treeVariablesShape_.ieta = towid.ieta() ;
            //treeVariablesShape_.iphi = towid.iphi() ;
            //treeVariablesShape_.ixXtal = id.iphi() ;
            //treeVariablesShape_.iyXtal = id.ieta() ;
            //treeVariablesShape_.TCCid = theMapping_->TCCid(towid);
            //treeVariablesShape_.TowerInTCC = theMapping_->iTT(towid);
            //const EcalTriggerElectronicsId elId = theMapping_->getTriggerElectronicsId(id) ;
            //treeVariablesShape_.strip = elId.pseudoStripId() ;
            //treeVariablesShape_.nbOfSamples = df.size() ;
            //for (int s=0 ; s<df.size() ; s++) treeVariablesShape_.samp[s] = df[s].adc() ; 
          }
        }
      }
    }
    //// Loop over towers
    //for(itTT = mapTower.begin(); itTT != mapTower.end(); ++itTT)
    //{
    //  if((itTT->second).eRec_ > 0.1)
    //    cout<<"TP="<<((itTT->second).tpgADC_ & 0xff)<<" eta="<<(itTT->second).ieta_*5*0.017<<" phi="<<(itTT->second).iphi_*5*0.017<<
    //      " RecEnergy:" << (itTT->second).eRec_ << " RecET: " << (itTT->second).etRec_ << endl;
    //}


    // End of L1 trigger analysis code




    //EBDigiCollection::const_iterator digiItr = ebDigis->begin();
    //while(digiItr != ebDigis->end() && digiItr->id() != (trackMatchedRecHits.end()-1)->id())
    //  ++digiItr;
    //if(digiItr==ebDigis->end())
    //  return;
    //EBDataFrame df(*digiItr);
    //double pedestal = 200;
    ////if(df.sample(0).gainId()!=1 || df.sample(1).gainId()!=1) return; //goes to the next digi
    ////else {
    ////  pedestal = (double)(df.sample(0).adc()+df.sample(1).adc())/(double)2;
    ////}
    //for (int i=0; (unsigned int)i< digiItr->size(); ++i ) {
    //  double gain = 12.;
    //  if(df.sample(i).gainId()==1)
    //    gain = 1.;
    //  else if(df.sample(i).gainId()==2)
    //    gain = 2.;

    //  sampleNumbers_.push_back(i);
    //  amplitudes_.push_back(pedestal+(df.sample(i).adc()-pedestal)*gain);
    //  //if(maxEnergyHit.time() > 14 || maxEnergyHit.time() < -2)
    //  //  std::cout << "\tStrange time: " << maxEnergyHit.time() << " hit; sample: " << i << " amplitude: " << df.sample(i).adc() << std::endl;

    //  //recHitMaxEnergyShapeProfile_->Fill(i,pedestal+(df.sample(i).adc()-pedestal)*gain);
    //}

  //  // Fill hit1 vs. hit2 hists
  //  // 1st hit is always the max. energy one
  //  if(numRecHitsFound==2)
  //  {
  //    std::map<int,float>::const_iterator firstCryItr = muonCrossedXtalCurvedMap.begin();
  //    std::map<int,float>::const_iterator secondCryItr = firstCryItr++;
  //    
  //    EcalRecHitCollection::const_iterator firstHit = ebRecHits->find(EBDetId(firstCryItr->first));
  //    if(firstHit!=ebRecHits->end())
  //    {
  //      EcalRecHitCollection::const_iterator secondHit = ebRecHits->find(EBDetId(secondCryItr->first));
  //      if(secondHit!=ebRecHits->end())
  //      {
  //        if(secondHit->energy() > firstHit->energy())
  //          swap(secondHit,firstHit);
  //        //energyFractionInTrackMatchedXtalsProfile_->Fill(0.,firstHit->energy()/crossedEnergy);
  //        //energyFractionInTrackMatchedXtalsProfile_->Fill(1.,secondHit->energy()/crossedEnergy);
  //        hit1EnergyVsHit2EnergyHist_->Fill(0.97*secondHit->energy(),0.97*firstHit->energy());
  //        hit1LengthVsHit2LengthHist_->Fill(secondCryItr->second,firstCryItr->second);
  //        hit1DeDxVsHit2DeDxHist_->Fill(1000*0.97*secondHit->energy()/secondCryItr->second,1000*0.97*firstHit->energy()/firstCryItr->second);
  //      }
  //    }
  //  }
  }

  // Set number of interesting tracks in the event
  myTreeVariables_.numTracks = trackIndex+1;

  myTree_->Fill();
}

// ------------ calcEcalDeposit method --------------------------------------------------
std::vector<SteppingHelixStateInfo> HSCPTimingAnalyzer::calcEcalDeposit(const FreeTrajectoryState* tkInnerState,
    const TrackAssociatorParameters& parameters)
{
  return calcDeposit(tkInnerState, parameters, *ecalDetIdAssociator_);
}


// ------------ calcDeposit method --------------------------------------------------
std::vector<SteppingHelixStateInfo> HSCPTimingAnalyzer::calcDeposit(const FreeTrajectoryState* tkInnerState,
    const TrackAssociatorParameters& parameters,
    const DetIdAssociator& associator)
{
  if (! parameters.useEcal && ! parameters.useCalo && ! parameters.useHcal &&
      ! parameters.useHO && ! parameters.useMuon )
    throw cms::Exception("ConfigurationError") << "Configuration error! No subdetector was selected for the track association.";

  // Set some params
  double minR = associator.volume().minR () ;
  double minZ = associator.volume().minZ () ;
  double maxR = associator.volume().maxR () ;
  double maxZ = associator.volume().maxZ () ;

  // Define the TrackOrigin (where the propagation starts)
  SteppingHelixStateInfo trackOrigin(*tkInnerState);

  // Define Propagator
  SteppingHelixPropagator* prop = new SteppingHelixPropagator (&*bField_, alongMomentum);
  prop -> setMaterialMode(false);
  prop -> applyRadX0Correction(true);

  // Build the necklace
  CachedTrajectory neckLace;
  neckLace.setStateAtIP(trackOrigin);
  neckLace.reset_trajectory();
  neckLace.setPropagator(prop);
  neckLace.setPropagationStep(0.1);
  neckLace.setMinDetectorRadius(minR);
  neckLace.setMinDetectorLength(minZ*2.);
  neckLace.setMaxDetectorRadius(maxR);
  neckLace.setMaxDetectorLength(maxZ*2.);
  //std::cout << "calcEcalDeposits::minDetectorRadius = " << minR << std::endl;
  //std::cout << "calcEcalDeposits::maxDetectorRadius = " << maxR << std::endl;
  //std::cout << "calcEcalDeposits::minDetectorLength = " << minZ << std::endl;
  //std::cout << "calcEcalDeposits::maxDetectorLength = " << maxZ << std::endl;

  // Propagate track
  bool isPropagationSuccessful = neckLace.propagateAll(trackOrigin);

  if (!isPropagationSuccessful)
  {
    std::cout << ">>>>>> calcEcalDeposits::propagateAll::failed " << "<<<<<<" << std::endl;
    //std::cout << "innerOrigin = " << glbTrackInnerOrigin.position() << "   innerR = " << innerR << std::endl; 
    return std::vector<SteppingHelixStateInfo> () ;
  }

  std::vector<SteppingHelixStateInfo> complicatePoints;
  neckLace.getTrajectory(complicatePoints, associator.volume(), 500);
  //std::cerr << "necklace size = " << complicatePoints.size() << std::endl;

  return complicatePoints;
}
      
// ------------ getDetailedTrackLength in Xtals -----------------------------------------
int HSCPTimingAnalyzer::getDetailedTrackLengthInXtals(std::map<int,float>& XtalInfo, 
    std::map<int,float>& muonCrossedXtalMap, 
    double& totalLengthCurved,
    GlobalPoint& internalPointCurved, 
    GlobalPoint& externalPointCurved,
    const CaloGeometry* theGeometry, 
    const CaloTopology * theTopology, 
    const std::vector<SteppingHelixStateInfo>& neckLace)
{
  //replace guts SIC OCT 11 2009
  GlobalPoint origin (0., 0., 0.);
  internalPointCurved = origin ;
  externalPointCurved = origin ;

  GlobalVector zero (0., 0., 0.);
  //internalMomentumCurved = zero ;
  //externalMomentumCurved = zero ;

  bool firstPoint = false;
  muonCrossedXtalMap.clear();

  const CaloSubdetectorGeometry* theBarrelSubdetGeometry = theGeometry->getSubdetectorGeometry(DetId::Ecal,1);
  const CaloSubdetectorGeometry* theEndcapSubdetGeometry = theGeometry->getSubdetectorGeometry(DetId::Ecal,2);

  for(std::vector<SteppingHelixStateInfo>::const_iterator itr = (neckLace.begin() + 1); itr != neckLace.end(); ++itr)
  {
    GlobalPoint probe_gp = (*itr).position();
    GlobalVector probe_gv = (*itr).momentum();
    std::vector<DetId> surroundingMatrix;

    EBDetId closestBarrelDetIdToProbe = ((theBarrelSubdetGeometry -> getClosestCell(probe_gp)).rawId());
    EEDetId closestEndcapDetIdToProbe = ((theEndcapSubdetGeometry -> getClosestCell(probe_gp)).rawId());

    // check if the probe is inside the xtal
    if( (closestEndcapDetIdToProbe) && (theGeometry->getSubdetectorGeometry(closestEndcapDetIdToProbe)->
          getGeometry(closestEndcapDetIdToProbe)->inside(probe_gp)) )
    {
      //std::cout << "ENDCAP " << std::endl;
      //std::cout << "point = " << probe_gp << "   eta = " << probe_gp.eta() << std::endl;
      double step = ((*itr).position() - (*(itr-1)).position()).mag();
      addStepToXtal(XtalInfo, muonCrossedXtalMap, closestEndcapDetIdToProbe,step);
      totalLengthCurved += step;

      if (firstPoint == false)
      {
        internalPointCurved = probe_gp ;
        //internalMomentumCurved = probe_gv ;
        firstPoint = true ;
      }

      externalPointCurved = probe_gp ;
      //externalMomentumCurved = probe_gv ;
    }

    if( (closestBarrelDetIdToProbe) && (theGeometry->getSubdetectorGeometry(closestBarrelDetIdToProbe)->
          getGeometry(closestBarrelDetIdToProbe)->inside(probe_gp)) )
    {
      //std::cout << "BARREL " << std::endl;
      double step = ((*itr).position() - (*(itr-1)).position()).mag();
      addStepToXtal(XtalInfo, muonCrossedXtalMap, closestBarrelDetIdToProbe,step);
      totalLengthCurved += step;

      if (firstPoint == false)
      {
        internalPointCurved = probe_gp ;
        //internalMomentumCurved = probe_gv ;
        firstPoint = true ;
      }

      externalPointCurved = probe_gp ;
      //externalMomentumCurved = probe_gv ;
    }

    else
    {
      // 3x3 matrix surrounding the probe
      surroundingMatrix = matrixDetId( theTopology, closestBarrelDetIdToProbe, -1, 1, -1, 1 );

      for( unsigned int k=0; k<surroundingMatrix.size(); ++k ) {
        if(theGeometry->getSubdetectorGeometry(surroundingMatrix.at(k))->getGeometry(surroundingMatrix.at(k))->inside(probe_gp))  
        {
          double step = ((*itr).position() - (*(itr-1)).position()).mag();
          addStepToXtal(XtalInfo, muonCrossedXtalMap, surroundingMatrix[k],step);
          totalLengthCurved += step;

          if (firstPoint == false)
          {
            internalPointCurved = probe_gp ;
            //internalMomentumCurved = probe_gv ;
            firstPoint = true ;
          }

          externalPointCurved = probe_gp ;
          //externalMomentumCurved = probe_gv ;
        }
      }

      // clear neighborhood matrix
      surroundingMatrix.clear();
    }
  }

  return 0;

}

// ------------ addStepToXtal -----------------------------------------------------------
void HSCPTimingAnalyzer::addStepToXtal(std::map<int,float>& xtalsMap,
    std::map<int,float>& muonCrossedXtalMap,
    DetId aDetId,
    float step)
{
  std::map<int,float>::iterator xtal = xtalsMap.find(aDetId.rawId());
  if (xtal!=xtalsMap.end())
    ((*xtal).second)+=step;
  else
    xtalsMap.insert(std::pair<int,float>(aDetId.rawId(),step));

  xtal = muonCrossedXtalMap.find(aDetId.rawId());
  if (xtal!= muonCrossedXtalMap.end())
    ((*xtal).second)+=step;
  else
    muonCrossedXtalMap.insert(std::pair<int,float>(aDetId.rawId(),step));

}

// ------------ matrixDetId method ------------------------------------------------------
std::vector<DetId> HSCPTimingAnalyzer::matrixDetId(const CaloTopology* topology, 
    DetId id, 
    int ixMin,
    int ixMax,
    int iyMin,
    int iyMax)
{
  CaloNavigator<DetId> cursor = CaloNavigator<DetId>(id, topology->getSubdetectorTopology(id));
  std::vector<DetId> v;
  for(int i = ixMin; i<=ixMax; ++i)
  {
    for(int j = iyMin; j <= iyMax; ++j)
    {
      //cout << "i = " << i << "; j = " << j << endl;
      cursor.home();
      cursor.offsetBy(i, j);
      if(*cursor != DetId(0))
        v.push_back(*cursor);
    }
  }
  return v;
}
                      

// ------------ method called once each job just before starting event loop  -------------
void 
HSCPTimingAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HSCPTimingAnalyzer::endJob()
{
   //recHitMaxEnergyShapeGraph_ = fileService->make<TGraph>(sampleNumbers_.size(), &(*sampleNumbers_.begin()),&(*amplitudes_.begin()));
   //recHitMaxEnergyShapeGraph_->SetTitle("Shape of max energy hits (ADC)");
   //recHitMaxEnergyShapeGraph_->SetName("recHitMaxEnergyShapeGraph");

   //energyVsDeDxHist_->Write();
   ////XXX: SIC TURNED OFF B/C NOT NEEDED
   ////for(int i=0;i<25;++i)
   ////  energyInDeDxSlice_[i]->Write();
   //
   //energyAndTimeNTuple_->Write();
   //file_->Close();
}

// ------------ Get the string from an int -----------------------------------------------
std::string HSCPTimingAnalyzer::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

