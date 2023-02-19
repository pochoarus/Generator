//____________________________________________________________________________
/*
*/
//____________________________________________________________________________

#include "Physics/Gravity/XSection/GravityPXSec.h"
#include "Physics/XSectionIntegration/XSecIntegratorI.h"
#include "Framework/Conventions/Constants.h"
#include "Framework/Conventions/RefFrame.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/ParticleData/PDGUtils.h"
#include "Framework/Utils/KineUtils.h"

#include <TMath.h>

#include "LHAPDF/LHAPDF.h"

using namespace genie;
using namespace genie::constants;

LHAPDF::PDF* pdf;

//____________________________________________________________________________
GravityPXSec::GravityPXSec() :
XSecAlgorithmI("genie::GravityPXSec")
{
}
//____________________________________________________________________________
GravityPXSec::GravityPXSec(string config) :
XSecAlgorithmI("genie::GravityPXSec", config)
{

}
//____________________________________________________________________________
GravityPXSec::~GravityPXSec()
{

}
//____________________________________________________________________________
double GravityPXSec::XSec(
     const Interaction * interaction, KinePhaseSpace_t kps) const
{

  const Kinematics  & kinematics  = interaction -> Kine();
  const InitialState & init_state = interaction -> InitState();

  double E     = init_state.ProbeE(kRfLab);
  double Mnuc  = init_state.Tgt().HitNucMass();
  double s     = 2.*Mnuc*E;

  double x     = kinematics.x();
  double Q2    = kinematics.Q2();
  double y     = kinematics.y();

  if (y>fymax) return 0;

  if (Q2<fQ2min) return 0;

  double Q    = TMath::Sqrt(Q2); 
   
  double bc   = x*s/2./TMath::Power(fM5,3);

  double mu = (Q>1./bc) ? Q2*bc : Q;

  double xsec = SumPDF(x,mu)*TMath::Power(Aeik(x*s,bc,Q),2);  

  xsec *= 1./16./TMath::Pi()/TMath::Power(x*s,2);

  LOG("GravityPXSec", pINFO) << "d2xsec/dxdy[FreeN] (x= " << x  << ", y= " << y << ", Q2= " << Q2 << ") = " << xsec;

  // The algorithm computes d^2xsec/dlog10xdlog10y
  if( kps!=kPSxQ2fE ) xsec *= utils::kinematics::Jacobian(interaction,kPSxQ2fE,kps);

  // If requested return the free nucleon xsec even for input nuclear tgt 
  if( interaction->TestBit(kIAssumeFreeNucleon) ) return xsec;

  // Compute nuclear cross section (simple scaling here, corrections must have been included in the structure functions)
  int NNucl = (pdg::IsProton(init_state.Tgt().HitNucPdg())) ? init_state.Tgt().Z() : init_state.Tgt().N(); 
  xsec *= NNucl; 

  return xsec;

}
//____________________________________________________________________________
double GravityPXSec::Aeik(double s, double bc, double Q) const
{
  double uval = bc*Q;
  double f1   = 1./TMath::Sqrt(1.57*TMath::Power(uval,3)+TMath::Power(uval,2));

  return 4*TMath::Pi()*s*TMath::Power(bc,2)*f1*( (1/TMath::TanH(TMath::Pi()*Q/fmc)) - fmc/(TMath::Pi()*Q) );
}
//____________________________________________________________________________
double GravityPXSec::SumPDF(double x, double Q) const
{
    double g     = TMath::Max(pdf->xfxQ( 0,x,Q),0.);
    double d     = TMath::Max(pdf->xfxQ( 1,x,Q),0.);
    double u     = TMath::Max(pdf->xfxQ( 2,x,Q),0.);
    double s     = TMath::Max(pdf->xfxQ( 3,x,Q),0.);
    double c     = TMath::Max(pdf->xfxQ( 4,x,Q),0.);
    double b     = TMath::Max(pdf->xfxQ( 5,x,Q),0.);
    double dbar  = TMath::Max(pdf->xfxQ(-1,x,Q),0.);
    double ubar  = TMath::Max(pdf->xfxQ(-2,x,Q),0.);
    double sbar  = TMath::Max(pdf->xfxQ(-3,x,Q),0.);
    double cbar  = TMath::Max(pdf->xfxQ(-4,x,Q),0.);
    double bbar  = TMath::Max(pdf->xfxQ(-5,x,Q),0.);  
    return (g+d+u+s+c+b+dbar+ubar+sbar+cbar+bbar)/x;
    //return (g+d+u+s+dbar+ubar+sbar)/x;
}
//____________________________________________________________________________
double GravityPXSec::Integral(const Interaction * interaction) const
{

  return fXSecIntegrator->Integrate(this,interaction);

}
//____________________________________________________________________________
bool GravityPXSec::ValidProcess(const Interaction * interaction) const
{

  if(interaction->TestBit(kISkipProcessChk)) return true;

  const ProcessInfo & proc_info  = interaction->ProcInfo();
  if(!proc_info.IsGravity()) return false;

  const InitialState & init_state = interaction -> InitState();
  int probe_pdg = init_state.ProbePdg();
  if(!pdg::IsLepton(probe_pdg)) return false;

  if(! init_state.Tgt().HitNucIsSet()) return false;

  int hitnuc_pdg = init_state.Tgt().HitNucPdg();
  if(!pdg::IsNeutronOrProton(hitnuc_pdg)) return false;

  return true;
}
//____________________________________________________________________________
void GravityPXSec::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityPXSec::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityPXSec::LoadConfig(void)
{

  //-- load the differential cross section integrator
  fXSecIntegrator = dynamic_cast<const XSecIntegratorI *> (this->SubAlg("XSec-Integrator"));
  assert(fXSecIntegrator);

  // Information about Structure Functions
  GetParam("LHAPDF-set",      fPDFset    );
  GetParam("LHAPDF-member",   fPDFmember );
  GetParam("M5",              fM5        );
  GetParam("mc",              fmc        );
  GetParam("Q2min",           fQ2min     ) ;
  GetParam("ymax",            fymax      ) ;

  pdf = LHAPDF::mkPDF(fPDFset, fPDFmember);

}