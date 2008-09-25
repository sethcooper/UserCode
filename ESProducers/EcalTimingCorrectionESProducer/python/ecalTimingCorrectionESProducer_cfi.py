import FWCore.ParameterSet.Config as cms

EcalTimingCorrectionESProducer = cms.ESSource("EcalTimingCorrectionESProducer",
    producedEcalTimingCorrections = cms.untracked.bool(True),
    ecalTimingCorrectionFile = cms.untracked.string("ESProducers/EcalTimingCorrectionESProducer/data/ecalTimingCorrections.txt")
)

