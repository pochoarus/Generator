//____________________________________________________________________________
/*
*/
//____________________________________________________________________________

#include "Physics/Gravity/EventGen/GravityGenerator.h"
#include "Physics/Hadronization/LeptoHadronization.h"
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Numerical/MathUtils.h"
#include "Framework/Numerical/RandomGen.h"
#include "Framework/Conventions/Constants.h"

using namespace genie;
using namespace genie::utils::math;

//___________________________________________________________________________
GravityGenerator::GravityGenerator() :
HadronicSystemGenerator("genie::GravityGenerator")
{
  this->Initialize();
}
//___________________________________________________________________________
GravityGenerator::GravityGenerator(string config) :
HadronicSystemGenerator("genie::GravityGenerator", config)
{
  this->Initialize();
}
//___________________________________________________________________________
GravityGenerator::~GravityGenerator()
{

}
//____________________________________________________________________________
void GravityGenerator::Initialize(void) const
{

}
//___________________________________________________________________________
void GravityGenerator::ProcessEventRecord(GHepRecord * evrec) const
{
// This method generates the final state hadronic system

  //-- Add the primary lepton
  this->AddPrimaryLepton(evrec);

}
//___________________________________________________________________________
void GravityGenerator::AddPrimaryLepton(GHepRecord * evrec) const
{


  Interaction * interaction = evrec->Summary();

  // Neutrino 4p
  LongLorentzVector p4v( * evrec->Probe()->P4() );
  LOG("GravityGenerator", pINFO) << "NEUTRINO @ LAB' =>  E = " << p4v.E() << " //  m = " << p4v.M() << " // p = " << p4v.P();
  LOG("GravityGenerator", pINFO) << "                  dir = " << p4v.Dx() << " , "  << p4v.Dy() << " , "  << p4v.Dz();

  // Look-up selected kinematics & other needed kinematical params
  long double Q2  = interaction->Kine().Q2(true);
  long double y   = interaction->Kine().y(true);
  long double Ev  = p4v.E();
  long double ml  = interaction->FSPrimLepton()->Mass();
  long double ml2 = powl(ml,2);

  // Compute the final state primary lepton energy and momentum components
  // along and perpendicular the neutrino direction
  long double El  = (1-y)*Ev;
  long double plp = El - 0.5*(Q2+ml2)/Ev;                          // p(//)
  long double plt = sqrtl(fmaxl(0.,El*El-plp*plp-ml2)); // p(-|)
  // Randomize transverse components
  RandomGen * rnd = RandomGen::Instance();
  long double phi  = 2 * constants::kPi * rnd->RndLep().Rndm();
  long double pltx = plt * cosl(phi);
  long double plty = plt * sinl(phi);

  // Lepton 4-momentum in the LAB frame
  LongLorentzVector p4llong( pltx, plty, plp, El );
  p4llong.Rotate(p4v);
  LOG("GravityGenerator", pINFO) << "LEPTON     @ LAB' =>  E = " << p4llong.E() << " //  m = " << p4llong.M() << " // p = " << p4llong.P();
  LOG("GravityGenerator", pINFO) << "                    dir = " << p4llong.Dx() << " , "  << p4llong.Dy() << " , "  << p4llong.Dz();

  // Translate from long double to double
  TLorentzVector p4l( (double)p4llong.Px(), (double)p4llong.Py(), (double)p4llong.Pz(), (double)p4llong.E() );

  // Add lepton to EventRecord
  int pdgl = interaction->FSPrimLepton()->PdgCode();
  evrec->AddParticle(pdgl, kIStStableFinalState, evrec->ProbePosition(),-1,-1,-1, p4l, *(evrec->Probe()->X4()));
  evrec->Summary()->KinePtr()->SetFSLeptonP4(p4l);

  LongLorentzVector p4N( * evrec->HitNucleon()->P4() );
  LongLorentzVector p4Hadlong( p4v.Px()+p4N.Px()-p4l.Px(), p4v.Py()+p4N.Py()-p4l.Py(), p4v.Pz()+p4N.Pz()-p4l.Pz(), p4v.E()+p4N.E()-p4l.E() );

  TLorentzVector p4Had( (double)p4Hadlong.Px(), (double)p4Hadlong.Py(), (double)p4Hadlong.Pz(), (double)p4Hadlong.E() );

  const InitialState & init_state = interaction->InitState();
  int nucpdgc = init_state.Tgt().HitNucPdg();
  evrec->AddParticle(nucpdgc, kIStStableFinalState, evrec->ProbePosition(),-1,-1,-1, p4Had, *(evrec->Probe()->X4()));

}
//___________________________________________________________________________
void GravityGenerator::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityGenerator::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityGenerator::LoadConfig(void)
{
}
