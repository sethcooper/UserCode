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


std::string getHistTitleBeg(int lowerNom, float lowerEta, float pSB, float ihSB, float ptSB, float iasSB,
    bool usePt, bool useIas)
{
  std::string histTitle = "NoM ";
  histTitle+=intToString(lowerNom);
  histTitle+="-";
  if(lowerNom==21)
    histTitle+="end";
  else
    histTitle+=intToString(lowerNom+1);
  histTitle+=", ";
  histTitle+=floatToString(lowerEta);
  histTitle+=" < #eta < ";
  histTitle+=floatToString(lowerEta+0.2);
  if(usePt)
  {
    histTitle+=", (pt < ";
    histTitle+=floatToString(ptSB);
  }
  else
  {
    histTitle+=", (p < ";
    histTitle+=floatToString(pSB);
  }
  if(useIas)
  {
    histTitle+=", Ias > ";
    histTitle+=floatToString(iasSB);
  }
  else
  {
    histTitle+=", Ih > ";
    histTitle+=floatToString(ihSB);
  }
  histTitle+=")";
  return histTitle;
}


// add the mass, p/pt, and ih SB thresholds to the output filename automatically
std::string generateFileNameEnd(double massCut, double pSideband, double ptSideband, bool usePt, double ihSideband)
{
  std::string fileNameEnd = ".massCut";
  fileNameEnd+=intToString(((int)massCut));
  if(usePt)
  {
    fileNameEnd+=".pt";
    fileNameEnd+=intToString(((int)ptSideband));
  }
  else
  {
    fileNameEnd+=".p";
    fileNameEnd+=intToString(((int)pSideband));
  }
  fileNameEnd+=".ih";
  // replace "." with "p" for the ih
  std::string ihSBStr = floatToString(ihSideband);
  ihSBStr.replace(ihSBStr.find("."),1,"p");
  fileNameEnd+=ihSBStr;

  fileNameEnd+=".root";
  return fileNameEnd;
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
    int lastBin = hist->FindBin(binArray.back());
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
  //  std::cout << "initial binArray[" << i << "]: " << binArray[i] << std::endl;

  while(hist->Integral(hist->FindBin(binArray.back()),hist->FindBin(hist->GetXaxis()->GetXmax())) <= 0
      && binArray.size() > 1)
  {
    binArray.erase(binArray.end()-1);
  }

  binArray.push_back(hist->GetXaxis()->GetXmax());

  //// debug
  //for(unsigned int i=0; i<binArray.size(); ++i)
  //  std::cout << "final binArray[" << i << "]: " << binArray[i] << std::endl;

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
  double ptSidebandThreshold (ana.getParameter<double>("PtSidebandThreshold"));
  bool usePtForSideband (ana.getParameter<bool>("UsePtForSideband"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  double iasSidebandThreshold (ana.getParameter<double>("IasSidebandThreshold"));
  bool useIasForSideband (ana.getParameter<bool>("UseIasForSideband"));
  double etaMin_ (ana.getParameter<double>("EtaMin"));
  double etaMax_ (ana.getParameter<double>("EtaMax"));
  int nomMin_ (ana.getParameter<int>("NoMMin"));
  int nomMax_ (ana.getParameter<int>("NoMMax"));

  // fileService initialization
  //string fileNameEnd = generateFileNameEnd(massCutIasHighPHighIh_,pSidebandThreshold,ptSidebandThreshold,usePtForSideband,ihSidebandThreshold);
  string fileNameEnd = "";
  fwlite::TFileService fs = fwlite::TFileService((outputHandler_.file()+fileNameEnd).c_str());
  TFileDirectory successRateDir = fs.mkdir("successRates");
  TFileDirectory iasPredictionFixedBinsDir = fs.mkdir("iasPredictionsFixedBins");
  TFileDirectory iasPredictionVarBinsDir = fs.mkdir("iasPredictionsVariableBins");
  TFileDirectory aRegionDir = fs.mkdir("aRegionHistograms");
  TFileDirectory bRegionDir = fs.mkdir("bRegionHistograms");
  TFileDirectory cRegionDir = fs.mkdir("cRegionHistograms");
  TFileDirectory ihMeanDir = fs.mkdir("ihMeanInIasBins");
  TFileDirectory minPCutMeanDir = fs.mkdir("minPCutInIasBins");
  TFileDirectory cRegionTracksOverMassCutDir = fs.mkdir("cRegionTracksOverIasCutInIasBins");
  // b region hist
  //TH1F* bRegionHist = fs.make<TH1F>("bRegionHist","bRegionHist",100,0,10);
  //bRegionHist->SetName("ihLowPsb_B");
  //bRegionHist->SetTitle("Ih in low P SB (B region);MeV/cm");
  // c region hist
  //TH1F* cRegionHist = fs.make<TH1F>("cRegionHist","cRegionHist",100,0,1000);
  //cRegionHist->SetName("pLowIhSB_C");
  //cRegionHist->SetTitle("P in low Ih SB (C region);GeV");
  // number of entries in datasets histos
  TH2F*  entriesInARegionHist = fs.make<TH2F>("entriesInARegion","Entries in A region (low P, low Ih);#eta;nom",12,0,2.4,9,5,23);
  entriesInARegionHist->GetYaxis()->SetNdivisions(509,false);
  TH2F*  entriesInBRegionHist = fs.make<TH2F>("entriesInBRegion","Entries in B region (Ih in low P SB);#eta;nom",12,0,2.4,9,5,23);
  entriesInBRegionHist->GetYaxis()->SetNdivisions(509,false);
  TH2F*  entriesInCRegionHist = fs.make<TH2F>("entriesInCRegion","Entries in C region (P in low Ih SB);#eta;nom",12,0,2.4,9,5,23);
  entriesInCRegionHist->GetYaxis()->SetNdivisions(509,false);
  TH2F*  entriesInDRegionHist = fs.make<TH2F>("entriesInDRegion","Entries in D region (search--> high Ih, high P);#eta;nom",12,0,2.4,9,5,23);
  entriesInDRegionHist->GetYaxis()->SetNdivisions(509,false);
  // ias total distribution from B region
  std::string iasBRegionTotalHistName ="iasBRegionTotalHist";
  std::string iasBRegionTotalHistTitle = "Ias in B region";
  TH1F* iasBRegionTotalHist = fs.make<TH1F>(iasBRegionTotalHistName.c_str(),iasBRegionTotalHistTitle.c_str(),100,0,1);
  // ih total distribution from B region
  std::string ihBRegionTotalHistName ="ihBRegionTotalHist";
  std::string ihBRegionTotalHistTitle = "Ih in B region";
  TH1F* ihBRegionTotalHist = fs.make<TH1F>(ihBRegionTotalHistName.c_str(),ihBRegionTotalHistTitle.c_str(),200,3,15);
  // eta distribution from B region
  std::string etaBRegionHistName ="etaBRegionHist";
  std::string etaBRegionHistTitle = "Eta in B region";
  TH1F* etaBRegionHist = fs.make<TH1F>(etaBRegionHistName.c_str(),etaBRegionHistTitle.c_str(),50,-3,3);
  // eta distribution from A region
  std::string etaARegionHistName ="etaARegionHist";
  std::string etaARegionHistTitle = "Eta in A region";
  TH1F* etaARegionHist = fs.make<TH1F>(etaARegionHistName.c_str(),etaARegionHistTitle.c_str(),50,-3,3);
  // c region Pt vs. eta
  std::string pEtaCRegionHistName = "pEtaCRegionHist";
  std::string pEtaCRegionHistTitle = "P vs. eta in C Region";
  TH2F* pEtaCRegionHist = fs.make<TH2F>(pEtaCRegionHistName.c_str(),pEtaCRegionHistTitle.c_str(),50,-3,3,200,45,1200);

  //TODO move to common file
  // RooFit observables and dataset
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarPt("rooVarPt","pt",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",-2.5,2.5);
  TFile* inFile = TFile::Open(inputHandler_.files()[0].c_str());
  if(!inFile)
  {
    std::cout << "ERROR: input file " << inputHandler_.files()[0].c_str() << " not found." << std::endl;
    return -2;
  }

  RooDataSet* rooDataSetInput = (RooDataSet*)inFile->Get("rooDataSetCandidates");
  //RooDataSet* rooDataSetInput = (RooDataSet*)inFile->Get("rooDataSetOneCandidatePerEvent");
  if(rooDataSetInput->numEntries() < 1)
  {
    std::cout << "Problem with RooDataSet named rooDataSetCandidates in file " << inputHandler_.files()[0].c_str() << std::endl;
    return -3;
  }

  std::cout << "Doing ias predictions..." << std::endl;
  std::string pSBcutString = "rooVarP<";
  pSBcutString+=floatToString(pSidebandThreshold);
  std::string pSearchCutString = "rooVarP>";
  pSearchCutString+=floatToString(pSidebandThreshold);
  std::string ptSBcutString = "rooVarPt<";
  ptSBcutString+=floatToString(ptSidebandThreshold);
  std::string ptSearchCutString = "rooVarPt>";
  ptSearchCutString+=floatToString(ptSidebandThreshold);
  std::string ihSBcutString = "rooVarIh<";
  ihSBcutString+=floatToString(ihSidebandThreshold);
  std::string ihSearchCutString = "rooVarIh>";
  ihSearchCutString+=floatToString(ihSidebandThreshold);
  std::string iasSBcutString = "rooVarIas<";
  iasSBcutString+=floatToString(iasSidebandThreshold);
  std::string iasSearchCutString = "rooVarIas>";
  iasSearchCutString+=floatToString(iasSidebandThreshold);
  //XXX TESTING CUT OFF PT < 50
  //RooDataSet* rooDataSetAll = (RooDataSet*) rooDataSetInput->reduce(Cut("rooVarPt>50"));
  //RooDataSet* rooDataSetAll = (RooDataSet*) rooDataSetInput->Clone();
  //XXX TURN OFF ETA > 1.5
  RooDataSet* rooDataSetAll = (RooDataSet*) rooDataSetInput->reduce(Cut("rooVarEta<1.5&&rooVarEta>-1.5"));
  std::cout << "INFO: Applying upper eta cut of 1.5" << std::endl;
  delete rooDataSetInput;
  // A ==> low P/Pt, low Ih
  RooDataSet* regionA1DataSet;
  RooDataSet* regionADataSet;
  if(usePtForSideband)
    regionA1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(ptSBcutString.c_str()));
  else
    regionA1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSBcutString.c_str()));
  if(useIasForSideband)
    regionADataSet = (RooDataSet*)regionA1DataSet->reduce(Cut(iasSBcutString.c_str()),
        SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarEta,rooVarIh,rooVarNoMias)));
  else
    regionADataSet = (RooDataSet*)regionA1DataSet->reduce(Cut(ihSBcutString.c_str()),
        SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarEta,rooVarIh,rooVarNoMias)));
  delete regionA1DataSet;
  // B ==> low P/Pt, high Ih
  RooDataSet* regionB1DataSet;
  RooDataSet* regionBDataSet;
  if(usePtForSideband)
    regionB1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(ptSBcutString.c_str()));
  else
    regionB1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSBcutString.c_str()));
  if(useIasForSideband)
    regionBDataSet = (RooDataSet*)regionB1DataSet->reduce(Cut(iasSearchCutString.c_str()),
          SelectVars(RooArgSet(rooVarPt,rooVarIh,rooVarIas,rooVarEta,rooVarNoMias)));
  else
    regionBDataSet = (RooDataSet*)regionB1DataSet->reduce(Cut(ihSearchCutString.c_str()),
          SelectVars(RooArgSet(rooVarPt,rooVarIh,rooVarIas,rooVarEta,rooVarNoMias)));
  delete regionB1DataSet;
  // C ==> high P/Pt, low Ih
  RooDataSet* regionC1DataSet;
  RooDataSet* regionCDataSet;
  if(usePtForSideband)
   regionC1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(ptSearchCutString.c_str()));
  else
   regionC1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSearchCutString.c_str()));
  //SIC FEB 10 -- cut off p > 1200
  //RooDataSet* pCutCDataSet = (RooDataSet*)regionC1DataSet->reduce(Cut("rooVarP<1200"));
  //std::cout << "INFO: Applying max p of 1200 GeV cut in C region." << std::endl;
  std::cout << "INFO: NOT applying max p of 1200 GeV cut in C region." << std::endl;
  if(useIasForSideband)
    regionCDataSet = (RooDataSet*)regionC1DataSet->reduce(Cut(iasSBcutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIh)));
    //regionCDataSet = (RooDataSet*)pCutCDataSet->reduce(Cut(iasSBcutString.c_str())
          //SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIh)));
  else
    regionCDataSet = (RooDataSet*)regionC1DataSet->reduce(Cut(ihSBcutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIh)));
    //regionCDataSet = (RooDataSet*)pCutCDataSet->reduce(Cut(ihSBcutString.c_str())
          //SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIh)));
  delete regionC1DataSet;
  std::cout << "INFO: " << regionCDataSet->numEntries() << " entries in total C region." << std::endl;
  // D ==> search region, high P/Pt, high Ih
  RooDataSet* regionD1DataSet;
  RooDataSet* regionDDataSet;
  if(usePtForSideband)
    regionD1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(ptSearchCutString.c_str()));
  else
    regionD1DataSet = (RooDataSet*)rooDataSetAll->reduce(Cut(pSearchCutString.c_str()));
  if(useIasForSideband)
    regionDDataSet = (RooDataSet*)regionD1DataSet->reduce(Cut(iasSearchCutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarEta,rooVarNoMias)));
  else
    regionDDataSet = (RooDataSet*)regionD1DataSet->reduce(Cut(ihSearchCutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarEta,rooVarNoMias)));
  delete regionD1DataSet;
  delete rooDataSetAll;

  int slicesToDo = (int)((etaMax_-etaMin_)/0.2)*(nomMax_-nomMin_+1)/2;
  int nomSlice = 0;
  // loop over given nom/eta slices
  for(int nom = nomMin_; nom < nomMax_; nom+=2)
  {
    nomSlice++;
    int etaSlice = 0;
    for(float lowerEta = etaMin_; lowerEta < etaMax_; lowerEta+=0.2)
    {
      etaSlice++;
      double successRateSumsInIasBins[100];
      double successRateCountsInIasBins[100];
      double successRateLastErrorInIasBins[100];
      double ihSumsInIasBins[100];
      for(int i=0; i<100; ++i)
      {
        successRateLastErrorInIasBins[i]=0;
        successRateCountsInIasBins[i]=0;
        successRateSumsInIasBins[i]=0;
        ihSumsInIasBins[i]=0;
      }

      // book histograms -- b region
      std::string bRegionHistName = getHistNameBeg(nom,lowerEta);
      bRegionHistName+="bRegionHist";
      std::string bRegionHistTitle = "Ih for ";
      bRegionHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold,ptSidebandThreshold,
          iasSidebandThreshold,usePtForSideband,useIasForSideband);
      bRegionHistTitle+="; MeV/cm";
      TH1F* bRegionHist = bRegionDir.make<TH1F>(bRegionHistName.c_str(),bRegionHistTitle.c_str(),100,0,10);
      // ias distribution from B region
      std::string iasBRegionHistName = getHistNameBeg(nom,lowerEta);
      iasBRegionHistName+="iasBRegionHist";
      std::string iasBRegionHistTitle = "Ias in B region for ";
      iasBRegionHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold,ptSidebandThreshold,
          iasSidebandThreshold,usePtForSideband,useIasForSideband);
      TH1F* iasBRegionHist = bRegionDir.make<TH1F>(iasBRegionHistName.c_str(),iasBRegionHistTitle.c_str(),100,0,1);
      // c region hist
      std::string cRegionHistName = getHistNameBeg(nom,lowerEta);
      cRegionHistName+="cRegionHist";
      std::string cRegionHistTitle = "P for ";
      cRegionHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold,ptSidebandThreshold,
          iasSidebandThreshold,usePtForSideband,useIasForSideband);
      cRegionHistTitle+="; GeV";
      TH1F* cRegionHist = cRegionDir.make<TH1F>(cRegionHistName.c_str(),cRegionHistTitle.c_str(),100,0,1000);
      // c region hist - cumulative
      std::string cRegionCumuHistName = getHistNameBeg(nom,lowerEta);
      cRegionCumuHistName+="cRegionCumuHist";
      std::string cRegionCumuHistTitle = "P for ";
      cRegionCumuHistTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold,ptSidebandThreshold,
          iasSidebandThreshold,usePtForSideband,useIasForSideband);
      cRegionCumuHistTitle+=" cumulative; GeV";
      TH1F* cRegionCumuHist = cRegionDir.make<TH1F>(cRegionCumuHistName.c_str(),cRegionCumuHistTitle.c_str(),100,0,1000);
      // c region over mass cut in ias bins hist
      std::string cRegionTracksOverMassCutProfileName = getHistNameBeg(nom,lowerEta);
      cRegionTracksOverMassCutProfileName+="tracksInCOverMassCutProfile";
      std::string cRegionTracksOverMassCutProfileTitle = "Tracks in C Over Mass Cut in Ias bins for ";
      cRegionTracksOverMassCutProfileTitle+=getHistTitleBeg(nom,lowerEta,pSidebandThreshold,ihSidebandThreshold,ptSidebandThreshold,
          iasSidebandThreshold,usePtForSideband,useIasForSideband);
      cRegionTracksOverMassCutProfileTitle+="; Ias";
      TProfile* cRegionTracksOverMassCutProfile =
        cRegionTracksOverMassCutDir.make<TProfile>(cRegionTracksOverMassCutProfileName.c_str(),
            cRegionTracksOverMassCutProfileTitle.c_str(),100,0,1);

      // B region dataset
      std::string nomCutString = "rooVarNoMias==";
      nomCutString+=intToString(nom);
      nomCutString+="||rooVarNoMias==";
      nomCutString+=intToString(nom+1);
      if(nom==21) // do nom 21+ in one slice
        nomCutString = "rooVarNoMias>=21";
      std::string etaCutString = "(rooVarEta>";
      etaCutString+=floatToString(lowerEta);
      etaCutString+="&&rooVarEta<";
      std::string upperEtaLimit;
      ////XXX TESTING eta 1.5 max
      //cout << "lowerEta = " << lowerEta << " 10*lowerEta = " << ((int)10*lowerEta) << " 10*lowerEta==14? " <<
      //  (((int)10*lowerEta)==14) << endl;
      //if(10*lowerEta==14)
      //  upperEtaLimit=floatToString(1.5);
      //else
      //  upperEtaLimit=floatToString(lowerEta+0.2);
      upperEtaLimit=floatToString(lowerEta+0.2);
      etaCutString+=upperEtaLimit;
      etaCutString+=")||(rooVarEta>-";
      etaCutString+=upperEtaLimit;
      etaCutString+="&&rooVarEta<-";
      etaCutString+=floatToString(lowerEta);
      etaCutString+=")";
      RooDataSet* nomCutBRegionDataSet = (RooDataSet*)regionBDataSet->reduce(Cut(nomCutString.c_str()),
          SelectVars(RooArgSet(rooVarPt,rooVarIh,rooVarIas,rooVarEta)));
      RooDataSet* etaCutNomCutBRegionDataSet = (RooDataSet*)nomCutBRegionDataSet->reduce(Cut(etaCutString.c_str()));
      // C region dataset
      // SIC Dec. 23: study shows we can use all NoM slices
      //RooDataSet* nomCutCRegionDataSet = (RooDataSet*)regionCDataSet->reduce(Cut(nomCutString.c_str()));
      RooDataSet* etaCutCRegionDataSet = (RooDataSet*)regionCDataSet->reduce(Cut(etaCutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarNoMias,rooVarEta,rooVarIh)));
      RooDataSet* etaCutNomCutCRegionDataSet = (RooDataSet*)etaCutCRegionDataSet->reduce(Cut(nomCutString.c_str()));
      // D region dataset
      RooDataSet* nomCutDRegionDataSet = (RooDataSet*)regionDDataSet->reduce(Cut(nomCutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarEta)));
      RooDataSet* etaCutNomCutDRegionDataSet = (RooDataSet*)nomCutDRegionDataSet->reduce(Cut(etaCutString.c_str()));
      // A region dataset
      RooDataSet* nomCutARegionDataSet = (RooDataSet*)regionADataSet->reduce(Cut(nomCutString.c_str()),
          SelectVars(RooArgSet(rooVarP,rooVarPt,rooVarEta,rooVarIh)));
      RooDataSet* etaCutNomCutARegionDataSet = (RooDataSet*)nomCutARegionDataSet->reduce(Cut(etaCutString.c_str()));

      entriesInARegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutARegionDataSet->numEntries());
      entriesInBRegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutBRegionDataSet->numEntries());
      entriesInCRegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutCRegionDataSet->numEntries());
      entriesInDRegionHist->Fill(lowerEta+0.1,nom,etaCutNomCutDRegionDataSet->numEntries());
      //debug/output
      std::cout << "eta=" <<
        lowerEta << "-";
      //XXX TESTING eta 1.5 max
      //if(10*lowerEta==14)
      //  std::cout << "1.5";
      //else
      //  std::cout << lowerEta+0.2;
      std::cout << lowerEta+0.2;
      std::cout << " nom=" << nom << "-" << nom+1 << std::endl;
      std::cout << "A = " << etaCutNomCutARegionDataSet->numEntries() <<
        " B = " << etaCutNomCutBRegionDataSet->numEntries() <<
        " C = " << etaCutCRegionDataSet->numEntries() << std::endl;
       // " D = " << etaCutNomCutDRegionDataSet->numEntries() << std::endl;

      const RooArgSet* argSet_A = etaCutNomCutARegionDataSet->get();
      RooRealVar* iasData_A = (RooRealVar*)argSet_A->find(rooVarIas.GetName());
      RooRealVar* ihData_A = (RooRealVar*)argSet_A->find(rooVarIh.GetName());
      RooRealVar* etaData_A = (RooRealVar*)argSet_A->find(rooVarEta.GetName());
      RooRealVar* ptData_A = (RooRealVar*)argSet_A->find(rooVarPt.GetName());
      for(int index=0; index < etaCutNomCutARegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutARegionDataSet->get(index);
        if(ihData_A->getVal() > 3 && ptData_A->getVal() > 45)
          etaARegionHist->Fill(etaData_A->getVal());
      }
      //delete argSet_A;
      //delete iasData_A;
      //delete ihData_A;
      //delete etaData_A;
      //delete ptData_A;
      const RooArgSet* argSet_C1 = etaCutNomCutCRegionDataSet->get();
      RooRealVar* pData_C1 = (RooRealVar*)argSet_C1->find(rooVarP.GetName());
      RooRealVar* ptData_C1 = (RooRealVar*)argSet_C1->find(rooVarPt.GetName());
      RooRealVar* ihData_C1 = (RooRealVar*)argSet_C1->find(rooVarIh.GetName());
      RooRealVar* etaData_C1 = (RooRealVar*)argSet_C1->find(rooVarEta.GetName());
      for(int index=0; index < etaCutNomCutCRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutCRegionDataSet->get(index);
        //std::cout << "Fill hist: " << " eta = " << etaData_C1->getVal() << std::endl;
        //std::cout << " pt = " << ptData_C1->getVal() << std::endl;
        //std::cout << "Entries in hist: " << pEtaCRegionHist->GetEntries() << std::endl;
        if(ihData_C1->getVal() > 3 && ptData_C1->getVal() > 45)
          pEtaCRegionHist->Fill(etaData_C1->getVal(),pData_C1->getVal());
      }
      //delete argSet_C1;
      //delete pData_C1;
      //delete ptData_C1;
      //delete ihData_C1;
      //delete etaData_C1;

      // immediately delete no-longer-needed datasets
      delete nomCutARegionDataSet;
      delete nomCutDRegionDataSet;
      delete etaCutNomCutDRegionDataSet;
      delete etaCutNomCutARegionDataSet;
      delete etaCutNomCutCRegionDataSet;
      delete nomCutBRegionDataSet;

      const RooArgSet* argSet_B = etaCutNomCutBRegionDataSet->get();
      RooRealVar* ihData_B = (RooRealVar*)argSet_B->find(rooVarIh.GetName());
      RooRealVar* iasData_B = (RooRealVar*)argSet_B->find(rooVarIas.GetName());
      RooRealVar* etaData_B = (RooRealVar*)argSet_B->find(rooVarEta.GetName());
      RooRealVar* ptData_B = (RooRealVar*)argSet_B->find(rooVarPt.GetName());
      //remove eta/nom double checking
      //RooRealVar* nomData_B = (RooRealVar*)argSet_B->find(rooVarNoMias.GetName());
      const RooArgSet* argSet_C = etaCutCRegionDataSet->get();
      RooRealVar* pData_C = (RooRealVar*)argSet_C->find(rooVarP.GetName());
      RooRealVar* ptData_C = (RooRealVar*)argSet_C->find(rooVarPt.GetName());
      RooRealVar* etaData_C = (RooRealVar*)argSet_C->find(rooVarEta.GetName());
      //RooRealVar* nomData_C = (RooRealVar*)argSet_C->find(rooVarNoMias.GetName());
      // fill b region hist
      for(int index=0; index < etaCutNomCutBRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutBRegionDataSet->get(index);
        if(useIasForSideband)
        {
          if(iasData_B->getVal() < iasSidebandThreshold)
            std::cout << "ERROR: (in B region) ias=" << iasData_B->getVal() << std::endl;
        }
        else
        {
          if(ihData_B->getVal() < ihSidebandThreshold)
            std::cout << "ERROR: (in B region) ih=" << ihData_B->getVal() << std::endl;
        }
        //if(nomData_B->getVal() < nom || (nomData_B->getVal() > nom+1 && nom<21))
        //  std::cout << "ERROR: nom=" << nom << "-" << nom+1 << " and (in B region) data point nom="
        //    << nomData_B->getVal() << std::endl;
        //if(etaData_B->getVal() < lowerEta || etaData_B->getVal() > lowerEta+0.2)
        //  std::cout << "ERROR: eta=" << lowerEta << "-" << lowerEta+0.2 << " and (in B region) data point eta="
        //    << etaData_B->getVal() << std::endl;

        bRegionHist->Fill(ihData_B->getVal());
        iasBRegionHist->Fill(iasData_B->getVal());
        if(ihData_B->getVal() > 3 && ptData_B->getVal() > 45)
        {
          etaBRegionHist->Fill(etaData_B->getVal());
          iasBRegionTotalHist->Fill(iasData_B->getVal());
          ihBRegionTotalHist->Fill(ihData_B->getVal());
        }
        ////debug
        //std::cout << "Filled B region hist: " << bRegionHist->GetName()
        //  << " index = " << index << std::endl;

      }
      // fill c region hist
      for(int index=0; index < etaCutCRegionDataSet->numEntries(); ++index)
      {
        etaCutCRegionDataSet->get(index);
        cRegionHist->Fill(pData_C->getVal());

        if(usePtForSideband)
        {
          if(ptData_C->getVal() < ptSidebandThreshold)
            std::cout << "ERROR: (in C region) pt=" << ptData_C->getVal() << std::endl;
        }
        else
        {
          if(pData_C->getVal() < pSidebandThreshold)
            std::cout << "ERROR: (in C region) p=" << pData_C->getVal() << std::endl;
        }
        // no NoM checking in c region; using eta cut only
        //if(nomData_C->getVal() < nom || nomData_C->getVal() > nom+1)
        //  std::cout << "ERROR: nom=" << nom << "-" << nom+1 << " and (in C region) data point nom="
        //    << nomData_C->getVal() << std::endl;
        //if(etaData_C->getVal() < lowerEta || etaData_C->getVal() > lowerEta+0.2)
        //  std::cout << "ERROR: eta=" << lowerEta << "-" << lowerEta+0.2 << " and (in C region) data point eta="
        //    << etaData_C->getVal() << std::endl;
      }

      // c region cumu hist
      cRegionHist->ComputeIntegral();
      Double_t *cRegionIntegral = cRegionHist->GetIntegral();
      cRegionCumuHist->SetContent(cRegionIntegral);

      // ias prediction histogram in this NoM/eta bin
      std::string iasPredictionFixedHistName = getHistNameBeg(nom,lowerEta);
      iasPredictionFixedHistName+="iasPredictionFixedHist";
      std::string iasPredictionFixedHistTitle = "Ias prediction for ";
      iasPredictionFixedHistTitle+=intToString(nom);
      iasPredictionFixedHistTitle+="-";
      if(nom==21)
        iasPredictionFixedHistTitle+="end";
      else
        iasPredictionFixedHistTitle+=intToString(nom+1);
      iasPredictionFixedHistTitle+=", ";
      iasPredictionFixedHistTitle+=floatToString(lowerEta);
      iasPredictionFixedHistTitle+=" < |#eta| < ";
      iasPredictionFixedHistTitle+=floatToString(lowerEta+0.2);
      iasPredictionFixedHistTitle+=", mass > ";
      iasPredictionFixedHistTitle+=floatToString(massCutIasHighPHighIh_);
      iasPredictionFixedHistTitle+=" GeV";
      TH1F* iasPredictionFixedHist = iasPredictionFixedBinsDir.make<TH1F>(iasPredictionFixedHistName.c_str(),iasPredictionFixedHistTitle.c_str(),100,0,1);
      //TProfile* iasPredictionFixedHist = iasPredictionFixedBinsDir.make<TProfile>(iasPredictionFixedHistName.c_str(),iasPredictionFixedHistTitle.c_str(),100,0,1);
      //iasPredictionFixedHist->SetErrorOption("g");
      iasPredictionFixedHist->Sumw2();
      // success rate histogram in this NoM/eta bin
      std::string iasSuccessRateHistName = getHistNameBeg(nom,lowerEta);
      iasSuccessRateHistName+="iasSuccessRateHist";
      std::string iasSuccessRateHistTitle = "Ias success rate for nom ";
      iasSuccessRateHistTitle+=intToString(nom);
      iasSuccessRateHistTitle+="-";
      if(nom==21)
        iasSuccessRateHistTitle+="end";
      else
        iasSuccessRateHistTitle+=intToString(nom+1);
      iasSuccessRateHistTitle+=", ";
      iasSuccessRateHistTitle+=floatToString(lowerEta);
      iasSuccessRateHistTitle+=" < |#eta| < ";
      iasSuccessRateHistTitle+=floatToString(lowerEta+0.2);
      iasSuccessRateHistTitle+=", mass > ";
      iasSuccessRateHistTitle+=floatToString(massCutIasHighPHighIh_);
      iasSuccessRateHistTitle+=" GeV";
      TH1F* iasSuccessRateHist = successRateDir.make<TH1F>(iasSuccessRateHistName.c_str(),iasSuccessRateHistTitle.c_str(),100,0,1);
      //iasSuccessRateHist->Sumw2();

      // require at least 1 entry in each dataset to do prediction
      if(etaCutNomCutBRegionDataSet->numEntries() < 1 || etaCutCRegionDataSet->numEntries() < 1)
      {
        std::cout << "WARNING: too few entries-not doing prediction for this slice: eta=" << lowerEta << "-";
        //XXX TESTING eta 1.5 max
        if(10*lowerEta==14)
          std::cout << "1.5";
        else
          std::cout << lowerEta+0.2;
        std::cout << " nom=" << nom << "-";
        if(nom==21)
          std::cout << "end" << std::endl;
        else
          std::cout << nom+1 << std::endl;

        std::cout << "\t\t" << etaCutNomCutBRegionDataSet->numEntries() <<
          " entries in B region and " << etaCutCRegionDataSet->numEntries() <<
          " entries in C region" << std::endl;
        continue;
      }

      // Ih mean in Ias bins histogram in this NoM/eta bin
      std::string ihMeanProfName = getHistNameBeg(nom,lowerEta);
      ihMeanProfName+="ihMeanProf";
      std::string ihMeanProfTitle = "Ih mean for nom ";
      ihMeanProfTitle+=intToString(nom);
      ihMeanProfTitle+="-";
      if(nom==21)
        ihMeanProfTitle+="end";
      else
        ihMeanProfTitle+=intToString(nom+1);
      ihMeanProfTitle+=", ";
      ihMeanProfTitle+=floatToString(lowerEta);
      ihMeanProfTitle+=" < |#eta| < ";
      ihMeanProfTitle+=floatToString(lowerEta+0.2);
      ihMeanProfTitle+=", mass > ";
      ihMeanProfTitle+=floatToString(massCutIasHighPHighIh_);
      ihMeanProfTitle+=" GeV";
      TProfile* ihMeanProf = ihMeanDir.make<TProfile>(ihMeanProfName.c_str(),ihMeanProfTitle.c_str(),100,0,1);
      // Min P cut mean in Ias bins histogram in this NoM/eta bin
      std::string minPCutMeanProfName = getHistNameBeg(nom,lowerEta);
      minPCutMeanProfName+="minPCutMeanProf";
      std::string minPCutMeanProfTitle = "Min P cut for nom ";
      minPCutMeanProfTitle+=intToString(nom);
      minPCutMeanProfTitle+="-";
      if(nom==21)
        minPCutMeanProfTitle+="end";
      else
        minPCutMeanProfTitle+=intToString(nom+1);
      minPCutMeanProfTitle+=", ";
      minPCutMeanProfTitle+=floatToString(lowerEta);
      minPCutMeanProfTitle+=" < |#eta| < ";
      minPCutMeanProfTitle+=floatToString(lowerEta+0.2);
      minPCutMeanProfTitle+=", mass > ";
      minPCutMeanProfTitle+=floatToString(massCutIasHighPHighIh_);
      minPCutMeanProfTitle+=" GeV";
      TProfile* minPCutMeanProf = minPCutMeanDir.make<TProfile>(minPCutMeanProfName.c_str(),minPCutMeanProfTitle.c_str(),100,0,1);

      std::cout << "INFO doing slice ( " << (nomSlice-1)*(1+(nomMax_-nomMin_)/2)+etaSlice << " / " << slicesToDo <<
        " ): eta=" <<
        lowerEta << "-" << lowerEta+0.2;
      if(nom==21)
        std::cout << " nom=" << nom << "-end" << std::endl;
      else
        std::cout << " nom=" << nom << "-" << nom+1 << std::endl;

      // loop over Ih/Ias and calculate successRate
      for(int index=0; index < etaCutNomCutBRegionDataSet->numEntries(); ++index)
      {
        etaCutNomCutBRegionDataSet->get(index);
        double thisIh = ihData_B->getVal();
        double thisIas = iasData_B->getVal();
        if(thisIas < 0) continue; // require ias >= 0
        // compute minimum p to pass mass cut
        double momSqr = (pow(massCutIasHighPHighIh_,2)*dEdx_k)/(thisIh-dEdx_c);
        if(momSqr<0)
        {
          std::cout << "ERROR: nom=" << nom << "-" << nom+1 << " and eta=" <<
            lowerEta << "-" << lowerEta+0.2 << "; momSqr for ih=" << thisIh <<
            " mass=" << massCutIasHighPHighIh_ << " = " << momSqr << std::endl;
          continue;
        }
        double minMomPassMass = sqrt(momSqr);
        RooDataSet* momPassDataSet = (RooDataSet*)etaCutCRegionDataSet->reduce(Cut(("rooVarP>"+floatToString(minMomPassMass)).c_str()),
            SelectVars(rooVarP));
        RooDataSet* momFailDataSet = (RooDataSet*)etaCutCRegionDataSet->reduce(Cut(("rooVarP<"+floatToString(minMomPassMass)).c_str()),
            SelectVars(rooVarP));
        int numMomPassingMass = momPassDataSet->numEntries();
        int numMomFailingMass = momFailDataSet->numEntries();
        delete momPassDataSet;
        delete momFailDataSet;
        int total = numMomPassingMass+numMomFailingMass;
        double successRate = numMomPassingMass/(double)total;
        double successRateError = sqrt(successRate*(1-successRate)/(double)total);
        successRateSumsInIasBins[iasPredictionFixedHist->FindBin(thisIas)-1]+=successRate;
        successRateCountsInIasBins[iasPredictionFixedHist->FindBin(thisIas)-1]++;
        cRegionTracksOverMassCutProfile->Fill(thisIas,numMomPassingMass);
        //FIXME?
        successRateLastErrorInIasBins[iasPredictionFixedHist->FindBin(thisIas)-1]=successRateError;
        ihMeanProf->Fill(thisIas,thisIh);
        minPCutMeanProf->Fill(thisIas,minMomPassMass);
        
        //if(nom==11 && (int)(lowerEta*10)==4)
        //{
        //std::cout << "numMomPassingMass: " << numMomPassingMass << " numMomFailingMass: " <<
        //  numMomFailingMass << " minMomPassMass:" << minMomPassMass << " thisIh: " << thisIh << std::endl;
        //std::cout << "ias: " << thisIas << " successRate: " << successRate << " successRateError:"
        //  << successRateError << " trials:"  << total << std::endl;
        //}
      }

      // fill the success rate histogram
      for(int i=0; i<100; ++i)
      {
        if(successRateCountsInIasBins[i] > 0)
        {
          iasSuccessRateHist->SetBinContent(i+1,successRateSumsInIasBins[i]/successRateCountsInIasBins[i]);
          iasSuccessRateHist->SetBinError(i+1,successRateLastErrorInIasBins[i]);
          //debug
          //std::cout << "ias: " << iasSuccessRateHist->GetBinCenter(i+1) << 
          //  " trials in this ias bin: " << successRateCountsInIasBins[i] <<
          //  " successRate: " << successRateSumsInIasBins[i]/successRateCountsInIasBins[i] << 
          //  " 28.36 (C region ratio)*successRate: " << 28.36*successRateSumsInIasBins[i]/successRateCountsInIasBins[i] <<
          //  std::endl;
        }
      }

      //TODO: move to scaling code
    ////XXX test histogram for particular slice
    //if(lowerNoM==11 && (int)(lowerEta*10)==4)
    //{
    //  cout << "INFO: Rescaling this slice to 0.3 of nominal. " << endl;
    //  for(int bin=1; bin<=histItr->GetNbinsX();++bin)
    //  {
    //    double binc = 0.3*histItr->GetBinContent(bin);
    //    double bine = 0.3*histItr->GetBinError(bin);
    //    histItr->SetBinContent(bin,binc);
    //    histItr->SetBinError(bin,bine);
    //  }
    //}

      // multiply by the original ias in this slice before mass cut
      iasPredictionFixedHist->Multiply(iasBRegionHist,iasSuccessRateHist);

      // ias prediction histogram in this NoM/eta bin -- variable
      std::string iasPredictionVarBinHistName = getHistNameBeg(nom,lowerEta);
      iasPredictionVarBinHistName+="iasPredictionVarBinHist";
      std::string iasPredictionVarBinHistTitle = "Ias prediction for nom ";
      iasPredictionVarBinHistTitle+=intToString(nom);
      iasPredictionVarBinHistTitle+="-";
      if(nom==21)
        iasPredictionVarBinHistTitle+="end";
      else
        iasPredictionVarBinHistTitle+=intToString(nom+1);
      iasPredictionVarBinHistTitle+=", ";
      iasPredictionVarBinHistTitle+=floatToString(lowerEta);
      iasPredictionVarBinHistTitle+=" < |#eta| < ";
      iasPredictionVarBinHistTitle+=floatToString(lowerEta+0.2);
      iasPredictionVarBinHistTitle+=", mass > ";
      iasPredictionVarBinHistTitle+=floatToString(massCutIasHighPHighIh_);
      iasPredictionVarBinHistTitle+=" GeV";
      // figure out variable bins
      std::vector<double> binVec = computeVariableBins(iasPredictionFixedHist);
      TH1F* iasPredictionVarBinHist = iasPredictionVarBinsDir.make<TH1F>(iasPredictionVarBinHistName.c_str(),
          iasPredictionVarBinHistTitle.c_str(),binVec.size()-1,&(*binVec.begin()));
      iasPredictionVarBinHist->Sumw2();

      // get bin contents/errors
      const int numVarBins = iasPredictionVarBinHist->GetNbinsX();
      std::vector<double> binContents;
      std::vector<double> binErrors;
      for(int i=0; i<numVarBins; ++i)
      {
        binContents.push_back(0);
        binErrors.push_back(0);
      }
      for(int bin=1; bin<=iasPredictionFixedHist->GetNbinsX();++bin)
      {
        double binc = iasPredictionFixedHist->GetBinContent(bin);
        double bine = iasPredictionFixedHist->GetBinError(bin);
        if(binc > 0)
        {
          binContents[iasPredictionVarBinHist->FindBin(iasPredictionFixedHist->GetBinCenter(bin))-1] += binc;
          binErrors[iasPredictionVarBinHist->FindBin(iasPredictionFixedHist->GetBinCenter(bin))-1] += (bine*bine);
        }
      }
      // fill it
      for(int i=1; i<numVarBins+1; ++i)
      {
        if(binContents[i-1] > 0)
        {
          // make bins contain number of events per horizontal unit
          double binWidthRatio = iasPredictionVarBinHist->GetBinWidth(1)/iasPredictionVarBinHist->GetBinWidth(i);
          iasPredictionVarBinHist->SetBinContent(i,binWidthRatio*binContents[i-1]);
          iasPredictionVarBinHist->SetBinError(i,binWidthRatio*sqrt(binErrors[i-1]));
        }
      }

      // if nonzero integral, check to make sure there are no empty bins in the prediction hist
      if(iasPredictionVarBinHist->Integral() > 0)
      {
        for(int bin=1; bin <= iasPredictionVarBinHist->GetNbinsX(); ++bin)
        {
          if(iasPredictionVarBinHist->GetBinContent(bin) <= 0)
          {
            std::cout << "ERROR: For this hist: " << iasPredictionVarBinHist->GetName()
              << " bin content for bin "
              << bin << " is " << iasPredictionVarBinHist->GetBinContent(bin)
              << " with binLowEdge= " << iasPredictionVarBinHist->GetBinLowEdge(bin)
              << " to binHighEdge="
              << iasPredictionVarBinHist->GetBinLowEdge(bin)+iasPredictionVarBinHist->GetBinWidth(bin)
              << ". Fix the binning.  Bailing out." << std::endl;
            return -9;
          }
        }
      }

      delete etaCutNomCutBRegionDataSet;
      delete etaCutCRegionDataSet;
      //delete argSet_B;
      //delete ihData_B;
      //delete iasData_B;
      //delete etaData_B;
      //delete ptData_B;
      //delete argSet_C;
      //delete pData_C;
      //delete ptData_C;
      //delete etaData_C;

    } // eta slices
    if(nom==21) break; // don't do any more after nom21, since we've done 21+ in that case
  } // nom slices

}
