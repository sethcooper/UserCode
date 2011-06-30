// Create my fit function
Double_t fitTail(Double_t *x, Double_t *par)
{
  double splicePoint1 = 5.;
  double splicePoint2 = 7;
  double fitVal = 0;

  if(x[0] < splicePoint1)
  {
    double gaus1 = par[3]*TMath::Gaus(splicePoint1,par[4],par[5]);
    double exp1 = TMath::Exp(par[2]+par[1]*splicePoint1+par[0]*splicePoint1*splicePoint1);
    double par7 = (TMath::Log(exp1-gaus1)-par[6])/splicePoint1;
    fitVal = par[3]*TMath::Gaus(x[0],par[4],par[5])+TMath::Exp(par[6]+par7*x[0]);
  }
  else if(x[0] < splicePoint2)
  {
    fitVal = TMath::Exp(par[2]+par[1]*x[0]+par[0]*x[0]*x[0]);
  }
  else
  {
    // constrained exp: exp^(alpha*x+beta)
    double alpha = 2*par[0]*splicePoint2+par[1];
    double beta = -1*par[0]*splicePoint2*splicePoint2+par[2];
    fitVal = TMath::Exp(alpha*x[0]+beta);
  }
  return fitVal;
}

fitPNoMSlice3()
{
  TVirtualFitter::SetMaxIterations(10000);
  trackDeDxNoMAndPSlices->cd();

  // Use my defined function
  TF1 *myFunc2 = new TF1("myFunc2",fitTail,3,25,7);
  myFunc2->SetLineColor(4);
  myFunc2->SetParameters(0.13,-3.0,17,5.9e4,3.14,0.3,19);
  histForPDFNoMPBinsIndex3->Fit("myFunc2","RL+");

  c1->SetLogy();

  // Calculate chi2 in restricted range
  double totalChi2 = 0;
  for(int binx=21; binx < 125; binx++)
  {
    double funcValBinCenter = myFunc2->Eval(histForPDFNoMPBinsIndex3->GetBinCenter(binx));
    double binEntries = histForPDFNoMPBinsIndex3->GetBinContent(binx);
    totalChi2+= pow(binEntries-funcValBinCenter,2)/funcValBinCenter;
  }
  int ndf = 125-20-7;
  std::cout << "chi2 / ndf in (4 <= x < 25) bin range 21 - 125 = " << totalChi2 << " / " << ndf << std::endl;
}
