
#include "TMath.h"
#include "TSystem.h"
#include "TTree.h"
#include "TFile.h"

#include "Framework/EventGen/XSecAlgorithmI.h"
#include "Framework/EventGen/InteractionList.h"
#include "Framework/EventGen/EventGeneratorI.h"
#include "Framework/EventGen/GEVGDriver.h"
#include "Framework/Utils/AppInit.h"
#include "Framework/Utils/RunOpt.h"
#include "Framework/Utils/KineUtils.h"
#include "Framework/Utils/CmdLnArgParser.h"
#include "Framework/Interaction/Interaction.h"
#include "Framework/Interaction/InitialState.h"
#include "Framework/Messenger/Messenger.h"
#include "Framework/ParticleData/PDGLibrary.h"
#include "Framework/Conventions/Units.h"
#include "Framework/Conventions/Constants.h"

#include <fstream>
#include <iostream>

using namespace genie;

void   PrintSyntax          (void);
void   DecodeCommandLine    (int argc, char * argv[]);

vector<double> ReadListFromPath (string path);

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//INPUT ARGUMENTS
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

double fM5;
string fPathElist;
string fOutFileName;

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//MAIN PROGRAM
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

int main(int argc, char ** argv) {

  DecodeCommandLine(argc,argv); 

  RunOpt::Instance()->BuildTune();
  utils::app_init::MesgThresholds(RunOpt::Instance()->MesgThresholdFiles());

  string fChannel = RunOpt::Instance()->EventGeneratorList();

  GEVGDriver evg_driver;
  InitialState init_state(1000010010, 14);
  evg_driver.SetEventGeneratorList(fChannel);
  evg_driver.Configure(init_state);

  vector<double> ve = ReadListFromPath(fPathElist);

  double ei;
  double bx;
  double by;
  double dxsec;
  
  TTree * tree = new TTree("diffxsec","diffxsec");
  tree->Branch( "Ei",       &ei,    "Ei/D"       );
  tree->Branch( "By",       &by,    "By/D"       );
  tree->Branch( "DiffXsec", &dxsec, "DiffXsec/D" );

  const InteractionList * intlst   = evg_driver.Interactions();

  InteractionList::const_iterator intliter;
  for(intliter = intlst->begin(); intliter != intlst->end(); ++intliter) {

    const Interaction * interaction = *intliter;

    const XSecAlgorithmI * xsec_alg = evg_driver.FindGenerator(interaction)->CrossSectionAlg();

    std::cout << interaction->AsString() << std::endl;

    for ( unsigned i=0; i<ve.size(); i++ ) {

      ei = ve[i];

      double s = 2.*constants::kProtonMass*ei;

      double fdlogy = 0.01;
      double fdlogx = 0.001;

      double flogymin = -20;
      double flogxmin = TMath::Log10(TMath::Power(fM5,2)/s);

      vector<double> vy;
      vector<double> vx;
      for ( int iy=0; iy<TMath::Abs(flogymin)/fdlogy; iy++) vy.push_back( TMath::Power( 10., flogymin + (iy+0.5)*fdlogy ) );
      for ( int ix=0; ix<TMath::Abs(flogxmin)/fdlogx; ix++) vx.push_back( TMath::Power( 10., flogxmin + (ix+0.5)*fdlogx ) );

      TLorentzVector p4(0,0,ei,ei);
      interaction->InitStatePtr()->SetProbeP4(p4);

      double xsec = 0;
      for ( unsigned j=0; j<vy.size(); j++ ) {
        by = vy[j];

        if ( by<0 || by>1 ) continue;
        interaction->KinePtr()->Sety(by);
        
        dxsec = 0.;
        for ( unsigned k=0; k<vx.size(); k++ ) {
          bx = vx[k];

          if ( bx<0 || bx>1 ) continue;
          interaction->KinePtr()->Setx(bx);

          utils::kinematics::UpdateWQ2FromXY(interaction);
          
          double dsigmadxdy = xsec_alg->XSec(interaction, kPSxyfE);

          dxsec += dsigmadxdy * bx;
          xsec  += dsigmadxdy * bx * by;

        }
        dxsec *=  fdlogx*TMath::Log(10) * (1e27/units::cm2) ;
        tree->Fill();

      }
  
      xsec *= fdlogx*TMath::Log(10) * fdlogy*TMath::Log(10) * (1e27/units::cm2) ;
      std::cout << ei << " " << xsec << std::endl;

    }

  }


  TFile * outfile = new TFile(fOutFileName.c_str(),"RECREATE");
  tree->Write(tree->GetName());
  outfile->Close();

}

void DecodeCommandLine(int argc, char * argv[]) {

  // Common run options. Set defaults and read.
  RunOpt::Instance()->ReadFromCommandLine(argc,argv);

  // Parse run options for this app
  CmdLnArgParser parser(argc,argv);
  
  if( parser.OptionExists('e') ){ 
    fPathElist = parser.ArgAsString('e');
    LOG("gcalchedisdiffxsec", pINFO) << "PathElist = " << fPathElist;
  }          
  else {
    LOG("gcalchedisdiffxsec", pFATAL) << "Unspecified input path to Elist!";
    PrintSyntax();
    exit(1);
  }

  if( parser.OptionExists('m') ){ 
    fM5 = parser.ArgAsDouble('m');
    LOG("gcalchedisdiffxsec", pINFO) << "M5 = " << fM5;
  }          
  else {
    LOG("gcalchedisdiffxsec", pFATAL) << "Unspecified M5 mass!";
    PrintSyntax();
    exit(1);
  }

  if( parser.OptionExists('o') ){ 
    fOutFileName = parser.ArgAsString('o');
    LOG("gcalchedisdiffxsec", pINFO) << "OutFileName = " << fOutFileName;
  }          
  else {
    LOG("gcalchedisdiffxsec", pFATAL) << "Unspecified output file name!";
    PrintSyntax();
    exit(1);
  }

}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//READ LIST
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
vector<double> ReadListFromPath(string path) {

  vector<double> list;

  std::ifstream infile(path.c_str());

  //check to see that the file was opened correctly:
  if (!infile.is_open()) {
    LOG("gcalchedisdiffxsec", pFATAL) << "There was a problem opening the input file!";
    exit(1);//exit or do additional error checking
  }

  double val = 0.;
  while (infile >> val) list.push_back(val);

  return list;

}

//____________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gcalcgravity", pNOTICE)
      << "\n\n" << "Syntax:" << "\n"
      << "   gcalcgravity -o root_file\n"
      << "            -e pathelist\n"
      << "            -m M5\n"
      << "            --tune genie_tune\n"
      << "            --event-generator-list list_name\n"
      << "            [--message-thresholds xml_file]\n";
}