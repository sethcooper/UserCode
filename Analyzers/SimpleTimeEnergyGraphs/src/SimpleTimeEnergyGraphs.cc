// -*- C++ -*-
//
// Package:    SimpleTimeEnergyGraphs
// Class:      SimpleTimeEnergyGraphs
// 
/**\class SimpleTimeEnergyGraphs SimpleTimeEnergyGraphs.cc Producers/SimpleTimeEnergyGraphs/src/SimpleTimeEnergyGraphs.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  
//         Created:  Tue Oct 13 14:56:06 CEST 2009
// $Id: SimpleTimeEnergyGraphs.cc,v 1.5 2010/03/18 10:48:04 scooper Exp $
//
//


// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronCoreFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronCore.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

//
// class decleration
//

class SimpleTimeEnergyGraphs : public edm::EDAnalyzer {
   public:
      explicit SimpleTimeEnergyGraphs(const edm::ParameterSet&);
      ~SimpleTimeEnergyGraphs();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      std::string intToString(int num);

      // ----------member data ---------------------------
      edm::InputTag EBRecHitCollection_;
      edm::InputTag EERecHitCollection_;
      edm::InputTag EBUncalibRecHitCollection_;
      edm::InputTag EEUncalibRecHitCollection_;
      double minAmpForTimingEB_;
      double minAmpForTimingEE_;
      std::string fileName_;

      TFile* file_;
      TH1F* timeOfHitsEBps_;
      TH1F* timeOfHitsEB_;
      TH1F* timeOfHitsEB5GeV_;
      TH1F* timeOfHitsEB10GeV_;
      TH1F* energyOfHitsEB_;
      TH1F* timeOfHitsEE_;
      TH1F* timeOfHitsEE5GeV_;
      TH1F* timeOfHitsEE10GeV_;
      TH1F* energyOfHitsEE_;

      TH1F* timeInEtaBinsEB_[6];
      TH1F* timeInEtaBinsEEP_[4];
      TH1F* timeInEtaBinsEEM_[4];

      TH2F* timeVsAmplitudeEB_;
      TH2F* timeVsAmplitudeEE_;
      TH2F* timeVsAeffEB_;
      TH2F* timeVsAeffEE_;
      
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
SimpleTimeEnergyGraphs::SimpleTimeEnergyGraphs(const edm::ParameterSet& iConfig) :
  EBRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EBRecHitCollection")),
  EERecHitCollection_ (iConfig.getParameter<edm::InputTag>("EERecHitCollection")),
  EBUncalibRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EBUncalibRecHitCollection")),
  EEUncalibRecHitCollection_ (iConfig.getParameter<edm::InputTag>("EEUncalibRecHitCollection")),
  minAmpForTimingEB_ (iConfig.getUntrackedParameter<double>("minAmpForTimingEB",5)),
  minAmpForTimingEE_ (iConfig.getUntrackedParameter<double>("minAmpForTimingEE",5)),
  fileName_ (iConfig.getUntrackedParameter<std::string>("rootFile","simpleTimeEnergyGraphs.root"))
{
   //now do what ever initialization is needed
   file_ = new TFile(fileName_.c_str(),"RECREATE");
   file_->cd();

   timeOfHitsEBps_ = new TH1F("timeOfHitsEBps", "Time of hits EB [ps]",2000,-100,100);
   timeOfHitsEB_ = new TH1F("timeOfHitsEB", "Time of hits EB [ns]",10000,-5,5);
   timeOfHitsEB5GeV_ = new TH1F("timeOfHitsEB5GeV", "Time of hits EB > 5 GeV [ns]",10000,-5,5);
   timeOfHitsEB10GeV_ = new TH1F("timeOfHitsEB10GeV", "Time of hits EB > 10 GeV [ns]",10000,-5,5);
   energyOfHitsEB_ = new TH1F("energyOfHitsEB", "Energy of hits EB [GeV]",2500,0,100);
   timeOfHitsEE_ = new TH1F("timeOfHitsEE", "Time of hits EE [ns]",10000,-5,5);
   timeOfHitsEE5GeV_ = new TH1F("timeOfHitsEE5GeV", "Time of hits EE > 5 GeV [ns]",10000,-5,5);
   timeOfHitsEE10GeV_ = new TH1F("timeOfHitsEE10GeV", "Time of hits EE > 10 GeV [ns]",10000,-5,5);
   energyOfHitsEE_ = new TH1F("energyOfHitsEE", "Energy of hits EE [GeV]",2500,0,100);

   timeVsAmplitudeEB_ = new TH2F("timeVsAmplitudeEB","Time vs. amplitude of hits EB;ADC;ns",1250,0,2500,1000,-25,25);
   timeVsAmplitudeEE_ = new TH2F("timeVsAmplitudeEE","Time vs. amplitude of hits EE;ADC;ns",1250,0,2500,1000,-25,25);
   
   timeVsAeffEB_ = new TH2F("timeVsAeffEB","Time vs. aeff/#sigma of hits EB;A/#sigma;ns",1250,0,2500,1000,-25,25);
   timeVsAeffEE_ = new TH2F("timeVsAeffEE","Time vs. aeff/#sigma of hits EE;A/#sigma;ns",1250,0,2500,1000,-25,25);

   for(int i=0; i<6; ++i)
   {
     std::string histTitle="timeOfHitsEBetaBin";
     histTitle+=intToString(i);
     timeInEtaBinsEB_[i] = new TH1F(histTitle.c_str(),histTitle.c_str(),1000,-5,5);
   }
   // EE
   for(int i=0; i<4; ++i)
   {
     std::string histTitle="timeOfHitsEEMetaBin";
     histTitle+=intToString(i);
     timeInEtaBinsEEM_[i] = new TH1F(histTitle.c_str(),histTitle.c_str(),1000,-5,5);
     std::string histTitle1="timeOfHitsEEPetaBin";
     histTitle1+=intToString(i);
     timeInEtaBinsEEP_[i] = new TH1F(histTitle1.c_str(),histTitle1.c_str(),1000,-5,5);
   }
}


SimpleTimeEnergyGraphs::~SimpleTimeEnergyGraphs()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
SimpleTimeEnergyGraphs::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   std::cout << "Processing run: " << iEvent.id().run() << " lumi: " << iEvent.luminosityBlock()
     << " event: " << iEvent.id().event() << std::endl;

  // RecHits
  Handle<EBRecHitCollection> ebRecHits;
  iEvent.getByLabel(EBRecHitCollection_,ebRecHits);
  if(!ebRecHits.isValid())
  {
    std::cout << "Cannot get ebRecHits from event!" << std::endl;
    return;
  }
  if(!ebRecHits->size() > 0)
    std::cout << "EBRecHits size is 0!" << std::endl;
  Handle<EERecHitCollection> eeRecHits;
  iEvent.getByLabel(EERecHitCollection_,eeRecHits);
  if(!eeRecHits.isValid())
  {
    std::cout << "Cannot get eeRecHits from event!" << std::endl;
    return;
  }
  if(!eeRecHits->size() > 0)
    std::cout << "EERecHits size is 0!" << std::endl;


  edm::Handle<reco::GsfElectronCollection> gsfElectrons;
  iEvent.getByLabel("gsfElectrons", gsfElectrons);
  if (!gsfElectrons.isValid()) {
    std::cerr << "Error! can't get the product ElectronCollection: " << std::endl;
  } else{
    // std::cerr << "BRAVO! GoT the product ElectronCollection. " << std::endl;
  }
  const reco::GsfElectronCollection* theElectrons =  gsfElectrons.product();


  //  loop on electrons in event and extract SC from it
  for(reco::GsfElectronCollection::const_iterator eleIt = theElectrons->begin();
      eleIt != theElectrons->end();
      eleIt++     )
  {
    //std::cout << "ele : " << (eleIt-theElectrons->begin()) << " pt: " << eleIt->pt()  << std::endl;
    if (eleIt->pt() < 10) continue;   // make this threshold configurable

    const reco::SuperCluster & sclus = *(eleIt->superCluster()) ;

    if(!eleIt->isEB()) {
      //    std::cout << "this ele is NOT in EB " << std::endl;
      continue;
    } else {
      //      std::cout << "this ele IS in EB " << std::endl;
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    // loop on barrel basic clusters which are inside SC from an electron
    for ( reco::CaloCluster_iterator  bClus =  sclus.clustersBegin()  ; 
        bClus != sclus.clustersEnd();
        ++bClus) // loop on barrel Bclusters
    {        
      // std::cout << "bc number: " << (bClus-sclus.clustersBegin()) << std::endl;

      double energy = (*bClus)->energy () ;
      double phi    = (*bClus)->phi () ;
      double eta    = (*bClus)->eta () ;
      double sinTheta         = fabs( sin( 2 *atan( exp(-1*(*bClus)->eta()) ) ) );
      double transverseEnergy = (*bClus)->energy () * sinTheta;
      double time = -1000.0 ; // gfdoc: work on this to provide a combination of crystals?
      double ampli = 0. ;
      double secondMin = 0. ;
      double secondTime = -1000. ;
      int numberOfXtalsInCluster=0 ;//counter for all xtals in cluster
      int numXtalsinCluster = 0 ;   //xtals in cluster above 3sigma noise  
      EBDetId maxDet ;
      EBDetId secDet ;

      std::vector<std::pair<DetId, float> > clusterDetIds = (*bClus)->hitsAndFractions() ; //get these from the cluster


      //////////////////////////////////////////////////////////////////////////////////////
      //loop on xtals in cluster
      for (std::vector<std::pair<DetId, float> >::const_iterator detitr = clusterDetIds.begin () ; 
          detitr != clusterDetIds.end (); 
          ++detitr)// loop on rechics of barrel basic clusters
      {
        //Here I use the "find" on a digi collection... I have been warned...   (GFdoc: ??)
        // GFdoc: check if DetId belongs to ECAL; if so, find it among those if this basic cluster
        if ( (detitr -> first).det () != DetId::Ecal) 
        { 
          std::cout << " det is " << (detitr -> first).det () << " (and not DetId::Ecal)" << std::endl ;
          continue ;
        }
        if ( (detitr -> first).subdetId () != EcalBarrel) 
        {
          std::cout << " subdet is " << (detitr -> first).subdetId () << " (and not EcalBarrel)" << std::endl ; 
          continue ; 
        }

        // GFdoc now find it!
        EcalRecHitCollection::const_iterator thishit = ebRecHits->find ( (detitr -> first) ) ;
        if (thishit == ebRecHits->end ()) 
        {
          continue ;
        }
        //The checking above should no longer be needed...... 
        //as only those in the cluster would already have rechits..

        // GFdoc this is one crystal in the basic cluster
        EcalRecHit myhit = (*thishit) ;

        // SIC Feb 14 2011 -- Add check on RecHit flags (takes care of spike cleaning in 42X)
        uint32_t rhFlag = myhit.recoFlag();
        if( !(rhFlag == EcalRecHit::kGood || rhFlag == EcalRecHit::kOutOfTime ||
              rhFlag == EcalRecHit::kPoorCalib) )
          continue;

        // thisamp is the EB amplitude of the current rechit
        double thisamp  = myhit.energy () ;
        double thistime = myhit.time ();
        double thisChi2 = myhit.chi2 ();
        double thisOutOfTimeChi2 = myhit.outOfTimeChi2 ();
        if (thisamp > 0.027) //cut on energy->number of crystals in cluster above 3sigma noise; gf: desirable?
        { 
          numXtalsinCluster++ ; 
        }
        if (thisamp > secondMin) 
        {
          secondMin = thisamp ; 
          secondTime = myhit.time () ; 
          secDet = (EBDetId) (detitr -> first) ;
        }
        if (secondMin > ampli) 
        {
          std::swap (ampli, secondMin) ; 
          std::swap (time, secondTime) ; 
          std::swap (maxDet, secDet) ;
        }


        if(EBDetId((detitr -> first)).iphi()==100 && EBDetId((detitr -> first)).ieta()==-78)
        {
          std::cout << "CRY FOUND IN BC OF SC OF ELECTRON! Time of EBRecHit: " << thishit->time() << " ns, "
            << " timeError: " << thishit->timeError() << " energy: "
            << thishit->energy() << " GeV" //, amplitude: " << uHitItr->amplitude()
            << std::endl;
        }




      } //end loop on rechits within barrel basic clusters
      //////////////////////////////////////////////////////


    } // loop on BC inside SC
  }// loop over electrons-SC


  // Loop over EBRecHits
  for(EBRecHitCollection::const_iterator recHitItr = ebRecHits->begin();
      recHitItr != ebRecHits->end(); ++recHitItr)
  {
    timeOfHitsEBps_->Fill(recHitItr->time()*1000);
    timeOfHitsEB_->Fill(recHitItr->time());
    energyOfHitsEB_->Fill(recHitItr->energy());
    if(recHitItr->energy() > 5)
      timeOfHitsEB5GeV_->Fill(recHitItr->time());
    if(recHitItr->energy() > 10)
      timeOfHitsEB10GeV_->Fill(recHitItr->time());
    EBDetId id = ((EBDetId)recHitItr->id());
    // Eta bins
    if(id.ieta() > 57 && id.ieta() < 86)
      timeInEtaBinsEB_[5]->Fill(recHitItr->time());
    if(id.ieta() > 29 && id.ieta() < 58)
      timeInEtaBinsEB_[4]->Fill(recHitItr->time());
    if(id.ieta() > 0 && id.ieta() < 30)
      timeInEtaBinsEB_[3]->Fill(recHitItr->time());
    if(id.ieta() > -30 && id.ieta() < 0)
      timeInEtaBinsEB_[2]->Fill(recHitItr->time());
    if(id.ieta() > -58 && id.ieta() < -29)
      timeInEtaBinsEB_[1]->Fill(recHitItr->time());
    if(id.ieta() > -86 && id.ieta() < -57)
      timeInEtaBinsEB_[0]->Fill(recHitItr->time());

    if(id.ieta() == -78 && id.iphi()==100)
    {
      std::cout << "Time of EBRecHit: " << recHitItr->time() << " ns, "
        << " timeError: " << recHitItr->timeError() << " energy: "
        << recHitItr->energy() << " GeV" //, amplitude: " << uHitItr->amplitude()
        << std::endl;
    }
  }
  // Loop over EERecHits
  for(EERecHitCollection::const_iterator recHitItr = eeRecHits->begin();
      recHitItr != eeRecHits->end(); ++recHitItr)
  {
    timeOfHitsEE_->Fill(recHitItr->time());
    energyOfHitsEE_->Fill(recHitItr->energy());
    if(recHitItr->energy() > 5)
      timeOfHitsEE5GeV_->Fill(recHitItr->time());
    if(recHitItr->energy() > 10)
      timeOfHitsEE10GeV_->Fill(recHitItr->time());

    //std::cout << "Time of EERecHit: " << recHitItr->time() << " ns, energy: "
    //  << recHitItr->energy() << " GeV, amplitude: " << uHitItr->amplitude()
    //  << std::endl;
  }

}


// ------------ method called once each job just before starting event loop  ------------
void 
SimpleTimeEnergyGraphs::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SimpleTimeEnergyGraphs::endJob() {
  file_->cd();

  timeOfHitsEBps_->Write();
  timeOfHitsEB_->Write();
  timeOfHitsEB5GeV_->Write();
  timeOfHitsEB10GeV_->Write();
  energyOfHitsEB_->Write();
  timeVsAmplitudeEB_->Write();
  timeVsAeffEB_->Write();
  timeOfHitsEE_->Write();
  timeOfHitsEE5GeV_->Write();
  timeOfHitsEE10GeV_->Write();
  energyOfHitsEE_->Write();
  timeVsAmplitudeEE_->Write();
  timeVsAeffEE_->Write();

  // EB
  for(int i=0; i<6; ++i)
    timeInEtaBinsEB_[i]->Write();
  // EE
  //for(int i=0; i<4; ++i)
  //{
  //  timeInEtaBinsEEM_[i]->Write();
  //  timeInEtaBinsEEP_[i]->Write();
  //}

  file_->Close();

}

std::string SimpleTimeEnergyGraphs::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

//define this as a plug-in
DEFINE_FWK_MODULE(SimpleTimeEnergyGraphs);
