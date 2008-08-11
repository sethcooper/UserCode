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
// $Id$
//
//


// system include files
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include "boost/shared_ptr.hpp"

// user include files
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/IOVSyncValue.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "IntraTTCorrection/EcalTimingCorrectionESProducer/interface/EcalTimingCorrection.h"
#include "IntraTTCorrection/EcalTimingCorrectionESProducer/interface/EcalTimingCorrectionRcd.h"



//
// class declaration
//

class EcalTimingCorrectionESProducer : public edm::ESProducer,
                                       public edm::EventSetupRecordIntervalFinder
{
   public:
      EcalTimingCorrectionESProducer(const edm::ParameterSet&);
      virtual ~EcalTimingCorrectionESProducer();

      //typedef boost::shared_ptr<EcalTimingCorrection> ReturnType;
      //ReturnType produce(const EcalTimingCorrectionRcd&);

      virtual std::auto_ptr<EcalTimingCorrection>
        getEcalTimingCorrectionsFromConfiguration(const EcalTimingCorrectionRcd& );

      virtual std::auto_ptr<EcalTimingCorrection>
        produceEcalTimingCorrections(const EcalTimingCorrectionRcd& );
   
   protected:
      //overriding from ContextRecordIntervalFinder
      virtual void setIntervalFor( const edm::eventsetup::EventSetupRecordKey&,
          const edm::IOVSyncValue& ,
          edm::ValidityInterval& ) ;
      
   private:
      bool producedEcalTimingCorrections_;
      std::string timingCorrectionsFile_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalTimingCorrectionESProducer::EcalTimingCorrectionESProducer(const edm::ParameterSet& iConfig)
{
   //the following line is needed to tell the framework what
   // data is being produced
  producedEcalTimingCorrections_ = iConfig.getUntrackedParameter<bool>("producedEcalTimingCorrections",true);
  timingCorrectionsFile_ = iConfig.getUntrackedParameter<std::string>("ecalTimingCorrectionsFile","");

  if(producedEcalTimingCorrections_)
  {
    if(timingCorrectionsFile_ != "")
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
  // initialize all to 0
  // barrel
  double correction(0);
  for(int stripId=1; stripId<6; ++stripId)
  {
    for(int xtalId=1; xtalId<6; ++xtalId)
    {
        timingCorrection->setValue(std::make_pair(stripId, xtalId), correction);
    }
  }
  // endcap
  // TODO: scheme for endcap?

  // Then overwrite from what we get in the file
  edm::LogInfo("EcalTimingCorrectionESProducer") << "Reading timing corrections from file "
    << edm::FileInPath(timingCorrectionsFile_).fullPath().c_str() ;
  std::ifstream timingCorrectionsFile(edm::FileInPath(timingCorrectionsFile_).fullPath().c_str());
  if (!timingCorrectionsFile.good()) {
    edm::LogError ("EcalTimingCorrectionESProducer") 
      << "*** Problems opening file: " << timingCorrectionsFile_ ;
    throw cms::Exception ("Cannot open ECAL timing corrections file") ;
  }
  // Format for file:
  // stripId <tab> xtalId <tab> timingCorrection

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
    //DEBUG
    std::cout << "EcalTimingCorrectionESProducer: strip " << stripId
      << " xtal: " << xtalId << " correction: " << correction << std::endl;

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
  return timingCorrection;
}


std::auto_ptr<EcalTimingCorrection>
EcalTimingCorrectionESProducer::produceEcalTimingCorrections(const EcalTimingCorrectionRcd&)
{
  std::auto_ptr<EcalTimingCorrection> timingCorrection = std::auto_ptr<EcalTimingCorrection>(new EcalTimingCorrection());
  // initialize all to 0
  // TODO: get this from the database
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

//define this as a plug-in
DEFINE_FWK_EVENTSETUP_MODULE(EcalTimingCorrectionESProducer);
