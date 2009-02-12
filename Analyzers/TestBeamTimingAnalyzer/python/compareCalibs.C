#include <iostream>
#include <fstream>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <map>
#include <TStyle.h>

int main(int argc, char* argv[])
{
  using namespace std;
  char* infile1 = argv[1];
  char* infile2 = argv[2];

  if(!infile1 && !infile2)
  {
    std::cout << "Error: 2 input files were not specified." << std::endl;
    return -1;
  }

  ifstream calibFile1, calibFile2;
  calibFile1.open(infile1,ifstream::in);
  calibFile2.open(infile2,ifstream::in);
  map<int,double> timingCalibMap1;
  map<int,double> timingCalibMap2;

  //Open and read calibFile1
  if(calibFile1.good())
  {
    int hashIndex;
    double calibConst;
    while(calibFile1.good())
    {
      calibFile1 >> hashIndex >> calibConst;
      //cout << "(Map1) Hash: " << hashIndex << "\t calib: " << calibConst << endl;
      timingCalibMap1.insert(std::make_pair(hashIndex,calibConst));
      //timingCalibMap1.Add(hashIndex,calibConst);
    }
  }
  else
  {
    std::cout << "ERROR: Calib file1 not opened." << std::endl;
  }
  //Open and read calibFile2
  if(calibFile2.good())
  {
    int hashIndex;
    double calibConst;
    while(calibFile2.good())
    {
      calibFile2 >> hashIndex >> calibConst;
      //cout << "(Map2) Hash: " << hashIndex << "\t calib: " << calibConst << endl;
      timingCalibMap2.insert(std::make_pair(hashIndex,calibConst));
      //timingCalibMap2.Add(hashIndex,calibConst);
    }
  }
  else
  {
    std::cout << "ERROR: Calib file2 not opened." << std::endl;
  }

  TCanvas can;
  can.cd();
  TH1F* differenceHist = new TH1F("differenceBetweenCalibs","Calib1-calib2 [ns]",100,-0.15,0.15);
  TCanvas can2;
  can2.cd();
  TH2F* scatterHist = new TH2F("calibConst1VsCalibConst2","calib1 vs. calib2 [ns]",25,-10,5,25,-10,5);

  // Loop over calibTimingMap1 and find the corresponding entries in map2
  for(std::map<int,double>::const_iterator map1Itr = timingCalibMap1.begin();
      map1Itr != timingCalibMap1.end(); ++map1Itr)
  {
    std::map<int,double>::const_iterator map2Itr = timingCalibMap2.find(map1Itr->first);
    if(map2Itr==timingCalibMap2.end())
      std::cout << "Could not find crystal: " << map1Itr->first << " in map2." << std::endl;

    differenceHist->Fill(map1Itr->second-map2Itr->second);
    scatterHist->Fill(map1Itr->second,map2Itr->second);
  }

  //set the background color to white
  gStyle->SetFillColor(10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFillStyle(0);
  gStyle->SetCanvasColor(10);
  gStyle->SetPadColor(10);
  gStyle->SetTitleFillColor(0);
  gStyle->SetStatColor(10);
  //dont put a colored frame around the plots
  gStyle->SetFrameBorderMode(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  //use the primary color palette
  gStyle->SetPalette(1,0);
  gStyle->SetNumberContours(255);
  //set the default line color for a histogram to be black
  gStyle->SetHistLineColor(kBlack);
  //set the default line color for a fit function to be red
  gStyle->SetFuncColor(kRed);
  //make the axis labels black
  gStyle->SetLabelColor(kBlack,"xyz");
  //set the default title color to be black
  gStyle->SetTitleColor(kBlack);
  //set line widths
  gStyle->SetHistLineWidth(1);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetFuncWidth(2);

  gStyle->SetOptStat(111111);
  gStyle->SetStatX(0.9);
  //gStyle->SetStatY(0.75);
  gStyle->SetOptFit(111111);

  can.cd();
  differenceHist->Fit("gaus","Q");
  differenceHist->Draw();
  can.Print("compareCalibsDifferenceHist.png");
  can2.cd();
  scatterHist->Draw("colz");
  can2.Print("compareCalibsScatterHist.png");
  return 0;
}
