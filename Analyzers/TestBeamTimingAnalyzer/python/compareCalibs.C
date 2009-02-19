#include <iostream>
#include <fstream>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <map>
#include <TStyle.h>

void SetEStyle()
{
  TStyle* EStyle = new TStyle("EStyle", "E's not Style");

  //set the background color to white
  EStyle->SetFillColor(10);
  EStyle->SetFrameFillColor(10);
  EStyle->SetFrameFillStyle(0);
  EStyle->SetFillStyle(0);
  EStyle->SetCanvasColor(10);
  EStyle->SetPadColor(10);
  EStyle->SetTitleFillColor(0);
  EStyle->SetStatColor(10);

  //dont put a colored frame around the plots
  EStyle->SetFrameBorderMode(0);
  EStyle->SetCanvasBorderMode(0);
  EStyle->SetPadBorderMode(0);

  //use the primary color palette
  EStyle->SetPalette(1,0);
  EStyle->SetNumberContours(255);

  //set the default line color for a histogram to be black
  EStyle->SetHistLineColor(kBlack);

  //set the default line color for a fit function to be red
  EStyle->SetFuncColor(kRed);

  //make the axis labels black
  EStyle->SetLabelColor(kBlack,"xyz");

  //set the default title color to be black
  EStyle->SetTitleColor(kBlack);
   
  // Sizes

  //For Small Plot needs
  //set the margins
 //  EStyle->SetPadBottomMargin(.2);
//   EStyle->SetPadTopMargin(0.08);
//   EStyle->SetPadLeftMargin(0.12);
//   EStyle->SetPadRightMargin(0.12);

//   //set axis label and title text sizes
//   EStyle->SetLabelSize(0.06,"xyz");
//   EStyle->SetTitleSize(0.06,"xyz");
//   EStyle->SetTitleOffset(1.,"x");
//   EStyle->SetTitleOffset(.9,"yz");
//   EStyle->SetStatFontSize(0.04);
//   EStyle->SetTextSize(0.06);
//   EStyle->SetTitleBorderSize(0.5);
  

  //set the margins
  EStyle->SetPadBottomMargin(.15);
  EStyle->SetPadTopMargin(0.08);
  EStyle->SetPadLeftMargin(0.14);
  EStyle->SetPadRightMargin(0.12);
  
  //set axis label and title text sizes
  EStyle->SetLabelSize(0.04,"xyz");
  EStyle->SetTitleSize(0.06,"xyz");
  EStyle->SetTitleOffset(1.,"x");
  EStyle->SetTitleOffset(1.1,"yz");
  EStyle->SetStatFontSize(0.04);
  EStyle->SetTextSize(0.04);
  EStyle->SetTitleBorderSize(0.5);
  //EStyle->SetTitleY(0.5);
  
  //set line widths
  EStyle->SetHistLineWidth(1);
  EStyle->SetFrameLineWidth(2);
  EStyle->SetFuncWidth(2);

  //Paper Size
  EStyle->SetPaperSize(TStyle::kUSLetter);

  // Misc

  //align the titles to be centered
  //Style->SetTextAlign(22);

  //set the number of divisions to show
  EStyle->SetNdivisions(506, "xy");

  //turn off xy grids
  EStyle->SetPadGridX(0);
  EStyle->SetPadGridY(0);

  //set the tick mark style
  EStyle->SetPadTickX(1);
  EStyle->SetPadTickY(1);

  //show the fit parameters in a box
  EStyle->SetOptFit(111111);

  //turn on all other stats
   //EStyle->SetOptStat(0000000);
  EStyle->SetOptStat(1111111);

  //Move stats box
  //EStyle->SetStatX(0.85);

  //marker settings
  EStyle->SetMarkerStyle(8);
  EStyle->SetMarkerSize(0.8);
   
  // Fonts
   EStyle->SetStatFont(42);
   EStyle->SetLabelFont(42,"xyz");
   EStyle->SetTitleFont(42,"xyz");
   EStyle->SetTextFont(42);
//  EStyle->SetStatFont(82);
//   EStyle->SetLabelFont(82,"xyz");
//   EStyle->SetTitleFont(82,"xyz");
//   EStyle->SetTextFont(82);


  //done
  EStyle->cd();
}


//XXX: Main
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

  //Set TStyle
  SetEStyle();

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
  TH1F* differenceHist = new TH1F("relativeDifferenceBetweenCalibs","2(calib1-calib2)/(calib1+calib2) [ns]",75,-0.075,0.075);
  TCanvas can2;
  can2.cd();
  TH2F* scatterHist = new TH2F("calibConst1VsCalibConst2","calib1 vs. calib2 [ns]",60,-10,10,60,-10,10);

  // Loop over calibTimingMap1 and find the corresponding entries in map2
  for(std::map<int,double>::const_iterator map1Itr = timingCalibMap1.begin();
      map1Itr != timingCalibMap1.end(); ++map1Itr)
  {
    std::map<int,double>::const_iterator map2Itr = timingCalibMap2.find(map1Itr->first);
    if(map2Itr==timingCalibMap2.end())
      std::cout << "Could not find crystal: " << map1Itr->first << " in map2." << std::endl;

    differenceHist->Fill(2*(map1Itr->second-map2Itr->second)/(map1Itr->second+map2Itr->second));
    scatterHist->Fill(map1Itr->second,map2Itr->second);
  }


  can.cd();
  differenceHist->Fit("gaus","Q");
  differenceHist->Draw();
  can.Print("compareCalibsDifferenceHist.png");
  can2.cd();
  gStyle->SetOptStat(111111);
  gStyle->SetStatX(0.9);
  scatterHist->Draw("colz");
  can2.Print("compareCalibsScatterHist.png");
  return 0;
}
