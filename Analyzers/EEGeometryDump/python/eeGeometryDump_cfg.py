import FWCore.ParameterSet.Config as cms

process = cms.Process("eeGeometryDump")
process.source = cms.Source("EmptySource")
#process.source = cms.Source("PoolSource",
#   skipEvents = cms.untracked.uint32(6),
#   fileNames = cms.untracked.vstring(
#   #'rfio:/castor/cern.ch/user/c/ccecal/BEAM/Skims/61642/Run61642_EventNumberSkim_RAW.root'
#   'file:/data/scooper/data/beamHaloSkimsSept2008/Run62068_EcalHighEnergySkim_RAW.root'
#   )
#)
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

#process.MessageLogger = cms.Service("MessageLogger",
#                                    destinations = cms.untracked.vstring("cout"),
#                                    cout = cms.untracked.PSet(threshold = cms.untracked.string("ERROR")))

process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("Geometry.EcalCommonData.EcalOnly_cfi")
process.load("Geometry.CaloEventSetup.CaloGeometry_cff")

#process.load("Geometry.EcalMapping.EcalMapping_cfi")
#process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")
#process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
#process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi")
#process.load("Configuration.StandardSequences.FakeConditions_cff")
#process.load("Configuration.StandardSequences.Geometry_cff")
#process.demo = cms.EDAnalyzer("PrintEventSetupContent")






process.geometryDump = cms.EDAnalyzer("EEGeometryDump")

process.p = cms.Path(
#process.demo*
process.geometryDump
)
