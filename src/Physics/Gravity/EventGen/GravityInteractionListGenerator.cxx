//____________________________________________________________________________
/*
*/
//____________________________________________________________________________

#include "Physics/Gravity/EventGen/GravityInteractionListGenerator.h"
#include "Framework/EventGen/InteractionList.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"

using namespace genie;

//___________________________________________________________________________
GravityInteractionListGenerator::GravityInteractionListGenerator() :
InteractionListGeneratorI("genie::GravityInteractionListGenerator")
{

}
//___________________________________________________________________________
GravityInteractionListGenerator::GravityInteractionListGenerator(string config) :
InteractionListGeneratorI("genie::GravityInteractionListGenerator", config)
{

}
//___________________________________________________________________________
GravityInteractionListGenerator::~GravityInteractionListGenerator()
{

}
//___________________________________________________________________________
InteractionList * GravityInteractionListGenerator::CreateInteractionList(
                                      const InitialState & init_state) const
{
  LOG("IntLst", pINFO)
     << "InitialState = " << init_state.AsString();

  int ppdg = init_state.ProbePdg();
  if( !pdg::IsLepton(ppdg) ) {
     LOG("IntLst", pWARN)
       << "Can not handle probe! Returning NULL InteractionList "
       << "for init-state: " << init_state.AsString();
     return 0;
  }

  InteractionList * intlist = new InteractionList;

  vector<int> nucl;
  if (init_state.Tgt().Z()>0)                      nucl.push_back(kPdgProton);
  if (init_state.Tgt().A()-init_state.Tgt().Z()>0) nucl.push_back(kPdgNeutron);

  vector<int>::const_iterator inucl = nucl.begin();
  for( ; inucl != nucl.end(); ++inucl) {
    ProcessInfo proc(kScGravity,kIntGravity);
    Interaction * interaction = new Interaction(init_state, proc);
    interaction->InitStatePtr()->TgtPtr()->SetHitNucPdg(*inucl);
    intlist->push_back(interaction);
  }

  if(intlist->size() == 0) {
     LOG("IntLst", pERROR)
         << "Returning NULL InteractionList for init-state: "
                                                  << init_state.AsString();
     delete intlist;
     return 0;
  }
  return intlist;

}
//___________________________________________________________________________
void GravityInteractionListGenerator::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfigData();
}
//____________________________________________________________________________
void GravityInteractionListGenerator::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfigData();
}
//____________________________________________________________________________
void GravityInteractionListGenerator::LoadConfigData(void)
{
}


