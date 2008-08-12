// -*- C++ -*-
//
// Package:    EcalTimingCorrectionESProducer
// Class:      EcalTimingCorrectionESProducer
// 
/**\class EcalTimingCorrectionESProducer EcalTimingCorrectionESProducer.h IntraTTCorrection/EcalTimingCorrectionESProducer/src/EcalTimingCorrectionESProducer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Seth Cooper
//         Created:  Tue Aug  5 15:14:28 CDT 2008
// $Id: EcalTimingCorrectionESProducer.cc,v 1.1 2008/08/11 20:20:25 scooper Exp $
//
//

#include "ESProducers/EcalTimingCorrectionESProducer/plugins/EcalTimingCorrectionESProducer.h"

//
// constructors and destructor
//
EcalTimingCorrectionESProducer::EcalTimingCorrectionESProducer(const edm::ParameterSet& iConfig)
{
   //the following line is needed to tell the framework what
   // data is being produced
  producedEcalTimingCorrections_ = iConfig.getUntrackedParameter<bool>("producedEcalTimingCorrections",true);
  timingCorrectionFile_ = iConfig.getUntrackedParameter<std::string>("ecalTimingCorrectionFile","");

  if(producedEcalTimingCorrections_)
  {
    if(timingCorrectionFile_ != "")
    { 
      // if file provided read it
      setWhatProduced(this, &EcalTimingCorrectionESProducer::getEcalTimingCorrectionsFromConfiguration);
    } 
    else
    {
      // for now initialize to 0; eventually read this from the conditions database
      setWhatProduced(this, &EcalTimingCorrectionESProducer::produceEcalTimingCorrections);
    }
    findingRecord<EcalTimingCorrectionRcd>();
  }
  //now do what ever other initialization is needed
}


std::auto_ptr<EcalTimingCorrection>
EcalTimingCorrectionESProducer::getEcalTimingCorrectionsFromConfiguration(const EcalTimingCorrectionRcd&)
{
  std::auto_ptr<EcalTimingCorrection> timingCorrection = std::auto_ptr<EcalTimingCorrection>(new EcalTimingCorrection());

  edm::LogInfo("EcalTimingCorrectionESProducer") << "Reading timing corrections from file "
    << edm::FileInPath(timingCorrectionFile_).fullPath().c_str() ;
  std::ifstream timingCorrectionsFile(edm::FileInPath(timingCorrectionFile_).fullPath().c_str());
  if (!timingCorrectionsFile.good()) {
    edm::LogError ("EcalTimingCorrectionESProducer") 
      << "*** Problems opening file: " << timingCorrectionFile_ ;
    throw cms::Exception ("Cannot open ECAL timing corrections file") ;
  }
  // Format for file:
  // stripId <tab> xtalId <tab> timingCorrection

  double correction(0);
  int stripId(0);
  int xtalId(0);
  std::string str;
  
  while (!timingCorrectionsFile.eof()) 
  {
    timingCorrectionsFile >> stripId;
    if (stripId < 1 || stripId > 6)
    {
      std::getline(timingCorrectionsFile,str);
      continue;
    }
    else
    {
      timingCorrectionsFile >> xtalId >> correction;
    }

    if(xtalId >= 1 && xtalId <= 5)
    {
      timingCorrection->setValue(std::make_pair(stripId,xtalId),correction);
    }
    else
    {
      edm::LogError("EcalTimingCorrectionESProducer")
        << " *** xtalId: " << xtalId << " is out of range";
    }

  }

  timingCorrectionsFile.close();
  // initialize missing entries to 0
  // Note that because of the way std::map works, only strips/xtals that are empty
  // will be modified here

  // barrel
  for(int stripId=1; stripId<6; ++stripId)
  {
    for(int xtalId=1; xtalId<6; ++xtalId)
    {
        timingCorrection->setValue(std::make_pair(stripId, xtalId), correction);
    }
  }
  // endcap
  // TODO: scheme for endcap?
  return timingCorrection;
}


std::auto_ptr<EcalTimingCorrection>
EcalTimingCorrectionESProducer::produceEcalTimingCorrections(const EcalTimingCorrectionRcd&)
{
  std::auto_ptr<EcalTimingCorrection> timingCorrection = std::auto_ptr<EcalTimingCorrection>(new EcalTimingCorrection());
  edm::LogInfo("EcalTimingCorrectionESProducer") << "Not Reading timing corrections from file";
  // initialize all to 0
  // TODO: get this from the database
  double correction(0);
  for(int stripId=1; stripId<6; ++stripId)
  {
    for(int xtalId=1; xtalId<6; ++xtalId)
    {
        timingCorrection->setValue(std::make_pair(stripId, xtalId), correction);
    }
  }
  return timingCorrection;
}


EcalTimingCorrectionESProducer::~EcalTimingCorrectionESProducer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}

void EcalTimingCorrectionESProducer::setIntervalFor( const edm::eventsetup::EventSetupRecordKey& rk,
    const edm::IOVSyncValue& iTime,
    edm::ValidityInterval& oValidity)
{
  //if(verbose_>=1) std::cout << "EcalTrivialConditionRetriever::setIntervalFor(): record key = " << rk.name() << "\ttime: " << iTime.time().value() << std::endl;
  //For right now, we will just use an infinite interval of validity
  oValidity = edm::ValidityInterval( edm::IOVSyncValue::beginOfTime(),edm::IOVSyncValue::endOfTime() );
}

// ------------ method called to produce the data  ------------
//EcalTimingCorrectionESProducer::ReturnType
//EcalTimingCorrectionESProducer::produce(const EcalTimingCorrectionRcd& iRecord)
//{
//   using namespace edm::es;
//   boost::shared_ptr<EcalTimingCorrection> pEcalTimingCorrection ;
//
//
//   return pEcalTimingCorrection ;
//}

