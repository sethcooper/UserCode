import FWCore.ParameterSet.Config as cms

process = cms.Process("ECALTIMECALIBANALYZER")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
             'file:/data/scooper/data/beamHaloSkimsSept2008/Run61642_EventNumberSkim_RAW.root'
             #'file:/data/scooper/data/beamHaloSkimsSept2008/Run62036_EventNumberSkim_RAW.root'
#'file:/data/scooper/data/beamHaloSkimsSept2008/Run62068_EcalHighEnergySkim_RAW.root'
    )
)

process.ecalTimeCalibAnalyzer = cms.EDAnalyzer("EcalTimeCalibAnalyzer",
    applyCalibration = cms.untracked.bool(True),
    calibrationFile = cms.untracked.string("ebSplashAvg_62068_v8.calibs.txt"),
    #calibrationFile = cms.untracked.string("ebSplashAvg_62068.calibs.txt"),
    #calibrationFile = cms.untracked.string("ebSplashAvg_61642_62063.calibs.txt"),
    #calibrationFile = cms.untracked.string("ebSplashAvg_61642_62063_62068.calibs.txt"),
    EcalRecHitCollectionEB = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    EcalRecHitCollectionEE = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    EcalUncalibRecHitCollectionEB = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")
)

process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
process.TFileService.fileName = 'ecalTimeCalibAnalyzer.root'

process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")

#process.load("RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi")
## Change beta value to be on the measured alpha-beta line
#process.ecalFixedAlphaBetaFitUncalibRecHit.betaEB = 1.55

process.load("Configuration.StandardSequences.RawToDigi_Data_cff")
process.load("Configuration.StandardSequences.L1Emulator_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "CRUZETALL_V8::All"


process.load("Configuration.StandardSequences.Reconstruction_cff")

# Create ratios uncalibRecHitProducer, here using data constants
process.ecalRatioUncalibRecHit = cms.EDProducer("EcalRatioMethodUncalibRecHitProducer",
    EBdigiCollection = cms.InputTag("ecalDigis","ebDigis"),
    EEdigiCollection = cms.InputTag("ecalDigis","eeDigis"),
    EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
    EEhitCollection = cms.string('EcalUncalibRecHitsEE'),
    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    EEtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    EBamplitudeFitParameters = cms.vdouble(1.138,1.652),
    EEamplitudeFitParameters = cms.vdouble(1.138,1.652),
    EBtimeFitLimits_Lower = cms.double(0.2),
    EBtimeFitLimits_Upper = cms.double(1.4),
    EEtimeFitLimits_Lower = cms.double(0.2),
    EEtimeFitLimits_Upper = cms.double(1.4)
    # MC Constants
    #EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    #EEtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    #EBamplitudeFitParameters = cms.vdouble(1.000269e+00, 4.034871e-03, -1.647478e-01, -1.764497e-03, 1.678786e-01, 1.471199e-01, -3.997733e-01, -2.253389e-01),
    #EEamplitudeFitParameters = cms.vdouble(1.000269e+00, 4.034871e-03, -1.647478e-01, -1.764497e-03, 1.678786e-01, 1.471199e-01, -3.997733e-01, -2.253389e-01),
    #EBtimeFitLimits_Lower = cms.double(0.100),
    #EBtimeFitLimits_Upper = cms.double(1.45),
    #EEtimeFitLimits_Lower = cms.double(0.10),
    #EEtimeFitLimits_Upper = cms.double(1.45)
    #TB Data constants            
    #    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    #    EEtimeFitParameters = cms.vdouble(-2.390548e+00, 3.553628e+00, -1.762341e+01, 6.767538e+01, -1.332130e+02, 1.407432e+02, -7.541106e+01, 1.620277e+01),
    #    EBamplitudeFitParameters = cms.vdouble(1.000269e+00, 4.034871e-03, -1.647478e-01, -1.764497e-03, 1.678786e-01, 1.471199e-01, -3.997733e-01, -2.253389e-01),
    #    EEamplitudeFitParameters = cms.vdouble(1.0023, 0.00134947, -0.207695, 0.0219929, 0.28912, 0.12666, -0.469445, -0.201696),
    #    EBtimeFitLimits_Lower = cms.double(0.05),
    #    EBtimeFitLimits_Upper = cms.double(1.45),
    #    EEtimeFitLimits_Lower = cms.double(0.10),
    #    EEtimeFitLimits_Upper = cms.double(1.40)
    )


# Geometry
process.load("RecoEcal.EgammaClusterProducers.geometryForClustering_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.GeometryPilot2_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")


# Trivial Conds
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
#process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(
process.RawToDigi*
#process.ecalFixedAlphaBetaFitUncalibRecHit*
#process.printContent*
process.ecalRatioUncalibRecHit *
process.ecalRecHit*
process.ecalTimeCalibAnalyzer)
