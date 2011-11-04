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
#include "TArray.h"

//RooFit
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooDataSet.h"
#include "RooHistPdf.h"
#include "RooPlot.h"
#include "RooBinning.h"
#include "RooAddPdf.h"
#include "RooMinuit.h"
#include "RooNLLVar.h"
#include "RooMCStudy.h"

#include "TSystem.h"
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

#include <vector>

namespace reco    { class Vertex; class Track; class GenParticle; class DeDxData; class MuonTimeExtra;}
namespace susybsm { class HSCParticle; class HSCPIsolation;}
namespace fwlite  { class ChainEvent;}
namespace trigger { class TriggerEvent;}
namespace edm     {class TriggerResults; class TriggerResultsByName; class InputTag;}

#if !defined(__CINT__) && !defined(__MAKECINT__)

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

#endif

#include "commonFunctions.h"

// ****** helper functions
int findNoMSlice(int nom)
{
  if(nom==5 || nom==6)
    return 0;
  else if(nom==7 || nom==8)
    return 1;
  else if(nom==9 || nom==10)
    return 2;
  else if(nom==11 || nom==12)
    return 3;
  else if(nom==13 || nom==14)
    return 4;
  else if(nom==15 || nom==16)
    return 5;
  else if(nom==17 || nom==18)
    return 6;
  else if(nom==19 || nom==20)
    return 7;
  else if(nom>=21)
    return 8;
  else
    return -1;
}
//
int findEtaSlice(double absEta)
{
  if(absEta < 0.2)
    return 0;
  else if(absEta < 0.4)
    return 1;
  else if(absEta < 0.6)
    return 2;
  else if(absEta < 0.8)
    return 3;
  else if(absEta < 1.0)
    return 4;
  else if(absEta < 1.2)
    return 5;
  else if(absEta < 1.4)
    return 6;
  else if(absEta < 1.6)
    return 7;
  else if(absEta < 1.8)
    return 8;
  else if(absEta < 2.0)
    return 9;
  else if(absEta < 2.2)
    return 10;
  else if(absEta < 2.4)
    return 11;
  else
    return -1;
}
//
float getLastBinLowEdge(int nom)
{
  if(nom<7)
    return 0.9;
  else if(nom==7 || nom==8)
    return 0.65;
  else if(nom==9 || nom==10)
    return 0.5;
  else if(nom==11 || nom==12)
    return 0.4;
  else if(nom==13 || nom==14)
    return 0.4;
  else if(nom==15 || nom==16)
    return 0.3;
  else if(nom==17 || nom==18)
    return 0.25;
  else if(nom==19 || nom==20)
    return 0.2;
  else
    return 0.1;
}
//
float getSecondToLastBinLowEdge(int nom)
{
  if(nom<7)
    return 0.8;
  else if(nom==7 || nom==8)
    return 0.55;
  else if(nom==9 || nom==10)
    return 0.4;
  else if(nom==11 || nom==12)
    return 0.35;
  else if(nom==13 || nom==14)
    return 0.35;
  else if(nom==15 || nom==16)
    return 0.25;
  else if(nom==17 || nom==18)
    return 0.2;
  else if(nom==19 || nom==20)
    return 0.15;
  else
    return 0.05;
}
// get NoM string from NoM slice #
std::string getNoMRangeFromSlice(int slice)
{
  if(slice==0)
    return std::string("5-6");
  else if(slice==1)
    return std::string("7-8");
  else if(slice==2)
    return std::string("9-10");
  else if(slice==3)
    return std::string("11-12");
  else if(slice==4)
    return std::string("13-14");
  else if(slice==5)
    return std::string("15-16");
  else if(slice==6)
    return std::string("17-18");
  else if(slice==7)
    return std::string("19-20");
  else if(slice==8)
    return std::string(">=21");
  else
    return std::string("-1");
}
// get eta string from eta slice #
std::string getEtaRangeFromSlice(int slice)
{
  if(slice==0)
    return std::string("0-0.2");
  else if(slice==1)
    return std::string("0.2-0.4");
  else if(slice==2)
    return std::string("0.4-0.6");
  else if(slice==3)
    return std::string("0.6-0.8");
  else if(slice==4)
    return std::string("0.8-1.0");
  else if(slice==5)
    return std::string("1.0-1.2");
  else if(slice==6)
    return std::string("1.2-1.4");
  else if(slice==7)
    return std::string("1.4-1.6");
  else if(slice==8)
    return std::string("1.6-1.8");
  else if(slice==9)
    return std::string("1.8-2.0");
  else if(slice==10)
    return std::string("2.0-2.2");
  else if(slice==11)
    return std::string("2.2-2.4");
  else
    return std::string("-1");
}

// ****** main
int main(int argc, char ** argv)
{
  using namespace RooFit;

  // load framework libraries
  gSystem->Load("libFWCoreFWLite");
  AutoLibraryLoader::enable();

  // parse arguments
  if(argc < 2)
  {
    std::cout << "Usage : " << argv[0] << " [parameters.py]" << std::endl;
    return 0;
  }

  // get the python configuration
  const edm::ParameterSet& process = edm::readPSetsFrom(argv[1])->getParameter<edm::ParameterSet>("process");
  // now get each parameterset
  const edm::ParameterSet& ana = process.getParameter<edm::ParameterSet>("doToyMC");
  // mass cut to use for the high-p high-Ih (search region) ias dist
  double massCut_ (ana.getParameter<double>("MassCut"));
  std::string backgroundPredictionRootFilename_ (ana.getParameter<std::string>("BackgroundPredictionInputRootFile"));
  std::string signalRootFilename_ (ana.getParameter<std::string>("SignalInputRootFile"));
  std::string outputRootFilename_ (ana.getParameter<std::string>("OutputRootFile"));
  // dE/dx calibration
  double dEdx_k (ana.getParameter<double>("dEdx_k"));
  double dEdx_c (ana.getParameter<double>("dEdx_c"));
  // definition of sidebands/search region
  double pSidebandThreshold (ana.getParameter<double>("PSidebandThreshold"));
  double ihSidebandThreshold (ana.getParameter<double>("IhSidebandThreshold"));
  int numBackgroundTracksToGen (ana.getParameter<int>("NumberOfBackgroundTracks"));
  int numSignalTracksToGen (ana.getParameter<int>("NumberOfSignalTracks"));
  int numTrialsToDo (ana.getParameter<int>("NumberOfTrials"));

  TFile* outputRootFile = new TFile(outputRootFilename_.c_str(),"recreate");

  TFile* backgroundPredictionRootFile = TFile::Open(backgroundPredictionRootFilename_.c_str());
  TFile* signalRootFile = TFile::Open(signalRootFilename_.c_str());

  const int numNoMslices = 9;
  const int numEtaSlices = 12;
  TH1F* iasBackgroundPredictionMassCutNoMSliceHists[numNoMslices][numEtaSlices];
  TH1F* iasSignalMassCutNoMSliceHists[numNoMslices][numEtaSlices];
  // get hists, skip NoM < 5
  for(int nom=5; nom < 23; nom+=2)
  {
    for(int etaSlice=0; etaSlice < 25; etaSlice+=2)
    {
      string dirName="iasPredictionsVariableBins";
      //string dirName="iasPredictionsFixedBins";

      string histName="nom";
      histName+=intToString(nom);
      histName+="to";
      histName+=intToString(nom+1);
      histName+="eta";
      histName+=intToString(etaSlice);
      histName+="to";
      histName+=intToString(etaSlice+2);
      string sigHistName = histName;
      sigHistName+="iasSignalVarBinHist";
      histName+="iasPredictionVarBinHist";
      //histName+="iasPredictionFixedHist";
      std::string iasSignalVarBinHistTitle = "Ias of signal for nom ";
      iasSignalVarBinHistTitle+=intToString(nom);
      iasSignalVarBinHistTitle+="-";
      iasSignalVarBinHistTitle+=intToString(nom+1);
      iasSignalVarBinHistTitle+=", ";
      iasSignalVarBinHistTitle+=floatToString(etaSlice/10.);
      iasSignalVarBinHistTitle+=" < #eta < ";
      iasSignalVarBinHistTitle+=floatToString((etaSlice+2)/10.);
      iasSignalVarBinHistTitle+=", mass > ";
      iasSignalVarBinHistTitle+=floatToString(massCut_);
      iasSignalVarBinHistTitle+=" GeV";

      string fullPath = dirName;
      fullPath+="/";
      fullPath+=histName;

      iasBackgroundPredictionMassCutNoMSliceHists[nom/2-2][etaSlice/2] =
        (TH1F*)backgroundPredictionRootFile->Get(fullPath.c_str());

      if(iasBackgroundPredictionMassCutNoMSliceHists[nom/2-2][etaSlice/2])
      {
        iasSignalMassCutNoMSliceHists[nom/2-2][etaSlice/2] =
          (TH1F*)iasBackgroundPredictionMassCutNoMSliceHists[nom/2-2][etaSlice/2]->Clone();
        iasSignalMassCutNoMSliceHists[nom/2-2][etaSlice/2]->Reset("ICESM");
        iasSignalMassCutNoMSliceHists[nom/2-2][etaSlice/2]->SetName(sigHistName.c_str());
        iasSignalMassCutNoMSliceHists[nom/2-2][etaSlice/2]->SetTitle(iasSignalVarBinHistTitle.c_str());
      }
      else
        iasSignalMassCutNoMSliceHists[nom/2-2][etaSlice/2] = 0;
    }
  }

  // define observables
  RooRealVar rooVarIas("rooVarIas","ias",0,1);
  RooRealVar rooVarIh("rooVarIh","ih",0,15);
  RooRealVar rooVarP("rooVarP","p",0,5000);
  RooRealVar rooVarNoMias("rooVarNoMias","nom",0,30);
  RooRealVar rooVarEta("rooVarEta","eta",0,2.5);

  // get roodataset from signal file
  RooDataSet* rooDataSetAllSignal = (RooDataSet*)signalRootFile->Get("rooDataSetAll");
  if(rooDataSetAllSignal->numEntries() < 1)
  {
    std::cout << "Problem with RooDataSet named rooDataSetAll in file " <<
      signalRootFilename_.c_str() << std::endl;
    return -1;
  }

  std::string pSearchCutString = "rooVarP>";
  pSearchCutString+=floatToString(pSidebandThreshold);
  std::string ihSearchCutString = "rooVarIh>";
  ihSearchCutString+=floatToString(ihSidebandThreshold);
  RooDataSet* regionD1DataSetSignal = (RooDataSet*)rooDataSetAllSignal->reduce(Cut(pSearchCutString.c_str()));
  RooDataSet* regionDDataSetSignal = (RooDataSet*)regionD1DataSetSignal->reduce(Cut(ihSearchCutString.c_str()));

  // construct signal prediction hists
  for(int nomSlice=0; nomSlice < numNoMslices; ++nomSlice)
  {
    for(int etaSlice=0; etaSlice < numEtaSlices; ++etaSlice)
    {
      if(!iasSignalMassCutNoMSliceHists[nomSlice][etaSlice])
        continue;

      std::string nomCutString = "rooVarNoMias==";
      nomCutString+=intToString(nomSlice*2+5);
      nomCutString+="||rooVarNoMias==";
      nomCutString+=intToString(nomSlice*2+6);
      RooDataSet* nomCutDRegionDataSetSignal =
        (RooDataSet*)regionDDataSetSignal->reduce(Cut(nomCutString.c_str()));
      std::string etaCutString = "rooVarEta>";
      etaCutString+=floatToString(etaSlice*0.2);
      etaCutString+="&&rooVarEta<";
      etaCutString+=floatToString((etaSlice+1)*0.2);
      RooDataSet* etaCutNomCutDRegionDataSetSignal =
        (RooDataSet*)nomCutDRegionDataSetSignal->reduce(Cut(etaCutString.c_str()));

      const RooArgSet* argSet = etaCutNomCutDRegionDataSetSignal->get();
      RooRealVar* iasData = (RooRealVar*)argSet->find(rooVarIas.GetName());
      RooRealVar* ihData = (RooRealVar*)argSet->find(rooVarIh.GetName());
      RooRealVar* pData = (RooRealVar*)argSet->find(rooVarP.GetName());
      for(int evt=0; evt < etaCutNomCutDRegionDataSetSignal->numEntries(); ++evt)
      {
        etaCutNomCutDRegionDataSetSignal->get(evt);
        // apply mass cut
        float massSqr = (ihData->getVal()-dEdx_c)*pow(pData->getVal(),2)/dEdx_k;
        if(massSqr < 0)
          continue;
        else if(sqrt(massSqr) >= massCut_)
          iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->Fill(iasData->getVal());
      }
      for(int bin=1; bin<=iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetNbinsX(); ++bin)
      {
        double binWidthRatio =
          iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetBinWidth(1)/
          iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetBinWidth(bin);
        double binc = iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetBinContent(bin);
        double bine = iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetBinError(bin);
        iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->SetBinContent(bin,binWidthRatio*binc);
        iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->SetBinError(bin,binWidthRatio*bine);
      }
    }
  }

  RooDataHist* iasBackgroundDataHists[numNoMslices][numEtaSlices];
  RooHistPdf* iasBackgroundHistPdfs[numNoMslices][numEtaSlices];
  RooDataHist* iasSignalDataHists[numNoMslices][numEtaSlices];
  RooHistPdf* iasSignalHistPdfs[numNoMslices][numEtaSlices];
  // Make the dataHists/histPdfs
  for(int nomSlice=0; nomSlice < numNoMslices; ++nomSlice)
  {
    for(int etaSlice=0; etaSlice < numEtaSlices; ++etaSlice)
    {
      std::string histName;
      std::string histTitle;
      if(iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice])
      {
        histName = iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice]->GetName();
        histTitle = iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice]->GetTitle();
        std::string dataHistName=histName;
        dataHistName+="DataHistBG";
        std::string dataHistTitle=histTitle;
        dataHistTitle+=" dataHistBG";
        std::string histPdfName=histName;
        histPdfName+="HistPdfBG";
        std::string histPdfTitle=histTitle;
        histPdfTitle+=" histPdfBG";
        iasBackgroundDataHists[nomSlice][etaSlice] = new RooDataHist(dataHistName.c_str(),dataHistTitle.c_str(),
            rooVarIas,iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice]);
        iasBackgroundHistPdfs[nomSlice][etaSlice] = new RooHistPdf(histPdfName.c_str(),histPdfTitle.c_str(),
            rooVarIas, *iasBackgroundDataHists[nomSlice][etaSlice]);
        // signal -- if there are any entries in this eta/nom slice
        if(iasSignalMassCutNoMSliceHists[nomSlice][etaSlice])
        {
          histName = iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetName();
          histTitle = iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->GetTitle();
          dataHistName=histName;
          dataHistName+="DataHistSig";
          dataHistTitle=histTitle;
          dataHistTitle+=" dataHistSig";
          histPdfName=histName;
          histPdfName+="HistPdfSig";
          histPdfTitle=histTitle;
          histPdfTitle+=" histPdfSig";
          iasSignalDataHists[nomSlice][etaSlice] = new RooDataHist(dataHistName.c_str(),dataHistTitle.c_str(),
              rooVarIas,iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]);
          iasSignalHistPdfs[nomSlice][etaSlice] = new RooHistPdf(histPdfName.c_str(),histPdfTitle.c_str(),
              rooVarIas, *iasSignalDataHists[nomSlice][etaSlice]);
        }
      }
      else
      {
        iasBackgroundDataHists[nomSlice][etaSlice] = 0;
        iasBackgroundHistPdfs[nomSlice][etaSlice] = 0;
        iasSignalDataHists[nomSlice][etaSlice] = 0;
        iasSignalHistPdfs[nomSlice][etaSlice] = 0;
      }
    }
  }

  outputRootFile->cd();

  RooRealVar fsig("fsig","signal frac",0.2,0,1);
  RooRealVar nsig("nSig","signal evts",11,0,100000);
  RooRealVar nback("nBack","background evts",71994,0,2e8);

  //for(int nomSlice=0; nomSlice < numNoMslices; ++nomSlice)
  //XXX TESTING
  for(int nomSlice=4; nomSlice < 5; ++nomSlice)
  {
    //for(int etaSlice=0; etaSlice < numEtaSlices; ++etaSlice)
    //XXX TESTING
    for(int etaSlice=1; etaSlice < 2; ++etaSlice)
    {
      if(!iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice])
        continue;

      // histPdf plots
      if(iasBackgroundHistPdfs[nomSlice][etaSlice])
      {
        RooAddPdf sigBackPdf("sigBackPdf","sigBackPdf",RooArgList(*iasSignalHistPdfs[nomSlice][etaSlice],
              *iasBackgroundHistPdfs[nomSlice][etaSlice]),fsig);
        //RooAddPdf sigBackPdf("sigBackPdf","sigBackPdf",RooArgList(*iasSignalHistPdfs[nomSlice][etaSlice],
        //      *iasBackgroundHistPdfs[nomSlice][etaSlice]),RooArgList(nsig,nback));
        int nBins = iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice]->GetNbinsX();
        RooBinning thisBinning(nBins,
            iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice]->GetXaxis()->GetXbins()->GetArray());
        rooVarIas.setBinning(thisBinning);
        //TODO check here for a signal prediction 
        const int numTrials = numTrialsToDo;
        int totalNumTracks = numSignalTracksToGen+numBackgroundTracksToGen;
        //fsig = 11/(double)(10799+11); // standard 1/fb hypothesis
        fsig = numSignalTracksToGen/(double)totalNumTracks;
        //RooMCStudy mcStudy(*iasBackgroundHistPdfs[nomSlice][etaSlice],rooVarIas,Silence(),Binned(true));
        RooMCStudy mcStudy(sigBackPdf,rooVarIas,Silence(),Binned(true));
        mcStudy.generate(numTrials,totalNumTracks,true); // keep gen data
        //mcStudy.generate(numTrials,10799+11,true); // keep gen data; standard 1/fb hypothesis
        //mcStudy.generate(numTrials,10799+22,true); // keep gen data; standard 1/fb hypothesis

        TH1F* nllBSatHist = new TH1F("nllBSatHist","#chi^{2} ~ 2*(NLL_b-NLL_sat)",1000,0,100);
        TH1F* nllSatOnlyHist = new TH1F("nllSatonlyHist","NLL sat only",30000,-30000,0);
        TH1F* nllBOnlyHist = new TH1F("nllBonlyHist","NLL B only",30000,-30000,0);
        TH1F* nllSOnlyHist = new TH1F("nllSonlyHist","NLL S only",30000,-30000,0);
        TH1F* nllSBHist = new TH1F("nllSBHist","NLL SB",30000,-30000,0);

        TH1F* numTracksPerTrialHist = new TH1F("numTracksPerTrial","Tracks per trial",
            500,totalNumTracks-250,totalNumTracks+250);

        for(int i=0; i<numTrials; ++i)
        {
          const RooDataSet* thisData = mcStudy.genData(i);
          RooDataHist* sampleData = new RooDataHist("sampleData","sampleData",rooVarIas,*thisData);
          //TH1F* sampleDataHist = (TH1F*)sampleData->createHistogram("sampleDataHist",rooVarIas,Binning(thisBinning));
          //sampleDataHist->Write();
          //RooDataSet* sampleData = iasBackgroundHistPdfs[nomSlice][etaSlice]->generate(rooVarIas,2000);
          //RooDataHist* sampleData = iasBackgroundHistPdfs[nomSlice][etaSlice]->generateBinned(rooVarIas,
          //    10799);//,Extended(true));
          //sampleData->add(*iasSignalHistPdfs[nomSlice][etaSlice]->generateBinned(rooVarIas,
          //      11,Extended(true)));
          numTracksPerTrialHist->Fill(thisData->sumEntries());

          // saturated model
          RooHistPdf saturatedModel("saturatedModel","saturatedModel",rooVarIas,*sampleData);

          std::string name = string(iasBackgroundHistPdfs[nomSlice][etaSlice]->GetName());
          std::string title = string(iasBackgroundHistPdfs[nomSlice][etaSlice]->GetTitle());
          //iasBackgroundHistPdfs[nomSlice][etaSlice]->fitTo(*sampleData);

          //RooPlot* iasFrame = rooVarIas.frame();
          //sampleData->plotOn(iasFrame);
          ////sampleData->statOn(iasFrame,Label("data"),Format("NEU",AutoPrecision(1)));
          sigBackPdf.fitTo(*sampleData);
          //sigBackPdf.plotOn(iasFrame,Components(*iasSignalHistPdfs[nomSlice][etaSlice]),LineStyle(kDashed),
          //      LineColor(kRed));
          //sigBackPdf.plotOn(iasFrame,Components(*iasBackgroundHistPdfs[nomSlice][etaSlice]),LineStyle(kDashed),
          //      LineColor(kBlue));
          ////iasBackgroundHistPdfs[nomSlice][etaSlice]->plotOn(iasFrame,LineColor(kBlue));
          ////iasBackgroundHistPdfs[nomSlice][etaSlice]->paramOn(iasFrame,Label("fit result"),Format("NEU",AutoPrecision(1)));
          //sigBackPdf.paramOn(iasFrame,Format("NEU",AutoPrecision(1)));
          //name+="Plot";
          //title+=" Hist PDF";
          //iasFrame->SetName(name.c_str());
          //iasFrame->SetTitle(title.c_str());
          //iasFrame->Write();
          //delete iasFrame;
          // NLLs
          RooNLLVar nllVarBOnly("nllVarBOnly","NLL B only",*iasBackgroundHistPdfs[nomSlice][etaSlice],*sampleData);
          RooNLLVar nllVarSOnly("nllVarSOnly","NLL S only",*iasSignalHistPdfs[nomSlice][etaSlice],*sampleData);
          RooNLLVar nllVarSB("nllVarSB","NLL SB",sigBackPdf,*sampleData);
          RooNLLVar nllVarSatModel("nllVarSatModel","NLL Saturated Model",saturatedModel,*sampleData);

          nllBSatHist->Fill(2*(nllVarBOnly.getVal()-nllVarSatModel.getVal()));
          nllSatOnlyHist->Fill(nllVarSatModel.getVal());
          nllBOnlyHist->Fill(nllVarBOnly.getVal());
          nllSOnlyHist->Fill(nllVarSOnly.getVal());
          nllSBHist->Fill(nllVarSB.getVal());
          //std::cout << "NLL SB: " << nllVarSB.getVal() << std::endl;
          //std::cout << "NLL S ONLY: " << nllVarSOnly.getVal() << std::endl;
          //std::cout << "NLL B ONLY: " << nllVarBOnly.getVal() << std::endl;
          //std::cout << "NLL saturated model: " << nllVarSatModel.getVal() << std::endl;
          //std::cout << "chi2 = 2*(NLL_B-Nll_sat) = " << 2*(nllVarBOnly.getVal()-nllVarSatModel.getVal()) << std::endl;

          delete sampleData;
          //delete thisData;
        }
        nllBSatHist->Write();
        nllSatOnlyHist->Write();
        nllBOnlyHist->Write();
        nllSOnlyHist->Write();
        nllSBHist->Write();
        numTracksPerTrialHist->Write();
        
      }
    }
  }

  TDirectory* iasBackgroundPredDir = outputRootFile->mkdir("iasBackgroundPredictions");
  TDirectory* iasSignalDir = outputRootFile->mkdir("iasSignal");
  // Write the dataHists/histPdfs
  for(int nomSlice=0; nomSlice < numNoMslices; ++nomSlice)
  {
    for(int etaSlice=0; etaSlice < numEtaSlices; ++etaSlice)
    {
      if(iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice])
      {
        iasBackgroundPredDir->cd();
        iasBackgroundPredictionMassCutNoMSliceHists[nomSlice][etaSlice]->Write();
        iasSignalDir->cd();
        iasSignalMassCutNoMSliceHists[nomSlice][etaSlice]->Write();
      }
    }
  }

  outputRootFile->Close();

  //std::cout << "Events with Ih >= 3.5 MeV/cm and P >= 50 GeV: " << eventsInHighIhHighPRegion
  //  << "; also passing mass >= " << massCutIasHighPHighIh_ << " : " << eventsInHighIhHighPRegionWithMassCut
  //  << std::endl;
}

