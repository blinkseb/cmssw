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
  std::string algo;
  std::string path;
};

// Constructor
JetResolutionDBWriter::JetResolutionDBWriter(const edm::ParameterSet& pSet)
{
  algo   = pSet.getUntrackedParameter<std::string>("algo");
  path   = pSet.getUntrackedParameter<edm::FileInPath>("file").fullPath();
}

// Begin Job
void JetResolutionDBWriter::beginJob()
{

    std::cout << "Loading jet resolution from '" << path << "'" << std::endl;

    JetResolutionObject* jerObject = new JetResolutionObject(path);

    std::cout << "Opening PoolDBOutputService" << std::endl;

    // now write it into the DB
    edm::Service<cond::service::PoolDBOutputService> s;
    if (s.isAvailable()) 
    {

        std::cout << "Setting up payload tag " << algo << std::endl;
        cond::Time_t sinceTime =  s->isNewTagRequest(algo) ? s->beginOfTime() : s->currentTime();
        s->writeOne<JetResolutionObject>(jerObject, sinceTime, algo);
    }
    std::cout << "Wrote in CondDB payload label: " << algo << std::endl;
}

DEFINE_FWK_MODULE(JetResolutionDBWriter);
