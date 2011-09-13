#include <iostream>

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"

#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"

#define MAX_VERTICES 1000
#define MAX_HSCPS    10000
#define MAX_GENS     10000
#define MAX_ECALCRYS 10

int main (int argc, char** argv)
{
  // HSCPTree variables
  bool           Event_triggerL1Bits[192];
  bool           Event_triggerHLTBits[128];
  bool           Event_technicalBits[64];
  unsigned int   Event_EventNumber;
  unsigned int   Event_RunNumber;
  unsigned int   Event_LumiSection;
  unsigned int   Event_BXCrossing;
  unsigned int   Event_Orbit;
  unsigned int   Event_Store;
  unsigned int   Event_Time;
  bool           Event_PhysicsDeclared;
  float          Event_BField;

  unsigned int   NVertices;
  float          Vertex_x           [MAX_VERTICES];
  float          Vertex_y           [MAX_VERTICES];
  float          Vertex_z           [MAX_VERTICES];
  float          Vertex_x_err       [MAX_VERTICES];
  float          Vertex_y_err       [MAX_VERTICES];
  float          Vertex_z_err       [MAX_VERTICES];
  int            Vertex_TrackSize   [MAX_VERTICES];
  float          Vertex_chi2        [MAX_VERTICES];
  float          Vertex_ndof        [MAX_VERTICES];
  bool           Vertex_isFake      [MAX_VERTICES];

  unsigned int   NHSCPs;
  bool           Hscp_hasTrack      [MAX_HSCPS];
  bool           Hscp_hasMuon       [MAX_HSCPS];
  bool           Hscp_hasRpc        [MAX_HSCPS];
  bool           Hscp_hasCalo       [MAX_HSCPS];
  int            Hscp_type          [MAX_HSCPS];
  unsigned int   Track_NOH          [MAX_HSCPS];
  float          Track_p            [MAX_HSCPS];
  float          Track_pt           [MAX_HSCPS];
  float          Track_pt_err       [MAX_HSCPS];
  float          Track_chi2         [MAX_HSCPS];
  unsigned int   Track_ndof         [MAX_HSCPS];
  float          Track_eta          [MAX_HSCPS];
  float          Track_eta_err      [MAX_HSCPS];
  float          Track_phi          [MAX_HSCPS];
  float          Track_phi_err      [MAX_HSCPS];
  float          Track_dz           [MAX_HSCPS];
  float          Track_d0           [MAX_HSCPS];
  int            Track_quality      [MAX_HSCPS];
  int            Track_charge       [MAX_HSCPS];
  float          Track_dEdxE1       [MAX_HSCPS];
  float          Track_dEdxE1_NOS   [MAX_HSCPS];
  unsigned int   Track_dEdxE1_NOM   [MAX_HSCPS];
  float          Track_dEdxE2       [MAX_HSCPS];
  float          Track_dEdxE2_NOS   [MAX_HSCPS];
  unsigned int   Track_dEdxE2_NOM   [MAX_HSCPS];
  float          Track_dEdxE3       [MAX_HSCPS];
  float          Track_dEdxE3_NOS   [MAX_HSCPS];
  unsigned int   Track_dEdxE3_NOM   [MAX_HSCPS];
  float          Track_dEdxD1       [MAX_HSCPS];
  float          Track_dEdxD1_NOS   [MAX_HSCPS];
  unsigned int   Track_dEdxD1_NOM   [MAX_HSCPS];
  float          Track_dEdxD2       [MAX_HSCPS];
  float          Track_dEdxD2_NOS   [MAX_HSCPS];
  unsigned int   Track_dEdxD2_NOM   [MAX_HSCPS];
  float          Track_dEdxD3       [MAX_HSCPS];
  float          Track_dEdxD3_NOS   [MAX_HSCPS];
  unsigned int   Track_dEdxD3_NOM   [MAX_HSCPS];

  float          Calo_ecal_crossedE         [MAX_HSCPS];
  float          Calo_ecal_beta             [MAX_HSCPS];
  float          Calo_ecal_beta_err         [MAX_HSCPS];
  float          Calo_ecal_invBeta_err      [MAX_HSCPS];
  float          Calo_ecal_dEdx             [MAX_HSCPS];
  float          Calo_ecal_time             [MAX_HSCPS];
  float          Calo_ecal_time_err         [MAX_HSCPS];
  int            Calo_ecal_numCrysCrossed   [MAX_HSCPS];
  float          Calo_ecal_swissCrossKs     [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_e1OverE9s        [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_trackLengths     [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_energies         [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_outOfTimeEnergies[MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_chi2s            [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_outOfTimeChi2s   [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_times            [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_timeErrors       [MAX_HSCPS][MAX_ECALCRYS];
  unsigned int   Calo_ecal_detIds           [MAX_HSCPS][MAX_ECALCRYS];
  // End HSCPTree variables

  char* inFile = argv[1];

  if(!inFile)
  {
    std::cout << " No input file specified !" << std::endl;
    return -1;
  }

  TFile* tFile = new TFile(inFile);
  tFile->cd();

  TTree::SetMaxTreeSize(1000*Long64_t(2000000000)); // authorize Trees up to 2 Terabytes
  TTree* MyTree = (TTree*) tFile->Get("HSCPTreeBuilder/HscpTree");
  if(!MyTree)
  {
    std::cout << "Couldn't get the TTree out of the file!" << std::endl;
    return -2;
  }

  MyTree->SetBranchAddress("Event_EventNumber"       ,&Event_EventNumber     );
  MyTree->SetBranchAddress("Event_RunNumber"         ,&Event_RunNumber       );
  MyTree->SetBranchAddress("Event_LumiSection"       ,&Event_LumiSection     );
  MyTree->SetBranchAddress("Event_BXCrossing"        ,&Event_BXCrossing      );
  MyTree->SetBranchAddress("Event_Orbit"             ,&Event_Orbit           );
  MyTree->SetBranchAddress("Event_Store"             ,&Event_Store           );
  MyTree->SetBranchAddress("Event_Time"              ,&Event_Time            );
  MyTree->SetBranchAddress("Event_PhysicsDeclared"   ,&Event_PhysicsDeclared );
  MyTree->SetBranchAddress("Event_technicalBits"     ,Event_technicalBits    );
  MyTree->SetBranchAddress("Event_triggerL1Bits"     ,Event_triggerL1Bits    );
  MyTree->SetBranchAddress("Event_triggerHLTBits"    ,Event_triggerHLTBits   );
  MyTree->SetBranchAddress("Event_BField"            ,&Event_BField          );

  MyTree->SetBranchAddress("NVertices"       ,&NVertices      );
  MyTree->SetBranchAddress("Vertex_x"        ,Vertex_x        );
  MyTree->SetBranchAddress("Vertex_y"        ,Vertex_y        );
  MyTree->SetBranchAddress("Vertex_z"        ,Vertex_z        );
  MyTree->SetBranchAddress("Vertex_x_err"    ,Vertex_x_err    );
  MyTree->SetBranchAddress("Vertex_y_err"    ,Vertex_y_err    );
  MyTree->SetBranchAddress("Vertex_z_err"    ,Vertex_z_err    );
  MyTree->SetBranchAddress("Vertex_TrackSize",Vertex_TrackSize);
  MyTree->SetBranchAddress("Vertex_chi2"     ,Vertex_chi2     );
  MyTree->SetBranchAddress("Vertex_ndof"     ,Vertex_ndof     );
  MyTree->SetBranchAddress("Vertex_isFake"   ,Vertex_isFake   );

  MyTree->SetBranchAddress("NHSCPs"             ,&NHSCPs            );
  MyTree->SetBranchAddress("Hscp_hasTrack"      ,Hscp_hasTrack      );
  MyTree->SetBranchAddress("Hscp_hasMuon"       ,Hscp_hasMuon       );
  MyTree->SetBranchAddress("Hscp_hasRpc"        ,Hscp_hasRpc        );
  MyTree->SetBranchAddress("Hscp_hasCalo"       ,Hscp_hasCalo       );
  MyTree->SetBranchAddress("Hscp_type"          ,Hscp_type          );
  MyTree->SetBranchAddress("Track_NOH"          ,Track_NOH          );
  MyTree->SetBranchAddress("Track_p"            ,Track_p            );
  MyTree->SetBranchAddress("Track_pt"           ,Track_pt           );
  MyTree->SetBranchAddress("Track_pt_err"       ,Track_pt_err       );
  MyTree->SetBranchAddress("Track_chi2"         ,Track_chi2         );
  MyTree->SetBranchAddress("Track_ndof"         ,Track_ndof         );
  MyTree->SetBranchAddress("Track_eta"          ,Track_eta          );
  MyTree->SetBranchAddress("Track_eta_err"      ,Track_eta_err      );
  MyTree->SetBranchAddress("Track_phi"          ,Track_phi          );
  MyTree->SetBranchAddress("Track_phi_err"      ,Track_phi_err      );
  MyTree->SetBranchAddress("Track_d0"           ,Track_d0           );
  MyTree->SetBranchAddress("Track_dz"           ,Track_dz           );
  MyTree->SetBranchAddress("Track_quality"      ,Track_quality      );
  MyTree->SetBranchAddress("Track_charge"       ,Track_charge       );
  MyTree->SetBranchAddress("Track_dEdxE1"       ,Track_dEdxE1       );
  MyTree->SetBranchAddress("Track_dEdxE1_NOS"   ,Track_dEdxE1_NOS   );
  MyTree->SetBranchAddress("Track_dEdxE1_NOM"   ,Track_dEdxE1_NOM   );
  MyTree->SetBranchAddress("Track_dEdxE2"       ,Track_dEdxE2       );
  MyTree->SetBranchAddress("Track_dEdxE2_NOS"   ,Track_dEdxE2_NOS   );
  MyTree->SetBranchAddress("Track_dEdxE2_NOM"   ,Track_dEdxE2_NOM   );
  MyTree->SetBranchAddress("Track_dEdxE3"       ,Track_dEdxE3       );
  MyTree->SetBranchAddress("Track_dEdxE3_NOS"   ,Track_dEdxE3_NOS   );
  MyTree->SetBranchAddress("Track_dEdxE3_NOM"   ,Track_dEdxE3_NOM   );
  MyTree->SetBranchAddress("Track_dEdxD1"       ,Track_dEdxD1       );
  MyTree->SetBranchAddress("Track_dEdxD1_NOS"   ,Track_dEdxD1_NOS   );
  MyTree->SetBranchAddress("Track_dEdxD1_NOM"   ,Track_dEdxD1_NOM   );
  MyTree->SetBranchAddress("Track_dEdxD2"       ,Track_dEdxD2       );
  MyTree->SetBranchAddress("Track_dEdxD2_NOS"   ,Track_dEdxD2_NOS   );
  MyTree->SetBranchAddress("Track_dEdxD2_NOM"   ,Track_dEdxD2_NOM   );
  MyTree->SetBranchAddress("Track_dEdxD3"       ,Track_dEdxD3       );
  MyTree->SetBranchAddress("Track_dEdxD3_NOS"   ,Track_dEdxD3_NOS   );
  MyTree->SetBranchAddress("Track_dEdxD3_NOM"   ,Track_dEdxD3_NOM   );

  MyTree->SetBranchAddress("Calo_ecal_crossedE"      ,Calo_ecal_crossedE);
  MyTree->SetBranchAddress("Calo_ecal_beta"          ,Calo_ecal_beta      );    
  MyTree->SetBranchAddress("Calo_ecal_beta_err"      ,Calo_ecal_beta_err      );
  MyTree->SetBranchAddress("Calo_ecal_invBeta_err"   ,Calo_ecal_invBeta_err   );
  MyTree->SetBranchAddress("Calo_ecal_dEdx"          ,Calo_ecal_dEdx          );
  MyTree->SetBranchAddress("Calo_ecal_time"          ,Calo_ecal_time          );
  MyTree->SetBranchAddress("Calo_ecal_time_err"      ,Calo_ecal_time_err      );
  MyTree->SetBranchAddress("Calo_ecal_numCrysCrossed",Calo_ecal_numCrysCrossed);
  MyTree->SetBranchAddress("Calo_ecal_swissCrossKs"  ,Calo_ecal_swissCrossKs  );
  MyTree->SetBranchAddress("Calo_ecal_e1OverE9s"     ,Calo_ecal_e1OverE9s     );
  MyTree->SetBranchAddress("Calo_ecal_trackLengths"  ,Calo_ecal_trackLengths  );
  MyTree->SetBranchAddress("Calo_ecal_energies"      ,Calo_ecal_energies      );
  MyTree->SetBranchAddress("Calo_ecal_outOfTimeEnergies",Calo_ecal_outOfTimeEnergies);
  MyTree->SetBranchAddress("Calo_ecal_chi2s"      ,Calo_ecal_chi2s      );
  MyTree->SetBranchAddress("Calo_ecal_outOfTimeChi2s"      ,Calo_ecal_outOfTimeChi2s      );
  MyTree->SetBranchAddress("Calo_ecal_times"         ,Calo_ecal_times         );
  MyTree->SetBranchAddress("Calo_ecal_timeErrors"    ,Calo_ecal_timeErrors    );
  MyTree->SetBranchAddress("Calo_ecal_detIds"        ,Calo_ecal_detIds        );

  // print output?
  bool speak = false;

  // define hists here -- why not?
  TFile* outFile = new TFile("ecalTreePlots.root","recreate");
  outFile->cd();
  TH1F* timeHist = new TH1F("timeHist","Time of track-matched Ecal recHits; ns",400,-50,50);
  TH1F* timeOver1GeVHist = new TH1F("time1GeV","Time of track-matched Ecal recHits E > 1 GeV, Fchi2 < 15; ns",400,-50,50);
  TH1F* time800to1GeVHist = new TH1F("time800to1GeV","Time of track-matched Ecal recHits 0.8 < E < 1 GeV, Fchi2 < 15; ns",400,-50,50);
  TH1F* time600to800MeVHist = new TH1F("time600to800MeV","Time of track-matched Ecal recHits 0.6 < E < 0.8 GeV, Fchi2 < 15; ns",400,-50,50);
  TH1F* time400to600MeVHist = new TH1F("time400to600MeV","Time of track-matched Ecal recHits 0.4 < E < 0.6 GeV, Fchi2 < 15; ns",400,-50,50);
  TH1F* timeOver1GeVEneSharingCutHist = new TH1F("time1GeVeneSharingCut","Time of track-matched Ecal recHits E > 1 GeV and energy-sharing cut; ns",400,-50,50);
  TH1F* timeOver1GeVe1e9cutHist = new TH1F("time1GeVe1e9cut","Time of track-matched Ecal recHits E > 1 GeV, e1/e9 < 0.9; ns",400,-50,50);
  TH1F* timeOver1GeVe1e9swissCutHist = new TH1F("time1GeVe1e9swiss","Time of track-matched Ecal recHits E > 1 GeV, e1/e9 < 0.9, swissCrossK < 0.85; ns",400,-50,50);
  TH1F* timeOver1GeVChi2LT10Hist = new TH1F("time1GeVChi2LT10","Time of track-matched Ecal recHits E > 1 GeV and chi2 < 10; ns",400,-50,50);
  TH1F* timeErrorHist = new TH1F("timeError","Time error of track-matched Ecal recHits; ns",20,0,5);
  TH1F* energyHist = new TH1F("energy","Energy of track-matched Ecal recHits; GeV",500,0,20);
  TH1F* dedxTotalHist = new TH1F("dedxTotal","Ecal dE/dx of each candidate; MeV/cm",10000,0,5000);
  TH1F* dedxHist = new TH1F("dedx","Ecal dE/dx of each track-matched hit; MeV/cm",10000,0,5000);
  TH1F* swissCrossKHist = new TH1F("swissCrossK","Swiss cross K of each track-matched hit",200,-2,2);
  TH1F* chi2Hist = new TH1F("chi2","#Chi^2 of each track-matched hit",200,0,100);
  TH1F* floatChi2Hist = new TH1F("floatChi2","Float #Chi^2 of each track-matched hit",200,0,100);
  TH1F* chi2TimeLTn5Hist = new TH1F("chi2TimeLTn5","#Chi^2 of each track-matched hit with t < -5 ns (E > 1 GeV)",200,0,100);
  TH1F* floatChi2TimeLTn5Hist = new TH1F("floatChi2TimeLTn5","Float #Chi^2 of each track-matched hit with t < -5 ns (E > 1 GeV)",200,0,100);
  TH1F* chi2TimeCentralHist = new TH1F("chi2TimeCentral","#Chi^2 of each track-matched hit with -5 < t < 5 (E > 1 GeV)",200,0,100);
  TH1F* floatChi2TimeCentralHist = new TH1F("floatChi2TimeCentral","Float #Chi^2 of each track-matched hit with -5 < t < 5 (E > 1 GeV)",200,0,100);
  TH1F* e1OverE9Hist = new TH1F("e1OverE9","E1/E9 of each track-matched hit",200,0,2);
  TH1F* trackLengthHist = new TH1F("trackLength","Track length of each track-matched hit; cm",48,0,24);
  TH1F* maxCryEnergyOverTotalEnergyHist = new TH1F("maxCryEnergyOverTotalEnergyHist","Max cry energy over total crossed energy;",150,0,1.5);
  TH1F* maxCryTkLengthOverTotalTkLengthHist = new TH1F("maxCryTkLengthOverTotalTkLengthHist","Max cry track length over total track length;",150,0,1.5);
  TH1F* trackNohHist = new TH1F("trackNoh","Track number of hits",30,0,30);
  TH1F* trackChi2Hist = new TH1F("trackChi2","Track chi2",100,0,100);
  TH1F* trackChi2NdfHist = new TH1F("trackChi2Ndf","Track chi2/Ndf",100,0,10);

  TH2F* energyVsTimeHist = new TH2F("energyVsTime","Energy vs. time of each track-matched hit; ns; GeV",400,-50,50,500,0,20);
  TH2F* energyVsChi2Hist = new TH2F("energyVsChi2","Energy vs. #Chi^2 of each track-matched hit;; GeV",200,0,100,500,0,20);
  TH2F* energyVsFloatChi2Hist = new TH2F("energyVsFloatChi2","Energy vs. float #Chi^2 of each track-matched hit;; GeV",200,0,100,500,0,20);
  TH2F* timeErrorVsTimeHist = new TH2F("timeErrorVsTime","Time error vs. time of each track-matched hit; ns; ns",400,-50,50,20,0,5);
  TH2F* swissCrossKvsTimeHist = new TH2F("swissCrossKvsTime","Swiss cross K vs. time of each track-matched hit; ns",400,-50,50,200,-2,2);
  TH2F* swissCrossKvsTime1GevHist = new TH2F("swissCrossKvsTime1GeV","Swiss cross K vs. time of each track-matched hit > 1 GeV; ns",400,-50,50,200,-2,2);
  TH2F* chi2vsTimeHist = new TH2F("chi2vsTime","#Chi^2 vs. time of each track-matched hit; ns",400,-50,50,200,0,100);
  TH2F* floatChi2vsTimeHist = new TH2F("floatChi2vsTime","Floating #Chi^2 vs. time of each track-matched hit; ns",400,-50,50,200,0,100);
  TH2F* chi2vsTime1GevHist = new TH2F("chi2vsTime1GeV","#Chi^2 vs. time of each track-matched hit > 1 GeV; ns",400,-50,50,200,0,100);
  TH2F* floatChi2vsTime1GevHist = new TH2F("floatChi2vsTime1GeV","Floating #Chi^2 vs. time of each track-matched hit > 1 GeV; ns",400,-50,50,200,0,100);
  TH2F* e1OverE9vsTimeHist = new TH2F("e1OverE9vsTime","E1/E9 vs. time of each track-matched hit; ns",400,-50,50,200,0,2);
  TH2F* e1OverE9vsTime1GevHist = new TH2F("e1OverE9vsTime1GeV","E1/E9 vs. time of each track-matched hit > 1 GeV; ns",400,-50,50,200,0,2);
  TH2F* numCrysCrossedVsTimeHist = new TH2F("numCrysCrossedVsTime","Number of crystals crossed vs. time of each track-matched hit; ns",400,-50,50,9,0,9);
  TH2F* trackLengthVsTimeHist = new TH2F("trackLengthVsTime","Track length vs. time of each track-matched hit;ns;cm",400,-50,50,48,0,24);
  TH2F* trackChi2VsTimeHist = new TH2F("trackChi2VsTime","Track chi2 vs. time of each track-matched hit; ns",400,-50,50,500,0,50);
  TH2F* trackPtVsTimeHist = new TH2F("trackPtVsTime","Track Pt vs time of each track-matched hit; ns; GeV",400,-50,50,1000,0,500);
  TH2F* trackD0VsTimeHist = new TH2F("trackD0VsTime","Track d0 vs. time of each track-matched hit; ns",400,-50,50,200,-10,10);
  TH2F* numCrysCrossedVsTotalEnergyHist = new TH2F("numCrysCrossedVsTotalEnergy","Number of crystals crossed vs. total crossed energy; GeV",400,-50,50,500,0,20);
  TH2F* maxEnergyCryVsTotalEnergyHist = new TH2F("maxCryEnergyVsTotalEnergy","Max cry energy vs. total crossed energy; GeV; GeV",500,0,20,500,0,20);
  TH2F* maxCryEnergyRatioVsMaxCryTrackRatioHist = new TH2F("maxCryEnergyRatioVsMaxCryTrackRatio","Max cry energy/total energy vs. max cry track length/total length",110,0,1.1,110,0,1.1);
  TH2F* dedxVsTimeHist = new TH2F("dedxVsTime","dE/dx vs. time of each track-matched hit; ns; MeV/cm",400,-50,50,10000,0,5000);
  tFile->cd();

  // Main loop over entries
  int nEntries = MyTree->GetEntries();
  for (int entry = 0; entry < nEntries; ++entry)
  {
    MyTree->GetEntry(entry);

    for(unsigned int i=0; i < NHSCPs; ++i)
    {
      if(speak)
        std::cout << "Looking at HSCP Tree Content event: " << Event_EventNumber << std::endl;
      if(!Hscp_hasCalo[i])
        continue;

      trackNohHist->Fill(Track_NOH[i]);
      trackChi2Hist->Fill(Track_chi2[i]);
      trackChi2NdfHist->Fill(Track_chi2[i]/Track_ndof[i]);
      dedxTotalHist->Fill(1000*Calo_ecal_dEdx[i]);
      numCrysCrossedVsTotalEnergyHist->Fill(Calo_ecal_crossedE[i],Calo_ecal_numCrysCrossed[i]);
      float maxCryEnergy = -1;
      float maxCryEnergyTkLength = -1;
      float crossedTrackLength = 0;
      float crossedEnergy = 0;
      if(speak)
      {
        std::cout << "Ecal HSCP Tree Content: " << std::endl;
        std::cout << "CrossedEnergy: " <<  Calo_ecal_crossedE[i] <<
          " calculated beta: " << Calo_ecal_beta[i] <<
          " calculated betaError: " << Calo_ecal_beta_err[i] <<
          " calculated 1betaError: " << Calo_ecal_invBeta_err[i] <<
          " calculated total dE/dx: " << 1000*Calo_ecal_dEdx[i] <<
          " calculated time: " << Calo_ecal_time[i] <<
          " calculated timeError: " << Calo_ecal_time_err[i] <<
          " calculated numCrysCrossed: " << Calo_ecal_numCrysCrossed[i] <<
          std::endl;
      }
      for(int j=0; j < Calo_ecal_numCrysCrossed[i]; ++j)
      {
        // Skip pure noise
        if(Calo_ecal_energies[i][j] <= 0)
          continue;

        if(Calo_ecal_energies[i][j] > maxCryEnergy)
        {
          maxCryEnergy = Calo_ecal_energies[i][j];
          maxCryEnergyTkLength = Calo_ecal_trackLengths[i][j];
        }

        crossedTrackLength+=Calo_ecal_trackLengths[i][j];
        crossedEnergy+=Calo_ecal_energies[i][j];
        DetId detId(Calo_ecal_detIds[i][j]);
        timeHist->Fill(Calo_ecal_times[i][j]);
        if(Calo_ecal_energies[i][j] > 1)
        {
          //if(Calo_ecal_outOfTimeChi2s[i][j] < 15)
            timeOver1GeVHist->Fill(Calo_ecal_times[i][j]);
          chi2vsTime1GevHist->Fill(Calo_ecal_times[i][j],Calo_ecal_chi2s[i][j]);
          floatChi2vsTime1GevHist->Fill(Calo_ecal_times[i][j],Calo_ecal_outOfTimeChi2s[i][j]);
          swissCrossKvsTime1GevHist->Fill(Calo_ecal_times[i][j],Calo_ecal_swissCrossKs[i][j]);
          e1OverE9vsTime1GevHist->Fill(Calo_ecal_times[i][j],Calo_ecal_e1OverE9s[i][j]);
          if(Calo_ecal_chi2s[i][j] < 10)
            timeOver1GeVChi2LT10Hist->Fill(Calo_ecal_times[i][j]);
          if(Calo_ecal_e1OverE9s[i][j] < 0.9)
          {
            timeOver1GeVe1e9cutHist->Fill(Calo_ecal_times[i][j]);
            if(Calo_ecal_swissCrossKs[i][j] < 0.85)
              timeOver1GeVe1e9swissCutHist->Fill(Calo_ecal_times[i][j]);
          }
          if(Calo_ecal_times[i][j] < -5)
          {
            chi2TimeLTn5Hist->Fill(Calo_ecal_chi2s[i][j]);
            floatChi2TimeLTn5Hist->Fill(Calo_ecal_outOfTimeChi2s[i][j]);
          }
          else if(Calo_ecal_times[i][j] < 5)
          {
            chi2TimeCentralHist->Fill(Calo_ecal_chi2s[i][j]);
            floatChi2TimeCentralHist->Fill(Calo_ecal_outOfTimeChi2s[i][j]);
          }
        }
        //else if(Calo_ecal_energies[i][j] > 0.8 && Calo_ecal_outOfTimeChi2s[i][j] < 15)
        else if(Calo_ecal_energies[i][j] > 0.8)
          time800to1GeVHist->Fill(Calo_ecal_times[i][j]);
        //else if(Calo_ecal_energies[i][j] > 0.6 && Calo_ecal_outOfTimeChi2s[i][j] < 15)
        else if(Calo_ecal_energies[i][j] > 0.6)
          time600to800MeVHist->Fill(Calo_ecal_times[i][j]);
        //else if(Calo_ecal_energies[i][j] > 0.4 && Calo_ecal_outOfTimeChi2s[i][j] < 15)
        else if(Calo_ecal_energies[i][j] > 0.4)
          time400to600MeVHist->Fill(Calo_ecal_times[i][j]);

        timeErrorHist->Fill(Calo_ecal_timeErrors[i][j]);
        energyHist->Fill(Calo_ecal_energies[i][j]);
        dedxHist->Fill(1000*Calo_ecal_energies[i][j]/Calo_ecal_trackLengths[i][j]);
        swissCrossKHist->Fill(Calo_ecal_swissCrossKs[i][j]);
        e1OverE9Hist->Fill(Calo_ecal_e1OverE9s[i][j]);
        chi2Hist->Fill(Calo_ecal_chi2s[i][j]);
        floatChi2Hist->Fill(Calo_ecal_outOfTimeChi2s[i][j]);
        trackLengthHist->Fill(Calo_ecal_trackLengths[i][j]);
        energyVsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_energies[i][j]);
        energyVsChi2Hist->Fill(Calo_ecal_chi2s[i][j],Calo_ecal_energies[i][j]);
        energyVsFloatChi2Hist->Fill(Calo_ecal_outOfTimeChi2s[i][j],Calo_ecal_energies[i][j]);
        timeErrorVsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_timeErrors[i][j]);
        numCrysCrossedVsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_numCrysCrossed[i]);
        trackLengthVsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_trackLengths[i][j]);
        trackChi2VsTimeHist->Fill(Calo_ecal_times[i][j],Track_chi2[i]);
        trackPtVsTimeHist->Fill(Calo_ecal_times[i][j],Track_pt[i]);
        trackD0VsTimeHist->Fill(Calo_ecal_times[i][j],Track_d0[i]);
        swissCrossKvsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_swissCrossKs[i][j]);
        e1OverE9vsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_e1OverE9s[i][j]);
        chi2vsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_chi2s[i][j]);
        floatChi2vsTimeHist->Fill(Calo_ecal_times[i][j],Calo_ecal_outOfTimeChi2s[i][j]);
        dedxVsTimeHist->Fill(Calo_ecal_times[i][j],1000*Calo_ecal_energies[i][j]/Calo_ecal_trackLengths[i][j]);

        if(speak)
        {
          std::cout << "\tCrossedCry" << j << std::endl;
          std::cout << "\tSwissCrossK: " << Calo_ecal_swissCrossKs[i][j] <<
            " e1/e9: " << Calo_ecal_e1OverE9s[i][j] <<
            " trackLength: " << Calo_ecal_trackLengths[i][j] <<
            " energy: " << Calo_ecal_energies[i][j] <<
            " time: " << Calo_ecal_times[i][j] <<
            " timeError: " << Calo_ecal_timeErrors[i][j];
          if(detId.subdetId()==EcalBarrel)
            std::cout << " detId: " << ((EBDetId)detId) << std::endl;
          else if(detId.subdetId()==EcalEndcap)
            std::cout << " detId: " << ((EEDetId)detId) << std::endl;
          else
            std::cout << " not an EB/EE DetId? subdet=" << detId.subdetId() << std::endl;
        }
      }
      if(maxCryEnergy > 0 && Calo_ecal_numCrysCrossed[i] > 1)
      {
        maxEnergyCryVsTotalEnergyHist->Fill(crossedEnergy,maxCryEnergy);
        maxCryEnergyOverTotalEnergyHist->Fill(maxCryEnergy/crossedEnergy);
        maxCryTkLengthOverTotalTkLengthHist->Fill(maxCryEnergyTkLength/crossedTrackLength);
        maxCryEnergyRatioVsMaxCryTrackRatioHist->Fill(maxCryEnergyTkLength/crossedTrackLength,maxCryEnergy/crossedEnergy);
      }
      for(int j=0; j < Calo_ecal_numCrysCrossed[i]; ++j)
      {
        if(Calo_ecal_energies[i][j] < 1)
          continue;

        if(Calo_ecal_energies[i][j]==maxCryEnergy &&
            maxCryEnergy/crossedEnergy < 0.5*maxCryEnergyTkLength/crossedTrackLength+0.65)
          timeOver1GeVEneSharingCutHist->Fill(Calo_ecal_times[i][j]);
      }
    }
  }


  // write hists here -- why not?
  outFile->cd();
  timeHist->Write();
  timeOver1GeVHist->Write();
  time800to1GeVHist->Write();
  time600to800MeVHist->Write();
  time400to600MeVHist->Write();
  timeOver1GeVEneSharingCutHist->Write();
  timeOver1GeVe1e9cutHist->Write();
  timeOver1GeVe1e9swissCutHist->Write();
  timeOver1GeVChi2LT10Hist->Write();
  timeErrorHist->Write();
  energyHist->Write();
  dedxTotalHist->Write();
  dedxHist->Write();
  swissCrossKHist->Write();
  e1OverE9Hist->Write();
  trackLengthHist->Write();
  chi2Hist->Write();
  floatChi2Hist->Write();
  chi2TimeLTn5Hist->Write();
  floatChi2TimeLTn5Hist->Write();
  chi2TimeCentralHist->Write();
  floatChi2TimeCentralHist->Write();
  numCrysCrossedVsTotalEnergyHist->Write();
  maxCryEnergyOverTotalEnergyHist->Write();
  maxCryTkLengthOverTotalTkLengthHist->Write();
  trackNohHist->Write();
  trackChi2Hist->Write();
  trackChi2NdfHist->Write();

  energyVsTimeHist->Write();
  energyVsChi2Hist->Write();
  energyVsFloatChi2Hist->Write();
  timeErrorVsTimeHist->Write();
  numCrysCrossedVsTimeHist->Write();
  trackLengthVsTimeHist->Write();
  trackChi2VsTimeHist->Write();
  trackPtVsTimeHist->Write();
  trackD0VsTimeHist->Write();
  swissCrossKvsTimeHist->Write();
  e1OverE9vsTimeHist->Write();
  swissCrossKvsTime1GevHist->Write();
  e1OverE9vsTime1GevHist->Write();
  chi2vsTimeHist->Write();
  floatChi2vsTimeHist->Write();
  chi2vsTime1GevHist->Write();
  floatChi2vsTime1GevHist->Write();
  maxEnergyCryVsTotalEnergyHist->Write();
  maxCryEnergyRatioVsMaxCryTrackRatioHist->Write();
  dedxVsTimeHist->Write();
}
