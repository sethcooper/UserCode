import FWCore.ParameterSet.Config as cms

process = cms.Process("ECALSIMPLETBANALYSIS")

process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    categories = cms.untracked.vstring('ecalSimpleTBAnalysis'),
    destinations = cms.untracked.vstring('cout'),
    suppressError = cms.untracked.vstring('ecalFixedAlphaBetaFitUncalibRecHit')
)

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(#'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/057/289/1E1407F1-106D-DD11-97A7-000423D985E4.root'
#'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/058/359/005A40D9-1470-DD11-A2B6-001617C3B6DE.root')
#'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/057/771/00D18762-386E-DD11-A081-0016177CA7A0.root')
#'file:/data/scooper/data/h4b-06/h4b.00013428.A.0.0.root'
#'file:/data/scooper/data/h4b-06/h4b.00016629.A.0.0.root'
'file:/export/scratch/users/cooper/h4b.00016428.A.0.0.root',
'file:/export/scratch/users/cooper/h4b.00016429.A.0.0.root',
'file:/export/scratch/users/cooper/h4b.00016430.A.0.0.root'
)
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000000)
)

#process.load("EventFilter.EcalTBRawToDigi.h4_mapping_cfi")
process.load("EventFilter.EcalTBRawToDigi.ecalTBunpack_cfi")
process.load("RecoTBCalo.EcalTBHodoscopeReconstructor.ecal2006TBHodoscopeReconstructor_cfi")
#process.load("RecoTBCalo.EcalTBRecProducers.ecal2006TBWeightUncalibRecHit_cfi")
process.load("RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi")
process.load("RecoTBCalo.EcalTBTDCReconstructor.ecal2006TBTDCReconstructor_cfi")
#process.load("RecoTBCalo.EcalTBTDCReconstructor.Ecal2006TBTDCRanges_v2_cff")
process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
#process.ecal2006TBWeightUncalibRecHit.EBdigiCollection = 'ebDigis'
process.load("RecoTBCalo.EcalSimpleTBAnalysis.ecalSimpleTBAnalyzer_cfi")

process.ecalRawTDCDump = cms.EDAnalyzer("EcalTBTDCRawInfoDumper",
    rawInfoCollection = cms.string(""),
    rawInfoProducer   = cms.string("ecalTBunpack")
)
#TB Trivial Conds
process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
process.EcalTrivialConditionRetriever.weightsForTB = True
process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.dumpEv = cms.EDAnalyzer("EventContentAnalyzer")

process.ecalSimpleTBAnalyzer.hitProducer = "ecalFixedAlphaBetaFitUncalibRecHit"
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
  process.ecalSimpleTBAnalyzer
)
