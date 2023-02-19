//____________________________________________________________________________
/*!
*/
//____________________________________________________________________________

#ifndef _GRAVITY_GENERATOR_H_
#define _GRAVITY_GENERATOR_H_

#include "Physics/Common/HadronicSystemGenerator.h"


namespace genie {

class GravityGenerator : public HadronicSystemGenerator {

public :
  GravityGenerator();
  GravityGenerator(string config);
 ~GravityGenerator();

  // implement the EventRecordVisitorI interface
  void Initialize        (void)               const;
  void ProcessEventRecord(GHepRecord * evrec) const;

  // overload the Algorithm::Configure() methods to load private data
  // members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:

  void AddPrimaryLepton         (GHepRecord * evrec) const;

  void LoadConfig (void);

};

}      // genie namespace

#endif // _Gravity_HADRONIC_SYSTEM_GENERATOR_H_
