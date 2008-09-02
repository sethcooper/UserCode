// -*- C++ -*-
//
// Package:    EcalCosmicTrackTimingProducer
// Class:      EcalCosmicTrackTimingProducer
// 
/**\class EcalCosmicTrackTimingProducer EcalCosmicTrackTimingProducer.cc Producers/EcalCosmicTrackTimingProducer/src/EcalCosmicTrackTimingProducer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Fri Aug 29 09:49:44 CDT 2008
// $Id: EcalCosmicTrackTimingProducer.cc,v 1.1 2008/08/29 20:51:13 scooper Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"


// *** for TrackAssociation
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Common/interface/Handle.h"
#include "TrackingTools/TrackAssociator/interface/TrackDetectorAssociator.h"
#include "TrackingTools/TrackAssociator/interface/TrackAssociatorParameters.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

//
// class decleration
//

class EcalCosmicTrackTimingProducer : public edm::EDProducer {
  public:
    explicit EcalCosmicTrackTimingProducer(const edm::ParameterSet&);
    ~EcalCosmicTrackTimingProducer();

  private:
    virtual void beginJob(const edm::EventSetup&) ;
    virtual void produce(edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;

    // ----------member data ---------------------------
    edm::InputTag ecalRecHitCollectionEB_;
    edm::InputTag ecalRecHitCollectionEE_;
    edm::InputTag barrelClusterCollection_;
    edm::InputTag endcapClusterCollection_;
  
    double minTimingAmpEB_;
    double minTimingAmpEE_;
    double minRecHitAmpEB_;
    double minRecHitAmpEE_;

    TrackDetectorAssociator trackAssociator_;
    TrackAssociatorParameters trackParameters_;

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
EcalCosmicTrackTimingProducer::EcalCosmicTrackTimingProducer(const edm::ParameterSet& iConfig) :
  ecalRecHitCollectionEB_ (iConfig.getParameter<edm::InputTag>("ecalRecHitCollectionEB")),
  ecalRecHitCollectionEE_ (iConfig.getParameter<edm::InputTag>("ecalRecHitCollectionEE")),
  barrelClusterCollection_ (iConfig.getParameter<edm::InputTag>("barrelClusterCollection")),
  endcapClusterCollection_ (iConfig.getParameter<edm::InputTag>("endcapClusterCollection"))
{
  //register your products
  /* Examples
     produces<ExampleData2>();

  //if do put with a label
  produces<ExampleData2>("label");
   */
  produces<std::vector<float> >("EcalCosmicTrackTiming");
  //now do what ever other initialization is needed

}


EcalCosmicTrackTimingProducer::~EcalCosmicTrackTimingProducer()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
  void
EcalCosmicTrackTimingProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  /* This is an event example
  //Read 'ExampleData' from the Event
  Handle<ExampleData> pIn;
  iEvent.getByLabel("example",pIn);

  //Use the ExampleData to create an ExampleData2 which 
  // is put into the Event
  std::auto_ptr<ExampleData2> pOut(new ExampleData2(*pIn));
  iEvent.put(pOut);
   */

  /* this is an EventSetup example
  //Read SetupData from the SetupRecord in the EventSetup
  ESHandle<SetupData> pSetup;
  iSetup.get<SetupRecord>().get(pSetup);
   */
  
  bool hasEndcapClusters = true;
  auto_ptr<vector<float> > trackTimes(new vector<float>);
  Handle<EcalRecHitCollection> hits;
  iEvent.getByLabel(ecalRecHitCollectionEB_, hits);
  if (!(hits.isValid())) 
  {
    LogWarning("EcalCosmicTrackTimingProducer") << ecalRecHitCollectionEB_ << " not available";
    return;
  } 
  Handle<EcalRecHitCollection> hitsEE;
  iEvent.getByLabel(ecalRecHitCollectionEE_, hitsEE);
  if (!(hitsEE.isValid())) 
  {
    LogWarning("EcalCosmicTrackTimingProducer") << ecalRecHitCollectionEE_ << " not available";
    return;
  }
  edm::Handle<reco::SuperClusterCollection> escHandle;
  iEvent.getByLabel(endcapClusterCollection_, escHandle);
  if (!(escHandle.isValid())) 
  {
    LogWarning("EcalCosmicTrackTimingProducer") << endcapClusterCollection_ << " not available";
    hasEndcapClusters = false;
    //return;
  }
  edm::Handle<reco::SuperClusterCollection> bscHandle;
  iEvent.getByLabel(barrelClusterCollection_, bscHandle);
  if (!(bscHandle.isValid()))
  {
    LogWarning("EcalCosmicTrackTimingProducer") << barrelClusterCollection_ << " not available";
    return;
  }

  std::vector<EBDetId> seeds;
  const reco::SuperClusterCollection *clusterCollection_p = bscHandle.product();
  for (reco::SuperClusterCollection::const_iterator clus = clusterCollection_p->begin(); clus != clusterCollection_p->end(); ++clus)
  {
    //double energy = clus->energy();
    //double phi    = clus->phi();
    //double eta    = clus->eta();
    double time = -1000.0;
    double ampli = 0.;
    double secondMin = 0.;
    double secondTime = -1000.;
    int numXtalsinCluster = 0;

    EBDetId maxDet;
    EBDetId secDet;

    std::vector<DetId> clusterDetIds = clus->getHitsByDetId();//get these from the cluster
    for(std::vector<DetId>::const_iterator detitr = clusterDetIds.begin(); detitr != clusterDetIds.end(); ++detitr)
    { 
      //Here I use the "find" on a digi collection... I have been warned...
      if ((*detitr).det() != DetId::Ecal) { std::cout << " det is " <<(*detitr).det() << std::endl;continue;}
      if ((*detitr).subdetId() != EcalBarrel) {std::cout << " subdet is " <<(*detitr).subdetId() << std::endl; continue; }
      EcalRecHitCollection::const_iterator thishit = hits->find((*detitr));
      if (thishit == hits->end()) continue;
      //The checking above should no longer be needed...... as only those in the cluster would already have rechits..

      EcalRecHit myhit = (*thishit);

      double thisamp = myhit.energy();
      if (thisamp > minRecHitAmpEB_) {numXtalsinCluster++; }
      if (thisamp > secondMin) {secondMin = thisamp; secondTime = myhit.time(); secDet = (EBDetId)(*detitr);}
      if (secondMin > ampli) {std::swap(ampli,secondMin); std::swap(time,secondTime); std::swap(maxDet,secDet);}
    }
  seeds.push_back(maxDet);
  }
  
     
  // *** TrackAssociator *** //
  
  edm::Handle<reco::TrackCollection> recoTracks;
  iEvent.getByLabel("cosmicMuons", recoTracks);  
  
  if ( recoTracks.isValid() ) {
    std::map<int,std::vector<DetId> > trackDetIdMap;
    int tracks = 0;
    for(reco::TrackCollection::const_iterator recoTrack = recoTracks->begin(); recoTrack != recoTracks->end(); ++recoTrack){
      
      //TODO: for now, make no geometric/quality requirements
      //if(fabs(recoTrack->d0())>70 || fabs(recoTrack->dz())>70)
      //  continue;
      //if(recoTrack->numberOfValidHits()<20)
      //  continue;

      TrackDetMatchInfo info = trackAssociator_.associate(iEvent, iSetup, *recoTrack, trackParameters_);      
      
      for (unsigned int i=0; i<info.crossedEcalIds.size(); i++) {	 
	// only checks for barrel
	if (info.crossedEcalIds[i].det() == DetId::Ecal && info.crossedEcalIds[i].subdetId() == 1) {	     
	  EBDetId ebDetId (info.crossedEcalIds[i]);	   
	  //trackAssoc_muonsEcal_->Fill(ebDetId.iphi(), ebDetId.ieta());

	  EcalRecHitCollection::const_iterator thishit = hits->find(ebDetId);
	  if (thishit == hits->end()) continue;
	  
	  EcalRecHit myhit = (*thishit);	 
	}
      }
      
      //numberofCrossedEcalIdsHist_->Fill(info.crossedEcalIds.size());
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

      for(std::vector<EBDetId>::const_iterator seedItr = seeds.begin(); seedItr != seeds.end(); ++seedItr) {
        for(std::map<int,std::vector<DetId> >::const_iterator mapItr = trackDetIdMap.begin();
            mapItr != trackDetIdMap.end(); ++mapItr) {
          for(unsigned int i=0; i<mapItr->second.size(); i++) {
            // only checks for barrel
            if(mapItr->second[i].det() == DetId::Ecal && mapItr->second[i].subdetId() == 1) {
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
              //debug
              LogWarning("EcalCosmicTrackTimingProducer") << "in loop for matching; bestDR : " << bestDr;
            }
          }
        }
      }
      if(bestDr < 1000) {
        //deltaRHist_->Fill(bestDr);
        //deltaPhiHist_->Fill(bestDPhi);
        //deltaEtaHist_->Fill(bestDEta);
        //deltaEtaDeltaPhiHist_->Fill(bestDEta,bestDPhi);
        //seedTrackEtaHist_->Fill(bestEtaSeed,bestEtaTrack);
        //seedTrackPhiHist_->Fill(bestPhiSeed,bestPhiTrack);
        
        // Find the RecHit corresponding to this DetId
        EcalRecHitCollection::const_iterator thishit = hits->find(bestSeed);
        if (thishit != hits->end())
          trackTimes->push_back(thishit->time());
        else
          trackTimes->push_back(-999);
        seeds.erase(find(seeds.begin(),seeds.end(), bestSeed));
        trackDetIdMap.erase(trackDetIdMap.find(bestTrack));
        trackDetIdToSeedMap[bestTrackDet] = bestSeed;
        numAssocTracks++;
        numAssocClusters++;
      }
      else {
	edm::LogVerbatim("TrackAssociator") << "could not match cluster seed to track; bestDR : " << bestDr;
        LogWarning("EcalCosmicTrackTimingProducer") << "could not match cluster seed to track; bestDR : " << bestDr;
        trackTimes->push_back(-999);
	break; // no match found
      }
    }
    if(numSeeds>0 && numTracks>0) {
      //ratioAssocClustersHist_->AddBinContent(1,numAssocClusters);
      //ratioAssocClustersHist_->AddBinContent(2,numSeeds);
    }
    if(numTracks>0) {
      //ratioAssocTracksHist_->AddBinContent(1,numAssocTracks);
      //ratioAssocTracksHist_->AddBinContent(2,numTracks);
      //numberofCosmicsWTrackHist_->Fill(numSeeds);
    }
    LogWarning("EcalCosmicTrackTimingProducer") << "!!!! Size of recoTracks trackCollection: " << recoTracks->size()
      << ";  number of associated tracks: " << numAssocTracks << "; size of trackTimes vector: " << trackTimes->size();
  } else {
    LogWarning("EcalCosmicTrackTimingProducer") << "!!! No TrackAssociator recoTracks";    
  }
  
  // *** end of TrackAssociator code *** //

  iEvent.put(trackTimes, "EcalCosmicTrackTiming");
}


// ------------ method called once each job just before starting event loop  ------------
void 
EcalCosmicTrackTimingProducer::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalCosmicTrackTimingProducer::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalCosmicTrackTimingProducer);
