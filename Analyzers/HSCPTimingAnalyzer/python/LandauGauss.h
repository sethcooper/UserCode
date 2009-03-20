/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
 * Copyright (c) 2000-2007, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef LANDAUGAUSS
#define LANDAUGAUSS

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
 
class LandauGauss : public RooAbsPdf {
public:
  LandauGauss() {} ; 
  LandauGauss(const char *name, const char *title,
	      RooAbsReal& _x,
	      RooAbsReal& _width,
	      RooAbsReal& _mp,
	      RooAbsReal& _area,
	      RooAbsReal& _gSigma);
  LandauGauss(const LandauGauss& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new LandauGauss(*this,newname); }
  inline virtual ~LandauGauss() { }

protected:

  RooRealProxy x ;
  RooRealProxy width ;
  RooRealProxy mp ;
  RooRealProxy area ;
  RooRealProxy gSigma ;
  
  Double_t evaluate() const ;

private:

  ClassDef(LandauGauss,1) // Your description goes here...
};
 
#endif
