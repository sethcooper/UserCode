import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(),
    maxEvents   = cms.int32(-1),
    outputEvery = cms.uint32(100000),
)

process.fwliteOutput = cms.PSet(
    #fileName  = cms.string('2.oct5.makeHSCParticlePlots.ZJetToMuMuPt30to50.root'),
    #fileName  = cms.string('1.oct7.makeHSCParticlePlots.EPSrunPostEPSprod.root'),

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
    fileName  = cms.string('oct14.makeHSCParticlePlots.DYToMuMu.root'),
)

process.makeHSCParticlePlots = cms.PSet(
    MassCutIasHighPHighIh = cms.double(100), # this is for the high Ih/high P Ias dist, e.g., "search region"
    #ScaleFactor = cms.double(37.93) # ZJetToMuMuPt0-15
    #ScaleFactor = cms.double(1.24) # ZJetToMuMuPt15-20
    #ScaleFactor = cms.double(16.19) # ZJetToMuMuPt20-30
    ScaleFactor = cms.double(1) # ZJetToMuMuPt30-50
    #ScaleFactor = cms.double(0.56) # ZJetToMuMuPt50-80
    #ScaleFactor = cms.double(0.168) # ZJetToMuMuPt80-120
    #ScaleFactor = cms.double(0.046) # ZJetToMuMuPt120-170
    #ScaleFactor = cms.double(0.012) # ZJetToMuMuPt170-230
    #ScaleFactor = cms.double(0.00332) # ZJetToMuMuPt230-300
    #ScaleFactor = cms.double(0.001279) # ZJetToMuMuPt300
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
    'file:/data3/scooper/data/hscp/428/Data/HSCParticles_HSCP2011_PostEPS_Sep30/MC_DYToMuMu.root',

]);

