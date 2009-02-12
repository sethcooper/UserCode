void recoTDCTimeMap(Char_t* infile = 0)  
{

//TODO: Run this in batch mode (root -b) or else the generated png looks funky!

  if (!infile)
  {
    cout << " No input file specified !" << endl;
    return;
  }
  
  gStyle->SetOptStat(111);
  gStyle->SetOptFit(0101);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.1);

  TFile* f = new TFile(infile);
  TCanvas tc("TDCtimingMap","Timing Map",0,0,1500,1500);
  tc.Divide(5,5);

  for(int i=0;i<25;i++)
  {
    tc.cd(i+1);
    string histName = "fitTimeVsTDC_Cry"+intToString(i);
    TH2F* hist = 0;
    f->GetObject(histName.c_str(),hist);
    //TH1D* hist1d = hist->ProjectionY();
    //hist1d->SetTitle(Form("Reco time of Cry %d [ns]",i));
    //hist1d->Fit("gaus","Q");
    //TF1 *fit = hist1d->GetFunction("gaus");
    ////double p0 = fit->GetParameter(0); //const
    //double p1 = fit->GetParameter(1); //mean
    ////double p2 = fit->GetParameter(2); //sigma
    ////double p2err = fit->GetParError(2);
    //TPaveText *pt = new TPaveText(0.20,0.7,0.5,0.87);//,"NDC");
    ////note the strict C++ syntax (not required for interactive CINT)
    ////                 // NDC sets the coordinates relative to the dimensions of the pad
    ////                                  // useful if you might change the layout of your histos
    //pt->SetFillColor(0);  //text now is black on white
    //pt->SetTextSize(0.04);
    //pt->SetTextAlign(12);
    //string textString = "mean of dist="+doubleToString(p1);
    //text = pt->AddText(textString.c_str());
    //pt->Draw();
    ////hist1d->GetXaxis()->SetRangeUser(-30,30);
    hist->GetXaxis()->SetRangeUser(0,25);
    hist->GetYaxis()->SetRangeUser(-10,30);
    hist->Draw("colz");
    //string fileName = "recoVsTDCTimeOfCry"+intToString(i);
    //fileName+=".png";
    //hist->Print(fileName.c_str());
    tc.Update();

    //tc.Print(fileName.c_str());
  }
  tc.Draw();
  //tc.Update();
  tc.Print("TDCVsRecoTimingMap.png");
}

std::string intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

std::string doubleToString(double num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}
