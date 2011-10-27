#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TPaveText.h" 
#include "TRandom3.h"
#include "TProfile.h"
#include "TDirectory.h"

#include "TSystem.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <vector>

namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}

#if !defined(__CINT__) && !defined(__MAKECINT__)

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"

#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/PythonProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Math/interface/deltaR.h"

#endif

#include "commonFunctions.h"

// ****** main
int main(int argc, char ** argv)
{
  // load framework libraries
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  // parse arguments
  if(argc < 2)
  {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process);
  fwlite::OutputFiles outputHandler_(process);
  int maxEvents_(inputHandler_.maxEvents());
  // now get each parameterset
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("makeHSCParticlePlots");
  double scaleFactor_(ana.getParameter<double>("ScaleFactor"));
  // mass cut to use for the high-p high-Ih (search region) ias dist
  double massCutIasHighPHighIh_ (ana.getParameter<double>("MassCut"));
  // dE/dx calibration
  double dEdx_k (ana.getParameter<double>("dEdx_k"));
  double dEdx_c (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  bool matchToHSCP (ana.getParameter<bool>("MatchToHSCP"));

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  //TFileDirectory dir = fs.mkdir("analyzeBasicPat");
  const int numNoMbins = 30;
  TH3F* trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[numNoMbins];
  TH2F* trackPMipSBNoMSliceHists[numNoMbins];
  TH2F* iasDistributionHighIhHighPNoMSliceMassCutHists[numNoMbins];
  TH2F* iasDistributionHighIhHighPNoMSliceNoMassCutHists[numNoMbins];
  TH3F* iasDistributionHighIhNoMSliceNoMassCutWithPHists[numNoMbins];
  TH3F* ihWithPandEtaHists[numNoMbins];
  //TH2F* ihInPSidebandMassCutHists[numNoMbins];
  TH1F* pDistributionHist = fs.make<TH1F>("pDistribution","P;GeV",200,0,2000);
  pDistributionHist->Sumw2();
  TH2F* pVsIasDistributionHists[numNoMbins];
  TH2F* pVsIasHighIhDistributionHists[numNoMbins];
  TH2F* pVsIhDistributionHists[numNoMbins];
  TH2F* pVsIasHist = fs.make<TH2F>("pVsIas","P vs Ias;;GeV",20,0,1,40,0,1000);
  pVsIasHist->Sumw2();
  TH1F* pDistributionUnscaledHist = fs.make<TH1F>("pDistributionUnscaled","P (unscaled);GeV",200,0,2000);
  pDistributionUnscaledHist->Sumw2();
  TH2F* trackEtaVsPHist = fs.make<TH2F>("trackEtaVsP","Track #eta vs. p;GeV",4000,0,2000,24,0,2.4);
  trackEtaVsPHist->Sumw2();
  // book hists
  for(int nom=1; nom < numNoMbins+1; ++nom)
  {
    // ias -- high P, high Ih region with mass cut
    std::string iasName = "trackIasHighPHighIhNoMSlice";
    iasName+=intToString(nom);
    std::string iasTitle="Ias for P >= ";
    iasTitle+=doubleToString(pSidebandThreshold);
    iasTitle+=" GeV, Ih >= ";
    iasTitle+=doubleToString(ihSidebandThreshold);
    iasTitle+=" MeV/cm, mass >= ";
    iasTitle+=doubleToString(massCutIasHighPHighIh_);
    if(nom==30) 
      iasTitle+=", NoM>=";
    else
      iasTitle+=", NoM=";
    iasTitle+=intToString(nom);
    iasTitle+=";;|#eta|";
    iasDistributionHighIhHighPNoMSliceMassCutHists[nom-1] = fs.make<TH2F>(iasName.c_str(),iasTitle.c_str(),100,0,1,24,0,2.4);
    iasDistributionHighIhHighPNoMSliceMassCutHists[nom-1]->Sumw2();
    // ias with eta -- high P, high Ih region, no mass cut
    std::string iasNoMassCutName = "trackIasHighPHighIhNoMassCutNoMSlice";
    iasNoMassCutName+=intToString(nom);
    std::string iasNoMassCutTitle="Ias for P >= ";
    iasNoMassCutTitle+=doubleToString(pSidebandThreshold);
    iasNoMassCutTitle+=" GeV, Ih >= ";
    iasNoMassCutTitle+=doubleToString(ihSidebandThreshold);
    iasNoMassCutTitle+=" MeV/cm, no mass cut";
    if(nom==30) 
      iasNoMassCutTitle+=", NoM>=";
    else
      iasNoMassCutTitle+=", NoM=";
    iasNoMassCutTitle+=intToString(nom);
    iasNoMassCutTitle+=";;|#eta|";
    iasDistributionHighIhHighPNoMSliceNoMassCutHists[nom-1] = fs.make<TH2F>(iasNoMassCutName.c_str(),iasNoMassCutTitle.c_str(),100,0,1,24,0,2.4);
    iasDistributionHighIhHighPNoMSliceNoMassCutHists[nom-1]->Sumw2();
    // ias with eta, p high Ih region, no mass cut
    std::string iasNoMassCutWithPName = "trackIasHighIhNoMassCutWithPNoMSlice";
    iasNoMassCutWithPName+=intToString(nom);
    std::string iasNoMassCutWithPTitle="Ias for Ih >= ";
    iasNoMassCutWithPTitle+=doubleToString(ihSidebandThreshold);
    iasNoMassCutWithPTitle+=" MeV/cm, no mass cut";
    if(nom==30) 
      iasNoMassCutWithPTitle+=", NoM>=";
    else
      iasNoMassCutWithPTitle+=", NoM=";
    iasNoMassCutWithPTitle+=intToString(nom);
    iasNoMassCutWithPTitle+=";;|#eta|";
    iasDistributionHighIhNoMSliceNoMassCutWithPHists[nom-1] = fs.make<TH3F>(iasNoMassCutWithPName.c_str(),iasNoMassCutWithPTitle.c_str(),100,0,1,100,0,1000,24,0,2.4);
    iasDistributionHighIhNoMSliceNoMassCutWithPHists[nom-1]->Sumw2();
    // dedx -- low P SB
    std::string dedxName = "trackDeDxE1VsDeDxD3LowPSBNoMSlice";
    dedxName+=intToString(nom);
    std::string dedxTitle = "Tracker dE/dx Ih vs. Ias for P < ";
    dedxTitle+=doubleToString(pSidebandThreshold);
    if(nom==30) 
      dedxTitle+=" GeV, NoM>=";
    else
      dedxTitle+=" GeV, NoM=";
    dedxTitle+=intToString(nom);
    dedxTitle+=";Ias;Ih [MeV/cm];|#eta|";
    // book the hist
    trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[nom-1] = fs.make<TH3F>(dedxName.c_str(),dedxTitle.c_str(),400,0,1,400,0,10,24,0,2.4);
    //trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[nom-1] = fs.make<TH3F>(dedxName.c_str(),dedxTitle.c_str(),binVec[nom-1].size()-1,&(*binVec[nom-1].begin()),250,ihBinArray,24,etaBinArray);
    trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[nom-1]->Sumw2();
    // p -- low Ih SB
    std::string pName = "trackPMipSBNoMSlice";
    pName+=intToString(nom);
    std::string pTitle="Track P for Ih < ";
    pTitle+=doubleToString(ihSidebandThreshold);
    if(nom==30) 
      pTitle+=" MeV/cm, NoM>=";
    else
      pTitle+=" MeV/cm, NoM=";
    pTitle+=intToString(nom);
    pTitle+=";GeV;|#eta|";
    trackPMipSBNoMSliceHists[nom-1] = fs.make<TH2F>(pName.c_str(),pTitle.c_str(),4000,0,2000,24,0,2.4);
    trackPMipSBNoMSliceHists[nom-1]->Sumw2();
    // p vs Ias
    std::string pVsIasName = "trackPvsIasNoMSlice";
    pVsIasName+=intToString(nom);
    std::string pVsIasTitle="Track P vs ias";
    if(nom==30) 
      pVsIasTitle+=", NoM>=";
    else
      pVsIasTitle+=", NoM=";
    pVsIasTitle+=intToString(nom);
    pVsIasTitle+=";;GeV";
    pVsIasDistributionHists[nom-1] = fs.make<TH2F>(pVsIasName.c_str(),pVsIasTitle.c_str(),400,0,1,100,0,1000);
    pVsIasDistributionHists[nom-1]->Sumw2();
    // p vs Ias -- high Ih
    std::string pVsIasHighIhName = "trackPvsIasHighIhNoMSlice";
    pVsIasHighIhName+=intToString(nom);
    std::string pVsIasHighIhTitle="Track P vs ias for ih > ";
    pVsIasHighIhTitle+=doubleToString(ihSidebandThreshold);
    if(nom==30) 
      pVsIasHighIhTitle+="MeV/cm, NoM>=";
    else
      pVsIasHighIhTitle+="MeV/cm, NoM=";
    pVsIasHighIhTitle+=intToString(nom);
    pVsIasHighIhTitle+=";;GeV";
    pVsIasHighIhDistributionHists[nom-1] = fs.make<TH2F>(pVsIasHighIhName.c_str(),pVsIasHighIhTitle.c_str(),400,0,1,100,0,1000);
    pVsIasHighIhDistributionHists[nom-1]->Sumw2();
    // p vs Ih
    std::string pVsIhName = "trackPvsIhNoMSlice";
    pVsIhName+=intToString(nom);
    std::string pVsIhTitle="Track P vs ih";
    if(nom==30) 
      pVsIhTitle+=", NoM>=";
    else
      pVsIhTitle+=", NoM=";
    pVsIhTitle+=intToString(nom);
    pVsIhTitle+=";;GeV";
    pVsIhDistributionHists[nom-1] = fs.make<TH2F>(pVsIhName.c_str(),pVsIhTitle.c_str(),400,0,10,100,0,1000);
    pVsIhDistributionHists[nom-1]->Sumw2();
    // Ih vs p vs eta
    std::string ihWithPandEtaName = "IhWithPandEtaNoMSlice";
    ihWithPandEtaName+=intToString(nom);
    std::string ihWithPandEtaTitle="Track #eta vs p vs ih";
    if(nom==30) 
      ihWithPandEtaTitle+=", NoM>=";
    else
      ihWithPandEtaTitle+=", NoM=";
    ihWithPandEtaTitle+=intToString(nom);
    ihWithPandEtaTitle+=";MeV/cm;GeV;";
    ihWithPandEtaHists[nom-1] = fs.make<TH3F>(ihWithPandEtaName.c_str(),ihWithPandEtaTitle.c_str(),400,0,10,100,0,1000,24,0,2.4);
    ihWithPandEtaHists[nom-1]->Sumw2();
  }

  // loop the events
  int ievt = 0;
  int eventsInHighIhHighPRegion = 0;
  int eventsInHighIhHighPRegionWithMassCut = 0;
  for(unsigned int iFile=0; iFile<inputHandler_.files().size(); ++iFile)
  {
    // open input file (can be located on castor)
    TFile* inFile = TFile::Open(inputHandler_.files()[iFile].c_str());
    if( inFile )
    {
      fwlite::Event ev(inFile);
      for(ev.toBegin(); !ev.atEnd(); ++ev, ++ievt)
      {
        // break loop if maximal number of events is reached 
        if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
        // simple event counter
        if(inputHandler_.reportAfter()!=0 ? (ievt>0 && ievt%inputHandler_.reportAfter()==0) : false) 
          std::cout << "  processing event: " << ievt << std::endl;

        fwlite::Handle<susybsm::HSCParticleCollection> hscpCollHandle;
        hscpCollHandle.getByLabel(ev,"HSCParticleProducer");
        if(!hscpCollHandle.isValid()){printf("HSCP Collection NotFound\n");continue;}
        const susybsm::HSCParticleCollection& hscpColl = *hscpCollHandle;

        fwlite::Handle<reco::DeDxDataValueMap> dEdxSCollH;
        dEdxSCollH.getByLabel(ev, "dedxASmi");
        if(!dEdxSCollH.isValid()){printf("Invalid dEdx Selection collection\n");continue;}

        fwlite::Handle<reco::DeDxDataValueMap> dEdxMCollH;
        dEdxMCollH.getByLabel(ev, "dedxHarm2");
        if(!dEdxMCollH.isValid()){printf("Invalid dEdx Mass collection\n");continue;}

        fwlite::Handle< std::vector<reco::GenParticle> > genCollHandle;
        std::vector<reco::GenParticle> genColl;
        if(matchToHSCP)
        {
          genCollHandle.getByLabel(ev, "genParticles");
          if(!genCollHandle.isValid()){printf("GenParticle Collection NotFound\n");continue;}
          genColl = *genCollHandle;
        }


        // loop over HSCParticles in this event
        for(unsigned int c=0;c<hscpColl.size();c++)
        {
          susybsm::HSCParticle hscp  = hscpColl[c];
          reco::MuonRef  muon  = hscp.muonRef();
          reco::TrackRef track = hscp.trackRef();
          if(track.isNull())
            continue;

          const reco::DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
          const reco::DeDxData& dedxMObj  = dEdxMCollH->get(track.key());

          // for signal MC
          int NChargedHSCP = 0;
          if(matchToHSCP)
          {
            int ClosestGen;
            if(DistToHSCP(hscp, genColl, ClosestGen)>0.03)
              continue;
            NChargedHSCP = HowManyChargedHSCP(genColl);
          }

          float ih = dedxMObj.dEdx();
          float ias = dedxSObj.dEdx();
          float trackP = track->p();
          int ihNoM = dedxMObj.numberOfMeasurements();
          float trackEta = track->eta();

          if(!passesPreselection(hscp,dedxSObj,dedxMObj,ev))
            continue;

          int myFineNoMbin = findFineNoMBin(ihNoM);
          if(myFineNoMbin<0)
            continue;

          pDistributionHist->Fill(trackP);
          pDistributionUnscaledHist->Fill(trackP);
          trackEtaVsPHist->Fill(trackP,fabs(trackEta));
          pVsIasHist->Fill(ias,trackP);
          //XXX testing
          //if(fabs(trackEta) < 0.4)
          //{
          //double massSqr = (ih-dEdx_c)*pow(trackP,2)/dEdx_k;
          //if(sqrt(massSqr) >= massCutIasHighPHighIh_)
          //{
            pVsIasDistributionHists[myFineNoMbin]->Fill(ias,trackP);
            pVsIhDistributionHists[myFineNoMbin]->Fill(ih,trackP);
            if(ih >= ihSidebandThreshold)
              pVsIasHighIhDistributionHists[myFineNoMbin]->Fill(ias,trackP);
          //}
          //}

          ihWithPandEtaHists[myFineNoMbin]->Fill(ih,trackP,fabs(trackEta));

          //XXX testing
          //if(trackP < pSidebandThreshold && ih > ihSidebandThreshold)
          if(trackP < pSidebandThreshold) // p SB
            trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[myFineNoMbin]->Fill(ias,ih,fabs(trackEta));
          if(ih < ihSidebandThreshold) // MIP peak
            trackPMipSBNoMSliceHists[myFineNoMbin]->Fill(trackP,fabs(trackEta));

          if(ih >= ihSidebandThreshold)
          {
            iasDistributionHighIhNoMSliceNoMassCutWithPHists[myFineNoMbin]->Fill(ias,trackP,fabs(trackEta));
            if(trackP >= pSidebandThreshold)
            {
              iasDistributionHighIhHighPNoMSliceNoMassCutHists[myFineNoMbin]->Fill(ias,fabs(trackEta));

              eventsInHighIhHighPRegion++;
              double massSqr = (ih-dEdx_c)*pow(trackP,2)/dEdx_k;
              if(sqrt(massSqr) >= massCutIasHighPHighIh_)
              {
                iasDistributionHighIhHighPNoMSliceMassCutHists[myFineNoMbin]->Fill(ias,fabs(trackEta));
                eventsInHighIhHighPRegionWithMassCut++;
              }
            }
          }
        }

      }  
      // close input file
      inFile->Close();
    }
    // break loop if maximal number of events is reached:
    // this has to be done twice to stop the file loop as well
    if(maxEvents_>0 ? ievt+1>maxEvents_ : false) break;
  }
  
  // optional: scale hists
  for(int nom=1; nom < numNoMbins+1; ++nom)
  {
    iasDistributionHighIhHighPNoMSliceMassCutHists[nom-1]->Scale(scaleFactor_);
    trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[nom-1]->Scale(scaleFactor_);
    trackPMipSBNoMSliceHists[nom-1]->Scale(scaleFactor_);
  }
  pDistributionHist->Scale(scaleFactor_);

  //TCanvas t;
  //t.cd();
  //pDistributionUnscaledHist->Draw();
  //t.Print("pDistUnscaled.C");
  //pDistributionHist->Draw();
  //t.Print("pDistScaled.C");

  //std::cout << "Events with Ih >= 3.5 MeV/cm and P >= 50 GeV: " << eventsInHighIhHighPRegion
  //  << "; also passing mass >= " << massCutIasHighPHighIh_ << " : " << eventsInHighIhHighPRegionWithMassCut
  //  << std::endl;
}

