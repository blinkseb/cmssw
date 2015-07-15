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

#include "CondFormats/JetMETObjects/interface/JetResolution.h"
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
 
  std::string mAlgo;
  bool mCreateTextFile, mPrintScreen;
};


JetResolutionDBReader::JetResolutionDBReader(const edm::ParameterSet& iConfig)
{
  mAlgo           = iConfig.getUntrackedParameter<std::string>("algo");
  mPrintScreen    = iConfig.getUntrackedParameter<bool>("printScreen", true);
  mCreateTextFile = iConfig.getUntrackedParameter<bool>("createTextFile", false);
}


JetResolutionDBReader::~JetResolutionDBReader()
{
 
}

void JetResolutionDBReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::ESHandle<JetResolutionObject> jerObjectHandle;
  std::cout << "Inspecting JER payload for algo: "<< mAlgo << std::endl;
  iSetup.get<JERRcd>().get(mAlgo, jerObjectHandle);

  if (mPrintScreen) {
    jerObjectHandle->dump();
  }

  if (mCreateTextFile) {
    jerObjectHandle->saveToFile(mAlgo + ".yml");
    std::cout << "JER payload saved as " << mAlgo << ".yml" << std::endl;
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
