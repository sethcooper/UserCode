import FWCore.ParameterSet.Config as cms

process = cms.Process("EcalCosmicTrackTimingProducer")

process.load("EventFilter.EcalRawToDigiDev.EcalUnpackerMapping_cfi")

process.load("EventFilter.EcalRawToDigiDev.EcalUnpackerData_cfi")

process.load("Geometry.CaloEventSetup.CaloTopology_cfi")

process.load("Geometry.EcalCommonData.EcalOnly_cfi")

process.load("Geometry.CaloEventSetup.CaloGeometry_cff")
process.load("Geometry.EcalMapping.EcalMapping_cfi")
process.load("Geometry.EcalMapping.EcalMappingRecord_cfi")
process.load("RecoEcal.EgammaClusterProducers.geometryForClustering_cff")
process.load("RecoEcal.EgammaClusterProducers.cosmicClusteringSequence_cff")

import RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi
process.ecalUncalibHit = RecoLocalCalo.EcalRecProducers.ecalFixedAlphaBetaFitUncalibRecHit_cfi.ecalFixedAlphaBetaFitUncalibRecHit.clone()
process.load("RecoLocalCalo.EcalRecProducers.ecalRecHit_cfi")

process.load("CalibCalorimetry.EcalLaserCorrection.ecalLaserCorrectionService_cfi")
process.load("HLTrigger.special.TriggerTypeFilter_cfi")
process.load("CalibCalorimetry.EcalTrivialCondModules.EcalTrivialCondRetriever_cfi")

process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.GlobalRuns.ForceZeroTeslaField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_noesprefer_cff")

process.load("CaloOnlineTools.EcalTools.EcalCosmicTrackTimingProducer_cfi")

process.MessageLogger = cms.Service("MessageLogger",
    #suppressInfo = cms.untracked.vstring('ecalEBunpacker'),
    cout = cms.untracked.PSet(
      threshold = cms.untracked.string('DEBUG')
      ),
    categories = cms.untracked.vstring('EcalCosmicTrackTimingProducer'),
    destinations = cms.untracked.vstring('cout')
)

process.source = cms.Source("PoolSource",
    skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(#'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/057/289/1E1407F1-106D-DD11-97A7-000423D985E4.root'
        #'/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/058/359/005A40D9-1470-DD11-A2B6-001617C3B6DE.root')
        '/store/data/Commissioning08/Cosmics/RAW/CRUZET4_v1/000/057/771/00D18762-386E-DD11-A081-0016177CA7A0.root')
)
        
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.p = cms.Path(process.triggerTypeFilter*process.ecalEBunpacker*process.ecalUncalibHit*process.ecalRecHit*process.cosmicClusteringSequence*process.ecalCosmicTrackTimingProducer)

process.GlobalTag.globaltag = 'CRUZET4_V1P::All'

process.ecalUncalibHit.EBdigiCollection = 'ecalEBunpacker:ebDigis'
process.ecalUncalibHit.EEdigiCollection = 'ecalEBunpacker:eeDigis'
process.ecalRecHit.ChannelStatusToBeExcluded = [1]
process.ecalRecHit.EBuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEB'
process.ecalRecHit.EEuncalibRecHitCollection = 'ecalUncalibHit:EcalUncalibRecHitsEE'
process.EcalTrivialConditionRetriever.producedEcalWeights = False
process.EcalTrivialConditionRetriever.producedEcalPedestals = False
process.EcalTrivialConditionRetriever.producedEcalIntercalibConstants = False
process.EcalTrivialConditionRetriever.producedEcalIntercalibErrors = False
process.EcalTrivialConditionRetriever.producedEcalGainRatios = False
process.EcalTrivialConditionRetriever.producedEcalADCToGeVConstant = False
process.EcalTrivialConditionRetriever.producedEcalLaserCorrection = False
process.EcalTrivialConditionRetriever.producedChannelStatus = False
process.EcalTrivialConditionRetriever.producedChannelStatus = True
process.EcalTrivialConditionRetriever.channelStatusFile = 'CaloOnlineTools/EcalTools/data/listCRUZET4.v2.hashed.txt'
es_prefer_EcalChannelStatus = cms.ESPrefer("EcalTrivialConditionRetriever","EcalChannelStatus")

