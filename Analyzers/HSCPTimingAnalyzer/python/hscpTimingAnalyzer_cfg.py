import FWCore.ParameterSet.Config as cms

process = cms.Process("HSCPTIMINGANALYZER")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        #'file:/data/whybee0c/user/cooper/cmssw/hscp/220/KKLeptonMC/src/SimG4Core/CustomPhysics/python/genSimReco.root'
        'file:/data/whybee0c/user/cooper/data/MC/hscp/kkTau_E500GeV_EBP5LowEta/genSimReco_1.root',
        'file:/data/whybee0c/user/cooper/data/MC/hscp/kkTau_E500GeV_EBP5LowEta/genSimReco_2.root',
        'file:/data/whybee0c/user/cooper/data/MC/hscp/kkTau_E500GeV_EBP5LowEta/genSimReco_3.root',
        'file:/data/whybee0c/user/cooper/data/MC/hscp/kkTau_E500GeV_EBP5LowEta/genSimReco_4.root',
        'file:/data/whybee0c/user/cooper/data/MC/hscp/kkTau_E500GeV_EBP5LowEta/genSimReco_5.root'
    )
)

process.load("Analyzers.HSCPTimingAnalyzer.hscpTimingAnalyzer_cfi")
process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
process.TFileService.fileName = 'allEvts.hscpTiming.root'

process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEE")
process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")

process.load("RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi")
process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.StandardSequences.L1Emulator_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "IDEAL_V5::All"
# Trivial Conds
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(
process.RawToDigi*
process.ecalFixedAlphaBetaFitUncalibRecHit*
#process.printContent*
process.ecalRecHit*
process.hscpTimingAnalyzer)
