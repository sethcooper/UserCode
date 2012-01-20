import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

## MASS CUT HERE
analysisCommonParameters.MassCut = XXX_MASSCUT_XXX

process.makeScaledPredictionHistograms = cms.PSet(
    analysisCommonParameters,
    IntegratedLuminosity= cms.double(XXX_INTLUMI_XXX),
    BackgroundPredictionInputRootFile = cms.string("XXX_BGINPUTFILE_XXX"),
    SignalInputRootFile = cms.string("XXX_SIGINPUTFILE_XXX"),
    OutputRootFile = cms.string("XXX_OUTPUTFILE_XXX"),

    IasCutForEfficiency = cms.double(XXX_IASCUT_XXX),
    UsePtForSideband = cms.bool(True),
    PtSidebandThreshold = cms.double(XXX_PTTHRESH_XXX),
)

if process.makeScaledPredictionHistograms.UsePtForSideband==cms.bool(True):
    print 'Running with pt threshold: ',
    print process.makeScaledPredictionHistograms.PtSidebandThreshold
else:
    print 'Running with p threshold: ',
    print analysisCommonParameters.PSidebandThreshold

print 'Running with Ih threshold:',
print analysisCommonParameters.IhSidebandThreshold
print 'Running with mass cut:',
print analysisCommonParameters.MassCut
