#include <iostream>
#include <math.h>
#include <vector>
#include <string>

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "meanEtaDependence.h"

#define MAX_VERTICES 1000
#define MAX_HSCPS    10000
#define MAX_GENS     10000
#define MAX_ECALCRYS 10
#define MAX_DEDX_HITS 50

// globals
int numEvents_ = -1;
std::string outputRootName_ = "readHSCPTreeTKplots.root";
std::vector<std::string> listOfFiles_;
float maxTrackEta_ = 1000;
bool enableDebug_ = false;

int numPtBins_ = 5;
int numPBins_ = 5;
int numNomBins_ = 5;
TH1F* histsForPDFNoMPtBins_[5][5]; // number of Pt bins first
TH1F* histsForPDFNoMPtBinsMatched_[5][5]; // number of Pt bins first
TH1F* histsForPDFNoMPBins_[5][5]; // number of P bins first
TH1F* histsForPDFNoMPBinsMatched_[5][5]; // number of P bins first
TH1F* histsForPDFNoMPBinsMuSB_[5][5]; // number of P bins first
TH1F* histsForPDFNoMPBinsMuSBMatched_[5][5]; // number of P bins first

TH1F* histsInNoMFinePBins_[200][5]; // P bins first
TH1F* histsInNoMFinePBinsMatched_[200][5]; // P bins first
TH1F* histsInFineNoMPBins_[10][30];
TH1F* histsInFineNoMPBinsMatched_[10][30];
TH1F* histsInFineNoMPBinsEtaDist_[10][30];
TH1F* histsInFineNoMPBinsMatchedEtaDist_[10][30];
TH1F* histsInFineEtaPBins_[10][25];
TH1F* histsInFineEtaPBinsMatched_[10][25];
// pulls
TH1F* pullsInFineNoMPBins_[10][30];
TH1F* pullsInFineNoMPBinsMatched_[10][30];
TH1F* pullsInFineEtaPBins_[10][25];
TH1F* pullsInFineEtaPBinsMatched_[10][25];

TH1F* dedxClosureInFinePBins_[200];
TH1F* dedxClosureInFinePBinsMatched_[200];
TH1F* dedxClosureInFineEtaBins_[25];
TH1F* dedxClosureInFineEtaBinsMatched_[25];
TH1F* dedxClosureInFineNoMBins_[30];
TH1F* dedxClosureInFineNoMBinsMatched_[30];

// ------------------ Function to parse the command-line arguments------------------------
void parseArguments(int argc, char** argv)
{
  std::string stringGenericOption    = "--";
  std::string stringGenericOption2    = "-";
  std::string stringHelp             = "--help";
  std::string stringInputFileName    = "-i";
  std::string stringOutFileName      = "-o";
  std::string stringNumEvents        = "-n";
  std::string stringMaxEta           = "--maxEta";
  std::string stringDebug           = "--debug";
  //std::string stringTriggers         = "--trig";
  //std::string stringTechTriggers     = "--techTrig";
  //TODO: implement trigger selection later if needed

  // if no arguments are passed, suggest help
  if(argc < 2){
    std::cerr << "\n\tERROR: specify arguments, or use --help\n" << std::endl ;
    exit (1) ;  
  }

  // loop over input options
  for(int v=1; v<argc; v++)
  {
    //debug
    //std::cout << "argv number " << v << " is: " << argv[v] << std::endl;

    if(argv[v] == stringHelp)
    { 
      // help message
      std::cout << " --help : display help" << std::endl ;
      std::cout << " -o : set name of output root file name (e.g. histograms.root)" << std::endl ;
      std::cout << " -n : number of events" << std::endl ;
      std::cout << " -i <list of strings> list of input files" << std::endl ;     
      std::cout << " --maxEta [value] : ignore tracks with | eta | > value" << std::endl ;     
      std::cout << " --debug : turn on debugging output" << std::endl ;     
      //std::cout << " --trig: L1 triggers to include (exclude with x)" << std::endl;
      //std::cout << " --techTrig: L1 technical triggers to include (exclude with x)" << std::endl;
      exit(1);
    }
    else if (argv[v] == stringNumEvents) { // set number of events
      std::cout << "events number" << std::endl;
      numEvents_ = atoi(argv[v+1]);
      v++;
    }
    else if (argv[v] == stringOutFileName) { // set output file
      outputRootName_ = argv[v+1];
      v++;
    }
    else if (argv[v] == stringMaxEta) { // set max track eta
      maxTrackEta_ = atof(argv[v+1]);
      v++;
    }
    else if (argv[v] == stringDebug) { // set debug
      enableDebug_ = true;
    }
    //else if (argv[v] == stringTriggers) { // set L1 triggers to include/exclude
    //  genIncludeExcludeVectors(std::string(argv[v+1]),trigIncludeVector,trigExcludeVector);
    //  v++;
    //}
    //else if (argv[v] == stringTechTriggers) { // set L1 technical triggers to include/exclude
    //  genIncludeExcludeVectors(std::string(argv[v+1]),ttrigIncludeVector,ttrigExcludeVector);
    //  v++;
    //}
    // handle here the case of multiple arguments for input files
    else if (argv[v] == stringInputFileName){// && v<(argc-1) ) 
      //debug
      //std::cout << "Looking at input files!" << std::endl;

      for (int u=v+1; u<argc; u++) {

        if ( 0==std::string(argv[u]).find( stringGenericOption ) ){
          if ( 0==listOfFiles_.size())  {std::cout << "no input files listed" << std::cout;}
          //else  {std::cout << "no more files listed, found: " << argv[u] << std::cout;}
          break;
        }

        else {  listOfFiles_.push_back(argv[u]);
          //debug
          //std::cout << "Pushing back " << argv[u] << " onto list of files" << std::endl;
          v++;
        }

      }// loop on arguments following --i

      continue;

    }//end 'if input files'
    else
    {
      std::cout << "input format unrecognized" << std::endl; exit(1);}
    }// loop over arguments input to the program
}


// Helper function with PDG IDs of the HSCP Gen particles
bool isGluino500(int pdgId)
{
  //XXX
  // ignoring doubly-charged HSCP
  if(pdgId==1000021 || 
      pdgId==1000993 || 
      pdgId==1009213 || 
      pdgId==1009313 || 
      pdgId==1009323 || 
      pdgId==1009113 || 
      pdgId==1009223 || 
      pdgId==1009333 || 
      pdgId==1091114 || 
      pdgId==1092114 || 
      pdgId==1092214 || 
      //pdgId==1092224 || 
      pdgId==1093114 || 
      pdgId==1093214 || 
      pdgId==1093224 || 
      pdgId==1093314 || 
      pdgId==1093324 || 
      pdgId==1093334 || 
      pdgId==-1009213 ||
      pdgId==-1009313 ||
      pdgId==-1009323 ||
      pdgId==-1091114 ||
      pdgId==-1092114 ||
      pdgId==-1092214 ||
      //pdgId==-1092224 ||
      pdgId==-1093114 ||
      pdgId==-1093214 ||
      pdgId==-1093224 ||
      pdgId==-1093314 ||
      pdgId==-1093324 ||
      pdgId==-1093334)  
    return true;
  else
    return false;
}

// Helper function with PDG IDs of the HSCP Gen particles
bool isGluino500Baryon(int pdgId)
{
  if(
      pdgId==1091114 || 
      pdgId==1092114 || 
      pdgId==1092214 || 
      pdgId==1092224 || 
      pdgId==1093114 || 
      pdgId==1093214 || 
      pdgId==1093224 || 
      pdgId==1093314 || 
      pdgId==1093324 || 
      pdgId==1093334 || 
      pdgId==-1091114 ||
      pdgId==-1092114 ||
      pdgId==-1092214 ||
      pdgId==-1092224 ||
      pdgId==-1093114 ||
      pdgId==-1093214 ||
      pdgId==-1093224 ||
      pdgId==-1093314 ||
      pdgId==-1093324 ||
      pdgId==-1093334)  
    return true;
  else
    return false;
}

std::string intToString(int num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}

std::string floatToString(float num)
{
    using namespace std;
    ostringstream myStream;
    myStream << num << flush;
    return (myStream.str ());
}

int findFinePBin(float p)
{
  if(p >= 1000) return 199;

  //int pRound = (int) round(p);
  int bin = p/5;
  return bin;
}

int findLessFinePBin(float p)
{
  if(p >= 1000) return 9;

  //int pRound = (int) round(p);
  int bin = p/100;
  return bin;
}

int findFineNoMBin(int nom)
{
  if(nom >= 30) return 29;

  return nom-1;
}

int findFineEtaBin(float eta)
{
  if(fabs(eta) >= 2.5)
    return 24;

  return (fabs(eta)/0.1);
}

std::string generateTitleEnd(int lpbound, int rpbound, int lnombound, int rnombound)
{
  std::string title = "";
  title+=" for ";
  title+=intToString(lpbound);
  title+=" < P < ";
  title+=intToString(rpbound);
  title+=" and ";
  title+=intToString(lnombound);
  title+=" < NoM <= ";
  title+=intToString(rnombound);
  return title;
}

std::string generateTitleEnd(int lpbound, int rpbound)
{
  std::string title = "";
  title+=" for ";
  title+=intToString(lpbound);
  title+=" < P < ";
  title+=intToString(rpbound);
  return title;
}

std::string generateEtaTitleEnd(float lpbound, float rpbound)
{
  std::string title = "";
  title+=" for ";
  title+=floatToString(lpbound);
  title+=" < |#eta| < ";
  title+=floatToString(rpbound);
  return title;
}

//XXX
int main(int argc, char** argv)
{
  //sic debug
  std::cout << "[INFO] parsing arguments" << std::endl;
  // First parse arguments
  parseArguments(argc, argv);

  // HSCPTree variables
  bool           Event_triggerL1Bits[192];
  bool           Event_triggerHLTBits[128];
  bool           Event_technicalBits[64];
  unsigned int   Event_EventNumber;
  unsigned int   Event_RunNumber;
  unsigned int   Event_LumiSection;
  unsigned int   Event_BXCrossing;
  unsigned int   Event_Orbit;
  unsigned int   Event_Store;
  unsigned int   Event_Time;
  bool           Event_PhysicsDeclared;
  float          Event_BField;

  int   NVertices;
  float          Vertex_x           [MAX_VERTICES];
  float          Vertex_y           [MAX_VERTICES];
  float          Vertex_z           [MAX_VERTICES];
  float          Vertex_x_err       [MAX_VERTICES];
  float          Vertex_y_err       [MAX_VERTICES];
  float          Vertex_z_err       [MAX_VERTICES];
  int            Vertex_TrackSize   [MAX_VERTICES];
  float          Vertex_chi2        [MAX_VERTICES];
  float          Vertex_ndof        [MAX_VERTICES];
  bool           Vertex_isFake      [MAX_VERTICES];

  int   NHSCPs;
  bool           Hscp_hasTrack      [MAX_HSCPS];
  bool           Hscp_hasMuon       [MAX_HSCPS];
  bool           Hscp_hasRpc        [MAX_HSCPS];
  bool           Hscp_hasCalo       [MAX_HSCPS];
  int            Hscp_type          [MAX_HSCPS];
  int            Track_NOH          [MAX_HSCPS];
  float          Track_p            [MAX_HSCPS];
  float          Track_pt           [MAX_HSCPS];
  float          Track_pt_err       [MAX_HSCPS];
  float          Track_chi2         [MAX_HSCPS];
  float          Track_ndof         [MAX_HSCPS];
  float          Track_eta          [MAX_HSCPS];
  float          Track_eta_err      [MAX_HSCPS];
  float          Track_phi          [MAX_HSCPS];
  float          Track_phi_err      [MAX_HSCPS];
  float          Track_dz           [MAX_HSCPS];
  float          Track_d0           [MAX_HSCPS];
  int            Track_quality      [MAX_HSCPS];
  int            Track_charge       [MAX_HSCPS];
  float          Track_dEdxE1       [MAX_HSCPS];
  float          Track_dEdxE1_NOS   [MAX_HSCPS];
  int            Track_dEdxE1_NOM   [MAX_HSCPS];
  float          Track_dEdxE2       [MAX_HSCPS];
  float          Track_dEdxE2_NOS   [MAX_HSCPS];
  int            Track_dEdxE2_NOM   [MAX_HSCPS];
  float          Track_dEdxE3       [MAX_HSCPS];
  float          Track_dEdxE3_NOS   [MAX_HSCPS];
  int            Track_dEdxE3_NOM   [MAX_HSCPS];
  float          Track_dEdxD1       [MAX_HSCPS];
  float          Track_dEdxD1_NOS   [MAX_HSCPS];
  int            Track_dEdxD1_NOM   [MAX_HSCPS];
  float          Track_dEdxD2       [MAX_HSCPS];
  float          Track_dEdxD2_NOS   [MAX_HSCPS];
  int            Track_dEdxD2_NOM   [MAX_HSCPS];
  float          Track_dEdxD3       [MAX_HSCPS];
  float          Track_dEdxD3_NOS   [MAX_HSCPS];
  int            Track_dEdxD3_NOM   [MAX_HSCPS];

  uint32_t       Track_dEdxE1_numHits[MAX_HSCPS];
  float          Track_dEdxE1_hitCharges[MAX_HSCPS][MAX_DEDX_HITS];
  float          Track_dEdxE1_hitDistances[MAX_HSCPS][MAX_DEDX_HITS];
  float          Track_dEdxE1_hitPathLengths[MAX_HSCPS][MAX_DEDX_HITS];

  float          Muon_p             [MAX_HSCPS];
  float          Muon_pt            [MAX_HSCPS];
  float          Muon_eta           [MAX_HSCPS];
  float          Muon_phi           [MAX_HSCPS];
  unsigned int   Muon_type          [MAX_HSCPS];
  bool           Muon_qualityValid  [MAX_HSCPS];
  unsigned int   Muon_charge        [MAX_HSCPS];
  float          Muon_dt_IBeta      [MAX_HSCPS];
  float          Muon_dt_IBeta_err  [MAX_HSCPS];
  float          Muon_dt_fIBeta     [MAX_HSCPS];
  float          Muon_dt_fIBeta_err [MAX_HSCPS];
  int            Muon_dt_ndof       [MAX_HSCPS];
  float          Muon_csc_IBeta     [MAX_HSCPS];
  float          Muon_csc_IBeta_err [MAX_HSCPS];
  float          Muon_csc_fIBeta    [MAX_HSCPS];
  float          Muon_csc_fIBeta_err[MAX_HSCPS];
  int            Muon_csc_ndof      [MAX_HSCPS];
  float          Muon_cb_IBeta      [MAX_HSCPS];
  float          Muon_cb_IBeta_err  [MAX_HSCPS];
  float          Muon_cb_fIBeta     [MAX_HSCPS];
  float          Muon_cb_fIBeta_err [MAX_HSCPS];
  int            Muon_cb_ndof       [MAX_HSCPS];          
  float          Rpc_beta           [MAX_HSCPS];


  float          Calo_ecal_crossedE         [MAX_HSCPS];
  float          Calo_ecal_beta             [MAX_HSCPS];
  float          Calo_ecal_beta_err         [MAX_HSCPS];
  float          Calo_ecal_invBeta_err      [MAX_HSCPS];
  float          Calo_ecal_dEdx             [MAX_HSCPS];
  float          Calo_ecal_time             [MAX_HSCPS];
  float          Calo_ecal_time_err         [MAX_HSCPS];
  int            Calo_ecal_numCrysCrossed   [MAX_HSCPS];
  float          Calo_ecal_swissCrossKs     [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_e1OverE9s        [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_trackLengths     [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_energies         [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_outOfTimeEnergies[MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_chi2s            [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_outOfTimeChi2s   [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_times            [MAX_HSCPS][MAX_ECALCRYS];
  float          Calo_ecal_timeErrors       [MAX_HSCPS][MAX_ECALCRYS];
  unsigned int   Calo_ecal_detIds           [MAX_HSCPS][MAX_ECALCRYS];

  int   NGens = 0;
  unsigned int   Gen_pdgId          [MAX_GENS];
  float          Gen_charge         [MAX_GENS];
  float          Gen_p              [MAX_GENS];
  float          Gen_px             [MAX_GENS];
  float          Gen_py             [MAX_GENS];
  float          Gen_pz             [MAX_GENS];
  float          Gen_pt             [MAX_GENS];
  float          Gen_eta            [MAX_GENS];
  float          Gen_phi            [MAX_GENS];
  float          Gen_beta           [MAX_GENS];
  float          Gen_mass           [MAX_GENS];
  // End HSCPTree variables

  if(listOfFiles_.size()==0)
  {
    std::cout << "\tno input file found" << std::endl;
    return(-1);
  }
  else
  {
    std::cout << "\tfound " << listOfFiles_.size() << " input files: " << std::endl;
    for(std::vector<std::string>::const_iterator  file_itr=listOfFiles_.begin(); file_itr!=listOfFiles_.end(); file_itr++){
      std::cout << "\t" << (*file_itr) << std::endl;
    }
    //if(listOfFiles_.size() > 1)
    //{
    //  std::cerr << "!!!!!Support for multiple input files not implemented!" << std::endl;
    //  return -2;
    //}
  }

  //TFile* tFile = new TFile(listOfFiles_[0].c_str());
  TTree::SetMaxTreeSize(1000*Long64_t(2000000000)); // authorize Trees up to 2 Terabytes
  //TTree* MyTree = (TTree*) tFile->Get("HscpTree");
  //if(!MyTree)
  //{
  //  // See if it was produced with the HSCPTreeBuilder
  //  MyTree = (TTree*) tFile->Get("HSCPTreeBuilder/HscpTree");
  //  if(!MyTree)
  //  {
  //    std::cout << "Couldn't get the TTree out of the file!" << std::endl;
  //    return -3;
  //  }
  //}
  //TODO: implement later if needed (support for multiple input files)
  // Tree construction
  TChain* MyTree = new TChain ("HSCPTreeBuilder/HscpTree") ;  // ntuple producer
  std::vector<std::string>::const_iterator file_itr;
  for(file_itr=listOfFiles_.begin(); file_itr!=listOfFiles_.end(); file_itr++){
    MyTree->Add( (*file_itr).c_str() );
  }
  int nEntries = MyTree->GetEntries();
  //int nEntries = MyTree->GetEntries();
  if (numEvents_==-1) numEvents_ = nEntries;

  std::cout << "\n\tFOUND "         <<  listOfFiles_.size() << " input files" << std::endl ;    
  std::cout << "\n\tFOUND "         <<  nEntries << " events" << std::endl ;    
  std::cout << "\tWILL run on: "    <<  numEvents_ << " events" << std::endl;
  std::cout << "\tOutput file: "    <<  outputRootName_ << std::endl;
  if(maxTrackEta_ < 1000)
    std::cout << "\tMax track eta set to: "  <<  maxTrackEta_ << std::endl;
  if(enableDebug_)
    std::cout << "\tDebugging output enabled." << std::endl;


  MyTree->SetBranchAddress("Event_EventNumber"       ,&Event_EventNumber     );
  MyTree->SetBranchAddress("Event_RunNumber"         ,&Event_RunNumber       );
  MyTree->SetBranchAddress("Event_LumiSection"       ,&Event_LumiSection     );
  MyTree->SetBranchAddress("Event_BXCrossing"        ,&Event_BXCrossing      );
  MyTree->SetBranchAddress("Event_Orbit"             ,&Event_Orbit           );
  MyTree->SetBranchAddress("Event_Store"             ,&Event_Store           );
  MyTree->SetBranchAddress("Event_Time"              ,&Event_Time            );
  MyTree->SetBranchAddress("Event_PhysicsDeclared"   ,&Event_PhysicsDeclared );
  MyTree->SetBranchAddress("Event_technicalBits"     ,Event_technicalBits    );
  MyTree->SetBranchAddress("Event_triggerL1Bits"     ,Event_triggerL1Bits    );
  MyTree->SetBranchAddress("Event_triggerHLTBits"    ,Event_triggerHLTBits   );
  MyTree->SetBranchAddress("Event_BField"            ,&Event_BField          );

  MyTree->SetBranchAddress("NVertices"       ,&NVertices      );
  MyTree->SetBranchAddress("Vertex_x"        ,Vertex_x        );
  MyTree->SetBranchAddress("Vertex_y"        ,Vertex_y        );
  MyTree->SetBranchAddress("Vertex_z"        ,Vertex_z        );
  MyTree->SetBranchAddress("Vertex_x_err"    ,Vertex_x_err    );
  MyTree->SetBranchAddress("Vertex_y_err"    ,Vertex_y_err    );
  MyTree->SetBranchAddress("Vertex_z_err"    ,Vertex_z_err    );
  MyTree->SetBranchAddress("Vertex_TrackSize",Vertex_TrackSize);
  MyTree->SetBranchAddress("Vertex_chi2"     ,Vertex_chi2     );
  MyTree->SetBranchAddress("Vertex_ndof"     ,Vertex_ndof     );
  MyTree->SetBranchAddress("Vertex_isFake"   ,Vertex_isFake   );

  MyTree->SetBranchAddress("NHSCPs"             ,&NHSCPs            );
  MyTree->SetBranchAddress("Hscp_hasTrack"      ,Hscp_hasTrack      );
  MyTree->SetBranchAddress("Hscp_hasMuon"       ,Hscp_hasMuon       );
  MyTree->SetBranchAddress("Hscp_hasRpc"        ,Hscp_hasRpc        );
  MyTree->SetBranchAddress("Hscp_hasCalo"       ,Hscp_hasCalo       );
  MyTree->SetBranchAddress("Hscp_type"          ,Hscp_type          );
  MyTree->SetBranchAddress("Track_NOH"          ,Track_NOH          );
  MyTree->SetBranchAddress("Track_p"            ,Track_p            );
  MyTree->SetBranchAddress("Track_pt"           ,Track_pt           );
  MyTree->SetBranchAddress("Track_pt_err"       ,Track_pt_err       );
  MyTree->SetBranchAddress("Track_chi2"         ,Track_chi2         );
  MyTree->SetBranchAddress("Track_ndof"         ,Track_ndof         );
  MyTree->SetBranchAddress("Track_eta"          ,Track_eta          );
  MyTree->SetBranchAddress("Track_eta_err"      ,Track_eta_err      );
  MyTree->SetBranchAddress("Track_phi"          ,Track_phi          );
  MyTree->SetBranchAddress("Track_phi_err"      ,Track_phi_err      );
  MyTree->SetBranchAddress("Track_d0"           ,Track_d0           );
  MyTree->SetBranchAddress("Track_dz"           ,Track_dz           );
  MyTree->SetBranchAddress("Track_quality"      ,Track_quality      );
  MyTree->SetBranchAddress("Track_charge"       ,Track_charge       );
  MyTree->SetBranchAddress("Track_dEdxE1"       ,Track_dEdxE1       );
  MyTree->SetBranchAddress("Track_dEdxE1_NOS"   ,Track_dEdxE1_NOS   );
  MyTree->SetBranchAddress("Track_dEdxE1_NOM"   ,Track_dEdxE1_NOM   );
  MyTree->SetBranchAddress("Track_dEdxE2"       ,Track_dEdxE2       );
  MyTree->SetBranchAddress("Track_dEdxE2_NOS"   ,Track_dEdxE2_NOS   );
  MyTree->SetBranchAddress("Track_dEdxE2_NOM"   ,Track_dEdxE2_NOM   );
  MyTree->SetBranchAddress("Track_dEdxE3"       ,Track_dEdxE3       );
  MyTree->SetBranchAddress("Track_dEdxE3_NOS"   ,Track_dEdxE3_NOS   );
  MyTree->SetBranchAddress("Track_dEdxE3_NOM"   ,Track_dEdxE3_NOM   );
  MyTree->SetBranchAddress("Track_dEdxD1"       ,Track_dEdxD1       );
  MyTree->SetBranchAddress("Track_dEdxD1_NOS"   ,Track_dEdxD1_NOS   );
  MyTree->SetBranchAddress("Track_dEdxD1_NOM"   ,Track_dEdxD1_NOM   );
  MyTree->SetBranchAddress("Track_dEdxD2"       ,Track_dEdxD2       );
  MyTree->SetBranchAddress("Track_dEdxD2_NOS"   ,Track_dEdxD2_NOS   );
  MyTree->SetBranchAddress("Track_dEdxD2_NOM"   ,Track_dEdxD2_NOM   );
  MyTree->SetBranchAddress("Track_dEdxD3"       ,Track_dEdxD3       );
  MyTree->SetBranchAddress("Track_dEdxD3_NOS"   ,Track_dEdxD3_NOS   );
  MyTree->SetBranchAddress("Track_dEdxD3_NOM"   ,Track_dEdxD3_NOM   );

  MyTree->SetBranchAddress("Track_dEdxE1_numHits"   ,Track_dEdxE1_numHits   );
  //MyTree->SetBranchAddress("Track_dEdxE1_hitPathLengths",Track_dEdxE1_hitPathLengths);
  MyTree->SetBranchAddress("Track_dEdxE1_hitCharges",Track_dEdxE1_hitCharges);
  MyTree->SetBranchAddress("Track_dEdxE1_hitDistances",Track_dEdxE1_hitDistances);

  MyTree->SetBranchAddress("Muon_p"             ,Muon_p             );
  MyTree->SetBranchAddress("Muon_pt"            ,Muon_pt            );
  MyTree->SetBranchAddress("Muon_eta"           ,Muon_eta           );
  MyTree->SetBranchAddress("Muon_phi"           ,Muon_phi           );
  MyTree->SetBranchAddress("Muon_type"          ,Muon_type          );
  MyTree->SetBranchAddress("Muon_qualityValid"  ,Muon_qualityValid  );
  MyTree->SetBranchAddress("Muon_charge"        ,Muon_charge        );
  MyTree->SetBranchAddress("Muon_dt_IBeta"      ,Muon_dt_IBeta      );
  MyTree->SetBranchAddress("Muon_dt_IBeta_err"  ,Muon_dt_IBeta_err  );
  MyTree->SetBranchAddress("Muon_dt_fIBeta"     ,Muon_dt_fIBeta     );
  MyTree->SetBranchAddress("Muon_dt_fIBeta_err" ,Muon_dt_fIBeta_err );
  MyTree->SetBranchAddress("Muon_dt_ndof"       ,Muon_dt_ndof       );
  MyTree->SetBranchAddress("Muon_csc_IBeta"     ,Muon_csc_IBeta     );
  MyTree->SetBranchAddress("Muon_csc_IBeta_err" ,Muon_csc_IBeta_err );
  MyTree->SetBranchAddress("Muon_csc_fIBeta"    ,Muon_csc_fIBeta    );
  MyTree->SetBranchAddress("Muon_csc_fIBeta_err",Muon_csc_fIBeta_err);
  MyTree->SetBranchAddress("Muon_csc_ndof"      ,Muon_csc_ndof      );
  MyTree->SetBranchAddress("Muon_cb_IBeta"      ,Muon_cb_IBeta      );
  MyTree->SetBranchAddress("Muon_cb_IBeta_err"  ,Muon_cb_IBeta_err  );
  MyTree->SetBranchAddress("Muon_cb_fIBeta"     ,Muon_cb_fIBeta     );
  MyTree->SetBranchAddress("Muon_cb_fIBeta_err" ,Muon_cb_fIBeta_err );
  MyTree->SetBranchAddress("Muon_cb_ndof"       ,Muon_cb_ndof       );
  MyTree->SetBranchAddress("Rpc_beta"           ,Rpc_beta           );

  MyTree->SetBranchAddress("Calo_ecal_crossedE"      ,Calo_ecal_crossedE);
  MyTree->SetBranchAddress("Calo_ecal_beta"          ,Calo_ecal_beta      );    
  MyTree->SetBranchAddress("Calo_ecal_beta_err"      ,Calo_ecal_beta_err      );
  MyTree->SetBranchAddress("Calo_ecal_invBeta_err"   ,Calo_ecal_invBeta_err   );
  MyTree->SetBranchAddress("Calo_ecal_dEdx"          ,Calo_ecal_dEdx          );
  MyTree->SetBranchAddress("Calo_ecal_time"          ,Calo_ecal_time          );
  MyTree->SetBranchAddress("Calo_ecal_time_err"      ,Calo_ecal_time_err      );
  MyTree->SetBranchAddress("Calo_ecal_numCrysCrossed",Calo_ecal_numCrysCrossed);
  MyTree->SetBranchAddress("Calo_ecal_swissCrossKs"  ,Calo_ecal_swissCrossKs  );
  MyTree->SetBranchAddress("Calo_ecal_e1OverE9s"     ,Calo_ecal_e1OverE9s     );
  MyTree->SetBranchAddress("Calo_ecal_trackLengths"  ,Calo_ecal_trackLengths  );
  MyTree->SetBranchAddress("Calo_ecal_energies"      ,Calo_ecal_energies      );
  MyTree->SetBranchAddress("Calo_ecal_outOfTimeEnergies",Calo_ecal_outOfTimeEnergies);
  MyTree->SetBranchAddress("Calo_ecal_chi2s"      ,Calo_ecal_chi2s      );
  MyTree->SetBranchAddress("Calo_ecal_outOfTimeChi2s"      ,Calo_ecal_outOfTimeChi2s      );
  MyTree->SetBranchAddress("Calo_ecal_times"         ,Calo_ecal_times         );
  MyTree->SetBranchAddress("Calo_ecal_timeErrors"    ,Calo_ecal_timeErrors    );
  MyTree->SetBranchAddress("Calo_ecal_detIds"        ,Calo_ecal_detIds        );

  MyTree->SetBranchAddress("NGens"              ,&NGens    );
  MyTree->SetBranchAddress("Gen_pdgId"          ,Gen_pdgId );
  MyTree->SetBranchAddress("Gen_charge"         ,Gen_charge);
  MyTree->SetBranchAddress("Gen_p"              ,Gen_p     );
  MyTree->SetBranchAddress("Gen_px"             ,Gen_px    );
  MyTree->SetBranchAddress("Gen_py"             ,Gen_py    );
  MyTree->SetBranchAddress("Gen_pz"             ,Gen_pz    );
  MyTree->SetBranchAddress("Gen_pt"             ,Gen_pt    );
  MyTree->SetBranchAddress("Gen_eta"            ,Gen_eta   );
  MyTree->SetBranchAddress("Gen_phi"            ,Gen_phi   );
  MyTree->SetBranchAddress("Gen_beta"           ,Gen_beta  );
  MyTree->SetBranchAddress("Gen_mass"           ,Gen_mass  );

  TFile* outFile = new TFile(outputRootName_.c_str(),"recreate");
  outFile->cd();
  // Book histos
  // TK general
  TH1F* trackChi2Hist = new TH1F("trackChi2","Track chi2",200,0,100);
  TH1F* trackChi2NdfHist = new TH1F("trackChi2Ndf","Track chi2/Ndf",100,0,50);
  TH1F* trackPtHist = new TH1F("trackPt","Track Pt;GeV",1000,0,1000);
  TH1F* trackPtMatchedHist = new TH1F("trackPtMatched","Track Pt (matched to HSCP);GeV",1000,0,1000);
  TH1F* trackEtaHist = new TH1F("trackEta","Track Eta",600,-3,3);
  TH1F* trackEtaMatchedHist = new TH1F("trackEtaMatched","Track eta (matched to HSCP);#eta",600,-3,3);
  TH1F* trackNohHist = new TH1F("trackNoH","Track number of hits",50,0,50);
  TH2F* trackEtaVsPtHist = new TH2F("trackEtaVsPt","Track eta vs. pt;Pt [GeV];#eta",2000,0,2000,600,-3,3);
  TH2F* trackEtaVsPtMatchedHist = new TH2F("trackEtaVsPtMatched","Track eta vs. pt (matched to HSCP);Pt [GeV];#eta",2000,0,2000,600,-3,3);
  TH2F* trackEtaVsPHist = new TH2F("trackEtaVsP","Track eta vs. p;P [GeV];#eta",2000,0,2000,600,-3,3);
  TH2F* trackEtaVsPMatchedHist = new TH2F("trackEtaVsPMatched","Track eta vs. p (matched to HSCP);P [GeV];#eta",2000,0,2000,600,-3,3);
  // TK with Mu timing requirement
  TH2F* trackEtaVsPtMuTimingSBHist = new TH2F("trackEtaVsPtMuTimingSB","Track eta vs. Pt (mu #beta > 1.25, err 1/#beta < 0.2);Pt [GeV];#eta",2000,0,2000,600,-3,3);
  TH2F* trackEtaVsPtMuTimingSBMatchedHist = new TH2F("trackEtaVsPtMuTimingSBMatched","Track eta vs. Pt (mu #beta > 1.25, err 1/#beta < 0.2, matched to HSCP);Pt [GeV];#eta",2000,0,2000,600,-3,3);
  TH2F* trackEtaVsPMuTimingSBHist = new TH2F("trackEtaVsPMuTimingSB","Track eta vs. P (mu #beta > 1.25, err 1/#beta < 0.2);P [GeV];#eta",2000,0,2000,600,-3,3);
  TH2F* trackEtaVsPMuTimingSBMatchedHist = new TH2F("trackEtaVsPMuTimingSBMatched","Track eta vs. P (mu #beta > 1.25, err 1/#beta < 0.2, matched to HSCP);P [GeV];#eta",2000,0,2000,600,-3,3);
  // TK dE/dx
  TH1F* trackDeDxE1Hist = new TH1F("trackDeDxE1Hist","Tracker dE/dx E1;MeV/cm",125,0,25);
  TH1F* trackDeDxE1ClosureHist = new TH1F("trackDeDxE1Closure","Tracker dE/dx E1 (with p, #eta, NoM dep. removed);MeV/cm",100,-10,10);
  TH1F* trackDeDxE1ClosureMatchedHist = new TH1F("trackDeDxE1ClosureMatched","Tracker dE/dx E1 (with p, #eta, NoM dep. removed, matched to HSCP);MeV/cm",100,-10,10);
  TH1F* trackDeDxE2Hist = new TH1F("trackDeDxE2Hist","Tracker dE/dx E2;MeV/cm",125,0,25);
  TH1F* trackDeDxE3Hist = new TH1F("trackDeDxE3Hist","Tracker dE/dx E3;MeV/cm",125,0,25);
  TH1F* trackDeDxD1Hist = new TH1F("trackDeDxD1Hist","Tracker dE/dx D1",100,0,1);
  TH1F* trackDeDxD2Hist = new TH1F("trackDeDxD2Hist","Tracker dE/dx D2",100,0,1);
  TH1F* trackDeDxD3Hist = new TH1F("trackDeDxD3Hist","Tracker dE/dx D3",100,0,1);
  TH2F* trackEtaVsDeDxE1Hist = new TH2F("trackEtaVsDeDxE1","Eta vs. dE/dx E1;MeV/cm;",125,0,25,300,-3,3);
  TH2F* trackEtaVsDeDxE1MatchedHist = new TH2F("trackEtaVsDeDxE1Matched","Eta vs. dE/dx E1 (matched to HSCP);MeV/cm;",125,0,25,300,-3,3);
  TH2F* trackEtaVsDeDxE1NoMRestrHist = new TH2F("trackEtaVsDeDxE1NoMRestr","Eta vs. dE/dx E1, NoM=10-12;MeV/cm;",125,0,25,300,-3,3);
  TH2F* trackEtaVsDeDxE1NoMRestrMatchedHist = new TH2F("trackEtaVsDeDxE1NoMRestrMatched","Eta vs. dE/dx E1 NoM=10-12 (matched to HSCP);MeV/cm;",125,0,25,300,-3,3);
  // TK est vs. disc
  TH2F* trackDeDxE1VsDeDxD3Hist = new TH2F("trackDeDxE1VsDeDxD3","Tracker dE/dx E1 vs. dE/dx D3",100,0,1,250,0,25);
  TH2F* trackDeDxE1VsDeDxD3MatchedHist = new TH2F("trackDeDxE1VsDeDxD3Matched","Tracker dE/dx E1 vs. dE/dx D3 (matched to HSCP)",100,0,1,250,0,25);
  // TK P/Pt vs. dE/dx
  TH2F* trackPvsDeDxE1Hist = new TH2F("trackPvsDeDxE1","Tracker P vs. dE/dx E1;MeV/cm;GeV",50,0,50,250,0,1000);
  TH2F* trackPvsDeDxE1MatchedHist = new TH2F("trackPvsDeDxE1Matched","Tracker P vs. dE/dx E1 (matched to Gen HSCP);MeV/cm;GeV",50,0,50,250,0,1000);
  TH2F* trackPvsDeDxD3Hist = new TH2F("trackPvsDeDxD3","Tracker P vs. dE/dx D3;;GeV",100,0,1,250,0,1000);
  TH2F* trackPvsDeDxD3MatchedHist = new TH2F("trackPvsDeDxD3Matched","Tracker P vs. dE/dx D3 (matched to Gen HSCP);;GeV",100,0,1,250,0,1000);
  TH2F* trackPtvsDeDxE1Hist = new TH2F("trackPtVsDeDxE1","Tracker Pt vs. dE/dx E1;MeV/cm;GeV",50,0,50,250,0,1000);
  TH2F* trackPtvsDeDxE1MatchedHist = new TH2F("trackPtVsDeDxE1Matched","Tracker P vs. dE/dx E1 (matched to Gen HSCP);MeV/cm;GeV",50,0,50,250,0,1000);
  TH2F* trackPtvsDeDxD3Hist = new TH2F("trackPtVsDeDxD3","Tracker Pt vs. dE/dx D3;;GeV",100,0,1,250,0,1000);
  TH2F* trackPtvsDeDxD3MatchedHist = new TH2F("trackPtVsDeDxD3Matched","Tracker Pt vs. dE/dx D3 (matched to Gen HSCP);;GeV",100,0,1,250,0,1000);
  // TK dE/dx -- Pt slices
  TH1F* trackDeDxE1PtSlice1Hist = new TH1F("trackDeDxE1PtSlice1","Tracker dE/dx E1 for track 15 < Pt < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice2Hist = new TH1F("trackDeDxE1PtSlice2","Tracker dE/dx E1 for track 25 < Pt < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice3Hist = new TH1F("trackDeDxE1PtSlice3","Tracker dE/dx E1 for track 50 < Pt < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice4Hist = new TH1F("trackDeDxE1PtSlice4","Tracker dE/dx E1 for track 75 < Pt < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice5Hist = new TH1F("trackDeDxE1PtSlice5","Tracker dE/dx E1 for track 100 < Pt < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice6Hist = new TH1F("trackDeDxE1PtSlice6","Tracker dE/dx E1 for track 125 < Pt < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice7Hist = new TH1F("trackDeDxE1PtSlice7","Tracker dE/dx E1 for track 150 < Pt < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice8Hist = new TH1F("trackDeDxE1PtSlice8","Tracker dE/dx E1 for track 175 < Pt < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice9Hist = new TH1F("trackDeDxE1PtSlice9","Tracker dE/dx E1 for track 200 < Pt < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice10Hist = new TH1F("trackDeDxE1PtSlice10","Tracker dE/dx E1 for track 300 < Pt < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice11Hist = new TH1F("trackDeDxE1PtSlice11","Tracker dE/dx E1 for track 400 < Pt < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PtSlice12Hist = new TH1F("trackDeDxE1PtSlice12","Tracker dE/dx E1 for track Pt > 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice1Hist = new TH1F("trackDeDxE2PtSlice1","Tracker dE/dx E2 for track 15 < Pt < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice2Hist = new TH1F("trackDeDxE2PtSlice2","Tracker dE/dx E2 for track 25 < Pt < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice3Hist = new TH1F("trackDeDxE2PtSlice3","Tracker dE/dx E2 for track 50 < Pt < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice4Hist = new TH1F("trackDeDxE2PtSlice4","Tracker dE/dx E2 for track 75 < Pt < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice5Hist = new TH1F("trackDeDxE2PtSlice5","Tracker dE/dx E2 for track 100 < Pt < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice6Hist = new TH1F("trackDeDxE2PtSlice6","Tracker dE/dx E2 for track 125 < Pt < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice7Hist = new TH1F("trackDeDxE2PtSlice7","Tracker dE/dx E2 for track 150 < Pt < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice8Hist = new TH1F("trackDeDxE2PtSlice8","Tracker dE/dx E2 for track 175 < Pt < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice9Hist = new TH1F("trackDeDxE2PtSlice9","Tracker dE/dx E2 for track 200 < Pt < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice10Hist = new TH1F("trackDeDxE2PtSlice10","Tracker dE/dx E2 for track 300 < Pt < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice11Hist = new TH1F("trackDeDxE2PtSlice11","Tracker dE/dx E2 for track 400 < Pt < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PtSlice12Hist = new TH1F("trackDeDxE2PtSlice12","Tracker dE/dx E2 for track Pt > 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice1Hist = new TH1F("trackDeDxE3PtSlice1","Tracker dE/dx E3 for track 15 < Pt < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice2Hist = new TH1F("trackDeDxE3PtSlice2","Tracker dE/dx E3 for track 25 < Pt < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice3Hist = new TH1F("trackDeDxE3PtSlice3","Tracker dE/dx E3 for track 50 < Pt < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice4Hist = new TH1F("trackDeDxE3PtSlice4","Tracker dE/dx E3 for track 75 < Pt < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice5Hist = new TH1F("trackDeDxE3PtSlice5","Tracker dE/dx E3 for track 100 < Pt < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice6Hist = new TH1F("trackDeDxE3PtSlice6","Tracker dE/dx E3 for track 125 < Pt < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice7Hist = new TH1F("trackDeDxE3PtSlice7","Tracker dE/dx E3 for track 150 < Pt < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice8Hist = new TH1F("trackDeDxE3PtSlice8","Tracker dE/dx E3 for track 175 < Pt < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice9Hist = new TH1F("trackDeDxE3PtSlice9","Tracker dE/dx E3 for track 200 < Pt < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice10Hist = new TH1F("trackDeDxE3PtSlice10","Tracker dE/dx E3 for track 300 < Pt < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice11Hist = new TH1F("trackDeDxE3PtSlice11","Tracker dE/dx E3 for track 400 < Pt < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PtSlice12Hist = new TH1F("trackDeDxE3PtSlice12","Tracker dE/dx E3 for track Pt > 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PtSlice1Hist = new TH1F("trackDeDxD1PtSlice1","Tracker dE/dx D1 for track Pt < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PtSlice2Hist = new TH1F("trackDeDxD1PtSlice2","Tracker dE/dx D1 for track 100 < Pt < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PtSlice3Hist = new TH1F("trackDeDxD1PtSlice3","Tracker dE/dx D1 for track 200 < Pt < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PtSlice4Hist = new TH1F("trackDeDxD1PtSlice4","Tracker dE/dx D1 for track 300 < Pt < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PtSlice5Hist = new TH1F("trackDeDxD1PtSlice5","Tracker dE/dx D1 for track 400 < Pt < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PtSlice6Hist = new TH1F("trackDeDxD1PtSlice6","Tracker dE/dx D1 for track Pt > 500 GeV;MeV/cm",125,0,25);
  // TK dE/dx -- P slices
  TH1F* trackDeDxE1PSlice1Hist = new TH1F("trackDeDxE1PSlice1","Tracker dE/dx E1 for track 15 < P < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice2Hist = new TH1F("trackDeDxE1PSlice2","Tracker dE/dx E1 for track 25 < P < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice3Hist = new TH1F("trackDeDxE1PSlice3","Tracker dE/dx E1 for track 50 < P < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice4Hist = new TH1F("trackDeDxE1PSlice4","Tracker dE/dx E1 for track 75 < P < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice5Hist = new TH1F("trackDeDxE1PSlice5","Tracker dE/dx E1 for track 100 < P < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice6Hist = new TH1F("trackDeDxE1PSlice6","Tracker dE/dx E1 for track 125 < P < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice7Hist = new TH1F("trackDeDxE1PSlice7","Tracker dE/dx E1 for track 150 < P < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice8Hist = new TH1F("trackDeDxE1PSlice8","Tracker dE/dx E1 for track 175 < P < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice9Hist = new TH1F("trackDeDxE1PSlice9","Tracker dE/dx E1 for track 200 < P < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice10Hist = new TH1F("trackDeDxE1PSlice10","Tracker dE/dx E1 for track 300 < P < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice11Hist = new TH1F("trackDeDxE1PSlice11","Tracker dE/dx E1 for track 400 < P < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE1PSlice12Hist = new TH1F("trackDeDxE1PSlice12","Tracker dE/dx E1 for track P > 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice1Hist = new TH1F("trackDeDxE2PSlice1","Tracker dE/dx E2 for track 15 < P < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice2Hist = new TH1F("trackDeDxE2PSlice2","Tracker dE/dx E2 for track 25 < P < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice3Hist = new TH1F("trackDeDxE2PSlice3","Tracker dE/dx E2 for track 50 < P < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice4Hist = new TH1F("trackDeDxE2PSlice4","Tracker dE/dx E2 for track 75 < P < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice5Hist = new TH1F("trackDeDxE2PSlice5","Tracker dE/dx E2 for track 100 < P < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice6Hist = new TH1F("trackDeDxE2PSlice6","Tracker dE/dx E2 for track 125 < P < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice7Hist = new TH1F("trackDeDxE2PSlice7","Tracker dE/dx E2 for track 150 < P < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice8Hist = new TH1F("trackDeDxE2PSlice8","Tracker dE/dx E2 for track 175 < P < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice9Hist = new TH1F("trackDeDxE2PSlice9","Tracker dE/dx E2 for track 200 < P < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice10Hist = new TH1F("trackDeDxE2PSlice10","Tracker dE/dx E2 for track 300 < P < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice11Hist = new TH1F("trackDeDxE2PSlice11","Tracker dE/dx E2 for track 400 < P < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE2PSlice12Hist = new TH1F("trackDeDxE2PSlice12","Tracker dE/dx E2 for track P > 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice1Hist = new TH1F("trackDeDxE3PSlice1","Tracker dE/dx E3 for track 15 < P < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice2Hist = new TH1F("trackDeDxE3PSlice2","Tracker dE/dx E3 for track 25 < P < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice3Hist = new TH1F("trackDeDxE3PSlice3","Tracker dE/dx E3 for track 50 < P < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice4Hist = new TH1F("trackDeDxE3PSlice4","Tracker dE/dx E3 for track 75 < P < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice5Hist = new TH1F("trackDeDxE3PSlice5","Tracker dE/dx E3 for track 100 < P < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice6Hist = new TH1F("trackDeDxE3PSlice6","Tracker dE/dx E3 for track 125 < P < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice7Hist = new TH1F("trackDeDxE3PSlice7","Tracker dE/dx E3 for track 150 < P < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice8Hist = new TH1F("trackDeDxE3PSlice8","Tracker dE/dx E3 for track 175 < P < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice9Hist = new TH1F("trackDeDxE3PSlice9","Tracker dE/dx E3 for track 200 < P < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice10Hist = new TH1F("trackDeDxE3PSlice10","Tracker dE/dx E3 for track 300 < P < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice11Hist = new TH1F("trackDeDxE3PSlice11","Tracker dE/dx E3 for track 400 < P < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxE3PSlice12Hist = new TH1F("trackDeDxE3PSlice12","Tracker dE/dx E3 for track P > 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PSlice1Hist = new TH1F("trackDeDxD1PSlice1","Tracker dE/dx D1 for track P < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PSlice2Hist = new TH1F("trackDeDxD1PSlice2","Tracker dE/dx D1 for track 100 < P < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PSlice3Hist = new TH1F("trackDeDxD1PSlice3","Tracker dE/dx D1 for track 200 < P < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PSlice4Hist = new TH1F("trackDeDxD1PSlice4","Tracker dE/dx D1 for track 300 < P < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PSlice5Hist = new TH1F("trackDeDxD1PSlice5","Tracker dE/dx D1 for track 400 < P < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxD1PSlice6Hist = new TH1F("trackDeDxD1PSlice6","Tracker dE/dx D1 for track P > 500 GeV;MeV/cm",125,0,25);
  // TK dE/dx -- beta slices
  TH1F* trackDeDxE1BetaSliceHists[20];
  trackDeDxE1BetaSliceHists[0] = new TH1F("trackDeDxE1BetaSlice1","Tracker dE/dx E1 for beta < 0.05;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[1] = new TH1F("trackDeDxE1BetaSlice2","Tracker dE/dx E1 for 0.05 < beta < 0.1;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[2] = new TH1F("trackDeDxE1BetaSlice3","Tracker dE/dx E1 for 0.1 < beta < 0.15;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[3] = new TH1F("trackDeDxE1BetaSlice4","Tracker dE/dx E1 for 0.15 < beta < 0.2;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[4] = new TH1F("trackDeDxE1BetaSlice5","Tracker dE/dx E1 for 0.2 < beta < 0.25;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[5] = new TH1F("trackDeDxE1BetaSlice6","Tracker dE/dx E1 for 0.25 < beta < 0.3;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[6] = new TH1F("trackDeDxE1BetaSlice7","Tracker dE/dx E1 for 0.3 < beta < 0.35;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[7] = new TH1F("trackDeDxE1BetaSlice8","Tracker dE/dx E1 for 0.35 < beta < 0.4;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[8] = new TH1F("trackDeDxE1BetaSlice9","Tracker dE/dx E1 for 0.4 < beta < 0.45;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[9] = new TH1F("trackDeDxE1BetaSlice10","Tracker dE/dx E1 for 0.45 < beta < 0.5;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[10] = new TH1F("trackDeDxE1BetaSlice11","Tracker dE/dx E1 for 0.5 < beta < 0.55;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[11] = new TH1F("trackDeDxE1BetaSlice12","Tracker dE/dx E1 for 0.55 < beta < 0.6;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[12] = new TH1F("trackDeDxE1BetaSlice13","Tracker dE/dx E1 for 0.6 < beta < 0.65;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[13] = new TH1F("trackDeDxE1BetaSlice14","Tracker dE/dx E1 for 0.65 < beta < 0.7;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[14] = new TH1F("trackDeDxE1BetaSlice15","Tracker dE/dx E1 for 0.7 < beta < 0.75;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[15] = new TH1F("trackDeDxE1BetaSlice16","Tracker dE/dx E1 for 0.75 < beta < 0.8;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[16] = new TH1F("trackDeDxE1BetaSlice17","Tracker dE/dx E1 for 0.8 < beta < 0.85;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[17] = new TH1F("trackDeDxE1BetaSlice18","Tracker dE/dx E1 for 0.85 < beta < 0.9;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[18] = new TH1F("trackDeDxE1BetaSlice19","Tracker dE/dx E1 for 0.9 < beta < 0.95;MeV/cm",125,0,25);
  trackDeDxE1BetaSliceHists[19] = new TH1F("trackDeDxE1BetaSlice20","Tracker dE/dx E1 for 0.95 < beta < 1;MeV/cm",125,0,25);
  // TK dE/dx matched to HSCP particle -- Pt slices
  TH1F* trackDeDxMatchedE1PtSlice1Hist = new TH1F("trackDeDxMatchedE1PtSlice1","Tracker dE/dx (matched to gen HSCP) E1 for track 15 < Pt < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice2Hist = new TH1F("trackDeDxMatchedE1PtSlice2","Tracker dE/dx (matched to gen HSCP) E1 for track 25 < Pt < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice3Hist = new TH1F("trackDeDxMatchedE1PtSlice3","Tracker dE/dx (matched to gen HSCP) E1 for track 50 < Pt < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice4Hist = new TH1F("trackDeDxMatchedE1PtSlice4","Tracker dE/dx (matched to gen HSCP) E1 for track 75 < Pt < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice5Hist = new TH1F("trackDeDxMatchedE1PtSlice5","Tracker dE/dx (matched to gen HSCP) E1 for track 100 < Pt < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice6Hist = new TH1F("trackDeDxMatchedE1PtSlice6","Tracker dE/dx (matched to gen HSCP) E1 for track 125 < Pt < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice7Hist = new TH1F("trackDeDxMatchedE1PtSlice7","Tracker dE/dx (matched to gen HSCP) E1 for track 150 < Pt < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice8Hist = new TH1F("trackDeDxMatchedE1PtSlice8","Tracker dE/dx (matched to gen HSCP) E1 for track 175 < Pt < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice9Hist = new TH1F("trackDeDxMatchedE1PtSlice9","Tracker dE/dx (matched to gen HSCP) E1 for track 200 < Pt < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice10Hist = new TH1F("trackDeDxMatchedE1PtSlice10","Tracker dE/dx (matched to gen HSCP) E1 for track 300 < Pt < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice11Hist = new TH1F("trackDeDxMatchedE1PtSlice11","Tracker dE/dx (matched to gen HSCP) E1 for track 400 < Pt < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PtSlice12Hist = new TH1F("trackDeDxMatchedE1PtSlice12","Tracker dE/dx (matched to gen HSCP) E1 for track Pt > 500 GeV;MeV/cm",125,0,25);
  // TK dE/dx matched to HSCP particle -- P slices
  TH1F* trackDeDxMatchedE1PSlice1Hist = new TH1F("trackDeDxMatchedE1PSlice1","Tracker dE/dx (matched to gen HSCP) E1 for track 15 < P < 25 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice2Hist = new TH1F("trackDeDxMatchedE1PSlice2","Tracker dE/dx (matched to gen HSCP) E1 for track 25 < P < 50 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice3Hist = new TH1F("trackDeDxMatchedE1PSlice3","Tracker dE/dx (matched to gen HSCP) E1 for track 50 < P < 75 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice4Hist = new TH1F("trackDeDxMatchedE1PSlice4","Tracker dE/dx (matched to gen HSCP) E1 for track 75 < P < 100 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice5Hist = new TH1F("trackDeDxMatchedE1PSlice5","Tracker dE/dx (matched to gen HSCP) E1 for track 100 < P < 125 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice6Hist = new TH1F("trackDeDxMatchedE1PSlice6","Tracker dE/dx (matched to gen HSCP) E1 for track 125 < P < 150 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice7Hist = new TH1F("trackDeDxMatchedE1PSlice7","Tracker dE/dx (matched to gen HSCP) E1 for track 150 < P < 175 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice8Hist = new TH1F("trackDeDxMatchedE1PSlice8","Tracker dE/dx (matched to gen HSCP) E1 for track 175 < P < 200 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice9Hist = new TH1F("trackDeDxMatchedE1PSlice9","Tracker dE/dx (matched to gen HSCP) E1 for track 200 < P < 300 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice10Hist = new TH1F("trackDeDxMatchedE1PSlice10","Tracker dE/dx (matched to gen HSCP) E1 for track 300 < P < 400 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice11Hist = new TH1F("trackDeDxMatchedE1PSlice11","Tracker dE/dx (matched to gen HSCP) E1 for track 400 < P < 500 GeV;MeV/cm",125,0,25);
  TH1F* trackDeDxMatchedE1PSlice12Hist = new TH1F("trackDeDxMatchedE1PSlice12","Tracker dE/dx (matched to gen HSCP) E1 for track P > 500 GeV;MeV/cm",125,0,25);

  TH1F* trackDeDxE2BetaSlice1Hist = new TH1F("trackDeDxE2BetaSlice1","Tracker dE/dx E2 for beta < 0.2;MeV/cm",125,0,25);
  TH1F* trackDeDxE2BetaSlice2Hist = new TH1F("trackDeDxE2BetaSlice2","Tracker dE/dx E2 for 0.2 < beta < 0.4;MeV/cm",125,0,25);
  TH1F* trackDeDxE2BetaSlice3Hist = new TH1F("trackDeDxE2BetaSlice3","Tracker dE/dx E2 for 0.4 < beta < 0.6;MeV/cm",125,0,25);
  TH1F* trackDeDxE2BetaSlice4Hist = new TH1F("trackDeDxE2BetaSlice4","Tracker dE/dx E2 for 0.6 < beta < 0.8;MeV/cm",125,0,25);
  TH1F* trackDeDxE2BetaSlice5Hist = new TH1F("trackDeDxE2BetaSlice5","Tracker dE/dx E2 for 0.8 < beta < 1;MeV/cm",125,0,25);
  TH1F* trackDeDxE3BetaSlice1Hist = new TH1F("trackDeDxE3BetaSlice1","Tracker dE/dx E3 for beta < 0.2;MeV/cm",125,0,25);
  TH1F* trackDeDxE3BetaSlice2Hist = new TH1F("trackDeDxE3BetaSlice2","Tracker dE/dx E3 for 0.2 < beta < 0.4;MeV/cm",125,0,25);
  TH1F* trackDeDxE3BetaSlice3Hist = new TH1F("trackDeDxE3BetaSlice3","Tracker dE/dx E3 for 0.4 < beta < 0.6;MeV/cm",125,0,25);
  TH1F* trackDeDxE3BetaSlice4Hist = new TH1F("trackDeDxE3BetaSlice4","Tracker dE/dx E3 for 0.6 < beta < 0.8;MeV/cm",125,0,25);
  TH1F* trackDeDxE3BetaSlice5Hist = new TH1F("trackDeDxE3BetaSlice5","Tracker dE/dx E3 for 0.8 < beta < 1;MeV/cm",125,0,25);
  TH1F* trackDeDxD1BetaSlice1Hist = new TH1F("trackDeDxD1BetaSlice1","Tracker dE/dx D1 for beta < 0.2;MeV/cm",100,0,1);
  TH1F* trackDeDxD1BetaSlice2Hist = new TH1F("trackDeDxD1BetaSlice2","Tracker dE/dx D1 for 0.2 < beta < 0.4;MeV/cm",100,0,1);
  TH1F* trackDeDxD1BetaSlice3Hist = new TH1F("trackDeDxD1BetaSlice3","Tracker dE/dx D1 for 0.4 < beta < 0.6;MeV/cm",100,0,1);
  TH1F* trackDeDxD1BetaSlice4Hist = new TH1F("trackDeDxD1BetaSlice4","Tracker dE/dx D1 for 0.6 < beta < 0.8;MeV/cm",100,0,1);
  TH1F* trackDeDxD1BetaSlice5Hist = new TH1F("trackDeDxD1BetaSlice5","Tracker dE/dx D1 for 0.8 < beta < 1;MeV/cm",100,0,1);
  TH1F* trackDeDxD2BetaSlice1Hist = new TH1F("trackDeDxD2BetaSlice1","Tracker dE/dx D2 for beta < 0.2;MeV/cm",100,0,1);
  TH1F* trackDeDxD2BetaSlice2Hist = new TH1F("trackDeDxD2BetaSlice2","Tracker dE/dx D2 for 0.2 < beta < 0.4;MeV/cm",100,0,1);
  TH1F* trackDeDxD2BetaSlice3Hist = new TH1F("trackDeDxD2BetaSlice3","Tracker dE/dx D2 for 0.4 < beta < 0.6;MeV/cm",100,0,1);
  TH1F* trackDeDxD2BetaSlice4Hist = new TH1F("trackDeDxD2BetaSlice4","Tracker dE/dx D2 for 0.6 < beta < 0.8;MeV/cm",100,0,1);
  TH1F* trackDeDxD2BetaSlice5Hist = new TH1F("trackDeDxD2BetaSlice5","Tracker dE/dx D2 for 0.8 < beta < 1;MeV/cm",100,0,1);
  TH1F* trackDeDxD3BetaSlice1Hist = new TH1F("trackDeDxD3BetaSlice1","Tracker dE/dx D3 for beta < 0.2;MeV/cm",100,0,1);
  TH1F* trackDeDxD3BetaSlice2Hist = new TH1F("trackDeDxD3BetaSlice2","Tracker dE/dx D3 for 0.2 < beta < 0.4;MeV/cm",100,0,1);
  TH1F* trackDeDxD3BetaSlice3Hist = new TH1F("trackDeDxD3BetaSlice3","Tracker dE/dx D3 for 0.4 < beta < 0.6;MeV/cm",100,0,1);
  TH1F* trackDeDxD3BetaSlice4Hist = new TH1F("trackDeDxD3BetaSlice4","Tracker dE/dx D3 for 0.6 < beta < 0.8;MeV/cm",100,0,1);
  TH1F* trackDeDxD3BetaSlice5Hist = new TH1F("trackDeDxD3BetaSlice5","Tracker dE/dx D3 for 0.8 < beta < 1;MeV/cm",100,0,1);
  TH2F* trackEstimator1VsGenBetaHist = new TH2F("trackEstimator1VsGenBeta","Tracker dE/dx E1 vs. gen beta;;MeV/cm",100,0,1,125,0,25);
  TH2F* trackEstimator2VsGenBetaHist = new TH2F("trackEstimator2VsGenBeta","Tracker dE/dx E2 vs. gen beta;;MeV/cm",100,0,1,125,0,25);
  TH2F* trackEstimator3VsGenBetaHist = new TH2F("trackEstimator3VsGenBeta","Tracker dE/dx E3 vs. gen beta;;MeV/cm",100,0,1,125,0,25);
  TH2F* trackDiscriminator1VsGenBetaHist = new TH2F("trackDiscriminator1VsGenBeta","Tracker dE/dx D1 vs. gen beta;;MeV/cm",100,0,1,100,0,1);
  TH2F* trackDiscriminator2VsGenBetaHist = new TH2F("trackDiscriminator2VsGenBeta","Tracker dE/dx D2 vs. gen beta;;MeV/cm",100,0,1,100,0,1);
  TH2F* trackDiscriminator3VsGenBetaHist = new TH2F("trackDiscriminator3VsGenBeta","Tracker dE/dx D3 vs. gen beta;;MeV/cm",100,0,1,100,0,1);
  // TK dE/dx -- eta slices
  TH1F* trackDeDxE1EtaSliceHists[10];
  trackDeDxE1EtaSliceHists[0] = new TH1F("trackDeDxE1EtaSlice1","Tracker dE/dx E1 for |eta| < 0.25",125,0,25);
  trackDeDxE1EtaSliceHists[1] = new TH1F("trackDeDxE1EtaSlice2","Tracker dE/dx E1 for 0.25 < |eta| < 0.5",125,0,25);
  trackDeDxE1EtaSliceHists[2] = new TH1F("trackDeDxE1EtaSlice3","Tracker dE/dx E1 for 0.5 < |eta| < 0.75",125,0,25);
  trackDeDxE1EtaSliceHists[3] = new TH1F("trackDeDxE1EtaSlice4","Tracker dE/dx E1 for 0.75 < |eta| < 1.0",125,0,25);
  trackDeDxE1EtaSliceHists[4] = new TH1F("trackDeDxE1EtaSlice5","Tracker dE/dx E1 for 1.0 < |eta| < 1.25",125,0,25);
  trackDeDxE1EtaSliceHists[5] = new TH1F("trackDeDxE1EtaSlice6","Tracker dE/dx E1 for 1.25 < |eta| < 1.5",125,0,25);
  trackDeDxE1EtaSliceHists[6] = new TH1F("trackDeDxE1EtaSlice7","Tracker dE/dx E1 for 1.5 < |eta| < 1.75",125,0,25);
  trackDeDxE1EtaSliceHists[7] = new TH1F("trackDeDxE1EtaSlice8","Tracker dE/dx E1 for 1.75 < |eta| < 2.0",125,0,25);
  trackDeDxE1EtaSliceHists[8] = new TH1F("trackDeDxE1EtaSlice9","Tracker dE/dx E1 for 2.0 < |eta| < 2.25",125,0,25);
  trackDeDxE1EtaSliceHists[9] = new TH1F("trackDeDxE1EtaSlice10","Tracker dE/dx E1 for 2.25 < |eta| < 2.5",125,0,25);
  // TK eta distribution in Pt slices
  TH1F* trackEtaPtSlice1Hist = new TH1F("trackEtaPtSlice1","Track #eta for track 15 < Pt < 25 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice2Hist = new TH1F("trackEtaPtSlice2","Track #eta for track 25 < Pt < 50 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice3Hist = new TH1F("trackEtaPtSlice3","Track #eta for track 50 < Pt < 75 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice4Hist = new TH1F("trackEtaPtSlice4","Track #eta for track 75 < Pt < 100 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice5Hist = new TH1F("trackEtaPtSlice5","Track #eta for track 100 < Pt < 125 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice6Hist = new TH1F("trackEtaPtSlice6","Track #eta for track 125 < Pt < 150 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice7Hist = new TH1F("trackEtaPtSlice7","Track #eta for track 150 < Pt < 175 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice8Hist = new TH1F("trackEtaPtSlice8","Track #eta for track 175 < Pt < 200 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice9Hist = new TH1F("trackEtaPtSlice9","Track #eta for track 200 < Pt < 300 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice10Hist = new TH1F("trackEtaPtSlice10","Track #eta for track 300 < Pt < 400 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice11Hist = new TH1F("trackEtaPtSlice11","Track #eta for track 400 < Pt < 500 GeV;MeV/cm",300,-3,3);
  TH1F* trackEtaPtSlice12Hist = new TH1F("trackEtaPtSlice12","Track #eta for track Pt > 500 GeV;MeV/cm",300,-3,3);
  // TK -- number of dE/dx measurements
  TH1F* trackNumberOfMeasurementsHist = new TH1F("trackNumberOfMeasurements","Track number of dE/dx measurements",35,0,35);
  TH1F* trackNumberOfMeasurementsMatchedHist = new TH1F("trackNumberOfMeasurementsMatched","Track number of dE/dx measurements (matched to HSCP)",35,0,35);
  TH2F* trackNumberOfMeasurementsVsEtaHist = new TH2F("trackNumberOfMeasurementsVsEta","Track number of dE/dx measurements vs. #eta;#eta",600,-3,3,35,0,35);
  TH2F* trackNumberOfMeasurementsVsEtaMatchedHist = new TH2F("trackNumberOfMeasurementsVsEtaMatched","Track number of dE/dx measurements vs. #eta (matched to HSCP);#eta",600,-3,3,35,0,35);
  TH2F* trackNumberOfMeasurementsVsPHist = new TH2F("trackNumberOfMeasurementsVsP","Track number of measurements vs. momentum",250,0,1000,35,0,35);
  TH2F* trackNumberOfMeasurementsVsPMatchedHist = new TH2F("trackNumberOfMeasurementsVsPMatched","Track number of measurements vs. momentum (matched to HSCP)",250,0,1000,35,0,35);
  // TK -- number of saturated dE/dx measurements
  TH1F* trackNumberOfSatMeasurementsHist = new TH1F("trackNumberOfSatMeasurements","Track number of saturated dE/dx measurements",35,0,35);
  TH1F* trackNumberOfSatMeasurementsMatchedHist = new TH1F("trackNumberOfSatMeasurementsMatched","Track number of saturated dE/dx measurements (matched to HSCP)",35,0,35);
  TH2F* trackNumberOfSatMeasurementsVsEtaHist = new TH2F("trackNumberOfSatMeasurementsVsEta","Track number of saturated dE/dx measurements vs. #eta;#eta",600,-3,3,35,0,35);
  TH2F* trackNumberOfSatMeasurementsVsEtaMatchedHist = new TH2F("trackNumberOfSatMeasurementsVsEtaMatched","Track number of saturated dE/dx measurements vs. #eta (matched to HSCP);#eta",600,-3,3,35,0,35);
  TH2F* trackNumberOfSatMeasurementsVsPHist = new TH2F("trackNumberOfSatMeasurementsVsP","Track number of saturated measurements vs. momentum",250,0,1000,35,0,35);
  TH2F* trackNumberOfSatMeasurementsVsPMatchedHist = new TH2F("trackNumberOfSatMeasurementsVsPMatched","Track number of saturated measurements vs. momentum (matched to HSCP)",250,0,1000,35,0,35);
  // TK -- dE/dx E1 in NoM slices
  TH1F* trackDeDxE1NoMSliceHists[5];
  trackDeDxE1NoMSliceHists[0] = new TH1F("trackDeDxE1NoMSlice1","Tracker dE/dx for 1-5 dE/dx measurements",125,0,25);
  trackDeDxE1NoMSliceHists[1] = new TH1F("trackDeDxE1NoMSlice2","Tracker dE/dx for 6-10 dE/dx measurements",125,0,25);
  trackDeDxE1NoMSliceHists[2] = new TH1F("trackDeDxE1NoMSlice3","Tracker dE/dx for 11-15 dE/dx measurements",125,0,25);
  trackDeDxE1NoMSliceHists[3] = new TH1F("trackDeDxE1NoMSlice4","Tracker dE/dx for 16-20 dE/dx measurements",125,0,25);
  trackDeDxE1NoMSliceHists[4] = new TH1F("trackDeDxE1NoMSlice5","Tracker dE/dx for 21+ dE/dx measurements",125,0,25);
  TH1F* trackDeDxE1NoM15Hist = new TH1F("trackDeDxE1NoM15","Tracker dE/dx for 15 dE/dx E1 measurements",250,0,50);
  TH1F* trackDeDxE1NoM10Hist = new TH1F("trackDeDxE1NoM10","Tracker dE/dx for 10 dE/dx E1 measurements",250,0,50);
  TH1F* trackDeDxE1NoM6Hist = new TH1F("trackDeDxE1NoM6","Tracker dE/dx for 6 dE/dx E1 measurements",250,0,50);
  TH1F* trackDeDxE1NoM4Hist = new TH1F("trackDeDxE1NoM4","Tracker dE/dx for 4 dE/dx E1 measurements",250,0,50);
  TH1F* trackDeDxE1NoM1Hist = new TH1F("trackDeDxE1NoM1","Tracker dE/dx for 1 dE/dx E1 measurement",250,0,50);
  // TK -- dE/dx E1 in NoM slices (matched)
  TH1F* trackDeDxE1MatchedNoMSliceHists[5];
  trackDeDxE1MatchedNoMSliceHists[0] = new TH1F("trackDeDxE1MatchedNoMSlice1","Tracker dE/dx for 1-5 dE/dx measurements (matched to HSCP)",125,0,25);
  trackDeDxE1MatchedNoMSliceHists[1] = new TH1F("trackDeDxE1MatchedNoMSlice2","Tracker dE/dx for 6-10 dE/dx measurements (matched to HSCP)",125,0,25);
  trackDeDxE1MatchedNoMSliceHists[2] = new TH1F("trackDeDxE1MatchedNoMSlice3","Tracker dE/dx for 11-15 dE/dx measurements (matched to HSCP)",125,0,25);
  trackDeDxE1MatchedNoMSliceHists[3] = new TH1F("trackDeDxE1MatchedNoMSlice4","Tracker dE/dx for 16-20 dE/dx measurements (matched to HSCP)",125,0,25);
  trackDeDxE1MatchedNoMSliceHists[4] = new TH1F("trackDeDxE1MatchedNoMSlice5","Tracker dE/dx for 21+ dE/dx measurements (matched to HSCP)",125,0,25);

  // TK -- Look at PDFs, number of measurements and Pt bins (integrated over eta)
  for(int i=0; i<numPtBins_; ++i)
  {
    for(int j=0; j<numNomBins_; ++j)
    {
      int index = numNomBins_*i+j+1;
      std::string name = "histForPDFNoMPtBinsIndex";
      name+=intToString(index);
      std::string nameMatched = "histForPDFNoMPtBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 in Pt/Nom bin ";
      title+=intToString(index);
      std::string titleMatched = "Track dE/dx E1 (matched to HSCP) in Pt/Nom bin ";
      titleMatched+=intToString(index);
      histsForPDFNoMPtBins_[i][j] = new TH1F(name.c_str(),title.c_str(),125,0,25);
      histsForPDFNoMPtBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),125,0,25);
    }
  }
  // TK -- Look at PDFs, number of measurements and P bins (integrated over eta)
  for(int i=0; i<numPBins_; ++i)
  {
    for(int j=0; j<numNomBins_; ++j)
    {
      int index = numNomBins_*i+j+1;
      std::string name = "histForPDFNoMPBinsIndex";
      name+=intToString(index);
      std::string nameMatched = "histForPDFNoMPBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 in P/Nom bin ";
      title+=intToString(index);
      std::string titleMatched = "Track dE/dx E1 (matched to HSCP) in P/Nom bin ";
      titleMatched+=intToString(index);
      histsForPDFNoMPBins_[i][j] = new TH1F(name.c_str(),title.c_str(),125,0,25);
      histsForPDFNoMPBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),125,0,25);
    }
  }
  // TK -- Look at PDFs, number of measurements and P bins (integrated over eta) -- Mu timing SB
  for(int i=0; i<numPBins_; ++i)
  {
    for(int j=0; j<numNomBins_; ++j)
    {
      int index = numNomBins_*i+j+1;
      std::string name = "histForPDFNoMPBinsMuSBIndex";
      name+=intToString(index);
      std::string nameMatched = "histForPDFNoMPBinsMuSBMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 in P/Nom (muon timing SB) bin ";
      title+=intToString(index);
      std::string titleMatched = "Track dE/dx E1 (matched to HSCP) in P/Nom (muon timing SB) bin ";
      titleMatched+=intToString(index);
      histsForPDFNoMPBinsMuSB_[i][j] = new TH1F(name.c_str(),title.c_str(),125,0,25);
      histsForPDFNoMPBinsMuSBMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),125,0,25);
    }
  }
  // TK -- dE/dx, Fine momentum bins
  for(int i=0; i<200; ++i)
  {
    for(int j=0; j<5; ++j)
    {
      int index = 5*i+j+1;
      std::string name = "histsInNoMFinePBinsIndex";
      //std::string name = "pullsInNoMFinePBinsIndex";
      name+=intToString(index);
      std::string nameMatched = "histsInNoMFinePBinsMatchedIndex";
      //std::string nameMatched = "pullsInNoMFinePBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx all hits in P/Nom bin ";
      title+=intToString(index);
      int leftPBound = i*5;
      int rightPBound = (i+1)*5;
      int leftNoMBound = 5*j+1;
      int rightNoMBound = 5*(j+1);
      title+=generateTitleEnd(leftPBound,rightPBound,leftNoMBound,rightNoMBound);
      std::string titleMatched = "Track dE/dx all hits (matched to HSCP) in P/Nom bin ";
      titleMatched+=intToString(index);
      titleMatched+=generateTitleEnd(leftPBound,rightPBound,leftNoMBound,rightNoMBound);
      histsInNoMFinePBins_[i][j] = new TH1F(name.c_str(),title.c_str(),125,0,25);
      histsInNoMFinePBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),125,0,25);
      //histsInNoMFinePBins_[i][j] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
      //histsInNoMFinePBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
    }
  }
  // TK -- dE/dx, fine momentum bins, all tracks for closure
  for(int i=0; i<200; ++i)
  {
    int index = i+1;
    std::string name = "dedxClosureInFinePBinsIndex";
    name+=intToString(index);
    std::string nameMatched = "dedxClosureInFinePBinsMatchedIndex";
    nameMatched+=intToString(index);
    std::string title = "Track dE/dx all hits in P bin ";
    title+=intToString(index);
    int leftPBound = i*5;
    int rightPBound = (i+1)*5;
    title+=generateTitleEnd(leftPBound,rightPBound);
    std::string titleMatched = "Track dE/dx all hits (matched to HSCP) in P bin ";
    titleMatched+=intToString(index);
    titleMatched+=generateTitleEnd(leftPBound,rightPBound);
    dedxClosureInFinePBins_[i] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
    dedxClosureInFinePBinsMatched_[i] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
    //dedxClosureInFinePBins_[i] = new TH1F(name.c_str(),title.c_str(),125,0,25);
    //dedxClosureInFinePBinsMatched_[i] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),125,0,25);
  }
  // TK -- dE/dx, Fine NoM bins
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<30; ++j)
    {
      int index = 30*i+j+1;
      std::string name = "histsInFineNoMPBinsIndex";
      name+=intToString(index);
      std::string nameMatched = "histsInFineNoMPBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 (p, #eta bias out) in P/Nom bin ";
      title+=intToString(index);
      title+=" for P in P bin ";
      title+=intToString(i);
      title+=" for Nom=";
      title+=intToString(j+1);
      std::string titleMatched = "Track dE/dx E1 (p, #eta bias out, matched to HSCP) in P/Nom bin ";
      titleMatched+=intToString(index);
      titleMatched+=" for P in P bin ";
      titleMatched+=intToString(i);
      titleMatched+=" for Nom=";
      titleMatched+=intToString(j+1);
      histsInFineNoMPBins_[i][j] = new TH1F(name.c_str(),title.c_str(),115,0,15);
      histsInFineNoMPBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),115,0,15);
      // eta dists in these bins
      name+="etaDist";
      nameMatched+= "etaDist";
      histsInFineNoMPBinsEtaDist_[i][j] = new TH1F(name.c_str(),title.c_str(),250,-2.5,2.5);
      histsInFineNoMPBinsMatchedEtaDist_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),250,-2.5,2.5);
    }
  }
  // TK -- dE/dx, Fine NoM bins pulls
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<30; ++j)
    {
      int index = 30*i+j+1;
      std::string name = "pullInFineNoMPBinsIndex";
      name+=intToString(index);
      std::string nameMatched = "pullInFineNoMPBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 in P/Nom bin ";
      title+=intToString(index);
      title+=" for P in P bin ";
      title+=intToString(i);
      title+=" for Nom=";
      title+=intToString(j+1);
      std::string titleMatched = "Track dE/dx E1 (matched to HSCP) in P/Nom bin ";
      titleMatched+=intToString(index);
      titleMatched+=" for P in P bin ";
      titleMatched+=intToString(i);
      titleMatched+=" for Nom=";
      titleMatched+=intToString(j+1);
      pullsInFineNoMPBins_[i][j] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
      pullsInFineNoMPBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
    }
  }
  // TK -- dE/dx, fine NoM bins for closure
  for(int i=0; i<30; ++i)
  {
    int index = i+1;
    std::string name = "dedxClosureInFineNoMBinsIndex";
    name+=intToString(index);
    std::string nameMatched = "dedxClosureInFineNoMBinsMatchedIndex";
    nameMatched+=intToString(index);
    std::string title = "Track dE/dx E1 in NoM bin ";
    title+=intToString(index);
    title+=" for Nom=";
    title+=intToString(i+1);
    std::string titleMatched = "Track dE/dx E1 (matched to HSCP) in NoM bin ";
    titleMatched+=intToString(index);
    titleMatched+=" for Nom=";
    titleMatched+=intToString(i+1);
    dedxClosureInFineNoMBins_[i] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
    dedxClosureInFineNoMBinsMatched_[i] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
  }
  // TK -- dE/dx, Fine eta bins
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<25; ++j)
    {
      int index = 25*i+j+1;
      //std::string name = "pullsInFineEtaPBinsIndex";
      std::string name = "histsInFineEtaPBinsIndex";
      name+=intToString(index);
      //std::string nameMatched = "pullsInFineEtaPBinsMatchedIndex";
      std::string nameMatched = "histsInFineEtaPBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 (p bias out) in P/Nom bin ";
      title+=intToString(index);
      title+=" for P in P bin ";
      title+=intToString(i);
      title+=" for eta bin ";
      title+=intToString(j+1);
      std::string titleMatched = "Track dE/dx E1 (p bias out, matched to HSCP) in P/Nom bin ";
      titleMatched+=intToString(index);
      titleMatched+=" for P in P bin ";
      titleMatched+=intToString(i);
      titleMatched+=" for eta bin ";
      titleMatched+=intToString(j+1);
      //histsInFineEtaPBins_[i][j] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
      //histsInFineEtaPBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
      histsInFineEtaPBins_[i][j] = new TH1F(name.c_str(),title.c_str(),115,0,15);
      histsInFineEtaPBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),115,0,15);
    }
  }
  // TK -- dE/dx, Fine eta bins pulls
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<25; ++j)
    {
      int index = 25*i+j+1;
      std::string name = "pullsInFineEtaPBinsIndex";
      name+=intToString(index);
      std::string nameMatched = "pullsInFineEtaPBinsMatchedIndex";
      nameMatched+=intToString(index);
      std::string title = "Track dE/dx E1 (p bias out) in P/Nom bin ";
      title+=intToString(index);
      title+=" for P in P bin ";
      title+=intToString(i);
      title+=" for eta bin ";
      title+=intToString(j+1);
      std::string titleMatched = "Track dE/dx E1 (p bias out, matched to HSCP) in P/Nom bin ";
      titleMatched+=intToString(index);
      titleMatched+=" for P in P bin ";
      titleMatched+=intToString(i);
      titleMatched+=" for eta bin ";
      titleMatched+=intToString(j+1);
      pullsInFineEtaPBins_[i][j] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
      pullsInFineEtaPBinsMatched_[i][j] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
    }
  }
  // TK -- dE/dx, fine eta bins, all tracks for closure
  for(int i=0; i<25; ++i)
  {
    int index = i+1;
    std::string name = "dedxClosureInFineEtaBinsIndex";
    name+=intToString(index);
    std::string nameMatched = "dedxClosureInFineEtaBinsMatchedIndex";
    nameMatched+=intToString(index);
    std::string title = "Track dE/dx all hits in eta bin ";
    title+=intToString(index);
    float leftPBound = i*0.1;
    float rightPBound = (i+1)*0.1;
    title+=generateEtaTitleEnd(leftPBound,rightPBound);
    std::string titleMatched = "Track dE/dx all hits (matched to HSCP) in eta bin ";
    titleMatched+=intToString(index);
    titleMatched+=generateEtaTitleEnd(leftPBound,rightPBound);
    dedxClosureInFineEtaBins_[i] = new TH1F(name.c_str(),title.c_str(),100,-10,10);
    dedxClosureInFineEtaBinsMatched_[i] = new TH1F(nameMatched.c_str(),titleMatched.c_str(),100,-10,10);
  }
  // TK dE/dx hit dists
  std::vector<TGraph> dedxHitGraphs;
  TH1F* trackDeDxAllHitsHist = new TH1F("trackDeDxAllHitsHist","Tracker dE/dx of all hits",800,0,200);
  TH1F* trackDeDxAllHits1NoMHist = new TH1F("trackDeDxAllHits1NoMHist","Tracker dE/dx of all hits with 1 dE/dx measurements",800,0,200);
  TH1F* trackDeDxAllHits4NoMHist = new TH1F("trackDeDxAllHits4NoMHist","Tracker dE/dx of all hits with 4 dE/dx measurements",800,0,200);
  TH1F* trackDeDxAllHits6NoMHist = new TH1F("trackDeDxAllHits6NoMHist","Tracker dE/dx of all hits with 6 dE/dx measurements",800,0,200);
  TH1F* trackDeDxAllHits10NoMHist = new TH1F("trackDeDxAllHits10NoMHist","Tracker dE/dx of all hits with 10 dE/dx measurements",800,0,200);
  TH1F* trackDeDxAllHits15NoMHist = new TH1F("trackDeDxAllHits15NoMHist","Tracker dE/dx of all hits with 15 dE/dx measurements",800,0,200);
  TH1F* trackDeDxAllHitsBigDeDxHist = new TH1F("trackDeDxAllHitsBigDeDxHist","Tracker dE/dx of all hits with dE/dx E1 > 9 MeV/cm",800,0,200);
  TH2F* trackDeDxAllHitsVsNosHist = new TH2F("trackDeDxAllHitsVsNosHist","Tracker dE/dx of all hits vs. number of saturating measurements",50,0,50,800,0,200);
  TH1F* trackDeDxRms3Hist = new TH1F("trackDeDxRms3","RMS of track dE/dx hist dist for dE/dx > 3 MeV/cm and NoM > 14",100,0,100);
  TH1F* trackDeDxRms4Hist = new TH1F("trackDeDxRms4","RMS of track dE/dx hist dist for dE/dx > 4 MeV/cm and NoM > 14",100,0,100);
  TH1F* trackDeDxRms5Hist = new TH1F("trackDeDxRms5","RMS of track dE/dx hist dist for dE/dx > 5 MeV/cm and NoM > 14",100,0,100);
  TH1F* trackDeDxRms3MatchedHist = new TH1F("trackDeDxRms3Matched","RMS of track dE/dx hist dist for dE/dx > 3 MeV/cm and NoM > 14 (matched to HSCP)",100,0,100);
  TH1F* trackDeDxRms4MatchedHist = new TH1F("trackDeDxRms4Matched","RMS of track dE/dx hist dist for dE/dx > 4 MeV/cm and NoM > 14 (matched to HSCP)",100,0,100);
  TH1F* trackDeDxRms5MatchedHist = new TH1F("trackDeDxRms5Matched","RMS of track dE/dx hist dist for dE/dx > 5 MeV/cm and NoM > 14 (matched to HSCP)",100,0,100);
  //TH2F* trackDeDxE1VsPathLengthHist = new TH2F("trackDeDxE1VsPathLength","Track dE/dx E1 vs. path length;cm;MeV/cm",100,0,100,125,0,25);
  //TH2F* trackDeDxE1VsPathLengthMatchedHist = new TH2F("trackDeDxE1VsPathLengthMatched","Track dE/dx E1 vs. path length (matched to HSCP);cm;MeV/cm",100,0,100,125,0,25);
  //TH2F* trackDeDxPathLengthVsEtaHist = new TH2F("trackDeDxPathLengthVsEta","Track dE/dx path length vs #eta",300,-3,3,100,0,100);
  //TH2F* trackDeDxPathLengthVsEtaMatchedHist = new TH2F("trackDeDxPathLengthVsEtaMatched","Track dE/dx path length vs #eta (matched to HSCP)",300,-3,3,100,0,100);

  // ECAL dE/dx
  TH1F* ecalDeDxTotalHist = new TH1F("ecalDeDxTotal","Ecal dE/dx total;MeV/cm",1000,0,100);
  TH1F* ecalDeDxHighPtHist = new TH1F("ecalDeDxHighPt","Ecal dE/dx when Pt > 30 GeV;MeV/cm",1000,0,100);
  TH1F* ecalDeDxLowPtHist = new TH1F("ecalDeDxLowPt","Ecal dE/dx when Pt < 30 GeV;MeV/cm",1000,0,100);
  TH1F* ecalDeDxMod1Hist = new TH1F("ecalDeDxMod1","Ecal dE/dx for hits in EB module 1",1000,0,100);
  TH1F* ecalDeDxMod2Hist = new TH1F("ecalDeDxMod2","Ecal dE/dx for hits in EB module 2",1000,0,100);
  TH1F* ecalDeDxMod3Hist = new TH1F("ecalDeDxMod3","Ecal dE/dx for hits in EB module 3",1000,0,100);
  TH1F* ecalDeDxMod4Hist = new TH1F("ecalDeDxMod4","Ecal dE/dx for hits in EB module 4",1000,0,100);
  TH1F* ecalDeDxEEHist = new TH1F("ecalDeDxEE","Ecal dE/dx for hits in EE",1000,0,100);
  TH1F* ecalDeDxMod12BoundHist = new TH1F("ecalDeDxMod12Bound","Ecal dE/dx for hits near EB module 1-2 boundary",1000,0,100);
  TH1F* ecalDeDxMod23BoundHist = new TH1F("ecalDeDxMod23Bound","Ecal dE/dx for hits near EB module 2-3 boundary",1000,0,100);
  TH1F* ecalDeDxMod34BoundHist = new TH1F("ecalDeDxMod34Bound","Ecal dE/dx for hits near EB module 3-4 boundary",1000,0,100);
  TH1F* ecalDeDxSMBoundHist = new TH1F("ecalDeDxSMBound","Ecal dE/dx for hits near SM boundaries",1000,0,100);
  // ECAL and Tk dE/dx
  TH2F* trackDeDxE1vsEcalDeDxHist = new TH2F("trackDeDxE1vsEcalDeDx","Track dE/dx E1 vs. Ecal dE/dx;Ecal [MeV/cm];Tk [MeV/cm]",1000,0,100,500,0,100);
  TH2F* trackDeDxE2vsEcalDeDxHist = new TH2F("trackDeDxE2vsEcalDeDx","Track dE/dx E2 vs. Ecal dE/dx;Ecal [MeV/cm];Tk [MeV/cm]",1000,0,100,500,0,100);
  TH2F* trackDeDxE3vsEcalDeDxHist = new TH2F("trackDeDxE3vsEcalDeDx","Track dE/dx E3 vs. Ecal dE/dx;Ecal [MeV/cm];Tk [MeV/cm]",1000,0,100,500,0,100);
  TH1F* ecalDeDxHighTKDeDxHist = new TH1F("ecalDeDxHighTKDeDx","Ecal dE/dx when TK dE/dx > 3 MeV/cm;MeV/cm",1000,0,100);
  TH1F* ecalDeDxLowTKDeDxHist = new TH1F("ecalDeDxHighTKDeDx","Ecal dE/dx when TK dE/dx < 3 MeV/cm;MeV/cm",1000,0,100);
  TH1F* ecalDeDxHighTKOverLowTKHist = new TH1F();
  // Ecal dE/dx, TK dE/dx, Pt
  TH1F* ecalDeDxHighTKDeDxHighPtHist = new TH1F("ecalDeDxHighTKDeDxHighPt","Ecal dE/dx when TK dE/dx > 3 MeV/cm and Pt > 30 GeV;MeV/cm",1000,0,100);
  TH1F* ecalDeDxLowTKDeDxHighPtHist = new TH1F("ecalDeDxLowTKDeDxHighPt","Ecal dE/dx when TK dE/dx < 3 MeV/cm and Pt > 30 GeV;MeV/cm",1000,0,100);
  TH1F* ecalDeDxHighTKDeDxLowPtHist = new TH1F("ecalDeDxHighTKDeDxLowPt","Ecal dE/dx when TK dE/dx > 3 MeV/cm and Pt < 30 GeV;MeV/cm",1000,0,100);
  TH1F* ecalDeDxLowTKDeDxLowPtHist = new TH1F("ecalDeDxLowTKDeDxLowPt","Ecal dE/dx when TK dE/dx < 3 MeV/cm and Pt < 30 GeV;MeV/cm",1000,0,100);
  // ECAL timing
  TH1F* ecalTimingHist = new TH1F("ecalTiming","Ecal timing;ns",200,-25,25);
  TH1F* ecalHitTimingEslice1Hist = new TH1F("ecalHitTimingEslice1","Ecal hit timing for E < 0.2 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice1noEBHist = new TH1F("ecalHitTimingEslice1noEB","Ecal EE hit timing for E < 0.2 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice2Hist = new TH1F("ecalHitTimingEslice2","Ecal hit timing for 0.2 < E < 0.4 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice2noEBHist = new TH1F("ecalHitTimingEslice2noEB","Ecal EE hit timing for 0.2 < E < 0.4 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice3Hist = new TH1F("ecalHitTimingEslice3","Ecal hit timing for 0.4 < E < 0.6 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice3noEBHist = new TH1F("ecalHitTimingEslice3noEB","Ecal EE hit timing for 0.4 < E < 0.6 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice4Hist = new TH1F("ecalHitTimingEslice4","Ecal hit timing for 0.6 < E < 0.8 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice4noEBHist = new TH1F("ecalHitTimingEslice4noEB","Ecal EE hit timing for 0.6 < E < 0.8 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice5Hist = new TH1F("ecalHitTimingEslice5","Ecal hit timing for 0.8 < E < 1.0 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice5noEBHist = new TH1F("ecalHitTimingEslice5noEB","Ecal EE hit timing for 0.8 < E < 1.0 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice6Hist = new TH1F("ecalHitTimingEslice6","Ecal hit timing for 1.0 < E < 1.5 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice6noEBHist = new TH1F("ecalHitTimingEslice6noEB","Ecal EE hit timing for 1.0 < E < 1.5 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice7Hist = new TH1F("ecalHitTimingEslice7","Ecal hit timing for 1.5 < E < 2.0 GeV;ns",200,-25,25);
  TH1F* ecalHitTimingEslice7noEBHist = new TH1F("ecalHitTimingEslice7noEB","Ecal EE hit timing for 1.5 < E < 2.0 GeV;ns",200,-25,25);
  // MUON timing
  TH1F* muonCombinedIBetaHist = new TH1F("muonCombinedIBeta","1/#beta from combined muon timing",200,0,2);
  TH1F* muonDtIBetaHist = new TH1F("muonDtIBeta","1/#beta from DT muon timing",200,0,2);
  TH1F* muonCscIBetaHist = new TH1F("muonCscIBeta","1/#beta from CSC muon timing",200,0,2);
  TH1F* muonCombinedIBetaErrHist = new TH1F("muonCombinedIBetaErr","1/#beta error from combined muon timing",100,0,1);
  TH1F* muonDtIBetaErrHist = new TH1F("muonDtIBetaErr","1/#beta error from DT muon timing",100,0,1);
  TH1F* muonCscIBetaErrHist = new TH1F("muonCscIBetaErr","1/#beta error from CSC muon timing",100,0,1);
  TH1F* muonCombinedBetaHist = new TH1F("muonCombinedBeta","#beta from combined muon timing",200,0,2);
  TH1F* muonDtBetaHist = new TH1F("muonDtBeta","#beta from DT muon timing",200,0,2);
  TH1F* muonCscBetaHist = new TH1F("muonCscBeta","#beta from CSC muon timing",200,0,2);
  // GEN
  TH1F* genBetaHist = new TH1F("genBeta","#beta of generated HSCPs",100,0,1);
  TH1F* genEtaHist = new TH1F("genEta","#eta of generated HSCPs",600,-3,3);
  TH1F* genPtHist = new TH1F("genPt","Pt of generated HSCPs;GeV",250,0,500);
  TH1F* genPHist = new TH1F("genP","P of generated HSCPs;GeV",500,0,1000);
  TH2F* genEtaVsPHist = new TH2F("genEtaVsP","Track eta vs. p of generated HSCPs;P [GeV];#eta",2000,0,2000,600,-3,3);
  TH1F* deltaRHist = new TH1F("deltaR","deltaR of tracks to gen HSCP",1000,0,5);
  TH1F* deltaREtaCentralHist = new TH1F("deltaREtaCentral","deltaR of tracks to gen HSCP, |eta| < 0.2",1000,0,5);
  TH1F* deltaREtaEndcapHist = new TH1F("deltaREtaEndcap","deltaR of tracks to gen HSCP, 2.0 < |eta| < 2.2",1000,0,5);

  //tFile->cd();

  bool speak = enableDebug_;
  int numEventsZeroMatchedTracks = 0;
  int numEventsOneMatchedTracks = 0;
  int numEventsTwoMatchedTracks = 0;
  int numTotalTracks = 0;

  // Dependence of mean(Ih) on P
  TF1* meanIhPDep = new TF1("meanIhPDep","[0]+[1]/x+[2]/(x*x)");
  meanIhPDep->SetParameters(1.669,1040,1.949e5);
  // Dependence of sigma(Ih) on P
  TF1* sigmaIhPDep = new TF1("sigmaIhPDep","[0]+[1]*x+[2]*x^2");
  sigmaIhPDep->SetParameters(1.061,-0.001565,7.994e-7);
  // see web: http://scooper.web.cern.ch/scooper/hscp/jul15/
  // fits from pcminn03:/data/scooper/cmssw/425/HSCPToyMC/src/HSCP2011/ToyMC/bin/dedxPlotsFinePSlices/

  // Dependence of mean(Ih) on eta (after removing P dependence)
  // see web: http://scooper.web.cern.ch/scooper/hscp/jul26/
  // fits from pcminn19:/data3/scooper/cmssw/426/HSCPStudiesAndToyMC/src/HSCP2011/ToyMC/bin/7.jul25.dedxPlotFineEtaBins.gluino500all.nom8to12only.biasOut
  // fits from pcminn19:/data3/scooper/cmssw/426/HSCPStudiesAndToyMC/src/HSCP2011/ToyMC/bin/11.jul26.dedxPlotFineEtaBins.gluino500all.etaBinsPbiasOut.nomOver9
  // for mean eta dependence
  TF1* meanIhEtaDep = new TF1("meanIhEtaDep",fitMeanEtaDependence,0,2.5,8);
  //meanIhEtaDep->SetParameters(-0.6376,1.079,1.334,0.1013,79.05,-67.23,25.82,-3.71);
  meanIhEtaDep->SetParameters(-0.6404,1.007,1.391,0.4401,43.74,-36.1,13.68,-1.962);

  // Dependence of mean(Ih) on NoM (after removing P and eta dependence)
  // fit from pcminn19:/data3/scooper/cmssw/426/HSCPStudiesAndToyMC/src/HSCP2011/ToyMC/bin/13.jul26.dedxPlots.g500.noPbiasNoEtaBias.nomOver6
  TF1* meanIhNoMDep = new TF1("meanIhNoMDep","[0]+[1]*x+[2]*x^2+[3]*x^3");
  meanIhNoMDep->SetParameters(0.7104,-0.1291,0.008316,-0.0002084);

  // Dependence of sigma(Ih) on eta (after removing P dependence)
  // fits from pcminn19:/data3/scooper/cmssw/426/HSCPStudiesAndToyMC/src/HSCP2011/ToyMC/bin/2.jul29.dedxEtaClosure
  TF1* sigmaIhEtaDep = new TF1("sigmaIhEtaDep","[0]+[1]*x+[2]*x^2+[3]*x^3+[4]*x^4+[5]*x^5+[6]*x^6+[7]*x^7");
  sigmaIhEtaDep->SetParameters(1.058,0.727,-5.421,12.84,-14.83,8.771,-2.547,0.2912);

  // Main loop over entries
  //numEvents_ = 26; // gluino500, assuming 60% trigger efficiency (23.3/pb)
  for(int entry = 0; entry < numEvents_; ++entry)
  {
    MyTree->GetEntry(entry);

    if(speak)
      std::cout << "Looking at HSCP Tree Content event: " << Event_EventNumber << std::endl;

    int numMatchedTracksInEvent = 0;
    bool firstGenLoop = true;

    for(int i=0; i < NHSCPs; ++i)
    {
      if(!Hscp_hasTrack[i]) // Need track infor
        continue;

      ++numTotalTracks;


      //XXX Cut on global muons
      //GLB==2, TK==4, STA==8, CALO==16, can combine
      //10      100    1000    10000
      // Require global muons
      //if((Muon_type[i]>>1 & 0x0001)!=1 || Muon_qualityValid[i]!=true)
      //  continue;
      // Veto any muon
      //if(Muon_type[i]!=0)
      //  continue;

      //XXX Cut on Ecal E/p (electron-like objects)
      //if(Calo_ecal_crossedE[i]/Track_p[i] > 0.9)
      //  continue;

      // Cut on track eta
      if(fabs(Track_eta[i]) > maxTrackEta_)
        continue;

      //XXX SIC TESTING ONLY
      //if(fabs(Track_eta[i]) < 1.6)
      //  continue;
      //if(Track_p[i] < 300 || Track_p[i] > 325)
      //  continue;
      //if(Track_dEdxE1_NOM[i] < 8 || Track_dEdxE1_NOM[i] > 12)
      //  continue;
      //if(Track_dEdxE1_NOS[i] < 5 || Track_dEdxE1_NOS[i] > 7)
      //  continue;
      //if(Track_dEdxE1_NOM[i] != 12)
      //  continue;
      //XXX FOR GLUINO 500, CUT ON TRACK P TO REMOVE HSCP THAT ARE TOO SLOW
      if(Track_p[i] < 200)
        continue;

      ////XXX Cut on track d0
      if(fabs(Track_d0[i]) > 0.2)
        continue;
      ////XXX Cut on dE/dx number of measurements
      if(Track_dEdxE1_NOM[i] < 6)
        continue;


      ////XXX FIXME REMOVE TEST TEST
      //if(Track_pt[i] <= 70)
      //  continue;
      //if(Track_dEdxD3[i] <= 0.3)
      //  continue;
      //XXX MASS CUT
      //float k = 2.468;
      //float c = 2.679;
      //float massSqr = (Track_dEdxE1[i]-c)*pow(Track_p[i],2)/k;
      //if(massSqr < 0)
      //  continue;
      //else if(sqrt(massSqr) < 267)
      //  continue;

      // AN NOTE selections
      //XXX track quality inner
      //XXX number of measurements inner
      //XXX pixel measurements inner
      //XXX chi2/ndf - global track
      //XXX eta (2.5 TK only, 2.4 TK+ToF)
      //XXX Pt > 25
      //XXX estimator > 3 MeV/cm
      //XXX TK iso --> Pt < 50 (100 for TK+tof) of tracks in deltaR < 0.3
      //XXX calorimeter iso --> 
      //XXX Pt err < 0.35
      //XXX sqrt(dz^2+dxy^2) < 2 cm
      //XXX TK+ToF, 1/beta > 1 (from tof)
      //XXX ToF NDF > 7

      trackChi2Hist->Fill(Track_chi2[i]);
      trackChi2NdfHist->Fill(Track_chi2[i]/Track_ndof[i]);
      trackDeDxE1Hist->Fill(Track_dEdxE1[i]);
      trackDeDxE2Hist->Fill(Track_dEdxE2[i]);
      trackDeDxE3Hist->Fill(Track_dEdxE3[i]);
      trackDeDxD1Hist->Fill(Track_dEdxD1[i]);
      trackDeDxD2Hist->Fill(Track_dEdxD2[i]);
      trackDeDxD3Hist->Fill(Track_dEdxD3[i]);
      trackPtHist->Fill(Track_pt[i]);
      trackEtaHist->Fill(Track_eta[i]);
      trackNohHist->Fill(Track_NOH[i]);
      trackEtaVsPtHist->Fill(Track_pt[i],Track_eta[i]);
      trackEtaVsPHist->Fill(Track_p[i],Track_eta[i]);
      trackNumberOfMeasurementsHist->Fill(Track_dEdxE1_NOM[i]);
      trackNumberOfSatMeasurementsHist->Fill(Track_dEdxE1_NOS[i]);
      trackNumberOfMeasurementsVsEtaHist->Fill(Track_eta[i],Track_dEdxE1_NOM[i]);
      trackNumberOfSatMeasurementsVsEtaHist->Fill(Track_eta[i],Track_dEdxE1_NOS[i]);
      trackNumberOfMeasurementsVsPHist->Fill(Track_p[i],Track_dEdxE1_NOM[i]);
      trackNumberOfSatMeasurementsVsPHist->Fill(Track_p[i],Track_dEdxE1_NOS[i]);
      trackPvsDeDxE1Hist->Fill(Track_dEdxE1[i],Track_p[i]);
      trackPvsDeDxD3Hist->Fill(Track_dEdxD3[i],Track_p[i]);
      trackPtvsDeDxE1Hist->Fill(Track_dEdxE1[i],Track_pt[i]);
      trackPtvsDeDxD3Hist->Fill(Track_dEdxD3[i],Track_pt[i]);
      trackDeDxE1VsDeDxD3Hist->Fill(Track_dEdxD3[i],Track_dEdxE1[i]);
      float trackPt = Track_pt[i];
      float trackP = Track_p[i];
      float trackEta = Track_eta[i];
      float trackPhi = Track_phi[i];
      float trackDeDxE1 = Track_dEdxE1[i];
      int trackNomE1 = Track_dEdxE1_NOM[i];
      // Muon timing plots
      if(Hscp_hasMuon[i])
      {
        muonCombinedIBetaHist->Fill(Muon_cb_IBeta[i]);
        muonCombinedIBetaErrHist->Fill(Muon_cb_IBeta_err[i]);
        muonCombinedBetaHist->Fill(1/Muon_cb_IBeta[i]);
        muonCscIBetaHist->Fill(Muon_csc_IBeta[i]);
        muonCscIBetaErrHist->Fill(Muon_csc_IBeta_err[i]);
        muonCscBetaHist->Fill(1/Muon_csc_IBeta[i]);
        muonDtIBetaHist->Fill(Muon_dt_IBeta[i]);
        muonDtIBetaErrHist->Fill(Muon_dt_IBeta_err[i]);
        muonDtBetaHist->Fill(1/Muon_dt_IBeta[i]);
        // track eta vs. pt plots in Muon timing sideband
        if(1/Muon_cb_IBeta[i] > 1.25 && Muon_cb_IBeta_err[i] < 0.2)
        {
          trackEtaVsPtMuTimingSBHist->Fill(Track_pt[i],Track_eta[i]);
          trackEtaVsPMuTimingSBHist->Fill(Track_p[i],Track_eta[i]);
          // Slices in P and NoM bins -- for PDF determination (5,5 currently)
          if(trackP < 50)
          {
            if(trackNomE1 >=1 && trackNomE1 <= 5)
              histsForPDFNoMPBinsMuSB_[0][0]->Fill(trackDeDxE1); 
            else if(trackNomE1 >= 6 && trackNomE1 <= 10)
              histsForPDFNoMPBinsMuSB_[0][1]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 11 && trackNomE1 <= 15)
              histsForPDFNoMPBinsMuSB_[0][2]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 16 && trackNomE1 <= 20)
              histsForPDFNoMPBinsMuSB_[0][3]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 21)
              histsForPDFNoMPBinsMuSB_[0][4]->Fill(trackDeDxE1);
          }
          else if(trackP < 150)
          {
            if(trackNomE1 >=1 && trackNomE1 <= 5)
              histsForPDFNoMPBinsMuSB_[1][0]->Fill(trackDeDxE1); 
            else if(trackNomE1 >= 6 && trackNomE1 <= 10)
              histsForPDFNoMPBinsMuSB_[1][1]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 11 && trackNomE1 <= 15)
              histsForPDFNoMPBinsMuSB_[1][2]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 16 && trackNomE1 <= 20)
              histsForPDFNoMPBinsMuSB_[1][3]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 21)
              histsForPDFNoMPBinsMuSB_[1][4]->Fill(trackDeDxE1);
          }
          else if(trackP < 300)
          {
            if(trackNomE1 >=1 && trackNomE1 <= 5)
              histsForPDFNoMPBinsMuSB_[2][0]->Fill(trackDeDxE1); 
            else if(trackNomE1 >= 6 && trackNomE1 <= 10)
              histsForPDFNoMPBinsMuSB_[2][1]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 11 && trackNomE1 <= 15)
              histsForPDFNoMPBinsMuSB_[2][2]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 16 && trackNomE1 <= 20)
              histsForPDFNoMPBinsMuSB_[2][3]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 21)
              histsForPDFNoMPBinsMuSB_[2][4]->Fill(trackDeDxE1);
          }
          else if(trackP < 450)
          {
            if(trackNomE1 >=1 && trackNomE1 <= 5)
              histsForPDFNoMPBinsMuSB_[3][0]->Fill(trackDeDxE1); 
            else if(trackNomE1 >= 6 && trackNomE1 <= 10)
              histsForPDFNoMPBinsMuSB_[3][1]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 11 && trackNomE1 <= 15)
              histsForPDFNoMPBinsMuSB_[3][2]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 16 && trackNomE1 <= 20)
              histsForPDFNoMPBinsMuSB_[3][3]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 21)
              histsForPDFNoMPBinsMuSB_[3][4]->Fill(trackDeDxE1);
          }
          else if(trackP > 450)
          {
            if(trackNomE1 >=1 && trackNomE1 <= 5)
              histsForPDFNoMPBinsMuSB_[4][0]->Fill(trackDeDxE1); 
            else if(trackNomE1 >= 6 && trackNomE1 <= 10)
              histsForPDFNoMPBinsMuSB_[4][1]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 11 && trackNomE1 <= 15)
              histsForPDFNoMPBinsMuSB_[4][2]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 16 && trackNomE1 <= 20)
              histsForPDFNoMPBinsMuSB_[4][3]->Fill(trackDeDxE1);
            else if(trackNomE1 >= 21)
              histsForPDFNoMPBinsMuSB_[4][4]->Fill(trackDeDxE1);
          }
        }
      }

      // TK dE/dx -- eta slices
      if(fabs(trackEta) < 0.25)
        trackDeDxE1EtaSliceHists[0]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 0.5)
        trackDeDxE1EtaSliceHists[1]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 0.75)
        trackDeDxE1EtaSliceHists[2]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 1.0)
        trackDeDxE1EtaSliceHists[3]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 1.25)
        trackDeDxE1EtaSliceHists[4]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 1.5)
        trackDeDxE1EtaSliceHists[5]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 1.75)
        trackDeDxE1EtaSliceHists[6]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 2.0)
        trackDeDxE1EtaSliceHists[7]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 2.25)
        trackDeDxE1EtaSliceHists[8]->Fill(trackDeDxE1);
      else if(fabs(trackEta) < 2.5)
        trackDeDxE1EtaSliceHists[9]->Fill(trackDeDxE1);

      // TK dE/dx -- NoM slices
      if(trackNomE1 >= 1 && trackNomE1 <= 5)
      {
        trackDeDxE1NoMSliceHists[0]->Fill(trackDeDxE1);
      }
      else if(trackNomE1 >= 6 && trackNomE1 <= 10)
        trackDeDxE1NoMSliceHists[1]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 11 && trackNomE1 <= 15)
        trackDeDxE1NoMSliceHists[2]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 16 && trackNomE1 <= 20)
        trackDeDxE1NoMSliceHists[3]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 21)
        trackDeDxE1NoMSliceHists[4]->Fill(trackDeDxE1);

      if(trackNomE1==4)
        trackDeDxE1NoM4Hist->Fill(trackDeDxE1);
      else if(trackNomE1==1)
        trackDeDxE1NoM1Hist->Fill(trackDeDxE1);
      else if(trackNomE1==6)
        trackDeDxE1NoM6Hist->Fill(trackDeDxE1);
      else if(trackNomE1==10)
        trackDeDxE1NoM10Hist->Fill(trackDeDxE1);
      else if(trackNomE1==15)
        trackDeDxE1NoM15Hist->Fill(trackDeDxE1);

      if(speak)
      {
        if(Track_dEdxE1[i] > 8)
        {
          std::cout << "[INFO] High dE/dx E1: " << trackDeDxE1 <<  " disc: " << Track_dEdxD3[i]
            << " Number of measurements: "
            << Track_dEdxE1_NOM[i]
            << " Number of saturating hits: "
            << Track_dEdxE1_NOS[i]
            << " Track eta: " 
            << trackEta
            << " Track phi: "
            << trackPhi
            << " Track Pt: "
            << trackPt
            << " Track P: "
            << trackP
            << std::endl;
        }
      }

      int numHits = Track_dEdxE1_numHits[i];
      if(numHits > 14)
      {
        if(speak)
        {
          std::cout << "[INFO] dE/dx E1: " << trackDeDxE1 <<  " Number of measurements: "
            << Track_dEdxE1_NOM[i]
            << " Number of saturating hits: "
            << Track_dEdxE1_NOS[i]
            << " Track eta: " 
            << trackEta
            << " Track phi: "
            << trackPhi
            << " Track Pt: "
            << trackPt
            << " Track P: "
            << trackP
            << std::endl;
        }
        std::vector<float> dEdxs;
        std::vector<float> yVals;
        for(int j=0; j<numHits; ++j)
        {
          float dEdx = Track_dEdxE1_hitCharges[i][j];
          dEdxs.push_back(dEdx);
          yVals.push_back(1);
          if(trackDeDxE1 > 9)
            trackDeDxAllHitsBigDeDxHist->Fill(dEdx);
          if(speak)
            std::cout << "\tHit: " << dEdx << " MeV/cm" << std::endl;
        }
        std::string name = "dEdxE1HitPatternGraph_dEdx";
        float nearest = floorf(trackDeDxE1*100+0.5)/100.;
        name+=floatToString(nearest);
        name+="_numHits";
        name+=intToString(numHits);
        TGraph graph(numHits,&(*dEdxs.begin()),&(*yVals.begin()));
        graph.SetName(name.c_str());
        graph.SetTitle(name.c_str());
        // only write the hit graphs for high dE/dx tracks
        if(trackDeDxE1 > 5)
          dedxHitGraphs.push_back(graph);

        float trunc = 0.2;
        std::vector<float> dedxVals;
        for(int j=0; j<numHits; ++j)
          dedxVals.push_back(Track_dEdxE1_hitCharges[i][j]);
        std::sort(dedxVals.begin(),dedxVals.end());
        int numHitsTrunc = numHits*(1-trunc);
        float avg = 0;
        for(int j=0; j<numHitsTrunc; ++j)
        {
          float dEdx = Track_dEdxE1_hitCharges[i][j];
          avg+=dEdx;
        }
        avg/=numHits;
        float rmsSqr = 0;
        for(int j=0; j<numHitsTrunc; ++j)
        {
          float dEdx = Track_dEdxE1_hitCharges[i][j];
          rmsSqr+=pow(dEdx-avg,2);
        }
        rmsSqr/=numHits;
        // dE/dx hit graph
        if(trackDeDxE1 > 3)
          trackDeDxRms3Hist->Fill(sqrt(rmsSqr));
        if(trackDeDxE1 > 4)
          trackDeDxRms4Hist->Fill(sqrt(rmsSqr));
        if(trackDeDxE1 > 5)
          trackDeDxRms5Hist->Fill(sqrt(rmsSqr));
      }
      if(numHits > 1)
      {
        for(int j=0; j<numHits; ++j)
        {
          float dEdx = Track_dEdxE1_hitCharges[i][j];
          trackDeDxAllHitsHist->Fill(dEdx);
          trackDeDxAllHitsVsNosHist->Fill(Track_dEdxE1_NOS[i],dEdx);
        }
      }
      if(numHits==1)
      {
        for(int j=0; j<numHits; ++j)
          trackDeDxAllHits1NoMHist->Fill(Track_dEdxE1_hitCharges[i][j]);
      }
      if(numHits==4)
      {
        for(int j=0; j<numHits; ++j)
          trackDeDxAllHits4NoMHist->Fill(Track_dEdxE1_hitCharges[i][j]);
      }
      if(numHits==6)
      {
        for(int j=0; j<numHits; ++j)
          trackDeDxAllHits6NoMHist->Fill(Track_dEdxE1_hitCharges[i][j]);
      }
      if(numHits==10)
      {
        for(int j=0; j<numHits; ++j)
          trackDeDxAllHits10NoMHist->Fill(Track_dEdxE1_hitCharges[i][j]);
      }
      if(numHits==15)
      {
        for(int j=0; j<numHits; ++j)
          trackDeDxAllHits15NoMHist->Fill(Track_dEdxE1_hitCharges[i][j]);
      }

      //debug
      if(speak)
      {
        std::cout << "[INFO] Track: Pt=" << Track_pt[i] << " eta=" << Track_eta[i] << " P="
          << Track_p[i] << std::endl;
      }

      // Slice in Pt bins -- ests and eta
      if(trackPt < 25)
      {
        trackDeDxE1PtSlice1Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice1Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice1Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice1Hist->Fill(trackEta);
      }
      else if(trackPt < 50)
      {
        trackDeDxE1PtSlice2Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice2Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice2Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice2Hist->Fill(trackEta);
      }
      else if(trackPt < 75)
      {
        trackDeDxE1PtSlice3Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice3Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice3Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice3Hist->Fill(trackEta);
      }
      else if(trackPt < 100)
      {
        trackDeDxE1PtSlice4Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice4Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice4Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice4Hist->Fill(trackEta);
      }
      else if(trackPt < 125)
      {
        trackDeDxE1PtSlice5Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice5Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice5Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice5Hist->Fill(trackEta);
      }
      else if(trackPt < 150)
      {
        trackDeDxE1PtSlice6Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice6Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice6Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice6Hist->Fill(trackEta);
      }
      else if(trackPt < 175)
      {
        trackDeDxE1PtSlice7Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice7Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice7Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice7Hist->Fill(trackEta);
      }
      else if(trackPt < 200)
      {
        trackDeDxE1PtSlice8Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice8Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice8Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice8Hist->Fill(trackEta);
      }
      else if(trackPt < 300)
      {
        trackDeDxE1PtSlice9Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice9Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice9Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice9Hist->Fill(trackEta);
      }
      else if(trackPt < 400)
      {
        trackDeDxE1PtSlice10Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice10Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice10Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice10Hist->Fill(trackEta);
      }
      else if(trackPt < 500)
      {
        trackDeDxE1PtSlice11Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice11Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice11Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice11Hist->Fill(trackEta);
      }
      else 
      {
        trackDeDxE1PtSlice12Hist->Fill(trackDeDxE1);
        trackDeDxE2PtSlice12Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PtSlice12Hist->Fill(Track_dEdxE3[i]);
        trackEtaPtSlice12Hist->Fill(trackEta);
      }

      if(trackPt < 100)
        trackDeDxD1PtSlice1Hist->Fill(Track_dEdxD1[i]);
      else if(trackPt < 200)
        trackDeDxD1PtSlice2Hist->Fill(Track_dEdxD1[i]);
      else if(trackPt < 300)
        trackDeDxD1PtSlice3Hist->Fill(Track_dEdxD1[i]);
      else if(trackPt < 400)
        trackDeDxD1PtSlice4Hist->Fill(Track_dEdxD1[i]);
      else if(trackPt < 500)
        trackDeDxD1PtSlice5Hist->Fill(Track_dEdxD1[i]);
      else
        trackDeDxD1PtSlice6Hist->Fill(Track_dEdxD1[i]);

      // Slice in P bins -- ests
      if(trackP < 25)
      {
        trackDeDxE1PSlice1Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice1Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice1Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 50)
      {
        trackDeDxE1PSlice2Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice2Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice2Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 75)
      {
        trackDeDxE1PSlice3Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice3Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice3Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 100)
      {
        trackDeDxE1PSlice4Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice4Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice4Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 125)
      {
        trackDeDxE1PSlice5Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice5Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice5Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 150)
      {
        trackDeDxE1PSlice6Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice6Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice6Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 175)
      {
        trackDeDxE1PSlice7Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice7Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice7Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 200)
      {
        trackDeDxE1PSlice8Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice8Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice8Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 300)
      {
        trackDeDxE1PSlice9Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice9Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice9Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 400)
      {
        trackDeDxE1PSlice10Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice10Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice10Hist->Fill(Track_dEdxE3[i]);
      }
      else if(trackP < 500)
      {
        trackDeDxE1PSlice11Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice11Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice11Hist->Fill(Track_dEdxE3[i]);
      }
      else 
      {
        trackDeDxE1PSlice12Hist->Fill(trackDeDxE1);
        trackDeDxE2PSlice12Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3PSlice12Hist->Fill(Track_dEdxE3[i]);
      }

      // Slices in Pt and NoM bins -- for PDF determination (5,5 currently)
      if(trackPt < 50)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBins_[0][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBins_[0][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBins_[0][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBins_[0][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBins_[0][4]->Fill(trackDeDxE1);
      }
      else if(trackPt < 150)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBins_[1][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBins_[1][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBins_[1][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBins_[1][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBins_[1][4]->Fill(trackDeDxE1);
      }
      else if(trackPt < 300)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBins_[2][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBins_[2][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBins_[2][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBins_[2][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBins_[2][4]->Fill(trackDeDxE1);
      }
      else if(trackPt < 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBins_[3][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBins_[3][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBins_[3][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBins_[3][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBins_[3][4]->Fill(trackDeDxE1);
      }
      else if(trackPt > 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBins_[4][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBins_[4][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBins_[4][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBins_[4][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBins_[4][4]->Fill(trackDeDxE1);
      }
      // Slices in P and NoM bins -- for PDF determination (5,5 currently)
      if(trackP < 50)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBins_[0][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBins_[0][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBins_[0][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBins_[0][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBins_[0][4]->Fill(trackDeDxE1);
      }
      else if(trackP < 150)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBins_[1][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBins_[1][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBins_[1][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBins_[1][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBins_[1][4]->Fill(trackDeDxE1);
      }
      else if(trackP < 300)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBins_[2][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBins_[2][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBins_[2][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBins_[2][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBins_[2][4]->Fill(trackDeDxE1);
      }
      else if(trackP < 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBins_[3][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBins_[3][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBins_[3][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBins_[3][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBins_[3][4]->Fill(trackDeDxE1);
      }
      else if(trackP > 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBins_[4][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBins_[4][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBins_[4][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBins_[4][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBins_[4][4]->Fill(trackDeDxE1);
      }

      // Look at path length -- inner strips only
      float pathLength = 0;
      for(int j=0; j<numHits; ++j)
      {
        float pl = Track_dEdxE1_hitPathLengths[i][j];
        // Look only at inner strips
        float hitDistance = Track_dEdxE1_hitDistances[i][j];
        if(hitDistance < 20 || hitDistance > 55)
          continue;
        pathLength+=pl;
      }
      //trackDeDxE1VsPathLengthHist->Fill(pathLength,trackDeDxE1);
      //trackDeDxPathLengthVsEtaHist->Fill(pathLength,trackEta);

      int myFinePBin = findFinePBin(trackP);
      int myLessFinePBin = findLessFinePBin(trackP);
      int myFineNoMBin = findFineNoMBin(trackNomE1);
      int myFineEtaBin = findFineEtaBin(trackEta);
      //float pullDeDx = (trackDeDxE1-meanIhPDep->Eval(trackP))/sigmaIhPDep->Eval(trackP);
      float dEdx = trackDeDxE1;
      float noPBiasDeDx = dEdx-meanIhPDep->Eval(trackP);
      float noPEtaBiasDeDx = noPBiasDeDx-meanIhEtaDep->Eval(fabs(trackEta));
      float noPEtaNoMBiasDeDx = noPEtaBiasDeDx-meanIhNoMDep->Eval(trackNomE1);
      // fill finePbins plot
      if(trackNomE1 >=1 && trackNomE1 <= 5)
        histsInNoMFinePBins_[myFinePBin][0]->Fill(noPEtaNoMBiasDeDx);
      else if(trackNomE1 >= 6 && trackNomE1 <= 10)
        histsInNoMFinePBins_[myFinePBin][1]->Fill(noPEtaNoMBiasDeDx);
      else if(trackNomE1 >= 11 && trackNomE1 <= 15)
        histsInNoMFinePBins_[myFinePBin][2]->Fill(noPEtaNoMBiasDeDx);
      else if(trackNomE1 >= 16 && trackNomE1 <= 20)
        histsInNoMFinePBins_[myFinePBin][3]->Fill(noPEtaNoMBiasDeDx);
      else if(trackNomE1 >= 21)
        histsInNoMFinePBins_[myFinePBin][4]->Fill(noPEtaNoMBiasDeDx);
      // fill fine p bins for closure
      dedxClosureInFinePBins_[myFinePBin]->Fill(noPEtaNoMBiasDeDx);
      //dedxClosureInFinePBins_[myFinePBin]->Fill(dEdx);
      //dedxClosureInFinePBins_[myFinePBin]->Fill(dEdx-meanIhEtaDep->Eval(fabs(trackEta))-meanIhNoMDep->Eval(trackNomE1));
      // fill fineNoMbins plot
      histsInFineNoMPBins_[myLessFinePBin][myFineNoMBin]->Fill(noPBiasDeDx-meanIhEtaDep->Eval(fabs(trackEta)));
      histsInFineNoMPBinsEtaDist_[myLessFinePBin][myFineNoMBin]->Fill(trackEta);
      // fill fineNoMbins pulls plot
      //TODO

      // fill fine NoM bins for closure
      dedxClosureInFineNoMBins_[myFineNoMBin]->Fill(noPEtaNoMBiasDeDx);
      //for(int j=0; j<numHits; ++j)
      //{
      //  float dEdx = Track_dEdxE1_hitCharges[i][j];
      //  // Look only at inner strips
      //  float hitDistance = Track_dEdxE1_hitDistances[i][j];
      //  if(hitDistance < 20 || hitDistance > 55)
      //    continue;
      //}

      histsInFineEtaPBins_[myLessFinePBin][myFineEtaBin]->Fill(noPBiasDeDx);
      //fill fine eta bins pulls
      pullsInFineEtaPBins_[myLessFinePBin][myFineEtaBin]->Fill(noPEtaNoMBiasDeDx/sigmaIhPDep->Eval(trackP));
      // fill fine eta bins for closure
      //dedxClosureInFineEtaBins_[myFineEtaBin]->Fill(dEdx-meanIhPDep->Eval(trackP)-meanIhEtaDep->Eval(fabs(trackEta))-meanIhNoMDep->Eval(trackNomE1));
      dedxClosureInFineEtaBins_[myFineEtaBin]->Fill(noPEtaNoMBiasDeDx/(sigmaIhPDep->Eval(trackP)*sqrt(sigmaIhEtaDep->Eval(fabs(trackEta)))));

      trackEtaVsDeDxE1Hist->Fill(trackDeDxE1,trackEta);
      if(Track_dEdxE1_numHits[i] >= 10 && Track_dEdxE1_numHits[i] <= 12)
        trackEtaVsDeDxE1NoMRestrHist->Fill(trackDeDxE1,trackEta);

      trackDeDxE1ClosureHist->Fill(noPBiasDeDx-meanIhEtaDep->Eval(fabs(trackEta))-meanIhNoMDep->Eval(trackNomE1));

      if(Hscp_hasCalo[i])
      {
        float ecalDeDx = 1000*Calo_ecal_dEdx[i];
        ecalDeDxTotalHist->Fill(ecalDeDx);
        trackDeDxE1vsEcalDeDxHist->Fill(ecalDeDx,Track_dEdxE1[i]);
        trackDeDxE2vsEcalDeDxHist->Fill(ecalDeDx,Track_dEdxE2[i]);
        trackDeDxE3vsEcalDeDxHist->Fill(ecalDeDx,Track_dEdxE3[i]);
        ecalTimingHist->Fill(Calo_ecal_time[i]);
        // Loop over hits
        for(int j=0; j < Calo_ecal_numCrysCrossed[i]; ++j)
        {
          float ecalEnergy = Calo_ecal_energies[i][j];
          float ecalTime   = Calo_ecal_times[i][j];
          bool isEndcap = (EcalSubdetector(DetId(Calo_ecal_detIds[i][j]).subdetId())==EcalEndcap) ? true : false;
          //debug
          //if(isEndcap)
          //  std::cout << "IS ECAL ENDCAP!" << " energy: " << ecalEnergy << " time: " << ecalTime << std::endl;
          //else if(EcalSubdetector(DetId(Calo_ecal_detIds[i][j]).subdetId())==EcalBarrel)
          //  std::cout << "IS ECAL BARREL!" << " energy: " << ecalEnergy << " time: " << ecalTime << std::endl;
          if(ecalEnergy < 0.2)
          {
            ecalHitTimingEslice1Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice1noEBHist->Fill(ecalTime);
          }
          else if(ecalEnergy < 0.4)
          {
            ecalHitTimingEslice2Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice2noEBHist->Fill(ecalTime);
          }
          else if(ecalEnergy < 0.6)
          {
            ecalHitTimingEslice3Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice3noEBHist->Fill(ecalTime);
          }
          else if(ecalEnergy < 0.8)
          {
            ecalHitTimingEslice4Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice4noEBHist->Fill(ecalTime);
          }
          else if(ecalEnergy < 1)
          {
            ecalHitTimingEslice5Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice5noEBHist->Fill(ecalTime);
          }
          else if(ecalEnergy < 1.5)
          {
            ecalHitTimingEslice6Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice6noEBHist->Fill(ecalTime);
          }
          else if(ecalEnergy < 2.0)
          {
            ecalHitTimingEslice7Hist->Fill(ecalTime);
            if(isEndcap)
              ecalHitTimingEslice7noEBHist->Fill(ecalTime);
          }
        }
        // eta slices
        if(fabs(trackEta) < 0.435)
          ecalDeDxMod1Hist->Fill(ecalDeDx);
        else if(fabs(trackEta) < 0.783)
          ecalDeDxMod2Hist->Fill(ecalDeDx);
        else if(fabs(trackEta) < 1.131)
          ecalDeDxMod3Hist->Fill(ecalDeDx);
        else if(fabs(trackEta) < 1.479)
          ecalDeDxMod4Hist->Fill(ecalDeDx);
        else
          ecalDeDxEEHist->Fill(ecalDeDx);

        if(Track_pt[i] > 30)
          ecalDeDxHighPtHist->Fill(ecalDeDx);
        else if(Track_pt[i] < 30)
          ecalDeDxLowPtHist->Fill(ecalDeDx);

        // TK dE/dx slices and combined with Pt
        if(Track_dEdxE3[i] > 3)
        {
          ecalDeDxHighTKDeDxHist->Fill(ecalDeDx);
          if(Track_pt[i] > 30)
            ecalDeDxHighTKDeDxHighPtHist->Fill(ecalDeDx);
          else if(Track_pt[i] < 30)
            ecalDeDxHighTKDeDxLowPtHist->Fill(ecalDeDx);
        }
        else if(Track_dEdxE3[i] < 3)
        {
          ecalDeDxLowTKDeDxHist->Fill(ecalDeDx);
          if(Track_pt[i] > 30)
            ecalDeDxLowTKDeDxHighPtHist->Fill(ecalDeDx);
          else if(Track_pt[i] < 30)
            ecalDeDxLowTKDeDxLowPtHist->Fill(ecalDeDx);
        }

        // SM, module boundaries
        EcalSubdetector subdet = EcalSubdetector(DetId(Calo_ecal_detIds[i][0]).subdetId());
        if(subdet==EcalBarrel)
        {
          EBDetId ebDet = EBDetId(Calo_ecal_detIds[i][0]);
          int ieta = ebDet.ietaSM(); // same as abs ieta
          int iphi = ebDet.iphiSM(); // 1-20
          if(iphi < 6 || iphi > 15) // do supermodule boundary plots
          {
            ecalDeDxSMBoundHist->Fill(ecalDeDx);
          }
          else if(ieta > 20 && ieta <= 30) // module 1-2 boundary
          {
            ecalDeDxMod12BoundHist->Fill(ecalDeDx);
          }
          else if(ieta > 40 && ieta <= 50) // module 2-3 boundary
          {
            ecalDeDxMod23BoundHist->Fill(ecalDeDx);
          }
          else if(ieta > 60 && ieta <= 70) // module 3-4 boundary
          {
            ecalDeDxMod34BoundHist->Fill(ecalDeDx);
          }
        }
      } // end calo part
      

      //if(speak)
      //{
      //  std::cout << "HSCP Tree Content: " << std::endl;
      //}

      if(NGens <= 0)
        continue; // if no gen particle, skip to next track (or we have real data)

      // Look at gen particles and match to track
      float bestDR = 1000;
      float bestBeta = -1;
      float bestPt = -1;
      for(int j=0; j< NGens; ++j)
      {
        int genPDGId = Gen_pdgId[j];
        if(!isGluino500(genPDGId)) // is gen particle an HSCP? -- IGNORE DOUBLY-CHARGED FOR NOW
          continue;
        if(Gen_p[i] < 60) // ignore the stuff at low P
          continue;

        //debug/check
        //if(isGluino500Baryon(genPDGId)) // is gen particle an HSCP?
        //  std::cout << "[INFO] R-baryon found in event!" << std::endl;

        float genEta = Gen_eta[j];
        float genPhi = Gen_phi[j];
        float beta = Gen_beta[j];
        float genPt = Gen_pt[j];
        float genP = Gen_p[i];
        // Fill beta hist whether or not we have a matching tracks (only first time through gen loop)
        if(firstGenLoop)
        {
          //debug
          //std::cout << "Gen particle PDG ID: " << genPDGId << " Pt: " << genPt
          //  << " p: " << Gen_p[j] << " eta: " << genEta << " phi:" << genPhi
          //  << " beta: " << beta << " mass: " << Gen_mass[j]
          //  << std::endl;
          genBetaHist->Fill(beta);
          genEtaHist->Fill(genEta);
          genPtHist->Fill(genPt);
          genPHist->Fill(genP);
          genEtaVsPHist->Fill(genP,genEta);
        }
        // Find matching track
        double dR = deltaR(trackEta,trackPhi,genEta,genPhi);
        if(dR < bestDR)
        {
          bestDR = dR;
          bestBeta = beta;
          bestPt = genPt;
        }
        deltaRHist->Fill(dR);
        if(fabs(trackEta) < 0.2)
          deltaREtaCentralHist->Fill(dR);
        if(fabs(trackEta) > 2.0 && fabs(trackEta) < 2.2)
          deltaREtaEndcapHist->Fill(dR);
      }
      firstGenLoop = false;

      //SIC DEBUG
      ////if(bestDR >= 0.01 && trackP > 25 && fabs(trackEta) > 1.5)
      //if(bestDR >= 0.01)
      //{
      //  std::cout << "TRACK: P (" << trackP << ") TRACK COULD NOT BE MATCHED TO ANY HSCP; best DR=" << bestDR  <<" eta=" << trackEta << " phi=" << trackPhi << std::endl;
      //}
      //else
      //{
      //  std::cout << "TRACK: P (" << trackP << ") TRACK WAS MATCHED TO AN HSCP; best DR=" << bestDR  <<" eta=" << trackEta << " phi=" << trackPhi << " genPt: " << bestPt << std::endl;
      //}


      // if no match found or bad match, skip this track
      if(bestDR > 0.01)
        continue;

      numMatchedTracksInEvent++;

      trackDiscriminator1VsGenBetaHist->Fill(bestBeta,Track_dEdxD1[i]);
      trackDiscriminator2VsGenBetaHist->Fill(bestBeta,Track_dEdxD2[i]);
      trackDiscriminator3VsGenBetaHist->Fill(bestBeta,Track_dEdxD3[i]);
      trackEstimator1VsGenBetaHist->Fill(bestBeta,Track_dEdxE1[i]);
      trackEstimator2VsGenBetaHist->Fill(bestBeta,Track_dEdxE2[i]);
      trackEstimator3VsGenBetaHist->Fill(bestBeta,Track_dEdxE3[i]);
      trackNumberOfMeasurementsMatchedHist->Fill(Track_dEdxE1_NOM[i]);
      trackNumberOfSatMeasurementsMatchedHist->Fill(Track_dEdxE1_NOS[i]);
      trackNumberOfMeasurementsVsEtaMatchedHist->Fill(trackEta,Track_dEdxE1_NOM[i]);
      trackNumberOfSatMeasurementsVsEtaMatchedHist->Fill(trackEta,Track_dEdxE1_NOS[i]);
      trackNumberOfMeasurementsVsPMatchedHist->Fill(Track_p[i],Track_dEdxE1_NOM[i]);
      trackNumberOfSatMeasurementsVsPMatchedHist->Fill(Track_p[i],Track_dEdxE1_NOS[i]);
      trackEtaMatchedHist->Fill(trackEta);
      trackEtaVsPtMatchedHist->Fill(trackPt,trackEta);
      trackEtaVsPMatchedHist->Fill(Track_p[i],trackEta);
      trackPtMatchedHist->Fill(trackPt);
      trackPvsDeDxE1MatchedHist->Fill(Track_dEdxE1[i],Track_p[i]);
      trackPvsDeDxD3MatchedHist->Fill(Track_dEdxD3[i],Track_p[i]);
      trackPtvsDeDxE1MatchedHist->Fill(Track_dEdxE1[i],Track_p[i]);
      trackPtvsDeDxD3MatchedHist->Fill(Track_dEdxD3[i],Track_p[i]);
      trackDeDxE1VsDeDxD3MatchedHist->Fill(Track_dEdxD3[i],Track_dEdxE1[i]);
      // track eta vs. pt plots in Muon timing sideband
      if(Hscp_hasMuon[i] && 1/Muon_cb_IBeta[i] > 1.25 && Muon_cb_IBeta_err[i] < 0.2)
      {
        trackEtaVsPtMuTimingSBMatchedHist->Fill(Track_pt[i],Track_eta[i]);
        trackEtaVsPMuTimingSBMatchedHist->Fill(Track_p[i],Track_eta[i]);
        // Slices in P and NoM bins (matched) -- for PDF determination (5,5 currently)
        if(trackP < 50)
        {
          if(trackNomE1 >=1 && trackNomE1 <= 5)
            histsForPDFNoMPBinsMuSBMatched_[0][0]->Fill(trackDeDxE1); 
          else if(trackNomE1 >= 6 && trackNomE1 <= 10)
            histsForPDFNoMPBinsMuSBMatched_[0][1]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 11 && trackNomE1 <= 15)
            histsForPDFNoMPBinsMuSBMatched_[0][2]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 16 && trackNomE1 <= 20)
            histsForPDFNoMPBinsMuSBMatched_[0][3]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 21)
            histsForPDFNoMPBinsMuSBMatched_[0][4]->Fill(trackDeDxE1);
        }
        else if(trackP < 150)
        {
          if(trackNomE1 >=1 && trackNomE1 <= 5)
            histsForPDFNoMPBinsMuSBMatched_[1][0]->Fill(trackDeDxE1); 
          else if(trackNomE1 >= 6 && trackNomE1 <= 10)
            histsForPDFNoMPBinsMuSBMatched_[1][1]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 11 && trackNomE1 <= 15)
            histsForPDFNoMPBinsMuSBMatched_[1][2]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 16 && trackNomE1 <= 20)
            histsForPDFNoMPBinsMuSBMatched_[1][3]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 21)
            histsForPDFNoMPBinsMuSBMatched_[1][4]->Fill(trackDeDxE1);
        }
        else if(trackP < 300)
        {
          if(trackNomE1 >=1 && trackNomE1 <= 5)
            histsForPDFNoMPBinsMuSBMatched_[2][0]->Fill(trackDeDxE1); 
          else if(trackNomE1 >= 6 && trackNomE1 <= 10)
            histsForPDFNoMPBinsMuSBMatched_[2][1]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 11 && trackNomE1 <= 15)
            histsForPDFNoMPBinsMuSBMatched_[2][2]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 16 && trackNomE1 <= 20)
            histsForPDFNoMPBinsMuSBMatched_[2][3]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 21)
            histsForPDFNoMPBinsMuSBMatched_[2][4]->Fill(trackDeDxE1);
        }
        else if(trackP < 450)
        {
          if(trackNomE1 >=1 && trackNomE1 <= 5)
            histsForPDFNoMPBinsMuSBMatched_[3][0]->Fill(trackDeDxE1); 
          else if(trackNomE1 >= 6 && trackNomE1 <= 10)
            histsForPDFNoMPBinsMuSBMatched_[3][1]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 11 && trackNomE1 <= 15)
            histsForPDFNoMPBinsMuSBMatched_[3][2]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 16 && trackNomE1 <= 20)
            histsForPDFNoMPBinsMuSBMatched_[3][3]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 21)
            histsForPDFNoMPBinsMuSBMatched_[3][4]->Fill(trackDeDxE1);
        }
        else if(trackP > 450)
        {
          if(trackNomE1 >=1 && trackNomE1 <= 5)
            histsForPDFNoMPBinsMuSBMatched_[4][0]->Fill(trackDeDxE1); 
          else if(trackNomE1 >= 6 && trackNomE1 <= 10)
            histsForPDFNoMPBinsMuSBMatched_[4][1]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 11 && trackNomE1 <= 15)
            histsForPDFNoMPBinsMuSBMatched_[4][2]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 16 && trackNomE1 <= 20)
            histsForPDFNoMPBinsMuSBMatched_[4][3]->Fill(trackDeDxE1);
          else if(trackNomE1 >= 21)
            histsForPDFNoMPBinsMuSBMatched_[4][4]->Fill(trackDeDxE1);
        }
      }
      // slice in beta bins -- E1 only
      if(bestBeta < 0.05)
        trackDeDxE1BetaSliceHists[0]->Fill(trackDeDxE1);
      else if(bestBeta < 0.1)
        trackDeDxE1BetaSliceHists[1]->Fill(trackDeDxE1);
      else if(bestBeta < 0.15)
        trackDeDxE1BetaSliceHists[2]->Fill(trackDeDxE1);
      else if(bestBeta < 0.2)
        trackDeDxE1BetaSliceHists[3]->Fill(trackDeDxE1);
      else if(bestBeta < 0.25)
        trackDeDxE1BetaSliceHists[4]->Fill(trackDeDxE1);
      else if(bestBeta < 0.3)
        trackDeDxE1BetaSliceHists[5]->Fill(trackDeDxE1);
      else if(bestBeta < 0.35)
        trackDeDxE1BetaSliceHists[6]->Fill(trackDeDxE1);
      else if(bestBeta < 0.4)
        trackDeDxE1BetaSliceHists[7]->Fill(trackDeDxE1);
      else if(bestBeta < 0.45)
        trackDeDxE1BetaSliceHists[8]->Fill(trackDeDxE1);
      else if(bestBeta < 0.5)
        trackDeDxE1BetaSliceHists[9]->Fill(trackDeDxE1);
      else if(bestBeta < 0.55)
        trackDeDxE1BetaSliceHists[10]->Fill(trackDeDxE1);
      else if(bestBeta < 0.6)
        trackDeDxE1BetaSliceHists[11]->Fill(trackDeDxE1);
      else if(bestBeta < 0.65)
        trackDeDxE1BetaSliceHists[12]->Fill(trackDeDxE1);
      else if(bestBeta < 0.7)
        trackDeDxE1BetaSliceHists[13]->Fill(trackDeDxE1);
      else if(bestBeta < 0.75)
        trackDeDxE1BetaSliceHists[14]->Fill(trackDeDxE1);
      else if(bestBeta < 0.8)
        trackDeDxE1BetaSliceHists[15]->Fill(trackDeDxE1);
      else if(bestBeta < 0.85)
        trackDeDxE1BetaSliceHists[16]->Fill(trackDeDxE1);
      else if(bestBeta < 0.9)
        trackDeDxE1BetaSliceHists[17]->Fill(trackDeDxE1);
      else if(bestBeta < 0.95)
        trackDeDxE1BetaSliceHists[18]->Fill(trackDeDxE1);
      else
        trackDeDxE1BetaSliceHists[19]->Fill(trackDeDxE1);

      // slice in beta bins -- rest
      if(bestBeta < 0.2)
      {
        trackDeDxD1BetaSlice1Hist->Fill(Track_dEdxD1[i]);
        trackDeDxD2BetaSlice1Hist->Fill(Track_dEdxD2[i]);
        trackDeDxD3BetaSlice1Hist->Fill(Track_dEdxD3[i]);
        trackDeDxE2BetaSlice1Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3BetaSlice1Hist->Fill(Track_dEdxE3[i]);
      }
      else if(bestBeta < 0.4)
      {
        trackDeDxD1BetaSlice2Hist->Fill(Track_dEdxD1[i]);
        trackDeDxD2BetaSlice2Hist->Fill(Track_dEdxD2[i]);
        trackDeDxD3BetaSlice2Hist->Fill(Track_dEdxD3[i]);
        trackDeDxE2BetaSlice2Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3BetaSlice2Hist->Fill(Track_dEdxE3[i]);
      }
      else if(bestBeta < 0.6)
      {
        trackDeDxD1BetaSlice3Hist->Fill(Track_dEdxD1[i]);
        trackDeDxD2BetaSlice3Hist->Fill(Track_dEdxD2[i]);
        trackDeDxD3BetaSlice3Hist->Fill(Track_dEdxD3[i]);
        trackDeDxE2BetaSlice3Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3BetaSlice3Hist->Fill(Track_dEdxE3[i]);
      }
      else if(bestBeta < 0.8)
      {
        trackDeDxD1BetaSlice4Hist->Fill(Track_dEdxD1[i]);
        trackDeDxD2BetaSlice4Hist->Fill(Track_dEdxD2[i]);
        trackDeDxD3BetaSlice4Hist->Fill(Track_dEdxD3[i]);
        trackDeDxE2BetaSlice4Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3BetaSlice4Hist->Fill(Track_dEdxE3[i]);
      }
      else if(bestBeta <= 1)
      {
        trackDeDxD1BetaSlice5Hist->Fill(Track_dEdxD1[i]);
        trackDeDxD2BetaSlice5Hist->Fill(Track_dEdxD2[i]);
        trackDeDxD3BetaSlice5Hist->Fill(Track_dEdxD3[i]);
        trackDeDxE2BetaSlice5Hist->Fill(Track_dEdxE2[i]);
        trackDeDxE3BetaSlice5Hist->Fill(Track_dEdxE3[i]);
      }

      // do Pt slices
      if(trackPt < 25)
      {
        trackDeDxMatchedE1PtSlice1Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 50)
      {
        trackDeDxMatchedE1PtSlice2Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 75)
      {
        trackDeDxMatchedE1PtSlice3Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 100)
      {
        trackDeDxMatchedE1PtSlice4Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 125)
      {
        trackDeDxMatchedE1PtSlice5Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 150)
      {
        trackDeDxMatchedE1PtSlice6Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 175)
      {
        trackDeDxMatchedE1PtSlice7Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 200)
      {
        trackDeDxMatchedE1PtSlice8Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 300)
      {
        trackDeDxMatchedE1PtSlice9Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 400)
      {
        trackDeDxMatchedE1PtSlice10Hist->Fill(trackDeDxE1);
      }
      else if(trackPt < 500)
      {
        trackDeDxMatchedE1PtSlice11Hist->Fill(trackDeDxE1);
      }
      else 
      {
        trackDeDxMatchedE1PtSlice12Hist->Fill(trackDeDxE1);
      }

      // Slice in P bins -- ests
      if(trackP < 25)
      {
        trackDeDxMatchedE1PSlice1Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 50)
      {
        trackDeDxMatchedE1PSlice2Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 75)
      {
        trackDeDxMatchedE1PSlice3Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 100)
      {
        trackDeDxMatchedE1PSlice4Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 125)
      {
        trackDeDxMatchedE1PSlice5Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 150)
      {
        trackDeDxMatchedE1PSlice6Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 175)
      {
        trackDeDxMatchedE1PSlice7Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 200)
      {
        trackDeDxMatchedE1PSlice8Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 300)
      {
        trackDeDxMatchedE1PSlice9Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 400)
      {
        trackDeDxMatchedE1PSlice10Hist->Fill(trackDeDxE1);
      }
      else if(trackP < 500)
      {
        trackDeDxMatchedE1PSlice11Hist->Fill(trackDeDxE1);
      }
      else 
      {
        trackDeDxMatchedE1PSlice12Hist->Fill(trackDeDxE1);
      }

      // TK dE/dx -- NoM slices (matched)
      if(trackNomE1 >= 1 && trackNomE1 <= 5)
        trackDeDxE1MatchedNoMSliceHists[0]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 6 && trackNomE1 <= 10)
        trackDeDxE1MatchedNoMSliceHists[1]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 11 && trackNomE1 <= 15)
        trackDeDxE1MatchedNoMSliceHists[2]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 16 && trackNomE1 <= 20)
        trackDeDxE1MatchedNoMSliceHists[3]->Fill(trackDeDxE1);
      else if(trackNomE1 >= 21)
        trackDeDxE1MatchedNoMSliceHists[4]->Fill(trackDeDxE1);

      //XXX
      // Slices in Pt and NoM bins (matched) -- for PDF determination (5,5 currently)
      if(trackPt < 50)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBinsMatched_[0][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBinsMatched_[0][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBinsMatched_[0][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBinsMatched_[0][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBinsMatched_[0][4]->Fill(trackDeDxE1);
      }
      else if(trackPt < 150)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBinsMatched_[1][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBinsMatched_[1][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBinsMatched_[1][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBinsMatched_[1][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBinsMatched_[1][4]->Fill(trackDeDxE1);
      }
      else if(trackPt < 300)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBinsMatched_[2][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBinsMatched_[2][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBinsMatched_[2][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBinsMatched_[2][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBinsMatched_[2][4]->Fill(trackDeDxE1);
      }
      else if(trackPt < 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBinsMatched_[3][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBinsMatched_[3][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBinsMatched_[3][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBinsMatched_[3][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBinsMatched_[3][4]->Fill(trackDeDxE1);
      }
      else if(trackPt > 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPtBinsMatched_[4][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPtBinsMatched_[4][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPtBinsMatched_[4][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPtBinsMatched_[4][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPtBinsMatched_[4][4]->Fill(trackDeDxE1);
      }
      // Slices in P and NoM bins (matched) -- for PDF determination (5,5 currently)
      if(trackP < 50)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBinsMatched_[0][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBinsMatched_[0][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBinsMatched_[0][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBinsMatched_[0][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBinsMatched_[0][4]->Fill(trackDeDxE1);
      }
      else if(trackP < 150)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBinsMatched_[1][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBinsMatched_[1][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBinsMatched_[1][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBinsMatched_[1][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBinsMatched_[1][4]->Fill(trackDeDxE1);
      }
      else if(trackP < 300)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBinsMatched_[2][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBinsMatched_[2][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBinsMatched_[2][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBinsMatched_[2][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBinsMatched_[2][4]->Fill(trackDeDxE1);
      }
      else if(trackP < 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBinsMatched_[3][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBinsMatched_[3][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBinsMatched_[3][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBinsMatched_[3][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBinsMatched_[3][4]->Fill(trackDeDxE1);
      }
      else if(trackP > 450)
      {
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsForPDFNoMPBinsMatched_[4][0]->Fill(trackDeDxE1); 
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsForPDFNoMPBinsMatched_[4][1]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsForPDFNoMPBinsMatched_[4][2]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsForPDFNoMPBinsMatched_[4][3]->Fill(trackDeDxE1);
        else if(trackNomE1 >= 21)
          histsForPDFNoMPBinsMatched_[4][4]->Fill(trackDeDxE1);
      }

      //trackDeDxE1VsPathLengthMatchedHist->Fill(pathLength,trackDeDxE1);
      //trackDeDxPathLengthVsEtaMatchedHist->Fill(pathLength,trackEta);

        // fill finePbins plot -- bin taken from before
        if(trackNomE1 >=1 && trackNomE1 <= 5)
          histsInNoMFinePBinsMatched_[myFinePBin][0]->Fill(noPEtaNoMBiasDeDx);
        else if(trackNomE1 >= 6 && trackNomE1 <= 10)
          histsInNoMFinePBinsMatched_[myFinePBin][1]->Fill(noPEtaNoMBiasDeDx);
        else if(trackNomE1 >= 11 && trackNomE1 <= 15)
          histsInNoMFinePBinsMatched_[myFinePBin][2]->Fill(noPEtaNoMBiasDeDx);
        else if(trackNomE1 >= 16 && trackNomE1 <= 20)
          histsInNoMFinePBinsMatched_[myFinePBin][3]->Fill(noPEtaNoMBiasDeDx);
        else if(trackNomE1 >= 21)
          histsInNoMFinePBinsMatched_[myFinePBin][4]->Fill(noPEtaNoMBiasDeDx);
        // fill dedx closure in fine p bins
        dedxClosureInFinePBinsMatched_[myFinePBin]->Fill(noPEtaNoMBiasDeDx);
        //dedxClosureInFinePBinsMatched_[myFinePBin]->Fill(dEdx);
        //dedxClosureInFinePBinsMatched_[myFinePBin]->Fill(dEdx-meanIhEtaDep->Eval(fabs(trackEta))-meanIhNoMDep->Eval(trackNomE1));

        // fill fineNoMbins plot -- bin taken from before
        histsInFineNoMPBinsMatched_[myLessFinePBin][myFineNoMBin]->Fill(noPBiasDeDx-meanIhEtaDep->Eval(fabs(trackEta)));
        histsInFineNoMPBinsMatchedEtaDist_[myLessFinePBin][myFineNoMBin]->Fill(trackEta);
        // fill fine NoM bins pulls
        //TODO
        // fill fine NoM bins for closure
        dedxClosureInFineNoMBinsMatched_[myFineNoMBin]->Fill(noPEtaNoMBiasDeDx);
        //for(int j=0; j<numHits; ++j)
        //{
        //  float dEdx = Track_dEdxE1_hitCharges[i][j];
        //  // Look only at inner strips
        //  float hitDistance = Track_dEdxE1_hitDistances[i][j];
        //  if(hitDistance < 20 || hitDistance > 55)
        //    continue;

        //}

        histsInFineEtaPBinsMatched_[myLessFinePBin][myFineEtaBin]->Fill(noPBiasDeDx);
        // fill fine eta bins pulls
        pullsInFineEtaPBinsMatched_[myLessFinePBin][myFineEtaBin]->Fill(noPEtaNoMBiasDeDx/sigmaIhPDep->Eval(trackP));
        // fill fine eta bins for closure
        //dedxClosureInFineEtaBinsMatched_[myFineEtaBin]->Fill(dEdx-meanIhPDep->Eval(trackP)-meanIhEtaDep->Eval(fabs(trackEta))-meanIhNoMDep->Eval(trackNomE1));
        dedxClosureInFineEtaBinsMatched_[myFineEtaBin]->Fill(noPEtaNoMBiasDeDx/(sigmaIhPDep->Eval(trackP)*sqrt(sigmaIhEtaDep->Eval(fabs(trackEta)))));

        trackEtaVsDeDxE1MatchedHist->Fill(trackDeDxE1,trackEta);
        if(Track_dEdxE1_numHits[i] >= 10 && Track_dEdxE1_numHits[i] <= 12)
          trackEtaVsDeDxE1NoMRestrMatchedHist->Fill(trackDeDxE1,trackEta);

        trackDeDxE1ClosureMatchedHist->Fill(noPBiasDeDx-meanIhEtaDep->Eval(fabs(trackEta))-meanIhNoMDep->Eval(trackNomE1));

      int numHits2 = Track_dEdxE1_numHits[i];
      if(numHits2 > 14)
      {
        float trunc = 0.2;
        std::vector<float> dedxVals;
        for(int j=0; j<numHits; ++j)
          dedxVals.push_back(Track_dEdxE1_hitCharges[i][j]);
        std::sort(dedxVals.begin(),dedxVals.end());
        int numHitsTrunc = numHits*(1-trunc);
        float avg = 0;
        for(int j=0; j<numHitsTrunc; ++j)
        {
          float dEdx = Track_dEdxE1_hitCharges[i][j];
          avg+=dEdx;
        }
        avg/=numHits;
        float rmsSqr = 0;
        for(int j=0; j<numHitsTrunc; ++j)
        {
          float dEdx = Track_dEdxE1_hitCharges[i][j];
          rmsSqr+=pow(dEdx-avg,2);
        }
        rmsSqr/=numHits;
        // dE/dx hit graph
        if(trackDeDxE1 > 3)
          trackDeDxRms3MatchedHist->Fill(sqrt(rmsSqr));
        if(trackDeDxE1 > 4)
          trackDeDxRms4MatchedHist->Fill(sqrt(rmsSqr));
        if(trackDeDxE1 > 5)
          trackDeDxRms5MatchedHist->Fill(sqrt(rmsSqr));
      }

    } // loop over HSCPs

    if(numMatchedTracksInEvent==0)
      numEventsZeroMatchedTracks++;
    else if(numMatchedTracksInEvent==1)
      numEventsOneMatchedTracks++;
    else if(numMatchedTracksInEvent==2)
      numEventsTwoMatchedTracks++;

  } // loop over tree


  //debug
  std::cout << "[INFO} -------- Number of events read: " << numEvents_ << std::endl;
  std::cout << "[INFO} -------- Number of total reco tracks: " << numTotalTracks << std::endl;
  std::cout << "[INFO} -------- Number of events with 0 matched tracks: " << numEventsZeroMatchedTracks << std::endl;
  std::cout << "[INFO} -------- Number of events with 1 matched track: " << numEventsOneMatchedTracks << std::endl;
  std::cout << "[INFO} -------- Number of events with 2 matched tracks: " << numEventsTwoMatchedTracks << std::endl;

  // Make ecal dE/dx high/low hist
  outFile->cd();
  ecalDeDxHighTKDeDxHist->Write();
  ecalDeDxLowTKDeDxHist->Write();
  ecalDeDxHighTKOverLowTKHist = (TH1F*) ecalDeDxHighTKDeDxHist->Clone();
  ecalDeDxHighTKOverLowTKHist->SetTitle("Ecal dE/dx from high TK dE/dx divided by ecal dE/dx from low TK dE/dx");
  ecalDeDxHighTKOverLowTKHist->Sumw2();
  ecalDeDxLowTKDeDxHist->Scale(ecalDeDxHighTKDeDxHist->GetMaximum()/ecalDeDxLowTKDeDxHist->GetMaximum());
  ecalDeDxHighTKOverLowTKHist->Divide(ecalDeDxLowTKDeDxHist); // Errors calc'ed assuming uncorrelated hists
  ecalDeDxHighTKOverLowTKHist->Write();

  // TK dE/dx -- NoM slices
  TDirectory* trackDeDxNoMSlicesDir = outFile->mkdir("trackDeDxNoMSlices");
  trackDeDxNoMSlicesDir->cd();
  for(int i=0; i<5; ++i)
    trackDeDxE1NoMSliceHists[i]->Write();
  trackDeDxE1NoM15Hist->Write();
  trackDeDxE1NoM10Hist->Write();
  trackDeDxE1NoM6Hist->Write();
  trackDeDxE1NoM4Hist->Write();
  trackDeDxE1NoM1Hist->Write();
  // TK dE/dx -- NoM slices (matched)
  TDirectory* trackDeDxMatchedNoMSlicesDir = outFile->mkdir("trackDeDxMatchedNoMSlices");
  trackDeDxMatchedNoMSlicesDir->cd();
  for(int i=0; i<5; ++i)
    trackDeDxE1MatchedNoMSliceHists[i]->Write();
  // TK dE/dx -- bins of NoM and Pt
  TDirectory* trackDeDxNoMAndPtSlicesDir = outFile->mkdir("trackDeDxNoMAndPtSlices");
  trackDeDxNoMAndPtSlicesDir->cd();
  for(int i=0; i<numPtBins_; ++i)
  {
    for(int j=0; j<numNomBins_; ++j)
    {
      histsForPDFNoMPtBins_[i][j]->Write();
      histsForPDFNoMPtBinsMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- bins of NoM and P
  TDirectory* trackDeDxNoMAndPSlicesDir = outFile->mkdir("trackDeDxNoMAndPSlices");
  trackDeDxNoMAndPSlicesDir->cd();
  for(int i=0; i<numPBins_; ++i)
  {
    for(int j=0; j<numNomBins_; ++j)
    {
      histsForPDFNoMPBins_[i][j]->Write();
      histsForPDFNoMPBinsMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- bins of NoM and P -- mu timing SB
  TDirectory* trackDeDxNoMAndPSlicesMuTimingSBDir = outFile->mkdir("trackDeDxNoMAndPSlicesMuTimingSB");
  trackDeDxNoMAndPSlicesMuTimingSBDir->cd();
  for(int i=0; i<numPBins_; ++i)
  {
    for(int j=0; j<numNomBins_; ++j)
    {
      histsForPDFNoMPBinsMuSB_[i][j]->Write();
      histsForPDFNoMPBinsMuSBMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- hists in fine P bins
  TDirectory* trackDeDxNoMAndFinePSlicesDir = outFile->mkdir("trackDeDxNoMAndFinePSlices");
  trackDeDxNoMAndFinePSlicesDir->cd();
  for(int i=0; i<200; ++i)
  {
    for(int j=0; j<5; ++j)
    {
      histsInNoMFinePBins_[i][j]->Write();
      histsInNoMFinePBinsMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- hists for closure in fine P bins
  TDirectory* trackDeDxClosureFinePSlicesDir = outFile->mkdir("trackDeDxClosureFinePSlices");
  trackDeDxClosureFinePSlicesDir->cd();
  for(int i=0; i<200; ++i)
  {
    dedxClosureInFinePBins_[i]->Write();
    dedxClosureInFinePBinsMatched_[i]->Write();
  }
  // TK dE/dx -- hists in fine NoM bins
  TDirectory* trackDeDxFineNoMAndPSlicesDir = outFile->mkdir("trackDeDxFineNoMAndPSlices");
  trackDeDxFineNoMAndPSlicesDir->cd();
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<30; ++j)
    {
      histsInFineNoMPBins_[i][j]->Write();
      histsInFineNoMPBinsMatched_[i][j]->Write();
      histsInFineNoMPBinsEtaDist_[i][j]->Write();
      histsInFineNoMPBinsMatchedEtaDist_[i][j]->Write();
    }
  }
  // TK dE/dx -- hists in fine NoM bins pulls
  TDirectory* trackDeDxFineNoMAndPSlicesPullsDir = outFile->mkdir("trackDeDxFineNoMAndPSlicesPulls");
  trackDeDxFineNoMAndPSlicesPullsDir->cd();
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<30; ++j)
    {
      pullsInFineNoMPBins_[i][j]->Write();
      pullsInFineNoMPBinsMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- hists for closure in fine NoM bins
  TDirectory* trackDeDxClosureFineNoMSlicesDir = outFile->mkdir("trackDeDxClosureFineNoMSlices");
  trackDeDxClosureFineNoMSlicesDir->cd();
  for(int i=0; i<30; ++i)
  {
    dedxClosureInFineNoMBins_[i]->Write();
    dedxClosureInFineNoMBinsMatched_[i]->Write();
  }
  // TK dE/dx -- hists in fine eta bins
  TDirectory* trackDeDxFineEtaAndPSlicesDir = outFile->mkdir("trackDeDxFineEtaAndPSlices");
  trackDeDxFineEtaAndPSlicesDir->cd();
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<25; ++j)
    {
      histsInFineEtaPBins_[i][j]->Write();
      histsInFineEtaPBinsMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- hists in fine eta bins pulls
  TDirectory* trackDeDxFineEtaAndPSlicesPullsDir = outFile->mkdir("trackDeDxFineEtaAndPSlicesPulls");
  trackDeDxFineEtaAndPSlicesPullsDir->cd();
  for(int i=0; i<10; ++i)
  {
    for(int j=0; j<25; ++j)
    {
      pullsInFineEtaPBins_[i][j]->Write();
      pullsInFineEtaPBinsMatched_[i][j]->Write();
    }
  }
  // TK dE/dx -- hists for closure in fine eta bins
  TDirectory* trackDeDxClosureFineEtaSlicesDir = outFile->mkdir("trackDeDxClosureFineEtaSlices");
  trackDeDxClosureFineEtaSlicesDir->cd();
  for(int i=0; i<25; ++i)
  {
    dedxClosureInFineEtaBins_[i]->Write();
    dedxClosureInFineEtaBinsMatched_[i]->Write();
  }


  outFile->cd();
  // write rest of hists here
  trackEtaVsDeDxE1Hist->Write();
  trackEtaVsDeDxE1MatchedHist->Write();
  trackEtaVsDeDxE1NoMRestrHist->Write();
  trackEtaVsDeDxE1NoMRestrMatchedHist->Write();
  trackChi2Hist->Write();
  trackChi2NdfHist->Write();
  trackDeDxE1Hist->Write();
  trackDeDxE1ClosureHist->Write();
  trackDeDxE1ClosureMatchedHist->Write();
  trackDeDxE2Hist->Write();
  trackDeDxE3Hist->Write();
  trackDeDxD1Hist->Write();
  trackDeDxD2Hist->Write();
  trackDeDxD3Hist->Write();
  trackPtHist->Write();
  trackEtaHist->Write();
  trackNohHist->Write();
  trackEtaMatchedHist->Write();
  trackEtaVsPtMatchedHist->Write();
  trackEtaVsPMatchedHist->Write();
  trackPtMatchedHist->Write();
  trackEtaVsPtHist->Write();
  trackEtaVsPHist->Write();
  trackEtaVsPtMuTimingSBHist->Write();
  trackEtaVsPMuTimingSBHist->Write();
  trackEtaVsPtMuTimingSBMatchedHist->Write();
  trackEtaVsPMuTimingSBMatchedHist->Write();
  genBetaHist->Write();
  genEtaHist->Write();
  genPtHist->Write();
  genPHist->Write();
  genEtaVsPHist->Write();
  deltaRHist->Write();
  deltaREtaEndcapHist->Write();
  deltaREtaCentralHist->Write();

  ecalDeDxTotalHist->Write();
  ecalDeDxHighPtHist->Write();
  ecalDeDxLowPtHist->Write();
  ecalDeDxHighTKDeDxHighPtHist->Write();
  ecalDeDxLowTKDeDxHighPtHist->Write(); 
  ecalDeDxHighTKDeDxLowPtHist->Write(); 
  ecalDeDxLowTKDeDxLowPtHist->Write();

  ecalTimingHist->Write();

  trackDeDxE1vsEcalDeDxHist->Write();
  trackDeDxE2vsEcalDeDxHist->Write();
  trackDeDxE3vsEcalDeDxHist->Write();

  trackDiscriminator1VsGenBetaHist->Write();
  trackDiscriminator2VsGenBetaHist->Write();
  trackDiscriminator3VsGenBetaHist->Write();
  trackEstimator1VsGenBetaHist->Write();
  trackEstimator2VsGenBetaHist->Write();
  trackEstimator3VsGenBetaHist->Write();

  trackPvsDeDxE1Hist->Write();
  trackPvsDeDxD3Hist->Write();
  trackPtvsDeDxE1Hist->Write();
  trackPtvsDeDxD3Hist->Write();
  trackPvsDeDxE1MatchedHist->Write();
  trackPvsDeDxD3MatchedHist->Write();
  trackPtvsDeDxE1MatchedHist->Write();
  trackPtvsDeDxD3MatchedHist->Write();
  trackDeDxE1VsDeDxD3Hist->Write();
  trackDeDxE1VsDeDxD3MatchedHist->Write();

  // TK dE/dx -- eta slices
  TDirectory* trackDeDxEtaSlicesDir = outFile->mkdir("trackDeDxEtaSlices");
  trackDeDxEtaSlicesDir->cd();
  for(int i=0; i<10; ++i)
    trackDeDxE1EtaSliceHists[i]->Write();

  // TK eta distributions in Pt slices
  TDirectory* trackEtaPtSlicesDir = outFile->mkdir("trackEtaPtSlices");
  trackEtaPtSlicesDir->cd();
  trackEtaPtSlice1Hist->Write();
  trackEtaPtSlice2Hist->Write();
  trackEtaPtSlice3Hist->Write();
  trackEtaPtSlice4Hist->Write();
  trackEtaPtSlice5Hist->Write();
  trackEtaPtSlice6Hist->Write();
  trackEtaPtSlice7Hist->Write();
  trackEtaPtSlice8Hist->Write();
  trackEtaPtSlice9Hist->Write();
  trackEtaPtSlice10Hist->Write();
  trackEtaPtSlice11Hist->Write();
  trackEtaPtSlice12Hist->Write();

  // misc
  outFile->cd();
  trackNumberOfMeasurementsHist->Write();
  trackNumberOfSatMeasurementsHist->Write();
  trackNumberOfMeasurementsMatchedHist->Write();
  trackNumberOfSatMeasurementsMatchedHist->Write();
  trackNumberOfMeasurementsVsEtaHist->Write();
  trackNumberOfSatMeasurementsVsEtaHist->Write();
  trackNumberOfMeasurementsVsEtaMatchedHist->Write();
  trackNumberOfSatMeasurementsVsEtaMatchedHist->Write();
  trackNumberOfMeasurementsVsPHist->Write();
  trackNumberOfMeasurementsVsPMatchedHist->Write();
  trackNumberOfSatMeasurementsVsPHist->Write();
  trackNumberOfSatMeasurementsVsPMatchedHist->Write();

  // Ecal timing E slices
  TDirectory* ecalHitTimingESlicesDir = outFile->mkdir("EcalHitTimingESlices");
  ecalHitTimingESlicesDir->cd();
  ecalHitTimingEslice1Hist->Write();
  ecalHitTimingEslice2Hist->Write();
  ecalHitTimingEslice3Hist->Write();
  ecalHitTimingEslice4Hist->Write();
  ecalHitTimingEslice5Hist->Write();
  ecalHitTimingEslice6Hist->Write();
  ecalHitTimingEslice7Hist->Write();
  ecalHitTimingEslice1noEBHist->Write();
  ecalHitTimingEslice2noEBHist->Write();
  ecalHitTimingEslice3noEBHist->Write();
  ecalHitTimingEslice4noEBHist->Write();
  ecalHitTimingEslice5noEBHist->Write();
  ecalHitTimingEslice6noEBHist->Write();
  ecalHitTimingEslice7noEBHist->Write();

  // SM, mod boundaries
  TDirectory* ecalDeDxBoundaryDir = outFile->mkdir("EcalDeDxModSMBoundaries");
  ecalDeDxBoundaryDir->cd();
  ecalDeDxMod12BoundHist->Write();
  ecalDeDxMod23BoundHist->Write();
  ecalDeDxMod34BoundHist->Write();
  ecalDeDxSMBoundHist->Write();

  // eta slices
  TDirectory* ecalDeDxEtaSlicesDir = outFile->mkdir("EcalDeDxEtaSlices");
  ecalDeDxEtaSlicesDir->cd();
  ecalDeDxMod1Hist->Write();
  ecalDeDxMod2Hist->Write();
  ecalDeDxMod3Hist->Write();
  ecalDeDxMod4Hist->Write();
  ecalDeDxEEHist->Write();

  // beta slices
  std::vector<float> betas;
  std::vector<float> betaErrors;
  std::vector<float> fitMeans;
  std::vector<float> fitMeanErrors;
  std::vector<float> fitSigmas;
  std::vector<float> fitSigmaErrors;
  std::vector<float> histMeans;
  std::vector<float> histMeanErrors;
  std::vector<float> histSigmas;
  std::vector<float> histSigmaErrors;
  betas.push_back(0.025);
  betas.push_back(0.075);
  betas.push_back(0.125);
  betas.push_back(0.175);
  betas.push_back(0.225);
  betas.push_back(0.275);
  betas.push_back(0.325);
  betas.push_back(0.375);
  betas.push_back(0.425);
  betas.push_back(0.475);
  betas.push_back(0.525);
  betas.push_back(0.575);
  betas.push_back(0.625);
  betas.push_back(0.675);
  betas.push_back(0.725);
  betas.push_back(0.775);
  betas.push_back(0.825);
  betas.push_back(0.875);
  betas.push_back(0.925);
  betas.push_back(0.975);
  for(int i=0; i < 20; ++i)
    betaErrors.push_back(0.025);
  TDirectory* tkDeDxEstBetaSlicesDir = outFile->mkdir("TkDeDxEstBetaSlices");
  tkDeDxEstBetaSlicesDir->cd();
  TF1* myGaus = new TF1("myGaus","gaus(0)",0,20);
  for(int i=0; i < 20; ++i)
  {
    trackDeDxE1BetaSliceHists[i]->Rebin();
    myGaus->SetParameter(1,5);
    myGaus->SetParameter(2,1);
    trackDeDxE1BetaSliceHists[i]->Fit("myGaus","RME");
    trackDeDxE1BetaSliceHists[i]->Write();
    fitMeans.push_back(myGaus->GetParameter(1));
    fitMeanErrors.push_back(myGaus->GetParError(1));
    fitSigmas.push_back(myGaus->GetParameter(2));
    fitSigmaErrors.push_back(myGaus->GetParError(2));
    histMeans.push_back(trackDeDxE1BetaSliceHists[i]->GetMean());
    histMeanErrors.push_back(trackDeDxE1BetaSliceHists[i]->GetMeanError());
    histSigmas.push_back(trackDeDxE1BetaSliceHists[i]->GetRMS());
    histSigmaErrors.push_back(trackDeDxE1BetaSliceHists[i]->GetRMSError());

  }
  // Make graphs of dE/dx mean, sigma vs. beta
  TGraphErrors* fitMeanGraph = new TGraphErrors(betas.size(),&(*betas.begin()),&(*fitMeans.begin()),&(*betaErrors.begin()),&(*fitMeanErrors.begin()));
  fitMeanGraph->SetName("fitMeanVsBeta");
  fitMeanGraph->SetTitle("dE/dx E1 fitted mean [MeV/cm] vs. beta");
  fitMeanGraph->GetXaxis()->SetTitle("#beta");
  fitMeanGraph->Write();
  TGraphErrors* histMeanGraph = new TGraphErrors(betas.size(),&(*betas.begin()),&(*histMeans.begin()),&(*betaErrors.begin()),&(*histMeanErrors.begin()));
  histMeanGraph->SetName("histMeanVsBeta");
  histMeanGraph->SetTitle("dE/dx E1 hist mean [MeV/cm] vs. beta");
  histMeanGraph->GetXaxis()->SetTitle("#beta");
  histMeanGraph->Write();
  TGraphErrors* fitSigmaGraph = new TGraphErrors(betas.size(),&(*betas.begin()),&(*fitSigmas.begin()),&(*betaErrors.begin()),&(*fitSigmaErrors.begin()));
  fitSigmaGraph->SetName("fitSigmaVsBeta");
  fitSigmaGraph->SetTitle("dE/dx E1 fitted sigma [MeV/cm] vs. beta");
  fitSigmaGraph->GetXaxis()->SetTitle("#beta");
  fitSigmaGraph->Write();
  TGraphErrors* histSigmaGraph = new TGraphErrors(betas.size(),&(*betas.begin()),&(*histSigmas.begin()),&(*betaErrors.begin()),&(*histSigmaErrors.begin()));
  histSigmaGraph->SetName("histSigmaVsBeta");
  histSigmaGraph->SetTitle("dE/dx E1 hist sigma [MeV/cm] vs. beta");
  histSigmaGraph->GetXaxis()->SetTitle("#beta");
  histSigmaGraph->Write();


  trackDeDxE2BetaSlice1Hist->Write();
  trackDeDxE2BetaSlice2Hist->Write();
  trackDeDxE2BetaSlice3Hist->Write();
  trackDeDxE2BetaSlice4Hist->Write();
  trackDeDxE2BetaSlice5Hist->Write();
  trackDeDxE3BetaSlice1Hist->Write();
  trackDeDxE3BetaSlice2Hist->Write();
  trackDeDxE3BetaSlice3Hist->Write();
  trackDeDxE3BetaSlice4Hist->Write();
  trackDeDxE3BetaSlice5Hist->Write();
  TDirectory* tkDeDxDiscBetaSlicesDir = outFile->mkdir("TkDeDxDiscBetaSlices");
  tkDeDxDiscBetaSlicesDir->cd();
  trackDeDxD1BetaSlice1Hist->Write();
  trackDeDxD1BetaSlice2Hist->Write();
  trackDeDxD1BetaSlice3Hist->Write();
  trackDeDxD1BetaSlice4Hist->Write();
  trackDeDxD1BetaSlice5Hist->Write();
  trackDeDxD2BetaSlice1Hist->Write();
  trackDeDxD2BetaSlice2Hist->Write();
  trackDeDxD2BetaSlice3Hist->Write();
  trackDeDxD2BetaSlice4Hist->Write();
  trackDeDxD2BetaSlice5Hist->Write();
  trackDeDxD3BetaSlice1Hist->Write();
  trackDeDxD3BetaSlice2Hist->Write();
  trackDeDxD3BetaSlice3Hist->Write();
  trackDeDxD3BetaSlice4Hist->Write();
  trackDeDxD3BetaSlice5Hist->Write();

  TDirectory* tkDeDxEstDiscPtSlicesDir = outFile->mkdir("TkDeDxEstDiscPtSlices");
  tkDeDxEstDiscPtSlicesDir->cd();
  trackDeDxE1PtSlice1Hist->Write();
  trackDeDxE1PtSlice2Hist->Write();
  trackDeDxE1PtSlice3Hist->Write();
  trackDeDxE1PtSlice4Hist->Write();
  trackDeDxE1PtSlice5Hist->Write();
  trackDeDxE1PtSlice6Hist->Write();
  trackDeDxE1PtSlice7Hist->Write();
  trackDeDxE1PtSlice8Hist->Write();
  trackDeDxE1PtSlice9Hist->Write();
  trackDeDxE1PtSlice10Hist->Write();
  trackDeDxE1PtSlice11Hist->Write();
  trackDeDxE1PtSlice12Hist->Write();
  trackDeDxE2PtSlice1Hist->Write();
  trackDeDxE2PtSlice2Hist->Write();
  trackDeDxE2PtSlice3Hist->Write();
  trackDeDxE2PtSlice4Hist->Write();
  trackDeDxE2PtSlice5Hist->Write();
  trackDeDxE2PtSlice6Hist->Write();
  trackDeDxE2PtSlice7Hist->Write();
  trackDeDxE2PtSlice8Hist->Write();
  trackDeDxE2PtSlice9Hist->Write();
  trackDeDxE2PtSlice10Hist->Write();
  trackDeDxE2PtSlice11Hist->Write();
  trackDeDxE2PtSlice12Hist->Write();
  trackDeDxE3PtSlice1Hist->Write();
  trackDeDxE3PtSlice2Hist->Write();
  trackDeDxE3PtSlice3Hist->Write();
  trackDeDxE3PtSlice4Hist->Write();
  trackDeDxE3PtSlice5Hist->Write();
  trackDeDxE3PtSlice6Hist->Write();
  trackDeDxE3PtSlice7Hist->Write();
  trackDeDxE3PtSlice8Hist->Write();
  trackDeDxE3PtSlice9Hist->Write();
  trackDeDxE3PtSlice10Hist->Write();
  trackDeDxE3PtSlice11Hist->Write();
  trackDeDxE3PtSlice12Hist->Write();
  trackDeDxD1PtSlice1Hist->Write();
  trackDeDxD1PtSlice2Hist->Write();
  trackDeDxD1PtSlice3Hist->Write();
  trackDeDxD1PtSlice4Hist->Write();
  trackDeDxD1PtSlice5Hist->Write();
  trackDeDxD1PtSlice6Hist->Write();

  TDirectory* tkDeDxEstDiscPSlicesDir = outFile->mkdir("TkDeDxEstDiscPSlices");
  tkDeDxEstDiscPSlicesDir->cd();
  trackDeDxE1PSlice1Hist->Write();
  trackDeDxE1PSlice2Hist->Write();
  trackDeDxE1PSlice3Hist->Write();
  trackDeDxE1PSlice4Hist->Write();
  trackDeDxE1PSlice5Hist->Write();
  trackDeDxE1PSlice6Hist->Write();
  trackDeDxE1PSlice7Hist->Write();
  trackDeDxE1PSlice8Hist->Write();
  trackDeDxE1PSlice9Hist->Write();
  trackDeDxE1PSlice10Hist->Write();
  trackDeDxE1PSlice11Hist->Write();
  trackDeDxE1PSlice12Hist->Write();
  trackDeDxE2PSlice1Hist->Write();
  trackDeDxE2PSlice2Hist->Write();
  trackDeDxE2PSlice3Hist->Write();
  trackDeDxE2PSlice4Hist->Write();
  trackDeDxE2PSlice5Hist->Write();
  trackDeDxE2PSlice6Hist->Write();
  trackDeDxE2PSlice7Hist->Write();
  trackDeDxE2PSlice8Hist->Write();
  trackDeDxE2PSlice9Hist->Write();
  trackDeDxE2PSlice10Hist->Write();
  trackDeDxE2PSlice11Hist->Write();
  trackDeDxE2PSlice12Hist->Write();
  trackDeDxE3PSlice1Hist->Write();
  trackDeDxE3PSlice2Hist->Write();
  trackDeDxE3PSlice3Hist->Write();
  trackDeDxE3PSlice4Hist->Write();
  trackDeDxE3PSlice5Hist->Write();
  trackDeDxE3PSlice6Hist->Write();
  trackDeDxE3PSlice7Hist->Write();
  trackDeDxE3PSlice8Hist->Write();
  trackDeDxE3PSlice9Hist->Write();
  trackDeDxE3PSlice10Hist->Write();
  trackDeDxE3PSlice11Hist->Write();
  trackDeDxE3PSlice12Hist->Write();
  trackDeDxD1PSlice1Hist->Write();
  trackDeDxD1PSlice2Hist->Write();
  trackDeDxD1PSlice3Hist->Write();
  trackDeDxD1PSlice4Hist->Write();
  trackDeDxD1PSlice5Hist->Write();
  trackDeDxD1PSlice6Hist->Write();

  TDirectory* tkDeDxMatchedEstPtSlicesDir = outFile->mkdir("TkDeDxMatchedEstPtSlices");
  tkDeDxMatchedEstPtSlicesDir->cd();
  trackDeDxMatchedE1PtSlice1Hist->Write();
  trackDeDxMatchedE1PtSlice2Hist->Write();
  trackDeDxMatchedE1PtSlice3Hist->Write();
  trackDeDxMatchedE1PtSlice4Hist->Write();
  trackDeDxMatchedE1PtSlice5Hist->Write();
  trackDeDxMatchedE1PtSlice6Hist->Write();
  trackDeDxMatchedE1PtSlice7Hist->Write();
  trackDeDxMatchedE1PtSlice8Hist->Write();
  trackDeDxMatchedE1PtSlice9Hist->Write();
  trackDeDxMatchedE1PtSlice10Hist->Write();
  trackDeDxMatchedE1PtSlice11Hist->Write();
  trackDeDxMatchedE1PtSlice12Hist->Write();

  TDirectory* tkDeDxMatchedEstPSlicesDir = outFile->mkdir("TkDeDxMatchedEstPSlices");
  tkDeDxMatchedEstPSlicesDir->cd();
  trackDeDxMatchedE1PSlice1Hist->Write();
  trackDeDxMatchedE1PSlice2Hist->Write();
  trackDeDxMatchedE1PSlice3Hist->Write();
  trackDeDxMatchedE1PSlice4Hist->Write();
  trackDeDxMatchedE1PSlice5Hist->Write();
  trackDeDxMatchedE1PSlice6Hist->Write();
  trackDeDxMatchedE1PSlice7Hist->Write();
  trackDeDxMatchedE1PSlice8Hist->Write();
  trackDeDxMatchedE1PSlice9Hist->Write();
  trackDeDxMatchedE1PSlice10Hist->Write();
  trackDeDxMatchedE1PSlice11Hist->Write();
  trackDeDxMatchedE1PSlice12Hist->Write();

  TDirectory* tkDeDxHitGraphs = outFile->mkdir("TkDeDxHitGraphs");
  tkDeDxHitGraphs->cd();
  for(std::vector<TGraph>::const_iterator itr = dedxHitGraphs.begin();
      itr != dedxHitGraphs.end(); ++itr)
    itr->Write();
  trackDeDxAllHitsHist->Write();
  trackDeDxAllHits1NoMHist->Write();
  trackDeDxAllHits4NoMHist->Write();
  trackDeDxAllHits6NoMHist->Write();
  trackDeDxAllHits10NoMHist->Write();
  trackDeDxAllHits15NoMHist->Write();
  trackDeDxAllHitsBigDeDxHist->Write();
  trackDeDxAllHitsVsNosHist->Write();
  trackDeDxRms3Hist->Write();
  trackDeDxRms4Hist->Write();
  trackDeDxRms5Hist->Write();
  trackDeDxRms3MatchedHist->Write();
  trackDeDxRms4MatchedHist->Write();
  trackDeDxRms5MatchedHist->Write();
  //trackDeDxPathLengthVsEtaHist->Write();
  //trackDeDxPathLengthVsEtaMatchedHist->Write();
  //trackDeDxE1VsPathLengthHist->Write();
  //trackDeDxE1VsPathLengthMatchedHist->Write();

  TDirectory* muonTimingDir = outFile->mkdir("MuonTiming");
  muonTimingDir->cd();
  muonCombinedIBetaHist->Write();
  muonDtIBetaHist->Write();
  muonCscIBetaHist->Write();
  muonCombinedIBetaErrHist->Write();
  muonDtIBetaErrHist->Write();
  muonCscIBetaErrHist->Write();
  muonCombinedBetaHist->Write();
  muonDtBetaHist->Write();
  muonCscBetaHist->Write();

}


