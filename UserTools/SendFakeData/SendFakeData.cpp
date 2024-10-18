#include "SendFakeData.h"
#include <deque>

SendFakeData::SendFakeData():Tool(){}


bool SendFakeData::Initialise(std::string configfile, DataModel &data){
  std::cout << "Here" << std::endl;
  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  //your code here


  ExportConfiguration();
  m_data= &data;
  
  // m_data->readout_window_vector.clear();

  std::string directory = "UserTools/EventDisplay/inputs/";
  std::string cardIDChannelMappingFile = "cardIDMapping.csv";
  std::cout << "Start read " << std::endl;
  std::vector<std::vector<int>> temp_csv = ReadCSV_int(directory+cardIDChannelMappingFile);
  channelList = ReadCSV_int(directory+cardIDChannelMappingFile)[0];
  std::cout << "Size of vector is " <<channelList.size() << std::endl;

  // m_data->currentReadoutWindow = nullptr;


  return true;
}


bool SendFakeData::Execute(){
  //make a readout window object and send to the DataModel
  std::cout <<"Start Execute send data" << std::endl;

  ReadoutWindow* readoutWindow = new ReadoutWindow();
  // ReadoutWindow* readoutWindow;
  m_data->currentReadoutWindow = readoutWindow;

  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  //make ~400 hits
  for(int i=0; i<400; i++){
    WCTEMPMTHit hit;
    int rand_ch_index = rand()%(1767+1);
    int channel_id = channelList[rand_ch_index];
    hit.SetCardID(channel_id/100);
    hit.SetChannel(channel_id%19);
    
    auto now = std::chrono::system_clock::now();
    int seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    
    int rand_counter = seconds_since_epoch + rand()%10;
    hit.SetCoarseCounter(rand_counter);
    hit.SetFineTime(rand()%1000);
    
    hit.SetCharge(rand()%2000);

    // m_data->currentReadoutWindow->mpmt_hits.push_back(hit);
    m_data->currentReadoutWindow->mpmt_hits.push_back(hit);
    
  }
  if(m_data->readout_window_vector.size()<m_data->max_deque_length){
    m_data->readout_window_vector.push_back(readoutWindow);
  }else{
    // delete m_data->readout_window_vector.front();
    m_data->readout_window_vector.pop_front();
    m_data->readout_window_vector.push_back(readoutWindow);
  }
  
  std::cout <<"Executed send data" << std::endl;

  // readoutWindow.mpmt_hits is a vector of mPMT hits
  // WCTEMPMTHit is a single hit



  return true;
}


bool SendFakeData::Finalise(){
  
  // Iterate through the deque and delete each pointer
  // for (int i=0; i< m_data->readout_window_vector.size(); i++) {
  //     delete m_data->readout_window_vector[i];
  // }
  // m_data->readout_window_vector.clear();

  std::cout <<"Finalise send fake data" << std::endl;
  return true;
}

std::vector<std::vector<int>> SendFakeData::ReadCSV_int(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> columns;
    std::string line;

    // Clear the vectors and prepare to read
    columns.clear();
    
    if(!file){
        std::cout << "Problem opening file :" <<  filename << " returning null vector" <<std::endl;
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