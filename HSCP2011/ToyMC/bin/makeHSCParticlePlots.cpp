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

#endif

// ****** helper functions
std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}

//
std::string floatToString(float num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}
//
int findFineNoMBin(int nom)
{
  if(nom >= 30) return 29;

  return nom-1;
}

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
  int maxEvents_( inputHandler_.maxEvents() );


  // preselections
  const int minTrackNoH = 11;
  //const float minTrackValidFraction = 0.8;
  //const int minValidPixelHits = 2;
  //const int minIasNoM = 5;
  //const int minTofNdof = 8;
  //const int minTofNdofDt = 6;
  //const int minTofNdofCsc = 6;
  //const int minTrackQualityMask = 2;
  //const float maxTrackChi2OverNdf = 5.0;
  //const float minTrackPt = 35.0;
  //const float minIas = 0.0;
  //const float minIh = 3.0;
  //const float minTofInvBeta = 1.0;
  //const float maxTofIntBetaErr = 0.07;
  //const int minNumVertices = 1;
  //const float maxV3D = 2.0;
  //const float maxTrackEtIso = 50;
  //const float maxCalEOverPIso = 0.3;
  //const float maxTrackPtErr = 0.25;
  //const float maxTrackEta = 2.5;

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  //TFileDirectory dir = fs.mkdir("analyzeBasicPat");
  const int numNoMbins = 30;
  TH3F* trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[numNoMbins];
  TH2F* trackPMipSBNoMSliceHists[numNoMbins];
  TH2F* iasDistributionHighIhHighPNoMSliceHists[numNoMbins];
  // book hists
  for(int nom=1; nom < numNoMbins+1; ++nom)
  {
    // ias -- high P, high Ih region
    std::string iasName = "trackIasHighPHighIhNoMSlice";
    iasName+=intToString(nom);
    std::string iasTitle;
    if(nom==30) 
      iasTitle="Ias for P >= 50 GeV, Ih >= 3.5 MeV/cm, NoM>=";
    else
      iasTitle="Ias for P >= 50 GeV, Ih >= 3.5 MeV/cm, NoM=";
    iasTitle+=intToString(nom);
    iasTitle+=";;|#eta|";
    iasDistributionHighIhHighPNoMSliceHists[nom-1] = fs.make<TH2F>(iasName.c_str(),iasTitle.c_str(),400,0,1,24,0,2.4);
    // dedx -- low P SB
    std::string dedxName = "trackDeDxE1VsDeDxD3LowPSBNoMSlice";
    dedxName+=intToString(nom);
    std::string dedxTitle;
    if(nom==30) 
      dedxTitle="Tracker dE/dx Ih vs. Ias for P < 50 GeV, NoM>=";
    else
      dedxTitle="Tracker dE/dx Ih vs. Ias for P < 50 GeV, NoM=";
    dedxTitle+=intToString(nom);
    dedxTitle+=";Ias;Ih [MeV/cm];|#eta|";
    trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[nom-1] = fs.make<TH3F>(dedxName.c_str(),dedxTitle.c_str(),100,0,1,250,0,25,24,0,2.4);
    // p -- low Ih SB
    std::string pName = "trackPMipSBNoMSlice";
    pName+=intToString(nom);
    std::string pTitle;
    if(nom==30) 
      pTitle="Track P for Ih < 3.5 MeV/cm, NoM>=";
    else
      pTitle="Track P for Ih < 3.5 MeV/cm, NoM=";
    pTitle+=intToString(nom);
    pTitle+=";GeV;|#eta|";
    trackPMipSBNoMSliceHists[nom-1] = fs.make<TH2F>(pName.c_str(),pTitle.c_str(),4000,0,2000,24,0,2.4);
  }

  // loop the events
  int ievt=0;  
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

        // loop over HSCParticles in this event
        for(unsigned int c=0;c<hscpColl.size();c++)
        {
          susybsm::HSCParticle hscp  = hscpColl[c];
          reco::MuonRef  muon  = hscp.muonRef();
          reco::TrackRef track = hscp.trackRef();
          if(track.isNull())continue;

          const reco::DeDxData& dedxSObj  = dEdxSCollH->get(track.key());
          const reco::DeDxData& dedxMObj  = dEdxMCollH->get(track.key());

          float ih = dedxMObj.dEdx();
          float ias = dedxSObj.dEdx();
          float trackP = track->p();
          int ihNoM = dedxMObj.numberOfMeasurements();
          float trackEta = track->eta();
          //if(track->found() >= minTrackNoH)

          int myFineNoMbin = findFineNoMBin(ihNoM);
          if(myFineNoMbin<0)
            continue;

          if(trackP < 50) // p SB
            trackDeDxE1VsDeDxD3LowPSBNoMSliceHists[myFineNoMbin]->Fill(ias,ih,fabs(trackEta));
          if(ih < 3.5) // MIP peak
            trackPMipSBNoMSliceHists[myFineNoMbin]->Fill(trackP,fabs(trackEta));

          if(trackP >= 50)
          {
            if(ih >= 3.5)
              iasDistributionHighIhHighPNoMSliceHists[myFineNoMbin]->Fill(ias);
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

}

