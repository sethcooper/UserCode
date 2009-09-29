import FWCore.ParameterSet.Config as cms

process = cms.Process("ECALTIMING")

process.load("EventFilter.EcalRawToDigiDev.EcalUnpackerMapping_cfi")
process.load("EventFilter.EcalRawToDigiDev.EcalUnpackerData_cfi")

process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("Geometry.EcalCommonData.EcalOnly_cfi")
process.load("Geometry.CaloEventSetup.CaloGeometry_cff")

process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source("PoolSource",
   skipEvents = cms.untracked.uint32(6),
   fileNames = cms.untracked.vstring(
   #'rfio:/castor/cern.ch/user/c/ccecal/BEAM/Skims/61642/Run61642_EventNumberSkim_RAW.root'
   'file:/data/scooper/data/beamHaloSkimsSept2008/Run62068_EcalHighEnergySkim_RAW.root'
   )
)

process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")
process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")


process.ecalRatioUncalibRecHit = cms.EDProducer("EcalRatioMethodUncalibRecHitProducer",
    EBdigiCollection = cms.InputTag("ecalEBunpacker","ebDigis"),
    EEdigiCollection = cms.InputTag("ecalEBunpacker","eeDigis"),
    EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
    EEhitCollection = cms.string('EcalUncalibRecHitsEE'),
    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    EEtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    EBamplitudeFitParameters = cms.vdouble(1.138,1.652),
    EEamplitudeFitParameters = cms.vdouble(1.138,1.652),
    #EBtimeFitLimits_Lower = cms.double(0.2),
    #EBtimeFitLimits_Upper = cms.double(1.4),
    #EBtimeFitLimits_Lower = cms.double(0.5),
    #EBtimeFitLimits_Upper = cms.double(1.2),
    EBtimeFitLimits_Lower = cms.double(0.01),
    EBtimeFitLimits_Upper = cms.double(1.4),
    EEtimeFitLimits_Lower = cms.double(0.2),
    EEtimeFitLimits_Upper = cms.double(1.4)
    )


#Digi skimmer?
# Shouldn't be needed here

process.load("Analyzers.EcalTimingNTupleMaker.ecalTimingNTupleMaker_cfi")
process.ecalTimingNTupleMaker.hitProducer = "ecalRatioUncalibRecHit"
process.ecalTimingNTupleMaker.hitProducerEE = "ecalRatioUncalibRecHit"
process.ecalTimingNTupleMaker.CorrectEcalReadout = True
process.ecalTimingNTupleMaker.CorrectBH = True
process.ecalTimingNTupleMaker.BeamHaloPlus = False
process.ecalTimingNTupleMaker.CorrectAverage = True
process.ecalTimingNTupleMaker.rootfile = "TimingPhysics_FullRun.62068.root"

process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")

process.p = cms.Path(
   process.ecalEBunpacker
   *
   process.ecalRatioUncalibRecHit
   *
   process.ecalRecHit
   *
   process.ecalTimingNTupleMaker
)

