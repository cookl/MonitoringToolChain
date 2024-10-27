#include "SendFakeData.h"
#include <deque>

SendFakeData::SendFakeData():Tool(){}


bool SendFakeData::Initialise(std::string configfile, DataModel &data){
   InitialiseTool(data);
   InitialiseConfiguration(configfile);
   ExportConfiguration();
   //get the data model and save it so we can add stuff to it while executing
   m_data= &data;
  
   //read in the mapping of the mPMTs to ensure when picking random channels they are real channels in the detector
   std::string directory = "UserTools/EventDisplay/inputs/";
   std::string cardIDChannelMappingFile = "cardIDMapping.csv";
   std::vector<std::vector<int>> temp_csv = ReadCSV_int(directory+cardIDChannelMappingFile);
   channelList = ReadCSV_int(directory+cardIDChannelMappingFile)[0];

   //pick a random run no or initialisation
   std::srand(static_cast<unsigned int>(std::time(nullptr)));
   runNo = (rand() % 10000000);
  
   //keep track of the time - we will only send an event periodically so as to not overwhelm database on testing
   last_sending_data = std::chrono::steady_clock::now();

   return true;
}


bool SendFakeData::Execute(){
   //send the data periodically - entirely a fudge for sending fake data
   auto now = std::chrono::steady_clock::now();
   auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_sending_data);
   int sending_period_ms = 1000;

   if(elapsed.count() > sending_period_ms){
      last_sending_data = std::chrono::steady_clock::now();
      //send new data
      //make a readout window object
      ReadoutWindow* readoutWindow = new ReadoutWindow();

      // auto now = std::chrono::system_clock::now();
      // int seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
      //pick a single time to be a large coarse time with small deviattion about it
      int coarse_time = 1000;
            
      
      //make 400 hits
      for(int i = 0; i < 400; i++){
         WCTEMPMTHit hit;
         int rand_ch_index = rand() % (1767);
         int channel_id = channelList[rand_ch_index];
         hit.SetCardID(channel_id / 100);
         hit.SetChannel(channel_id % 100);
      
         int rand_counter = coarse_time + (rand() % 10);
         hit.SetCoarseCounter(rand_counter);
         hit.SetFineTime(rand() % 1000);
         hit.SetCharge(rand() % 2000);

         readoutWindow->mpmt_hits.push_back(hit);
      }

      //will need to be adjusted once the trigger is updated in the raw data file
      //also need to check - what is time? Assume coarse counter
      TriggerInfo trigger;
      trigger.time = coarse_time;
      trigger.type = static_cast<TriggerType>(rand() % 4);
      readoutWindow->triggers_info.push_back(trigger);

      //set the current readout window in the datamodel to this readout window we made here.  
      m_data->currentReadoutWindow = readoutWindow;
      readoutWindow->runNo = this->runNo;
      readoutWindow->spillNo = rand() % 1000;
      readoutWindow->windowNo = rand() % 1000;

      //make a deque of the readout windows - also handles deletion of pointer 
      if(m_data->readout_window_vector.size() < m_data->max_deque_length){
         m_data->readout_window_vector.push_back(readoutWindow);
      } else {
         delete m_data->readout_window_vector.front();
         m_data->readout_window_vector.pop_front();
         m_data->readout_window_vector.push_back(readoutWindow);
      }
   } else {
      //don't send new data 
      m_data->currentReadoutWindow = nullptr;
   }

   return true;
}


bool SendFakeData::Finalise(){
   return true;
}

std::vector<std::vector<int>> SendFakeData::ReadCSV_int(const std::string& filename) {
   std::ifstream file(filename);
   std::vector<std::vector<int>> columns;
   std::string line;

   // Clear the vectors and prepare to read
   columns.clear();
   
   if(!file){
      std::cout << "Problem opening file :" << filename << " returning null vector" << std::endl;
      return columns;
   }

   // Read header line (skip if needed)
   std::getline(file, line);

   // Read data lines
   while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string item;

      int columnIndex = 0;
      while (std::getline(ss, item, ',')) {
         int value = std::stoi(item);

         // Resize columns vector if necessary
         if (columnIndex >= columns.size()) {
            columns.resize(columnIndex + 1);
         }

         // Append value to the appropriate column
         columns[columnIndex].push_back(value);
         ++columnIndex;
      }
   }

   return columns;
}
