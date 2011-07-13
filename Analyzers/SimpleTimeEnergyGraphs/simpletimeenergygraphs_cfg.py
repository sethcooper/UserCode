import FWCore.ParameterSet.Config as cms

process = cms.Process("SIMPLETIMEENERGYGRAPHS")

#process.load("Configuration.StandardSequences.DigiToRaw_cff")
#process.load("Configuration.StandardSequences.Digi_cff")
#process.load("Configuration.StandardSequences.Simulation_cff")
#process.load("Configuration.StandardSequences.SimL1Emulator_cff")
#process.load("Configuration.StandardSequences.RawToDigi_cff")
#process.load("Configuration.StandardSequences.L1Emulator_cff")
#process.load("Configuration.StandardSequences.DigiToRaw_cff")
#process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = ""

# Geometry
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
# Trivial Conds
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
#process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")
#process.load("Geometry.EcalMapping.EcalMapping_cfi")
#process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    #suppressWarning = cms.untracked.vstring('ecalEBunpacker','ecalRecHit'),
    #suppressInfo = cms.untracked.vstring('ecalEBunpacker','ecalRecHit'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    categories = cms.untracked.vstring('ecalEBunpacker','ecalFixedAlphaBetaFitUncalibRecHit',
      'hscpTimingAnalyzer'),
    destinations = cms.untracked.vstring('cout')
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),
    #duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/14BA2820-7713-DF11-815D-001BFCDBD1B6.root',
#'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/54B0EC28-7913-DF11-B900-002618943951.root',
#'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/54D70BA4-7613-DF11-AEC1-0018F3D0962E.root',
#'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/8CE8D47E-7513-DF11-B99D-00261894383C.root',
#'/store/relval/CMSSW_3_4_2/RelValSingleMuPt1000/GEN-SIM-DIGI-RAW-HLTDEBUG/MC_3XY_V15-v1/0011/AC131D34-B413-DF11-BBA4-0026189438B1.root'
        'file:pickevents_1_1_0Cq.root',
        'file:pickevents_3_1_BtM.root',
        'file:pickevents_5_1_Gw2.root',
        'file:pickevents_2_1_n8q.root',
        'file:pickevents_4_1_lnk.root',
        'file:pickevents_6_1_cFQ.root'
)
)

process.simpleTimeEnergyGraphs = cms.EDAnalyzer("SimpleTimeEnergyGraphs",
    EBRecHitCollection = cms.InputTag('reducedEcalRecHitsEB',''),
    EERecHitCollection = cms.InputTag('reducedEcalRecHitsEE',''),
    EBUncalibRecHitCollection = cms.InputTag('ecalGlobalUncalibRecHit','EcalUncalibRecHitsEB'),
    EEUncalibRecHitCollection = cms.InputTag('ecalGlobalUncalibRecHit','EcalUncalibRecHitsEE')
)

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(
#process.ecalDigis
#*process.ecalRatioUncalibRecHit
#*process.ecalGlobalUncalibRecHit
#*process.ecalRecHit
#*
process.simpleTimeEnergyGraphs
)

