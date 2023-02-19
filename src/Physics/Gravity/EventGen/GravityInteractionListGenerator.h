//____________________________________________________________________________
/*!
*/
//____________________________________________________________________________

#ifndef _GRAVITY_INTERACTION_LIST_GENERATOR_H_
#define _GRAVITY_INTERACTION_LIST_GENERATOR_H_

#include <map>

#include "Framework/EventGen/InteractionListGeneratorI.h"
#include "Framework/Interaction/Interaction.h"

using std::multimap;

namespace genie {

class Interaction;

class GravityInteractionListGenerator : public InteractionListGeneratorI {

public :
  GravityInteractionListGenerator();
  GravityInteractionListGenerator(string config);
 ~GravityInteractionListGenerator();

  // implement the InteractionListGeneratorI interface
  InteractionList * CreateInteractionList(const InitialState & init) const;

  // overload the Algorithm::Configure() methods to load private data
  // members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:
  void LoadConfigData(void);
  
};

}      // genie namespace

#endif // _Gravity_INTERACTION_LIST_GENERATOR_H_
