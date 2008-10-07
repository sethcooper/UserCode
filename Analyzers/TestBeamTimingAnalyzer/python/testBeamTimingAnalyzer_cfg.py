import FWCore.ParameterSet.Config as cms

process = cms.Process("TESTBEAMTIMINGANALYSIS")

process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    categories = cms.untracked.vstring('testBeamTimingAnalyzer'),
    destinations = cms.untracked.vstring('cout'),
    suppressError = cms.untracked.vstring('ecalFixedAlphaBetaFitUncalibRecHit')
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(#'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/057/289/1E1407F1-106D-DD11-97A7-000423D985E4.root'
#'file:/data/scooper/data/h4b-06/h4b.00013428.A.0.0.root'
#'file:/data/scooper/data/h4b-06/h4b.00016629.A.0.0.root'
'file:/data/scooper/data/h4b-06/h4b.00016428.A.0.0.root',
'file:/data/scooper/data/h4b-06/h4b.00016429.A.0.0.root',
'file:/data/scooper/data/h4b-06/h4b.00016430.A.0.0.root'
)
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100000)
)

process.load("EventFilter.EcalTBRawToDigi.ecalTBunpack_cfi")
process.load("RecoTBCalo.EcalTBHodoscopeReconstructor.ecal2006TBHodoscopeReconstructor_cfi")
process.load("RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi")
# Change beta value to be on the measured alpha-beta line
process.ecalFixedAlphaBetaFitUncalibRecHit.betaEB = 1.55

process.load("RecoTBCalo.EcalTBTDCReconstructor.ecal2006TBTDCReconstructor_cfi")
process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.load("Analyzers.TestBeamTimingAnalyzer.testBeamTimingAnalyzer_cfi")

process.ecalRawTDCDump = cms.EDAnalyzer("EcalTBTDCRawInfoDumper",
    rawInfoCollection = cms.string(""),
    rawInfoProducer   = cms.string("ecalTBunpack")
)
#TB Trivial Conds
process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
process.EcalTrivialConditionRetriever.weightsForTB = True
process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.dumpEv = cms.EDAnalyzer("EventContentAnalyzer")

process.testBeamTimingAnalyzer.hitProducer = "ecalFixedAlphaBetaFitUncalibRecHit"
process.ecalFixedAlphaBetaFitUncalibRecHit.EBdigiCollection = 'ecalTBunpack:ebDigis'
process.ecalFixedAlphaBetaFitUncalibRecHit.EEdigiCollection = 'ecalTBunpack:eeDigis'
process.ecalRecHit.EBuncalibRecHitCollection = 'ecal2006TBWeightUncalibRecHit:EcalUncalibRecHitsEB'
#process.ecalRecHit.EEuncalibRecHitCollection = 'ecal2006TBWeightUncalibRecHit:EcalUncalibRecHitsEE'


process.p = cms.Path(
  process.ecalTBunpack *
#  process.ecal2006TBWeightUncalibRecHit *
process.ecalFixedAlphaBetaFitUncalibRecHit *
#  process.ecalRecHit *
  process.ecal2006TBHodoscopeReconstructor *
  process.ecal2006TBTDCReconstructor *
#process.ecalRawTDCDump *
#  process.dumpEv *
  process.testBeamTimingAnalyzer
)
