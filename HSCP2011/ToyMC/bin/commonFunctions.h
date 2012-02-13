#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"

#include <iostream>

// preselection -- adapted from Analysis_Step234.C
bool passesPreselection(const susybsm::HSCParticle& hscp,  const reco::DeDxData& dedxSObj,
    const reco::DeDxData& dedxMObj, const reco::MuonTimeExtra* tof,
    const reco::MuonTimeExtra* dttof, const reco::MuonTimeExtra* csctof,
    const fwlite::Event& ev, bool considerToF)
{
  //TODO: use common py for these
  // preselections
  const int minTrackNoH = 11;
  const float minTrackValidFraction = 0.8;
  const int minValidPixelHits = 2;
  const unsigned int minIasNoM = 5;
  const unsigned int minIhNoM = 5;
  const int minTofNdof = 8;
  const int minTofNdofDt = 6;
  const int minTofNdofCsc = 6;
  const int minTrackQualityMask = 2;
  const float maxTrackChi2OverNdf = 5.0;
  //const float minTrackPt = 35.0;
  //const float minIas = 0.0;
  //const float minIh = 3.0;
  const float minTofInvBeta = 1.0;
  const float maxTofInvBetaErr = 0.07;
  const unsigned int minNumVertices = 1;
  const float maxV3D = 0.5;
  const float maxTrackEtIso = 50;
  const float maxCalEOverPIso = 0.3;
  const float maxTrackPtErr = 0.25;
  const float maxTrackEta = 2.5;

  reco::TrackRef track = hscp.trackRef();
  if(track.isNull())
    return false;

  // track
  if(dedxMObj.numberOfMeasurements() < minIhNoM)
    return false;
  if(track->found() < minTrackNoH)
    return false;
  if(track->validFraction() < minTrackValidFraction)
    return false;
  if(track->hitPattern().numberOfValidPixelHits() < minValidPixelHits)
    return false;
  if(dedxSObj.numberOfMeasurements() < minIasNoM)
    return false;
  if(track->qualityMask() < minTrackQualityMask)
    return false;
  if(track->chi2()/track->ndof() > maxTrackChi2OverNdf)
    return false;
  if(fabs(track->eta()) > maxTrackEta)
    return false;
  // ptError
  if(track->ptError()/track->pt() > maxTrackPtErr)
    return false;

  // vertex
  fwlite::Handle< std::vector<reco::Vertex> > vertexCollHandle;
  vertexCollHandle.getByLabel(ev,"offlinePrimaryVertices");
  if(!vertexCollHandle.isValid()){printf("Vertex Collection NotFound\n");return false;}
  const std::vector<reco::Vertex>& vertexColl = *vertexCollHandle;
  if(vertexColl.size()<minNumVertices){printf("NO VERTEX\n"); return false;}
  double dz  = track->dz (vertexColl[0].position());
  double dxy = track->dxy(vertexColl[0].position());
  for(unsigned int i=1;i<vertexColl.size();i++){
    if(fabs(track->dz (vertexColl[i].position())) < fabs(dz) ){
      dz  = track->dz (vertexColl[i].position());
      dxy = track->dxy(vertexColl[i].position());
    }
  }
  double v3d = sqrt(dz*dz+dxy*dxy);
  if(v3d > maxV3D)
    return false;

  // isolation
  fwlite::Handle<susybsm::HSCPIsolationValueMap> IsolationH;
  IsolationH.getByLabel(ev, "HSCPIsolation03");
  if(!IsolationH.isValid()){printf("Invalid IsolationH\n");return false;}
  const edm::ValueMap<susybsm::HSCPIsolation>& IsolationMap = *IsolationH.product();
  susybsm::HSCPIsolation hscpIso = IsolationMap.get((size_t)track.key());
  if(hscpIso.Get_TK_SumEt() > maxTrackEtIso)
    return false;
  double EoP = (hscpIso.Get_ECAL_Energy() + hscpIso.Get_HCAL_Energy())/track->p();
  if(EoP > maxCalEOverPIso)
    return false;

  // ToF
  // ndof check
  if(considerToF &&
      tof->nDof() < minTofNdof &&
      (dttof->nDof() < minTofNdofDt || csctof->nDof() < minTofNdofCsc) )
    return false;
  // Min ToF
  if(considerToF && tof->inverseBeta() < minTofInvBeta)
    return false;
  // Max ToF err
  if(considerToF && tof->inverseBetaErr() > maxTofInvBetaErr)
    return false;


  return true;
}
//
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
std::string doubleToString(double num)
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
//
std::string getHistNameBeg(int lowerNom, float lowerEta)
{
  int etaSlice = lowerEta*10;
  string histName="nom";
  histName+=intToString(lowerNom);
  histName+="to";
  histName+=intToString(lowerNom+1);
  histName+="eta";
  histName+=intToString(etaSlice);
  histName+="to";
  histName+=intToString(etaSlice+2);
  return histName;
}
//
int getLowerNoMFromHistName(std::string histName, std::string histNameEnd)
{
  std::string histNameBeg = histName.substr(0,histName.size()-histNameEnd.size());
  std::string nomString = histNameBeg.substr(3,histNameBeg.find("to")-3);
  return atoi(nomString.c_str());
}
//
float getLowerEtaFromHistName(std::string histName, std::string histNameEnd)
{
  std::string histNameBeg = histName.substr(0,histName.size()-histNameEnd.size());
  std::string etaPart = histNameBeg.substr(histNameBeg.find("eta"));
  std::string etaString = etaPart.substr(3,etaPart.find("to")-3);
  int etaSlice = atof(etaString.c_str());
  return etaSlice/10.0;
}
//
std::string getHistTitleEnd(int lowerNom, float lowerEta, float massCut)
{
  std::string title =" for ";
  title+=intToString(lowerNom);
  title+="-";
  title+=intToString(lowerNom+1);
  title+=", ";
  title+=floatToString(lowerEta);
  title+=" < #eta < ";
  title+=floatToString(lowerEta+0.2);
  title+=", mass > ";
  title+=floatToString(massCut);
  title+=" GeV";
  return title;
}

// taken from Analysis_Step234.C
double DistToHSCP(const susybsm::HSCParticle& hscp, const std::vector<reco::GenParticle>& genColl, int& IndexOfClosest){
  reco::TrackRef   track = hscp.trackRef(); if(track.isNull())return false;

  double RMin = 9999; IndexOfClosest=-1;
  for(unsigned int g=0;g<genColl.size();g++){
    if(genColl[g].pt()<5)continue;
    if(genColl[g].status()!=1)continue;
    int AbsPdg=abs(genColl[g].pdgId());
    if(AbsPdg<1000000)continue;    

    double dR = deltaR(track->eta(), track->phi(), genColl[g].eta(), genColl[g].phi());
    if(dR<RMin){RMin=dR;IndexOfClosest=g;}
  }
  return RMin;
}

// modified from Analysis_Step234.C
int GetInitialNumberOfMCEvent(const vector<string>& fileNames)
{
  int Total = 0;
  fwlite::ChainEvent tree(fileNames);

  for(unsigned int f=0;f<fileNames.size();f++){
    TFile file(fileNames[f].c_str() );
    fwlite::LuminosityBlock ls( &file );
    for(ls.toBegin(); !ls.atEnd(); ++ls){
      fwlite::Handle<edm::MergeableCounter> nEventsTotalCounter;
      nEventsTotalCounter.getByLabel(ls,"nEventsBefSkim");
      if(!nEventsTotalCounter.isValid()){printf("Invalid nEventsTotalCounterH\n");continue;}
      Total+= nEventsTotalCounter->value;
    }
  }
  return Total;
}

// taken from Analysis_Step234.C
bool passesTrigger(const fwlite::Event& ev, bool considerMuon = true, bool considerMET = true)
{
      edm::TriggerResultsByName tr = ev.triggerResultsByName("MergeHLT");
      if(!tr.isValid())return false;

      if(considerMuon)
      {
        if(tr.accept(tr.triggerIndex("HscpPathSingleMu")))return true;
//      if(tr.accept(tr.triggerIndex("HscpPathDoubleMu")))return true;
      }
      if(considerMET)
      {
        if(tr.accept(tr.triggerIndex("HscpPathPFMet")))return true;
//      if(tr.accept(tr.triggerIndex("HscpPathCaloMet")))return true;
      }
      return false;
}

