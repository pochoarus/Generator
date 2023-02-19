//____________________________________________________________________________
/*!
*/
//____________________________________________________________________________

#ifndef _GRAVITY_PXSEC_H_
#define _GRAVITY_PXSEC_H_

#include "Framework/EventGen/XSecAlgorithmI.h"

namespace genie {

  class XSecIntegratorI;

  class GravityPXSec : public XSecAlgorithmI {

    public:
      GravityPXSec();
      GravityPXSec(string config);
      virtual ~GravityPXSec();

      // XSecAlgorithmI interface implementation
      double XSec            (const Interaction * i, KinePhaseSpace_t k) const;
      double Integral        (const Interaction * i) const;
      bool   ValidProcess    (const Interaction * i) const;

      // overload the Algorithm::Configure() methods to load private data
      // members from configuration options
      void Configure(const Registry & config);
      void Configure(string config);

    private:
      void   LoadConfig (void);
      
      double Aeik(double s, double bc, double Q) const;
      double SumPDF(double x, double Q) const;

      const XSecIntegratorI *        fXSecIntegrator;     ///< diff. xsec integrator
    
      double fM5;            ///< Scale of gravity
      double fmc;            ///< Scale of gravity
      double fQ2min;         ///< Scale of gravity
      double fymax;          ///< Scale of gravity

      string fPDFset;
      int    fPDFmember;

  };

}       // genie namespace

#endif  // _Gravity_PXSEC_H_
