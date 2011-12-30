import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(),
    maxEvents   = cms.int32(-1),
)

process.fwliteOutput = cms.PSet(
    fileName = cms.string('2.dec6.test1slice.EPSrunPostEPSprod.makeMassPredictions.allPvalues.root'),
)

process.makeIasPredictions = cms.PSet(
    analysisCommonParameters,
)

print 'Running with p threshold: ',
print analysisCommonParameters.PSidebandThreshold
print 'Running with Ih threshold:',
print analysisCommonParameters.IhSidebandThreshold

process.fwliteInput.fileNames.extend([
    'file:nov2.withRooDataSet.makeHSCParticlePlots.EPSrunPostEPSprod.root'
    #'file:nov30.withRooDataSet.pt40.makeHSCParticlePlots.EPSrunPostEPSprod.root'
]);

