#include <iostream>
#include <string>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "Analyzers/HSCPTimingAnalyzer/interface/HSCPEcalTreeContent.h"


void Scale0TProfile2D(TProfile2D* myprof)
{
  int nxbins = myprof->GetNbinsX();
  int nybins = myprof->GetNbinsY();

  for (int i=0; i<=(nxbins+2)*(nybins+2); i++ ) {   
    //Double_t oldcont = myprof->GetBinContent(i);
    Double_t binents = myprof->GetBinEntries(i);
    if (binents == 0 ) {binents =1.;myprof->SetBinEntries(i,1);myprof->SetBinContent(i,-150.); }
  }
}

int main(int argc,  char * argv[])
{
  using namespace std;

  char* infile = argv[1];
  if(!infile)
  {
    cout << " No input file specified!" << endl;
    return -1;
  }
  string outfileName = "plotHSCPEcalTree.root";
  if(argc >= 3)
    outfileName = argv[2];
 
  TFile* f = TFile::Open(infile);
  f->cd();

  //
  std::cout << "Getting Tree from file..." << std::endl;

  TTree* eventTimingInfoTree = ((TTree*) f->Get("hscpTimingAnalyzer/hscpEcalTree"));
  if(!eventTimingInfoTree)
  {
    std::cout << " No TTree in the event named hscpEcalTree." << std::endl;
    return -2;
  } 

  TFile* outfile = new TFile(outfileName.c_str(),"recreate");
  outfile->cd();
  //book hists
  TH1F* pullEB = new TH1F("pullEB","pull EB",100,-10,10);
  TH1F* pullEE = new TH1F("pullEE","pull EE",100,-10,10);
  TH1F* pullEBp5GeV = new TH1F("pullEBp5","pull EB hits, 1 GeV > E > 0.5 GeV",100,-10,10);
  TH1F* pullEEp5GeV = new TH1F("pullEEp5","pull EE hits, 1 GeV > E > 0.5 GeV",100,-10,10);
  TH1F* pullEB1GeV = new TH1F("pullEB1","pull EB hits, 2 GeV > E > 1 GeV",100,-10,10);
  TH1F* pullEE1GeV = new TH1F("pullEE1","pull EE hits, 2 GeV > E > 1 GeV",100,-10,10);
  TH1F* pullEB2GeV = new TH1F("pullEB2","pull EB hits, 3 GeV > E > 2 GeV",100,-10,10);
  TH1F* pullEE2GeV = new TH1F("pullEE2","pull EE hits, 3 GeV > E > 2 GeV",100,-10,10);
  TH1F* pullEB3GeV = new TH1F("pullEB3","pull EB hits, E > 3 GeV",100,-10,10);
  TH1F* pullEE3GeV = new TH1F("pullEE3","pull EE hits, E > 3 GeV",100,-10,10);
  
  TH1F* timingDistEB = new TH1F("timingDistEB","Timing of EB hits",300,-15,15);
  TH1F* timingDistEE = new TH1F("timingDistEE","Timing of EE hits",300,-15,15);
  TH1F* timingDistEBp5GeV = new TH1F("timingDistEBp5GeV","Timing of EB hits, 1 GeV > E > 0.5 GeV",300,-15,15);
  TH1F* timingDistEEp5GeV = new TH1F("timingDistEEp5GeV","Timing of EE hits, 1 GeV > E > 0.5 GeV",300,-15,15);
  TH1F* timingDistEB1GeV = new TH1F("timingDistEB1GeV","Timing of EB hits, 2 GeV > E > 1 GeV",300,-15,15);
  TH1F* timingDistEE1GeV = new TH1F("timingDistEE1GeV","Timing of EE hits, 2 GeV > E > 1 GeV",300,-15,15);
  TH1F* timingDistEB2GeV = new TH1F("timingDistEB2GeV","Timing of EB hits, 3 GeV > E > 2 GeV",300,-15,15);
  TH1F* timingDistEE2GeV = new TH1F("timingDistEE2GeV","Timing of EE hits, 3 GeV > E > 2 GeV",300,-15,15);
  TH1F* timingDistEB3GeV = new TH1F("timingDistEB3GeV","Timing of EB hits, E > 3 GeV",300,-15,15);
  TH1F* timingDistEE3GeV = new TH1F("timingDistEE3GeV","Timing of EE hits, E > 3 GeV",300,-15,15);

  HSCPEcalTreeContent treeVars;
  setBranchAddresses(eventTimingInfoTree, treeVars);

  int nEnts = eventTimingInfoTree->GetEntries();
  for(int i=0; i<nEnts;i++)
  {
     eventTimingInfoTree->GetEvent(i);

     cout << "DEBUG: " << "num tracks: " << treeVars.numTracks << " L1a: " << treeVars.eventL1a
       << " run num: " << treeVars.runNum << " lumi: " << treeVars.lumiNum << endl;

     for(int tk=0; tk < treeVars.numTracks; ++tk) // loop over tracks
     {
       cout << "DEBUG: " << "num crys: " << treeVars.numCrys[tk] << " track Pt: " << treeVars.trackPt[tk] << endl;
       for(int cry=0; cry < treeVars.numCrys[tk]; ++cry)  // loop over crys associated to the track
       {
         float cryEne = treeVars.cryEnergy[tk][cry];
         float cryTime = treeVars.cryTime[tk][cry];
         float cryTimeError = treeVars.cryTimeError[tk][cry];
         bool isEBcry = treeVars.isEBcry[tk][cry];
         cout << "Debug: " << " Cry energy: " << cryEne << " cry time: " << cryTime << " cryTimeError: " << cryTimeError
           << " isEBcry " << isEBcry << " hashedIndex " << treeVars.hashedIndex[tk][cry]
           << " cryTrackLength: " << treeVars.cryTrackLength[tk][cry] << endl;

         if(isEBcry)
         {
           pullEB->Fill(cryTime/cryTimeError);
           timingDistEB->Fill(cryTime);
           if(cryEne < 0.5)
             continue;
           else if(cryEne < 1)
           {
             pullEBp5GeV->Fill((cryTime-0.5346)/cryTimeError);
             timingDistEBp5GeV->Fill(cryTime);
           }
           else if(cryEne < 2)
           {
             pullEB1GeV->Fill((cryTime-0.05669)/cryTimeError);
             timingDistEB1GeV->Fill(cryTime);
           }
           else if(cryEne < 3)
           {
             pullEB2GeV->Fill(cryTime/cryTimeError);
             timingDistEB2GeV->Fill(cryTime);
           }
           else
           {
             pullEB3GeV->Fill(cryTime/cryTimeError);
             timingDistEB3GeV->Fill(cryTime);
           }
         }
         else
         {
           pullEE->Fill(cryTime/cryTimeError);
           timingDistEE->Fill(cryTime);
           if(cryEne < 0.5)
             continue;
           else if(cryEne < 1)
           {
             pullEEp5GeV->Fill(cryTime/cryTimeError);
             timingDistEEp5GeV->Fill(cryTime);
           }
           else if(cryEne < 2)
           {
             pullEE1GeV->Fill(cryTime/cryTimeError);
             timingDistEE1GeV->Fill(cryTime);
           }
           else if(cryEne < 3)
           {
             pullEE2GeV->Fill(cryTime/cryTimeError);
             timingDistEE2GeV->Fill(cryTime);
           }
           else
           {
             pullEE3GeV->Fill(cryTime/cryTimeError);
             timingDistEE3GeV->Fill(cryTime);
           }
         }

       }
     }

  }

  outfile->cd();
  pullEB->Write(); 
  pullEE->Write();
  pullEBp5GeV->Write();
  pullEEp5GeV->Write();
  pullEB1GeV->Write(); 
  pullEE1GeV->Write(); 
  pullEB2GeV->Write(); 
  pullEE2GeV->Write(); 
  pullEB3GeV->Write(); 
  pullEE3GeV->Write(); 

  timingDistEB->Write();
  timingDistEE->Write();
  timingDistEBp5GeV->Write();
  timingDistEEp5GeV->Write();
  timingDistEB1GeV->Write();
  timingDistEE1GeV->Write();
  timingDistEB2GeV->Write();
  timingDistEE2GeV->Write();
  timingDistEB3GeV->Write();
  timingDistEE3GeV->Write();

  outfile->Close();
  f->Close();

  return 0;
}
