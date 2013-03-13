import FWCore.ParameterSet.Config as cms

process = cms.Process("HCALFWAnalyzer")

#process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
     #suppressInfo = cms.untracked.vstring(),
     cout = cms.untracked.PSet(
               threshold = cms.untracked.string('WARNING')
           ),
     categories = cms.untracked.vstring('*'),
     destinations = cms.untracked.vstring('cout')
)


##-- GT conditions for all
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.autoCond import autoCond
process.GlobalTag.globaltag = autoCond['com10'] ## == GR_R_53_V16::All in 5_3_7
###-- Customized particular conditions
from CondCore.DBCommon.CondDBSetup_cfi import *

#----------------------------------- replacing conditions with txt ones
process.es_ascii = cms.ESSource("HcalTextCalibrations",
 input = cms.VPSet(
   cms.PSet(
     object = cms.string('ElectronicsMap'),
     file = cms.FileInPath('HCALSourcing/HCALFWAnalyzer/emap_H2_25oct2012.txt')
   )
 )
)
process.es_prefer = cms.ESPrefer('HcalTextCalibrations','es_ascii')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("HcalTBSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:HTB_006062.root'
        #'file:HTB_006036.root'
        #file:HTB_006080.root'
        #'file:HTB_006067.root'
        #'file:HTB_006058.root'
        # long run at 100 Hz
        #'file:HTB_006087.root',
        #'file:HTB_006087.1.root',
        #'file:HTB_006087.2.root'
    )
)

# TB data unpacker
#process.load('RecoTBCalo.HcalTBObjectUnpacker.HcalTBObjectUnpacker_SourceCal_cfi')
#process.tbunpack = cms.EDProducer("HcalTBObjectUnpacker",
#    HcalSlowDataFED = cms.untracked.int32(12),
#    HcalSourcePositionFED = cms.untracked.int32(-1),
#    HcalTriggerFED = cms.untracked.int32(1)
#)

process.tbunpack = cms.EDProducer("HcalTBObjectUnpacker",
    HcalSlowDataFED = cms.untracked.int32(-1),
    HcalSourcePositionFED = cms.untracked.int32(12),
    HcalTriggerFED = cms.untracked.int32(1),
    fedRawDataCollectionTag = cms.InputTag('rawDataCollector')
)

# histo unpacker
process.load("EventFilter.HcalRawToDigi.HcalHistogramRawToDigi_cfi")
process.hcalhistos.HcalFirstFED = cms.untracked.int32(700)
process.hcalhistos.FEDs = cms.untracked.vint32(700)

process.TFileService = cms.Service("TFileService", 
    fileName = cms.string("hcalFWAnalyzer.root"),
)

process.hcalFWAnalyzer = cms.EDAnalyzer('HCALFWAnalyzer'
)

process.load("FWCore.Modules.printContent_cfi")

process.p = cms.Path(process.tbunpack
                     *process.hcalhistos
                     #*process.printContent
                     *process.hcalFWAnalyzer
                    )


