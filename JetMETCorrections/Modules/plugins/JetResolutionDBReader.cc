// -*- C++ -*-
//
// Author: Sébastien Brochet 
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "CondFormats/JetMETObjects/interface/JetResolutionObject.h"
#include "CondFormats/DataRecord/interface/JERRcd.h"

//
// class declaration
//

class JetResolutionDBReader : public edm::EDAnalyzer {
public:
  explicit JetResolutionDBReader(const edm::ParameterSet&);
  ~JetResolutionDBReader();
  
  
private:
  virtual void beginJob() override ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override ;
 
  std::string m_algo;
  std::string m_era;
  std::string m_tag;

  bool m_save_file;
  bool m_print;
};


JetResolutionDBReader::JetResolutionDBReader(const edm::ParameterSet& iConfig)
{
  m_era           = iConfig.getUntrackedParameter<std::string>("era");
  m_algo          = iConfig.getUntrackedParameter<std::string>("algorithm");
  m_print         = iConfig.getUntrackedParameter<bool>("dump", true);
  m_save_file     = iConfig.getUntrackedParameter<bool>("saveFile", false);

  m_tag = m_era + "_" + m_algo;
}


JetResolutionDBReader::~JetResolutionDBReader()
{
 
}

void JetResolutionDBReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::ESHandle<JetResolutionObject> jerObjectHandle;
  std::cout << "Inspecting JER payload for tag: "<< m_tag << std::endl;

  iSetup.get<JERRcd>().get(m_tag, jerObjectHandle);

  if (m_print) {
    jerObjectHandle->dump();
  }

  if (m_save_file) {
    jerObjectHandle->saveToFile(m_tag + ".txt");
    std::cout << "JER payload saved as " << m_tag << ".txt" << std::endl;
  }
}

void 
JetResolutionDBReader::beginJob()
{
}

void 
JetResolutionDBReader::endJob() 
{
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetResolutionDBReader);
