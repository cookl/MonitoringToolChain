#include "mPMTHitMap.h"
#include "TH2F.h"
#include "TBufferJSON.h"


mPMTHitMap::mPMTHitMap():Tool(){}


bool mPMTHitMap::Initialise(std::string configfile, DataModel &data){
  
  InitialiseTool(data);
  InitialiseConfiguration(configfile);
  //m_variables.Print();

  //your code here

  //save data model to the function to access in execute
  m_data= &data;

  ExportConfiguration();

  return true;
}


bool mPMTHitMap::Execute(){
  
  if(m_data->currentReadoutWindow!=nullptr){
    std::cout <<"Execute readout window " << m_data->currentReadoutWindow << std::endl;
    //make a 2d hit map for this event of the channelID and cardID
    
    //root graph to make
    std::string plotTitle = "Electronics Channel Charge Run: "+std::to_string(m_data->currentReadoutWindow->runNo)+" Spill: "+std::to_string(m_data->currentReadoutWindow->spillNo) +" Window: "+std::to_string(m_data->currentReadoutWindow->windowNo);
    plotTitle = plotTitle + "Trigger: " +triggerTypeToString(m_data->currentReadoutWindow->triggers_info[0].type);
    TH2F* hitMap = new TH2F("hitMap",plotTitle.c_str(),150,0,150,20,0,20);
    //fill the historgram from the data 
    TriggerType trigger = m_data->currentReadoutWindow->triggers_info[0].type;
    int runNo = m_data->currentReadoutWindow->runNo;
    int windowNo = m_data->currentReadoutWindow->windowNo;
    int spillNo = m_data->currentReadoutWindow->windowNo;
    

    for(int i=0; i<m_data->currentReadoutWindow->mpmt_hits.size(); i++){
      hitMap->Fill(m_data->currentReadoutWindow->mpmt_hits[i].GetCardID(),m_data->currentReadoutWindow->mpmt_hits[i].GetChannel(),m_data->currentReadoutWindow->mpmt_hits[i].GetCharge());
    }

    //convert to json and send to database
    std::string jsonData = TBufferJSON::ToJSON(hitMap).Data();
    std::string graphName = "mPMTChannelHits";
    std::string graph_draw_options = "COLZ";
    int graph_db_ver =0;
    bool sentData = m_data->services->SendTemporaryROOTplot(graphName, graph_draw_options, jsonData, &graph_db_ver, 0);


    delete hitMap;


  }


  return true;
}


bool mPMTHitMap::Finalise(){

  return true;
}
