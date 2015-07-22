import FWCore.ParameterSet.Config as cms

JetResolutionESProducer_AK4PFchs = cms.ESProducer("JetResolutionESProducer",
        era = cms.string('Summer15_v5_MC_JER'),
        algorithm = cms.string('AK4PFchs')
)

JetResolutionESProducer_SF_AK4PFchs = cms.ESProducer("JetResolutionScaleFactorESProducer",
        era = cms.string('Summer15_v5_MC_JER_SF'),
        algorithm = cms.string('AK4PFchs')
)
