// -*- C++ -*-
//
// Package:    EcalTimeCalibGraphs
// Class:      EcalTimeCalibGraphs
// 
/**\class EcalTimeCalibGraphs EcalTimeCalibGraphs.cc Analyzers/EcalTimeCalibGraphs/src/EcalTimeCalibGraphs.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Tue Apr 28 21:38:51 CDT 2009
// $Id$
//
//


// system include files
#include <memory>
#include <fstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"



//
// class decleration
//

class EcalTimeCalibGraphs : public edm::EDAnalyzer {
   public:
      explicit EcalTimeCalibGraphs(const edm::ParameterSet&);
      ~EcalTimeCalibGraphs();


   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      void SetEStyle();

      // ----------member data ---------------------------
      std::string infile;
      const EcalElectronicsMapping* ecalElectronicsMap_;
      
      TProfile* fedAvgProfile_;
      
      //TFile* outfile_;

      static edm::Service<TFileService> fileService;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//
edm::Service<TFileService> EcalTimeCalibGraphs::fileService;


//
// constructors and destructor
//
EcalTimeCalibGraphs::EcalTimeCalibGraphs(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
   infile = iConfig.getUntrackedParameter<std::string>("infile","");
}


EcalTimeCalibGraphs::~EcalTimeCalibGraphs()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
EcalTimeCalibGraphs::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  SetEStyle();

  //outfile_->cd();
  fedAvgProfile_ = fileService->make<TProfile>("fedAvgProfile","FED Average timing [ns]",54,601,655);

  ifstream calibFile;
  cout << "Attempting to open file: " << infile << endl;
  calibFile.open(infile.c_str(),ifstream::in);
  if(calibFile.good())
  {
    int hashIndex;
    double calibConst, calibConstErr;
    string subDet;

    while(calibFile.good())
    {
      calibFile >>  subDet >> hashIndex >> calibConst >> calibConstErr;
      int FEDid = -1;
      if(subDet=="EB")
      {
        EBDetId det = EBDetId::unhashIndex(hashIndex);
        EcalElectronicsId elecId = ecalElectronicsMap_->getElectronicsId(det);
        FEDid = 600+elecId.dccId();
      }
      else if(subDet=="EE")
      {
        EEDetId det = EEDetId::unhashIndex(hashIndex);
        EcalElectronicsId elecId = ecalElectronicsMap_->getElectronicsId(det);
        FEDid = 600+elecId.dccId();
      }
      else
        cout << "Couldn't find in EB/EE hash: " << hashIndex << endl;

      if(FEDid > -1)
        fedAvgProfile_->Fill(FEDid,-1*calibConst);
      else
        cout << "Didn't understand hash: " << hashIndex << endl;
    }
  }
  else
  {
    std::cout << "ERROR: Calib file1 not opened." << std::endl;
    return;
  }

  fedAvgProfile_->SetErrorOption("s");
  fedAvgProfile_->SetXTitle("FED");
  fedAvgProfile_->SetYTitle("-1*calibConst [ns]");
}


// ------------ method called once each job just before starting event loop  ------------
void 
EcalTimeCalibGraphs::beginJob(const edm::EventSetup& c)
{
  edm::ESHandle< EcalElectronicsMapping > handle;
  c.get< EcalMappingRcd >().get(handle);
  ecalElectronicsMap_ = handle.product();

}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalTimeCalibGraphs::endJob() {
//  outfile_ = new TFile("ecalTimeCalibGraphs.root","RECREATE");
//  fedAvgProfile_->Write();
//  outfile_->Close();
}

void EcalTimeCalibGraphs::SetEStyle() {
  TStyle* EStyle = new TStyle("EStyle", "E's not Style");

  //set the background color to white
  EStyle->SetFillColor(10);
  EStyle->SetFrameFillColor(10);
  EStyle->SetFrameFillStyle(0);
  EStyle->SetFillStyle(0);
  EStyle->SetCanvasColor(10);
  EStyle->SetPadColor(10);
  EStyle->SetTitleFillColor(0);
  EStyle->SetStatColor(10);

  //dont put a colored frame around the plots
  EStyle->SetFrameBorderMode(0);
  EStyle->SetCanvasBorderMode(0);
  EStyle->SetPadBorderMode(0);

  //use the primary color palette
  EStyle->SetPalette(1,0);
  EStyle->SetNumberContours(255);

  //set the default line color for a histogram to be black
  EStyle->SetHistLineColor(kBlack);

  //set the default line color for a fit function to be red
  EStyle->SetFuncColor(kRed);

  //make the axis labels black
  EStyle->SetLabelColor(kBlack,"xyz");

  //set the default title color to be black
  EStyle->SetTitleColor(kBlack);
   
  // Sizes

  //For Small Plot needs
  //set the margins
 //  EStyle->SetPadBottomMargin(.2);
//   EStyle->SetPadTopMargin(0.08);
//   EStyle->SetPadLeftMargin(0.12);
//   EStyle->SetPadRightMargin(0.12);

//   //set axis label and title text sizes
//   EStyle->SetLabelSize(0.06,"xyz");
//   EStyle->SetTitleSize(0.06,"xyz");
//   EStyle->SetTitleOffset(1.,"x");
//   EStyle->SetTitleOffset(.9,"yz");
//   EStyle->SetStatFontSize(0.04);
//   EStyle->SetTextSize(0.06);
//   EStyle->SetTitleBorderSize(0.5);
  

  //set the margins
  EStyle->SetPadBottomMargin(.15);
  EStyle->SetPadTopMargin(0.08);
  EStyle->SetPadLeftMargin(0.14);
  EStyle->SetPadRightMargin(0.12);
  
  //set axis label and title text sizes
  EStyle->SetLabelSize(0.04,"xyz");
  EStyle->SetTitleSize(0.06,"xyz");
  EStyle->SetTitleOffset(1.,"x");
  EStyle->SetTitleOffset(1.1,"yz");
  EStyle->SetStatFontSize(0.04);
  EStyle->SetTextSize(0.04);
  EStyle->SetTitleBorderSize(Width_t(0.5));
  //EStyle->SetTitleY(0.5);
  
  //set line widths
  EStyle->SetHistLineWidth(1);
  EStyle->SetFrameLineWidth(2);
  EStyle->SetFuncWidth(2);

  //Paper Size
  EStyle->SetPaperSize(TStyle::kUSLetter);

  // Misc

  //align the titles to be centered
  //Style->SetTextAlign(22);

  //set the number of divisions to show
  EStyle->SetNdivisions(506, "xy");

  //turn off xy grids
  EStyle->SetPadGridX(0);
  EStyle->SetPadGridY(0);

  //set the tick mark style
  //EStyle->SetPadTickX(1);
  //EStyle->SetPadTickY(1);

  //show the fit parameters in a box
  EStyle->SetOptFit(111111);

  //turn on all other stats
   //EStyle->SetOptStat(0000000);
  //EStyle->SetOptStat(1111111);
  //With errors
  EStyle->SetOptStat(1112211);

  //Move stats box
  //EStyle->SetStatX(0.85);

  //marker settings
  EStyle->SetMarkerStyle(8);
  EStyle->SetMarkerSize(0.8);
   
  // Fonts
   EStyle->SetStatFont(42);
   EStyle->SetLabelFont(42,"xyz");
   EStyle->SetTitleFont(42,"xyz");
   EStyle->SetTextFont(42);
//  EStyle->SetStatFont(82);
//   EStyle->SetLabelFont(82,"xyz");
//   EStyle->SetTitleFont(82,"xyz");
//   EStyle->SetTextFont(82);


  //done
  EStyle->cd();
}
//define this as a plug-in
DEFINE_FWK_MODULE(EcalTimeCalibGraphs);
