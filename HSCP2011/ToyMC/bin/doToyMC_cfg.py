import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

process.doToyMC = cms.PSet(
    analysisCommonParameters,
    #BackgroundPredictionInputRootFile = cms.string("4.nov1.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"),
    BackgroundPredictionInputRootFile = cms.string("3.nov2.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"),
    SignalInputRootFile = cms.string("nov1.withRooDataSet.makeHSCParticlePlots.GMstau200.root"),
    OutputRootFile = cms.string("3.nov3.rooMCStudy.11sigTracksFixedSigFrac.toyMC.root"),
    NumberOfBackgroundTracks = cms.int32(10000),
    NumberOfSignalTracks = cms.int32(10)
)

print 'Running with p threshold: ',
print analysisCommonParameters.PSidebandThreshold
print 'Running with Ih threshold:',
print analysisCommonParameters.IhSidebandThreshold

