import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(),
    maxEvents   = cms.int32(-1),
    outputEvery = cms.uint32(100000),
)

process.fwliteOutput = cms.PSet(
    fileName  = cms.string('nov17.11signal.71994background.summed.root'),

)

process.addLikelihoods = cms.PSet(
    analysisCommonParameters,
    MaxEta = cms.double(1.5),
    MinNoM = cms.int32(7)
)

process.fwliteInput.fileNames.extend([
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM11eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM13eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM15eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM17eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM19eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM5eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM7eta8.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta0.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta10.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta12.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta14.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta16.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta18.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta2.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta20.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta22.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta4.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta6.root',
    'file:/data3/scooper/data/hscp/428/toyMC/nov17_11signalTracks_71994backgroundTracks/toyMCoutput_NoM9eta8.root',

]);
