#include "TriggerEventDisplay.h"
#include <vector>
#include"TGraph.h"
#include "TBufferJSON.h"
#include "TriggerStringMap.h"


TriggerEventDisplay::TriggerEventDisplay():Tool(){}


bool TriggerEventDisplay::Initialise(std::string configfile, DataModel &data){

  InitialiseTool(data);
  InitialiseConfiguration(configfile);

    //m_variables.Print();

  m_data= &data;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  eventDisplay = EventDisplay("mPMTOrientations.csv","mPMT_2D_projection.csv","cardIDMapping.csv");  
  //index of the graph in terms of graph name 

  
  return true;
}


bool TriggerEventDisplay::Execute(){
  
  if(m_data->currentReadoutWindow!=nullptr){
    //get info from first trigger in event 
    TriggerType trigger = m_data->currentReadoutWindow->triggers_info[0].type;
    //NB assumes trigger time is in ns
    double triggerTime = m_data->currentReadoutWindow->triggers_info[0].time;

    //let all graphs for the same event have the same suffix for ease of getitng them from the DB  
    std::string graphSuffix = "_"+triggerTypeToString(trigger)+"_run_"+std::to_string(m_data->currentReadoutWindow->runNo)+"_windowNo_"+std::to_string(m_data->currentReadoutWindow->windowNo);

    //debug
    std::cout << "First Hit card" <<m_data->currentReadoutWindow->mpmt_hits[0].GetCardID() << " ch "<< m_data->currentReadoutWindow->mpmt_hits[0].GetChannel() << std::endl;

    //make some 1D charge and time distributions
    std::string time_dist_name = "TimeDist"+graphSuffix;
    TH1F* time_dist = new TH1F(time_dist_name.c_str(),time_dist_name.c_str(),100,-500,500); //graph range very wide for now
    
    std::string charge_dist_name = "ChargeDist"+graphSuffix;
    TH1F* charge_dist = new TH1F(charge_dist_name.c_str(),charge_dist_name.c_str(),200,0,2000); //graph range very wide for now

    //process the hits for plotting event display and fill 1D histograms
    std::vector<int> hit_card_id;
    std::vector<int> hit_chanel_id;
    std::vector<double> hit_charge;
    std::vector<double> hit_time;

    for(int i=0; i<m_data->currentReadoutWindow->mpmt_hits.size(); i++){
      hit_chanel_id.push_back(m_data->currentReadoutWindow->mpmt_hits[i].GetChannel());
      hit_card_id.push_back(m_data->currentReadoutWindow->mpmt_hits[i].GetCardID());
      hit_charge.push_back(m_data->currentReadoutWindow->mpmt_hits[i].GetCharge());
      //convert from coarse counter + fine counter to a time in ns 
      //8.0ns *coarse + fine /8192.0
      //assumes trigger time is also a coarse counter
      double ns_time = 8.0*m_data->currentReadoutWindow->mpmt_hits[i].GetCoarseCounter() + (m_data->currentReadoutWindow->mpmt_hits[i].GetFineTime()/8192.0);
      hit_time.push_back(ns_time-8.0*triggerTime);
      time_dist->Fill(ns_time-8.0*triggerTime);
      charge_dist->Fill(m_data->currentReadoutWindow->mpmt_hits[i].GetCharge());
      
    }

    //make 2d event displays for charge and time NB TScatter here isn't a pointer
    std::string charge_ev_display_name = "ChargeEventDisplay"+graphSuffix;
    TScatter chargeEventDisplay = eventDisplay.makeEventDisplayPlot(charge_ev_display_name,hit_card_id,hit_chanel_id,hit_charge);

    std::string time_ev_display_name = "TimeEventDisplay"+graphSuffix;
    TScatter timeEventDisplay = eventDisplay.makeEventDisplayPlot(time_ev_display_name,hit_card_id,hit_chanel_id,hit_time);

    //send everything to the database
    std::string chargeEventDisplay_jsonData = TBufferJSON::ToJSON(&chargeEventDisplay).Data();
    int graph_db_ver =0; // won't use so set to 0 for all
    std::string graph_draw_options = "AP"; //not used for the event display as plotted with plotly
    // m_data->services->SendTemporaryROOTplot(charge_ev_display_name, graph_draw_options, chargeEventDisplay_jsonData, &graph_db_ver, 0);

    std::string timeEventDisplay_jsonData = TBufferJSON::ToJSON(&timeEventDisplay).Data();
    graph_draw_options = "AP"; //not used for the event display as plotted with plotly
    // m_data->services->SendTemporaryROOTplot(time_ev_display_name, graph_draw_options, timeEventDisplay_jsonData, &graph_db_ver, 0);

    //send the 1d histograms 

    std::string timeDist_jsonData = TBufferJSON::ToJSON(time_dist).Data();
    graph_draw_options = ""; //not used for the event display as plotted with plotly
    // m_data->services->SendTemporaryROOTplot(time_dist_name, graph_draw_options, timeDist_jsonData, &graph_db_ver, 0);
    
    std::string chargeDist_jsonData = TBufferJSON::ToJSON(charge_dist).Data();
    graph_draw_options = ""; //not used for the event display as plotted with plotly
    // m_data->services->SendTemporaryROOTplot(charge_dist_name, graph_draw_options, chargeDist_jsonData, &graph_db_ver, 0);

  


    // std::string graphName = "EvDisp_"+triggerTypeToString(trigger)+"_run_"+std::to_string(m_data->currentReadoutWindow->runNo)+"_windowNo_"+std::to_string(m_data->currentReadoutWindow->windowNo);
    

    // //if the time stamp is set as an arg -last variable in SendTemporaryROOTplot, it prevents it being sent to the db?
    // bool sentData = m_data->services->SendTemporaryROOTplot(graphName, graph_draw_options, jsonData, &graph_db_ver, 0);



    // // TScatter eventDisplayScatter = makeEventDisplayFromHits(trigger, m_data->currentReadoutWindow->triggers_info[0].time ,m_data->currentReadoutWindow->mpmt_hits);
    
    // std::string jsonData = TBufferJSON::ToJSON(&eventDisplayScatter).Data();
    
    // std::string graphName = "EvDisp_"+triggerTypeToString(trigger)+"_run_"+std::to_string(m_data->currentReadoutWindow->runNo)+"_windowNo_"+std::to_string(m_data->currentReadoutWindow->windowNo);
    // std::string graph_draw_options = "AP"; //not used for the event display as plotted with plotly
    // int graph_db_ver =0;

    // //if the time stamp is set as an arg -last variable in SendTemporaryROOTplot, it prevents it being sent to the db?
    // bool sentData = m_data->services->SendTemporaryROOTplot(graphName, graph_draw_options, jsonData, &graph_db_ver, 0);
    // std::cout << "Sent data " << sentData << std::endl;



  }else{
    std::cout << "No readout window" << std::endl;
  }
  
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // switch (trigger) {
    //     case TriggerType::LASER: 
          
    //     case TriggerType::NHITS: return "NHITS";
    //     case TriggerType::LED: return "LED";
    //     case TriggerType::BEAM: return "BEAM";
    //     case TriggerType::NONE: return "NONE";
    //     default: return "UNKNOWN"; // Handle invalid case
    // }
    
    
    // store it in the database
    // For now this setting only affects whether the request is sent over multicast (false, default) or tcp (true)
    // At a later date we may add the option for plots to only be stored in a temporary table.
    // std::cout<<"Sending to database via TCP"<<std::endl;
    // std::cout << "json_data " << graph_json << std::endl;
    //vector to keep track of which version of the plot has been uploaded to the database
  // }
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));
  // auto now = std::chrono::steady_clock::now();
  // auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_sending_data);
  // std::cout << "Elapsed count " << elapsed.count() << std::endl;
  // //if more than 500ms have passed since the last time we sent data to the database
  // if(elapsed.count()>500 && !eventDisplayBuffer.empty()){
  //   std::cout << "Sending data " << std::endl;
  //   last_sending_data = now;
  //   //send the oldest event in the buffer to the database
  //   std::string graph_json = eventDisplayBuffer.front().jsonData;
  //   int64_t timestamp = eventDisplayBuffer.front().timestamp;
     
  //   std::string graphName = "NowEventDisplay";
  //   std::string graph_draw_options = "AP";
  //   int graph_db_ver =0;

  //   std::cout << "graph_json" << graph_json << std::endl;
  //   bool sentData = m_data->services->SendTemporaryROOTplot(graphName, graph_draw_options, graph_json, &graph_db_ver, 0);
  //   std::cout << "Sent data " << sentData << std::endl;
  //   //remove event from front 
  //   eventDisplayBuffer.pop_front();
  // } 

  // m_data->services->SendTemporaryROOTplot(graphName, graph_draw_options, graph_json, &graph_db_ver, 0);
  // SendTemporaryROOTplot(plot_name, draw_options, json_data, version, timestamp);
	// bool ok = DAQ_inter.SendROOTplot(graph_name, graph_draw_options, graph_json, persistent, &graph_db_ver);

  // delete canvas;
  // delete eventDisplayScatter;


  return true;
}


bool TriggerEventDisplay::Finalise(){
  std::cout <<"Finalise TriggerEventDisplay" << std::endl;
  return true;
}

// std::vector<TScatter> TriggerEventDisplay::makeEventDisplayFromHits(TriggerType trigger,double triggerTime,std::vector<WCTEMPMTHit> mpmt_hits){
  
//   std::vector<TScatter> outVector;

//   std::string graphName = "EventDisplay_"+triggerTypeToString(trigger);
  
//   //process the hits for plotting 
//   std::vector<int> hit_card_id;
//   std::vector<int> hit_chanel_id;
//   std::vector<double> hit_charge;
//   std::vector<double> hit_time;

//   for(int i=0; i<mpmt_hits.size(); i++){
//     hit_chanel_id.push_back(mpmt_hits[i].GetChannel());
//     hit_card_id.push_back(mpmt_hits[i].GetCardID());
//     hit_charge.push_back(mpmt_hits[i].GetCharge());
//     //convert from coarse counter + fine counter to a time in ns 
//     //8.0ns *coarse + fine /8192.0
//     //assumes trigger time in ns 
//     double ns_time = 8.0*mpmt_hits[i].GetCoarseCounter() + mpmt_hits[i].GetFineTime()/8192.0;
//     hit_time.push_back(ns_time-triggerTime);
//   }
//   std::cout << "Call makeEventDisplayPlot " << std::endl;
//   TScatter chargeEventDisplay = eventDisplay.makeEventDisplayPlot(graphName,hit_card_id,hit_chanel_id,hit_charge);
//   outVector.push_back(chargeEventDisplay);

//   return eventDisplayScatter;
// }


