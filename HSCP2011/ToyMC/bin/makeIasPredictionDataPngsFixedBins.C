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


makeIasPredictionDataPngsFixedBins()
{
  TCanvas t("myCanvas","myCanvas",100,100,800,600);
  t.cd();
  t.SetLogy();

  for(int nom=5; nom < 23; nom+=2)
  {
    for(int etaSlice=0; etaSlice < 16; etaSlice+=2)
    {
      string histName="nom";
      histName+=intToString(nom);
      histName+="to";
      histName+=intToString(nom+1);
      histName+="eta";
      histName+=intToString(etaSlice);
      histName+="to";
      histName+=intToString(etaSlice+2);
      histName+="iasPredictionFixedHist";

      TH1D* iasPredHistThisSlice = (TH1D*)_file0->Get(histName.c_str());

      histName="nom";
      histName+=intToString(nom);
      histName+="to";
      histName+=intToString(nom+1);
      histName+="eta";
      histName+=intToString(etaSlice);
      histName+="to";
      histName+=intToString(etaSlice+2);
      histName+="dRegionFixedHist";

      TH1D* iasDataHistThisSlice = (TH1D*)_file0->Get(histName.c_str());

      iasDataHistThisSlice->SetLineColor(kBlack);
      iasDataHistThisSlice->SetMarkerColor(kBlack);
      iasDataHistThisSlice->SetMarkerStyle(23);
      iasDataHistThisSlice->Draw();
      iasPredHistThisSlice->SetLineColor(kBlue);
      iasPredHistThisSlice->SetMarkerColor(kBlue);
      iasPredHistThisSlice->SetMarkerStyle(22);
      iasPredHistThisSlice->Draw("same");
      string name="nom";
      name+=intToString(nom);
      name+="to";
      name+=intToString(nom+1);
      name+="eta";
      name+=intToString(etaSlice);
      name+="to";
      name+=intToString(etaSlice+2);
      name+="iasDataPred";
      t.Print((name+".png").c_str());
    }
  }
}
