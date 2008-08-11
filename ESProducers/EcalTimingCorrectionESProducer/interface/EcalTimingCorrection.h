#ifndef CondFormats_EcalObjects_EcalTimingCorrection_H
#define CondFormats_EcalObjects_EcalTimingCorrection_H
//
/**
 * Author: Seth Cooper
 * Created: 5 Aug 2008
 **/

#include <map>

#include "CondFormats/EcalObjects/interface/EcalCondObjectContainer.h"


class EcalTimingCorrection
{
  public:
    typedef std::map< std::pair< int,int >, double > EcalTimingCorrectionMap;

    EcalTimingCorrection();
    ~EcalTimingCorrection();

    // modifiers
    void setValue(const int& stripId, const int& xtalId, const double& correction);
    void setValue(const std::pair<int,int>& keyPair, const double& correction);

    // accessors
    const EcalTimingCorrectionMap& getMap() const
    {
      return map_;
    }

  private:
    EcalTimingCorrectionMap map_;
};
#endif
