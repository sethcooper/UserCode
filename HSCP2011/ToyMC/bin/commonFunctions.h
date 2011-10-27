#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/Math/interface/deltaR.h"

// preselection -- adapted from Analysis_Step234.C
bool passesPreselection(const susybsm::HSCParticle& hscp,  const reco::DeDxData& dedxSObj,
    const reco::DeDxData& dedxMObj, const fwlite::Event& ev)
{
  //TODO: use common py for these
  // preselections
  const int minTrackNoH = 11;
  const float minTrackValidFraction = 0.8;
  const int minValidPixelHits = 2;
  const unsigned int minIasNoM = 5;
  const unsigned int minIhNoM = 5;
  //const int minTofNdof = 8;
  //const int minTofNdofDt = 6;
  //const int minTofNdofCsc = 6;
  const int minTrackQualityMask = 2;
  const float maxTrackChi2OverNdf = 5.0;
  //const float minTrackPt = 35.0;
  //const float minIas = 0.0;
  //const float minIh = 3.0;
  //const float minTofInvBeta = 1.0;
  //const float maxTofIntBetaErr = 0.07;
  const unsigned int minNumVertices = 1;
  const float maxV3D = 0.5;
  const float maxTrackEtIso = 50;
  const float maxCalEOverPIso = 0.3;
  //const float maxTrackPtErr = 0.25;
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


  return true;
}

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
// taken from Analysis_Step234.C
int HowManyChargedHSCP(const std::vector<reco::GenParticle>& genColl){
  int toReturn = 0;
  for(unsigned int g=0;g<genColl.size();g++){
    if(genColl[g].pt()<5)continue;
    if(genColl[g].status()!=1)continue;
    int AbsPdg=abs(genColl[g].pdgId());
    if(AbsPdg<1000000)continue;
    if(AbsPdg==1000993 || AbsPdg==1009313 || AbsPdg==1009113 || AbsPdg==1009223 || AbsPdg==1009333 || AbsPdg==1092114 || AbsPdg==1093214 || AbsPdg==1093324)continue; //Skip neutral gluino RHadrons
    if(AbsPdg==1000622 || AbsPdg==1000642 || AbsPdg==1006113 || AbsPdg==1006311 || AbsPdg==1006313 || AbsPdg==1006333)continue;  //skip neutral stop RHadrons
    toReturn++;
  }
  return toReturn;
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


