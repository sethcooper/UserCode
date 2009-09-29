import FWCore.ParameterSet.Config as cms

process = cms.Process("ECALTIMECALIBGRAPHS")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.source = cms.Source("EmptySource")
#,
#    # replace 'myfile.root' with the source file you want to use
#    fileNames = cms.untracked.vstring(
#        'file:myfile.root'
#    )
#)

process.ecalTimeCalibGraphs = cms.EDAnalyzer('EcalTimeCalibGraphs',
  #infile = cms.untracked.string('splashAll.calibs.txt')
  infile = cms.untracked.string('ebSplashAvg_61642.calibs.txt')
)

process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.TFileService = cms.Service("TFileService",
  fileName = cms.string('ecalTimeCalibGraphs.root')
)

process.p = cms.Path(process.ecalTimeCalibGraphs)
