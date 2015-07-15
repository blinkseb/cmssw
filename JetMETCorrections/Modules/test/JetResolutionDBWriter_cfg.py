import FWCore.ParameterSet.Config as cms 
process = cms.Process('jerdb')

process.load('CondCore.DBCommon.CondDBCommon_cfi')
process.CondDBCommon.connect = 'sqlite_file:JER_v1.db'
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source('EmptySource')
process.PoolDBOutputService = cms.Service('PoolDBOutputService',
        process.CondDBCommon,
        toPut = cms.VPSet(
            cms.PSet(
                record = cms.string('JER_example'),
                tag    = cms.string('JetResolutionObject_v1_JER_example'),
                label  = cms.string('JER_example')
                ),
            cms.PSet(
                record = cms.string('JER_SF_AK5PF_Run1'),
                tag    = cms.string('JetResolutionObject_v1_JER_SF_AK5PF_Run1'),
                label  = cms.string('JER_SF_AK5PF_Run1')
                ),
            )
        )

process.dbWriterAK4PF_JER = cms.EDAnalyzer('JetResolutionDBWriter',
        algo = cms.untracked.string('JER_example'),
        file = cms.untracked.FileInPath('CondFormats/JetMETObjects/data/JER_example.yml')
        )

process.dbWriterAK4PF_JER_SF = cms.EDAnalyzer('JetResolutionDBWriter',
        algo = cms.untracked.string('JER_SF_AK5PF_Run1'),
        file = cms.untracked.FileInPath('CondFormats/JetMETObjects/data/JER_SF_v1_AK5PF_Run1.yml')
        )

process.p = cms.Path(process.dbWriterAK4PF_JER * process.dbWriterAK4PF_JER_SF)
