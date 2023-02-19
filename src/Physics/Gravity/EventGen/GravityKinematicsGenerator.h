//____________________________________________________________________________
/*!
*/
//____________________________________________________________________________

#ifndef _GRAVITY_KINEMATICS_GENERATOR_H_
#define _GRAVITY_KINEMATICS_GENERATOR_H_

#include "Physics/Common/KineGeneratorWithCache.h"

namespace genie {

class GravityKinematicsGenerator : public KineGeneratorWithCache {

public :
  GravityKinematicsGenerator();
  GravityKinematicsGenerator(string config);
  ~GravityKinematicsGenerator();

  double ComputeMaxXSec       (const Interaction * interaction) const;

  // implement the EventRecordVisitorI interface
  void ProcessEventRecord(GHepRecord * event_rec) const;

  // overload the Algorithm::Configure() methods to load private data
  // members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:

  double Scan(Interaction * interaction, Range1D_t xrange,Range1D_t Q2range, int NKnotsQ2, int NKnotsX, double ME2, double & x_scan, double & Q2_scan) const;

  void   LoadConfig           (void);

  double fM5;   ///< Scale of gravity

};

}      // genie namespace

#endif // _Gravity_KINEMATICS_GENERATOR_H_
