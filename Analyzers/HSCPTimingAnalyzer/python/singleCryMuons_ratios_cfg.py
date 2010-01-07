import FWCore.ParameterSet.Config as cms

process = cms.Process("SINGLECRYMUONS")

#process.load("FWCore.MessageService.MessageLogger_cfi")
#process.MessageLogger.cerr.threshold = "DEBUG"
process.MessageLogger = cms.Service("MessageLogger",
    suppressWarning = cms.untracked.vstring('ecalEBunpacker','ecalRecHit'),
    suppressInfo = cms.untracked.vstring('ecalEBunpacker','ecalRecHit'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    categories = cms.untracked.vstring('ecalEBunpacker','ecalFixedAlphaBetaFitUncalibRecHit',
      'hscpTimingAnalyzer'),
    destinations = cms.untracked.vstring('cout')
)


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_2.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_3.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_4.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_5.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_8.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_9.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_10.root'
#
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_11.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_12.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_13.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_14.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_15.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_16.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_17.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_18.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_19.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_20.root'

#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_21.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_22.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_23.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_24.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_25.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_26.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_27.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_28.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_29.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_30.root'

#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_31.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_32.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_33.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_34.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_5_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_35.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_36.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_37.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_38.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_39.root',
#'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_40.root'

##files missing
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_42.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_43.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_44.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_6_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_45.root',
# does not work for some reason 'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_5_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_46.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_5_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_46.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_5_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_47.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_5_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_49.root',
'rfio:/castor/cern.ch/user/s/scooper/hscp/311/muon_ieta-15iphi68_corner_Pt50GeV_Bfield_5_singleCryFiltered/singleCryCrossedFilter_muon_ieta-15iphi68_Pt50GeV_corner_genSimReco_50.root'

)
)

process.load("Analyzers.HSCPTimingAnalyzer.hscpTimingAnalyzer_cfi")
process.hscpTimingAnalyzer.EBUncalibRecHits = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")
process.hscpTimingAnalyzer.EEUncalibRecHits = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
#process.hscpTimingAnalyzer.EBUncalibRecHits = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")
#process.hscpTimingAnalyzer.EBUncalibRecHits = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEB")
#process.hscpTimingAnalyzer.RootFileName = cms.untracked.string("muons1CrossedCry_files21-30_hscpTimeAna_ratiosV3.root")
process.hscpTimingAnalyzer.muonCollection = cms.InputTag("muons")
process.hscpTimingAnalyzer.energyCut = 0.

#process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
#process.TFileService.fileName = 'timingAnalyzer.ratios.HSCP_V11.root'
process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("muons1CrossedCry_files42-50_run6_hscpTimeAna_ratiosV3.root"),
    closeFileFast = cms.untracked.bool(False)
)


process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")
process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE")
process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB")
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB")
#process.ecalRecHit.EEuncalibRecHitCollection = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEE")
#process.ecalRecHit.EBuncalibRecHitCollection = cms.InputTag("ecalWeightUncalibRecHit","EcalUncalibRecHitsEB")

#process.load("RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi")
## Change beta value to be on the measured alpha-beta line
#process.ecalFixedAlphaBetaFitUncalibRecHit.betaEB = 1.61
#process.ecalFixedAlphaBetaFitUncalibRecHit.alphaEB = 1.12
#process.load("RecoLocalCalo.EcalRecProducers.ecalWeightUncalibRecHit_cfi")

process.load("Configuration.StandardSequences.RawToDigi_cff")
process.load("Configuration.StandardSequences.L1Emulator_cff")
process.load("Configuration.StandardSequences.DigiToRaw_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.GlobalTag.globaltag = "MC_31X_v2::All"
#process.GlobalTag.globaltag = "MC_3XY_V10::All"

process.load("Configuration.StandardSequences.Reconstruction_cff")

# Create ratios uncalibRecHitProducer, here using data constants
process.load("RecoLocalCalo.EcalRecProducers.ecalRatioUncalibRecHit_cfi")
#process.ecalRatioUncalibRecHit = cms.EDProducer("EcalRatioMethodUncalibRecHitProducer",
#    EBdigiCollection = cms.InputTag("ecalDigis","ebDigis"),
#    EEdigiCollection = cms.InputTag("ecalDigis","eeDigis"),
#    EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
#    EEhitCollection = cms.string('EcalUncalibRecHitsEE'),
#    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
#    EEtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
#    EBamplitudeFitParameters = cms.vdouble(1.138,1.652),
#    EEamplitudeFitParameters = cms.vdouble(1.138,1.652),
#    EBtimeFitLimits_Lower = cms.double(0.2),
#    EBtimeFitLimits_Upper = cms.double(1.4),
#    EEtimeFitLimits_Lower = cms.double(0.2),
#    EEtimeFitLimits_Upper = cms.double(1.4)
    # MC Constants
process.ecalRatioUncalibRecHit.EBtimeFitParameters = (-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
process.ecalRatioUncalibRecHit.EEtimeFitParameters = (-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01)
process.ecalRatioUncalibRecHit.EBamplitudeFitParameters = cms.vdouble(1.138,1.652)
process.ecalRatioUncalibRecHit.EEamplitudeFitParameters = cms.vdouble(1.138,1.652)
process.ecalRatioUncalibRecHit.EBtimeFitLimits_Lower = cms.double(0.10)
process.ecalRatioUncalibRecHit.EBtimeFitLimits_Upper = cms.double(1.45)
process.ecalRatioUncalibRecHit.EEtimeFitLimits_Lower = cms.double(0.10)
process.ecalRatioUncalibRecHit.EEtimeFitLimits_Upper = cms.double(1.45)
    #TB Data constants            
    #    EBtimeFitParameters = cms.vdouble(-2.015452e+00, 3.130702e+00, -1.234730e+01, 4.188921e+01, -8.283944e+01, 9.101147e+01, -5.035761e+01, 1.105621e+01),
    #    EEtimeFitParameters = cms.vdouble(-2.390548e+00, 3.553628e+00, -1.762341e+01, 6.767538e+01, -1.332130e+02, 1.407432e+02, -7.541106e+01, 1.620277e+01),
    #    EBamplitudeFitParameters = cms.vdouble(1.000269e+00, 4.034871e-03, -1.647478e-01, -1.764497e-03, 1.678786e-01, 1.471199e-01, -3.997733e-01, -2.253389e-01),
    #    EEamplitudeFitParameters = cms.vdouble(1.0023, 0.00134947, -0.207695, 0.0219929, 0.28912, 0.12666, -0.469445, -0.201696),
    #    EBtimeFitLimits_Lower = cms.double(0.05),
    #    EBtimeFitLimits_Upper = cms.double(1.45),
    #    EEtimeFitLimits_Lower = cms.double(0.10),
    #    EEtimeFitLimits_Upper = cms.double(1.40)
    #)


# Geometry
#process.load("RecoEcal.EgammaClusterProducers.geometryForClustering_cff")
process.load("Configuration.StandardSequences.Geometry_cff")
#process.load("Configuration.StandardSequences.MagneticField_cff")
#process.load("Configuration.StandardSequences.GeometryPilot2_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")


# Trivial Conds
#process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")
#process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")

process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(
process.RawToDigi*
#process.ecalWeightUncalibRecHit*
#process.ecalFixedAlphaBetaFitUncalibRecHit*
#process.printContent*
process.ecalRatioUncalibRecHit*
process.ecalRecHit*
process.hscpTimingAnalyzer)
