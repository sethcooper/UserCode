import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

process.doToyMC = cms.PSet(
    analysisCommonParameters,
    #BackgroundPredictionInputRootFile = cms.string("4.nov1.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"),
    #BackgroundPredictionInputRootFile = cms.string("3.nov2.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"),
    BackgroundPredictionInputRootFile = cms.string("2.nov4.allSlices.EPSrunPostEPSprod.makeIasPredictions.root"),
    SignalInputRootFile = cms.string("nov1.withRooDataSet.makeHSCParticlePlots.GMstau200.root"),
    OutputRootFile = cms.string("1.nov7.rooMCStudy.110sigTracksOneEtaNomSlice.toyMC.root"),
    NumberOfBackgroundTracksInDRegion = cms.int32(1079901), # after all skim/presels
    NumberOfSignalTracks = cms.int32(110), # for this int lumi, after all skim/presels
    LowerNoMOfSlice = cms.int32(13),
    LowerEtaOfSlice = cms.double(0.2),
    NumberOfTrials = cms.int32(10000)
)

print 'Running with p threshold: ',
print analysisCommonParameters.PSidebandThreshold
print 'Running with Ih threshold:',
print analysisCommonParameters.IhSidebandThreshold
print 'Running on lowerNoM',
print process.doToyMC.LowerNoMOfSlice
print 'Running on lowerEta',
print process.doToyMC.LowerEtaOfSlice
#print 'Doing:',
#print process.doToyMC.NumberOfTrials,
#print ' trials for',
#print process.doToyMC.NumberOfSignalTracks,
#print ' signal tracks and ',
#print process.doToyMC.NumberOfBackgroundTracksInDRegion,
#print ' background tracks in D region (after skim/preselections)'

