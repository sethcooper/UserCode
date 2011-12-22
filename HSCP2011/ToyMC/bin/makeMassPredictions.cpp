#include "TROOT.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TChain.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TTree.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TPaveText.h" 
#include "TRandom3.h"
#include "TProfile.h"
#include "TDirectory.h"
#include "TRandom3.h"

#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooArgSet.h"
#include "RooPlot.h"
#include "RooKeysPdf.h"

#include <vector>
#include <string>
#include <algorithm>

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "PhysicsTools/FWLite/interface/CommandLineParser.h"
#include "FWCore/ParameterSet/interface/ProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/PythonProcessDesc.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/TrackReco/interface/DeDxData.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCParticle.h"
#include "AnalysisDataFormats/SUSYBSMObjects/interface/HSCPIsolation.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "commonFunctions.h"


std::string getHistTitleBeg(int lowerNom, float lowerEta, float pSB, float ihSB)
{
  std::string histTitle = "NoM ";
  histTitle+=intToString(lowerNom);
  histTitle+="-";
  histTitle+=intToString(lowerNom+1);
  histTitle+=", ";
  histTitle+=floatToString(lowerEta);
  histTitle+=" < #eta < ";
  histTitle+=floatToString(lowerEta+0.2);
  histTitle+=", (pt < ";
  histTitle+=floatToString(pSB);
  histTitle+=", Ih > ";
  histTitle+=floatToString(ihSB);
  histTitle+=")";
  return histTitle;
}

void setBinLabelsEntriesHist(TH2F* hist)
{
  hist->GetXaxis()->SetBinLabel(1,"0.0-0.2");
  hist->GetXaxis()->SetBinLabel(2,"0.2-0.4");
  hist->GetXaxis()->SetBinLabel(3,"0.4-0.6");
  hist->GetXaxis()->SetBinLabel(4,"0.6-0.8");
  hist->GetXaxis()->SetBinLabel(5,"0.8-1.0");
  hist->GetXaxis()->SetBinLabel(6,"1.0-1.2");
  hist->GetXaxis()->SetBinLabel(7,"1.2-1.4");
  hist->GetXaxis()->SetBinLabel(8,"1.4-1.6");
  hist->GetXaxis()->SetBinLabel(9,"1.6-1.8");
  hist->GetXaxis()->SetBinLabel(10,"1.8-2.0");
  hist->GetXaxis()->SetBinLabel(11,"2.0-2.2");
  hist->GetXaxis()->SetBinLabel(12,"2.0-2.4");
  hist->GetYaxis()->SetBinLabel(1,"5-6");
  hist->GetYaxis()->SetBinLabel(2,"7-8");
  hist->GetYaxis()->SetBinLabel(3,"9-10");
  hist->GetYaxis()->SetBinLabel(4,"11-12");
  hist->GetYaxis()->SetBinLabel(5,"13-14");
  hist->GetYaxis()->SetBinLabel(6,"15-16");
  hist->GetYaxis()->SetBinLabel(7,"17-18");
  hist->GetYaxis()->SetBinLabel(8,"19-20");
  hist->GetYaxis()->SetBinLabel(9,"21-22");
}

//std::vector<double> computeVariableBins(TH1* hist)
//{
//  double binWidth = 0.02;
//  std::vector<double> binArray;
//
//  int lastBinWithContent = hist->GetNbinsX();
//  while(hist->GetBinContent(lastBinWithContent)<=0 && lastBinWithContent>0)
//  {
//    //std::cout << "BinContent for bin: " << lastBinWithContent << " = " <<
//    //  hist->GetBinContent(lastBinWithContent) << std::endl;
//    lastBinWithContent--;
//  }
//
//  if(lastBinWithContent==0)
//    lastBinWithContent=hist->GetNbinsX();
//
//  double xCoordOfLastBinWithContent = hist->GetBinLowEdge(lastBinWithContent);
//  for(double x = hist->GetXaxis()->GetXmin(); x < xCoordOfLastBinWithContent; x+=binWidth)
//    binArray.push_back(x);
//
//  if(xCoordOfLastBinWithContent-binArray.back() >= binWidth)
//    binArray.push_back(xCoordOfLastBinWithContent);
//  binArray.push_back(hist->GetXaxis()->GetXmax());
//
//  return binArray;
//}

// compute variable bin array from fixed bin hist
std::vector<double> computeVariableBins(TH1* hist)
{
  std::vector<double> binArray;
  binArray.push_back(hist->GetXaxis()->GetXmin());

  int binsExamined = 0;
  int binsToCombine = 2;
  while(binsExamined < hist->GetNbinsX())
  {
    int lastBin = hist->FindFixBin(binArray.back());
    int nextBin = lastBin+binsToCombine-1;

    double proposedBinContent = hist->Integral(lastBin,nextBin);
    // don't include that bin in the integral, but use its lower edge for the bin array
    nextBin++;

    ////debug
    //std::cout << "binsExamined: " << binsExamined << " lastBin: " << lastBin
    //  << " nextBin: " << nextBin
    //  << " lastBinLEdge: " << hist->GetBinLowEdge(lastBin)
    //  << " nextBin LEdge: " << hist->GetBinLowEdge(nextBin)
    //  << " proposedBinContent: " << proposedBinContent << " binsToCombine: " << binsToCombine
    //  << std::endl;
     
    binsExamined=nextBin;

    if(proposedBinContent > 0)
      binArray.push_back(hist->GetBinLowEdge(nextBin));
    else
      binsToCombine+=2;
  }

  //// debug
  //for(unsigned int i=0; i<binArray.size(); ++i)
  //{
  //  std::cout << "initial binArray[" << i << "]: " << binArray[i];
  //  if(i>0)
  //    std::cout << " bin content: "
  //      << hist->Integral(hist->FindFixBin(binArray[i-1]),hist->FindFixBin(binArray[i]))
  //      << std::endl;
  //  else
  //    std::cout << std::endl;
  //}

  while(hist->Integral(hist->FindFixBin(binArray.back()),hist->FindFixBin(hist->GetXaxis()->GetXmax())-1) <= 0)
  {
    binArray.erase(binArray.end()-1);
  }
  if(binArray.back() > hist->GetXaxis()->GetXmax())
    binArray.erase(binArray.end()-1);

  binArray.push_back(hist->GetXaxis()->GetXmax());

  //// debug
  //for(unsigned int i=0; i<binArray.size(); ++i)
  //{
  //  std::cout << "final binArray[" << i << "]: " << binArray[i];
  //  if(i>0)
  //    std::cout << " bin content: "
  //      << hist->Integral(hist->FindFixBin(binArray[i-1]),hist->FindFixBin(binArray[i]))
  //      << std::endl;
  //  else
  //    std::cout << std::endl;
  //}

  return binArray;
}

//// compute variable bin array from fixed bin hist
//std::vector<double> computeVariableBins(TH1* hist)
//{
//  std::vector<double> binArray;
//  binArray.push_back(hist->GetXaxis()->GetXmax());
//
//  // loop over x bins from right to left
//  for(int bin=hist->GetNbinsX(); bin > 0; --bin)
//  {
//    if(hist->GetBinContent(bin) > 0)
//      binArray.push_back(hist->GetBinLowEdge(bin));
//  }
//
//  binArray.erase(binArray.end()-1);
//  binArray.push_back(hist->GetXaxis()->GetXmin());
//
//  std::reverse(binArray.begin(),binArray.end());
//  return binArray;
//}

// ****** main
int main(int argc, char ** argv)
{

  // parse arguments
  if(argc < 2)
  {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return -1;
  }

  using namespace RooFit;

  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  fwlite::InputSource inputHandler_(process);
  fwlite::OutputFiles outputHandler_(process);
  //int maxEvents_(inputHandler_.maxEvents());
  // now get each parameterset
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("makeIasPredictions");
  // mass cut to use for the high-p high-Ih (search region) ias dist
  double massCutIasHighPHighIh_ (ana.getParameter<double>("MassCut"));
  // dE/dx calibration
  double dEdx_k (ana.getParameter<double>("dEdx_k"));
  double dEdx_c (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));

  // fileService initialization
  fwlite::TFileService fs = fwlite::TFileService(outputHandler_.file().c_str());
  TFileDirectory successRateDir = fs.mkdir("successRates");
  TFileDirectory massPredictionFixedBinsDir = fs.mkdir("massPredictionsFixedBins");
  TFileDirectory massPredictionVarBinsDir = fs.mkdir("massPredictionsVariableBins");
  TFileDirectory bRegionDir = fs.mkdir("bRegionHistograms");
  TFileDirectory cRegionDir = fs.mkdir("cRegionHistograms");
  TFileDirectory ihMeanDir = fs.mkdir("ihMeanInIasBins");
  // b region hist
  TH1F* bRegionHist = fs.make<TH1F>("bRegionHist","bRegionHist",100,0,10);
  bRegionHist->SetName("ihLowPsb_B");
  bRegionHist->SetTitle("Ih in low P SB (B region);MeV/cm");
  // c region hist
  TH1F* cRegionHist = fs.make<TH1F>("cRegionHist","cRegionHist",100,0,1000);
  cRegionHist->SetName("pLowIhSB_C");
  cRegionHist->SetTitle("P in low Ih SB (C region);GeV");
  // number of entries in datasets histos
  TH2F* entriesInARegionHist = fs.make<TH2F>("entriesInARegion","Entries in A region (low P, low Ih);#eta;nom",12,0,2.4,9,5,23);
  TH2F* entriesInCRegionHist = fs.make<TH2F>("entriesInCRegion","Entries in C region (P in low Ih SB);#eta;nom",12,0,2.4,9,5,23);
  TH2F* entriesInBRegionHist = fs.make<TH2F>("entriesInBRegion","Entries in B region (Ih in low P SB);#eta;nom",12,0,2.4,9,5,23);
  TH2F* entriesInDRegionHist = fs.make<TH2F>("entriesInDRegion","Entries in D region (search--> high Ih, high P);#eta;nom",12,0,2.4,9,5,23);


  // RooFit observables and dataset
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarPt("rooVarPt","pt",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);
  RooRealVar rooVarMass("rooVarMass","mass",0,2000);
  TFile* inFile = TFile::Open(inputHandler_.files()[0].c_str());
  if(!inFile)
  {
    std::cout << "ERROR: input file " << inputHandler_.files()[0].c_str() << " not found." << std::endl;
    return -2;
  }

  RooDataSet* rooDataSetAll = (RooDataSet*)inFile->Get("rooDataSetAll");
  if(rooDataSetAll->numEntries() < 1)
  {
    std::cout << "Problem with RooDataSet named rooDataSetAll in file " << inputHandler_.files()[0].c_str() << std::endl;
    return -3;
  }

  std::cout << "Doing mass predictions..." << std::endl;
  std::string pSBcutString = "rooVarP<";
  pSBcutString+=floatToString(pSidebandThreshold);
  std::string pSearchCutString = "rooVarP>";
  pSearchCutString+=floatToString(pSidebandThreshold);
  std::string ihSBcutString = "rooVarIh<";
  ihSBcutString+=floatToString(ihSidebandThreshold);
  std::string ihSearchCutString = "rooVarIh>";
  ihSearchCutString+=floatToString(ihSidebandThreshold);
  // A ==> low P, low Ih
  RooDataSet* regionA1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSBcutString.c_str()));
  RooDataSet* regionADataSet = (RooDataSet*)regionA1DataSet->reduce(Cut(ihSBcutString.c_str()));
  // B ==> low P, high Ih
  RooDataSet* regionB1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSBcutString.c_str()));
  RooDataSet* regionBDataSet = (RooDataSet*)regionB1DataSet->reduce(Cut(ihSearchCutString.c_str()));
  // C ==> high P, low Ih
  RooDataSet* regionC1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSearchCutString.c_str()));
  RooDataSet* regionCDataSet = (RooDataSet*)regionC1DataSet->reduce(Cut(ihSBcutString.c_str()));
  // D ==> search region, high P, high Ih
  RooDataSet* regionD1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSearchCutString.c_str()));
  RooDataSet* regionDDataSet = (RooDataSet*)regionD1DataSet->reduce(Cut(ihSearchCutString.c_str()));

  int nomSlice = 0;
  // loop over all nom/eta slices
  //for(int nom=5; nom < 22; nom+=2)
  //XXX TESTING
  for(int nom=9; nom < 10; nom+=2)
  {
    nomSlice++;
    int etaSlice = 0;
    //for(float lowerEta = 0.0; lowerEta < 2.3; lowerEta+=0.2)
    //XXX TESTING
    for(float lowerEta = 0.0; lowerEta < 0.1; lowerEta+=0.2)
    {
      etaSlice++;
      double ihSumsInIasBins[100];
      for(int i=0; i<100; ++i)
      {
        ihSumsInIasBins[i]=0;
      }

      // book histograms -- b region
      std::string bRegionHistName = getHistNameBeg(nom,lowerEta);
      bRegionHistName+="bRegionHist";
      std::string bRegionHistTitle = "Ih for ";
      bRegionHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold);
      bRegionHistTitle+="; MeV/cm";
      TH1F* bRegionHist = bRegionDir.make<TH1F>(bRegionHistName.c_str(),bRegionHistTitle.c_str(),100,0,10);
      //// mass distribution from B region
      //std::string massBRegionHistName = getHistNameBeg(nom,lowerEta);
      //massBRegionHistName+="massBRegionHist";
      //std::string massBRegionHistTitle = "Mass in B region for ";
      //massBRegionHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold);
      //TH1F* massBRegionHist = bRegionDir.make<TH1F>(massBRegionHistName.c_str(),massBRegionHistTitle.c_str(),2000,0,2000);
      // c region hist
      std::string cRegionHistName = getHistNameBeg(nom,lowerEta);
      cRegionHistName+="cRegionHist";
      std::string cRegionHistTitle = "P for ";
      cRegionHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold);
      cRegionHistTitle+="; GeV";
      TH1F* cRegionHist = cRegionDir.make<TH1F>(cRegionHistName.c_str(),cRegionHistTitle.c_str(),100,0,1000);
      // c region hist - cumulative
      std::string cRegionCumuHistName = getHistNameBeg(nom,lowerEta);
      cRegionCumuHistName+="cRegionCumuHist";
      std::string cRegionCumuHistTitle = "P for ";
      cRegionCumuHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold);
      cRegionCumuHistTitle+=" cumulative; GeV";
      TH1F* cRegionCumuHist = cRegionDir.make<TH1F>(cRegionCumuHistName.c_str(),cRegionCumuHistTitle.c_str(),100,0,1000);
      // B region dataset
      std::string nomCutString = "rooVarNoMias==";
      nomCutString+=intToString(nom);
      nomCutString+="||rooVarNoMias==";
      nomCutString+=intToString(nom+1);
      std::string etaCutString = "rooVarEta>";
      etaCutString+=floatToString(lowerEta);
      etaCutString+="&&rooVarEta<";
      etaCutString+=floatToString(lowerEta+0.2);
      RooDataSet* nomCutBRegionDataSet = (RooDataSet*)regionBDataSet->reduce(Cut(nomCutString.c_str()));
      RooDataSet* etaCutNomCutBRegionDataSet = (RooDataSet*)nomCutBRegionDataSet->reduce(Cut(etaCutString.c_str()));
      // C region dataset
      RooDataSet* nomCutCRegionDataSet = (RooDataSet*)regionCDataSet->reduce(Cut(nomCutString.c_str()));
      RooDataSet* etaCutNomCutCRegionDataSet = (RooDataSet*)nomCutCRegionDataSet->reduce(Cut(etaCutString.c_str()));
      // D region dataset
      RooDataSet* nomCutDRegionDataSet = (RooDataSet*)regionDDataSet->reduce(Cut(nomCutString.c_str()));
      RooDataSet* etaCutNomCutDRegionDataSet = (RooDataSet*)nomCutDRegionDataSet->reduce(Cut(etaCutString.c_str()));
      // A region dataset
      RooDataSet* nomCutARegionDataSet = (RooDataSet*)regionADataSet->reduce(Cut(nomCutString.c_str()));
      RooDataSet* etaCutNomCutARegionDataSet = (RooDataSet*)nomCutARegionDataSet->reduce(Cut(etaCutString.c_str()));

      entriesInARegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutARegionDataSet->numEntries());
      entriesInBRegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutBRegionDataSet->numEntries());
      entriesInCRegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutCRegionDataSet->numEntries());
      entriesInDRegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutDRegionDataSet->numEntries());

      // require at least 25 entries in each dataset to do prediction
      if(etaCutNomCutBRegionDataSet->numEntries() < 25 || etaCutNomCutCRegionDataSet->numEntries() < 25)
      {
        std::cout << "WARNING: too few entries-not doing prediction for this slice: eta=" <<
          lowerEta << "-" << lowerEta+0.2 << " nom=" << nom << "-" << nom+1 << std::endl <<
         "\t\t" << etaCutNomCutBRegionDataSet->numEntries() <<
          " entries in B region and " << etaCutNomCutCRegionDataSet->numEntries() <<
          " entries in C region" << std::endl;
        continue;
      }

      const RooArgSet* argSet_B = etaCutNomCutBRegionDataSet->get();
      RooRealVar* ihData_B = (RooRealVar*)argSet_B->find(rooVarIh.GetName());
      RooRealVar* iasData_B = (RooRealVar*)argSet_B->find(rooVarIas.GetName());
      RooRealVar* nomData_B = (RooRealVar*)argSet_B->find(rooVarNoMias.GetName());
      RooRealVar* etaData_B = (RooRealVar*)argSet_B->find(rooVarEta.GetName());
      const RooArgSet* argSet_C = etaCutNomCutCRegionDataSet->get();
      RooRealVar* pData_C = (RooRealVar*)argSet_C->find(rooVarP.GetName());
      RooRealVar* ptData_C = (RooRealVar*)argSet_C->find(rooVarPt.GetName());
      RooRealVar* nomData_C = (RooRealVar*)argSet_C->find(rooVarNoMias.GetName());
      RooRealVar* etaData_C = (RooRealVar*)argSet_C->find(rooVarEta.GetName());
      // fill b region hist
      for(int index=0; index < etaCutNomCutBRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutBRegionDataSet->get(index);
        bRegionHist->Fill(ihData_B->getVal());
        //  massBRegionHist->Fill(sqrt(massSqr));

        if(ihData_B->getVal() < ihSidebandThreshold)
          std::cout << "ERROR: (in B region) ih=" << ihData_B->getVal() << std::endl;
        if(nomData_B->getVal() < nom || nomData_B->getVal() > nom+1)
          std::cout << "ERROR: nom=" << nom << "-" << nom+1 << " and (in B region) data point nom="
            << nomData_B->getVal() << std::endl;
        if(etaData_B->getVal() < lowerEta || etaData_B->getVal() > lowerEta+0.2)
          std::cout << "ERROR: eta=" << lowerEta << "-" << lowerEta+0.2 << " and (in B region) data point eta="
            << etaData_B->getVal() << std::endl;
      }
      // fill c region hist
      for(int index=0; index < etaCutNomCutCRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutCRegionDataSet->get(index);
        cRegionHist->Fill(pData_C->getVal());

        if(pData_C->getVal() < pSidebandThreshold)
          std::cout << "ERROR: (in C region) p=" << pData_C->getVal() << std::endl;
        if(nomData_C->getVal() < nom || nomData_C->getVal() > nom+1)
          std::cout << "ERROR: nom=" << nom << "-" << nom+1 << " and (in C region) data point nom="
            << nomData_C->getVal() << std::endl;
        if(etaData_C->getVal() < lowerEta || etaData_C->getVal() > lowerEta+0.2)
          std::cout << "ERROR: eta=" << lowerEta << "-" << lowerEta+0.2 << " and (in C region) data point eta="
            << etaData_C->getVal() << std::endl;
      }

      // c region cumu hist
      cRegionHist->ComputeIntegral();
      Double_t *cRegionIntegral = cRegionHist->GetIntegral();
      cRegionCumuHist->SetContent(cRegionIntegral);

      // mass prediction histogram in this NoM/eta bin
      std::string massPredictionFixedHistName = getHistNameBeg(nom,lowerEta);
      massPredictionFixedHistName+="massPredictionFixedHist";
      std::string massPredictionFixedHistTitle = "mass prediction for ";
      massPredictionFixedHistTitle+=intToString(nom);
      massPredictionFixedHistTitle+="-";
      massPredictionFixedHistTitle+=intToString(nom+1);
      massPredictionFixedHistTitle+=", ";
      massPredictionFixedHistTitle+=floatToString(lowerEta);
      massPredictionFixedHistTitle+=" < #eta < ";
      massPredictionFixedHistTitle+=floatToString(lowerEta+0.2);
      //massPredictionFixedHistTitle+=", mass > ";
      //massPredictionFixedHistTitle+=floatToString(massCutIasHighPHighIh_);
      //massPredictionFixedHistTitle+=" GeV";
      TH1F* massPredictionFixedHist = massPredictionFixedBinsDir.make<TH1F>(massPredictionFixedHistName.c_str(),massPredictionFixedHistTitle.c_str(),200,0,2000);
      //TProfile* massPredictionFixedHist = massPredictionFixedBinsDir.make<TProfile>(massPredictionFixedHistName.c_str(),massPredictionFixedHistTitle.c_str(),100,0,1);
      //massPredictionFixedHist->SetErrorOption("g");
      massPredictionFixedHist->Sumw2();
      // dataset for masses
      RooDataSet massesDataSet("massesDataSet","massesDataSet",rooVarMass);
      // Ih mean in Ias bins histogram in this NoM/eta bin
      std::string ihMeanProfName = getHistNameBeg(nom,lowerEta);
      ihMeanProfName+="ihMeanProf";
      std::string ihMeanProfTitle = "Ih mean for nom ";
      ihMeanProfTitle+=intToString(nom);
      ihMeanProfTitle+="-";
      ihMeanProfTitle+=intToString(nom+1);
      ihMeanProfTitle+=", ";
      ihMeanProfTitle+=floatToString(lowerEta);
      ihMeanProfTitle+=" < #eta < ";
      ihMeanProfTitle+=floatToString(lowerEta+0.2);
      ihMeanProfTitle+=", mass > ";
      ihMeanProfTitle+=floatToString(massCutIasHighPHighIh_);
      ihMeanProfTitle+=" GeV";
      TProfile* ihMeanProf = ihMeanDir.make<TProfile>(ihMeanProfName.c_str(),ihMeanProfTitle.c_str(),100,0,1);
      std::cout << "INFO doing slice ( " << (nomSlice-1)*12+etaSlice << " / " << "108 ): eta=" <<
        lowerEta << "-" << lowerEta+0.2 << " nom=" << nom << "-" << nom+1 << std::endl;

      TRandom3 myRandom;

      // loop over Ih/Ias
      for(int index=0; index < etaCutNomCutBRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutBRegionDataSet->get(index);
        double thisIh = ihData_B->getVal();
        double thisIas = iasData_B->getVal();

        // get random P value from C SB
        int idx = myRandom.Integer(etaCutNomCutCRegionDataSet->numEntries());
        etaCutNomCutCRegionDataSet->get(idx);
        // loop over P values
        //for(int idx=0; idx < etaCutNomCutCRegionDataSet->numEntries(); ++idx)
        //{
        //  etaCutNomCutCRegionDataSet->get(idx);
          double thisP = pData_C->getVal();
          double massSqr = (thisIh-dEdx_c)*pow(thisP,2)/dEdx_k;
          if(massSqr >= 0)
          {
            massPredictionFixedHist->Fill(sqrt(massSqr));
            rooVarMass = sqrt(massSqr);
            massesDataSet.add(rooVarMass);
            std::cout << "Adding mass: " << rooVarMass.getVal() << std::endl;
          }
        //}
        ihMeanProf->Fill(thisIas,thisIh);
      }

      // rookeyspdf for mass
      std::string massPredictionKeysPdfName = getHistNameBeg(nom,lowerEta);
      massPredictionKeysPdfName+="massPredictionKeysPdf";
      std::string massPredictionKeysPdfTitle = "mass prediction (keys) for nom ";
      massPredictionKeysPdfTitle+=intToString(nom);
      massPredictionKeysPdfTitle+="-";
      massPredictionKeysPdfTitle+=intToString(nom+1);
      massPredictionKeysPdfTitle+=", ";
      massPredictionKeysPdfTitle+=floatToString(lowerEta);
      massPredictionKeysPdfTitle+=" < #eta < ";
      massPredictionKeysPdfTitle+=floatToString(lowerEta+0.2);
      RooKeysPdf keysPdf(massPredictionKeysPdfName.c_str(),massPredictionKeysPdfTitle.c_str(),
          rooVarMass,massesDataSet,RooKeysPdf::MirrorBoth);
      massPredictionKeysPdfName+="Hist";
      TH1F* keysPdfHist = (TH1F*)keysPdf.createHistogram(massPredictionKeysPdfName.c_str(),rooVarMass);
      keysPdfHist->Write();

      // mass prediction histogram in this NoM/eta bin -- variable
      std::string massPredictionVarBinHistName = getHistNameBeg(nom,lowerEta);
      massPredictionVarBinHistName+="massPredictionVarBinHist";
      std::string massPredictionVarBinHistTitle = "mass prediction for nom ";
      massPredictionVarBinHistTitle+=intToString(nom);
      massPredictionVarBinHistTitle+="-";
      massPredictionVarBinHistTitle+=intToString(nom+1);
      massPredictionVarBinHistTitle+=", ";
      massPredictionVarBinHistTitle+=floatToString(lowerEta);
      massPredictionVarBinHistTitle+=" < #eta < ";
      massPredictionVarBinHistTitle+=floatToString(lowerEta+0.2);
      //massPredictionVarBinHistTitle+=", mass > ";
      //massPredictionVarBinHistTitle+=floatToString(massCutIasHighPHighIh_);
      //massPredictionVarBinHistTitle+=" GeV";
      // figure out variable bins
      std::vector<double> binVec = computeVariableBins(massPredictionFixedHist);
      TH1F* massPredictionVarBinHist = massPredictionVarBinsDir.make<TH1F>(massPredictionVarBinHistName.c_str(),
          massPredictionVarBinHistTitle.c_str(),binVec.size()-1,&(*binVec.begin()));
      massPredictionVarBinHist->Sumw2();

      // get bin contents/errors
      const int numVarBins = massPredictionVarBinHist->GetNbinsX();
      std::vector<double> binContents;
      std::vector<double> binErrors;
      for(int i=0; i<numVarBins; ++i)
      {
        binContents.push_back(0);
        binErrors.push_back(0);
      }
      for(int bin=1; bin<=massPredictionFixedHist->GetNbinsX();++bin)
      {
        double binc = massPredictionFixedHist->GetBinContent(bin);
        double bine = massPredictionFixedHist->GetBinError(bin);
        if(binc > 0)
        {
          binContents[massPredictionVarBinHist->FindBin(massPredictionFixedHist->GetBinCenter(bin))-1] += binc;
          binErrors[massPredictionVarBinHist->FindBin(massPredictionFixedHist->GetBinCenter(bin))-1] += (bine*bine);
        }
      }
      // fill it
      for(int i=1; i<numVarBins+1; ++i)
      {
        if(binContents[i-1] > 0)
        {
          // make bins contain number of events per horizontal unit
          double binWidthRatio = massPredictionVarBinHist->GetBinWidth(1)/massPredictionVarBinHist->GetBinWidth(i);
          massPredictionVarBinHist->SetBinContent(i,binWidthRatio*binContents[i-1]);
          massPredictionVarBinHist->SetBinError(i,binWidthRatio*sqrt(binErrors[i-1]));
        }
      }

      // check to make sure there are no empty bins in the prediction hist
      for(int bin=1; bin <= massPredictionVarBinHist->GetNbinsX(); ++bin)
      {
        if(massPredictionVarBinHist->GetBinContent(bin) <= 0)
        {
          std::cout << "ERROR: For this hist: " << massPredictionVarBinHist->GetName()
            << " bin content for bin "
             << bin << " is " << massPredictionVarBinHist->GetBinContent(bin)
             << " with binLowEdge= " << massPredictionVarBinHist->GetBinLowEdge(bin)
             << " to binHighEdge="
             << massPredictionVarBinHist->GetBinLowEdge(bin)+massPredictionVarBinHist->GetBinWidth(bin)
             << ". Fix the binning.  Bailing out." << std::endl;
          return -9;
        }
      }

      delete nomCutBRegionDataSet;
      delete nomCutCRegionDataSet;
      delete nomCutDRegionDataSet;
      delete etaCutNomCutBRegionDataSet;
      delete etaCutNomCutCRegionDataSet;
      delete etaCutNomCutDRegionDataSet;

    } // eta slices
  } // nom slices

}
