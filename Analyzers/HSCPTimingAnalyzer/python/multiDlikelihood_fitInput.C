#include "multiDlikelihood.h"

//XXX: Main
int main(int argc, char* argv[])
{
  using namespace std;

  stringstream roofitstream;

  // parse arguments and load data
  parseArguments(argc, argv);

  SetEStyle();
  gStyle->SetMarkerSize(0.5);
  gStyle->SetStatX(0.4);
  gStyle->SetStatW(0.3);


  // *********** Set bins
  //crystalsumEnergy.setBins(125);
  //maxCryTime.setBins(100);
  //sumDeDx.setBins(250);


  // *********************** Load dataset **************************************
  RooDataSet* dedxTimeChi2TrackLengthData = new RooDataSet("dedxTimeChi2Data","dedxChi2TimeData",energyTimeTNtuple_,
      RooArgSet(sumDeDx,maxCryTime,maxCryTimeError,sumEnergy,sumTrackLength));

  //cout << "Number of entries in initial dataset is: " << dedxmaxCryTimeChi2TrackLengthData->numEntries() << endl;
  ////TCut chi2Cut1 = "chi2 < 0.96";
  TCut chi2Cut2 = "maxCryTimeError > 0";
  RooDataSet* dedxTimeChi2CutData = (RooDataSet*) dedxTimeChi2TrackLengthData->reduce(chi2Cut2);
  //cout << "Number of entries after chi2 > 0 cut is: " << dedxmaxCryTimeChi2CutData->numEntries() << endl;
  ////TCut cut1 = "sumEnergy > 0.4 || maxCryTime < 15";
  ////TCut sumEnergyCut1 = "crystalsumEnergy > 0.4";
  //TCut sumTrackLengthCut = "sumTrackLength > 23.05 && sumTrackLength < 23.15";
  ////RooDataSet* dedxTimeChi2CutTLCutData = (RooDataSet*) dedxTimeChi2CutData->reduce(sumTrackLengthCut);
  ////cout << "Number of entries after sumTrackLength cut is: " << dedxTimeChi2CutTLCutData->numEntries() << endl;
  ////RooDataSet* dedxTimesumEnergyCutData = (RooDataSet*) dedxTimeDataChi2cut->reduce(sumEnergyCut1);
  //// Dataset with only dE/dx, maxCryTime
  ////RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimesumEnergyCutData,RooArgSet(sumDeDx,maxCryTime));
  //RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeChi2CutData,RooArgSet(sumDeDx,maxCryTime));//chi2 cut only
  ////RooDataSet* dedxTimeData = new RooDataSet("dedxTimeData","dedxTimeData",dedxTimeChi2CutTLCutData,RooArgSet(sumDeDx,maxCryTime));//chi2+tkLength cuts

  RooDataSet* dedxTimeData = dedxTimeChi2CutData;

  //// 1-D datasets
  RooDataSet* dedxData = new RooDataSet("dedxData","dedxData",dedxTimeData,RooArgSet(sumDeDx));
  RooDataSet* timeData = new RooDataSet("timeData","timeData",dedxTimeData,RooArgSet(maxCryTime));

  int numEntries = dedxTimeData->numEntries();
  cout << "****************Created RooDataSet with " << numEntries << " entries!" << endl;
  if(numEntries < 10)
  {
    cout << "Number of entries in the dataset is too few to continue. " << endl;
    return -3;
  }

  int autoPrecNum = 3;
  RooFitResult *fitResult = 0;

  // 1-D fits
  //std::cout << "Performing 1-D fits..." << std::endl;
  //RooNLLVar nllTiming("nllTiming","nllTiming",timeModel1D,*timeData,DataError(RooAbsData::SumW2));
  //RooMinuit mTiming(nllTiming);
  //mTiming.setErrorLevel(0.2);
  //mTiming.setStrategy(2);
  //mTiming.hesse();
  ////mTiming.setVerbose(true);
  //mTiming.optimizeConst(true);
  //mTiming.setProfile(true);
  //mTiming.migrad();
  ////mTiming.improve();
  ////mTiming.hesse();
  //mTiming.hesse();
  //mTiming.minos();
  timeModel1D.fitTo(*timeData);
  //
  //RooNLLVar nllDeDx("nllDeDx","nllDeDx",dedxModel1D,*dedxData,DataError(RooAbsData::SumW2));
  //RooMinuit mDeDx(nllDeDx);
  //mDeDx.setErrorLevel(0.001);
  //mDeDx.setStrategy(2);
  //mDeDx.hesse();
  ////mDeDx.setVerbose(true);
  ////mDeDx.optimizeConst(true);
  ////mDeDx.setProfile(true);
  //mDeDx.migrad();
  ////mDeDx.improve();
  //mDeDx.hesse();
  ////mDeDx.hesse();
  //mDeDx.minos();
  dedxModel1D.fitTo(*dedxData);

  // Fit, maximizing signal fraction
  //RooFitResult* fitResult =  dedxAndTimeModel.fitTo(*dedxTimeData,Save(),SumW2Error(true));
  // Background fit
  dedxAndTimeBackModel.fitTo(*dedxTimeData);

  // Custom fitting stuff -- 2D fits
  //RooAbsData::ErrorType fitError = RooAbsData::SumW2;
  //RooAbsData::ErrorType fitError = RooAbsData::Poisson;
  //RooNLLVar nll("nll","nll",dedxAndTimeModel,*dedxTimeData,ConditionalObservables(maxCryTime),DataError(RooAbsData::SumW2));
  RooNLLVar nll("nll","nll",dedxAndTimeModelReal,*dedxTimeData,DataError(RooAbsData::SumW2));
  RooMinuit m(nll);
  m.setErrorLevel(0.2);
  m.setStrategy(2);
  m.hesse();
  //m.setVerbose(true);
  m.optimizeConst(true);
  m.setProfile(true);
  m.migrad();
  //m.improve();
  //m.hesse();
  m.hesse();
  m.minos();
  fitResult = m.save();
  //#if ROOT_VERSION_CODE <= ROOT_VERSION(5,19,0)
  //    fitResult->defaultStream(&roofitstream);
  //#else
  //    fitResult->defaultPrintStream(&roofitstream);
  //#endif
  //    fitResult->Print("v");
  //    cout << roofitstream.str();
  //    roofitstream.str(std::string(""));

  // Fit the hist PDF
  //cout << "************** Fitting the hist PDF" << endl;
  //fullHistPdf.fitTo(*dedxTimeChi2TrackLengthData);

  //TODO: fix this background model fitting issue...
  // Well, you can't fit a function with fixed parameters...
  //energyAndTimeBackModel.fitTo(*energyTimeData);
  //backEnergyModel1D.fitTo(*energyData);
  // backTimeModel1D.fitTo(*timeData);

  cout << "************** Fitting done, now time to plot." << endl;
  TFile* outputFile = new TFile("likelihoodOutput_fitInput.root","recreate");
  outputFile->cd();

  //// HistPDF plot
  //TCanvas* combinedCanvasPDF = new TCanvas("combinedCanvasPDF","combinedCanvasPDF",1,1,2500,1000);
  //combinedCanvasPDF->Divide(2,1);
  //combinedCanvasPDF->cd(1);
  ////Plot energy curves
  //RooPlot* dedxFramePDF = sumDeDx.frame();
  ////dedxTimeChi2TrackLengthData->plotOn(dedxFramePDF);
  //dedxTimeData->plotOn(dedxFram
  ////fullHistPdf.plotOn(dedxFramePDF);
  ////fullHistPdf.plotOn(dedxFrame,Components("gaussianDeDxHSCP"),LineStyle(kDashed),LineColor(2));
  ////fullHistPdf.paramOn(dedxFramePDF,Layout(0.35,0.8,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  ////dedxAndTimeModel.plotOn(dedxFrame,LineStyle(kDashed),LineColor(2));
  //dedxFramePDF->Draw("e0");
  //combinedCanvasPDF->cd(2);
  ////Plot time curves
  //RooPlot* timeFramePDF = maxCryTime.frame();
  ////dedxTimeChi2TrackLengthData->plotOn(timeFramePDF);
  ////fullHistPdf.plotOn(timeFramePDF);
  ////fullHistPdf.plotOn(timeFramePDF,Components("gaussianTimeHSCP"),LineStyle(kDashed),LineColor(2));
  //fullHistPdf.paramOn(timeFramePDF,Layout(0.25,0.75,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //timeFramePDF->Draw("e0");
  //combinedCanvasPDF->Write();


  //float sigFracFit = sigFrac.getVal();
  TCanvas* combinedCanvas = new TCanvas("combinedCanvas","combinedCanvas",1,1,2500,1000);
  combinedCanvas->Divide(3,1);
  combinedCanvas->cd(1);
  //Plot energy curves
  RooPlot* dedxFrame = sumDeDx.frame();
  dedxTimeData->plotOn(dedxFrame);
  dedxAndTimeModel.plotOn(dedxFrame);
  dedxAndTimeModel.plotOn(dedxFrame,Components("gaussianDeDxHSCP"),LineStyle(kDashed),LineColor(2));
  dedxAndTimeModel.paramOn(dedxFrame,Layout(0.35,0.8,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //sigFrac = 0.516;
  //dedxAndTimeModel.plotOn(dedxFrame,LineStyle(kDashed),LineColor(2));
  dedxFrame->Draw("e0");
  double ModelChiSquare2D = dedxFrame->chiSquare();
  TPaveLabel *t1 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquare2D));
  dedxFrame->addObject(t1);
  combinedCanvas->cd(2);
  //Plot time curves
  //sigFrac = sigFracFit;
  RooPlot* timeFrame = maxCryTime.frame();
  dedxTimeData->plotOn(timeFrame,Name("data"));
  dedxAndTimeModel.plotOn(timeFrame);
  dedxAndTimeModel.plotOn(timeFrame,Components("gaussianTimeHSCP"),LineStyle(kDashed),LineColor(2));
  dedxAndTimeModel.paramOn(timeFrame,Layout(0.25,0.75,0.92),Format("NEU",AutoPrecision(autoPrecNum)));
  //sigFrac = 0.516;
  //dedxAndTimeModel.plotOn(timeFrame,LineStyle(kDashed),LineColor(2));
  timeFrame->Draw("e0");
  double ModelChiSquareT2D = dedxFrame->chiSquare();
  TPaveLabel *t2 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquareT2D));
  dedxFrame->addObject(t2);

  combinedCanvas->cd(3);
  //testing
  //RooPlot* energyFrame = crystalEnergy.frame();
  //dedxTimeEnergyCutData->plotOn(energyFrame);
  //energyFrame->Draw("e0");
  //Make 2-D plot of PDF
  TH1* hh_model = dedxAndTimeModel.createHistogram("hh_model",sumDeDx,Binning(50),YVar(maxCryTime,Binning(50))) ;
  hh_model->SetLineColor(kBlue);
  //TCanvas* modelCanvas = new TCanvas("2DCanvas","2DCanvas",600,600);
  //modelCanvas->cd();
  hh_model->Draw("surf");
  //energyTimeData->Draw("energy");
  //energyTimeData->Draw("maxCryTime");
  //Print
  //combinedCanvas->Print("plotLikelihoods.png");
  combinedCanvas->Write();
  //modelCanvas->Print("plotLikelihoods2D_data.png");
  
  // Back only model
  TCanvas* combinedCanvasBack = new TCanvas("combinedCanvasBack","combinedCanvasBack",1,1,2500,1000);
  combinedCanvasBack->Divide(3,1);
  combinedCanvasBack->cd(1);
  //Plot energy curves
  RooPlot* dedxFrameBack = sumDeDx.frame();
  dedxTimeData->plotOn(dedxFrameBack);
  dedxAndTimeBackModel.plotOn(dedxFrameBack);
  //dedxAndTimeBackModel.plotOn(dedxFrameBack,Components("landauDeDxMuonBack"),LineStyle(kDashed));
  dedxAndTimeBackModel.paramOn(dedxFrameBack,Layout(0.175,0.95,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  dedxFrameBack->Draw("e0");
  combinedCanvasBack->cd(2);
  //Plot time curves
  RooPlot* timeFrameBack = maxCryTime.frame();
  dedxTimeData->plotOn(timeFrameBack);
  dedxAndTimeBackModel.plotOn(timeFrameBack);
  //dedxAndTimeBackModel.plotOn(timeFrameBack,Components("gaussianTimeMuonPBack"),LineStyle(kDashed));
  dedxAndTimeBackModel.paramOn(timeFrameBack,Layout(0.175,0.95,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  timeFrameBack->Draw("e0");
  combinedCanvasBack->cd(3);
  TH1* hh_modelBack = dedxAndTimeBackModel.createHistogram("hh_modelBack",sumDeDx,Binning(50),YVar(maxCryTime,Binning(50))) ;
  hh_modelBack->SetLineColor(kBlue);
  hh_modelBack->Draw("surf");
  //combinedCanvasBack->Print("plotLikelihoodsBack.png");
  combinedCanvasBack->Write();

  // Signal only model
  //TCanvas* combinedCanvasSignal = new TCanvas("combinedCanvasSignal","combinedCanvasSignal",1,1,2500,1000);
  //combinedCanvasSignal->Divide(3,1);
  //combinedCanvasSignal->cd(1);
  ////Plot energy curves
  //RooPlot* dedxFrameSignal = sumDeDx.frame();
  //dedxTimeData->plotOn(dedxFrameSignal);
  //dedxAndTimeSignalModel.plotOn(dedxFrameSignal);
  ////dedxAndTimeSignalModel.plotOn(dedxFrameSignal,Components("gaussianDeDxMuonSignal"),LineStyle(kDashed));
  //dedxAndTimeSignalModel.paramOn(dedxFrameSignal,Layout(0.25,0.75,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  //dedxFrameSignal->Draw("e0");
  //combinedCanvasSignal->cd(2);
  ////Plot time curves
  //RooPlot* timeFrameSignal = maxCryTime.frame();
  //dedxTimeData->plotOn(timeFrameSignal);
  //dedxAndTimeSignalModel.plotOn(timeFrameSignal);
  ////dedxAndTimeSignalModel.plotOn(timeFrameSignal,Components("gaussianTimeHSCPSignal"),LineStyle(kDashed));
  //dedxAndTimeSignalModel.paramOn(timeFrameSignal,Layout(0.1,0.7,0.9),ShowConstants(true),Format("NEU",AutoPrecision(autoPrecNum)));
  //timeFrameSignal->Draw("e0");
  //combinedCanvasSignal->cd(3);
  //TH1* hh_modelSignal = dedxAndTimeSignalModel.createHistogram("hh_modelSignal",sumDeDx,Binning(50),YVar(maxCryTime,Binning(50))) ;
  //hh_modelSignal->SetLineColor(kBlue);
  //hh_modelSignal->Draw("surf");
  ////combinedCanvasSignal->Print("plotLikelihoodsSignal.png");
  //combinedCanvasSignal->Write();

  // 1-D plots
  TCanvas* combinedCanvas1D = new TCanvas("combinedCanvas1D","combinedCanvas1D",1,1,1800,2000);
  combinedCanvas1D->Divide(1,2);
  combinedCanvas1D->cd(1);
  RooPlot* dedxFrame2 = sumDeDx.frame();
  dedxData->plotOn(dedxFrame2);
  dedxModel1D.plotOn(dedxFrame2);
  dedxModel1D.plotOn(dedxFrame2,Components("gaussianDeDx1D"),LineStyle(kDashed),LineColor(2));
  dedxModel1D.paramOn(dedxFrame2,Layout(0.55));
  dedxFrame2->Draw("e0");
  double ModelChiSquare1D = dedxFrame2->chiSquare();
  TPaveLabel *t3 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquare1D));
  dedxFrame2->addObject(t3);
  combinedCanvas1D->cd(2);
  RooPlot* timeFrame2 = maxCryTime.frame();
  timeData->plotOn(timeFrame2);
  timeModel1D.plotOn(timeFrame2);
  timeModel1D.plotOn(timeFrame2,Components("gaussianTimeHSCP1D"),LineStyle(kDashed),LineColor(2));
  timeModel1D.paramOn(timeFrame2,Layout(0.7));
  timeFrame2->Draw("e0");
  double ModelChiSquareT1D = timeFrame2->chiSquare();
  TPaveLabel *t4 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquareT1D));
  dedxFrame->addObject(t4);
  //Print
  //combinedCanvas1D->Print("plotLikelihoods1D.png");
  combinedCanvas1D->Write();

  langausDeDxBack.fitTo(*dedxData);
  gaussianTimeMuonBack.fitTo(*timeData);

  // 1-D plots -- back only
  TCanvas* combinedCanvas1Dback = new TCanvas("combinedCanvas1Dback","combinedCanvas1Dback",1,1,1800,2000);
  combinedCanvas1Dback->Divide(1,2);
  combinedCanvas1Dback->cd(1);
  RooPlot* dedxFrame3 = sumDeDx.frame();
  dedxData->plotOn(dedxFrame3);
  langausDeDxBack.plotOn(dedxFrame3);
  langausDeDxBack.paramOn(dedxFrame3,Layout(0.55));
  dedxFrame3->Draw("e0");
  double ModelChiSquare1Dback = dedxFrame3->chiSquare();
  TPaveLabel *t5 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquare1Dback));
  dedxFrame3->addObject(t5);
  combinedCanvas1Dback->cd(2);
  RooPlot* timeFrame3 = maxCryTime.frame();
  timeData->plotOn(timeFrame3);
  gaussianTimeMuonBack.plotOn(timeFrame3);
  gaussianTimeMuonBack.paramOn(timeFrame3);
  timeFrame3->Draw("e0");
  double ModelChiSquareT1Dback = timeFrame3->chiSquare();
  TPaveLabel *t6 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquareT1Dback));
  timeFrame3->addObject(t6);
  //Print
  //combinedCanvas1D->Print("plotLikelihoods1D.png");
  combinedCanvas1Dback->Write();


  gaussianDeDxHSCPSignal.fitTo(*dedxData);
  gaussianTimeHSCPSignal1D.fitTo(*timeData);

  // 1-D plots -- signal
  TCanvas* combinedCanvas1Dsignal = new TCanvas("combinedCanvas1Dsignal","combinedCanvas1Dsignal",1,1,1800,2000);
  combinedCanvas1Dsignal->Divide(1,2);
  combinedCanvas1Dsignal->cd(1);
  RooPlot* dedxFrame4 = sumDeDx.frame();
  dedxData->plotOn(dedxFrame4);
  gaussianDeDxHSCPSignal.plotOn(dedxFrame4);
  gaussianDeDxHSCPSignal.paramOn(dedxFrame4,Layout(0.55));
  dedxFrame4->Draw("e0");
  double ModelChiSquare1Dsig = dedxFrame4->chiSquare();
  TPaveLabel *t7 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquare1Dsig));
  dedxFrame4->addObject(t7);
  combinedCanvas1Dsignal->cd(2);
  RooPlot* timeFrame4 = maxCryTime.frame();
  timeData->plotOn(timeFrame4);
  gaussianTimeHSCPSignal1D.plotOn(timeFrame4);
  gaussianTimeHSCPSignal1D.paramOn(timeFrame4);
  timeFrame4->Draw("e0");
  double ModelChiSquareT1Dsig = timeFrame4->chiSquare();
  TPaveLabel *t8 = new TPaveLabel(0.7,0.6,0.9,0.68, Form("#chi^{2} = %f", ModelChiSquareT1Dsig));
  timeFrame4->addObject(t8);
  //Print
  //combinedCanvas1D->Print("plotLikelihoods1D.png");
  combinedCanvas1Dsignal->Write();

  //// Make dE/dx and timing hists -- binned
  //TCanvas* combinedCanvasBinned = new TCanvas("combinedCanvasBinned","combinedCanvasBinned",1,1,2000,1000);
  //combinedCanvasBinned->Divide(2,1);
  //combinedCanvasBinned->cd(1);
  //sumDeDx.setBins(15); // bin dedx var
  //time.setBins(25); // bin time var
  //RooDataHist* modelPDFBinned = dedxAndTimeModel.generateBinned(RooArgSet(sumDeDx,maxCryTime),dedxTimeData->numEntries());
  ////Plot dE/dx curves
  //RooPlot* dedxFrameBinned = sumDeDx.frame();
  //dedxTimeData->plotOn(dedxFrameBinned);
  //modelPDFBinned->plotOn(dedxFrameBinned,LineColor(4));
  //dedxFrameBinned->Draw("e0");
  ////Plot time curves
  //combinedCanvasBinned->cd(2);
  ////RooBinning bins(-25,25,12.5,"bins");
  //RooPlot* timeFrameBinned = maxCryTime.frame();
  //dedxTimeData->plotOn(timeFrameBinned);
  //modelPDFBinned->plotOn(timeFrameBinned,LineColor(4));
  //timeFrameBinned->Draw("e0");
  //combinedCanvasBinned->Write();
  //// Plot binned PDF value / data value in bin
  //TCanvas* combinedCanvasRatio = new TCanvas("combinedCanvasRatio","combinedCanvasRatio",1,1,2000,1000);
  //combinedCanvasRatio->Divide(2,1);
  //combinedCanvasRatio->cd(1);
  //vector<float> ratios;
  //vector<float> ratioErrors;
  //vector<float> binCenters;
  //vector<float> binWidths;
  //TH1* dedxDataHist = dedxTimeData->createHistogram("dedxData",sumDeDx);
  //TH1* timeDataHist = dedxTimeData->createHistogram("timeData",maxCryTime);
  //TH1* dedxPDFHist = modelPDFBinned->createHistogram("dedxPDF",sumDeDx);
  //TH1* timePDFHist = modelPDFBinned->createHistogram("timePDF",maxCryTime);
  //for(int i=1; i < dedxDataHist->GetNbinsX()-1; ++i)
  //{
  //  binCenters.push_back(dedxDataHist->GetBinCenter(i));
  //  binWidths.push_back(dedxDataHist->GetBinWidth(i));
  //  if(dedxPDFHist->GetBinContent(i) != 0)
  //    ratios.push_back(dedxDataHist->GetBinContent(i)/dedxPDFHist->GetBinContent(i));
  //  else
  //    ratios.push_back(0);
  //  ratioErrors.push_back(0.5*dedxDataHist->GetBinError(i)/dedxDataHist->GetBinContent(i));
  //  //cout << "ratios: " << dedxPDFHist->GetBinContent(i)/dedxDataHist->GetBinContent(i)
  //  //  << " binCenter: " << dedxDataHist->GetBinCenter(i) << endl;

  //}
  //TGraphErrors dedxRatioGraph(binCenters.size(),&(*binCenters.begin()),&(*ratios.begin()),&(*binWidths.begin()),&(*ratioErrors.begin()));
  //dedxRatioGraph.SetTitle("Data/PDF: dE/dx");
  //dedxRatioGraph.Draw("ap");
  //combinedCanvasRatio->cd(2);
  //ratios.clear();
  //binCenters.clear();
  //ratioErrors.clear();
  //binWidths.clear();
  //for(int i=1; i < timeDataHist->GetNbinsX()-1; ++i)
  //{
  //  binCenters.push_back(timeDataHist->GetBinCenter(i));
  //  binWidths.push_back(timeDataHist->GetBinWidth(i));
  //  if(timePDFHist->GetBinContent(i) != 0)
  //    ratios.push_back(timeDataHist->GetBinContent(i)/timePDFHist->GetBinContent(i));
  //  else
  //    ratios.push_back(0);
  //  ratioErrors.push_back(0.5*timeDataHist->GetBinError(i)/timeDataHist->GetBinContent(i));
  //  //cout << "ratios: " << timePDFHist->GetBinContent(i)/timeDataHist->GetBinContent(i)
  //  //  << " binCenter: " << timeDataHist->GetBinCenter(i) << endl;

  //}
  //TGraphErrors timeRatioGraph(binCenters.size(),&(*binCenters.begin()),&(*ratios.begin()),&(*binWidths.begin()),&(*ratioErrors.begin()));
  //timeRatioGraph.SetTitle("Data/PDF: maxCryTime");
  //timeRatioGraph.Draw("ap");
  //combinedCanvasRatio->Write();
  

  //Loop over hists: energyHist, timeHist and calculate the likelihood
  //float logLikelihood = 0;
  //float logLikelihoodBack = 0;
  //float logLikelihoodSignal = 0;
  //for(int entry = 0; entry < dedxTimeData->numEntries(); ++entry)
  //{
  //  RooArgSet temp = *dedxTimeData->get(entry);
  //  sumDeDx = temp.getRealValue("sumDeDx");
  //  time = temp.getRealValue("maxCryTime");
  //  //std::cout << "sumDeDx: " << sumDeDx.getVal() << " maxCryTime: " << maxCryTime.getVal() << std::endl;
  //  //std::cout << "-LogLikelihood = " << -1*log(dedxAndTimeModel.getVal(new RooArgSet(sumDeDx,maxCryTime))) << std::endl;
  //  //std::cout << "-LogLikelihoodBack = " << -1*log(dedxAndTimeBackModel.getVal(new RooArgSet(sumDeDx,maxCryTime))) << std::endl;
  //  logLikelihood-=log(dedxAndTimeModel.getVal(new RooArgSet(sumDeDx,maxCryTime)));
  //  logLikelihoodBack-=log(dedxAndTimeBackModel.getVal(new RooArgSet(sumDeDx,maxCryTime)));
  //  logLikelihoodSignal-=log(dedxAndTimeSignalModel.getVal(new RooArgSet(sumDeDx,maxCryTime)));
  //  //std::cout << "-LogLikelihood+LogLikelihoodBack = " << logLikelihood-logLikelihoodBack << std::endl;
  //}
  
  //cout << "Beginning binned fit" << endl;
  //landauDeDxMuonMean = 14.1;
  //landauDeDxMuonSigma = 2.183;
  //gaussianTimeMuonMean = 2.33;
  //sumDeDx.setBins(33);
  //maxCryTime.setBins(125);
  //cout << "sumDeDxBins: " << sumDeDx.getBins() << endl;
  //// Create binned dataset
  //RooDataHist* dh = new RooDataHist("dh","binned version of dedxTimeData",RooArgSet(sumDeDx,maxCryTime),*dedxTimeData);
  ////RooChi2Var chi2("chi2","chi2",dedxAndTimeModel,*dh) ;
  ////RooMinuit m2(chi2);
  //RooNLLVar nll2("nll2","nll2",dedxAndTimeModel,*dh,DataError(RooAbsData::SumW2));
  //RooMinuit m2(nll2);
  //m2.migrad();
  //m2.hesse();
  //RooFitResult* r2 = m2.save();
  //r2->Print("v");


  //XXX:Create plot of likelihood vs sigFrac
  //vector<float> sigFracs;
  //vector<float> likelihoods;
  //for(float i=0; i < 1.01; i+=0.01)
  //{
  //  double totLikelihood = 0;
  //  sigFracs.push_back(i);
  //  sigFrac = i;
  //  for(int entry = 0; entry < dedxTimeData->numEntries(); ++entry)
  //  {
  //    RooArgSet temp = *dedxTimeData->get(entry);
  //    sumDeDx = temp.getRealValue("sumDeDx");
  //    time = temp.getRealValue("maxCryTime");
  //    totLikelihood-=log(dedxAndTimeModel.getVal(new RooArgSet(sumDeDx,maxCryTime)));
  //  }
  //  likelihoods.push_back(totLikelihood);
  //}
  //TCanvas* likelihoodPlotCanvas = new TCanvas("llPlotCanvas","llPlotCanvas",1,1,500,500);
  //likelihoodPlotCanvas->cd();
  //TGraph* likelihoodPlot = new TGraph(sigFracs.size(),&(*sigFracs.begin()),&(*likelihoods.begin()));
  //likelihoodPlot->GetXaxis()->SetTitle("signalFraction");
  //likelihoodPlot->GetYaxis()->SetTitle("-LogLikelihood");
  //likelihoodPlot->Draw("alp");
  ////likelihoodPlotCanvas->Print("plotLikelihoodsCurve.png");
  //likelihoodPlotCanvas->Write();
  
  outputFile->Close();
  //myInputRootFile_->Close();

  //cout << "-LogLikelihood for SB model: " << logLikelihood << " -LogLikelihood for B only model: " << logLikelihoodBack
  //  << " -LogLikelihood for S only model: " << logLikelihoodSignal << endl;
  //cout << "-LogLikelihood difference (B-SB; + indicates SB more likely): " << logLikelihoodBack-logLikelihood << endl;
  //cout << "-LogLikelihood difference (S-SB; + indicates SB more likely): " << logLikelihoodSignal-logLikelihood << endl;
  
  //ofstream outputStream("likelihoodDifferences_muons.txt",ios::app);
  //if(!outputStream.is_open())
  //{
  //  cout << "ERROR: Unable to write to file (file is not open)." << endl;
  //  return -1;
  //}
  //if(!outputStream.good())
  //{
  //  cout << "ERROR: Unable to write to file (file is not open)." << endl;
  //  return -1;
  //}

  //// Get to the end of the file
  //outputStream.seekp(0,ios::end);
  //// Energy, tab, time
  //outputStream << logLikelihood-logLikelihoodBack << std::endl;
  //outputStream.close();

  return 0;
}
