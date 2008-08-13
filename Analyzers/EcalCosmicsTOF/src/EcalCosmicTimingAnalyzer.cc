// -*- C++ -*-
//
// Package:   EcalCosmicTimingAnalyzer 
// Class:     EcalCosmicTimingAnalyzer  
// 
/**\class EcalCosmicTimingAnalyzer EcalCosmicTimingAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/

 
#include "Analyzers/EcalCosmicsTOF/interface/EcalCosmicTimingAnalyzer.h"

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
EcalCosmicTimingAnalyzer::EcalCosmicTimingAnalyzer(const edm::ParameterSet& iConfig) :
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


EcalCosmicTimingAnalyzer::~EcalCosmicTimingAnalyzer()
{
}


//
// member functions
//

// ------------ method called to for each event  ------------
  void
EcalCosmicTimingAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  int ievt = iEvent.id().event();
  auto_ptr<EcalElectronicsMapping> ecalElectronicsMap(new EcalElectronicsMapping);

  edm::Handle<reco::SuperClusterCollection> bscHandle;  
  edm::Handle<reco::SuperClusterCollection> escHandle;  

  naiveEvtNum_++;

  iEvent.getByLabel(barrelClusterCollection_, bscHandle);
  if (!(bscHandle.isValid())) 
  {
    LogWarning("EcalCosmicTimingAnalyzer") << barrelClusterCollection_ << " not available";
    return;
  }
  LogDebug("EcalCosmicTimingAnalyzer") << "event " << ievt;
  iEvent.getByLabel(endcapClusterCollection_, escHandle);
  if (!(escHandle.isValid())) 
  {
    LogWarning("EcalCosmicTimingAnalyzer") << endcapClusterCollection_ << " not available";
    //return;
  }
  Handle<EcalRecHitCollection> hits;
  iEvent.getByLabel(ecalRecHitCollection_, hits);
  if (!(escHandle.isValid())) 
  {
    LogWarning("EcalCosmicTimingAnalyzer") << ecalRecHitCollection_ << " not available";
    return;
  }

  std::vector<EBDetId> seeds;

  // now loop over super clusters!  
  const reco::SuperClusterCollection *clusterCollection_p = bscHandle.product();
  for (reco::SuperClusterCollection::const_iterator clus = clusterCollection_p->begin(); clus != clusterCollection_p->end(); ++clus)
  {
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
    seeds.push_back(maxDet);

  }//++++++++++++++++++END LOOP OVER SUPERCLUSTERS+++++++++++++++++++++++++++++++++++

  // *** TrackAssociator *** //

  // get reco tracks 
  edm::Handle<reco::TrackCollection> recoTracks;
  iEvent.getByLabel("cosmicMuons", recoTracks);  

  if ( recoTracks.isValid() ) {
    std::map<int,std::vector<DetId> > trackDetIdMap;
    int tracks = 0;
    for(reco::TrackCollection::const_iterator recoTrack = recoTracks->begin(); recoTrack != recoTracks->end(); ++recoTrack){

      if(fabs(recoTrack->d0())>70 || fabs(recoTrack->dz())>70)
        continue;
      if(recoTrack->numberOfValidHits()<20)
        continue;

      TrackDetMatchInfo info = trackAssociator_.associate(iEvent, iSetup, *recoTrack, trackParameters_);      
      for (unsigned int i=0; i<info.crossedEcalIds.size(); i++) {	 
        // only checks for barrel
        if (info.crossedEcalIds[i].det() == DetId::Ecal && info.crossedEcalIds[i].subdetId() == 1) {	     
          EBDetId ebDetId (info.crossedEcalIds[i]);	   
          trackAssoc_muonsEcal_->Fill(ebDetId.iphi(), ebDetId.ieta());

          EcalRecHitCollection::const_iterator thishit = hits->find(ebDetId);
          if (thishit == hits->end()) continue;
          EcalRecHit myhit = (*thishit);	 
        }
      }
      tracks++;
      if(info.crossedEcalIds.size()>0)
        trackDetIdMap.insert(std::pair<int,std::vector<DetId> > (tracks,info.crossedEcalIds));
    }      

    // Now to match recoTracks with cosmic clusters
    int numAssocTracks = 0;
    int numAssocClusters = 0;
    edm::LogVerbatim("TrackAssociator") << "Matching cosmic clusters to tracks...";
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
        deltaRHist_->Fill(bestDr);
        deltaEtaDeltaPhiHist_->Fill(bestDEta,bestDPhi);
        seeds.erase(find(seeds.begin(),seeds.end(), bestSeed));
        trackDetIdMap.erase(trackDetIdMap.find(bestTrack));
        trackDetIdToSeedMap[bestTrackDet] = bestSeed;
        numAssocTracks++;
        numAssocClusters++;

        EcalRecHitCollection::const_iterator thishit = hits->find(bestSeed);
        if (thishit == hits->end()) continue;
        EcalRecHit myHit = (*thishit);
        matchedTimeHist_->Fill(myHit.time());
        matchedEnergyHist_->Fill(myHit.energy());
      }
      else {
        edm::LogVerbatim("TrackAssociator") << "could not match cluster seed to track." << bestDr;
        break; // no match found
      }
    }
  }

  // *** end of TrackAssociator code *** //

}


// ------------ method called once each job just before starting event loop  ------------
  void 
EcalCosmicTimingAnalyzer::beginJob(const edm::EventSetup&)
{
  int numBins = 200;//(int)round(histRangeMax_-histRangeMin_)+1;

  matchedTimeHist_   = new TH1F("trackMatchedRecoTime","Reco time for track-matched seeds (1 clock = 25 ns)",78,-7,7);
  matchedEnergyHist_ = new TH1F("trackMatchedRecoEnergy","Recoe energy for track-matched seeds (GeV)",numBins,histRangeMin_,histRangeMax_);

  deltaRHist_ = new TH1F("deltaRHist","deltaR",500,-0.5,499.5); 
  //deltaEtaHist_ = new TH1F("deltaIEtaHist","deltaIEta",170,-85.5,84.5);
  //deltaPhiHist_ = new TH1F("deltaIPhiHist","deltaIPhi",720,-360.5,359.5);
  //ratioAssocTracksHist_ = new TH1F("ratioAssocTracks","num assoc. tracks/tracks through Ecal",11,0,1.1);
  //ratioAssocClustersHist_ = new TH1F("ratioAssocClusters","num assoc. clusters/total clusters",11,0,1.1);
  trackAssoc_muonsEcal_= new TH2F("trackAssoc_muonsEcal","Map of muon hits in Ecal", 360,1.,361.,172,-86,86);//360, 0 , 360, 170,-85 ,85);
  deltaEtaDeltaPhiHist_ = new TH2F("deltaEtaDeltaPhi","Delta ieta vs. delta iphi",170,-85.5,84.5,720,-360.5,359.5); 
  //seedTrackEtaHist_ = new TH2F("seedTrackEta","track ieta vs. seed ieta",170,-85.5,84.5,170,-85.5,84.5); 
  //seedTrackPhiHist_ = new TH2F("seedTrackPhi","track iphi vs. seed iphi",720,-360.5,359.5,720,-360.5,359.5); 

}

// ------------ method called once each job just after ending the event loop  ------------
  void 
EcalCosmicTimingAnalyzer::endJob()
{
  LogWarning("EcalCosmicTimingAnalyzer") << "---> endJob()";

  using namespace std;
  if (runInFileName_) {
    fileName_ += "-"+intToString(runNum_)+".graph.root";
  } else {
    fileName_ += ".root";
  }

  TFile root_file_(fileName_.c_str() , "RECREATE");

  deltaRHist_->Write();
  //deltaEtaHist_->Write();
  //deltaPhiHist_->Write();
  //ratioAssocClustersHist_->Write();
  //ratioAssocTracksHist_->Write();
  deltaEtaDeltaPhiHist_->Write();
  //seedTrackPhiHist_->Write();
  //seedTrackEtaHist_->Write();
  trackAssoc_muonsEcal_->Write();

  matchedTimeHist_->Write();
  matchedEnergyHist_->Write();
  
  root_file_.Close();

  LogWarning("EcalCosmicTimingAnalyzer") << "---> Number of cosmic events: " << cosmicCounter_ << " in " << naiveEvtNum_ << " events.";
  LogWarning("EcalCosmicTimingAnalyzer") << "---> Number of top+bottom cosmic events: " << cosmicCounterTopBottom_ << " in " << cosmicCounter_ << " cosmics in " << naiveEvtNum_ << " events.";

}


std::string EcalCosmicTimingAnalyzer::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

DEFINE_FWK_MODULE(EcalCosmicTimingAnalyzer);
