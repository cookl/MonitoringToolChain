#ifndef SendFakeData_H
#define SendFakeData_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib> // for rand() and srand()

#include "Tool.h"
#include "DataModel.h"

#include <WCTERawData.h>
#include <ReadoutWindow.h>

#include <chrono>   // for system_clock
#include <ctime>    // for std::gmtime, std::strftime

/**
 * \class SendFakeData
 *
 * This is a balnk template for a Tool used by the script to generate a new custom tool. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
*/

class SendFakeData: public Tool {


 public:

  SendFakeData(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.


 private:
  std::vector<std::vector<int>> ReadCSV_int(const std::string& filename);
  std::vector<int> channelList;  
  DataModel* m_data;
  


};


#endif
