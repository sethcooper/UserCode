/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * Copyright (c) 2000-2005, Regents of the University of California          * 
 *                          and Stanford University. All rights reserved.    * 
 *                                                                           * 
 * Redistribution and use in source and binary forms,                        * 
 * with or without modification, are permitted according to the terms        * 
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             * 
 *****************************************************************************/ 

// -- CLASS DESCRIPTION [PDF] -- 
// Your description goes here... 

#include "Riostream.h" 

#include "LandauGauss.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
#include "TMath.h"

ClassImp(LandauGauss) 

  LandauGauss::LandauGauss(const char *name, const char *title, 
      RooAbsReal& _x,
      RooAbsReal& _width,
      RooAbsReal& _mp,
      RooAbsReal& _area,
      RooAbsReal& _gSigma) :
    RooAbsPdf(name,title), 
    x("x","x",this,_x),
    width("width","width",this,_width),
    mp("mp","mp",this,_mp),
    area("area","area",this,_area),
    gSigma("gSigma","gSigma",this,_gSigma)
{ 
} 


LandauGauss::LandauGauss(const LandauGauss& other, const char* name) :  
  RooAbsPdf(other,name), 
  x("x",this,other.x),
  width("width",this,other.width),
  mp("mp",this,other.mp),
  area("area",this,other.area),
  gSigma("gSigma",this,other.gSigma)
{ 
} 



Double_t LandauGauss::evaluate() const 
{ 
  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation), 
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

  double par[4] = {width,mp,area,gSigma};

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;


  // MP shift correction
  mpc = par[1] - mpshift * par[0]; 

  // Range of convolution integral
  xlow = x - sc * par[3];
  xupp = x + sc * par[3];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x,xx,par[3]);

    xx = xupp - (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x,xx,par[3]);
  }

  return (par[2] * step * sum * invsq2pi / par[3]);
} 



