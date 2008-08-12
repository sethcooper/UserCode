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
//         Created:  Tue Aug  12 09:14:28 CDT 2008
// $Id: EcalTimingCorrectionESProducer.cc,v 1.1 2008/08/12 20:20:25 scooper Exp $
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
#include "ESProducers/EcalTimingCorrectionESProducer/interface/EcalTimingCorrection.h"
#include "ESProducers/EcalTimingCorrectionESProducer/interface/EcalTimingCorrectionRcd.h"



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
      std::string timingCorrectionFile_;
};
