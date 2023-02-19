//____________________________________________________________________________
/*!
*/
//____________________________________________________________________________

#ifndef _GRAVITY_XSEC_H_
#define _GRAVITY_XSEC_H_

#include "Physics/XSectionIntegration/XSecIntegratorI.h"

namespace genie {

class GravityXSec : public XSecIntegratorI {

public:
  GravityXSec();
  GravityXSec(string config);
  virtual ~GravityXSec();

  //! XSecIntegratorI interface implementation
  double Integrate(const XSecAlgorithmI * model, const Interaction * i) const;

  //! Overload the Algorithm::Configure() methods to load private data
  //! members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:
  void   LoadConfig (void);

  double fM5;   ///< Scale of gravity
  double fQ2min;   ///< Scale of gravity

};

}       // genie namespace
#endif  // _DIS_XSEC_H_
