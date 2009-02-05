import FWCore.ParameterSet.Config as cms

hscpTimingAnalyzer = cms.EDAnalyzer('HSCPTimingAnalyzer',
    EBRecHitCollection = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    EERecHitCollection = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    EBDigiCollection = cms.InputTag("ecalDigis","ebDigis"),
    EBUncalibRecHits = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")
)
