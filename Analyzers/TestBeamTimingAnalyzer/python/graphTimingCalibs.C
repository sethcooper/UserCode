
void graphTimingCalibs(char* infile = 0)
{
  if(!infile)
  {
    cout << "Error!  no calib input file specified" << endl;
    return;
  }

  ifstream calibFileStream;
  calibFileStream.open(infile);

  TH1F* calibHist = new TH1F("calibrations","time calibrations",500,-2,2);
  string cryName;
  double calibration;
  while(calibFileStream.good())
  {
    if(calibFileStream >> cryName >> calibration)
    {
      calibHist->Fill(calibration);
      cout << cryName << " timing calib:" << calibration << endl;
    }
  }
  calibFileStream.close();

  calibHist->SetXTitle("correction [ns]");
  calibHist->Draw();
}
