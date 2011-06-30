#include <iomanip>

std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}

void makeSignalEventNumbers()
{
  //XXX Signal events -- some hardcoding here as well, also assuming 5 Pt bins
  //assume file is already attached as _file0
  //int dedxFirstSearchBin = 20; // this is 2 <= dE/dx < 4 MeV/cm
  //int dedxFirstSearchBin = 25; // this is 2 <= dE/dx < 5 MeV/cm
  int dedxFirstSearchBin = 30; // this is 2 <= dE/dx < 6 MeV/cm (look above 6 MeV/cm)

  float scaleFactor = 26.0/51689.0; // use big-stat sample and scale to number of expected events
  
  // set output to 5 digits
  std::cout << setprecision(5);

  int numPtBins_ = 5;
  int numNomBins_ = 5;
  TH1F* histsForPDFNoMPtBinsMatched_[5][5];

  // read in the hists
  for(int i=0; i<5;++i)
  {
    for(int j=0; j<5; ++j)
    {
      int index = numPtBins_*i+j+1;
      std::string name = "histForPDFNoMPtBinsMatchedIndex";
      name+=intToString(index);
      std::string fullPath = "trackDeDxNoMAndPtSlices/";
      fullPath+=name;
      histsForPDFNoMPtBinsMatched_[i][j] = (TH1F*) _file0->Get(fullPath.c_str());
      if(!histsForPDFNoMPtBinsMatched_[i][j])
      {
        std::cout << "Getting hist failed for hist: " << fullPath.c_str() << std::endl;
        return;
      }
    }
  }

  std::cout << "------------------Signal events" << std::endl;
  std::cout << "Number of signal events wth dE/dx > search bin" << std::endl;
  std::cout << setw(20) << "NoM 6-10" << setw(20) << "NoM 11-15" << setw(20) << "NoM16-20" << setw(20) << "NoM21+" << std::endl;
  std::cout << "Pt 50-150: " << setw(10);
  for(int j=1; j<numNomBins_; ++j)
  {
    std::cout << (float)(scaleFactor*histsForPDFNoMPtBinsMatched_[1][j]->Integral(dedxFirstSearchBin,125));
    std::cout << setw(20);
  }
  std::cout << setw(0) << std::endl << "Pt 150-300: " << setw(10);
  for(int j=1; j<numNomBins_; ++j)
  {
    std::cout << (float)(scaleFactor*histsForPDFNoMPtBinsMatched_[2][j]->Integral(dedxFirstSearchBin,125));
    std::cout << setw(20);
  }
  std::cout << setw(0) << std::endl << "Pt 300-450: " << setw(10);
  for(int j=1; j<numNomBins_; ++j)
  {
    std::cout << (float)(scaleFactor*histsForPDFNoMPtBinsMatched_[3][j]->Integral(dedxFirstSearchBin,125));
    std::cout << setw(20);
  }
  std::cout << setw(0) << std::endl << "Pt > 450: " << setw(10);
  for(int j=1; j<numNomBins_; ++j)
  {
    std::cout << (float)(scaleFactor*histsForPDFNoMPtBinsMatched_[4][j]->Integral(dedxFirstSearchBin,125));
    std::cout << setw(20);
  }
  std::cout << setw(0) << std::endl;

}
