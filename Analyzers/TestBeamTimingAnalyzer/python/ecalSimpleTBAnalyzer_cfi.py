import FWCore.ParameterSet.Config as cms

ecalSimpleTBAnalyzer = cms.EDAnalyzer("EcalSimpleTBAnalyzer",
   hitProducer   = cms.string("ecal2006TBWeightUncalibRecHit"),
   hitCollection = cms.string("EcalUncalibRecHitsEB"),
   hodoRecInfoProducer   = cms.string("ecal2006TBHodoscopeReconstructor"),
   hodoRecInfoCollection = cms.string("EcalTBHodoscopeRecInfo"),
   tdcRecInfoProducer   = cms.string("ecal2006TBTDCReconstructor"),
   tdcRecInfoCollection = cms.string("EcalTBTDCRecInfo"),
   eventHeaderProducer   = cms.string("ecalTBunpack"),
   eventHeaderCollection = cms.string(""),
   rootfile = cms.untracked.string("EcalSimpleTBAnalysis.root"),
   digiCollection = cms.string("ebDigis"),
   digiProducer = cms.string("ecalTBunpack")
)
