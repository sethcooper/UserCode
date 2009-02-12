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
    //tc->Print(histName.c_str());
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
  graph2->GetYaxis()->SetTitle(yTitle.c_str());
  graph2->GetXaxis()->SetTitle("A/#sigma");
  graph2->GetYaxis()->SetRangeUser(0,8);
  graph2->GetXaxis()->SetRangeUser(0,1100);
  graph2->SetMarkerColor(1);
  graph2->SetMarkerStyle(8);
  graph2->SetMarkerSize(1);
  myfit->SetParameter(0,1000);
  myfit->SetParameter(1,0.4);
  graph2->Fit("myfit","R");
  graph2->Draw("AP");
  float sqrtPar0 = TMath::Sqrt(myfit->GetParameter(0));
  float sqrtPar1 = TMath::Sqrt(myfit->GetParameter(1));
  char buffer [50];
  sprintf(buffer,"#sigma(#Deltat)=#frac{%f}{A/#sigma}#oplus%f",sqrtPar0,sqrtPar1);
  TLatex latex;
  latex.DrawLatex(280,3.45,buffer);
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

