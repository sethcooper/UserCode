import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

## MASS CUT HERE
analysisCommonParameters.MassCut = 420.0

process.makeScaledPredictionHistograms = cms.PSet(
    analysisCommonParameters,
    IntegratedLuminosity= cms.double(1091),
    #BackgroundPredictionInputRootFile = cms.string("1.nov9.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"),
    #SignalInputRootFile = cms.string("nov1.withRooDataSet.makeHSCParticlePlots.GMstau200.root"),
    #BackgroundPredictionInputRootFile = cms.string("dec15.mass160.EPSrunPostEPSprod.makeIasPredictions.root"),
    #SignalInputRootFile = cms.string("dec15.makeHSCParticlePlots.GMStau308.root"),
    BackgroundPredictionInputRootFile = cms.string("dec15.mass420.EPSrunPostEPSprod.makeIasPredictions.root"),
    SignalInputRootFile = cms.string("dec9.makeHSCParticlePlots.gluino1000.tof.root"),
    OutputRootFile = cms.string("1.dec16.makeScaledPredictionHistograms.gluino1000.mass420.unrolledHists.root"),
)

print 'Running with p threshold: ',
print analysisCommonParameters.PSidebandThreshold
print 'Running with Ih threshold:',
print analysisCommonParameters.IhSidebandThreshold
print 'Running with mass cut:',
print analysisCommonParameters.MassCut
