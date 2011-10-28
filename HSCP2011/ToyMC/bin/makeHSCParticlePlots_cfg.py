import FWCore.ParameterSet.Config as cms

process = cms.PSet()

from HSCP2011.ToyMC.makeHSCParticlePlotsAndDoLikelihoodFitsCommonParameters_cfi import *

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(),
    maxEvents   = cms.int32(-1),
    #maxEvents   = cms.int32(200000),
    outputEvery = cms.uint32(100000),
)

process.fwliteOutput = cms.PSet(
    #fileName  = cms.string('2.oct5.makeHSCParticlePlots.ZJetToMuMuPt30to50.root'),
    #fileName  = cms.string('oct17.makeHSCParticlePlots.EPSrunPostEPSprod.100GeVpSB.root'),
    #fileName  = cms.string('oct17.makeHSCParticlePlots.EPSrunPostEPSprod.50GeVpSB.root'),
    #fileName  = cms.string('oct18.makeHSCParticlePlots.EPSrunPostEPSprod.100GeVpSB.root'),
    #fileName  = cms.string('oct26.makeHSCParticlePlots.jetTriggeredData.100GeVpSB.root'),
    #fileName  = cms.string('oct18.makeHSCParticlePlots.EPSrunPostEPSprod.100GeVpSB.root'),
    #fileName  = cms.string('oct27.NewTest.100kevts.makeHSCParticlePlots.EPSrunPostEPSprod.100GeVpSB.root'),
    #fileName  = cms.string('oct28.new.makeHSCParticlePlots.EPSrunPostEPSprod.100GeVpSB.3p5MeVcmIhSB.root'),
    fileName  = cms.string('oct28.withRooDataSet.makeHSCParticlePlots.EPSrunPostEPSprod.root'),

    #fileName  = cms.string('1.oct12.makeHSCParticlePlots.ZJetToMuMuPt0to15.root'),
    #fileName  = cms.string('2.oct12.makeHSCParticlePlots.ZJetToMuMuPt15to20.root'),
    #fileName  = cms.string('3.oct12.makeHSCParticlePlots.ZJetToMuMuPt20to30.root'),
    #fileName  = cms.string('4.oct12.makeHSCParticlePlots.ZJetToMuMuPt30to50.root'),
    #fileName  = cms.string('5.oct12.makeHSCParticlePlots.ZJetToMuMuPt50to80.root'),
    #fileName  = cms.string('6.oct12.makeHSCParticlePlots.ZJetToMuMuPt80to120.root'),
    #fileName  = cms.string('7.oct12.makeHSCParticlePlots.ZJetToMuMuPt120to170.root'),
    #fileName  = cms.string('8.oct12.makeHSCParticlePlots.ZJetToMuMuPt170to230.root'),
    #fileName  = cms.string('9.oct12.makeHSCParticlePlots.ZJetToMuMuPt230to300.root'),
    #fileName  = cms.string('10.oct12.makeHSCParticlePlots.ZJetToMuMuPt300.root'),
    #fileName  = cms.string('oct14.makeHSCParticlePlots.DYToMuMu.root'),
    #fileName  = cms.string('oct14.test.EPSrunPostEPSprod.varBins.root'),
    #fileName  = cms.string('oct17.test.GMSBstau100.matched.varBins.root'),
    #fileName  = cms.string('oct17.test.GMSBstau200.matched.varBins.root'),
)

process.makeHSCParticlePlots = cms.PSet(
    analysisCommonParameters,
    MatchToHSCP = cms.bool(False),
    ScaleFactor = cms.double(1)
)

#print 'Running with p threshold: ',
#print analysisCommonParameters.PSidebandThreshold
#print 'Running with Ih threshold:',
#print analysisCommonParameters.IhSidebandThreshold

process.fwliteInput.fileNames.extend([
    # EPS runs -- from post EPS production (trigger thresh different?)
    # ==> 1.09/fb
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_160404_163869.root',
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_165001_166033.root',
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166034_166500.root',
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166894_167151.root',
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166501_166893.root',
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_167153_167913.root',

    # Jet-triggered data; should be same run range as EPS runs
    #'file:/data3/scooper/data/hscp/428/Data/MyProcessing_SkimAndHSCParticles/RunA_May10ReReco_Jet_160404_163869/filtered_merged.root',
    #'file:/data3/scooper/data/hscp/428/Data/MyProcessing_SkimAndHSCParticles/RunA_PromptRecoV4_Jet_165001_166033/filtered_merged.root',
    #'file:/data3/scooper/data/hscp/428/Data/MyProcessing_SkimAndHSCParticles/RunA_PromptRecoV4_Jet_166034_166500/filtered_merged.root',
    #'file:/data3/scooper/data/hscp/428/Data/MyProcessing_SkimAndHSCParticles/RunA_PromptRecoV4_Jet_166894_167151/filtered_merged.root',
    #'file:/data3/scooper/data/hscp/428/Data/MyProcessing_SkimAndHSCParticles/RunA_PromptRecoV4_Jet_166501_166893/filtered_merged.root',
    #'file:/data3/scooper/data/hscp/428/Data/MyProcessing_SkimAndHSCParticles/RunA_PromptRecoV4_Jet_167153_167913/filtered_merged.root',

    # Z+Jet --> MuMu MonteCarlo Summer11
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-0to15.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-15to20.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-20to30.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-30to50.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-50to80.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-80to120.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-120to170.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-170to230.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-230to300.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-300.root',
    # DY (m20) to MuMu
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_DYToMuMu.root',
    # GMSB STAU 100
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/GMStau100.root'
    # GMSB STAU 200
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/GMStau200.root'

]);

