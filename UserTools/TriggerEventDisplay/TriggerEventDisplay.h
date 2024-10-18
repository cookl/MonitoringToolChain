#ifndef TriggerEventDisplay_H
#define TriggerEventDisplay_H

#include <string>
#include <iostream>

#include "Tool.h"
#include "DataModel.h"
#include "EventDisplay.h"

#include "TScatter.h"
#include "TCanvas.h"

#include <TColor.h>
#include <TStyle.h>
/**
 * \class TriggerEventDisplay
 *
 * This is a balnk template for a Tool used by the script to generate a new custom tool. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class TriggerEventDisplay: public Tool {


 public:

  TriggerEventDisplay(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.

  
private:
  EventDisplay eventDisplay;
  int iEv;
  
  DataModel* m_data;
  // std::string voltage_change_func(const char* key);
  // std::string start_func(const char* key);
  // void new_event_func(const char* alert);

  // void SendMonitoring();
 
  // Store monitoring_data; // sorage object for monitoring vales;

  
  
  
};


#endif
