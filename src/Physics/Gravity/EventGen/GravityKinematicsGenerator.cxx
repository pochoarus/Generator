//____________________________________________________________________________
/*
*/
//____________________________________________________________________________

#include "Physics/Gravity/EventGen/GravityKinematicsGenerator.h"
#include "Framework/Conventions/Controls.h"
#include "Framework/Conventions/KineVar.h"
#include "Framework/Conventions/KinePhaseSpace.h"
#include "Framework/EventGen/EVGThreadException.h"
#include "Framework/EventGen/EventGeneratorI.h"
#include "Framework/EventGen/RunningThreadInfo.h"
#include "Framework/GHEP/GHepRecord.h"
#include "Framework/GHEP/GHepFlags.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/Numerical/RandomGen.h"
#include "Framework/Utils/KineUtils.h"
#include "Framework/Utils/Range1.h"
#include "Framework/Utils/RunOpt.h"

#include <TMath.h>

#include <string>

using namespace genie;
using namespace genie::controls;
using namespace genie::utils;

//___________________________________________________________________________
GravityKinematicsGenerator::GravityKinematicsGenerator() :
KineGeneratorWithCache("genie::GravityKinematicsGenerator")
{

}
//___________________________________________________________________________
GravityKinematicsGenerator::GravityKinematicsGenerator(string config) :
KineGeneratorWithCache("genie::GravityKinematicsGenerator", config)
{

}
//___________________________________________________________________________
GravityKinematicsGenerator::~GravityKinematicsGenerator()
{

}
//___________________________________________________________________________
void GravityKinematicsGenerator::ProcessEventRecord(GHepRecord * evrec) const
{

  //-- Get the random number generators
  RandomGen * rnd = RandomGen::Instance();

  //-- Access cross section algorithm for running thread
  RunningThreadInfo * rtinfo = RunningThreadInfo::Instance();
  const EventGeneratorI * evg = rtinfo->RunningThread();
  fXSecModel = evg->CrossSectionAlg();

  //-- Get the interaction 
  Interaction * interaction = evrec->Summary();
  interaction->SetBit(kISkipProcessChk);

  //-- Get neutrino energy and hit 'nucleon mass' 
  const InitialState & init_state = interaction->InitState();
  double Ev  = init_state.ProbeE(kRfLab);
  double M   = init_state.Tgt().HitNucP4().M(); // can be off m-shell
  double s   = 2*M*Ev;

  //-- Get the physical W range 
  const KPhaseSpace & kps = interaction->PhaseSpace();
  Range1D_t xl;
  Range1D_t Q2l;

  //-- x and y lower limit restrict by limits in SF tables
  Q2l.min = 1.;
  Q2l.max = s;
  xl.min  = TMath::Power(fM5,2)/s;
  xl.max  = 1.-1e-10;

  LOG("GravityKinematics", pNOTICE) << "x: [" << xl.min << ", " << xl.max << "]"; 
  LOG("GravityKinematics", pNOTICE) << "Q2: [" << Q2l.min << ", " << Q2l.max << "]"; 

  //-- For the subsequent kinematic selection with the rejection method:
  
  //Scan through a wide region to find the maximum
  Range1D_t xrange_wide(xl.min,xl.max); 
  Range1D_t Q2range_wide(Q2l.min,Q2l.max); 
  double x_wide    = 0.;
  double Q2_wide   = 0.;
  double xsec_wide = this->Scan(interaction,xrange_wide,Q2range_wide,10,10,2*M*Ev,x_wide,Q2_wide);

  //Scan through a fine region to find the maximum
  Range1D_t xrange_fine(TMath::Max(x_wide/5,xrange_wide.min),TMath::Min(x_wide*5,xrange_wide.max)); 
  Range1D_t Q2range_fine(TMath::Max(Q2_wide/5,Q2range_wide.min),TMath::Min(Q2_wide*5,Q2range_wide.max)); 
  double x_fine    = 0.;
  double Q2_fine   = 0.;
  double xsec_fine = this->Scan(interaction,xrange_fine,Q2range_fine,20,20,2*M*Ev,x_fine,Q2_fine);

  //Apply safety factor
  double xsec_max = fSafetyFactor * TMath::Max(xsec_wide,xsec_fine);

  //-- Try to select a valid (x,y) pair using the rejection method
  double log10xmin  = TMath::Log10(xl.min);  
  double log10xmax  = TMath::Log10(xl.max); 
  double log10Q2min = TMath::Log10(Q2l.min);  
  double log10Q2max = TMath::Log10(Q2l.max); 

  double dlog10x = log10xmax - log10xmin; 
  double dlog10Q2 = log10Q2max - log10Q2min; 
  
  double gx=-1, gQ2=-1, xsec=-1;

  unsigned int iter = 0;
  bool accept = false;
  while(1) {
     iter++;
     if(iter > kRjMaxIterations) {
       LOG("GravityKinematics", pWARN)
         << " Couldn't select kinematics after " << iter << " iterations";
       evrec->EventFlags()->SetBitNumber(kKineGenErr, true);
       genie::exceptions::EVGThreadException exception;
       exception.SetReason("Couldn't select kinematics");
       exception.SwitchOnFastForward();
       throw exception;
     }
    
     gx = TMath::Power( 10., log10xmin + dlog10x * rnd->RndKine().Rndm() ); 
     gQ2 = TMath::Power( 10., log10Q2min + dlog10Q2 * rnd->RndKine().Rndm() ); 

     interaction->KinePtr()->Setx(gx);
     interaction->KinePtr()->SetQ2(gQ2);
     kinematics::UpdateWYFromXQ2(interaction);

     LOG("GravityKinematics", pDEBUG) 
        << "Trying: x = " << gx << ", Q2 = " << gQ2 
        << " (W  = " << interaction->KinePtr()->W()  << ","
        << "  y = " << interaction->KinePtr()->y() << ")";

     //-- compute the cross section for current kinematics
     xsec = fXSecModel->XSec(interaction, kPSlog10xlog10Q2fE);

     //-- decide whether to accept the current kinematics
     this->AssertXSecLimits(interaction, xsec, xsec_max);

     double t = xsec_max * rnd->RndKine().Rndm();

     LOG("GravityKinematics", pDEBUG) << "xsec= " << xsec << ", Rnd= " << t;

     accept = (t < xsec);

     //-- If the generated kinematics are accepted, finish-up module's job
     if(accept) {
         LOG("GravityKinematics", pNOTICE) 
            << "Selected:  x = " << gx << ", Q2 = " << gQ2
            << " (W  = " << interaction->KinePtr()->W()  << ","
            << " (Y = " << interaction->KinePtr()->y() << ")";

         // reset trust bits
         interaction->ResetBit(kISkipProcessChk);
         interaction->ResetBit(kISkipKinematicChk);

         // set the cross section for the selected kinematics
         evrec->SetDiffXSec(xsec,kPSxQ2fE);

         // lock selected kinematics & clear running values
         interaction->KinePtr()->SetW (interaction->KinePtr()->W(),  true);
         interaction->KinePtr()->Sety (interaction->KinePtr()->y(),  true);
         interaction->KinePtr()->SetQ2(gQ2, true);
         interaction->KinePtr()->Setx (gx,  true);
         interaction->KinePtr()->ClearRunningValues();
         return;
     }
  } // iterations
}
//___________________________________________________________________________
double GravityKinematicsGenerator::Scan(Interaction * interaction, Range1D_t xrange,Range1D_t Q2range, int NKnotsQ2, int NKnotsX, double ME2, double & x_scan, double & Q2_scan) const
{

  double xsec_scan = 0.;
  Q2_scan   = 0.;
  x_scan    = 0.;

  double stepQ2 = TMath::Power(Q2range.max/Q2range.min,1./(NKnotsQ2-1));

  for ( int iq=0; iq<NKnotsQ2; iq++) {
    double Q2_aux = Q2range.min*TMath::Power(stepQ2,iq);
    xrange.min = TMath::Max(xrange.min,Q2_aux/ME2);
    double stepx = TMath::Power(xrange.max/xrange.min,1./(NKnotsX-1));
    for ( int ix=0; ix<NKnotsX; ix++) {
      double x_aux = xrange.min*TMath::Power(stepx,ix);
      interaction->KinePtr()->Setx(x_aux);
      interaction->KinePtr()->SetQ2(Q2_aux);
      kinematics::UpdateWYFromXQ2(interaction);      
      double xsec_aux = fXSecModel->XSec(interaction, kPSlog10xlog10Q2fE);
      LOG("GravityKinematics", pDEBUG) << "x = " << x_aux << " , Q2 = " << Q2_aux << ", xsec = " << xsec_aux; 
      if (xsec_aux>xsec_scan) {
        xsec_scan = xsec_aux;
        Q2_scan   = Q2_aux;
        x_scan    = x_aux;
      }
    }
  }

  LOG("GravityKinematics", pNOTICE) << "scan -> x = " << x_scan << " , Q2 = " << Q2_scan << ", xsec = " << xsec_scan; 

  return xsec_scan;

}
//__//___________________________________________________________________________
double GravityKinematicsGenerator::ComputeMaxXSec(const Interaction * /* interaction */ ) const
{
  return 0;
}
//___________________________________________________________________________
void GravityKinematicsGenerator::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityKinematicsGenerator::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void GravityKinematicsGenerator::LoadConfig(void)
{

  //-- Safety factor for the maximum differential cross section
  GetParamDef("MaxXSec-SafetyFactor", fSafetyFactor, 2. ) ;
  //-- Maximum allowed fractional cross section deviation from maxim cross
  //   section used in rejection method
  GetParamDef("MaxXSec-DiffTolerance", fMaxXSecDiffTolerance, 999999. ) ;
  assert(fMaxXSecDiffTolerance>=0);

  GetParam("M5",  fM5 ) ;

}
