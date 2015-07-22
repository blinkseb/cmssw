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
#include "CondFormats/JetMETObjects/interface/JetResolutionObject.h"

class JetResolutionESProducer : public edm::ESProducer
{
    private:
        edm::ParameterSet mParameterSet;
        std::string m_era;
        std::string m_algo;
        std::string m_tag;

    public:
        JetResolutionESProducer(edm::ParameterSet const& fConfig) : mParameterSet(fConfig) 
        {
            std::string label = fConfig.getParameter<std::string>("@module_label"); 
            m_era             = fConfig.getParameter<std::string>("era");
            m_algo            = fConfig.getParameter<std::string>("algorithm");

            m_tag = m_era + "_" + m_algo;

            setWhatProduced(this, m_tag);
        }

        ~JetResolutionESProducer() {}

        boost::shared_ptr<JME::JetResolution> produce(JERRcd const& iRecord) {
            
            // Get object from record
            edm::ESHandle<JetResolutionObject> jerObjectHandle;
            iRecord.get(m_tag, jerObjectHandle);

            // Convert this object to a JetResolution object
            JetResolutionObject const& jerObject = (*jerObjectHandle);
            return boost::shared_ptr<JME::JetResolution>(new JME::JetResolution(jerObject));
        }
};

class JetResolutionScaleFactorESProducer : public edm::ESProducer
{
    private:
        edm::ParameterSet mParameterSet;
        std::string m_era;
        std::string m_algo;
        std::string m_tag;

    public:
        JetResolutionScaleFactorESProducer(edm::ParameterSet const& fConfig) : mParameterSet(fConfig) 
        {
            std::string label = fConfig.getParameter<std::string>("@module_label"); 
            m_era             = fConfig.getParameter<std::string>("era");
            m_algo            = fConfig.getParameter<std::string>("algorithm");

            m_tag = m_era + "_" + m_algo;

            setWhatProduced(this, m_tag);
        }

        ~JetResolutionScaleFactorESProducer() {}

        boost::shared_ptr<JME::JetResolutionScaleFactor> produce(JERRcd const& iRecord) {
            
            // Get object from record
            edm::ESHandle<JetResolutionObject> jerObjectHandle;
            iRecord.get(m_tag, jerObjectHandle);

            // Convert this object to a JetResolution object
            JetResolutionObject const& jerObject = (*jerObjectHandle);
            return boost::shared_ptr<JME::JetResolutionScaleFactor>(new JME::JetResolutionScaleFactor(jerObject));
        }
};
#endif
