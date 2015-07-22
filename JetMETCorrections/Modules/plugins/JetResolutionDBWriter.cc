// Author: Sébastien Brochet

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "CondFormats/JetMETObjects/interface/JetResolution.h"

class JetResolutionDBWriter : public edm::EDAnalyzer
{
 public:
  JetResolutionDBWriter(const edm::ParameterSet&);
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override {}
  virtual void endJob() override {}
  ~JetResolutionDBWriter() {}

 private:
  std::string m_algo;
  std::string m_era;
  std::string m_tag;
  std::string m_path;
};

// Constructor
JetResolutionDBWriter::JetResolutionDBWriter(const edm::ParameterSet& pSet)
{
  m_era  = pSet.getUntrackedParameter<std::string>("era");
  m_algo = pSet.getUntrackedParameter<std::string>("algorithm");
  m_path = pSet.getUntrackedParameter<edm::FileInPath>("file").fullPath();

  m_tag = m_era + "_" + m_algo;
}

// Begin Job
void JetResolutionDBWriter::beginJob()
{

    std::cout << "Loading data from '" << m_path << "'" << std::endl;

    JetResolutionObject* jerObject = new JetResolutionObject(m_path);

    std::cout << "Opening PoolDBOutputService" << std::endl;

    // now write it into the DB
    edm::Service<cond::service::PoolDBOutputService> s;
    if (s.isAvailable()) 
    {

        std::cout << "Setting up payload tag " << m_tag << std::endl;
        cond::Time_t sinceTime =  s->isNewTagRequest(m_tag) ? s->beginOfTime() : s->currentTime();
        s->writeOne<JetResolutionObject>(jerObject, sinceTime, m_tag);

        std::cout << "Object saved into the database with the tag: " << m_tag << std::endl;
    }
}

DEFINE_FWK_MODULE(JetResolutionDBWriter);
