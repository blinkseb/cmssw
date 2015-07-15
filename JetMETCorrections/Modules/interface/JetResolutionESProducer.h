#ifndef JERESProducer_h
#define JERESProducer_h

//
// Author: Sébastien Brochet
//

#include <string>
#include "boost/shared_ptr.hpp"

#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CondFormats/DataRecord/interface/JERRcd.h"
#include "CondFormats/JetMETObjects/interface/JetResolution.h"

class JetResolutionESProducer : public edm::ESProducer
{
    private:
        edm::ParameterSet mParameterSet;
        std::string mAlgo;

    public:
        JetResolutionESProducer(edm::ParameterSet const& fConfig) : mParameterSet(fConfig) 
        {
            std::string label = fConfig.getParameter<std::string>("@module_label"); 
            mAlgo             = fConfig.getParameter<std::string>("algorithm");

            setWhatProduced(this, mAlgo);
        }

        ~JetResolutionESProducer() {}

        boost::shared_ptr<JetResolution> produce(JERRcd const& iRecord) 
        {
            edm::ESHandle<JetResolutionObject> jerObjectHandle;
            iRecord.get(mAlgo, jerObjectHandle); 
            JetResolutionObject const& jerObject = (*jerObjectHandle);
            return boost::shared_ptr<JetResolution>(new JetResolution(jerObject));
        }
};
#endif
