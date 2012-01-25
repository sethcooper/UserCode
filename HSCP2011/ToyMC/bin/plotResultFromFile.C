{
  using namespace RooStats;
  HypoTestInverterResult* r =  (HypoTestInverterResult*) _file0->Get("result_SigXsec");
  double upperLimit = r->UpperLimit();
  double ulError = r->UpperLimitEstimatedError();
  std::cout << "The computed upper limit is: " << upperLimit << " +/- " << ulError << std::endl;
  // compute expected limit
  std::cout << " expected limit (median) " << r->GetExpectedUpperLimit(0) << std::endl;
  std::cout << " expected limit (-1 sig) " << r->GetExpectedUpperLimit(-1) << std::endl;
  std::cout << " expected limit (+1 sig) " << r->GetExpectedUpperLimit(1) << std::endl;
  std::cout << " expected limit (-2 sig) " << r->GetExpectedUpperLimit(-2) << std::endl;
  std::cout << " expected limit (+2 sig) " << r->GetExpectedUpperLimit(2) << std::endl;

  HypoTestInverterPlot *plot = new HypoTestInverterPlot("HTI_Result_Plot","result",r);
  //plot->Draw("CLb 2CL");
  plot->Draw("2CL");
}
