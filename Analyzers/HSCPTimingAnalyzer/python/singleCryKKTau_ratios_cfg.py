import FWCore.ParameterSet.Config as cms

process = cms.Process("SINGLECRYKKTAU")

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
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/kktau_ieta-15iphi68_cornerAdj_Pt200GeV_Bfield_2/kktau_constPt200GeV_ieta-15iphi68cornerAdj_genSimReco_1.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/kktau_ieta-15iphi68_cornerAdj_Pt200GeV_Bfield_2/kktau_constPt200GeV_ieta-15iphi68cornerAdj_genSimReco_2.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/kktau_ieta-15iphi68_cornerAdj_Pt200GeV_Bfield_2/kktau_constPt200GeV_ieta-15iphi68cornerAdj_genSimReco_3.root'
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/kktau_ieta-15iphi68_cornerAdj_Pt200GeV_Bfield_2/kktau_constPt200GeV_ieta-15iphi68cornerAdj_genSimReco_4.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/kktau_ieta-15iphi68_cornerAdj_Pt200GeV_Bfield_2/kktau_constPt200GeV_ieta-15iphi68cornerAdj_genSimReco_5.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/kktau_ieta-15iphi68_cornerAdj_Pt200GeV_Bfield_2/kktau_constPt200GeV_ieta-15iphi68cornerAdj_genSimReco_6.root'
)
)

process.load("Analyzers.HSCPTimingAnalyzer.hscpTimingAnalyzer_cfi")
process.hscpTimingAnalyzer.EBUncalibRecHits = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")
process.hscpTimingAnalyzer.EEUncalibRecHits = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
#process.hscpTimingAnalyzer.EBUncalibRecHits = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")
#process.hscpTimingAnalyzer.EBUncalibRecHits = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEB")
process.hscpTimingAnalyzer.RootFileName = cms.untracked.string("kktau_hscpTimeAna_ratiosV3.root")
process.hscpTimingAnalyzer.muonCollection = cms.InputTag("muons")
process.hscpTimingAnalyzer.energyCut = 0.

#process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
#process.TFileService.fileName = 'timingAnalyzer.ratios.HSCP_V11.root'

process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEB")

#process.load("RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi")
## Change beta value to be on the measured alpha-beta line
#process.ecalFixedAlphaBetaFitUncalibRecHit.betaEB = 1.61
#process.ecalFixedAlphaBetaFitUncalibRecHit.alphaEB = 1.12
#process.load("RecoLocalCalo.EcalRecProducers.ecalWeightUncalibRecHit_cfi")

process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.StandardSequences.L1Emulator_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "MC_31X_v2::All"
#process.GlobalTag.globaltag = "MC_3XY_V10::All"

process.load("Configuration.StandardSequences.Reconstruction_cff")

# Create ratios uncalibRecHitProducer, here using data constants
process.load("RecoLocalCalo.EcalRecProducers.ecalRatioUncalibRecHit_cfi")
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
    # MC Constants
process.ecalRatioUncalibRecHit.EBtimeFitParameters = (-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
process.ecalRatioUncalibRecHit.EEtimeFitParameters = (-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
process.ecalRatioUncalibRecHit.EBamplitudeFitParameters = cms.vdouble(1.138,1.652)
process.ecalRatioUncalibRecHit.EEamplitudeFitParameters = cms.vdouble(1.138,1.652)
process.ecalRatioUncalibRecHit.EBtimeFitLimits_Lower = cms.double(0.100)
process.ecalRatioUncalibRecHit.EBtimeFitLimits_Upper = cms.double(1.45)
process.ecalRatioUncalibRecHit.EEtimeFitLimits_Lower = cms.double(0.10)
process.ecalRatioUncalibRecHit.EEtimeFitLimits_Upper = cms.double(1.45)
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


# Geometry
#process.load("RecoEcal.EgammaClusterProducers.geometryForClustering_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
#process.load("Configuration.StandardSequences.MagneticField_cff")
#process.load("Configuration.StandardSequences.GeometryPilot2_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")


# Trivial Conds
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
#process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(
process.RawToDigi*
#process.ecalWeightUncalibRecHit*
#process.ecalFixedAlphaBetaFitUncalibRecHit*
#process.printContent*
process.ecalRatioUncalibRecHit*
process.ecalRecHit*
process.hscpTimingAnalyzer)
