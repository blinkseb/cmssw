#include "CondFormats/JetMETObjects/src/headers.h"

#include <vector>

JetCorrectorParameters corr;
JetCorrectorParameters::Definitions def;
JetCorrectorParameters::Record record;
std::vector<JetCorrectorParameters> corrv;
std::vector<JetCorrectorParameters::Record> recordv;
JetCorrectorParametersCollection coll;
JetCorrectorParametersCollection::pair_type pair_type;
JetCorrectorParametersCollection::collection_type colltype;
std::vector<JetCorrectorParametersCollection> collv;
FFTJetCorrectorParameters fftcorr;
QGLikelihoodCategory qgcat;
QGLikelihoodObject qgobj;
QGLikelihoodObject::Entry qgentry;
std::vector< QGLikelihoodObject::Entry > qgentryv;
QGLikelihoodSystematicsObject qgsystobj;
QGLikelihoodSystematicsObject::Entry qgsystentry;
std::vector< QGLikelihoodSystematicsObject::Entry > qgsystentryv;
METCorrectorParameters METcorr;
JetResolutionObject jerobj;
JetResolutionObject::Definition jerdef;
JetResolutionObject::Record jerrecord;
JetResolutionObject::Variable jervar;
JetResolutionObject::Bin jerbin;
std::vector<JetResolutionObject::Record> jerrecordvec;
std::vector<JetResolutionObject::Variable> jervarvec;
std::vector<JetResolutionObject::Bin> jerbinvec;
JetResolution jer;
JetResolutionScaleFactor jer_sf;
