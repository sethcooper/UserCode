import FWCore.ParameterSet.Config as cms

process = cms.Process("SIMPLETIMEENERGYGRAPHS")

process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.Digi_cff")
process.load("Configuration.StandardSequences.Simulation_cff")
process.load("Configuration.StandardSequences.SimL1Emulator_cff")
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.StandardSequences.L1Emulator_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "MC_3XY_V15::All"

# Geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
# Trivial Conds
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
#process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")
process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

#process.load("FWCore.MessageService.MessageLogger_cfi")
#process.MessageLogger.cerr.threshold = "DEBUG"
process.MessageLogger = cms.Service("MessageLogger",
    suppressWarning = cms.untracked.vstring('ecalEBunpacker','ecalRecHit'),
    suppressInfo = cms.untracked.vstring('ecalEBunpacker','ecalRecHit'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    categories = cms.untracked.vstring('ecalEBunpacker','ecalFixedAlphaBetaFitUncalibRecHit',
      'hscpTimingAnalyzer'),
    destinations = cms.untracked.vstring('cout')
)


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/14BA2820-7713-DF11-815D-001BFCDBD1B6.root',
'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/54B0EC28-7913-DF11-B900-002618943951.root',
'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/54D70BA4-7613-DF11-AEC1-0018F3D0962E.root',
'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/8CE8D47E-7513-DF11-B99D-00261894383C.root',
'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/AC131D34-B413-DF11-BBA4-0026189438B1.root'

)
)


process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.ecalRecHit.ebDetIdToBeRecovered = ""
process.ecalRecHit.eeDetIdToBeRecovered = ""
process.ecalRecHit.ebFEToBeRecovered = ""
process.ecalRecHit.eeFEToBeRecovered = ""
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalGlobalUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalGlobalUncalibRecHit","EcalUncalibRecHitsEB")
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")

process.load("RecoLocalCalo.EcalRecProducers.ecalGlobalUncalibRecHit_cfi")
# default CMSSW CVS params
process.ecalGlobalUncalibRecHit.EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
process.ecalGlobalUncalibRecHit.EEtimeFitParameters = cms.vdouble(-2.390548e+00, 3.553628e+00, -1.762341e+01, 6.767538e+01, -1.332130e+02, 1.407432e+02, -7.541106e+01, 1.620277e+01)
process.ecalGlobalUncalibRecHit.EBamplitudeFitParameters = cms.vdouble(1.138,1.652)
process.ecalGlobalUncalibRecHit.EEamplitudeFitParameters = cms.vdouble(1.890,1.400)
process.ecalGlobalUncalibRecHit.EBtimeFitLimits_Lower = cms.double(0.2)
process.ecalGlobalUncalibRecHit.EBtimeFitLimits_Upper = cms.double(1.4)
process.ecalGlobalUncalibRecHit.EEtimeFitLimits_Lower = cms.double(0.2)
process.ecalGlobalUncalibRecHit.EEtimeFitLimits_Upper = cms.double(1.4)


# Create ratios uncalibRecHitProducer, here using data constants
#process.load("RecoLocalCalo.EcalRecProducers.ecalRatioUncalibRecHit_cfi")
#process.ecalRatioUncalibRecHit = cms.EDProducer("EcalRatioMethodUncalibRecHitProducer",
#    EBdigiCollection = cms.InputTag("ecalDigis","ebDigis"),
#    EEdigiCollection = cms.InputTag("ecalDigis","eeDigis"),
#    EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
#    EEhitCollection = cms.string('EcalUncalibRecHitsEE'),
#    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
#    EEtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
#    EBamplitudeFitParameters = cms.vdouble(1.138,1.652),
#    EEamplitudeFitParameters = cms.vdouble(1.138,1.652),
#    EBtimeFitLimits_Lower = cms.double(0.2),
#    EBtimeFitLimits_Upper = cms.double(1.4),
#    EEtimeFitLimits_Lower = cms.double(0.2),
#    EEtimeFitLimits_Upper = cms.double(1.4)
    #TB Data constants            
    #    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    #    EEtimeFitParameters = cms.vdouble(-2.390548e+00, 3.553628e+00, -1.762341e+01, 6.767538e+01, -1.332130e+02, 1.407432e+02, -7.541106e+01, 1.620277e+01),
    #    EBamplitudeFitParameters = cms.vdouble(1.000269e+00, 4.034871e-03, -1.647478e-01, -1.764497e-03, 1.678786e-01, 1.471199e-01, -3.997733e-01, -2.253389e-01),
    #    EEamplitudeFitParameters = cms.vdouble(1.0023, 0.00134947, -0.207695, 0.0219929, 0.28912, 0.12666, -0.469445, -0.201696),
    #    EBtimeFitLimits_Lower = cms.double(0.05),
    #    EBtimeFitLimits_Upper = cms.double(1.45),
    #    EEtimeFitLimits_Lower = cms.double(0.10),
    #    EEtimeFitLimits_Upper = cms.double(1.40)
    #)
    # MC Constants
#process.ecalGlobalUncalibRecHit.EBtimeFitParameters = (-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
#process.ecalGlobalUncalibRecHit.EEtimeFitParameters = (-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
#process.ecalGlobalUncalibRecHit.EBamplitudeFitParameters = cms.vdouble(1.138,1.652)
#process.ecalGlobalUncalibRecHit.EEamplitudeFitParameters = cms.vdouble(1.138,1.652)
#process.ecalGlobalUncalibRecHit.EBtimeFitLimits_Lower = cms.double(0.10)
#process.ecalGlobalUncalibRecHit.EBtimeFitLimits_Upper = cms.double(1.45)
#process.ecalGlobalUncalibRecHit.EEtimeFitLimits_Lower = cms.double(0.10)
#process.ecalGlobalUncalibRecHit.EEtimeFitLimits_Upper = cms.double(1.45)
# SIC NOTE: I was using the above until Feb 23 2010, when I began to test the new constants below
# constants from Ledovskoy email Jan 21 2010 "Re: prelimininary results..."
# which are measured using the tabulated MC shapes in CMSSW
#process.ecalGlobalUncalibRecHit.EBtimeFitParameters = cms.vdouble(-2.100341e+00, 4.928155e+00,
#                                     -2.643388e+01, 9.293189e+01,
#                                     -1.791409e+02, 1.886506e+02,
#                                     -1.007539e+02, 2.144269e+01)
#process.ecalGlobalUncalibRecHit.EEtimeFitParameters = cms.vdouble(-2.381592e+00, 3.225976e+00,
#                                     -1.119777e+01, 3.638195e+01,
#                                     -6.525757e+01, 6.625517e+01,
#                                     -3.510426e+01, 7.636064e+00)
#process.ecalGlobalUncalibRecHit.EBamplitudeFitParameters = cms.vdouble(1.14878,1.59803)
#process.ecalGlobalUncalibRecHit.EEamplitudeFitParameters = cms.vdouble(1.49615,1.48646)
#process.ecalGlobalUncalibRecHit.EBtimeFitLimits_Lower = cms.double(0.05)
#process.ecalGlobalUncalibRecHit.EBtimeFitLimits_Upper = cms.double(1.35)
#process.ecalGlobalUncalibRecHit.EEtimeFitLimits_Lower = cms.double(0.05)
#process.ecalGlobalUncalibRecHit.EEtimeFitLimits_Upper = cms.double(1.35)

process.simpleTimeEnergyGraphs = cms.EDAnalyzer("SimpleTimeEnergyGraphs",
    EBRecHitCollection = cms.InputTag('ecalRecHit','EcalRecHitsEB'),
    EERecHitCollection = cms.InputTag('ecalRecHit','EcalRecHitsEE'),
    EBUncalibRecHitCollection = cms.InputTag('ecalGlobalUncalibRecHit','EcalUncalibRecHitsEB'),
    EEUncalibRecHitCollection = cms.InputTag('ecalGlobalUncalibRecHit','EcalUncalibRecHitsEE')
)

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(
process.ecalDigis
#*process.ecalRatioUncalibRecHit
*process.ecalGlobalUncalibRecHit
*process.ecalRecHit
*process.simpleTimeEnergyGraphs
)

