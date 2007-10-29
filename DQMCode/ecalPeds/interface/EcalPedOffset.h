#ifndef EcalPedOffset_H
#define EcalPedOffset_H

/**
 * \file EcalPedOffset.h
 * \class EcalPedOffset
 * \brief calculate the best DAC value to obtain a pedestal = 200
 * $Date: 2007/02/19 10:18:19 $
 * $Revision: 1.7 $
 * \author P. Govoni (pietro.govoni@cernNOSPAM.ch)
 *
*/

#include <map>
#include <string>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "CalibCalorimetry/EcalPedestalOffsets/interface/TPedValues.h"
#include "CalibCalorimetry/EcalPedestalOffsets/interface/TPedResult.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

class EcalPedOffset: public edm::EDAnalyzer
{

  public:
    
    //! Constructor
    EcalPedOffset(const edm::ParameterSet& ps);
    
    //! Destructor
    virtual ~EcalPedOffset();
   
    ///! Analyze
    void analyze(edm::Event const& event, edm::EventSetup const& eventSetup);
    
    //! BeginJob
    void beginJob(edm::EventSetup const& eventSetup);
    
    //! EndJob
    void endJob(void);
    
    //! write the results into xml format
    void writeXMLFiles(std::string fileName);
    
    //! WriteDB
    void writeDb();
    
    //! create the plots of the DAC pedestal trend
    void makePlots();


  private:
 
    std::string intToString(int num);
    void readDACs(edm::Handle<EBDigiCollection> pDigis, std::map<int,int> DACvalues);
    void readDACs(edm::Handle<EEDigiCollection> pDigis, std::map<int,int> DACvalues);
    
    std::string m_barrelDigiCollection; //!< secondary name given to collection of digis
    std::string m_endcapDigiCollection; //!< secondary name given to collection of digis
    std::string m_digiProducer;   //!< name of module/plugin/producer making digis
    std::string m_headerProducer; //!< name of module/plugin/producer making headers

    std::string m_xmlFile;        //!< name of the xml file to be saved

    std::map<int,TPedValues*> m_pedValues;
    std::map<int,TPedResult*> m_pedResult;
     
    int m_DACmin;
    int m_DACmax;
    double m_RMSmax;
    int m_bestPed;
    //int m_SMnum; //! FIXME temporary until the fix in CMSSW
    
    //! database host name
    std::string m_dbHostName;
    //! database name
    std::string m_dbName;
    //! database user name
    std::string m_dbUserName;
    //! database user password
    std::string m_dbPassword;
    //! database 
    int m_dbHostPort;
    //!allow the creation of a new moniov if not existing in the DB
    //!by default it is false.
    bool m_create_moniov;
    // used to retrieve the run_iov 
    std::string m_location;
    //! run number
    int m_run;
    
    //! the root file where to store the detail plots
    std::string m_plotting;

} ; 

#endif
