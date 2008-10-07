#include <string>
#include <vector>

void createResGraph(Char_t* infile = 0, int numBins = 40)  
{
  if (!infile)
  {
    cout << " No input file specified !" << endl;
    return;
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
  energies.push_back(3);
  energyErrors.push_back(2);
  energies.push_back(7.5);
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

  for(int i=0;i<numBins;i++)
  {
    string histName = "deltaBetCrysEnergyBin_"+intToString(i);
    TH1F* hist = 0;
    f->GetObject(histName.c_str(),hist);
    hist->GetXaxis()->SetTitle("Difference from average time [ns]");
    hist->Fit("gaus","Q");
    TF1 *fit = hist->GetFunction("gaus");
    //double p0 = fit->GetParameter(0); //const
    //double p1 = fit->GetParameter(1); //mean
    double p2 = fit->GetParameter(2); //sigma
    double p2err = fit->GetParError(2);
    sigmas.push_back(p2);
    sigmaErrors.push_back(p2err);
    //cout << "param0: " << p0 << " param1: " << p1 << " param2:" << p2 << endl;
    //cout << "Bin: " << i << " sigma: " << p2 << " error:" << p2err << endl;
  }

  //turn off x errors
  energyErrors.clear();
  for(int i=0;i<numBins;++i)
    energyErrors.push_back(0);

  TGraphErrors* graph = new TGraphErrors(energies.size(), &(*energies.begin()),&(*sigmas.begin()),&(*energyErrors.begin()),&(*sigmaErrors.begin()));
  string yTitle = "sigma(t) [ns]";
  string xTitle = "energy [GeV]";
  graph->GetYaxis()->SetTitle(yTitle.c_str());
  graph->GetXaxis()->SetTitle(xTitle.c_str());
  graph->SetTitle("");
  graph->SetMarkerColor(1);
  graph->SetMarkerStyle(8);
  graph->SetMarkerSize(1);
  TF1 *myfit = new TF1("myfit","[0]/sqrt(x)+[1]", 0, 50);
  graph->Fit("myfit","R");
  graph->Draw("AP");
  
  TCanvas* t = new TCanvas();
  t->cd();
  vector<double>::const_iterator energy = energies.begin()+3;
  vector<double>::const_iterator sigma = sigmas.begin()+3;
  vector<double>::const_iterator sigmaErr = sigmaErrors.begin()+3;
  TGraphErrors* graph2 = new TGraphErrors(energies.size()-3, &(*energy),&(*sigma),&(*energyErrors.begin()),&(*sigmaErr));
  graph2->SetTitle("Non-fitted energy bins excluded");
  graph2->SetMarkerColor(1);
  graph2->SetMarkerStyle(8);
  graph2->SetMarkerSize(1);
  graph2->Fit("myfit","R");
  graph2->Draw("AP");


  //Draw Fit vs. TDC time
  TCanvas* t2 = new TCanvas();
  t2->cd();
  string histName2 = "fitVsTDCTimeEnergyBin_12";//+intToString(12);
  TH2F* hist2 = 0;
  f->GetObject(histName2.c_str(),hist2);
  TProfile* prof = hist2->ProfileX();
  prof->Fit("pol1");
  prof->Draw();
}

std::string intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}
