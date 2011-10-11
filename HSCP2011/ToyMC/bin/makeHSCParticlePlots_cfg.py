import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(),
    maxEvents   = cms.int32(-1),
    outputEvery = cms.uint32(100000),
)

process.fwliteOutput = cms.PSet(
    #fileName  = cms.string('2.oct5.makeHSCParticlePlots.ZJetToMuMuPt30to50.root'),
    fileName  = cms.string('2.oct7.makeHSCParticlePlots.ZJetToMuMuPt120to170.root'),
    #fileName  = cms.string('1.oct7.makeHSCParticlePlots.EPSrunPostEPSprod.root'),
)

process.fwliteInput.fileNames.extend([
    # EPS runs -- from post EPS production (trigger thresh different?)
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_160404_163869.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_165001_166033.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166034_166500.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166894_167151.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_166501_166893.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/Data_RunA_167153_167913.root',
    # Z+Jet --> MuMu MonteCarlo Summer11
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-0to15.root',
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-120to170.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-15to20.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-170to230.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-20to30.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-230to300.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-300.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-30to50.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-50to80.root',
    #'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_ZJetToMuMu_Pt-80to120.root',


]);
