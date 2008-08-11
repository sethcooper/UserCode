// -*- C++ -*-
//
// Package:   EcalTimingAnalyzer 
// Class:     EcalTimingAnalyzer  
// 
/**\class EcalTimingAnalyzer EcalTimingAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/

 
#include "Analyzers/EcalTimingAnalyzer/interface/EcalTimingAnalyzer.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

#include <vector>

 
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
EcalTimingAnalyzer::EcalTimingAnalyzer(const edm::ParameterSet& iConfig) :
  ecalRecHitCollection_ (iConfig.getParameter<edm::InputTag>("ecalRecHitCollection")),
  barrelClusterCollection_ (iConfig.getParameter<edm::InputTag>("barrelClusterCollection")),
  endcapClusterCollection_ (iConfig.getParameter<edm::InputTag>("endcapClusterCollection")),
  l1GTReadoutRecTag_ (iConfig.getUntrackedParameter<std::string>("L1GlobalReadoutRecord","gtDigis")),
  l1GMTReadoutRecTag_ (iConfig.getUntrackedParameter<std::string>("L1GlobalMuonReadoutRecord","gtDigis")),
  runNum_(-1),
  histRangeMax_ (iConfig.getUntrackedParameter<double>("histogramMaxRange",1.8)),
  histRangeMin_ (iConfig.getUntrackedParameter<double>("histogramMinRange",0.0)),
  minTimingAmp_ (iConfig.getUntrackedParameter<double>("MinTimingAmp",.100)),
  fileName_ (iConfig.getUntrackedParameter<std::string>("fileName", std::string("ecalCosmicHists"))),
  runInFileName_(iConfig.getUntrackedParameter<bool>("runInFileName",true))
{
  naiveEvtNum_ = 0;
  cosmicCounter_ = 0;
  cosmicCounterTopBottom_ = 0;

  // TrackAssociator parameters
  edm::ParameterSet trkParameters = iConfig.getParameter<edm::ParameterSet>("TrackAssociatorParameters");
  trackParameters_.loadParameters( trkParameters );
  trackAssociator_.useDefaultPropagator();

  string title1 = "Seed Energy for All Feds; Seed Energy (GeV)";
  string name1 = "SeedEnergyAllFEDs";
  int numBins = 200;//(int)round(histRangeMax_-histRangeMin_)+1;
  allFedsHist_ = new TH1F(name1.c_str(),title1.c_str(),numBins,histRangeMin_,histRangeMax_);

  fedMap_ = new EcalFedMap();



}


EcalTimingAnalyzer::~EcalTimingAnalyzer()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
  void
EcalTimingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  int ievt = iEvent.id().event();
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);

  edm::Handle<reco::SuperClusterCollection> bscHandle;  
  edm::Handle<reco::SuperClusterCollection> escHandle;  

  naiveEvtNum_++;

  //  cout << "  My Event: " << naiveEvtNum_ << " " << iEvent.id().run() << " " << iEvent.id().event() << " " << iEvent.time().value() << endl;
  //  cout << "Timestamp: " << iEvent.id().run() << " " << iEvent.id().event() << " " << iEvent.time().value() << endl;

  iEvent.getByLabel(barrelClusterCollection_, bscHandle);
  if (!(bscHandle.isValid())) 
  {
    LogWarning("EcalTimingAnalyzer") << barrelClusterCollection_ << " not available";
    return;
  }
  LogDebug("EcalTimingAnalyzer") << "event " << ievt;

  iEvent.getByLabel(endcapClusterCollection_, escHandle);
  if (!(escHandle.isValid())) 
  {
    LogWarning("EcalTimingAnalyzer") << endcapClusterCollection_ << " not available";
    //return;
  }

  Handle<EcalRecHitCollection> hits;
  iEvent.getByLabel(ecalRecHitCollection_, hits);
  if (!(escHandle.isValid())) 
  {
    LogWarning("EcalTimingAnalyzer") << ecalRecHitCollection_ << " not available";
    //return;
  }


  // get the GMTReadoutCollection
  Handle<L1MuGMTReadoutCollection> gmtrc_handle; 
  iEvent.getByLabel(l1GMTReadoutRecTag_,gmtrc_handle);
  L1MuGMTReadoutCollection const* gmtrc = gmtrc_handle.product();
  if (!(gmtrc_handle.isValid())) 
  {
    LogWarning("EcalTimingAnalyzer") << "l1MuGMTReadoutCollection" << " not available";
    //return;
  }


  // get hold of L1GlobalReadoutRecord
  Handle<L1GlobalTriggerReadoutRecord> L1GTRR;
  iEvent.getByLabel(l1GTReadoutRecTag_,L1GTRR);
  bool isEcalL1 = false;
  const unsigned int sizeOfDecisionWord(L1GTRR->decisionWord().size());
  if (!(L1GTRR.isValid()))
  {
    LogWarning("EcalTimingAnalyzer") << l1GTReadoutRecTag_ << " not available";
    //return;
  }
  else if(sizeOfDecisionWord<128)
  {
    LogWarning("EcalTimingAnalyzer") << "size of L1 decisionword is " << sizeOfDecisionWord
      << "; L1 Ecal triggering bits not available";
  } 
  else
  {
    l1Names_.resize(sizeOfDecisionWord);
    l1Accepts_.resize(sizeOfDecisionWord);
    for (unsigned int i=0; i!=sizeOfDecisionWord; ++i) {
      l1Accepts_[i]=0;
      l1Names_[i]="NameNotAvailable";
    }
    for (unsigned int i=0; i!=sizeOfDecisionWord; ++i) {
      if (L1GTRR->decisionWord()[i])
      {
        l1Accepts_[i]++;
        //cout << "L1A bit: " << i << endl;
      }
    }

    if(l1Accepts_[14] || l1Accepts_[15] || l1Accepts_[16] || l1Accepts_[17]
        || l1Accepts_[18] || l1Accepts_[19] || l1Accepts_[20])
      isEcalL1 = true;
    if(l1Accepts_[73] || l1Accepts_[74] || l1Accepts_[75] || l1Accepts_[76]
        || l1Accepts_[77] || l1Accepts_[78])
      isEcalL1 = true;
  } 

  bool isRPCL1 = false;
  bool isDTL1 = false;
  bool isCSCL1 = false;
  bool isHCALL1 = false;

  std::vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  std::vector<L1MuGMTReadoutRecord>::const_iterator igmtrr;

  for(igmtrr=gmt_records.begin(); igmtrr!=gmt_records.end(); igmtrr++) {
    std::vector<L1MuRegionalCand>::const_iterator iter1;
    std::vector<L1MuRegionalCand> rmc;
    //DT triggers
    int idt = 0;
    rmc = igmtrr->getDTBXCands();
    for(iter1=rmc.begin(); iter1!=rmc.end(); iter1++) {
      if ( !(*iter1).empty() ) {
        idt++;
      }
    }
    //if(idt>0) std::cout << "Found " << idt << " valid DT candidates in bx wrt. L1A = " 
    //  << igmtrr->getBxInEvent() << std::endl;
    if(igmtrr->getBxInEvent()==0 && idt>0) isDTL1 = true;

    //RPC triggers
    int irpcb = 0;
    rmc = igmtrr->getBrlRPCCands();
    for(iter1=rmc.begin(); iter1!=rmc.end(); iter1++) {
      if ( !(*iter1).empty() ) {
        irpcb++;
      }
    }
    //if(irpcb>0) std::cout << "Found " << irpcb << " valid RPC candidates in bx wrt. L1A = " 
    //  << igmtrr->getBxInEvent() << std::endl;
    if(igmtrr->getBxInEvent()==0 && irpcb>0) isRPCL1 = true;

    //CSC Triggers
    int icsc = 0;
    rmc = igmtrr->getCSCCands();
    for(iter1=rmc.begin(); iter1!=rmc.end(); iter1++) {
      if ( !(*iter1).empty() ) {
        icsc++;
      }
    }
    //if(icsc>0) std::cout << "Found " << icsc << " valid CSC candidates in bx wrt. L1A = " 
    //  << igmtrr->getBxInEvent() << std::endl;
    if(igmtrr->getBxInEvent()==0 && icsc>0) isCSCL1 = true;
  }


  L1GlobalTriggerReadoutRecord const* gtrr = L1GTRR.product();
  for(int ibx=-1; ibx<=1; ibx++) {
    bool hcal_top = false;
    bool hcal_bot = false;
    const L1GtPsbWord psb = gtrr->gtPsbWord(0xbb0d,ibx);
    std::vector<int> valid_phi;
    if((psb.aData(4)&0x3f) >= 1) {valid_phi.push_back( (psb.aData(4)>>10)&0x1f ); }
    if((psb.bData(4)&0x3f) >= 1) {valid_phi.push_back( (psb.bData(4)>>10)&0x1f ); }
    if((psb.aData(5)&0x3f) >= 1) {valid_phi.push_back( (psb.aData(5)>>10)&0x1f ); }
    if((psb.bData(5)&0x3f) >= 1) {valid_phi.push_back( (psb.bData(5)>>10)&0x1f ); }
    std::vector<int>::const_iterator iphi;
    for(iphi=valid_phi.begin(); iphi!=valid_phi.end(); iphi++) {
      //std::cout << "Found HCAL mip with phi=" << *iphi << " in bx wrt. L1A = " << ibx << std::endl;
      if(*iphi<9) hcal_top=true;
      if(*iphi>8) hcal_bot=true;
    }
    if(ibx==0 && hcal_top && hcal_bot) isHCALL1=true;
  }
  std::cout << "**** Trigger Source ****" << std::endl;
  if(isDTL1) std::cout << "DT" << std::endl;
  if(isRPCL1) std::cout << "RPC" << std::endl;
  if(isCSCL1) cout << "CSC" << endl;
  if(isHCALL1) std::cout << "HCAL" << std::endl;
  if(isEcalL1) std::cout << "ECAL" << std::endl;
  std::cout << "************************" << std::endl;

  if(runNum_==-1)
  {
    runNum_ = iEvent.id().run();
  }      

  int  numberOfCosmics = 0;

  int  numberOfCosmicsTop = 0;
  int  numberOfCosmicsBottom = 0;

  std::vector<EBDetId> seeds;

  // now loop over super clusters!  
  const reco::SuperClusterCollection *clusterCollection_p = bscHandle.product();
  for (reco::SuperClusterCollection::const_iterator clus = clusterCollection_p->begin(); clus != clusterCollection_p->end(); ++clus)
  {
    double energy = clus->energy();
    double phi    = clus->phi();
    double eta    = clus->eta();
    double time = -1000.0;
    double ampli = 0.;
    double secondMin = 0.;
    double secondTime = -1000.;
    int numXtalsinCluster = 0;
    EBDetId maxDet;
    EBDetId secDet;

    std::vector<DetId> clusterDetIds = clus->getHitsByDetId();
    for(std::vector<DetId>::const_iterator detitr = clusterDetIds.begin(); detitr != clusterDetIds.end(); ++detitr)
    {
      if ((*detitr).det() != DetId::Ecal) { std::cout << " det is " <<(*detitr).det() << std::endl;continue;}
      if ((*detitr).subdetId() != EcalBarrel) {std::cout << " subdet is " <<(*detitr).subdetId() << std::endl; continue; }
      EcalRecHitCollection::const_iterator thishit = hits->find((*detitr));
      if (thishit == hits->end()) 
        continue;

      EcalRecHit myhit = (*thishit);
      double thisamp = myhit.energy();
      if (thisamp > 0.027) {numXtalsinCluster++; }
      if (thisamp > secondMin) {secondMin = thisamp; secondTime = myhit.time(); secDet = (EBDetId)(*detitr);}
      if (secondMin > ampli) {std::swap(ampli,secondMin); std::swap(time,secondTime); std::swap(maxDet,secDet);}
    }

    EcalElectronicsId elecId = ecalElectronicsMap->getElectronicsId((EBDetId) maxDet);
    int FEDid = 600+elecId.dccId();
    numberOfCosmics++;

    //Set some more values
    seeds.push_back(maxDet);
    int ieta = maxDet.ieta();
    int iphi = maxDet.iphi();
    //int ietaSM = maxDet.ietaSM();
    //int iphiSM = maxDet.iphiSM();

    int LM = ecalElectronicsMap->getLMNumber(maxDet) ;//FIX ME
    // print out some info
    //     LogWarning("EcalTimingAnalyzer") << "hit! " << " amp " << ampli  << " : " 
    //       //<< fedMap_->getSliceFromFed(FEDid) 
    //       //<< " : ic " <<  ic << " : hashedIndex " << hashedIndex 
    //				    << " : ieta " << ieta << " iphi " << iphi 
    //				    << " : nCosmics " << " " << cosmicCounter_ << " / " << naiveEvtNum_ << endl;      
    // top and bottom clusters
    if (iphi>0&&iphi<180) { 
      numberOfCosmicsTop++;
    } else {
      numberOfCosmicsBottom++;
    }      

    allFedsHist_->Fill(ampli);
    allFedsenergyHist_->Fill(energy);

    allFedsFrequencyHist_->Fill(naiveEvtNum_);
    allFedsiPhiProfileHist_->Fill(iphi);
    allFedsiEtaProfileHist_->Fill(ieta);
    allOccupancy_->Fill(iphi, ieta);
    TrueOccupancy_->Fill(phi, eta);
    allOccupancyCoarse_->Fill(iphi, ieta);
    TrueOccupancyCoarse_->Fill(phi, eta);

    // Exclusive trigger plots

    if(isEcalL1&&!isDTL1&&!isRPCL1&&!isCSCL1&&!isHCALL1) {
      allOccupancyExclusiveECAL_->Fill(iphi, ieta);
      allOccupancyCoarseExclusiveECAL_->Fill(iphi, ieta);
      if (ampli > minTimingAmp_) {
        allFedsTimingHistECAL_->Fill(time);
        allFedsTimingPhiEtaHistECAL_->Fill(iphi,ieta,time);
        allFedsTimingTTHistECAL_->Fill(iphi,ieta,time);
        allFedsTimingLMHistECAL_->Fill(LM,time);
      }      
    } 

    if(!isEcalL1&&!isDTL1&&!isRPCL1&&!isCSCL1&&isHCALL1) {   
      if (ampli > minTimingAmp_) {
        allFedsTimingHistHCAL_->Fill(time);
      }      
    }

    if(!isEcalL1&&isDTL1&&!isRPCL1&&!isCSCL1&&!isHCALL1) {
      if (ampli > minTimingAmp_) {
        allFedsTimingHistDT_->Fill(time);
      }      
    }

    if(!isEcalL1&&!isDTL1&&isRPCL1&&!isCSCL1&&!isHCALL1) {
      if (ampli > minTimingAmp_) {
        allFedsTimingHistRPC_->Fill(time);
      }      
    }

    if(!isEcalL1&&!isDTL1&&!isRPCL1&&isCSCL1&&!isHCALL1) {   
      if (ampli > minTimingAmp_) {
        allFedsTimingHistCSC_->Fill(time);
      }      
    }

    // Fill histo for Ecal+muon coincidence
    //if(isEcalL1&&(isCSCL1||isRPCL1||isDTL1)&&!isHCALL1)
    //  allFedsTimingHistEcalMuon_->Fill(time);

    if (ampli > minTimingAmp_) {
      allFedsTimingHist_->Fill(time);
      allFedsTimingVsAmpHist_->Fill(time, ampli);
      allFedsTimingVsFreqHist_->Fill(time, naiveEvtNum_);
      allFedsTimingPhiHist_->Fill(iphi,time);
      allFedsTimingPhiEtaHist_->Fill(iphi,ieta,time);
      allFedsTimingTTHist_->Fill(iphi,ieta,time);
      allFedsTimingLMHist_->Fill(LM,time);
      if (FEDid>=610&&FEDid<=627)  allFedsTimingPhiEbmHist_->Fill(iphi,time);
      if (FEDid>=628&&FEDid<=645)  allFedsTimingPhiEbpHist_->Fill(iphi,time);

      if (FEDid>=610&&FEDid<=627)  allFedsTimingEbmHist_->Fill(time);
      if (FEDid>=628&&FEDid<=645)  allFedsTimingEbpHist_->Fill(time);
      if (FEDid>=613&&FEDid<=616)  allFedsTimingEbmTopHist_->Fill(time);
      if (FEDid>=631&&FEDid<=634)  allFedsTimingEbpTopHist_->Fill(time);
      if (FEDid>=622&&FEDid<=625)  allFedsTimingEbmBottomHist_->Fill(time);
      if (FEDid>=640&&FEDid<=643)  allFedsTimingEbpBottomHist_->Fill(time);
    }

  }//++++++++++++++++++END LOOP OVER SUPERCLUSTERS+++++++++++++++++++++++++++++++++++

  numberofCosmicsHist_->Fill(numberOfCosmics);
  if (numberOfCosmics > 0)
    cosmicCounter_++;
  

  // *** TrackAssociator *** //

  // get reco tracks 
  edm::Handle<reco::TrackCollection> recoTracks;
  iEvent.getByLabel("cosmicMuons", recoTracks);  

  if ( recoTracks.isValid() ) {
    //    LogWarning("EcalTimingAnalyzer") << "... Valid TrackAssociator recoTracks !!! " << recoTracks.product()->size();
    std::map<int,std::vector<DetId> > trackDetIdMap;
    int tracks = 0;
    for(reco::TrackCollection::const_iterator recoTrack = recoTracks->begin(); recoTrack != recoTracks->end(); ++recoTrack){

      if(fabs(recoTrack->d0())>70 || fabs(recoTrack->dz())>70)
        continue;
      if(recoTrack->numberOfValidHits()<20)
        continue;

      //if (recoTrack->pt() < 2) continue; // skip low Pt tracks       

      TrackDetMatchInfo info = trackAssociator_.associate(iEvent, iSetup, *recoTrack, trackParameters_);      

      //       edm::LogVerbatim("TrackAssociator") << "\n-------------------------------------------------------\n Track (pt,eta,phi): " <<
      //    recoTrack->pt() << " , " << recoTrack->eta() << " , " << recoTrack->phi() ;
      //       edm::LogVerbatim("TrackAssociator") << "Ecal energy in crossed crystals based on RecHits: " <<
      //    info.crossedEnergy(TrackDetMatchInfo::EcalRecHits);
      //       edm::LogVerbatim("TrackAssociator") << "Ecal energy in 3x3 crystals based on RecHits: " <<
      //    info.nXnEnergy(TrackDetMatchInfo::EcalRecHits, 1);
      //       edm::LogVerbatim("TrackAssociator") << "Hcal energy in crossed towers based on RecHits: " <<
      //    info.crossedEnergy(TrackDetMatchInfo::HcalRecHits);
      //       edm::LogVerbatim("TrackAssociator") << "Hcal energy in 3x3 towers based on RecHits: " <<
      //    info.nXnEnergy(TrackDetMatchInfo::HcalRecHits, 1);
      //       edm::LogVerbatim("TrackAssociator") << "Number of muon segment matches: " << info.numberOfSegments();

      //       std::cout << "\n-------------------------------------------------------\n Track (pt,eta,phi): " <<
      //    recoTrack->pt() << " , " << recoTrack->eta() << " , " << recoTrack->phi() << std::endl;
      //       std::cout << "Ecal energy in crossed crystals based on RecHits: " <<
      //    info.crossedEnergy(TrackDetMatchInfo::EcalRecHits) << std::endl;
      //       std::cout << "Ecal energy in 3x3 crystals based on RecHits: " <<
      //    info.nXnEnergy(TrackDetMatchInfo::EcalRecHits, 1) << std::endl;
      //       std::cout << "Hcal energy in crossed towers based on RecHits: " <<
      //    info.crossedEnergy(TrackDetMatchInfo::HcalRecHits) << std::endl;
      //       std::cout << "Hcal energy in 3x3 towers based on RecHits: " <<
      //    info.nXnEnergy(TrackDetMatchInfo::HcalRecHits, 1) << std::endl;

      for (unsigned int i=0; i<info.crossedEcalIds.size(); i++) {	 
        // only checks for barrel
        if (info.crossedEcalIds[i].det() == DetId::Ecal && info.crossedEcalIds[i].subdetId() == 1) {	     
          EBDetId ebDetId (info.crossedEcalIds[i]);	   
          trackAssoc_muonsEcal_->Fill(ebDetId.iphi(), ebDetId.ieta());
          //std::cout << "Crossed iphi: " << ebDetId.iphi() 
          //    << " ieta: " << ebDetId.ieta() << " : nCross " << info.crossedEcalIds.size() << std::endl;

          EcalRecHitCollection::const_iterator thishit = hits->find(ebDetId);
          if (thishit == hits->end()) continue;

          EcalRecHit myhit = (*thishit);	 
          //double thisamp = myhit.energy();
          //std::cout << " Crossed energy: " << thisamp << " : nCross " << info.crossedEcalIds.size() << std::endl;	  
        }
      }

      //edm::LogVerbatim("TrackAssociator") << " crossedEcalIds size: " << info.crossedEcalIds.size()
      //				  << " crossedEcalRecHits size: " << info.crossedEcalRecHits.size();
      numberofCrossedEcalIdsHist_->Fill(info.crossedEcalIds.size());
      tracks++;
      if(info.crossedEcalIds.size()>0)
        trackDetIdMap.insert(std::pair<int,std::vector<DetId> > (tracks,info.crossedEcalIds));
    }      

    // Now to match recoTracks with cosmic clusters

    int numAssocTracks = 0;
    int numAssocClusters = 0;
    edm::LogVerbatim("TrackAssociator") << "Matching cosmic clusters to tracks...";
    int numSeeds = seeds.size();
    int numTracks = trackDetIdMap.size();
    while(seeds.size() > 0 && trackDetIdMap.size() > 0)
    {
      double bestDr = 1000;
      double bestDPhi = 1000;
      double bestDEta = 1000;
      double bestEtaTrack = 1000;
      double bestEtaSeed = 1000;
      double bestPhiTrack = 1000;
      double bestPhiSeed = 1000;
      EBDetId bestTrackDet;
      EBDetId bestSeed;
      int bestTrack;
      std::map<EBDetId,EBDetId> trackDetIdToSeedMap;

      //edm::LogVerbatim("TrackAssociator") << "NumTracks:" << trackDetIdMap.size() << " numClusters:" << seeds.size();

      for(std::vector<EBDetId>::const_iterator seedItr = seeds.begin(); seedItr != seeds.end(); ++seedItr)
      {
        for(std::map<int,std::vector<DetId> >::const_iterator mapItr = trackDetIdMap.begin();
            mapItr != trackDetIdMap.end(); ++mapItr) {
          for(unsigned int i=0; i<mapItr->second.size(); i++) {
            // only checks for barrel
            if(mapItr->second[i].det() == DetId::Ecal && mapItr->second[i].subdetId() == 1)
            {
              EBDetId ebDet = (mapItr->second[i]);
              double seedEta = seedItr->ieta();
              double deta = ebDet.ieta()-seedEta;
              if(seedEta * ebDet.ieta() < 0 )
                deta > 0 ? (deta=deta-1.) : (deta=deta+1.); 
              double dR;
              double dphi = ebDet.iphi()-seedItr->iphi();
              if (abs(dphi) > 180)
                dphi > 0 ?  (dphi=360-dphi) : (dphi=-360-dphi);
              dR = sqrt(deta*deta + dphi*dphi);
              if(dR < bestDr)
              {
                bestDr = dR;
                bestDPhi = dphi;
                bestDEta = deta;
                bestTrackDet = mapItr->second[i];
                bestTrack = mapItr->first;
                bestSeed = (*seedItr);
                bestEtaTrack = ebDet.ieta();
                bestEtaSeed = seedEta;
                bestPhiTrack = ebDet.iphi();
                bestPhiSeed = seedItr->iphi();
              }
            }
          }
        }
      }
      if(bestDr < 1000)
      {
        //edm::LogVerbatim("TrackAssociator") << "Best deltaR from matched DetId's to cluster:" << bestDr;
        deltaRHist_->Fill(bestDr);
        deltaPhiHist_->Fill(bestDPhi);
        deltaEtaHist_->Fill(bestDEta);
        deltaEtaDeltaPhiHist_->Fill(bestDEta,bestDPhi);
        seedTrackEtaHist_->Fill(bestEtaSeed,bestEtaTrack);
        seedTrackPhiHist_->Fill(bestPhiSeed,bestPhiTrack);
        seeds.erase(find(seeds.begin(),seeds.end(), bestSeed));
        trackDetIdMap.erase(trackDetIdMap.find(bestTrack));
        trackDetIdToSeedMap[bestTrackDet] = bestSeed;
        numAssocTracks++;
        numAssocClusters++;
      }
      else {
        edm::LogVerbatim("TrackAssociator") << "could not match cluster seed to track." << bestDr;
        break; // no match found
      }
    }
    if(numSeeds>0 && numTracks>0) {
      ratioAssocClustersHist_->AddBinContent(1,numAssocClusters);
      ratioAssocClustersHist_->AddBinContent(2,numSeeds);
    }
    if(numTracks>0) {
      ratioAssocTracksHist_->AddBinContent(1,numAssocTracks);
      ratioAssocTracksHist_->AddBinContent(2,numTracks);
      numberofCosmicsWTrackHist_->Fill(numSeeds);
    }
  } else {
    //    LogWarning("EcalTimingAnalyzer") << "!!! No TrackAssociator recoTracks !!!";

  }

  // *** end of TrackAssociator code *** //

}


// ------------ method called once each job just before starting event loop  ------------
  void 
EcalTimingAnalyzer::beginJob(const edm::EventSetup&)
{
  //Here I will init some of the specific histograms
  int numBins = 200;//(int)round(histRangeMax_-histRangeMin_)+1;

  //=============Special Bins for TT and Modules borders=============================
  double ttEtaBins[36] = {-85, -80, -75, -70, -65, -60, -55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 1, 6, 11, 16, 21, 26, 31, 36, 41, 46, 51, 56, 61, 66, 71, 76, 81, 86 };
  double modEtaBins[10]={-85, -65, -45, -25, 0, 1, 26, 46, 66, 86};
  double ttPhiBins[73];
  double modPhiBins[19];
  double timingBins[79];
  double highEBins[11];
  for (int i = 0; i < 79; ++i)
  {
    timingBins[i]=-7.+double(i)*14./78.;
    if (i<73) 
    {
      ttPhiBins[i]=1+5*i;
      if ( i < 19) 
      {
        modPhiBins[i]=1+20*i;
        if (i < 11)
        {
          highEBins[i]=10.+double(i)*20.;
        }
      }
    }

  }
  //=============END Special Bins for TT and Modules borders===========================

  allFedsenergyHist_           = new TH1F("energy_AllClusters","energy_AllClusters;Cluster Energy (GeV)",numBins,histRangeMin_,histRangeMax_);
  allFedsTimingHist_       = new TH1F("timeForAllFeds","timeForAllFeds;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingVsAmpHist_  = new TH2F("timeVsAmpAllEvents","time Vs Amp All Events;Relative Time (1 clock = 25ns);Amplitude (GeV)",78,-7,7,numBins,histRangeMin_,histRangeMax_);

  allOccupancy_            = new TH2F("OccupancyAllEvents","Occupancy all events;i#phi;i#eta",360,1.,361.,172,-86,86);
  TrueOccupancy_            = new TH2F("TrueOccupancyAllEvents","True Occupancy all events;#phi;#eta",360,-3.14159,3.14159,172,-1.5,1.5);
  allOccupancyCoarse_      = new TH2F("OccupancyAllEventsCoarse","Occupancy all events Coarse;i#phi;i#eta",360/5,1,361.,35,ttEtaBins);
  TrueOccupancyCoarse_      = new TH2F("TrueOccupancyAllEventsCoarse","True Occupancy all events Coarse;#phi;#eta",360/5,-3.14159,3.14159,34,-1.5,1.5);

  allFedsTimingPhiHist_          = new TH2F("timePhiAllFEDs","time vs Phi for all FEDs (TT binning);i#phi;Relative Time (1 clock = 25ns)",72,1,361,78,-7,7);
  allFedsTimingPhiEbpHist_       = new TH2F("timePhiEBP","time vs Phi for FEDs in EB+ (TT binning) ;i#phi;Relative Time (1 clock = 25ns)",72,1,361,78,-7,7);
  allFedsTimingPhiEbmHist_       = new TH2F("timePhiEBM","time vs Phi for FEDs in EB- (TT binning);i#phi;Relative Time (1 clock = 25ns)",72,1,361,78,-7,7);
  allFedsTimingPhiEtaHist_       = new TH3F("timePhiEtaAllFEDs","(Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,modPhiBins,9,modEtaBins,78,timingBins);  
  allFedsTimingTTHist_           = new TH3F("timeTTAllFEDs","(Phi,Eta,time) for all FEDs (SM,TT binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",72,ttPhiBins,35,ttEtaBins,78,timingBins); 
  allFedsTimingLMHist_           = new TH2F("timeLMAllFEDs","(LM,time) for all FEDs (SM,LM binning);LM;Relative Time (1 clock = 25ns)",92, 1, 92,78,-7,7);

  allFedsTimingEbpHist_       = new TH1F("timeEBP","time for FEDs in EB+;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbmHist_       = new TH1F("timeEBM","time for FEDs in EB-;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbpTopHist_    = new TH1F("timeEBPTop","time for FEDs in EB+ Top;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbmTopHist_    = new TH1F("timeEBMTop","time for FEDs in EB- Top;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbpBottomHist_ = new TH1F("timeEBPBottom","time for FEDs in EB+ Bottom;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingEbmBottomHist_ = new TH1F("timeEBMBottom","time for FEDs in EB- Bottom;Relative Time (1 clock = 25ns)",78,-7,7);

  numberofCosmicsHist_ = new TH1F("numberofCosmicsPerEvent","Number of cosmics per event;Number of Cosmics",30,0,30);
  numberofCosmicsWTrackHist_ = new TH1F("numberofCosmicsWTrackPerEvent","Number of cosmics with track per event",30,0,30);
  numberofCosmicsTopBottomHist_ = new TH1F("numberofCosmicsTopBottomPerEvent","Number of top bottom cosmics per event;Number of Cosmics",30,0,30);
  numberofGoodEvtFreq_  = new TH1F("frequencyOfGoodEvents","Number of events with cosmic vs Event;Event Number;Number of Good Events/100 Events",2000,0,200000);

  numberofCrossedEcalIdsHist_ = new TH1F("numberofCrossedEcalCosmicsPerEvent","Number of crossed ECAL cosmics per event;Number of Crossed Cosmics",10,0,10);

  allOccupancyExclusiveECAL_            = new TH2F("OccupancyAllEvents_ExclusiveECAL","Occupancy all events Exclusive ECAL ;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseExclusiveECAL_      = new TH2F("OccupancyAllEventsCoarse_ExclusiveECAL","Occupancy all events Coarse Exclusive ECAL;i#phi;i#eta",360/5,ttPhiBins,35, ttEtaBins);
  allOccupancyECAL_            = new TH2F("OccupancyAllEvents_ECAL","Occupancy all events ECAL;i#phi;i#eta",360,1.,361.,172,-86,86);
  allOccupancyCoarseECAL_      = new TH2F("OccupancyAllEventsCoarse_ECAL","Occupancy all events Coarse ECAL;i#phi;i#eta",360/5,ttPhiBins,35, ttEtaBins);
  allFedsTimingHistECAL_       = new TH1F("timeForAllFeds_ECAL","timeForAllFeds ECAL;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingPhiEtaHistECAL_ = new TH3F("timePhiEtaAllFEDs_ECAL","ECAL (Phi,Eta,time) for all FEDs (SM,M binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",18,modPhiBins,9, modEtaBins,78,timingBins); 
  allFedsTimingTTHistECAL_     = new TH3F("timeTTAllFEDs_ECAL","ECAL (Phi,Eta,time) for all FEDs (SM,TT binning);i#phi;i#eta;Relative Time (1 clock = 25ns)",72,ttPhiBins,35,ttEtaBins,78,timingBins); 
  allFedsTimingLMHistECAL_     = new TH2F("timeLMAllFEDs_ECAL","ECAL (LM,time) for all FEDs (SM,LM binning);LM;Relative Time (1 clock = 25ns)",92, 1, 92,78,-7,7); 
  allFedsTimingHistDT_       = new TH1F("timeForAllFeds_DT","timeForAllFeds DT;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingHistRPC_       = new TH1F("timeForAllFeds_RPC","timeForAllFeds RPC;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingHistCSC_       = new TH1F("timeForAllFeds_CSC","timeForAllFeds CSC;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingHistHCAL_       = new TH1F("timeForAllFeds_HCAL","timeForAllFeds HCAL;Relative Time (1 clock = 25ns)",78,-7,7);
  allFedsTimingHistEcalMuon_   = new TH1F("timeForAllFeds_EcalMuon","timeForAllFeds Ecal+Muon;Relative Time (1 clock = 25ns)",78,-7,7);

  deltaRHist_ = new TH1F("deltaRHist","deltaR",500,-0.5,499.5); 
  deltaEtaHist_ = new TH1F("deltaIEtaHist","deltaIEta",170,-85.5,84.5);
  deltaPhiHist_ = new TH1F("deltaIPhiHist","deltaIPhi",720,-360.5,359.5);
  ratioAssocTracksHist_ = new TH1F("ratioAssocTracks","num assoc. tracks/tracks through Ecal",11,0,1.1);
  ratioAssocClustersHist_ = new TH1F("ratioAssocClusters","num assoc. clusters/total clusters",11,0,1.1);
  trackAssoc_muonsEcal_= new TH2F("trackAssoc_muonsEcal","Map of muon hits in Ecal", 360,1.,361.,172,-86,86);//360, 0 , 360, 170,-85 ,85);
  deltaEtaDeltaPhiHist_ = new TH2F("deltaEtaDeltaPhi","Delta ieta vs. delta iphi",170,-85.5,84.5,720,-360.5,359.5); 
  seedTrackEtaHist_ = new TH2F("seedTrackEta","track ieta vs. seed ieta",170,-85.5,84.5,170,-85.5,84.5); 
  seedTrackPhiHist_ = new TH2F("seedTrackPhi","track iphi vs. seed iphi",720,-360.5,359.5,720,-360.5,359.5); 

}

// ------------ method called once each job just after ending the event loop  ------------
  void 
EcalTimingAnalyzer::endJob()
{
  using namespace std;
  if (runInFileName_) {
    fileName_ += "-"+intToString(runNum_)+".graph.root";
  } else {
    fileName_ += ".root";
  }

  TFile root_file_(fileName_.c_str() , "RECREATE");

  allFedsHist_->Write();
  allFedsenergyHist_->Write();
  allFedsTimingHist_->Write();
  allFedsTimingVsAmpHist_->Write();
  allFedsFrequencyHist_->Write();
  allFedsTimingVsFreqHist_->Write();
  allFedsiEtaProfileHist_->Write();
  allFedsiPhiProfileHist_->Write();
  allOccupancy_->Write();
  TrueOccupancy_->Write();
  allOccupancyCoarse_->Write();
  TrueOccupancyCoarse_->Write();
  allFedsTimingPhiHist_->Write();
  allFedsTimingPhiEbpHist_->Write();
  allFedsTimingPhiEbmHist_->Write();
  allFedsTimingEbpHist_->Write();
  allFedsTimingEbmHist_->Write();
  allFedsTimingEbpTopHist_->Write();
  allFedsTimingEbmTopHist_->Write();
  allFedsTimingEbpBottomHist_->Write();
  allFedsTimingEbmBottomHist_->Write();
  allFedsTimingPhiEtaHist_->Write();
  allFedsTimingTTHist_->Write();
  allFedsTimingLMHist_->Write();

  allOccupancyExclusiveECAL_->Write();
  allOccupancyCoarseExclusiveECAL_->Write();
  allOccupancyECAL_->Write();
  allOccupancyCoarseECAL_->Write();
  allFedsTimingPhiEtaHistECAL_->Write();
  allFedsTimingHistECAL_->Write();
  allFedsTimingTTHistECAL_->Write();
  allFedsTimingLMHistECAL_->Write();

  allFedsTimingHistEcalMuon_->Write();

  triggerHist_->Write();
  triggerExclusiveHist_->Write();

  numberofCosmicsHist_->Write();
  numberofCosmicsWTrackHist_->Write();
  numberofCosmicsTopBottomHist_->Write();
  numberofGoodEvtFreq_->Write();
  numberofCrossedEcalIdsHist_->Write();

  runNumberHist_->SetBinContent(1,runNum_);
  runNumberHist_->Write();

  deltaRHist_->Write();
  deltaEtaHist_->Write();
  deltaPhiHist_->Write();
  ratioAssocClustersHist_->Write();
  ratioAssocTracksHist_->Write();
  deltaEtaDeltaPhiHist_->Write();
  seedTrackPhiHist_->Write();
  seedTrackEtaHist_->Write();
  trackAssoc_muonsEcal_->Write();

  root_file_.Close();

  LogWarning("EcalTimingAnalyzer") << "---> Number of cosmic events: " << cosmicCounter_ << " in " << naiveEvtNum_ << " events.";
  LogWarning("EcalTimingAnalyzer") << "---> Number of top+bottom cosmic events: " << cosmicCounterTopBottom_ << " in " << cosmicCounter_ << " cosmics in " << naiveEvtNum_ << " events.";

}


std::string EcalTimingAnalyzer::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

