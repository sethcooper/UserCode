#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TProfile2D.h"

void createTimingCalibs(char* infile = 0)
{
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  //recoTimeMinusTDCTimeCry_x
  using namespace std;
  if (!infile)
  {
    cout << " No input file specified !" << endl;
    return;
  }
  
  //hardcoded input file
  //string infile = "35.xtalInBeamBugFix.noTimeCalibs.1M.root";
  //string infile = "1.duplicateResults.SM6.root";
  TFile* f = new TFile(infile);

  TCanvas* can = new TCanvas();
  can->cd();
  TH1F* calibHist = new TH1F("timingCalibs","timingCalibs [ns]",500,-25,25);

  TCanvas* can2 = new TCanvas();
  can2->cd();
  TH2F* calibsVsErrors = new TH2F("timingCalibsAndErrors","TimingCalibs vs. errors [ns]",60,0,2,60,0,3);
  
  TCanvas* can3 = new TCanvas();
  can3->cd();
  TProfile2D* calibConstEtaPhiMap = new TProfile2D("calibConstEtaPhiMap","calib const map [ns]",86,1,87,20,1,21);

  //output text file
  ofstream fileStream;
  string fileName = string(infile)+".calibs.txt";
  fileStream.open(fileName.c_str());

  TTree* namesListTree = (TTree*) gDirectory->Get("histoNames");
  std::vector<std::string>* histoNames = new std::vector<std::string>();
  namesListTree->SetBranchAddress("names",&histoNames);
  namesListTree->GetEntry(0);
  
  std::cout << "Found " << histoNames->size() << " histos." << std::endl;

  for(int i=0; i < histoNames->size(); ++i)
  {
    std::string name = histoNames->at(i);
    std::string hiString = name.substr(24,name.length()-1);
    int hashedIndex = atoi(hiString.c_str());
    int ieta = getIeta(hashedIndex);
    int iphi = getIphi(hashedIndex);

    const TH1F* hist = 0;
    hist = (TH1F*) gDirectory->Get(name.c_str());
    //string histName = "recoTimeMinusTDCTimeCry_"+intToString(i);
    //f->GetObject(histName.c_str(),hist);
    if(hist==NULL)
    {
      std::cout << "Null hist found, named " << name.c_str() << std::endl;
      continue;
    }
    hist->Fit("gaus","Q");
    TF1* fit = hist->GetFunction("gaus");
    //double p0 = fit->GetParameter(0); //const
    double p1 = 0;
    double p1err = 0;
    //double p2 = fit->GetParameter(2); //sigma
    //double p2err = fit->GetParError(2);
    //cout << "Bin: " << i << " Sigma of fit: " << p2 << endl;
    if(hist->GetEntries() > 100)
    {
      p1 = fit->GetParameter(1); //mean
      p1err = fit->GetParError(1);
      if(p1err < 0.1)
      {
        //std::cout << "Cry: " << hashedIndex << "\t Calib: " << p1 << "\t Error: " << p1err << std::endl;
        fileStream << hashedIndex << "\t" << p1 << endl;
        calibHist->Fill(p1);
        calibConstEtaPhiMap->Fill(ieta,iphi,p1);
      }
      calibsVsErrors->Fill(p1err, p1 > 0 ? p1 : -1*p1);
    }
  }
  fileStream.close();


  can->cd();
  calibHist->Draw();
  can->Print("calibs1D.png");

  can2->cd();
  calibsVsErrors->SetYTitle("AbsCalibConst");
  calibsVsErrors->SetXTitle("calibConstError");
  calibsVsErrors->Draw("colz");
  can2->Print("calibs2D.png");

  can3->cd();
  calibConstEtaPhiMap->SetYTitle("i#phi");
  calibConstEtaPhiMap->SetXTitle("i#eta");
  double minVal=1;
  double maxVal=-1;
  for(int i=0; i<=88*22; ++i)
  {
    int ents = calibConstEtaPhiMap->GetBinEntries(i);
    if(ents==0)
    {
      calibConstEtaPhiMap->SetBinEntries(i,1);
      calibConstEtaPhiMap->SetBinContent(i,-9999);
    }
    else if(calibConstEtaPhiMap->GetBinContent(i) < minVal)
    {
      minVal = calibConstEtaPhiMap->GetBinContent(i);
    }
    else if(calibConstEtaPhiMap->GetBinContent(i) > maxVal)
    {
      maxVal = calibConstEtaPhiMap->GetBinContent(i);
    }
  }
  gStyle->SetOptStat(1111); 
  calibConstEtaPhiMap->SetMaximum(maxVal+0.5);
  calibConstEtaPhiMap->SetMinimum(minVal-0.5);
  calibConstEtaPhiMap->Draw("colz");
  can3->Print("calibMap.png");
}

std::string intToString(int num)
{
  calibsVsErrors->Draw("colz");
}

std::string intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

int getIeta(int hashedIndex)
{
  EBDetId thisCry = EBDetId::unhashIndex(hashedIndex);
  return thisCry.ieta();
}

int getIphi(int hashedIndex)
{
  EBDetId thisCry = EBDetId::unhashIndex(hashedIndex);
  return thisCry.iphi();
}
