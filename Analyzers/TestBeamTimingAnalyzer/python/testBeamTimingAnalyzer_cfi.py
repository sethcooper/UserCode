import FWCore.ParameterSet.Config as cms

testBeamTimingAnalyzer = cms.EDAnalyzer("TestBeamTimingAnalyzer",
   hitProducer   = cms.string("ecal2006TBWeightUncalibRecHit"),
   hitCollection = cms.string("EcalUncalibRecHitsEB"),
   hodoRecInfoProducer   = cms.string("ecal2006TBHodoscopeReconstructor"),
   hodoRecInfoCollection = cms.string("EcalTBHodoscopeRecInfo"),
   tdcRecInfoProducer   = cms.string("ecal2006TBTDCReconstructor"),
   tdcRecInfoCollection = cms.string("EcalTBTDCRecInfo"),
   eventHeaderProducer   = cms.string("ecalTBunpack"),
   eventHeaderCollection = cms.string(""),
   rootfile = cms.untracked.string("testBeamTimingAnalyzer.root"),
   digiCollection = cms.string("ebDigis"),
   digiProducer = cms.string("ecalTBunpack")
)
