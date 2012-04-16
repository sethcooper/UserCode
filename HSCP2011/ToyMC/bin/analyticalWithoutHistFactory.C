#include "RooRealVar.h"
#include "RooRealConstant.h"
#include "RooWorkspace.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "RooArgSet.h"
#include "RooExtendPdf.h"
#include "RooDataSet.h"
#include "RooProduct.h"
#include "RooRealSumPdf.h"
#include "RooProdPdf.h"
#include "RooMsgService.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/ProfileLikelihoodTestStat.h"

using namespace RooFit;
using namespace RooStats;

analyticalWithoutHistFactory()
{
  RooWorkspace* wspace = new RooWorkspace("combined");

  // lumi
  RooRealVar Lumi("Lumi","Lumi",1.0,0,2);
  RooRealVar nominalLumi("nominalLumi","nominalLumi",1.0);
  nominalLumi.setConstant();
  RooGaussian lumiConstraint("lumiConstraint","lumiConstraint",Lumi,nominalLumi,RooRealConstant::value(0.022));

  // observable
  RooRealVar ias("ias","ias",0,100);
  ias.setBins(100);
  // poi
  RooRealVar sigXSec("sigXSec","sigXSec",0,1);
  RooArgSet poi("poi");
  poi.add(sigXSec);

  // background pdf -- gluino1200 exp
  RooRealVar expoSlope("expoSlope","expoSlope",-0.2437);
  expoSlope.setConstant();
  RooExponential backgroundExp("backgroundExp","backgroundExp",ias,expoSlope);
  RooRealVar backNorm("backNorm","backNorm",0.08735); // normalize to exp events
  backNorm.setConstant();
  RooExtendPdf backgroundOnly("backgroundOnly","backgroundOnly",backgroundExp,backNorm);

  // signal pdf -- taken from Gluino1200, from apr5 directory on laptop (one slice)
  RooRealVar gausMean("gausMean","gausMean",73.31);
  RooRealVar gausWidth("gausWidth","gausWidth",11.09);
  gausMean.setConstant();
  gausWidth.setConstant();
  RooGaussian signalGaus("signalGaus","signalGaus",ias,gausMean,gausWidth);
  RooRealVar sigNorm("sigNorm","sigNorm",655.8); // normalize to exp events with 1 pb cross section
  sigNorm.setConstant();
  RooProduct lumiXCrossSection("lumiXCrossSection","lumiXCrossSection",RooArgSet(Lumi,sigXSec));
  RooProduct sigNormXLumiXCrossSection("sigNormXLumiXCrossSection","sigNormXLumiXCrossSection",RooArgSet(sigNorm,lumiXCrossSection));
  //RooExtendPdf signal("signal","signal",signalGaus,lumiXCrossSection);

  RooAddPdf combinedModel("combinedModel","combinedModel",RooArgList(backgroundExp,signalGaus),
      RooArgList(backNorm,sigNormXLumiXCrossSection));

  RooProdPdf model("model","model",RooArgSet(lumiConstraint,combinedModel));
  // bg only
  RooProdPdf bgOnlyModel("bgOnlyModel","bgOnlyModel",RooArgSet(lumiConstraint,backgroundOnly));

  // generate background-only data
  RooDataSet* asimovDataSet = backgroundExp.generate(ias,RooFit::ExpectedData());
  wspace->import(*asimovDataSet,Rename("AsimovData"));

  // create set of global observables (need to be defined as constants)
  RooArgSet globalObs("global_obs");
  globalObs.add(nominalLumi);

  // create set of nuisance parameters
  RooArgSet nuis("nuis");
  nuis.add(Lumi);

  // create set of observables
  RooArgSet obs("observables");
  obs.add(ias);

  ModelConfig* modelConfig = new ModelConfig("modelConfig");
  modelConfig->SetWorkspace(*wspace);
  modelConfig->SetPdf(model);
  modelConfig->SetParametersOfInterest(poi);
  modelConfig->SetGlobalObservables(globalObs);
  modelConfig->SetNuisanceParameters(nuis);
  modelConfig->SetObservables(obs);

  // set parameter snapshot that corresponds to the best fit to data
  RooAbsReal* pNll = modelConfig->GetPdf()->createNLL( *asimovDataSet );
  RooAbsReal* pProfile = pNll->createProfile( globalObs ); // do not profile global observables
  pProfile->getVal(); // this will do fit and set POI and nuisance parameters to fitted values
  RooArgSet* pPoiAndNuisance = new RooArgSet("poiAndNuisance");
  pPoiAndNuisance->add(*modelConfig->GetParametersOfInterest());
  modelConfig->SetSnapshot(*pPoiAndNuisance);
  delete pProfile;
  delete pNll;
  delete pPoiAndNuisance;

  wspace->import(*modelConfig);
  wspace->Print();
  modelConfig->Print();
  
  TFile* myFile = new TFile("myFile.root","recreate");
  myFile->cd();
  modelConfig->Write();
  wspace->Write();
  myFile->Close();

  ///////////////////////
  // generate my own toys

  sigXSec.setVal(0);
  std::cout << "Cross section value: " << sigXSec.getVal() << std::endl;
  // check to make sure I have expectedEvents()
  std::cout << "Expected events (ias): " << model.expectedEvents(ias) << std::endl;

  // test stat - SB
  ProfileLikelihoodTestStat profll(model);
  profll.SetOneSided(true);
  //profll.SetStrategy(2);
  //profll.SetTolerance(1e-10);
  //profll.SetPrintLevel(3);
  //RooMsgService::instance().getStream(1).addTopic(Generation);
  // hists of test stat value
  TH1F* bgDataTestStatHist = new TH1F("bgDataTestStat","BG-only data test stat",2500,0,2.5);
  TH1F* sbDataTestStatHist = new TH1F("sbDataTestStat","SB data test stat",2500,0,2.5);
  double sigma1 = 0.002588;
  double sigma2 = 0.002714;

  TH1* modelHistogram = model.createHistogram("ias",100);
  modelHistogram->Scale(model.expectedEvents(ias));

  int numToys = 2000;
  for(int i=0; i < numToys; ++i)
  {
    sigXSec.setVal(sigma1);
    //RooAbsData* lumiData = lumiConstraint.generate(RooArgSet(Lumi),1);
    //RooArgSet* set = lumiData->get();
    //RooRealVar* lData = set->find(Lumi.GetName());
    //Lumi.setVal(lData->getVal());
    RooAbsData* toySBData = model.generate(RooArgSet(ias),Extended());
    //// select only datasets with >= 1 event
    //if(toyData->numEntries()==0)
    //  continue;
    ////TH1* toyHist = toyData->createHistogram("ias",50);
    sigXSec.setVal(sigma1);
    double testStatValue = profll.Evaluate(*toySBData,RooArgSet(sigXSec));
    sbDataTestStatHist->Fill(testStatValue);
    sigXSec.setVal(0);
    RooAbsData* toyBData = model.generate(RooArgSet(ias),Extended());
    sigXSec.setVal(sigma1);
    testStatValue = profll.Evaluate(*toyBData,RooArgSet(sigXSec));
    bgDataTestStatHist->Fill(testStatValue);

    delete toySBData;
    delete toyBData;
  }
  //modelHistogram->Draw();
  //toyHist->SetMarkerColor(kRed);
  //toyHist->SetLineColor(kRed);
  //toyHist->Draw("sames");

  bgDataTestStatHist->SetLineColor(kBlue);
  bgDataTestStatHist->SetMarkerColor(kBlue);
  bgDataTestStatHist->Draw();
  sbDataTestStatHist->SetLineColor(kRed);
  sbDataTestStatHist->SetMarkerColor(kRed);
  sbDataTestStatHist->Draw("sames");
}

