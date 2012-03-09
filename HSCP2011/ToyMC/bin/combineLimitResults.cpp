//#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

#include "TFile.h"
#include "RooStats/HypoTestInverterResult.h"

using namespace std;
using namespace RooStats;

vector<string> getFilesInDir(string dir, string signal)
{
  vector<string> files;
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL)
  {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return vector<string>();
  }

  while ((dirp = readdir(dp)) != NULL) {
    string fileName = string(dirp->d_name);
    if(fileName.find("doLimits") == string::npos)
      continue;
    if(fileName.find(signal) == string::npos)
      continue;
    string toAdd = dir;
    toAdd+="/";
    toAdd+=fileName;
    files.push_back(toAdd);
  }
  closedir(dp);

  return files;
}

int main()
{
  //string outputDir = "FARM_makeScaledAndDoLimits_allTracks_allEvents_etaReweight_sameBinning_bugfix_ptErr_absEta_feb6/outputs/";
  //string outputDir = "FARM_makeScaledAndDoLimits_allTracks_allEvents_pt50Ias0p1_ih3_pt45_sameBins_ptPresel_absEta_feb8/outputs/";
  //string outputDir = "FARM_makeScaledAndDoLimits_allTracks_allEvents_pt50Ias0p1_ih3_pt45_sameBins_ptPresel_absEta_feb15/outputs/";
  string outputDir = "FARM_moreModels_cutPt50GeVIas0p1_includeTightRPC_Mar05/outputs/";

  vector<string> signals;
  signals.push_back("Gluino600");
  signals.push_back("Gluino1000");
  signals.push_back("Gluino1100");
  signals.push_back("Gluino1200");
  signals.push_back("GMStau100");
  signals.push_back("GMStau200");
  signals.push_back("GMStau308");
  signals.push_back("GMStau432");
  signals.push_back("Stop200");
  //signals.push_back("Stop600");

  for(vector<string>::const_iterator signalItr = signals.begin();
      signalItr != signals.end(); ++signalItr)
  {
    int fileCounter = 0;
    cout << "Doing " << *signalItr << endl;
    //vector<string> thisSignalFiles = getFilesInDir(outputDir+*signalItr);
    vector<string> thisSignalFiles = getFilesInDir(outputDir, *signalItr);
    HypoTestInverterResult* r = 0;
    HypoTestInverterResult* thisResult = 0;
    TFile* thisTFile = 0;
    for(vector<string>::const_iterator fileItr = thisSignalFiles.begin();
        fileItr != thisSignalFiles.end(); ++fileItr)
    {
      thisTFile = new TFile(fileItr->c_str(),"read");
      if(!r)
        r =  dynamic_cast<HypoTestInverterResult*>(thisTFile->Get("result_SigXsec"));
      else
      {
        delete thisResult;
        thisResult = dynamic_cast<HypoTestInverterResult*>(thisTFile->Get("result_SigXsec"));
        r->Add(*thisResult);
      }
      ++fileCounter;
    }

    cout << "Done merging " << fileCounter << " files, writing file " << *signalItr+"_doLimits_combined.root"
      << endl;
    string outputPath = outputDir;
    outputPath+=*signalItr;
    outputPath+="_doLimits_combined.root";
    TFile* thisSignalCombinedTFile = new TFile(outputPath.c_str(),"recreate");
    thisSignalCombinedTFile->cd();
    r->SetName("result_SigXsec");
    r->Write();
    thisSignalCombinedTFile->Close();
  }

  return 0;
}
