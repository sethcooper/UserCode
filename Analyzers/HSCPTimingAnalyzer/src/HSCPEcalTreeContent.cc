#include "Analyzers/HSCPTimingAnalyzer/interface/HSCPEcalTreeContent.h"


void setBranchAddresses(TTree* tree, HSCPEcalTreeContent& treeVars)
{
  tree->SetBranchAddress("numCrys",treeVars.numCrys);
  tree->SetBranchAddress("isEBcry",treeVars.isEBcry);
  tree->SetBranchAddress("hashedIndex",treeVars.hashedIndex);
  tree->SetBranchAddress("cryEnergy",treeVars.cryEnergy);
  tree->SetBranchAddress("cryAmplitude",treeVars.cryAmplitude);
  tree->SetBranchAddress("cryTime",treeVars.cryTime);
  tree->SetBranchAddress("cryTimeError",treeVars.cryTimeError);
  tree->SetBranchAddress("cryTrackLength",treeVars.cryTrackLength);
  tree->SetBranchAddress("cryDeDx",treeVars.cryDeDx);

  // Track variables
  tree->SetBranchAddress("trackPt",treeVars.trackPt);
  tree->SetBranchAddress("trackP",treeVars.trackP);
  tree->SetBranchAddress("trackEta",treeVars.trackEta);
  tree->SetBranchAddress("trackPhi",treeVars.trackPhi);
  tree->SetBranchAddress("trackDz",treeVars.trackDz);
  tree->SetBranchAddress("trackD0",treeVars.trackD0);
  tree->SetBranchAddress("trackQuality",treeVars.trackQuality);
  tree->SetBranchAddress("trackCharge",treeVars.trackCharge);
  tree->SetBranchAddress("trackChi2",treeVars.trackChi2);
  tree->SetBranchAddress("trackNdof",treeVars.trackNdof);
  tree->SetBranchAddress("trackNOH",treeVars.trackNOH);
  tree->SetBranchAddress("numCrysCrossed",treeVars.numCrysCrossed);

  tree->SetBranchAddress("numTracks",&treeVars.numTracks);
  tree->SetBranchAddress("eventL1a",&treeVars.eventL1a);
  tree->SetBranchAddress("runNum",&treeVars.runNum);
  tree->SetBranchAddress("lumiNum",&treeVars.lumiNum);
}

void setBranches(TTree* tree, HSCPEcalTreeContent& treeVars)
{
  tree->Branch("numTracks",&treeVars.numTracks,"numTracks/I");
  tree->Branch("eventL1a",&treeVars.eventL1a,"eventL1a/I");
  tree->Branch("runNum",&treeVars.runNum,"runNum/I");
  tree->Branch("lumiNum",&treeVars.lumiNum,"lumiNum/I");

  tree->Branch("numCrys",treeVars.numCrys,"numCrys[numTracks]/I");
  tree->Branch("isEBcry",treeVars.isEBcry,"isEBcry[numTracks][numCrys]/B");
  tree->Branch("hashedIndex",treeVars.hashedIndex,"hashedIndex[numTracks][numCrys]/I");
  tree->Branch("cryEnergy",treeVars.cryEnergy,"cryEnergy[numTracks][numCrys]/F");
  tree->Branch("cryAmplitude",treeVars.cryAmplitude,"cryAmplitude[numTracks][numCrys]/F");
  tree->Branch("cryTime",treeVars.cryTime,"cryTime[numTracks][numCrys]/F");
  tree->Branch("cryTimeError",treeVars.cryTimeError,"cryTimeError[numTracks][numCrys]/F");
  tree->Branch("cryTrackLength",treeVars.cryTrackLength,"cryTrackLength[numTracks][numCrys]/F");
  tree->Branch("cryDeDx",treeVars.cryDeDx,"cryDeDx[numTracks][numCrys]/F");

  // Track variables
  tree->Branch("trackPt",treeVars.trackPt,"trackPt[numTracks]/F");
  tree->Branch("trackP",treeVars.trackP,"trackP[numTracks]/F");
  tree->Branch("trackEta",treeVars.trackEta,"trackEta[numTracks]/F");
  tree->Branch("trackPhi",treeVars.trackPhi,"trackPhi[numTracks]/F");
  tree->Branch("trackDz",treeVars.trackDz,"trackDz[numTracks]/F");
  tree->Branch("trackD0",treeVars.trackD0,"trackD0[numTracks]/F");
  tree->Branch("trackQuality",treeVars.trackQuality,"trackQuality[numTracks]/I");
  tree->Branch("trackCharge",treeVars.trackCharge,"trackCharge[numTracks]/I");
  tree->Branch("trackChi2",treeVars.trackChi2,"trackChi2[numTracks]/F");
  tree->Branch("trackNdof",treeVars.trackNdof,"trackNdof[numTracks]/I");
  tree->Branch("trackNOH",treeVars.trackNOH,"trackNOH[numTracks]/I");
  tree->Branch("numCrysCrossed",treeVars.numCrysCrossed,"numCrysCrossed[numTracks]/I");
}

void initializeBranches(TTree* tree, HSCPEcalTreeContent& treeVars)
{
  treeVars.numTracks = -1;
  treeVars.eventL1a = -1;
  treeVars.runNum = -1;
  treeVars.lumiNum = -1;

  treeVars.numTracks = -1;
  for(int i=0; i<MAXTRACKS; ++i)
  {
    treeVars.trackPt[i] = -1;
    treeVars.trackP[i] = -1;
    treeVars.trackEta[i] = -10;
    treeVars.trackPhi[i] = -10;
    treeVars.trackDz[i] = -10;
    treeVars.trackD0[i] = -10;
    treeVars.trackQuality[i] = -10;
    treeVars.trackCharge[i] = -10;
    treeVars.trackChi2[i] = -10;
    treeVars.trackNdof[i] = -10;
    treeVars.trackNOH[i] = -10;
    treeVars.numCrysCrossed[i] = -10;
    treeVars.numCrys[i] = -10;

    for(int j=0; j<MAXCRYS; ++j)
    {
      treeVars.isEBcry[i][j] = false;
      treeVars.hashedIndex[i][j] = -10;
      treeVars.cryEnergy[i][j] = -10;
      treeVars.cryAmplitude[i][j] = -10;
      treeVars.cryTime[i][j] = -10;
      treeVars.cryTimeError[i][j] = -10;
      treeVars.cryTrackLength[j][j] = -10;
      treeVars.cryDeDx[i][j] = -10;
    }
  }


}
