#include "TChain.h" 

const int MAXCRYS = 25; // max crys per track (5x5 around track)
const int MAXTRACKS  = 100; // max interesting tracks per event

struct HSCPEcalTreeContent // 1 entry per event
{
  int numCrys[MAXTRACKS]; // num crys associated to the track
  bool isEBcry[MAXTRACKS][MAXCRYS];
  int hashedIndex[MAXTRACKS][MAXCRYS];
  float cryEnergy[MAXTRACKS][MAXCRYS];
  float cryAmplitude[MAXTRACKS][MAXCRYS];
  float cryTime[MAXTRACKS][MAXCRYS];
  float cryTimeError[MAXTRACKS][MAXCRYS];
  float cryTrackLength[MAXTRACKS][MAXCRYS];
  float cryDeDx[MAXTRACKS][MAXCRYS];

  // Track variables
  float trackPt[MAXTRACKS];
  float trackP[MAXTRACKS];
  float trackEta[MAXTRACKS];
  float trackPhi[MAXTRACKS];
  float trackDz[MAXTRACKS];
  float trackD0[MAXTRACKS];
  int trackQuality[MAXTRACKS];
  int trackCharge[MAXTRACKS];
  float trackChi2[MAXTRACKS];
  unsigned int trackNdof[MAXTRACKS];
  unsigned int trackNOH[MAXTRACKS];
  int numCrysCrossed[MAXTRACKS];
  int numTracks;

  int eventL1a;
  int runNum;
  int lumiNum;
  //TODO
  //int triggers[200];
  //int techtriggers[200];

};

// ------------------------------------------------------------------------
//! branch addresses settings
void setBranchAddresses(TTree* tree, HSCPEcalTreeContent& treeVars);

// ------------------------------------------------------------------------
//! create branches for a tree
void setBranches(TTree* tree, HSCPEcalTreeContent& treeVars);

// ------------------------------------------------------------------------
//! initialize branches
void initializeBranches(TTree* tree, HSCPEcalTreeContent& treeVars);

