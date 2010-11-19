// Generate Toy Monte Carlo events by hand
// Seth Cooper, created July 13 2010
// Adapted from FORTRAN code provided by Y. Kubota

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "generateToyMCevents.h"

// ROOT includes
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"

std::vector<double> NLLs_;

// Generate a random eta value based on gluino200 eta PDF
// Assumes random number generator has already been seeded and histogram initialized
double generateRandomEtaGluino200()
{
  // Use gluino200 hist (defined in .h)
  double maxBinContent = 0;
  maxBinContent = gluino200EtaHist_->GetBinContent(gluino200EtaHist_->GetMaximumBin());
  if(maxBinContent==0)
  {
    std::cout << "Normalization not correct for gluino200 Eta hist.  Exiting." << std::endl;
    std::exit(-5);
  }
  double scaling = maxBinContent*1.001;

  int accepted = 0;
  double etaX = -999;
  while(accepted < 1)
  {
    etaX = rand()/(float)RAND_MAX; // to generate eta
    double acceptEtax = rand()/(float)RAND_MAX; // test for acceptance

    // Rescale etaX so that it is in the range
    double etaMin = gluino200EtaHist_->GetXaxis()->GetXmin();
    double histRange = gluino200EtaHist_->GetXaxis()->GetXmax()-etaMin;
    int numBins = gluino200EtaHist_->GetNbinsX();
    double binWidth = histRange/numBins;
    etaX = etaX*histRange + etaMin;
    double etaPdfVal = gluino200EtaHist_->GetBinContent(1+(int)round((etaX-etaMin)/binWidth));

    // Rescale acceptance point
    acceptEtax*=scaling;
    //std::cout << "DEBUG: " << " etaX " << etaX << " etaPdfVal " << etaPdfVal <<
    //  " acceptEtaX " << acceptEtax << std::endl;
    if(acceptEtax > etaPdfVal)
      continue;
    else
      accepted++; // same as break here
  }

  return etaX;
}


// Generate a random Pt value based on gluino200 Pt PDF
// Assumes random number generator has already been seeded and histogram initialized
double generateRandomPtGluino200()
{
  // Use gluino200 hist (defined in .h)
  double maxBinContent = 0;
  maxBinContent = gluino200PtHist_->GetBinContent(gluino200PtHist_->GetMaximumBin());
  if(maxBinContent==0)
  {
    std::cout << "Normalization not correct for gluino200 Pt hist.  Exiting." << std::endl;
    std::exit(-5);
  }
  double scaling = maxBinContent*1.001;

  int accepted = 0;
  double ptX = -999;
  while(accepted < 1)
  {
    ptX = rand()/(float)RAND_MAX; // to generate Pt
    double acceptPtx = rand()/(float)RAND_MAX; // test for acceptance

    // Rescale ptX so that it is in the range
    double histRange = gluino200PtHist_->GetXaxis()->GetXmax()-gluino200PtHist_->GetXaxis()->GetXmin();
    int numBins = gluino200PtHist_->GetNbinsX();
    double binWidth = histRange/numBins;
    ptX = ptX*histRange + gluino200PtHist_->GetXaxis()->GetXmin();
    double ptPdfVal = gluino200PtHist_->GetBinContent(1+(int)round(ptX/binWidth));

    // Rescale acceptance point
    acceptPtx*=scaling;
    if(acceptPtx > ptPdfVal)
      continue;
    else
      accepted++; // same as break here
  }

  return ptX;
}

int main()
{
  using namespace std;

  // Define variables
  double dEdxScale; // dEdx units: MeV/cm
  double dEdxMin = 0, dEdxRange = 220.0, dEdxMaxY = 0;
  double timeScale; // time units: ns
  double timeMin = -12.5, timeRange = 100.0;
  double dEdxMaxYNull = 0;
  int nEvents = 3;
  //lossFac = 25;
  int numTrials = 1000;
  int nAccept;

  // Seed the (pseudo) random number generator
  srand(time(NULL));
  initializeGluino200PtHist();
  initializeGluino200EtaHist();

  // Set up output file and book histograms
  TFile output("toyMCplots.root","RECREATE");
  output.cd();

  TH1F* NLLHist = new TH1F("NLLHist","-Log(likelihood) null hypothesis",1500,-50,100);
  TH1F* NLLGenPDFHist = new TH1F("NLLGenPDFHist","-Log(likelihood) generation PDF",1500,-50,100);
  TH2F* NLLvsAvgPtEvtHist = new TH2F("NLLvsAvgPtInEvent","NLL vs. avg. Pt in event;GeV",500,0,2000,1500,-50,100);
  TH2F* NLLvsMinPtEvtHist = new TH2F("NLLvsMinPtInEvent","NLL vs. min. Pt in event;GeV",500,0,2000,1500,-50,100);
  TH2F* NLLvsMaxPtEvtHist = new TH2F("NLLvsMaxPtInEvent","NLL vs. max. Pt in event;GeV",500,0,2000,1500,-50,100);
  TH2F* timingNLLvsPtHist = new TH2F("timingNLLvsPt","Timing NLL vs. Pt of candidate (dEdx > 20 MeV/cm);GeV",500,0,2000,1500,-50,100);
  TH2F* timingNLLmeanVsPtHist = new TH2F("timingNLLmeanVsPt","Timing NLL (mean val) vs. Pt of candidate (dEdx > 20 MeV/cm);GeV",500,0,2000,1500,-50,100);
  TH2F* timingNLLmeanP1SigmaVsPtHist = new TH2F("timingNLLmeanP1SigmaVsPt","Timing NLL (mean+1#sigma) vs. Pt of candidate (dEdx > 20 MeV/cm);GeV",500,0,2000,1500,-50,100);
  TH2F* timingNLLmeanM1SigmaVsPtHist = new TH2F("timingNLLmeanM1SigmaVsPt","Timing NLL (mean - 1#sigma) vs. Pt of candidate (dEdx > 20 MeV/cm);GeV",500,0,2000,1500,-50,100);
  TH2F* dEdxNLLvsPtHist = new TH2F("dEdxNLLvsPt","dE/dx NLL vs. Pt*(e^eta+e^-eta)^2 of candidate (dE/dx > 20 MeV/cm);GeV",500,0,2000,1500,-50,100);
  TH2F* delayVsPtHist = new TH2F("delayVsPt","Delay vs. Pt of candidate;GeV;ns",500,0,2000,500,0,50);
  TH1F* dEdxPdfHist = new TH1F("dEdxPdfHist","Generated dE/dx PDF points;MeV/cm",221,0,440);
  TH1F* timePdfHist = new TH1F("timePdfHist","Generated time PDF points;ns",250,-12.5,12.5);
  TH1F* timingSigmasUsedHist = new TH1F("timingSigmasUsed","Sigma of timing gauss used for gen;ns",250,0,25);
  TH1F* generatedGluino200PtHist_ = new TH1F("genGluino200Pt","Pt of generated points, gluino 200 GeV;GeV",500,0,2000);
  TH1F* generatedGluino200EtaHist_ = new TH1F("genGluino200Eta","Eta of generated points, gluino 200 GeV;GeV",100,-5,5);
  TH1F* generatedPathLengthHist_ = new TH1F("genPathLength","Generated Path Lengths;m",500,0,5);
  TH1F* generatedBetaHist_ = new TH1F("genBeta","Generated beta",100,0,1);
  TH1F* generatedDelayHist_ = new TH1F("genDelay","Generated delays;ns",500,0,50);
  TH1F* generatedThetaHist_ = new TH1F("genTheta","Generated theta;ns",90,0,TMath::PiOver2());
  TH2F* dEdxVsTimingHist = new TH2F("dEdxVsTiming","Generated dE/dx vs. timing;ns;MeV/cm",250,-12.5,12.5,221,0,440);

  bool isMuon = false;  // Generate muons or HSCPs?
  //XXX PDFs used to GEN for Muon/HSCP below
  TF1* dEdxPdf;
  // Define dE/dx PDF (MeV/cm)
  // For muons (single cry Pt=50 GeV, eta ~ 0.01)
  if(isMuon)
  {
    dEdxPdf = new TF1("dEdxPdf","TMath::Landau(x,[0],[1],0)",dEdxMin,dEdxMin+dEdxRange);
    dEdxPdf->SetParameter(0,12.56); // MPV
    dEdxPdf->SetParameter(1,1.386); // Sigma
  }
  else
  {
    // For gluino200
    //TF1* dEdxPdf = new TF1("dEdxPdf","TMath::Landau(x,[0],[1],0)",dEdxMin,dEdxMin+dEdxRange);
    //dEdxPdf->SetParameter(0,18.65); // MPV
    //dEdxPdf->SetParameter(1,5.947); // Sigma
    // For kkTau300
    dEdxPdf = new TF1("dEdxPdf","TMath::Gaus(x,[0],[1],0)",dEdxMin,dEdxMin+dEdxRange);
    //dEdxPdf->SetParameter(0,22.99); // mean, set this based on the beta later
    dEdxPdf->SetParameter(1,2.127); // sigma
  }
  // Define time PDF (ns)
  TF1* timePdf;
  // For muons (single cry Pt=50 GeV, eta ~ 0.01)
  if(isMuon)
  {
    timePdf = new TF1("timePdf","TMath::Gaus(x,[0],[1],0)",timeMin,timeMin+timeRange);
    //timePdf->SetParameter(1,1); // sigma, set this later based on dE/dx
  }
  else
  {
    // For HSCP
    timePdf = new TF1("timePdf","TMath::Gaus(x,[0],[1],0)",timeMin,timeMin+timeRange);
    //timePdf->SetParameter(0,6); // mean, set this later based on kinematic PDFs
    //timePdf->SetParameter(1,1); // sigma, set this later based on dE/dx
  }
  //XXX End of section for GEN PDFs


  // Null hypothesis PDFs
  TF1* dEdxPdfNull = new TF1("dEdxPdfNull","TMath::Landau(x,[0],[1],0)",dEdxMin,dEdxMin+dEdxRange);
  dEdxPdfNull->SetParameter(0,12.56); // MPV
  dEdxPdfNull->SetParameter(1,1.386); // Sigma
  // Find max value of dEdxPDF (in range dEdxMin to dEdxMin+dEdxRange)
  dEdxMaxYNull = dEdxPdfNull->GetMaximum(dEdxMin,dEdxMin+dEdxRange);
  if(dEdxMaxYNull==0)
  {
    cout << "dE/dx null PDF Value stayed negative.  Exiting. " << endl;
    return -3;
  }
  TF1* timePdfNull = new TF1("timePdfNull","TMath::Gaus(x,[0],[1],0)",timeMin,timeMin+timeRange);
  timePdfNull->SetParameter(0,0); // mean
  //timePdfNull->SetParameter(1,1); // sigma, set this later based on dE/dx

  for(int i=0; i < numTrials; ++i)
  {
    std::vector<double> dEdxEvents;
    std::vector<double> timeEvents;
    std::vector<double> delays;
    std::vector<double> betas;
    std::vector<double> etas;
    std::vector<double> pts;
    dEdxEvents.reserve(nEvents);
    timeEvents.reserve(nEvents);
    delays.reserve(nEvents);
    betas.reserve(nEvents);
    etas.reserve(nEvents);
    pts.reserve(nEvents);


    //XXX Start event generation
    nAccept = 0;
    //for(int j=0; j < nEvents*lossFac && nAccept < nEvents; ++j)
    for(int j=0; nAccept < nEvents; ++j)
    {
      // Generate pair of random numbers
      double x1dEdx = rand()/(float)RAND_MAX; // dE/dx
      double x1time = rand()/(float)RAND_MAX; // time
      double y2dEdx = rand()/(float)RAND_MAX; // test for acceptance dE/dx
      double y2time = rand()/(float)RAND_MAX; // test for acceptance time

      double delay = 0;
      double beta = 1;
      double eta = -10;
      double pt = -1;
      if(!isMuon)
      {
        // Gluino Pt
        double gluino200Pt = generateRandomPtGluino200(); // GeV
        // Gluino Eta
        double gluino200Eta = generateRandomEtaGluino200();
        // Gluino Mass
        double mass = 200.; // GeV
        // Calculate quantities from gluino gen points
        double theta = 2*atan(exp(-1*gluino200Eta));
        if(theta > TMath::PiOver2())
          theta = TMath::Pi() - theta;
        double pathLength = 1.5/sin(theta); // in meters
        double p = gluino200Pt/sin(theta); // GeV
        beta = p/sqrt(p*p+mass*mass);
        delay = pathLength/(beta*0.29979)-pathLength/0.29979; // ns
        eta = gluino200Eta;
        pt = gluino200Pt;

        // Cut on 10 GeV of Pt at least
        if(gluino200Pt < 10)
          continue;
        // Require it to be in the barrel
        if(fabs(gluino200Eta) > 1.6)
          continue;
        // Cut on generated beta
        if(beta <= 0.2)
          continue;

        //cout << "DEBUG: " << "Generated beta: " << beta << " eta: " << gluino200Eta
        //  << " Theta (deg): " << theta*180/3.141592 << " pathLength: " << pathLength
        //  << " Pt: " << gluino200Pt << " delay: "
        //  << delay <<  " beta: " << beta << endl;
        generatedGluino200PtHist_->Fill(gluino200Pt);
        generatedGluino200EtaHist_->Fill(gluino200Eta);
        generatedPathLengthHist_->Fill(pathLength);
        generatedBetaHist_->Fill(beta);
        generatedDelayHist_->Fill(delay);
        generatedThetaHist_->Fill(theta);
      }

      // Set dE/dx mean parameter based on beta if HSCP
      if(!isMuon)
        dEdxPdf->SetParameter(0,7.08/(beta*beta)); // mean
      // Find max value of dEdxPDF (in range dEdxMin to dEdxMin+dEdxRange)
      dEdxMaxY = dEdxPdf->GetMaximum(dEdxMin,dEdxMin+dEdxRange);
      if(dEdxMaxY==0)
      {
        cout << "dE/dx PDF Value stayed negative.  Exiting. " << endl;
        return -1;
      }
      // Scale dEdxMaxY to make sure PDF max is smaller than max of random numbers
      dEdxScale = dEdxMaxY*1.001;
      // Rescale x so that it is in the range of dE/dx
      double xdEdx = dEdxMin+dEdxRange*x1dEdx; // Generated dE/dx value
      double dEdxPdfVal = dEdxPdf->Eval(xdEdx); 

      // Set time sigma parameter based on dE/dx value and time res
      timePdf->SetParameter(1,61/xdEdx);
      if(isMuon)
        timePdf->SetParameter(0,0); // mean
      else
        timePdf->SetParameter(0,delay);
      // Renormalize PDF
      double timeMaxY = 0;
      timeMaxY = timePdf->GetMaximum(timeMin,timeMin+timeRange);
      if(timeMaxY==0)
      {
        cout << "time PDF Value stayed negative.  Exiting. " << endl;
        return -2;
      }
      // Scale timeMaxY to make sure PDF max is smaller than max of random numbers
      timeScale = timeMaxY*1.001;
      // Rescale x2
      double x2time = timeMin+timeRange*x1time; // Generated time value
      double timePdfVal = timePdf->Eval(x2time); 

      // Rescale y2dEDx so that the max is ~ dEDxPDFmax
      y2dEdx*=dEdxScale;
      if(y2dEdx > dEdxPdfVal)
        continue;

      // Rescale y2time so that the max is ~ timePDFmax
      y2time*=timeScale;
      if(y2time > timePdfVal)
        continue;

      // Accept event
      nAccept++;

      dEdxEvents.push_back(xdEdx);
      timeEvents.push_back(x2time);
      betas.push_back(beta);
      delays.push_back(delay);
      etas.push_back(eta);
      pts.push_back(pt);
      timingSigmasUsedHist->Fill(61/xdEdx);
    }

    // Calculate likelihood
    // dE/dx * time
    double likelihood = 0;
    double likelihoodGen = 0;
    vector<double>::const_iterator timeItr = timeEvents.begin();
    vector<double>::const_iterator etaItr = etas.begin();
    vector<double>::const_iterator ptItr = pts.begin();
    vector<double>::const_iterator delayItr = delays.begin();
    for(vector<double>::const_iterator dedxItr = dEdxEvents.begin();
        dedxItr != dEdxEvents.end(); ++dedxItr)
    {
      // Set time sigma parameter based on dE/dx value
      timePdfNull->SetParameter(1,61/(*dedxItr));
      // Renormalize timing PDF
      double timeMaxYNull = 0;
      timeMaxYNull = timePdfNull->GetMaximum(timeMin,timeMin+timeRange);
      if(timeMaxYNull==0)
      {
        cout << "time null PDF Value stayed negative.  Exiting. " << endl;
        return -4;
      }
      // Set time sigma parameter based on dE/dx value (gen PDF)
      timePdf->SetParameter(1,61/(*dedxItr));
      // Renormalize timing PDF
      double timeMaxY = 0;
      timeMaxY = timePdf->GetMaximum(timeMin,timeMin+timeRange);
      if(timeMaxY==0)
      {
        cout << "time PDF Value stayed negative.  Exiting. " << endl;
        return -4;
      }

      delayVsPtHist->Fill(*ptItr,*delayItr);
      likelihood+=-1*log((dEdxPdfNull->Eval(*dedxItr)*timePdfNull->Eval(*timeItr))/(dEdxMaxYNull*timeMaxYNull));
      likelihoodGen+=-1*log((dEdxPdf->Eval(*dedxItr)*timePdf->Eval(*timeItr))/(dEdxMaxY*timeMaxY));
      if(*dedxItr > 20)
      {
        timingNLLvsPtHist->Fill(*ptItr,-1*log(timePdfNull->Eval(*timeItr)/timeMaxYNull));
        timingNLLmeanVsPtHist->Fill(*ptItr,-1*log(timePdfNull->Eval(*delayItr)/timeMaxYNull));
        timingNLLmeanP1SigmaVsPtHist->Fill(*ptItr,-1*log(timePdfNull->Eval(*delayItr+61/(*dedxItr))/timeMaxYNull));
        timingNLLmeanM1SigmaVsPtHist->Fill(*ptItr,-1*log(timePdfNull->Eval(*delayItr-61/(*dedxItr))/timeMaxYNull));
        double ptMult = (exp(*etaItr)+exp(-1*(*etaItr)))/2;
        dEdxNLLvsPtHist->Fill((*ptItr)*ptMult,-1*log(dEdxPdfNull->Eval(*dedxItr)/dEdxMaxYNull));
      }
      //more info logging
      cout << " Event dEdx NLL: " << -1*log(dEdxPdfNull->Eval(*dedxItr)/dEdxMaxYNull)
        << " time NLL: " << -1*log(timePdfNull->Eval(*timeItr)/timeMaxYNull)
        << " event total NLL: "
        << -1*log((dEdxPdfNull->Eval(*dedxItr)*timePdfNull->Eval(*timeItr))/(dEdxMaxYNull*timeMaxYNull))
        << endl;
      timeItr++;
      etaItr++;
      ptItr++;
      ++delayItr;
    }
    // Fill example histograms
    if(dEdxPdfHist->GetEntries()==0)
    {
      vector<double>::const_iterator timeItr = timeEvents.begin();
      for(vector<double>::const_iterator dedxItr = dEdxEvents.begin();
          dedxItr != dEdxEvents.end(); ++dedxItr)
      {
        dEdxPdfHist->Fill(*dedxItr);
        timePdfHist->Fill(*timeItr);
        dEdxVsTimingHist->Fill(*timeItr,*dedxItr);
        timeItr++;
      }
    }

    cout << "-LL: " << likelihood << endl;
    NLLs_.push_back(likelihood);
    NLLHist->Fill(likelihood);
    NLLGenPDFHist->Fill(likelihoodGen);
    // Get info on events hard to distinguish from muons
    if(!isMuon)
    {
      float sumPt = 0;
      vector<double>::const_iterator betaItr = betas.begin();
      vector<double>::const_iterator etaItr = etas.begin();
      vector<double>::const_iterator delayItr = delays.begin();
      vector<double>::const_iterator ptItr = pts.begin();
      vector<double>::const_iterator timeItr = timeEvents.begin();
      if(likelihood < 6)
        std::cout << " Event likelihood: " << likelihood << " details: " << std::endl;
      for(vector<double>::const_iterator dedxItr = dEdxEvents.begin();
          dedxItr != dEdxEvents.end(); ++dedxItr)
      {
        if(likelihood < 6)
        {
          std::cout << "\tPoint beta: " << *betaItr << " eta: " << *etaItr
            << " delay: " << *delayItr << " pt: " << *ptItr
            << " dE/dx: " << *dedxItr << " time: " << *timeItr << std::endl;
        }
        sumPt+=(*ptItr);
        pts.push_back(*ptItr);
        timeItr++;
        betaItr++;
        etaItr++;
        delayItr++;
        ptItr++;
      }
      NLLvsAvgPtEvtHist->Fill(sumPt/nEvents,likelihood);
      NLLvsMinPtEvtHist->Fill(*min_element(pts.begin(),pts.end()),likelihood);
      NLLvsMaxPtEvtHist->Fill(*max_element(pts.begin(),pts.end()),likelihood);
    }
  }

  NLLHist->Write();
  NLLGenPDFHist->Write();
  NLLvsAvgPtEvtHist->Write();
  NLLvsMinPtEvtHist->Write();
  NLLvsMaxPtEvtHist->Write();
  timingNLLvsPtHist->Write();
  timingNLLmeanVsPtHist->Write();
  timingNLLmeanP1SigmaVsPtHist->Write();
  timingNLLmeanM1SigmaVsPtHist->Write();
  dEdxNLLvsPtHist->Write();
  delayVsPtHist->Write();
  dEdxPdfHist->Write();
  timePdfHist->Write();
  dEdxVsTimingHist->Write();
  timingSigmasUsedHist->Write();
  generatedGluino200PtHist_->Write();
  generatedGluino200EtaHist_->Write();
  generatedPathLengthHist_->Write();
  generatedBetaHist_->Write();
  generatedDelayHist_->Write();
  generatedThetaHist_->Write();
  output.Close();

  return 0;
}
