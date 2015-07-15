import FWCore.ParameterSet.Config as cms

JetResolutionESProducer_AK4PF = cms.ESProducer("JetResolutionESProducer",
    algorithm = cms.string('JER_example')
)

JetResolutionESProducer_SF_AK4PF = cms.ESProducer("JetResolutionESProducer",
    algorithm = cms.string('JER_SF_AK5PF_Run1')
)
