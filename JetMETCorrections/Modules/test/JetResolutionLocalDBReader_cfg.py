import FWCore.ParameterSet.Config as cms

process = cms.Process("JERDBLocalReader")

process.load('Configuration.StandardSequences.Services_cff')
process.load("JetMETCorrections.Modules.JetResolutionESProducer_cfi")

from CondCore.DBCommon.CondDBSetup_cfi import *

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))

process.source = cms.Source("EmptySource")

process.PoolDBESSource = cms.ESSource("PoolDBESSource",
      CondDBSetup,
      toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('JERRcd'),
            tag    = cms.string('JetResolutionObject_v1_JER_example'),
            label  = cms.untracked.string('JER_example')
        ),
        cms.PSet(
            record = cms.string('JERRcd'),
            tag    = cms.string('JetResolutionObject_v1_JER_SF_AK5PF_Run1'),
            label  = cms.untracked.string('JER_SF_AK5PF_Run1')
        ),
      ),
      connect = cms.string('sqlite:JER_v1.db')
)


process.demo1 = cms.EDAnalyzer('JetResolutionDBReader', 
        algo = cms.untracked.string('JER_example'),
        createTextFile = cms.untracked.bool(True)
        )

process.demo2 = cms.EDAnalyzer('JetResolutionDBReader', 
        algo = cms.untracked.string('JER_SF_AK5PF_Run1'),
        createTextFile = cms.untracked.bool(True)
        )


process.p = cms.Path(process.demo1 * process.demo2)
