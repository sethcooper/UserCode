//
/**
 * Author: Seth Cooper
 * Created: 5 Aug 2008
 **/

#include "ESProducers/EcalTimingCorrectionESProducer/interface/EcalTimingCorrection.h"

EcalTimingCorrection::EcalTimingCorrection()
{
}

EcalTimingCorrection::~EcalTimingCorrection()
{
}

void EcalTimingCorrection::setValue(const int& stripId, const int& xtalId, const double& correction)
{
  setValue(std::make_pair(stripId,xtalId),correction);
}

void EcalTimingCorrection::setValue(const std::pair<int,int>& keyPair, const double& correction)
{
  map_.insert(std::make_pair(keyPair,correction));
}

