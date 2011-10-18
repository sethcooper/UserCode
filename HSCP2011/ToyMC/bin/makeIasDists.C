// ****** helper functions
std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}
//
std::string floatToString(float num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}


// start from 0 for slices
// 0-11 for NoM (slice 11--> NoM>=24)
// 0-11 for eta (slice 11--> 2.2 < |eta| < 2.4)
makeIasDists(int nomSlice, int etaSlice, float massCut, int numEntries)
{
  if(nomSlice < 0 || nomSlice > 11 || etaSlice < 0 || etaSlice > 11)
  {
    cout << "Usage error." << endl
      << "makeIasDists(nomSlice, etaSlice, massCut, numEntries" << endl
      << "nomSlice, etaSlice can be between 0 and 11" << endl;
    return;
  }

  string histName="trackIasFactorizedIhPNoM";
  histName+=intToString(nomSlice);
  histName+="etaSlice";
  histName+=intToString(etaSlice);
  histName+="massCut";
  histName+=intToString(massCut);

  int nomLow = nomSlice*2+1;
  int nomHigh = nomSlice*2+2;
  float etaLow = etaSlice*0.2;
  float etaHigh = (etaSlice+1)*0.2;
  string histTitle="Track Ias from Ih,P SB hists with mass > ";
  histTitle+=floatToString(massCut);
  histTitle+=" for nom ";
  histTitle+=intToString(nomLow);
  histTitle+="-";
  histTitle+=intToString(nomHigh);
  histTitle+=", |#eta| ";
  histTitle+=floatToString(etaLow);
  histTitle+="-";
  histTitle+=floatToString(etaHigh);


  TFile* myOutputFile = new TFile((histName+".root").c_str(),"recreate");
  myOutputFile->cd();
  TH1F* trackIasFactorizedIhPHist = new TH1F(histName.c_str(),histTitle.c_str(),400,0,1);
  TH1F* massGeneratedHist = new TH1F("massGenerated","Masses generated;GeV",100,0,1000);
  _file0->cd();
  const float k = 2.468;
  const float c = 2.679;

  string dedxHistName="trackDeDxE1VsDeDxD3LowPSBNoMSlice";
  dedxHistName+=intToString(nomSlice*2+1);
  TH3F* dedxHist = (TH3F*)_file0->Get(dedxHistName.c_str());
  string dedxHistName2="trackDeDxE1VsDeDxD3LowPSBNoMSlice";
  dedxHistName2+=intToString(nomSlice*2+2);
  TH3F* dedxHist2 = (TH3F*)_file0->Get(dedxHistName2.c_str());
  dedxHist->Add(dedxHist2);
  string pHistName="trackPMipSBNoMSlice";
  pHistName+=intToString(nomSlice*2+1);
  TH2F* pHist = (TH2F*)_file0->Get(pHistName.c_str());
  string pHistName2="trackPMipSBNoMSlice";
  pHistName2+=intToString(nomSlice*2+2);
  TH2F* pHist2 = (TH2F*)_file0->Get(pHistName2.c_str());
  pHist->Add(pHist2);
  // handle last NoM slice case (NoM>=23)
  if(nomSlice==11)
  {
    // in this case, we add NoM bins 23-30 together
    // already have 23-24, so add 25,26,27,28,29,30
    string dedxHistName25="trackDeDxE1VsDeDxD3LowPSBNoMSlice25";
    TH3F* dedxHist25 = (TH3F*)_file0->Get(dedxHistName25.c_str());
    string dedxHistName26="trackDeDxE1VsDeDxD3LowPSBNoMSlice26";
    TH3F* dedxHist26 = (TH3F*)_file0->Get(dedxHistName26.c_str());
    string dedxHistName27="trackDeDxE1VsDeDxD3LowPSBNoMSlice27";
    TH3F* dedxHist27 = (TH3F*)_file0->Get(dedxHistName27.c_str());
    string dedxHistName28="trackDeDxE1VsDeDxD3LowPSBNoMSlice28";
    TH3F* dedxHist28 = (TH3F*)_file0->Get(dedxHistName28.c_str());
    string dedxHistName29="trackDeDxE1VsDeDxD3LowPSBNoMSlice29";
    TH3F* dedxHist29 = (TH3F*)_file0->Get(dedxHistName29.c_str());
    string dedxHistName30="trackDeDxE1VsDeDxD3LowPSBNoMSlice30";
    TH3F* dedxHist30 = (TH3F*)_file0->Get(dedxHistName30.c_str());
    dedxHist->Add(dedxHist25);
    dedxHist->Add(dedxHist26);
    dedxHist->Add(dedxHist27);
    dedxHist->Add(dedxHist28);
    dedxHist->Add(dedxHist29);
    dedxHist->Add(dedxHist30);
    // now do the same for the P hist
    string pHistName25="trackPMipSBNoMSlice25";
    TH2F* pHist25 = (TH2F*)_file0->Get(pHistName25.c_str());
    string pHistName26="trackPMipSBNoMSlice26";
    TH2F* pHist26 = (TH2F*)_file0->Get(pHistName26.c_str());
    string pHistName27="trackPMipSBNoMSlice27";
    TH2F* pHist27 = (TH2F*)_file0->Get(pHistName27.c_str());
    string pHistName28="trackPMipSBNoMSlice28";
    TH2F* pHist28 = (TH2F*)_file0->Get(pHistName28.c_str());
    string pHistName29="trackPMipSBNoMSlice29";
    TH2F* pHist29 = (TH2F*)_file0->Get(pHistName29.c_str());
    string pHistName30="trackPMipSBNoMSlice30";
    TH2F* pHist30 = (TH2F*)_file0->Get(pHistName30.c_str());
    pHist->Add(pHist25);
    pHist->Add(pHist26);
    pHist->Add(pHist27);
    pHist->Add(pHist28);
    pHist->Add(pHist29);
    pHist->Add(pHist30);
  }

  // set ranges to make the eta slice and project
  dedxHist->GetZaxis()->SetRange(etaSlice*2,etaSlice*2+1);
  TH2F* etaSlicedNoMSlicedDeDxHist = dedxHist->Project3D("yx");
  TH1D* etaSlicedNoMSlicedPHist = pHist->ProjectionX("etaSlicedNoMSlicedPHist_px",etaSlice*2,(etaSlice+1)*2);
  // check to make sure everything went OK (not all hists checked for NoM>=23 case)
  if(etaSlicedNoMSlicedPHist->GetEntries() <= 0 || etaSlicedNoMSlicedDeDxHist->GetEntries() <= 0
      || pHist->GetEntries() <= 0 || dedxHist->GetEntries() <= 0
      || pHist2->GetEntries() <= 0 || dedxHist2->GetEntries() <= 0)
  {
    cout << "ERROR in histogram: " << endl
      << "\t etaSlicedNoMSlicedPHist\t\t" << etaSlicedNoMSlicedPHist->GetEntries() << endl
      << "\t etaSlicedNoMSlicedDeDxHist\t\t" << etaSlicedNoMSlicedDeDxHist->GetEntries() << endl
      << "\t " << pHistName << "\t\t\t" << pHist->GetEntries() << endl
      << "\t " << pHistName2 << "\t\t\t" << pHist2->GetEntries() << endl
      << "\t " << dedxHistName << "\t" << dedxHist->GetEntries() << endl
      << "\t " << dedxHistName2 << "\t" << dedxHist2->GetEntries() << endl;
    return;
  }

  // generate the Ias dist
  int entryCount = 0;
  int trialsCount = 0;
  while(entryCount < numEntries)
  {
    double ih, ias;
    etaSlicedNoMSlicedDeDxHist->GetRandom2(ias,ih);
    double p = etaSlicedNoMSlicedPHist->GetRandom();
    float massSqr = (ih-c)*pow(p,2)/k;
    ////debug
    //std::cout << "Generated points--  Ias=" << ias << " ; mass=" << sqrt(massSqr) << std::endl;
    ++trialsCount;

    if(massSqr < 0)
      continue;
    //XXX TURNED OFF FOR TESTING XXX
    //else if(sqrt(massSqr) < massCut)
    //  continue;
    //XXX TURNED OFF FOR TESTING XXX

    massGeneratedHist->Fill(sqrt(massSqr));
    trackIasFactorizedIhPHist->Fill(ias);
    ++entryCount;
  }
  
  //std::cout << "Generated " << numEntries << " points using " << trialsCount << " trials: "
  //  << 100*(float)(trialsCount-numEntries)/trialsCount << "\% loss" << std::endl;

  //trackIasFactorizedIhPHist->Draw();
  //TCanvas t;
  //t.cd();
  //t.SetLogy();
  //trackIasFactorizedIhPHist->Draw();
  //t.Print((histName+".C").c_str());
  //t.Print((histName+".png").c_str());
  myOutputFile->cd();
  trackIasFactorizedIhPHist->Write();
  pHist->Write();
  dedxHist->Write();
  etaSlicedNoMSlicedPHist->Write();
  etaSlicedNoMSlicedDeDxHist->Write();
  massGeneratedHist->Write();
  myOutputFile->Close();

}
