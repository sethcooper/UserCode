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

// compute variable bin array from fixed bin hist
std::vector<double> computeVariableBins(TH1F* hist)
{
  std::vector<double> binArray;
  binArray.push_back(hist->GetXaxis()->GetXmax());

  // loop over x bins from right to left
  for(int bin=hist->GetNbinsX(); bin > 0; --bin)
  {
    if(hist->GetBinContent(bin) > 0)
      binArray.push_back(hist->GetBinLowEdge(bin));
  }

  binArray.erase(binArray.end()-1);
  binArray.push_back(hist->GetXaxis()->GetXmin());

  std::reverse(binArray.begin(),binArray.end());
  return binArray;
}

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
  TFileDirectory iasPredictionFixedBinsDir = fs.mkdir("iasPredictionsFixedBins");
  TFileDirectory iasPredictionVarBinsDir = fs.mkdir("iasPredictionsVariableBins");
  TFileDirectory bRegionDir = fs.mkdir("bRegionHistograms");
  TFileDirectory cRegionDir = fs.mkdir("cRegionHistograms");
  // b region hist
  TH1F* bRegionHist = fs.make<TH1F>("bRegionHist","bRegionHist",100,0,10);
  bRegionHist->SetName("ihLowPsb_B");
  bRegionHist->SetTitle("Ih in low P SB (B region);MeV/cm");
  // c region hist
  TH1F* cRegionHist = fs.make<TH1F>("cRegionHist","cRegionHist",100,0,1000);
  cRegionHist->SetName("pLowIhSB_C");
  cRegionHist->SetTitle("P in low Ih SB (C region);GeV");
  // number of entries in datasets histos
  TH2F* entriesInCRegionHist = fs.make<TH2F>("entriesInCRegion","Entries in C region (P in low Ih SB);#eta;nom",12,1,13,9,1,10);
  setBinLabelsEntriesHist(entriesInCRegionHist);
  TH2F* entriesInBRegionHist = fs.make<TH2F>("entriesInBRegion","Entries in B region (Ih in low P SB);#eta;nom",12,1,13,9,1,10);
  setBinLabelsEntriesHist(entriesInBRegionHist);


  // RooFit observables and dataset
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);
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

  std::cout << "Doing ias predictions..." << std::endl;
  std::string pSBcutString = "rooVarP<";
  pSBcutString+=floatToString(pSidebandThreshold);
  std::string pSearchCutString = "rooVarP>";
  pSearchCutString+=floatToString(pSidebandThreshold);
  std::string ihSBcutString = "rooVarIh<";
  ihSBcutString+=floatToString(ihSidebandThreshold);
  std::string ihSearchCutString = "rooVarIh>";
  ihSearchCutString+=floatToString(ihSidebandThreshold);
  // B ==> low P, high Ih
  RooDataSet* regionB1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSBcutString.c_str()));
  RooDataSet* regionBDataSet = (RooDataSet*)regionB1DataSet->reduce(Cut(ihSearchCutString.c_str()));
  // C ==> high P, low Ih
  RooDataSet* regionC1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSearchCutString.c_str()));
  RooDataSet* regionCDataSet = (RooDataSet*)regionC1DataSet->reduce(Cut(ihSBcutString.c_str()));

  int nomSlice = 1;
  // loop over all nom/eta slices
  for(int nom=5; nom < 22; nom+=2)
  {
    int etaSlice = 1;
    for(float lowerEta = 0.0; lowerEta < 2.3; lowerEta+=0.2)
    {
      // book histograms -- b region
      std::string bRegionHistName = "nom";
      bRegionHistName+=intToString(nom);
      bRegionHistName+="to";
      bRegionHistName+=intToString(nom+1);
      bRegionHistName+="eta";
      bRegionHistName+=intToString(lowerEta*10);
      bRegionHistName+="to";
      bRegionHistName+=intToString((lowerEta+0.2)*10);
      bRegionHistName+="bRegionHist";
      std::string bRegionHistTitle = "Ih for nom ";
      bRegionHistTitle+=intToString(nom);
      bRegionHistTitle+="-";
      bRegionHistTitle+=intToString(nom+1);
      bRegionHistTitle+=", ";
      bRegionHistTitle+=floatToString(lowerEta);
      bRegionHistTitle+=" < #eta < ";
      bRegionHistTitle+=floatToString(lowerEta+0.2);
      bRegionHistTitle+=", (p < ";
      bRegionHistTitle+=floatToString(pSidebandThreshold);
      bRegionHistTitle+=", Ih > ";
      bRegionHistTitle+=floatToString(ihSidebandThreshold);
      bRegionHistTitle+="); MeV/cm";
      TH1F* bRegionHist = bRegionDir.make<TH1F>(bRegionHistName.c_str(),bRegionHistTitle.c_str(),100,0,10);
      // c region hist
      std::string cRegionHistName = "nom";
      cRegionHistName+=intToString(nom);
      cRegionHistName+="to";
      cRegionHistName+=intToString(nom+1);
      cRegionHistName+="eta";
      cRegionHistName+=intToString(lowerEta*10);
      cRegionHistName+="to";
      cRegionHistName+=intToString((lowerEta+0.2)*10);
      cRegionHistName+="cRegionHist";
      std::string cRegionHistTitle = "P for nom ";
      cRegionHistTitle+=intToString(nom);
      cRegionHistTitle+="-";
      cRegionHistTitle+=intToString(nom+1);
      cRegionHistTitle+=", ";
      cRegionHistTitle+=floatToString(lowerEta);
      cRegionHistTitle+=" < #eta < ";
      cRegionHistTitle+=floatToString(lowerEta+0.2);
      cRegionHistTitle+=", (Ih < ";
      cRegionHistTitle+=floatToString(ihSidebandThreshold);
      cRegionHistTitle+=", p > ";
      cRegionHistTitle+=floatToString(pSidebandThreshold);
      cRegionHistTitle+="); GeV";
      TH1F* cRegionHist = cRegionDir.make<TH1F>(cRegionHistName.c_str(),cRegionHistTitle.c_str(),100,0,1000);
      // B region dataset
      std::string nomCutString = "rooVarNoMias==";
      nomCutString+=intToString(nom);
      nomCutString+="||rooVarNoMias==";
      nomCutString+=intToString(nom+1);
      RooDataSet* nomCutBRegionDataSet = (RooDataSet*)regionBDataSet->reduce(Cut(nomCutString.c_str()));
      std::string etaCutString = "rooVarEta>";
      etaCutString+=floatToString(lowerEta);
      etaCutString+="&&rooVarEta<";
      etaCutString+=floatToString(lowerEta+0.2);
      RooDataSet* etaCutNomCutBRegionDataSet = (RooDataSet*)nomCutBRegionDataSet->reduce(Cut(etaCutString.c_str()));
      // C region dataset
      RooDataSet* nomCutCRegionDataSet = (RooDataSet*)regionCDataSet->reduce(Cut(nomCutString.c_str()));
      RooDataSet* etaCutNomCutCRegionDataSet = (RooDataSet*)nomCutCRegionDataSet->reduce(Cut(etaCutString.c_str()));

      entriesInBRegionHist->Fill(etaSlice,nomSlice,etaCutNomCutBRegionDataSet->numEntries());
      entriesInCRegionHist->Fill(etaSlice,nomSlice,etaCutNomCutCRegionDataSet->numEntries());

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
      RooRealVar* nomData_C = (RooRealVar*)argSet_C->find(rooVarNoMias.GetName());
      RooRealVar* etaData_C = (RooRealVar*)argSet_C->find(rooVarEta.GetName());
      // fill b region hist
      for(int index=0; index < etaCutNomCutBRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutBRegionDataSet->get(index);
        bRegionHist->Fill(ihData_B->getVal());

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

      // ias prediction histogram in this NoM/eta bin
      std::string iasPredictionFixedHistName = "nom";
      iasPredictionFixedHistName+=intToString(nom);
      iasPredictionFixedHistName+="to";
      iasPredictionFixedHistName+=intToString(nom+1);
      iasPredictionFixedHistName+="eta";
      iasPredictionFixedHistName+=intToString(lowerEta*10);
      iasPredictionFixedHistName+="to";
      iasPredictionFixedHistName+=intToString((lowerEta+0.2)*10);
      iasPredictionFixedHistName+="iasPredictionFixedHist";
      std::string iasPredictionFixedHistTitle = "Ias prediction for nom ";
      iasPredictionFixedHistTitle+=intToString(nom);
      iasPredictionFixedHistTitle+="-";
      iasPredictionFixedHistTitle+=intToString(nom+1);
      iasPredictionFixedHistTitle+=", ";
      iasPredictionFixedHistTitle+=floatToString(lowerEta);
      iasPredictionFixedHistTitle+=" < #eta < ";
      iasPredictionFixedHistTitle+=floatToString(lowerEta+0.2);
      iasPredictionFixedHistTitle+=", mass > ";
      iasPredictionFixedHistTitle+=floatToString(massCutIasHighPHighIh_);
      iasPredictionFixedHistTitle+=" GeV";
      TH1F* iasPredictionFixedHist = iasPredictionFixedBinsDir.make<TH1F>(iasPredictionFixedHistName.c_str(),iasPredictionFixedHistTitle.c_str(),100,0,1);
      iasPredictionFixedHist->Sumw2();

      TRandom3 tRandom;
      for(int trial=0; trial<etaCutNomCutCRegionDataSet->numEntries(); trial++)
      {
        int entryIndexB = (int)etaCutNomCutBRegionDataSet->numEntries()*tRandom.Rndm();
        etaCutNomCutBRegionDataSet->get(entryIndexB);
        double ih = ihData_B->getVal();
        double ias = iasData_B->getVal();

        int entryIndexC = (int)etaCutNomCutCRegionDataSet->numEntries()*tRandom.Rndm();
        etaCutNomCutCRegionDataSet->get(entryIndexC);
        double p = pData_C->getVal();

        double massSqr = (ih-dEdx_c)*pow(p,2)/dEdx_k;
        if(sqrt(massSqr) >= massCutIasHighPHighIh_)
        {
          iasPredictionFixedHist->Fill(ias);
        }
      }

      // figure out variable bins
      std::vector<double> binVec = computeVariableBins(iasPredictionFixedHist);
      // ias prediction histogram in this NoM/eta bin -- variable
      std::string iasPredictionVarBinHistName = "nom";
      iasPredictionVarBinHistName+=intToString(nom);
      iasPredictionVarBinHistName+="to";
      iasPredictionVarBinHistName+=intToString(nom+1);
      iasPredictionVarBinHistName+="eta";
      iasPredictionVarBinHistName+=intToString(lowerEta*10);
      iasPredictionVarBinHistName+="to";
      iasPredictionVarBinHistName+=intToString((lowerEta+0.2)*10);
      iasPredictionVarBinHistName+="iasPredictionVarBinHist";
      std::string iasPredictionVarBinHistTitle = "Ias prediction for nom ";
      iasPredictionVarBinHistTitle+=intToString(nom);
      iasPredictionVarBinHistTitle+="-";
      iasPredictionVarBinHistTitle+=intToString(nom+1);
      iasPredictionVarBinHistTitle+=", ";
      iasPredictionVarBinHistTitle+=floatToString(lowerEta);
      iasPredictionVarBinHistTitle+=" < #eta < ";
      iasPredictionVarBinHistTitle+=floatToString(lowerEta+0.2);
      iasPredictionVarBinHistTitle+=", mass > ";
      iasPredictionVarBinHistTitle+=floatToString(massCutIasHighPHighIh_);
      iasPredictionVarBinHistTitle+=" GeV";
      TH1F* iasPredictionVarBinHist = iasPredictionVarBinsDir.make<TH1F>(iasPredictionVarBinHistName.c_str(),iasPredictionVarBinHistTitle.c_str(),binVec.size()-1,&(*binVec.begin()));
      //iasPredictionVarBinHist->Sumw2();
      // fill it
      for(int bin=1; bin<=iasPredictionFixedHist->GetNbinsX();++bin)
      {
        double binc = iasPredictionFixedHist->GetBinContent(bin);
        if(binc > 0)
        {
          iasPredictionVarBinHist->Fill(iasPredictionFixedHist->GetBinCenter(bin),binc);
        }
      }

      ++etaSlice;
    }
    ++nomSlice;
  }

}
