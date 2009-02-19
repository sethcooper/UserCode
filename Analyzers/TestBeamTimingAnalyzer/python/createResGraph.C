#include <string>
#include <iostream>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TF1.h>
#include <vector>
#include <sstream>


std::string intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

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

int main(int argc, char* argv[])
{
  using namespace std;

  char* infile = argv[1];
  if (!infile)
  {
    cout << " No input file specified !" << endl;
    return -1;
  }
  
  TFile* f = new TFile(infile);
  vector<double> sigmas; 
  vector<double> sigmaErrors;
  vector<double> energies;
  vector<double> energyErrors;

  energies.push_back(.050);
  energyErrors.push_back(.050);
  energies.push_back(.150);
  energyErrors.push_back(.50);
  energies.push_back(.250);
  energyErrors.push_back(.50);
  energies.push_back(.350);
  energyErrors.push_back(.50);
  energies.push_back(.450);
  energyErrors.push_back(.50);
  energies.push_back(.550);
  energyErrors.push_back(.50);
  energies.push_back(.650);
  energyErrors.push_back(.50);
  energies.push_back(.750);
  energyErrors.push_back(.50);
  energies.push_back(.850);
  energyErrors.push_back(.50);
  energies.push_back(.950);
  energyErrors.push_back(.50);
  energies.push_back(1.5);
  energyErrors.push_back(1);
  energies.push_back(2.5);
  energyErrors.push_back(1);
  energies.push_back(3.5);
  energyErrors.push_back(1);
  energies.push_back(4.5);
  energyErrors.push_back(1);
  energies.push_back(6.25);
  energyErrors.push_back(2.5);
  energies.push_back(8.75);
  energyErrors.push_back(2.5);

  energies.push_back(15);
  energyErrors.push_back(5);
  energies.push_back(25);
  energyErrors.push_back(5);
  energies.push_back(35);
  energyErrors.push_back(5);
  energies.push_back(45);
  energyErrors.push_back(5);
  energies.push_back(55);
  energyErrors.push_back(5);
  energies.push_back(65);
  energyErrors.push_back(5);
  energies.push_back(75);
  energyErrors.push_back(5);
  energies.push_back(85);
  energyErrors.push_back(5);
  energies.push_back(95);
  energyErrors.push_back(5);
  energies.push_back(105);
  energyErrors.push_back(5);
  energies.push_back(115);
  energyErrors.push_back(5);
  energies.push_back(125);
  energyErrors.push_back(5);
  energies.push_back(135);
  energyErrors.push_back(5);
  energies.push_back(145);
  energyErrors.push_back(5);
  energies.push_back(155);
  energyErrors.push_back(5);
  energies.push_back(165);
  energyErrors.push_back(5);
  energies.push_back(175);
  energyErrors.push_back(5);
  energies.push_back(185);
  energyErrors.push_back(5);
  energies.push_back(195);
  energyErrors.push_back(5);
  
  //deltaBetCrysEnergyBin_20 fitVsTDCTimeEnergyBin_36
  //crysConsideredEtaPhiEnergyBin_36

  //Set TStyle
  SetEStyle();
  
  for(int i=0;i<20;i++)
  {
    gStyle->SetStatX(0);
    TCanvas* tc = new TCanvas("DeltaBetCrys","Delta between crys",12,12,1000,500);
    tc->Divide(2,1);
    tc->cd(1);

    string histName = "deltaBetCrysEnergyBin_"+intToString(i);
    TH1F* hist = 0;
    f->GetObject(histName.c_str(),hist);
    if(!hist)
      continue;

    hist->GetXaxis()->SetTitle("Difference between crystal pairs [ns]");
    if(i<10)
      hist->GetXaxis()->SetRangeUser(-10,10);
    else
      hist->GetXaxis()->SetRangeUser(-5,5);
    hist->Fit("gaus","Q");

    string histName2 = "crysConsideredEtaPhiEnergyBin_"+intToString(i);
    TH2F* hist2 = 0;
    f->GetObject(histName2.c_str(),hist2);
    tc->cd(2);
    hist2->Draw("colz");
    gStyle->SetStatX(0.9);

    histName+=".png";
    //XXX
    tc->Print(histName.c_str());
    TF1 *fit = hist->GetFunction("gaus");
    //double p0 = fit->GetParameter(0); //const
    //double p1 = fit->GetParameter(1); //mean
    double p2 = fit->GetParameter(2); //sigma
    double p2err = fit->GetParError(2);
    //cout << "Bin: " << i << " Sigma of fit: " << p2 << endl;
    sigmas.push_back(p2);
    sigmaErrors.push_back(p2err);
    //cout << "param0: " << p0 << " param1: " << p1 << " param2:" << p2 << endl;
    //cout << "Bin: " << i << " sigma: " << p2 << " error:" << p2err << endl;
  }

  //turn off x errors
  energyErrors.clear();
  for(int i=0;i<40;++i)
    energyErrors.push_back(0);

  vector<double> aOverSigmas;
  for(vector<double>::const_iterator itr = energies.begin(); itr != energies.end(); ++itr)
  {
    aOverSigmas.push_back(*itr/(1.2*0.035));
  }

  TGraphErrors* graph = new TGraphErrors(aOverSigmas.size(), &(*aOverSigmas.begin()),&(*sigmas.begin()),&(*energyErrors.begin()),&(*sigmaErrors.begin()));
  string yTitle = "sigma(t) [ns]";
  string xTitle = "energy [GeV]";
  graph->GetYaxis()->SetTitle(yTitle.c_str());
  graph->GetXaxis()->SetTitle(xTitle.c_str());
  graph->SetTitle("");
  graph->SetMarkerColor(1);
  graph->SetMarkerStyle(8);
  graph->SetMarkerSize(1);
  //TF1 *myfit = new TF1("myfit","[0]/sqrt(x)+[1]", 0, 50);
  //TF1 *myfit = new TF1("myfit","[0]/(x*x+[2])+[1]", 0, 50);
  TF1 *myfit = new TF1("myfit","sqrt([0]/(x*x)+[1])", 0.0001, 1000);
  myfit->SetParameter(0,1000);
  myfit->SetParameter(1,0.4);
  graph->Fit("myfit","R");
  graph->Draw("AP");
  
  TCanvas* t = new TCanvas();
  t->cd();
  //vector<double>::const_iterator energy = energies.begin()+2;
  vector<double>::const_iterator aOverSigma = aOverSigmas.begin()+2;
  vector<double>::const_iterator sigma = sigmas.begin()+2;
  vector<double>::const_iterator sigmaErr = sigmaErrors.begin()+2;
  TGraphErrors* graph2 = new TGraphErrors(aOverSigmas.size()-2, &(*aOverSigma),&(*sigma),&(*energyErrors.begin()),&(*sigmaErr));
  graph2->SetTitle("Non-fitted energy bins excluded");
  graph2->GetYaxis()->SetTitle("#sigma(#Deltat) [ns]");
  graph2->GetXaxis()->SetTitle("A/#sigma");
  graph2->GetYaxis()->SetTitleOffset(0.4);
  graph2->GetYaxis()->SetRangeUser(0,8);
  graph2->GetXaxis()->SetRangeUser(0,1100);
  graph2->SetMarkerColor(1);
  graph2->SetMarkerStyle(8);
  graph2->SetMarkerSize(1);
  myfit->SetParameter(0,1000);
  myfit->SetParameter(1,0.4);
  graph2->Fit("myfit","R");
  gStyle->SetStatX(0.95);
  t->SetLogy();
  t->SetLogx();
  graph2->GetYaxis()->SetRangeUser(pow(10,-1),8);
  graph2->Draw("AP");
  float sqrtPar0 = TMath::Sqrt(myfit->GetParameter(0));
  float sqrtPar1 = TMath::Sqrt(myfit->GetParameter(1));
  char buffer [50];
  sprintf(buffer,"#sigma(#Deltat) = #frac{%f}{A/#sigma} #oplus %f ns",sqrtPar0,sqrtPar1);
  TLatex latex;
  latex.DrawLatex(50,2.0,buffer);
  t->Print("resolution.png");

  return 0;
  //Draw Fit vs. TDC time
  //TCanvas* t2 = new TCanvas();
  //t2->cd();
  //string histName2 = "fitVsTDCTimeEnergyBin_12";//+intToString(12);
  //TH2F* hist2 = 0;
  //f->GetObject(histName2.c_str(),hist2);
  //TProfile* prof = hist2->ProfileX();
  //prof->Fit("pol1");
  //prof->Draw();
}

