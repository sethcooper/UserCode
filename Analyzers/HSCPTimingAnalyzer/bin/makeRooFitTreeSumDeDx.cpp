#include <iostream>
#include <string>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"

#include "Analyzers/HSCPTimingAnalyzer/interface/HSCPEcalTreeContent.h"
// **************************************************************************************
// SIC May 27 2010
// Convert the new ttree from the HSCPTimingAnalyzer from arrays to straight numbers
// Do this by combining all track-matching crystals together
// **************************************************************************************


// Globals
TFile* myInputRootFile_;
TTree* energyTimeTNtuple_;


// **************************************************************************************
// *******************  Function to parse arguments and load hists **********************
void parseArguments(int arc, char* argv[])
{
  char* infile = argv[1];
  if (!infile)
  {
    std::cout << " No input file specified !" << std::endl;
    exit(-1);
  }

  myInputRootFile_ = new TFile(infile);
  std::string dirName="hscpTimingAnalyzer/";
  std::string ntupleName = "hscpEcalTree";
  std::string fullPath = dirName+ntupleName;
  energyTimeTNtuple_ = (TTree*)myInputRootFile_->Get(fullPath.c_str());
  if(!energyTimeTNtuple_)
  {
    std::cout << "****************No TNtuple with name " << ntupleName << " found in file: " << infile << std::endl;
    exit(-2);
  }
  else
    std::cout << "****************TNtuple found with " << energyTimeTNtuple_->GetEntries() << " entries." << std::endl;

}


int main(int argc, char* argv[])
{
  // Parse arguments and load data
  parseArguments(argc, argv);

  HSCPEcalTreeContent myTreeContent;

  setBranchAddresses(energyTimeTNtuple_,myTreeContent);

  TFile* myOutputFile = new TFile("rooFitHSCPEcalTree.root","recreate");
  // new ttree with dE/dx, time, timeError, energy, trackLength
  TNtuple myNtuple("hscpEcalRooFitTree","hscpEcalRooFitTree","sumDeDx:maxCryTime:maxCryTimeError:maxCryEnergy:sumEnergy:sumTrackLength");

  // Loop over the input tree
  for(int i=0; i < energyTimeTNtuple_->GetEntries(); ++i)
  {
    energyTimeTNtuple_->GetEntry(i);

    // Now here's the magic --
    // Plan for now, for each group of crystals matched to a track:
    //  1) Combine trackLengths and energy to make summed dE/dx (if possible)
    //  2) Take time/timeError from largest energy hit

    // Loop over the tracks in the event
    for(int tk=0; tk < myTreeContent.numTracks; ++tk)
    {
      float dEdx = -1;  //NB: MeV/cm
      float time = -1;
      float timeError = -1;
      float energy = 0;
      float trackLength = 0;

      float maxCryEnergy = -999;

      // Loop over crys associated to the track
      for(int cry=0; cry < myTreeContent.numCrys[tk]; ++cry)
      {
        if(myTreeContent.cryEnergy[tk][cry]>0)
        {
          energy+=myTreeContent.cryEnergy[tk][cry];
          trackLength+=myTreeContent.cryTrackLength[tk][cry];
        }
        if(myTreeContent.cryEnergy[tk][cry]>maxCryEnergy)
        {
          maxCryEnergy=myTreeContent.cryEnergy[tk][cry];
          time = myTreeContent.cryTime[tk][cry];
          timeError = myTreeContent.cryTimeError[tk][cry];
        }
      }
      if(trackLength > 0)
      {
        dEdx = 1000*(energy/trackLength);
        myNtuple.Fill(dEdx,time,timeError,maxCryEnergy,energy,trackLength);
      }
    }
  }

  myNtuple.Write();
  myOutputFile->Close();

  myInputRootFile_->Close();

  return 0;
}
