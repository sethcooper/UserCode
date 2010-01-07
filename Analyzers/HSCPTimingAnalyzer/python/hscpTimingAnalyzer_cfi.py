import FWCore.ParameterSet.Config as cms

hscpTimingAnalyzer = cms.EDAnalyzer('HSCPTimingAnalyzer',
    EBRecHitCollection = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    EERecHitCollection = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    EBDigiCollection = cms.InputTag("ecalDigis","ebDigis"),
    #EBUncalibRecHits = cms.InputTag("ecalFixedAlphaBetaFitUncalibRecHit","EcalUncalibRecHitsEB"),
    minimumTrackPt = cms.double(10),
    RootFileName = cms.untracked.string("hscpTimingAnalyzer.root"),
    muonCollection = cms.InputTag("muons"),
    trackCollection = cms.InputTag("generalTracks"),
    energyCut = cms.untracked.double(0.0),
#    TrackAssociatorParameterBlock = cms.PSet(
#      TrackAssociatorParameters = cms.PSet(
#        muonMaxDistanceSigmaX = cms.double(0.0),
#        muonMaxDistanceSigmaY = cms.double(0.0),
#        CSCSegmentCollectionLabel = cms.InputTag("cscSegments"),
#        dRHcal = cms.double(9999.0),
#        dREcal = cms.double(9999.0),
#        CaloTowerCollectionLabel = cms.InputTag("towerMaker"),
#        useEcal = cms.bool(True),
#        dREcalPreselection = cms.double(0.05),
#        HORecHitCollectionLabel = cms.InputTag("horeco"),
#        dRMuon = cms.double(9999.0),
#        crossedEnergyType = cms.string('SinglePointAlongTrajectory'),
#        muonMaxDistanceX = cms.double(5.0),
#        muonMaxDistanceY = cms.double(5.0),
#        useHO = cms.bool(True),
#        accountForTrajectoryChangeCalo = cms.bool(False),
#        DTRecSegment4DCollectionLabel = cms.InputTag("dt4DSegments"),
#        EERecHitCollectionLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
#        dRHcalPreselection = cms.double(0.2),
#        useMuon = cms.bool(True),
#        useCalo = cms.bool(False),
#        EBRecHitCollectionLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
#        dRMuonPreselection = cms.double(0.2),
#        truthMatch = cms.bool(False),
#        HBHERecHitCollectionLabel = cms.InputTag("hbhereco"),
#        useHcal = cms.bool(True)
#      )
#    ),
    TrackAssociatorParameters = cms.PSet(
        muonMaxDistanceSigmaX = cms.double(0.0),
        muonMaxDistanceSigmaY = cms.double(0.0),
        CSCSegmentCollectionLabel = cms.InputTag("cscSegments"),
        dRHcal = cms.double(9999.0),
        dREcal = cms.double(9999.0),
        CaloTowerCollectionLabel = cms.InputTag("towerMaker"),
        useEcal = cms.bool(True),
        dREcalPreselection = cms.double(0.05),
        HORecHitCollectionLabel = cms.InputTag("horeco"),
        dRMuon = cms.double(9999.0),
        trajectoryUncertaintyTolerance = cms.double(-1.0),
        crossedEnergyType = cms.string('SinglePointAlongTrajectory'),
        muonMaxDistanceX = cms.double(5.0),
        muonMaxDistanceY = cms.double(5.0),
        useHO = cms.bool(True),
        accountForTrajectoryChangeCalo = cms.bool(False),
        DTRecSegment4DCollectionLabel = cms.InputTag("dt4DSegments"),
        EERecHitCollectionLabel = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
        dRHcalPreselection = cms.double(0.2),
        useMuon = cms.bool(True),
        useCalo = cms.bool(False),
        EBRecHitCollectionLabel = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
        dRMuonPreselection = cms.double(0.2),
        usePreshower = cms.bool(False),
        dRPreshowerPreselection = cms.double(0.2),
        truthMatch = cms.bool(False),
        HBHERecHitCollectionLabel = cms.InputTag("hbhereco"),
        useHcal = cms.bool(True)
    )
)
