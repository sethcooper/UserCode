std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}

makeBackPredictions()
{
  //XXX Background predictions -- some hardcoding here, assuming 5 Pt bins
  //assume file is already attached as _file0
  int dedxSideBandLeftBin = 11;  // 2 MeV/cm dE/dx
  //int dedxSideBandRightBin = 20; // this is 2 <= dE/dx < 4 MeV/cm
  //int dedxSideBandRightBin = 25; // this is 2 <= dE/dx < 5 MeV/cm
  int dedxSideBandRightBin = 30; // this is 2 <= dE/dx < 6 MeV/cm

  int numPtBins_ = 5;
  int numNomBins_ = 5;
  TH1F* histsForPDFNoMPtBins_[5][5];

  // read in the hists
  for(int i=0; i<5;++i)
  {
    for(int j=0; j<5; ++j)
    {
      int index = numPtBins_*i+j+1;
      std::string name = "histForPDFNoMPtBinsIndex";
      name+=intToString(index);
      std::string fullPath = "trackDeDxNoMAndPtSlices/";
      fullPath+=name;
      histsForPDFNoMPtBins_[i][j] = (TH1F*) _file0->Get(fullPath.c_str());
      if(!histsForPDFNoMPtBins_[i][j])
      {
        std::cout << "Getting hist failed for hist: " << fullPath.c_str() << std::endl;
        return;
      }
    }
  }

  std::cout << "------------------Background predictions" << std::endl;
  std::cout << "H_SB(dE/dx 2-4;Pt 50-150)" << "    L_sig(dE/dx > 4;Pt 15-50" << "    L_SB(dE/dx 2-4;Pt 15-50" << "    H_SB*L_sig/L_SB" << std::endl;
  int Lsb[4]; // in terms of NoM
  int Lsig[4]; // in terms of NoM
  // lowest Pt bin
  for(int j=1; j<numNomBins_; ++j)
  {
    Lsb[j-1] = histsForPDFNoMPtBins_[0][j]->Integral(dedxSideBandLeftBin,dedxSideBandRightBin);
    Lsig[j-1] = histsForPDFNoMPtBins_[0][j]->Integral(dedxSideBandRightBin+1,125);
  }
  // next Pt bin
  std::cout << "Pt 50-150" << std::endl;
  for(int j=1; j<numNomBins_; ++j)
  {
    int Hsb = histsForPDFNoMPtBins_[1][j]->Integral(dedxSideBandLeftBin,dedxSideBandRightBin);
    float backPred = Hsb*Lsig[j-1]/(float)Lsb[j-1];
    std::cout << "NoM bin " << j << "\t\t" << Hsb << "\t\t" << Lsig[j-1] << "\t\t\t" << Lsb[j-1] << "\t\t\t" << backPred << std::endl;
  }
  // next Pt bin
  std::cout << "Pt 150-300" << std::endl;
  for(int j=1; j<numNomBins_; ++j)
  {
    int Hsb = histsForPDFNoMPtBins_[2][j]->Integral(dedxSideBandLeftBin,dedxSideBandRightBin);
    float backPred = Hsb*Lsig[j-1]/(float)Lsb[j-1];
    std::cout << "NoM bin " << j << "\t\t" << Hsb << "\t\t" << Lsig[j-1] << "\t\t\t" << Lsb[j-1] << "\t\t\t" << backPred << std::endl;
  }
  // next Pt bin
  std::cout << "Pt 300-450" << std::endl;
  for(int j=1; j<numNomBins_; ++j)
  {
    int Hsb = histsForPDFNoMPtBins_[3][j]->Integral(dedxSideBandLeftBin,dedxSideBandRightBin);
    float backPred = Hsb*Lsig[j-1]/(float)Lsb[j-1];
    std::cout << "NoM bin " << j << "\t\t" << Hsb << "\t\t" << Lsig[j-1] << "\t\t\t" << Lsb[j-1] << "\t\t\t" << backPred << std::endl;
  }
  // next Pt bin
  std::cout << "Pt > 450" << std::endl;
  for(int j=1; j<numNomBins_; ++j)
  {
    int Hsb = histsForPDFNoMPtBins_[4][j]->Integral(dedxSideBandLeftBin,dedxSideBandRightBin);
    float backPred = Hsb*Lsig[j-1]/(float)Lsb[j-1];
    std::cout << "NoM bin " << j << "\t\t" << Hsb << "\t\t" << Lsig[j-1] << "\t\t\t" << Lsb[j-1] << "\t\t\t" << backPred << std::endl;
  }
}
