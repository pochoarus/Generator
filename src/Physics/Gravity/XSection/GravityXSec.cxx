//____________________________________________________________________________
/*
*/
//____________________________________________________________________________

#include "Physics/Gravity/XSection/GravityXSec.h"
#include "Physics/XSectionIntegration/GSLXSecFunc.h"
#include "Framework/Conventions/Constants.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/ParticleData/PDGCodes.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/Utils/Range1.h"
#include "Framework/Utils/XSecSplineList.h"
#include "Framework/Utils/RunOpt.h"
#include "Framework/Numerical/Spline.h"
#include "Framework/Numerical/GSLUtils.h"

#include <TMath.h>
#include <Math/IFunction.h>
#include <Math/IntegratorMultiDim.h>

using namespace genie;
using namespace genie::constants;

//____________________________________________________________________________
GravityXSec::GravityXSec() :
XSecIntegratorI("genie::GravityXSec")
{

}
//____________________________________________________________________________
GravityXSec::GravityXSec(string config) :
XSecIntegratorI("genie::GravityXSec", config)
{

}
//____________________________________________________________________________
GravityXSec::~GravityXSec()
{

}
//____________________________________________________________________________
double GravityXSec::Integrate(
                 const XSecAlgorithmI * model, const Interaction * in) const
{

  if(! model->ValidProcess(in) ) return 0.;

  const InitialState & init_state = in->InitState();
  double Ev = init_state.ProbeE(kRfLab);
 
  // If the input interaction is off a nuclear target, then chek whether 
  // the corresponding free nucleon cross section already exists at the 
  // cross section spline list. 
  // If yes, calculate the nuclear cross section based on that value.
  //
  XSecSplineList * xsl = XSecSplineList::Instance();
  if(init_state.Tgt().IsNucleus() && !xsl->IsEmpty() ) {

    int nucpdgc = init_state.Tgt().HitNucPdg();
    int NNucl   = (pdg::IsProton(nucpdgc)) ? init_state.Tgt().Z() : init_state.Tgt().N();

    Interaction * interaction = new Interaction(*in);
    Target * target = interaction->InitStatePtr()->TgtPtr();
    if(pdg::IsProton(nucpdgc)) { target->SetId(kPdgTgtFreeP); }
    else                       { target->SetId(kPdgTgtFreeN); }
    if(xsl->SplineExists(model,interaction)) {
      const Spline * spl = xsl->GetSpline(model, interaction);
      double xsec = spl->Evaluate(Ev);
      LOG("GravityXSec", pINFO) << "From XSecSplineList: XSec[Gravity,free nucleon] (E = " << Ev << " GeV) = " << xsec;
      if( !interaction->TestBit(kIAssumeFreeNucleon) ) { 
          xsec *= NNucl; 
          LOG("GravityXSec", pINFO)  << "XSec[Gravity] (E = " << Ev << " GeV) = " << xsec;
      }
      delete interaction;
      return xsec;
    }
    delete interaction;
  }

  double M = init_state.Tgt().HitNucMass();
  double s = 2.*M*Ev;

  double log10xmin  = TMath::Log10(TMath::Power(fM5,2)/s);
  double log10xmax  = TMath::Log10(1.);
  double log10Q2min = TMath::Log10(fQ2min);
  double log10Q2max = TMath::Log10(s);

  // Just go ahead and integrate the input differential cross section for the 
  // specified interaction.
  //
  double xsec = 0.;

  if ( log10xmin<0. ) {

    Interaction * interaction = new Interaction(*in);
    
    // If a GSL option has been chosen, then the total xsec is recomptued
    ROOT::Math::IBaseFunctionMultiDim * func = new utils::gsl::d2XSec_dlog10xdlog10Q2_E(model, interaction);
    ROOT::Math::IntegrationMultiDim::Type ig_type = utils::gsl::IntegrationNDimTypeFromString(fGSLIntgType);
    double abstol = 0; //In vegas we care about number of interactions
    double reltol = 0; //In vegas we care about number of interactions
    ROOT::Math::IntegratorMultiDim ig(*func, ig_type, abstol, reltol, fGSLMaxEval);
    double kine_min[2] = { log10xmin, log10Q2min };
    double kine_max[2] = { log10xmax, log10Q2max };
    xsec = ig.Integral(kine_min, kine_max) * (1E-38 * units::cm2);
    delete func;

    delete interaction;

  }

  LOG("GravityXSec", pINFO)  << "XSec[Gravity] (E = " << Ev << " GeV) = " << xsec * (1E+38/units::cm2) << " x 1E-38 cm^2";

  return xsec;

}
//____________________________________________________________________________
void GravityXSec::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityXSec::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityXSec::LoadConfig(void)
{

  // Get GSL integration type & relative tolerance
  GetParamDef( "gsl-integration-type", fGSLIntgType, string("vegas") ) ;

  int max_eval ;
  GetParamDef( "gsl-max-eval", max_eval, 500000 ) ;
  fGSLMaxEval  = (unsigned int) max_eval ;

  GetParam("M5",     fM5    ) ;
  GetParam("Q2min",  fQ2min ) ;

}
